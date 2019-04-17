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
* @file   rtosal_time_api.h
* @author Ronen Haen
* @date   21.01.2019 
* @brief  The file defines the RTOS AL time interface
*/
#ifndef  __RTOSAL_TIME_API_H__
#define  __RTOSAL_TIME_API_H__

/**
* include files
*/
#include "rtosal_config.h"
#include "rtosal_defines.h"
#include "rtosal_types.h"

/**
* definitions
*/

/**
* macros
*/
#ifdef D_USE_FREERTOS
   #define M_TIMER_CB_SIZE_IN_BYTES       sizeof(StaticTimer_t)
#elif D_USE_THREADX
   #define M_TIMER_CB_SIZE_IN_BYTES       sizeof(TBD)
#endif /* #ifdef D_USE_FREERTOS */

/**
* types
*/
#ifdef D_USE_FREERTOS
   typedef void* timerHandlerParam_t;
#elif D_USE_THREADX
#error *** TODO: need to define the TBD ***
   typedef TBD   timerHandlerParam_t;
#endif /* #ifdef D_USE_FREERTOS */

/* timer */
typedef struct rtosalTimer
{
#ifdef D_USE_FREERTOS
   void* timerHandle;
#endif /* #ifdef D_USE_FREERTOS */
   s08_t cTimerCB[M_TIMER_CB_SIZE_IN_BYTES];
}rtosalTimer_t;

/* timer handler definition */
typedef void (*rtosalTimerHandler_t)(timerHandlerParam_t);

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
* APIs
*/

/**
* @brief Timer creation function
*/
u32_t rtosTimerCreate(rtosalTimer_t* pRtosalTimerCb, s08_t *pRtosTimerName,
                     rtosalTimerHandler_t fptrRtosTimerCallcabk,
                     u32_t uTimeCallbackParam, u32_t uiAutoActivate,
                     u32_t uiTicks, u32_t uiRescheduleTicks);

/**
* @brief Destroy a timer
*/
u32_t rtosTimerDestroy(rtosalTimer_t* pRtosalTimerCb);

/**
* @brief Start a timer
*/
u32_t rtosTimerStart(rtosalTimer_t* pRtosalTimerCb);

/**
* @brief Stop a timer
*/
u32_t rtosTimerStop(rtosalTimer_t* pRtosalTimerCb);

/**
* @brief Modify the timer expiration value
*/
u32_t rtosTimerModifyPeriod(rtosalTimer_t* pRtosalTimerCb, u32_t uiTicks, u32_t uiRescheduleTicks);

#endif /* __RTOSAL_TIME_API_H__ */
