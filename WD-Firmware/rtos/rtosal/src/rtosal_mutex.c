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
* @file   rtosal_mutex.c
* @author Ronen Haen
* @date   21.01.2019 
* @brief  The file implements the RTOS AL mutex API
* 
*/

/**
* include files
*/
#include "rtosal_mutex_api.h"
#include "rtosal_macros.h"
#include "rtosal.h"
#ifdef D_USE_FREERTOS
   #include "semphr.h"
#else
   #error "Add appropriate RTOS definitions"
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
* Mutex creation function
*
* @param  pRtosalMutexCb    - pointer to the mutex control block to be created
* @param  pRtosalMutexName  - string of the mutex name (for debuging)
* @param  uiPriorityInherit - this mutex can be inherit or not
*                             values: D_RTOSAL_INHERIT or D_RTOSAL_NO_INHERIT
*
* @return u32_t          - D_RTOSAL_SUCCESS
*                        - D_RTOSAL_MUTEX_ERROR - the group pRtosalMutexCb is invalid or been used
*                        - D_RTOSAL_CALLER_ERROR - the caller can not call this function 
*                        - D_RTOSAL_INHERIT_ERROR - bad parm on uiPriorityInherit
*/
RTOSAL_SECTION u32_t rtosalMutexCreate(rtosalMutex_t* pRtosalMutexCb, s08_t* pRtosalMutexName, u32_t uiPriorityInherit)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalMutexCb, pRtosalMutexCb == NULL, D_RTOSAL_MUTEX_ERROR);

#ifdef D_USE_FREERTOS
   /* D_RTOSAL_NO_INHERIT isn't supported by FreeRTOS */
   M_RTOSAL_VALIDATE_FUNC_PARAM(uiPriorityInherit, uiPriorityInherit == D_RTOSAL_NO_INHERIT, D_RTOSAL_INHERIT_ERROR);
   /* create the mutex */
   pRtosalMutexCb->mutexHandle = xSemaphoreCreateMutexStatic((StaticSemaphore_t*)pRtosalMutexCb->cMutexCB);
   if (pRtosalMutexCb->mutexHandle != NULL)
   {
      uiRes = D_RTOSAL_SUCCESS;
      /* assign a name to the created mutex */
      vQueueAddToRegistry((QueueHandle_t)pRtosalMutexCb->mutexHandle, (const char*)pRtosalMutexName);
   }
   else
   {
      uiRes = D_RTOSAL_MUTEX_ERROR;
   }
#elif D_USE_THREADX
   #error "Add THREADX appropriate definitions"
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Destroys a mutex
*
* @param  pRtosalMutexCb - pointer to the mutex control block to be deleted
*
* @return u32_t      - D_RTOSAL_SUCCESS
*                    - D_RTOSAL_MUTEX_ERROR - the group pRtosalMutexCb is invalid or been used
*                    - D_RTOSAL_CALLER_ERROR - the caller can not call this function
*/
RTOSAL_SECTION u32_t rtosalMutexDestroy(rtosalMutex_t* pRtosalMutexCb)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalMutexCb, pRtosalMutexCb == NULL, D_RTOSAL_MUTEX_ERROR);

#ifdef D_USE_FREERTOS
   vSemaphoreDelete(pRtosalMutexCb->mutexHandle);
   uiRes = D_RTOSAL_SUCCESS;
#elif D_USE_THREADX
   #error "Add THREADX appropriate definitions"
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Obtain ownership of a mutex
*
* @param  pRtosalMutexCb     - pointer to the mutex control block to be locked
* @param  uiWaitTimeoutTicks - define how many ticks to wait in case the mutex
*                              is not available.
*                              Parms: D_RTOSAL_NO_WAIT, D_RTOSAL_WAIT_FOREVER, 32bit timer ticks value
*
* @return u32_t           - D_RTOSAL_SUCCESS
*                         - D_RTOSAL_DELETED - the mutex was deleted when this api was called
*                         - D_RTOSAL_NOT_AVAILABLE - time out while waiting for the mutex 
*                         - D_RTOSAL_WAIT_ABORTED - aborted by different consumer (like other thread)
*                         - D_RTOSAL_MUTEX_ERROR - the ptr in the mutex CB is invalid
*                         - D_RTOSAL_WAIT_ERROR - illegal use of wait (wait can be used only from thread)
*                         - D_RTOSAL_CALLER_ERROR - the caller can not call this function 
*/
RTOSAL_SECTION u32_t rtosalMutexWait(rtosalMutex_t* pRtosalMutexCb, u32_t uiWaitTimeoutTicks)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalMutexCb, pRtosalMutexCb == NULL, D_RTOSAL_MUTEX_ERROR);

#ifdef D_USE_FREERTOS
   uiRes = xSemaphoreTake(pRtosalMutexCb->mutexHandle, uiWaitTimeoutTicks);
   if (uiRes == pdPASS)
   {
      uiRes = D_RTOSAL_SUCCESS;
   }
   else
   {
      uiRes = D_RTOSAL_NOT_AVAILABLE;
   }
#elif D_USE_THREADX
   #error "Add THREADX appropriate definitions"
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Release ownership of a mutex
*
* @param  pRtosalMutexCb - pointer to the mutex control block to be unlocked
*
* @return u32_t      - D_RTOSAL_SUCCESS
*                    - D_RTOSAL_NOT_OWNED - mutex is already taken by different consumer
*                    - D_RTOSAL_MUTEX_ERROR - the ptr in the mutex CB is invalid
*                    - D_RTOSAL_CALLER_ERROR - the caller can not call this function 
*/
RTOSAL_SECTION u32_t rtosalMutexRelease(rtosalMutex_t* pRtosalMutexCb)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalMutexCb, pRtosalMutexCb == NULL, D_RTOSAL_MUTEX_ERROR);

#ifdef D_USE_FREERTOS
   uiRes = xSemaphoreGive((SemaphoreHandle_t)pRtosalMutexCb->mutexHandle);
   if (uiRes == pdPASS)
   {
      uiRes = D_RTOSAL_SUCCESS;
   }
   else
   {
      uiRes = D_RTOSAL_NOT_OWNED;
   }
#elif D_USE_THREADX
   #error "Add THREADX appropriate definitions"
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}
