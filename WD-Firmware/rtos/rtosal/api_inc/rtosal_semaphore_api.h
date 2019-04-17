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
* @file   rtosal_semaphore_api.h
* @author Ronen Haen
* @date   07.02.2019
* @brief  The file defines the RTOS AL semaphore interfaces
*/
#ifndef __RTOSAL_SEMAPHORE_API_H__
#define __RTOSAL_SEMAPHORE_API_H__

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
   #define M_SEMAPHORE_CB_SIZE_IN_BYTES   sizeof(StaticSemaphore_t)
#elif D_USE_THREADX
   #define M_SEMAPHORE_CB_SIZE_IN_BYTES   sizeof(TBD)
#endif /* #ifdef D_USE_FREERTOS */

/**
* types
*/
/* semaphore */
typedef struct rtosalSemaphore
{
#ifdef D_USE_FREERTOS
   void* semaphoreHandle;
#endif /* #ifdef D_USE_FREERTOS */
   s08_t cSemaphoreCB[M_SEMAPHORE_CB_SIZE_IN_BYTES];
} rtosalSemaphore_t;

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
* Create a semaphore
*/
u32_t rtosalSemaphoreCreate(rtosalSemaphore_t* pRtosalSemaphoreCb, s08_t *pRtosalSemaphoreName,
                            s32_t iSemaphoreInitialCount, u32_t uiSemaphoreMaxCount);

/**
* Delete a semaphore
*/
u32_t rtosalSemaphoreDestroy(rtosalSemaphore_t* pRtosalSemaphoreCb);

/**
* Wait for a semaphore to become available
*/
u32_t rtosalSemaphoreWait(rtosalSemaphore_t* pRtosalSemaphoreCb, u32_t uiWaitTimeoutTicks);

/**
* Release a semaphore
*/
u32_t rtosalSemaphoreRelease(rtosalSemaphore_t* pRtosalSemaphoreCb);

#endif /* __RTOSAL_SEMAPHORE_API_H__ */
