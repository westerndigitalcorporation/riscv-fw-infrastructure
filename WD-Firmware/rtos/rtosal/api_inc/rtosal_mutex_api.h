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
* @file   rtosal_mutex_api.h
* @author Ronen Haen
* @date   07.02.2019
* @brief  The file defines the RTOS AL mutex interfaces
*/
#ifndef __RTOSAL_MUTEX_API_H__
#define __RTOSAL_MUTEX_API_H__

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
   #define M_MUTEX_CB_SIZE_IN_BYTES       sizeof(StaticSemaphore_t)
#elif D_USE_THREADX
   #define M_MUTEX_CB_SIZE_IN_BYTES       sizeof(TBD) // size of the CB struct 
#endif /* #ifdef D_USE_FREERTOS */

/**
* types
*/
/* mutex */
typedef struct rtosalMutex
{
#ifdef D_USE_FREERTOS
   void* mutexHandle;
#endif /* #ifdef D_USE_FREERTOS */
   s08_t cMutexCB[M_MUTEX_CB_SIZE_IN_BYTES];
} rtosalMutex_t;

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
* Mutex creation function
*/
u32_t rtosalMutexCreate(rtosalMutex_t* pRtosalMutexCb, s08_t* pRtosalMutexName, u32_t uiPriorityInherit);

/**
* Destroys a mutex
*/
u32_t rtosalMutexDestroy(rtosalMutex_t* pRtosalMutexCb);

/**
* Obtain ownership of a mutex
*/
u32_t rtosalMutexWait(rtosalMutex_t* pRtosalMutexCb, u32_t uiWaitTimeoutTicks);

/**
* Release ownership of a mutex
*/
u32_t rtosalMutexRelease(rtosalMutex_t* pRtosalMutexCb);

#endif /* __RTOSAL_MUTEX_API_H__ */
