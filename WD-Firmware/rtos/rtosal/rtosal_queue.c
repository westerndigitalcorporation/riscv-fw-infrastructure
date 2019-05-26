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
#include "psp_defines.h"
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
* @param pRtosalMsgQueueCb      - pointer to queue control block to be created
* @param pRtosMsgQueueBuffer    -
* @param uiRtosMsgQueueSize     -
* @param uiRtosMsgQueueItemSize -
* @param pRtosalMsgQueueName    -
*
* @return u32_t               - D_RTOSAL_SUCCESS
*                             - D_RTOSAL_QUEUE_ERROR
*                             - D_RTOSAL_PTR_ERROR
*                             - D_RTOSAL_SIZE_ERROR
*                             - D_RTOSAL_CALLER_ERROR
*/
RTOSAL_SECTION u32_t rtosalMsgQueueCreate(rtosalMsgQueue_t* pRtosalMsgQueueCb, void* pRtosMsgQueueBuffer,
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
   // TODO:
   //uiRes = add a call to ThreadX queue create API
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Destroy a queue
*
* @param pRtosalMsgQueueCb - pointer to queue control block to be destroyed
*
* @return u32_t          - D_RTOSAL_SUCCESS
*                        - D_RTOSAL_QUEUE_ERROR
*                        - D_RTOSAL_CALLER_ERROR
*/
RTOSAL_SECTION u32_t rtosalMsgQueueDestroy(rtosalMsgQueue_t* pRtosalMsgQueueCb)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalMsgQueueCb, pRtosalMsgQueueCb == NULL, D_RTOSAL_QUEUE_ERROR);

#ifdef D_USE_FREERTOS
   vQueueDelete(pRtosalMsgQueueCb->msgQueueHandle);
   uiRes = D_RTOSAL_SUCCESS;
#elif D_USE_THREADX
   // TODO:
   //uiRes = add a call to ThreadX queue destroy API
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Add an item to the queue front/back
*
* @param pRtosalMsgQueueCb   - pointer to queue control block to add the item to
* @param pRtosalMsgQueueItem -
* @param uiWaitTimeoutTicks  -
* @param uiSendToFront       - D_RTOSAL_TRUE:
*                              D_RTOSAL_FALSE:
*
* @return u32_t            - D_RTOSAL_SUCCESS
*                          - D_RTOSAL_DELETED
*                          - D_RTOSAL_QUEUE_FULL
*                          - D_RTOSAL_WAIT_ABORTED
*                          - D_RTOSAL_QUEUE_ERROR
*                          - D_RTOSAL_PTR_ERROR
*                          - D_RTOSAL_WAIT_ERROR
*/
RTOSAL_SECTION u32_t rtosalMsgQueueSend(rtosalMsgQueue_t* pRtosalMsgQueueCb, const void* pRtosalMsgQueueItem,
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
      if (pspIsInterruptContext() == D_PSP_INT_CONTEXT)
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
      if (pspIsInterruptContext() == D_PSP_INT_CONTEXT)
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
   // TODO:
   //uiRes = add a call to ThreadX queue add front
   }
   else
   {
   // TODO:
   //uiRes = add a call to ThreadX queue add back
   }

   return uiRes;
}
#endif /* #ifdef D_USE_FREERTOS */

/**
* Retrieve an item from the queue.
*
* @param pRtosalMsgQueueCb   - pointer to queue control block to get the item from
* @param pRtosalMsgQueueItem -
* @param uiWaitTimeoutTicks  -
*
* @return u32_t            - D_RTOSAL_SUCCESS
*                          - D_RTOSAL_DELETED
*                          - D_RTOSAL_QUEUE_EMPTY
*                          - D_RTOSAL_WAIT_ABORTED
*                          - D_RTOSAL_QUEUE_ERROR
*                          - D_RTOSAL_PTR_ERROR
*                          - D_RTOSAL_WAIT_ERROR
*/
RTOSAL_SECTION u32_t rtosalMsgQueueRecieve(rtosalMsgQueue_t* pRtosalMsgQueueCb, void* pRtosalMsgQueueItem,
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
   if (pspIsInterruptContext() == D_PSP_INT_CONTEXT)
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
   // TODO:
   //uiRes = add a call to ThreadX queue recieve API
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}
