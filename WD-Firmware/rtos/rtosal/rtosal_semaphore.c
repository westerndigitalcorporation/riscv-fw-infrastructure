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
* @file   rtosal_semaphore.c
* @author Ronen Haen
* @date   21.01.2019 
* @brief  The file implements the RTOS AL queue API
* 
*/

/**
* include files
*/
#include "rtosal_semaphore_api.h"
#include "rtosal_macro.h"
#include "rtosal.h"
#include "psp_api.h"
#ifdef D_USE_FREERTOS
   #include "semphr.h"
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

/**
* external prototypes
*/

/**
* global variables
*/

/**
* Create a semaphore
*
* @param  pRtosalSemaphoreCb     - pointer to semaphore control block to be created
* @param  pRtosalSemaphoreName   - pointer to the name of the semaphore
* @param  iSemaphoreInitialCount - semaphore initial count
* @param  uiSemaphoreMaxCount    - maximum semaphore count value that can be reached
*
* @return u32_t                  - D_RTOSAL_SUCCESS
*                                - D_RTOSAL_SEMAPHORE_ERROR Invalid semaphore pointer
*                                - D_RTOSAL_CALLER_ERROR Invalid caller of this service
*/
u32_t rtosalSemaphoreCreate(rtosalSemaphore_t* pRtosalSemaphoreCb, s08_t *pRtosalSemaphoreName,
                            s32_t iSemaphoreInitialCount, u32_t uiSemaphoreMaxCount)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalSemaphoreCb, pRtosalSemaphoreCb == NULL, D_RTOSAL_SEMAPHORE_ERROR);

#ifdef D_USE_FREERTOS
   /* create the semaphore */
   pRtosalSemaphoreCb->semaphoreHandle = xSemaphoreCreateCountingStatic(uiSemaphoreMaxCount,
                                               iSemaphoreInitialCount,
                                               (StaticSemaphore_t*)pRtosalSemaphoreCb->cSemaphoreCB);
   /* semaphore created successfuly */
   if (pRtosalSemaphoreCb->semaphoreHandle != NULL)
   {
      /* assign a name to the created semaphore */
      vQueueAddToRegistry((QueueHandle_t)pRtosalSemaphoreCb->semaphoreHandle, (const char*)pRtosalSemaphoreName);
      uiRes = D_RTOSAL_SUCCESS;
   }
   else
   {
      uiRes = D_RTOSAL_SEMAPHORE_ERROR;
   }
#elif D_USE_THREADX
   uiRes = tx_semaphore_create((TX_SEMAPHORE*)pRtosalSemaphoreCb->cMsgQueueCB,
                             pRtosalSemaphoreName, iSemaphoreInitialCount);
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Delete a semaphore
*
* @param  pRtosalSemaphoreCb - pointer to semaphore control block to be destroyed
*
* @return u32_t           - D_RTOSAL_SUCCESS
*                         - D_RTOSAL_SEMAPHORE_ERROR Invalid semaphore pointer
*                         - D_RTOSAL_CALLER_ERROR Invalid caller of this service
*/
u32_t rtosalSemaphoreDestroy(rtosalSemaphore_t* pRtosalSemaphoreCb)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalSemaphoreCb, pRtosalSemaphoreCb == NULL, D_RTOSAL_SEMAPHORE_ERROR);

#ifdef D_USE_FREERTOS
   vSemaphoreDelete(pRtosalSemaphoreCb->semaphoreHandle);
   uiRes = D_RTOSAL_SUCCESS;
#elif D_USE_THREADX
   uiRes = tx_semaphore_delete((TX_SEMAPHORE*)pRtosalSemaphoreCb->cMsgQueueCB);
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Wait for a semaphore to become available
*
* @param  pRtosalSemaphoreCb  - pointer to semaphore control block to wait for
* @param  uiWaitTimeoutTicks - define how many ticks to wait in case the
*                             semaphore isn’t available: D_RTOSAL_NO_WAIT,
*                             D_RTOSAL_WAIT_FOREVER or timer ticks value
*
* @return u32_t            - D_RTOSAL_SUCCESS
*                         - D_RTOSAL_DELETED Counting semaphore was deleted 
*                            while task was suspended
*                          - D_RTOSAL_NO_INSTANCE Service was unable to retrieve 
*                            an instance of the counting semaphore (semaphore
*                            count is zero within the specified time to wait)
*                          - D_RTOSAL_WAIT_ABORTED Suspension was aborted by another
*                            task, timer, or ISR
*                          - D_RTOSAL_SEMAPHORE_ERROR Invalid counting semaphore 
*                            pointer
*                          - D_RTOSAL_WAIT_ERROR A wait option other than D_RTOSAL_NO_WAIT
*                            was specified on a call from a non task
*/
u32_t rtosalSemaphoreWait(rtosalSemaphore_t* pRtosalSemaphoreCb, u32_t uiWaitTimeoutTicks)
{
   u32_t uiRes;
#ifdef D_USE_FREERTOS
   /* specify if a context switch is needed as a uiResult calling FreeRTOS ...ISR function */
   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#endif /* #ifdef D_USE_FREERTOS */

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalSemaphoreCb, pRtosalSemaphoreCb == NULL, D_RTOSAL_SEMAPHORE_ERROR);

#ifdef D_USE_FREERTOS
   /* rtosalSemaphoreWait invoked from an ISR context */
   if (pspIsInterruptContext() == D_INT_CONTEXT)
   {
      uiRes = xSemaphoreTakeFromISR(pRtosalSemaphoreCb->semaphoreHandle, &xHigherPriorityTaskWoken);
   }
   else
   {
      uiRes = xSemaphoreTake(pRtosalSemaphoreCb->semaphoreHandle, uiWaitTimeoutTicks);
   }
   /* successfuly obtained the semaphore */
   if (uiRes == pdPASS)
   {
      uiRes = D_RTOSAL_SUCCESS;
   }
   else
   {
      uiRes = D_RTOSAL_NO_INSTANCE;
   }
   /* due to the wait we got an indicating that a context 
   switch should be requested before the interrupt exits */ 
   if (uiRes == D_RTOSAL_SUCCESS && xHigherPriorityTaskWoken == pdTRUE)
   {
      rtosalContextSwitchIndicationSet();
   }
#elif D_USE_THREADX
   uiRes = tx_semaphore_get((TX_SEMAPHORE*)pRtosalSemaphoreCb->cMsgQueueCB, uiWaitTimeoutTicks);
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Release a semaphore
*
* @param  pRtosalSemaphoreCb - pointer to semaphore control block to be released
*
* @return u32_t           - D_RTOSAL_SUCCESS
*                         - D_RTOSAL_SEMAPHORE_ERROR Invalid semaphore pointer
*/
u32_t rtosalSemaphoreRelease(rtosalSemaphore_t* pRtosalSemaphoreCb)
{
   u32_t uiRes;
#ifdef D_USE_FREERTOS
   /* specify if a context switch is needed as a uiResult calling FreeRTOS ...ISR function */
   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#endif /* #ifdef D_USE_FREERTOS */

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalSemaphoreCb, pRtosalSemaphoreCb == NULL, D_RTOSAL_SEMAPHORE_ERROR);

#ifdef D_USE_FREERTOS
   /* rtosalSemaphoreRelease invoked from an ISR context */
   if (pspIsInterruptContext() == D_INT_CONTEXT)
   {
      uiRes = xSemaphoreGiveFromISR(pRtosalSemaphoreCb->semaphoreHandle, &xHigherPriorityTaskWoken);
   }
   else
   {
      uiRes = xSemaphoreGive(pRtosalSemaphoreCb->semaphoreHandle);
   }
   /* give eas successful */
   if (uiRes == pdPASS)
   {
      uiRes = D_RTOSAL_SUCCESS;
   }
   else
   {
      uiRes = D_RTOSAL_NO_INSTANCE;
   }
   /* due to the wait we got an indicating that a context 
   switch should be requested before the interrupt exits */ 
   if (uiRes == D_RTOSAL_SUCCESS && xHigherPriorityTaskWoken == pdTRUE)
   {
      rtosalContextSwitchIndicationSet();
   }
#elif D_USE_THREADX
   uiRes = tx_semaphore_put((TX_SEMAPHORE*)pRtosalSemaphoreCb->cMsgQueueCB);
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}
