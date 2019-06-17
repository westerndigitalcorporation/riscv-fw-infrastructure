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
* @file   rtosal_time.c
* @author Ronen Haen
* @date   21.01.2019 
* @brief  The file implements the RTOS AL timeer API
* 
*/

/**
* include files
*/
#include "rtosal_time_api.h"
#include "rtosal_macro.h"
#include "rtosal.h"
#include "psp_api.h"
#ifdef D_USE_FREERTOS
   #include "timers.h"
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
* @brief Timer creation function
*
* @param pRtosalTimerCb    - Pointer to the timer control block to be created
* @param pTimerName        - String of the timer name (for debuging)
* @param fptrTimerCallcabk - Function handler for timer expiration processing
* @param pTimeParam        - A parameter to pass to the timer expiration function
*                            handler
* @param uiAutoActivate    - Auto start timer after creation, Usage: 
*                            D_RTOSAL_AUTO_START timer shall be activated when created
*                            D_RTOSAL_NO_ACTIVATE dont start timer on creation 
*                            created; otherwise D_RTOSAL_NO_ACTIVATE
* @param uiTicks           - Timer expiration period
* @param uiRescheduleTicks - Timer period after first expiration; used for
*                            periodic timer, meaning after expiration of uiTicks,
*                            how many ticks to wait before activating again 
*
* @return u32_t            - D_RTOSAL_SUCCESS
*                          - D_RTOSAL_TIMER_ERROR - The ptr, cTaskCB, in the pRtosalTimerCb is invalid
*                                                   or fptrTimerCallcabk is inavlid
*                          - D_RTOSAL_TICK_ERROR - Invalid uiTicks
*                          - D_RTOSAL_ACTIVATE_ERROR - Invalid uiAutoActivate
*                          - D_RTOSAL_CALLER_ERROR - The caller can not call this function
*/
RTOSAL_SECTION u32_t rtosTimerCreate(rtosalTimer_t* pRtosalTimerCb, s08_t *pRtosTimerName,
                     rtosalTimerHandler_t fptrRtosTimerCallcabk,
							u32_t uiTimeCallbackParam, u32_t uiAutoActivate,
							u32_t uiTicks, u32_t uiRescheduleTicks)
{
   u32_t uiRes;
#ifdef D_USE_FREERTOS
   UBaseType_t uiAutoReload;
#endif /* #ifdef D_USE_FREERTOS */

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalTimerCb, pRtosalTimerCb == NULL, D_RTOSAL_TIMER_ERROR);

#ifdef D_USE_FREERTOS
   /* for one time timer */
   uiAutoReload = (uiRescheduleTicks != 0) ? (pdTRUE) : (pdFALSE);
   pRtosalTimerCb->timerHandle = xTimerCreateStatic((const char *)pRtosTimerName, uiTicks, uiAutoReload,
                                     (void*)uiTimeCallbackParam, (TimerCallbackFunction_t)fptrRtosTimerCallcabk,
                                     (StaticTimer_t*)pRtosalTimerCb->cTimerCB);
   /* failed to create the timer */
   if (pRtosalTimerCb->timerHandle == NULL)
   {
      uiRes = D_RTOSAL_TIMER_ERROR;
   }
   /* do we need to activate it now */
   else if (uiAutoActivate == D_RTOSAL_AUTO_START)
   {
      uiRes = xTimerStart(pRtosalTimerCb->timerHandle, 0);
      if (uiRes == pdPASS)
      {
         uiRes = D_RTOSAL_SUCCESS;
      }
      /* failed to activate the timer */
      else
      {
         uiRes = D_RTOSAL_ACTIVATE_ERROR;
      }
   }
   else
   {
      uiRes = D_RTOSAL_SUCCESS;
   }
#elif D_USE_THREADX
   // TODO:
   //uiRes = add a call to ThreadX timer create API
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* @brief Destroy a timer
*
* @param pRtosalTimerCb    - Pointer to the timer control block to be destroyed
*
* @return u32_t            - D_RTOSAL_SUCCESS
*                          - D_RTOSAL_TIMER_ERROR - The ptr, cTaskCB, in the pRtosalTimerCb is invalid
*                          - D_RTOSAL_CALLER_ERROR - The caller can not call this function
*/
RTOSAL_SECTION u32_t rtosTimerDestroy(rtosalTimer_t* pRtosalTimerCb)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalTimerCb, pRtosalTimerCb == NULL, D_RTOSAL_TIMER_ERROR);

#ifdef D_USE_FREERTOS
   uiRes = xTimerDelete(pRtosalTimerCb->timerHandle, 0);
   if (uiRes == pdPASS)
   {
      uiRes = D_RTOSAL_SUCCESS;
   }
   else
   {
      uiRes = D_RTOSAL_TIMER_ERROR;
   }
#elif D_USE_THREADX
   // TODO:
   //uiRes = add a call to ThreadX timer delete API
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* @brief Start a timer
*
* @param pRtosalTimerCb    - Pointer to the timer control block to be started
*
* @return u32_t            - D_RTOSAL_SUCCESS
*                          - D_RTOSAL_TIMER_ERROR - The ptr, cTaskCB, in the pRtosalTimerCb is invalid
*                          - D_RTOSAL_ACTIVATE_ERROR - Timer is already running or already expried
*                          - D_RTOSAL_FAIL - Timers was not activated, request to active was rejected
*/
RTOSAL_SECTION u32_t rtosTimerStart(rtosalTimer_t* pRtosalTimerCb)
{
   u32_t uiRes;
#ifdef D_USE_FREERTOS
   /* specify if a context switch is needed as a uiResult calling FreeRTOS ...ISR function */
   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#endif /* #ifdef D_USE_FREERTOS */

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalTimerCb, pRtosalTimerCb == NULL, D_RTOSAL_TIMER_ERROR);

#ifdef D_USE_FREERTOS
   /* rtosTimerStart invoked from an ISR context */
   if (pspIsInterruptContext() == D_PSP_INT_CONTEXT)
   {
      uiRes = xTimerStartFromISR(pRtosalTimerCb->timerHandle, &xHigherPriorityTaskWoken);
   }
   else
   {
      uiRes = xTimerStart(pRtosalTimerCb->timerHandle, 0);
   }
   /* align the return code */
   if (uiRes == pdPASS)
   {
      uiRes = D_RTOSAL_SUCCESS;
   }
   else
   {
      /* The message could not be sent */
      uiRes = D_RTOSAL_ACTIVATE_ERROR;
   }
   /* due to the start we got an indicating that a context switch
      should be requested before the interrupt exits */
   if (uiRes == D_RTOSAL_SUCCESS && xHigherPriorityTaskWoken == pdTRUE)
   {
      rtosalContextSwitchIndicationSet();
   }
#elif D_USE_THREADX
   // TODO:
   //uiRes = add a call to ThreadX timer start API
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* @brief Stop a timer
*
* @param pRtosalTimerCb    - Pointer to the timer control block to be started
*
* @return u32_t            - D_RTOSAL_SUCCESS
*                          - D_RTOSAL_TIMER_ERROR - The ptr, cTaskCB, in the pRtosalTimerCb is invalid
*                          - D_RTOSAL_FAIL - Timers was not Stopped, request to stop was rejected
*/
RTOSAL_SECTION u32_t rtosTimerStop(rtosalTimer_t* pRtosalTimerCb)
{
   u32_t uiRes;
#ifdef D_USE_FREERTOS
   /* specify if a context switch is needed as a uiResult calling FreeRTOS ...ISR function */
   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#endif /* #ifdef D_USE_FREERTOS */

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalTimerCb, pRtosalTimerCb == NULL, D_RTOSAL_TIMER_ERROR);

#ifdef D_USE_FREERTOS
   /* rtosTimerStop invoked from an ISR context */
   if (pspIsInterruptContext() == D_PSP_INT_CONTEXT)
   {
      uiRes = xTimerStopFromISR(pRtosalTimerCb->timerHandle, &xHigherPriorityTaskWoken);
   }
   else
   {
      uiRes = xTimerStop(pRtosalTimerCb->timerHandle, 0);
   }
   /* align the return code */
   if (uiRes == pdPASS)
   {
      uiRes = D_RTOSAL_SUCCESS;
   }
   else
   {
      /* The message could not be sent */
      uiRes = D_RTOSAL_FAIL;
   }
   /* due to the stop we got an indicating that a context switch
      should be requested before the interrupt exits */
   if (uiRes == D_RTOSAL_SUCCESS && xHigherPriorityTaskWoken == pdTRUE)
   {
      rtosalContextSwitchIndicationSet();
   }
#elif D_USE_THREADX
   // TODO:
   //uiRes = add a call to ThreadX timer stop API
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* @brief Modify the timer expiration value
*
* @param pRtosalTimerCb    - Pointer to the timer control block to be modified
* @param uiTicks           - Timer expiration period
* @param uiRescheduleTicks - Timer period after first expiration; used for
*                            periodic timer, meaning after expiration of uiTicks,
*                            how many ticks to wait before activating again
*
* @return u32_t            - D_RTOSAL_SUCCESS
*                          - D_RTOSAL_TIMER_ERROR - The ptr, cTaskCB, in the pRtosalTimerCb is invalid
*                          - D_RTOSAL_TICK_ERROR - Invalid uiTicks
*                          - D_RTOSAL_CALLER_ERROR - The caller can not call this function
*                          - D_RTOSAL_ACTIVATE_ERROR - Timer is already running or already expried
*/
RTOSAL_SECTION u32_t rtosTimerModifyPeriod(rtosalTimer_t* pRtosalTimerCb, u32_t uiTicks, u32_t uiRescheduleTicks)
{
   u32_t uiRes;
#ifdef D_USE_FREERTOS
   /* specify if a context switch is needed as a uiResult calling FreeRTOS ...ISR function */
   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#elif D_USE_THREADX
   UINT uiActive;
#endif /* #ifdef D_USE_FREERTOS */

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalTimerCb, pRtosalTimerCb == NULL, D_RTOSAL_TIMER_ERROR);

#ifdef D_USE_FREERTOS
   /* rtosTimerModifyPeriod invoked from an ISR context */
   if (pspIsInterruptContext() == D_PSP_INT_CONTEXT)
   {
      uiRes = xTimerChangePeriodFromISR(pRtosalTimerCb->timerHandle, uiTicks, &xHigherPriorityTaskWoken);
   }
   else
   {
      uiRes = xTimerChangePeriod(pRtosalTimerCb->timerHandle, uiTicks, 0);
   }
   /* align the return code */
   if (uiRes == pdPASS)
   {
      uiRes = D_RTOSAL_SUCCESS;
   }
   else
   {
      /* The message could not be sent */
      uiRes = D_RTOSAL_FAIL;
   }
   /* due to the period change we got an indicating that a context switch
      should be requested before the interrupt exits */
   if (uiRes == D_RTOSAL_SUCCESS && xHigherPriorityTaskWoken == pdTRUE)
   {
      rtosalContextSwitchIndicationSet();
   }
#elif D_USE_THREADX
   /* get timer info */
   // TODO:
   //uiRes = add a call to ThreadX timer get period API
   if (uiRes == D_RTOSAL_SUCCESS)
   {
      /* check if the timer is active - we need to stop it */
      if (uiActive == TX_TRUE)
      {
         //uiRes = add a call to ThreadX timer stop API
      }
      /* deactivate was successful */
      if (uiRes == D_RTOSAL_SUCCESS)
      {
         /* now we are sure the timer is inactive, we can change it */
         //uiRes = add a call to ThreadX timer change period API
         /* check if the timer was active - we need to restart it */
         if (uiRes == D_RTOSAL_SUCCESS && uiActive == TX_TRUE)
         {
            //uiRes = add a call to ThreadX timer start API
         }
      }
   }
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}
