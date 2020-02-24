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
* @file   rtosal_defines.h
* @author Ronen Haen
* @date   21.01.2019 
* @brief  The defines rtos-al private interfaces 
* 
*/
#ifndef  __RTOSAL_DEFINES_H__
#define  __RTOSAL_DEFINES_H__

/**
* include files
*/

/**
* definitions
*/
/* function return codes */
#ifdef D_USE_FREERTOS
   #define D_RTOSAL_SUCCESS                      0x00
   #define D_RTOSAL_DELETED                      0x01
   #define D_RTOSAL_NO_MEMORY                    0x02
   #define D_RTOSAL_POOL_ERROR                   0x03
   #define D_RTOSAL_PTR_ERROR                    0x04
   #define D_RTOSAL_WAIT_ERROR                   0x05
   #define D_RTOSAL_SIZE_ERROR                   0x06
   #define D_RTOSAL_GROUP_ERROR                  0x07
   #define D_RTOSAL_NO_EVENTS                    0x08
   #define D_RTOSAL_OPTION_ERROR                 0x09
   #define D_RTOSAL_QUEUE_ERROR                  0x0A
   #define D_RTOSAL_QUEUE_EMPTY                  0x0B
   #define D_RTOSAL_QUEUE_FULL                   0x0C
   #define D_RTOSAL_SEMAPHORE_ERROR              0x0D
   #define D_RTOSAL_NO_INSTANCE                  0x0E
   #define D_RTOSAL_TASK_ERROR                   0x0F
   #define D_RTOSAL_PRIORITY_ERROR               0x10
   #define D_RTOSAL_START_ERROR                  0x11
   #define D_RTOSAL_DELETE_ERROR                 0x12
   #define D_RTOSAL_RESUME_ERROR                 0x13
   #define D_RTOSAL_CALLER_ERROR                 0x14
   #define D_RTOSAL_SUSPEND_ERROR                0x15
   #define D_RTOSAL_TIMER_ERROR                  0x16
   #define D_RTOSAL_TICK_ERROR                   0x17
   #define D_RTOSAL_ACTIVATE_ERROR               0x18
   #define D_RTOSAL_THRESH_ERROR                 0x19
   #define D_RTOSAL_SUSPEND_REMOVED              0x1A
   #define D_RTOSAL_WAIT_ABORTED                 0x1B
   #define D_RTOSAL_WAIT_ABORT_ERROR             0x1C
   #define D_RTOSAL_MUTEX_ERROR                  0x1D
   #define D_RTOSAL_NOT_AVAILABLE                0x1E
   #define D_RTOSAL_NOT_OWNED                    0x1F
   #define D_RTOSAL_INHERIT_ERROR                0x20
   #define D_RTOSAL_NOT_DONE                     0x21
   #define D_RTOSAL_CEILING_EXCEEDED             0x22
   #define D_RTOSAL_INVALID_CEILING              0x23
   #define D_RTOSAL_FAIL                         0xFE
   #define D_RTOSAL_FEATURE_NOT_ENABLED          0xFF
#elif D_USE_THREADX
   #define D_RTOSAL_SUCCESS                      TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_DELETED                      TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_NO_MEMORY                    TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_POOL_ERROR                   TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_PTR_ERROR                    TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_WAIT_ERROR                   TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_SIZE_ERROR                   TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_GROUP_ERROR                  TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_NO_EVENTS                    TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_OPTION_ERROR                 TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_QUEUE_ERROR                  TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_QUEUE_EMPTY                  TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_QUEUE_FULL                   TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_SEMAPHORE_ERROR              TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_NO_INSTANCE                  TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_TASK_ERROR                   TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_PRIORITY_ERROR               TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_START_ERROR                  TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_DELETE_ERROR                 TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_RESUME_ERROR                 TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_CALLER_ERROR                 TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_SUSPEND_ERROR                TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_TIMER_ERROR                  TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_TICK_ERROR                   TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_ACTIVATE_ERROR               TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_THRESH_ERROR                 TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_SUSPEND_REMOVED              TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_WAIT_ABORTED                 TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_WAIT_ABORT_ERROR             TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_MUTEX_ERROR                  TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_NOT_AVAILABLE                TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_NOT_OWNED                    TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_INHERIT_ERROR                TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_NOT_DONE                     TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_CEILING_EXCEEDED             TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_INVALID_CEILING              TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_FAIL                         TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_FEATURE_NOT_ENABLED          TBD_TAKE_VAL_FROM_TX
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

/* function input definitions */
#ifdef D_USE_FREERTOS
   #define D_RTOSAL_OR                           0
   #define D_RTOSAL_OR_CLEAR                     1
   #define D_RTOSAL_AND                          2
   #define D_RTOSAL_AND_CLEAR                    3
   #define D_RTOSAL_INHERIT                      1
   #define D_RTOSAL_NO_INHERIT                   0
   #define D_RTOSAL_NO_WAIT                      0
   #define D_RTOSAL_WAIT_FOREVER                 0xFFFFFFFF
   #define D_RTOSAL_TRUE                         1
   #define D_RTOSAL_FALSE                        0
   #define D_RTOSAL_DONT_START                   0
   #define D_RTOSAL_AUTO_START                   1
   #define D_RTOSAL_NO_TIME_SLICE                0
#elif D_USE_THREADX
   #define D_RTOSAL_OR                           TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_OR_CLEAR                     TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_AND                          TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_AND_CLEAR                    TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_INHERIT                      TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_NO_INHERIT                   TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_NO_WAIT                      TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_WAIT_FOREVER                 TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_TRUE                         TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_FALSE                        TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_DONT_START                   TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_AUTO_START                   TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_NO_TIME_SLICE                TBD_TAKE_VAL_FROM_TX
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

#ifdef D_USE_FREERTOS
   #define D_RTOSAL_SCHEDULER_NOT_STARTED        0
   #define D_RTOSAL_SCHEDULER_RUNNING            1
   #define D_RTOSAL_SCHEDULER_SUSPENDED          2
   #define D_RTOSAL_SCHEDULER_UNKNOWN_STATE      3
#elif D_USE_THREADX
   #define D_RTOSAL_SCHEDULER_NOT_STARTED        TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_SCHEDULER_RUNNING            TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_SCHEDULER_SUSPENDED          TBD_TAKE_VAL_FROM_TX
   #define D_RTOSAL_SCHEDULER_UNKNOWN_STATE      TBD_TAKE_VAL_FROM_TX
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

#endif /* __RTOSAL_DEFINES_H__ */
