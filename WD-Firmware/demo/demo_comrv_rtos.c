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
#ifdef D_HI_FIVE1
    #include <stdlib.h>
#endif
#include "common_types.h"
#include "comrv_api.h"
#include "demo_platform_al.h"
#include "rtosal_task_api.h"
#include "rtosal_mutex_api.h"
#include "rtosal_queue_api.h"

/**
* definitions
*/

/* The rate the data is sent to the queue, specified in milliseconds, and
converted to ticks (using the definition of D_TICK_TIME_MS) */
#define D_MAIN_QUEUE_SEND_PERIOD_TICKS         (200/D_TICK_TIME_MS)

/* The number of items the queue can hold.  This is 1 as the receive task
has a higher priority than the send task, so will remove items as they are added,
meaning the send task should always find the queue empty. */
#define D_MAIN_QUEUE_LENGTH                    (1)

/* Stack size of the tasks in this application */
#define D_RX_TASK_STACK_SIZE                    450
#define D_TX_TASK_STACK_SIZE                    450

/**
* macros
*/

/**
* types
*/

/**
* local prototypes
*/
void demoRtosalCreateTasks(void *pParam);
void demoRtosalReceiveMsgTask( void *pvParameters );
void demoRtosalTxMsgTask( void *pvParameters );

/**
* external prototypes
*/

/**
* global variables
*/
extern void* _OVERLAY_STORAGE_START_ADDRESS_;

#ifdef D_COMRV_FW_INSTRUMENTATION
comrvInstrumentationArgs_t g_stInstArgs;
#endif /* D_COMRV_FW_INSTRUMENTATION */

/* globals related to the tasks in this demo */
static rtosalTask_t stRxTask;
static rtosalTask_t stTxTask;
static rtosalStackType_t uRxTaskStackBuffer[D_RX_TASK_STACK_SIZE];
static rtosalStackType_t uTxTaskStackBuffer[D_TX_TASK_STACK_SIZE];
static s08_t cQueueBuffer[D_MAIN_QUEUE_LENGTH * sizeof(u32_t)];
static rtosalMsgQueue_t stMsgQueue;
static rtosalMutex_t stComrvMutex;

/**
* functions
*/

/**
 * demoStart - startup point of the demo application. called from main function.
 *
 */
void demoStart(void)
{
   comrvInitArgs_t stComrvInitArgs = { 1 };

   /* set mutex address */
   stComrvInitArgs.pStMutex = &stComrvMutex;

   /* init comrv */
   comrvInit(&stComrvInitArgs);

   rtosalStart(demoRtosalCreateTasks);
}

/**
 * demoRtosalCreateTasks
 *
 * Initialize the application:
 * - Register unhandled exceptions, Ecall exception and Timer ISR
 * - Create the message queue
 * - Create the Tx and Rx tasks
 * - Create the comrv mutex
 *
 * This function is called from RTOS abstraction layer. After its
 * completion, the scheduler is kicked on and the tasks are start to be active
 *
 */
void demoRtosalCreateTasks(void *pParam)
{

   u32_t res;
   pspExceptionCause_t cause;

   /* Disable the machine & timer interrupts until setup is done. */
   M_PSP_CLEAR_CSR(mie, D_PSP_MIP_MEIP);
   M_PSP_CLEAR_CSR(mie, D_PSP_MIP_MTIP);

   /* register exception handlers - at the beginning, register 'pspTrapUnhandled' to all exceptions */
   for (cause = E_EXC_INSTRUCTION_ADDRESS_MISALIGNED ; cause < E_EXC_LAST_COMMON ; cause++)
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

   /* Create the queue used by the send-msg and receive-msg tasks. */
   res = rtosalMsgQueueCreate(&stMsgQueue, cQueueBuffer, D_MAIN_QUEUE_LENGTH,
            sizeof(u32_t), NULL);
   if (res != D_RTOSAL_SUCCESS)
   {
      demoOutputMsg("Msg-Q creation failed\n", 22);
      M_ENDLESS_LOOP();
   }

   /* Create the rx task */
   res = rtosalTaskCreate(&stRxTask, (s08_t*)"RX", E_RTOSAL_PRIO_29,
            demoRtosalReceiveMsgTask, (u32_t)NULL, D_RX_TASK_STACK_SIZE,
            uRxTaskStackBuffer, 0, D_RTOSAL_AUTO_START, 0);
   if (res != D_RTOSAL_SUCCESS)
   {
      demoOutputMsg("Rx-Task creation failed\n", 24);
      M_ENDLESS_LOOP();
   }

   /* Create the tx task in exactly the same way */
   res = rtosalTaskCreate(&stTxTask, (s08_t*)"TX", E_RTOSAL_PRIO_30,
            demoRtosalTxMsgTask, (u32_t)NULL, D_TX_TASK_STACK_SIZE,
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
 * send an item to the rtos queue
 *
 * pValueToSend - Item to send to the queue
 */
void _OVERLAY_ OvlFuncTx(u32_t* pValueToSend)
{
   /* Place this task in the blocked state until it is time to run again.
   The block time is specified in ticks.
   The task will not consume any CPU time while it is in the Blocked state. */
   rtosalTaskSleep(D_MAIN_QUEUE_SEND_PERIOD_TICKS);

   demoOutputMsg("Sending to queue\n", 17);

   /* Send to the queue - causing the queue receive task to unblock and
   increment its counter.  0 is used as the block time so the sending
   operation will not block - it shouldn't need to block as the queue
   should always be empty at this point in the code. */
   rtosalMsgQueueSend(&stMsgQueue, pValueToSend, 0, D_RTOSAL_FALSE);
}

/**
 * Tx task function
 *
 * pvParameters - not in use
 */
void demoRtosalTxMsgTask( void *pvParameters )
{
   u32_t ulValueToSend = 0UL;

   for( ;; )
   {
      /* value to send to the queue */
      ulValueToSend++;

      /* send to queue */
      OvlFuncTx(&ulValueToSend);
   }
}

/**
 * Rx function
 *
 * pvParameters - holds the received queue item value
 */
void _OVERLAY_ OvlFuncRx(u32_t* pReceivedValue)
{
   rtosalMsgQueueRecieve(&stMsgQueue, pReceivedValue, portMAX_DELAY);
}

/**
 * demoRtosalReceiveMsgTask - Rx task function
 *
 * void *pvParameters - not in use
 */
void demoRtosalReceiveMsgTask( void *pvParameters )
{
#ifdef D_HI_FIVE1
   char stringValue[10];
#endif /* D_HI_FIVE1 */
   u32_t ulReceivedValue;

   for( ;; )
   {
      OvlFuncRx(&ulReceivedValue);

#ifdef D_HI_FIVE1
      itoa(ulReceivedValue,stringValue, 10);
      demoOutputMsg("Received: ", 10);
      demoOutputMsg(stringValue, 3);
      demoOutputMsg("\n",1);
#else
      M_PSP_NOP();
      M_PSP_NOP();
      M_PSP_NOP();
      M_PSP_NOP();
#endif
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
   comrvMemcpyHook(pLoadArgs->pDest, (u08_t*)&_OVERLAY_STORAGE_START_ADDRESS_ + pLoadArgs->uiGroupOffset, pLoadArgs->uiSizeInBytes);
   /* order device I/O an memory accesses */
   asm volatile ("fence");
   /* sync the instruction and data stream */
   asm volatile ("fence.i");
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
   M_ENDLESS_LOOP();
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
