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
* @file   rtosal_queue.c
* @author Ronen Haen
* @date   21.01.2019 
* @brief  The file implements the RTOS AL queue API
* 
*/

/**
* include files
*/
#include "common_types.h"
#include "common_defines.h"
#include "rtosal_queue_api.h"
#include "rtosal_macro.h"
#include "rtosal.h"
#include "psp_api.h"
#ifdef D_USE_FREERTOS
   #include "queue.h"
#endif /* #ifdef D_USE_FREERTOS */

/**
* definitions
*/

/**
* macros
*/

/**
* types
*/

/**
* local prototypes
*/
D_ALWAYS_INLINE
D_INLINE u32_t msgQueueSend(rtosalMsgQueue_t* pRtosalMsgQueueCb, const void* pRtosalMsgQueueItem,
                   u32_t uiWaitTimeoutTicks, u32_t uiSendToFront);

/**
* external prototypes
*/

/**
* global variables
*/


/**
* Create a message queue
*
* @param pRtosalMsgQueueCb       - pointer to queue control block to be created
* @param pRtosMsgQueueBuffer   - queue buffer (its size must be
*                               uiRtosMsgQueueSize * uiRtosMsgQueueItemSize)
* @param uiRtosMsgQueueSize     - maximum number of items
* @param uiRtosMsgQueueItemSize - queue item size in bytes
* @param pRtosalMsgQueueName     - pointer to the name of the queue
*
* @return u32_t                - D_RTOSAL_SUCCESS
*                             - D_RTOSAL_QUEUE_ERROR Invalid message queue pointer
*                             - D_RTOSAL_PTR_ERROR Invalid starting adduiRess
*                               of the message queue
*                             - D_RTOSAL_SIZE_ERROR Size of message queue is invalid
*                             - D_RTOSAL_CALLER_ERROR Invalid caller of this service
*/
u32_t rtosalMsgQueueCreate(rtosalMsgQueue_t* pRtosalMsgQueueCb, void* pRtosMsgQueueBuffer,
                           u32_t uiRtosMsgQueueSize, u32_t uiRtosMsgQueueItemSize,
                           s08_t* pRtosalMsgQueueName)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalMsgQueueCb, pRtosalMsgQueueCb == NULL, D_RTOSAL_QUEUE_ERROR);

#ifdef D_USE_FREERTOS
   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosMsgQueueBuffer, pRtosMsgQueueBuffer == NULL, D_RTOSAL_PTR_ERROR);
   /* create the queue */
   pRtosalMsgQueueCb->msgQueueHandle = xQueueCreateStatic(uiRtosMsgQueueSize, uiRtosMsgQueueItemSize,
                                           (uint8_t*)pRtosMsgQueueBuffer,
                                           (StaticQueue_t*)pRtosalMsgQueueCb->cMsgQueueCB);
   /* queue created successfully */
   if (pRtosalMsgQueueCb->msgQueueHandle != NULL)
   {
      /* assign a name to the created queue */
      vQueueAddToRegistry(pRtosalMsgQueueCb->msgQueueHandle, (const char*)pRtosalMsgQueueName);
      uiRes = D_RTOSAL_SUCCESS;
   }
   else
   {
      uiRes = D_RTOSAL_QUEUE_ERROR;
   }
#elif D_USE_THREADX
   uiRes = tx_queue_create((TX_QUEUE*)pRtosalMsgQueueCb->cMsgQueueCB,
                         pRtosalMsgQueueName, uiRtosMsgQueueItemSize,
                         pRtosMsgQueueBuffer, uiRtosMsgQueueSize);
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Destroy a queue
*
* @param pRtosalMsgQueueCb - pointer to queue control block to be destroyed
*
* @return u32_t          - D_RTOSAL_SUCCESS
*                        - D_RTOSAL_QUEUE_ERROR Invalid message queue pointer
*                        - D_RTOSAL_CALLER_ERROR Invalid caller of this service
*/
u32_t rtosalMsgQueueDestroy(rtosalMsgQueue_t* pRtosalMsgQueueCb)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalMsgQueueCb, pRtosalMsgQueueCb == NULL, D_RTOSAL_QUEUE_ERROR);

#ifdef D_USE_FREERTOS
   vQueueDelete(pRtosalMsgQueueCb->msgQueueHandle);
   uiRes = D_RTOSAL_SUCCESS;
#elif D_USE_THREADX
   uiRes = tx_queue_delete((TX_QUEUE*)pRtosalMsgQueueCb->cMsgQueueCB);
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Add an item to the queue front/back
*
* @param pRtosalMsgQueueCb   - pointer to queue control block to add the item to
* @param pRtosalMsgQueueItem - pointer to a memory containing the item add (copy)
* @param uiWaitTimeoutTicks - in case queue is full, how many ticks to wait until
*                            the queue can accommodate a new item: D_RTOSAL_NO_WAIT,
*                            D_RTOSAL_WAIT_FOREVER or timer ticks value
* @param uiSendToFront      - D_RTOSAL_TRUE: place the item in the queue front;
*                            D_RTOSAL_FALSE: place the item in the queue back
*
* @return u32_t            - D_RTOSAL_SUCCESS
*                          - D_RTOSAL_DELETED Message queue was deleted while
*                            thread was suspended
*                          - D_RTOSAL_QUEUE_FULL Service was unable to send message 
*                            because the queue was full for the duration of 
*                            the specified time to wait
*                          - D_RTOSAL_WAIT_ABORTED Suspension was aborted by 
*                            another thread, timer, or ISR
*                          - D_RTOSAL_QUEUE_ERROR Invalid message queue pointer
*                          - D_RTOSAL_PTR_ERROR Invalid source pointer for message
*                          - D_RTOSAL_WAIT_ERROR A wait option other than TX_NO_WAIT
*                            was specified on a call from a nonthread
*/
u32_t rtosalMsgQueueSend(rtosalMsgQueue_t* pRtosalMsgQueueCb, const void* pRtosalMsgQueueItem,
                       u32_t uiWaitTimeoutTicks, u32_t uiSendToFront)
{
   return msgQueueSend(pRtosalMsgQueueCb, pRtosalMsgQueueItem,
                       uiWaitTimeoutTicks, uiSendToFront);
}

#ifdef D_USE_FREERTOS
D_ALWAYS_INLINE
D_INLINE u32_t msgQueueSend(rtosalMsgQueue_t* pRtosalMsgQueueCb, const void* pRtosalMsgQueueItem,
                       u32_t uiWaitTimeoutTicks, u32_t uiSendToFront)
{
   u32_t uiRes;
   /* specify if a context switch is needed as a uiResult calling FreeRTOS ...ISR function */
   BaseType_t xHigherPriorityTaskWoken = pdFALSE;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalMsgQueueCb, pRtosalMsgQueueCb == NULL, D_RTOSAL_QUEUE_ERROR);
   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalMsgQueueItem, pRtosalMsgQueueItem == NULL, D_RTOSAL_PTR_ERROR);

   /* check if message should be sent to the queue front */
   if (uiSendToFront == D_RTOSAL_TRUE)
   {
      /* msgQueueSend invoked from an ISR context */
      if (pspIsInterruptContext() == D_INT_CONTEXT)
      {
         /* send the queue message */
         uiRes = xQueueSendToFrontFromISR(pRtosalMsgQueueCb->msgQueueHandle, pRtosalMsgQueueItem, &xHigherPriorityTaskWoken);
      }
      else
      {
         uiRes = xQueueSendToFront(pRtosalMsgQueueCb->msgQueueHandle, pRtosalMsgQueueItem, uiWaitTimeoutTicks);
      }
   }
   /* send message to the queue back */
   else
   {
      /* msgQueueSend invoked from an ISR context */
      if (pspIsInterruptContext() == D_INT_CONTEXT)
      {
         uiRes = xQueueSendToBackFromISR(pRtosalMsgQueueCb->msgQueueHandle, pRtosalMsgQueueItem, &xHigherPriorityTaskWoken);
      }
      else
      {
         uiRes = xQueueSendToBack(pRtosalMsgQueueCb->msgQueueHandle, pRtosalMsgQueueItem, uiWaitTimeoutTicks);
      }
   } /* if (uiSendToFront == D_RTOSAL_TRUE) */

   /* queue was full */
   if (uiRes == errQUEUE_FULL)
   {
      /* The message could not be sent */
      uiRes = D_RTOSAL_QUEUE_FULL;
   }
   else
   {
      uiRes = D_RTOSAL_SUCCESS;
   }
   /* due to the message send we got an indicating that a context 
   switch should be requested before the interrupt exits */ 
   if (uiRes == D_RTOSAL_SUCCESS && xHigherPriorityTaskWoken == pdTRUE)
   {
      rtosalContextSwitchIndicationSet();
   }

   return uiRes;
}
#elif D_USE_THREADX
D_ALWAYS_INLINE
D_INLINE u32_t msgQueueSend(rtosalMsgQueue_t* pRtosalMsgQueueCb, void* pRtosalMsgQueueItem,
                       u32_t uiWaitTimeoutTicks, u32_t uiSendToFront)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalMsgQueueCb, pRtosalMsgQueueCb == NULL, D_RTOSAL_QUEUE_ERROR);

   /* check if message should be sent to the queue front */
   if (uiSendToFront == D_RTOSAL_TRUE)
   {
      uiRes = tx_queue_front_send((TX_QUEUE*)pRtosalMsgQueueCb->cMsgQueueCB, pRtosalMsgQueueItem, uiWaitTimeoutTicks);
   }
   else
   {
      uiRes = tx_queue_send((TX_QUEUE*)pRtosalMsgQueueCb->cMsgQueueCB, pRtosalMsgQueueItem, uiWaitTimeoutTicks);
   }

   return uiRes;
}
#endif /* #ifdef D_USE_FREERTOS */

/**
* Retrieve an item from the queue. The message to retrieve shall be copied to a
* user provided buffer and shall be deleted from the message queue
*
* @param pRtosalMsgQueueCb   - pointer to queue control block to get the item from
* @param pRtosalMsgQueueItem - pointer to a memory which the item shall be copied to
* @param uiWaitTimeoutTicks - in case queue is empty, how many ticks to wait until
*                            the queue becomes non-empty: D_RTOSAL_NO_WAIT, D_RTOSAL_WAIT_FOREVER
*                            or timer ticks value
*
* @return u32_t            - D_RTOSAL_SUCCESS
*                          - D_RTOSAL_DELETED Message queue was deleted while
*                            thread was suspended
*                          - D_RTOSAL_QUEUE_EMPTY Service was unable to retrieve 
*                            a message because the queue was empty for the
*                            duration of the specified time to wait
*                          - D_RTOSAL_WAIT_ABORTED Suspension was aborted by 
*                            another thread, timer, or ISR
*                          - D_RTOSAL_QUEUE_ERROR Invalid message queue pointer
*                          - D_RTOSAL_PTR_ERROR Invalid starting adduiRess
*                          - D_RTOSAL_WAIT_ERROR A wait option other than TX_NO_WAIT
*                            was specified on a call from a nonthread
*/
u32_t rtosalMsgQueueRecieve(rtosalMsgQueue_t* pRtosalMsgQueueCb, void* pRtosalMsgQueueItem,
                            u32_t uiWaitTimeoutTicks)
{
   u32_t uiRes;
#ifdef D_USE_FREERTOS
   /* specify if a context switch is needed as a uiResult calling FreeRTOS ...ISR function */
   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#endif /* #ifdef D_USE_FREERTOS */

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalMsgQueueCb, pRtosalMsgQueueCb == NULL, D_RTOSAL_QUEUE_ERROR);

#ifdef D_USE_FREERTOS
   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalMsgQueueItem, pRtosalMsgQueueItem == NULL, D_RTOSAL_PTR_ERROR);
   /* rtosalMsgQueueRecieve invoked from an ISR context */
   if (pspIsInterruptContext() == D_INT_CONTEXT)
   {
      uiRes = xQueueReceiveFromISR(pRtosalMsgQueueCb->msgQueueHandle, pRtosalMsgQueueItem, &xHigherPriorityTaskWoken);
   }
   else
   {
      uiRes = xQueueReceive(pRtosalMsgQueueCb->msgQueueHandle, pRtosalMsgQueueItem, uiWaitTimeoutTicks);
   }

   /* message sent successfuly */
   if (uiRes == pdPASS)
   {
      uiRes = D_RTOSAL_SUCCESS;
   }
   else
   {
      /* queue is empty */
      uiRes = D_RTOSAL_QUEUE_EMPTY;
   }
   /* due to the message send we got an indicating that a context 
   switch should be requested before the interrupt exits */ 
   if (uiRes == D_RTOSAL_SUCCESS && xHigherPriorityTaskWoken == pdTRUE)
   {
      rtosalContextSwitchIndicationSet();
   }
#elif D_USE_THREADX
   uiRes = tx_queue_receive((TX_QUEUE*)pRtosalMsgQueueCb->cMsgQueueCB, pRtosalMsgQueueItem, uiWaitTimeoutTicks);
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}
