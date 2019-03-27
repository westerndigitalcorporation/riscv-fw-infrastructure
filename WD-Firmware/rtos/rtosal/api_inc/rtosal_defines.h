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
   #define D_RTOSAL_SUSPEND_LIFTED               0x1A
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
   #define D_RTOSAL_SUCCESS                      TX_SUCCESS
   #define D_RTOSAL_DELETED                      TX_DELETED
   #define D_RTOSAL_NO_MEMORY                    TX_NO_MEMORY
   #define D_RTOSAL_POOL_ERROR                   TX_POOL_ERROR
   #define D_RTOSAL_PTR_ERROR                    TX_PTR_ERROR
   #define D_RTOSAL_WAIT_ERROR                   TX_WAIT_ERROR
   #define D_RTOSAL_SIZE_ERROR                   TX_SIZE_ERROR
   #define D_RTOSAL_GROUP_ERROR                  TX_GROUP_ERROR
   #define D_RTOSAL_NO_EVENTS                    TX_NO_EVENTS
   #define D_RTOSAL_OPTION_ERROR                 TX_OPTION_ERROR
   #define D_RTOSAL_QUEUE_ERROR                  TX_QUEUE_ERROR
   #define D_RTOSAL_QUEUE_EMPTY                  TX_QUEUE_EMPTY
   #define D_RTOSAL_QUEUE_FULL                   TX_QUEUE_FULL
   #define D_RTOSAL_SEMAPHORE_ERROR              TX_SEMAPHORE_ERROR
   #define D_RTOSAL_NO_INSTANCE                  TX_NO_INSTANCE
   #define D_RTOSAL_TASK_ERROR                   TX_THREAD_ERROR
   #define D_RTOSAL_PRIORITY_ERROR               TX_PRIORITY_ERROR
   #define D_RTOSAL_START_ERROR                  TX_START_ERROR
   #define D_RTOSAL_DELETE_ERROR                 TX_DELETE_ERROR
   #define D_RTOSAL_RESUME_ERROR                 TX_RESUME_ERROR
   #define D_RTOSAL_CALLER_ERROR                 TX_CALLER_ERROR
   #define D_RTOSAL_SUSPEND_ERROR                TX_SUSPEND_ERROR
   #define D_RTOSAL_TIMER_ERROR                  TX_TIMER_ERROR
   #define D_RTOSAL_TICK_ERROR                   TX_TICK_ERROR
   #define D_RTOSAL_ACTIVATE_ERROR               TX_ACTIVATE_ERROR
   #define D_RTOSAL_THRESH_ERROR                 TX_THRESH_ERROR
   #define D_RTOSAL_SUSPEND_LIFTED               TX_SUSPEND_LIFTED
   #define D_RTOSAL_WAIT_ABORTED                 TX_WAIT_ABORTED
   #define D_RTOSAL_WAIT_ABORT_ERROR             TX_WAIT_ABORT_ERROR
   #define D_RTOSAL_MUTEX_ERROR                  TX_MUTEX_ERROR
   #define D_RTOSAL_NOT_AVAILABLE                TX_NOT_AVAILABLE
   #define D_RTOSAL_NOT_OWNED                    TX_NOT_OWNED
   #define D_RTOSAL_INHERIT_ERROR                TX_INHERIT_ERROR
   #define D_RTOSAL_NOT_DONE                     TX_NOT_DONE
   #define D_RTOSAL_CEILING_EXCEEDED             TX_CEILING_EXCEEDED
   #define D_RTOSAL_INVALID_CEILING              TX_INVALID_CEILING
   #define D_RTOSAL_FAIL                         TX_FEATURE_NOT_ENABLED-1
   #define D_RTOSAL_FEATURE_NOT_ENABLED          TX_FEATURE_NOT_ENABLED
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
   #define D_RTOSAL_OR                           TX_OR
   #define D_RTOSAL_OR_CLEAR                     TX_OR_CLEAR
   #define D_RTOSAL_AND                          TX_AND
   #define D_RTOSAL_AND_CLEAR                    TX_AND_CLEAR
   #define D_RTOSAL_INHERIT                      TX_INHERIT
   #define D_RTOSAL_NO_INHERIT                   TX_NO_INHERIT
   #define D_RTOSAL_NO_WAIT                      TX_NO_WAIT
   #define D_RTOSAL_WAIT_FOREVER                 TX_WAIT_FOREVER
   #define D_RTOSAL_TRUE                         TX_TRUE
   #define D_RTOSAL_FALSE                        TX_FALSE
   #define D_RTOSAL_DONT_START                   TX_DONT_START
   #define D_RTOSAL_AUTO_START                   TX_AUTO_ACTIVATE
   #define D_RTOSAL_NO_TIME_SLICE                TX_NO_TIME_SLICE
#endif /* #ifdef D_USE_FREERTOS */

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

#endif /* __RTOSAL_DEFINES_H__ */
