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
* @file   rtosal_task_api.h
* @author Ronen Haen
* @date   07.02.2019
* @brief  The file defines the RTOS AL task interfaces
*/
#ifndef __RTOSAL_TASK_API_H__
#define __RTOSAL_TASK_API_H__

/**
* include files
*/
#include "rtosal_config.h"
#include "rtosal_defines.h"
#include "rtosal_types.h"
#include "rtosal_macro.h"
/**
* definitions
*/

/**
* macros
*/
#ifdef D_USE_FREERTOS
   #define M_TASK_CB_SIZE_IN_BYTES        sizeof(StaticTask_t)
#elif D_USE_THREADX
   #define M_TASK_CB_SIZE_IN_BYTES        sizeof(TBD)  // size of the CB struct 
#endif /* #ifdef D_USE_FREERTOS */

#ifdef D_USE_FREERTOS
   #define D_MAX_PRIORITY                 (configMAX_PRIORITIES-1)
#elif D_USE_THREADX
   #define D_MAX_PRIORITY                 (TBD)  // size of the CB struct 
#endif /* #ifdef D_USE_FREERTOS */

/**
* types
*/
#ifdef D_USE_FREERTOS
   typedef void* entryPointParam_t;
#elif D_USE_THREADX
#error *** TODO: need to define the TBD ***
   typedef TBD   entryPointParam_t;
#endif /* #ifdef D_USE_FREERTOS */

typedef enum rtosalPriority
{
#ifdef D_USE_FREERTOS
   E_RTOSAL_PRIO_0 = 31,  /* highest priority */
   E_RTOSAL_PRIO_1 = 30,
   E_RTOSAL_PRIO_2 = 29,
   E_RTOSAL_PRIO_3 = 28,
   E_RTOSAL_PRIO_4 = 27,
   E_RTOSAL_PRIO_5 = 26,
   E_RTOSAL_PRIO_6 = 25,
   E_RTOSAL_PRIO_7 = 24,
   E_RTOSAL_PRIO_8 = 23,
   E_RTOSAL_PRIO_9 = 22,
   E_RTOSAL_PRIO_10 = 21,
   E_RTOSAL_PRIO_11 = 20,
   E_RTOSAL_PRIO_12 = 19,
   E_RTOSAL_PRIO_13 = 18,
   E_RTOSAL_PRIO_14 = 17,
   E_RTOSAL_PRIO_15 = 16,
   E_RTOSAL_PRIO_16 = 15,
   E_RTOSAL_PRIO_17 = 14,
   E_RTOSAL_PRIO_18 = 13,
   E_RTOSAL_PRIO_19 = 12,
   E_RTOSAL_PRIO_20 = 11,
   E_RTOSAL_PRIO_21 = 10,
   E_RTOSAL_PRIO_22 = 9,
   E_RTOSAL_PRIO_23 = 8,
   E_RTOSAL_PRIO_24 = 7,
   E_RTOSAL_PRIO_25 = 6,
   E_RTOSAL_PRIO_26 = 5,
   E_RTOSAL_PRIO_27 = 4,
   E_RTOSAL_PRIO_28 = 3,
   E_RTOSAL_PRIO_29 = 2,
   E_RTOSAL_PRIO_30 = 1,
   E_RTOSAL_PRIO_31 = 0,
#elif D_USE_THREADX
   E_RTOSAL_PRIO_0 = 0,  /* highest priority */
   E_RTOSAL_PRIO_1 = 1,
   E_RTOSAL_PRIO_2 = 2,
   E_RTOSAL_PRIO_3 = 3,
   E_RTOSAL_PRIO_4 = 4,
   E_RTOSAL_PRIO_5 = 5,
   E_RTOSAL_PRIO_6 = 6,
   E_RTOSAL_PRIO_7 = 7,
   E_RTOSAL_PRIO_8 = 8,
   E_RTOSAL_PRIO_9 = 9,
   E_RTOSAL_PRIO_10 = 10,
   E_RTOSAL_PRIO_11 = 11,
   E_RTOSAL_PRIO_12 = 12,
   E_RTOSAL_PRIO_13 = 13,
   E_RTOSAL_PRIO_14 = 14,
   E_RTOSAL_PRIO_15 = 15,
   E_RTOSAL_PRIO_16 = 16,
   E_RTOSAL_PRIO_17 = 17,
   E_RTOSAL_PRIO_18 = 18,
   E_RTOSAL_PRIO_19 = 19,
   E_RTOSAL_PRIO_20 = 20,
   E_RTOSAL_PRIO_21 = 21,
   E_RTOSAL_PRIO_22 = 22,
   E_RTOSAL_PRIO_23 = 23,
   E_RTOSAL_PRIO_24 = 24,
   E_RTOSAL_PRIO_25 = 25,
   E_RTOSAL_PRIO_26 = 26,
   E_RTOSAL_PRIO_27 = 27,
   E_RTOSAL_PRIO_28 = 28,
   E_RTOSAL_PRIO_29 = 29,
   E_RTOSAL_PRIO_30 = 30,
   E_RTOSAL_PRIO_31 = 31,
#endif /* #ifdef D_USE_FREERTOS */
   E_RTOSAL_PRIO_MAX = E_RTOSAL_PRIO_31
} rtosalPriority_t;

typedef struct rtosalTask
{
#ifdef D_USE_FREERTOS
   void* taskHandle;
#endif /* #ifdef D_USE_FREERTOS */
   s08_t cTaskCB[M_TASK_CB_SIZE_IN_BYTES];
} rtosalTask_t;

/* task handler definition */
typedef void (*rtosalTaskHandler_t)(entryPointParam_t);

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
* Task creation function
*/
u32_t rtosalTaskCreate(rtosalTask_t* pRtosalTaskCb, const s08_t* pTaskName, rtosalPriority_t uiPriority,
                     rtosalTaskHandler_t fptrRtosTaskEntryPoint, u32_t uiTaskEntryPointParameter,
                     u32_t uiStackSize, void * pStackBuffer, u32_t uiTimeSliceTicks,
                     u32_t uiAutoStart, u32_t uiPreemptThuiReshold);

/**
* Delete a task
*/
u32_t rtosalTaskDestroy(rtosalTask_t* pRtosalTaskCb);

/**
* Change the priority of a specific task
*/
u32_t rtosalTaskPriorityChange(rtosalTask_t* pRtosalTaskCb, u32_t uiNewPriority,
                               u32_t *pOldPriority);

/**
* Voluntarily yield CPU time to another task
*/
void rtosalTaskYield(void);

/**
* Resume a suspended task
*/
u32_t rtosalTaskResume(rtosalTask_t* pRtosalTaskCb);

/**
* Suspend the execution of a specific task for a specific time
*/
u32_t rtosalTaskSleep(u32_t uiTimerTicks);

/**
* Suspend the execution of a specific task
*/
u32_t rtosalTaskSuspend(rtosalTask_t* pRtosalTaskCb);

/**
* Abort a task which is in currently blocked
*/
u32_t rtosalTaskWaitAbort(rtosalTask_t* pRtosalTaskCb);

#endif /* __RTOSAL_TASK_API_H__ */
