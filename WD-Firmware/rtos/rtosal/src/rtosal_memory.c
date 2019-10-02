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
* @file   rtosal_memory.c
* @author Ronen Haen
* @date   21.01.2019 
* @brief  The file implements the RTOS AL memory API
* 
*/

/**
* include files
*/
#include "rtosal_api.h"
#include "rtosal.h"

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
*
*
* @param 
*
* @return u32_t            - D_RTOSAL_SUCCESS
*                          - D_RTOSAL_DELETED
*/
u32_t rtosalMsgQueueRecieve (rtosalMsgQueue_t* pRtosalMsgQueueCb, void* pRtosalMsgQueueItem,
                             u32_t uiWaitTimeoutTicks)
{
#ifdef D_USE_FREERTOS

#elif D_USE_THREADX

#else
   #error "Add appropriate RTOS definitions"
#endif
}
