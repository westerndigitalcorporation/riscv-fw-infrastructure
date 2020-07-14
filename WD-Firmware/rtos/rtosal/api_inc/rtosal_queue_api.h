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
* @file   rtosal_queue_api.h
* @author Ronen Haen
* @date   07.02.2019
* @brief  The file defines the RTOS AL queue interfaces
*/
#ifndef __RTOSAL_QUEUE_API_H__
#define __RTOSAL_QUEUE_API_H__

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
   #define M_MSG_QUEUE_CB_SIZE_IN_BYTES   sizeof(StaticQueue_t)
#elif D_USE_THREADX
   #define M_MSG_QUEUE_CB_SIZE_IN_BYTES   sizeof(TBD)  // size of the CB struct 
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

/**
* types
*/
/* message queue */
typedef struct rtosalMsgQueue
{
   s08_t cMsgQueueCB[M_MSG_QUEUE_CB_SIZE_IN_BYTES];
} rtosalMsgQueue_t;

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
* Create a message queue
*/
u32_t rtosalMsgQueueCreate(rtosalMsgQueue_t* pRtosalMsgQueueCb, void* pRtosMsgQueueBuffer,
                           u32_t uiRtosMsgQueueSize, u32_t uiRtosMsgQueueItemSize,
                           s08_t* pRtosalMsgQueueName);
/**
* Destroy a queue
*/
u32_t rtosalMsgQueueDestroy(rtosalMsgQueue_t* pRtosalMsgQueueCb);

/**
* Add an item to the queue front/back
*/
u32_t rtosalMsgQueueSend(rtosalMsgQueue_t* pRtosalMsgQueueCb, const void* pRtosalMsgQueueItem,
                       u32_t uiWaitTimeoutTicks, u32_t uiSendToFront);

/**
* Retrieve an item from the queue. The message to retrieve shall be copied to a
* user provided buffer and shall be deleted from the message queue
*/
u32_t rtosalMsgQueueRecieve(rtosalMsgQueue_t* pRtosalMsgQueueCb, void* pRtosalMsgQueueItem,
                            u32_t uiWaitTimeoutTicks);

#endif /* __RTOSAL_QUEUE_API_H__ */
