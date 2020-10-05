/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2019 Western Digital Corporation or its affiliates.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http:*www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/**
* include files
*/
#include "common_types.h"
#include "demo_platform_al.h"
#include "demo_utils.h"
#include "cti_api.h"
#include "comrv_api.h"
#include "rtosal_task_api.h"
#include "rtosal_time_api.h"
#include "rtosal_mutex_api.h"
#include "rtosal_event_api.h"
#include "psp_api.h"

/**
* definitions
*/
#define M_DEMO_COMRV_RTOS_FENCE()   M_PSP_INST_FENCE(); \
                                    M_PSP_INST_FENCEI();

/* The rate the data is sent to the queue, specified in milliseconds, and
converted to ticks (using the definition of D_TICK_TIME_MS) */
#define D_MAIN_QUEUE_SEND_PERIOD_TICKS         (200/D_TICK_TIME_MS)

/* Stack size of the tasks in this application */
#define D_TASK_A_STACK_SIZE                    450
#define D_TASK_B_STACK_SIZE                    450
#define D_DEMO_TASK_RUNNING                    0
#define D_DEMO_TASK_COMPLETED                  1
#define D_DEMO_EVENT                           0xFFFF

/**
* macros
*/

/**
* types
*/
typedef u32_t (*DemoExternalErrorHook_t)(const comrvErrorArgs_t* pErrorArgs);

/**
* local prototypes
*/
void demoCreateTasks(void *pParam);
void demoTestTaskA(void* pValueToSend);
void demoTestTaskB(void* pValueToSend);

/**
* external prototypes
*/
extern void* _OVERLAY_STORAGE_START_ADDRESS_;
extern u32_t xcrc32(const u08_t *pBuf, s32_t siLen, u32_t uiInit);

/**
* global variables
*/

/* globals related to the tasks in this demo */
static rtosalTask_t stTestTaskA;
static rtosalTask_t stTestTaskB;
static rtosalStackType_t uTestTaskAStackBuffer[D_TASK_A_STACK_SIZE];
static rtosalStackType_t uTestTaskBStackBuffer[D_TASK_B_STACK_SIZE];
rtosalMutex_t stComrvMutex;
static u32_t uiPrevIntState;
rtosalEventGroup_t stRtosalEventGroupCb;
u32_t G_uiTaskBStatus;
DemoExternalErrorHook_t fptrDemoExternalErrorHook = NULL;

/**
* functions
*/
void demoStart(void)
{
   comrvInitArgs_t stComrvInitArgs = { 1 };

   M_DEMO_START_PRINT();

   /* Disable the timer interrupts until setup is done. */
   pspDisableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_TIMER);

   /* Init ComRV engine */
   comrvInit(&stComrvInitArgs);

   rtosalStart(demoCreateTasks);
}

/**
 * demoCreateTasks
 *
 * Initialize the application:
 * - Register unhandled exceptions, Ecall exception and Timer ISR
 * - Create the Rx tasks
 * - Create the comrv mutex
 *
 * This function is called from RTOS abstraction layer. After its
 * completion, the scheduler is kicked on and the tasks are start to be active
 *
 */
void demoCreateTasks(void *pParam)
{
   u32_t res;

   /* Disable the machine external & timer interrupts until setup is done. */
   pspDisableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_EXT);
   pspDisableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_TIMER);

   /* Enable the Machine-External bit in MIE */
   pspEnableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_EXT);

   /* Create the rx task */
   res = rtosalTaskCreate(&stTestTaskA, (s08_t*)"TASK-A-RV", E_RTOSAL_PRIO_30,
         demoTestTaskA, (u32_t)NULL, D_TASK_A_STACK_SIZE,
         uTestTaskAStackBuffer, 0, D_RTOSAL_AUTO_START, 0);
   if (res != D_RTOSAL_SUCCESS)
   {
      demoOutputMsg("Test-A Task creation failed\n", 24);
      M_DEMO_ERR_PRINT();
      M_DEMO_ENDLESS_LOOP();
   }

   /* Create the test task */
   res = rtosalTaskCreate(&stTestTaskB, (s08_t*)"TASK-B-RV", E_RTOSAL_PRIO_29,
         demoTestTaskB, (u32_t)NULL, D_TASK_B_STACK_SIZE,
         uTestTaskBStackBuffer, 0, D_RTOSAL_AUTO_START, 0);
   if (res != D_RTOSAL_SUCCESS)
   {
      demoOutputMsg("Test-B Task creation failed\n", 24);
      M_DEMO_ERR_PRINT();
      M_DEMO_ENDLESS_LOOP();
   }

   /* create the comrv mutex */
   res = rtosalMutexCreate(&stComrvMutex, (s08_t*)"comrv", D_RTOSAL_INHERIT);
   if (res != D_RTOSAL_SUCCESS)
   {
      demoOutputMsg("comrv mutex creation failed\n", 28);
      M_DEMO_ERR_PRINT();
      M_DEMO_ENDLESS_LOOP();
   }

   res = rtosalEventGroupCreate(&stRtosalEventGroupCb, NULL);
   if (res != D_RTOSAL_SUCCESS)
   {
      demoOutputMsg("event creation failed\n", 22);
      M_DEMO_ERR_PRINT();
      M_DEMO_ENDLESS_LOOP();
   }

   /* set timer tick period */
   rtosalTimerSetPeriod(D_TICK_TIME_MS * (D_CLOCK_RATE / D_PSP_MSEC));
}

/**
 * Test task A function (low priority)
 *
 * pvParameters - not in use
 */
void demoTestTaskA(void* pValueToSend)
{
   rtosalEventBits_t stRtosalEventBits;

   /* call the test environment */
   ctiMain();

   /* if both tasks tests completed */
   if (G_uiTaskBStatus == D_DEMO_TASK_COMPLETED)
   {
      M_DEMO_END_PRINT();
   }

   /* no need to continue from here - wait for a non existing event */
   rtosalEventGroupGet(&stRtosalEventGroupCb, D_DEMO_EVENT,
                  &stRtosalEventBits, D_RTOSAL_OR_CLEAR, D_RTOSAL_WAIT_FOREVER);
}

/**
 * Test task B function (higher priority than task A)
 *
 * pvParameters - not in use
 */
void demoTestTaskB(void* pValueToSend)
{
   rtosalEventBits_t stRtosalEventBits;

   /* mark the task has started running */
   G_uiTaskBStatus = D_DEMO_TASK_RUNNING;

   /* run the test */
   ctiTaskBTest();

   /* mark task has completed */
   G_uiTaskBStatus = D_DEMO_TASK_COMPLETED;

   /* no need to continue from here - wait for a non existing event */
   rtosalEventGroupGet(&stRtosalEventGroupCb, D_DEMO_EVENT,
                  &stRtosalEventBits, D_RTOSAL_OR_CLEAR, D_RTOSAL_WAIT_FOREVER);
}

/**
* memory copy hook
*
* @param  none
*
* @return none
*/
void comrvMemcpyHook(void* pDest, void* pSrc, u32_t uiSizeInBytes)
{
   u32_t loopCount = uiSizeInBytes/(sizeof(u32_t)), i;
   /* copy dwords */
   for (i = 0; i < loopCount ; i++)
   {
      *((u32_t*)pDest + i) = *((u32_t*)pSrc + i);
   }
   loopCount = uiSizeInBytes - (loopCount*(sizeof(u32_t)));
   /* copy bytes */
   for (i = (i-1)*(sizeof(u32_t)) ; i < loopCount ; i++)
   {
      *((u08_t*)pDest + i) = *((u08_t*)pSrc + i);
   }
}

/**
* load overlay group hook
*
* @param pLoadArgs - refer to comrvLoadArgs_t for exact args
*
* @return loaded address or NULL if unable to load
*/
void* comrvLoadOvlayGroupHook(comrvLoadArgs_t* pLoadArgs)
{
   g_stCtiOvlFuncsHitCounter.uiComrvLoad++;
   comrvMemcpyHook(pLoadArgs->pDest, (u08_t*)&_OVERLAY_STORAGE_START_ADDRESS_ + pLoadArgs->uiGroupOffset, pLoadArgs->uiSizeInBytes);
   /* it is upto the end user of comrv to synchronize the instruction and data stream after
      overlay data has been written to destination memory */
   M_DEMO_COMRV_RTOS_FENCE();
   return pLoadArgs->pDest;
}

/**
* set a function pointer to be called by comrvErrorHook
*
* @param  pErrorArgs - pointer to error arguments
*
* @return none
*/
void demoComrvSetErrorHandler(void* fptrAddress)
{
   fptrDemoExternalErrorHook = fptrAddress;
}

/**
* error hook
*
* @param  pErrorArgs - pointer to error arguments
*
* @return none
*/
void comrvErrorHook(const comrvErrorArgs_t* pErrorArgs)
{
   comrvStatus_t stComrvStatus;
   comrvGetStatus(&stComrvStatus);
   /* if external error handler was set e.g. cti */
   if (fptrDemoExternalErrorHook == NULL)
   {
      /* we can't continue so loop forever */
      M_DEMO_ERR_PRINT();
      M_DEMO_ENDLESS_LOOP();
   }
   else
   {
      fptrDemoExternalErrorHook(pErrorArgs);
   }
}

/**
* Invalidate data cache hook
*
* @param pAddress         - memory address to invalidate
*        uiNumSizeInBytes - number of bytes to invalidate
*
* @return none
*/
void comrvInvalidateDataCacheHook(const void* pAddress, u32_t uiNumSizeInBytes)
{
   (void)pAddress;
   (void)uiNumSizeInBytes;
}

/**
* enter critical section
*
* @param None
*
* @return 0 - success, non-zero - failure
*/
u32_t comrvEnterCriticalSectionHook(void)
{
   if (rtosalGetSchedulerState() != D_RTOSAL_SCHEDULER_NOT_STARTED)
   {
      if (rtosalMutexWait(&stComrvMutex, D_RTOSAL_WAIT_FOREVER) != D_RTOSAL_SUCCESS)
      {
         return 1;
      }
   }
   else
   {
      pspInterruptsDisable(&uiPrevIntState);
   }

   return 0;
}

/**
* exit critical section
*
* @param None
*
* @return 0 - success, non-zero - failure
*/
u32_t comrvExitCriticalSectionHook(void)
{
   if (rtosalGetSchedulerState() != D_RTOSAL_SCHEDULER_NOT_STARTED)
   {
      if (rtosalMutexRelease(&stComrvMutex) != D_RTOSAL_SUCCESS)
      {
         return 1;
      }
   }
   else
   {
      pspInterruptsRestore(uiPrevIntState);
   }

   return 0;
}

/**
* crc calculation hook
*
* @param pAddress         - memory address to calculate
*        memSizeInBytes   - number of bytes to calculate
*        uiExpectedResult - expected crc result
*
* @return calculated CRC
*/
#ifdef D_COMRV_ENABLE_CRC_SUPPORT
u32_t comrvCrcCalcHook(const void* pAddress, u16_t usMemSizeInBytes, u32_t uiExpectedResult)
{
   volatile u32_t uiCrc;
   uiCrc = xcrc32(pAddress, usMemSizeInBytes, 0xffffffff);
   return !(uiExpectedResult == uiCrc);
}
#endif /* D_COMRV_ENABLE_CRC_SUPPORT */

