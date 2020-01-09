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
#include "common_types.h"
#include "psp_macros.h"
#include "comrv_api.h"

#ifdef D_HI_FIVE1
    #include <stdlib.h>
#endif

#include "common_types.h"
#include "demo_platform_al.h"

#include "psp_api.h"

#include "rtosal_task_api.h"
#include "rtosal_mutex_api.h"

/**
* definitions
*/

/* The rate the data is sent to the queue, specified in milliseconds, and
converted to ticks (using the definition of D_TICK_TIME_MS) */
#define D_MAIN_QUEUE_SEND_PERIOD_TICKS            200/D_TICK_TIME_MS

/* The period of the example software timer, specified in milliseconds, and
converted to ticks (using the definition of D_TICK_TIME_MS) */
#define D_MAIN_SOFTWARE_TIMER_PERIOD_TICKS        1000/D_TICK_TIME_MS

/* The number of items the queue can hold.  This is 1 as the receive task
has a higher priority than the send task, so will remove items as they are added,
meaning the send task should always find the queue empty. */
#define D_MAIN_QUEUE_LENGTH                    ( 1 )

/* Stack size of the tasks in this application */
#define D_RX_TASK_STACK_SIZE  600
#define D_TX_TASK_STACK_SIZE  450

extern void* __OVERLAY_STORAGE_START__ADDRESS__;

#ifdef D_COMRV_FW_INSTRUMENTATION
comrvInstrumentationArgs_t g_stInstArgs;
#endif /* D_COMRV_FW_INSTRUMENTATION */

#define OVL_LedBlink _OVERLAY_
#define OVL_SendMsg  _OVERLAY_
#define OVL_MyItoa   _OVERLAY_

//extern int OVL_benchmark benchmark(void);

volatile u32_t gLedBlink = 0;

static rtosalMutex_t stComrvMutex;
static rtosalTask_t stLedTask;
static rtosalTask_t stTxTask;
static rtosalStackType_t uLedTaskStackBuffer[D_RX_TASK_STACK_SIZE];
static rtosalStackType_t uTxTaskStackBuffer[D_TX_TASK_STACK_SIZE];

static void demoLedTask( void *pvParameters );
static void demoRtosalSendMsgTask( void *pvParameters );
static void demoCreateTasks(void *pParam);

/* overlay function */
void OVL_LedBlink LedBlink(void)
{
   demoOutputLed(D_LED_BLUE_ON);
   rtosalTaskSleep(500);
   demoOutputLed(D_LED_GREEN_ON);
}
unsigned int g_MyCounter = 0;

/* integer to ascii */
u08_t OVL_MyItoa MyItoa(u32_t uiValue, u08_t *pStr)
{
   g_MyCounter++;
   return 0;
}

/* send counter value to uart */
void OVL_SendMsg SendMsg(u32_t uiCount)
{
   u08_t ucSize, ucArr[15];
   ucSize = MyItoa(uiCount, ucArr) + 1;
   demoOutputMsg(ucArr, ucSize);
}

/**
 * demoStart - startup point of the demo application. called from main function.
 *
 */
void demoStart(void)
{
   /* run the rtos */
   rtosalStart(demoCreateTasks);
}

/**
 * demoCreateTasks
 *
 * Initialize the application:
 * - Register unhandled exceptions, Ecall exception and Timer ISR
 * - Create the Tx and Led tasks
 *
 * This function is called from RTOS abstraction layer. After its completion, the
 * scheduler is kicked on and the tasks are start to be active
 *
 */
void demoCreateTasks(void *pParam)
{
   u32_t res;
   pspExceptionCause_t cause;
   comrvInitArgs_t stComrvInitArgs = { 1 };

   /* set mutex address */
   stComrvInitArgs.pMutex = &stComrvMutex;

   /* init comrv */
   comrvInit(&stComrvInitArgs);

   /* Disable the machine & timer interrupts until setup is done. */
   M_PSP_CLEAR_CSR(mie, D_PSP_MIP_MEIP);
   M_PSP_CLEAR_CSR(mie, D_PSP_MIP_MTIP);

   /* register exception handlers - at the beginning, register 'pspTrapUnhandled' to all exceptions */
   for (cause = E_EXC_INSTRUCTION_ADDRESS_MISALIGNED ; cause < E_EXC_LAST ; cause++)
   {
      /* Skip ECALL entry as we already registered there a handler */
      if (E_EXC_ENVIRONMENT_CALL_FROM_MMODE == cause)
      {
        continue;
      }
      pspRegisterExceptionHandler(pspTrapUnhandled, cause);
   }

   /* Enable the Machine-External bit in MIE */
   M_PSP_SET_CSR(mie, D_PSP_MIP_MEIP);

   /* Create the led task */
   res = rtosalTaskCreate(&stLedTask, (s08_t*)"LED", E_RTOSAL_PRIO_30,
                          demoLedTask, (u32_t)NULL, D_RX_TASK_STACK_SIZE,
                          uLedTaskStackBuffer, 0, D_RTOSAL_AUTO_START, 0);
   if (res != D_RTOSAL_SUCCESS)
   {
      demoOutputMsg("Led task creation failed\n", 25);
      M_ENDLESS_LOOP();
   }

   /* Create the queue-send task in exactly the same way */
   res = rtosalTaskCreate(&stTxTask, (s08_t*)"TX", E_RTOSAL_PRIO_30,
              demoRtosalSendMsgTask, (u32_t)NULL, D_TX_TASK_STACK_SIZE,
             uTxTaskStackBuffer, 0, D_RTOSAL_AUTO_START, 0);
   if (res != D_RTOSAL_SUCCESS)
   {
      demoOutputMsg("Tx-Task creation failed\n", 24);
      M_ENDLESS_LOOP();
   }

   /* create the comrv mutex */
   res = rtosalMutexCreate(&stComrvMutex, (s08_t*)"comrv", D_RTOSAL_INHERIT);
   if (res != D_RTOSAL_SUCCESS)
   {
      demoOutputMsg("comrv mutex creation failed\n", 28);
      M_ENDLESS_LOOP();
   }

}

/**
 * demoLedTask - Rx task function
 *
 * void *pvParameters - not in use
 *
 */
static void demoLedTask( void *pvParameters )
{
   while (1)
   {
      LedBlink();
   }
}

/**
 * demoRtosalSendMsgTask - Tx task function
 *
 * void *pvParameters - not in use
 *
 */
static void demoRtosalSendMsgTask( void *pvParameters )
{
   u32_t uiCount = 0;
   while (1)
   {
      SendMsg(uiCount);
      uiCount++;
   }
}

/**
* memory copy hook
*
* @param  none
*
* @return none
*/
void comrvMemcpyHook(void* pDest, void* pSrc, u32_t sizeInBytes)
{
   u32_t loopCount = sizeInBytes/(sizeof(u32_t)), i;
   /* copy dwords */
   for (i = 0; i < loopCount ; i++)
   {
      *((u32_t*)pDest + i) = *((u32_t*)pSrc + i);
   }
   loopCount = sizeInBytes - (loopCount*(sizeof(u32_t)));
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
   comrvMemcpyHook(pLoadArgs->pDest, (u08_t*)&__OVERLAY_STORAGE_START__ADDRESS__ + pLoadArgs->uiGroupOffset, pLoadArgs->uiSizeInBytes);
   return pLoadArgs->pDest;
}

/**
* notification hook
*
* @param  pInstArgs - pointer to instrumentation arguments
*
* @return none
*/
#ifdef D_COMRV_FW_INSTRUMENTATION
void comrvInstrumentationHook(const comrvInstrumentationArgs_t* pInstArgs)
{
   g_stInstArgs = *pInstArgs;
}
#endif /* D_COMRV_FW_INSTRUMENTATION */

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
   /* we can't continue so loop forever */
   while (1);
}

/**
* crc calculation hook (itt)
*
* @param pAddress         - memory address to calculate
*        memSizeInBytes   - number of bytes to calculate
*        uiExpectedResult - expected crc result
*
* @return calculated CRC
*/
u32_t comrvCrcCalcHook (const void* pAddress, u16_t usMemSizeInBytes, u32_t uiExpectedResult)
{
   return 0;
}

/******************** start temporary build issue workaround ****************/
void _kill(void)
{
}
void _sbrk(void)
{
}
void _getpid(void)
{
}
/******************** end temporary build issue workaround ****************/

