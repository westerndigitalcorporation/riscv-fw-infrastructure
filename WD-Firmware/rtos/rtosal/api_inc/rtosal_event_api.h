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
* @file   rtosal_event_api.h
* @author Ronen Haen
* @date   07.02.2019
* @brief  The file defines the RTOS AL event interfaces
*/
#ifndef __RTOSAL_EVENT_API_H__
#define __RTOSAL_EVENT_API_H__

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
/* define control block size */
#ifdef D_USE_FREERTOS
   #define M_EVENT_GROUP_CB_SIZE_IN_BYTES sizeof(StaticEventGroup_t)
#elif D_USE_THREADX
   #define M_EVENT_GROUP_CB_SIZE_IN_BYTES sizeof(TBD)  // size of the CB struct 
#endif /* #ifdef D_USE_FREERTOS */

/**
* types
*/
/* event group */
typedef struct rtosalEventGroup
{
#ifdef D_USE_FREERTOS
   void* eventGroupHandle;
#endif /* #ifdef D_USE_FREERTOS */
   s08_t cEventGroupCB[M_EVENT_GROUP_CB_SIZE_IN_BYTES];
} rtosalEventGroup_t;

/* event group bits */
typedef u32_t rtosalEventBits_t;

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
* Create an event group
*/
u32_t rtosalEventGroupCreate(rtosalEventGroup_t* pRtosalEventGroupCb, s08_t* pRtosalEventGroupName);

/**
* Destroy a specific event group
*/
u32_t rtosalEventGroupDestroy(rtosalEventGroup_t* pRtosalEventGroupCb);

/**
* Set the event bits of a specific event group
*/
u32_t rtosalEventGroupSet(rtosalEventGroup_t* pRtosalEventGroupCb,
                          rtosalEventBits_t stSetRtosalEventBits,
                          u32_t uiSetOption, rtosalEventBits_t* pRtosalEventBits);

/**
* Retrieve the event bits of a specific event group
*/
u32_t rtosalEventGroupGet(rtosalEventGroup_t* pRtosalEventGroupCb, u32_t uiRetrieveEvents,
                          rtosalEventBits_t* pRtosalEventBits, u32_t uiRetrieveOption,
                          u32_t uiWaitTimeoutTicks);

#endif /* __RTOSAL_EVENT_API_H__ */
