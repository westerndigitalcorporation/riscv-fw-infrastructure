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
* @file   rtosal.h
* @author Ronen Haen
* @date   21.01.2019 
* @brief  The defines rtos-al private interfaces 
* 
*/
#ifndef  __RTOSAL_H__
#define  __RTOSAL_H__

/**
* include files
*/

/**
* definitions
*/
/* interrupt context indication values */
#define D_RTOSAL_NON_INT_CONTEXT              0
#define D_RTOSAL_INT_CONTEXT                  1

/* define task stack registers offset */
#ifdef D_USE_FREERTOS
   #define D_RTOSAL_NUM_OF_STACK_ENTRIES              32
   #define D_RTOSAL_MEPC_INDEX_ON_TASK_STACK          0
   #define D_RTOSAL_SP_INDEX_ON_TASK_STACK            2
   #define D_RTOSAL_RA_INDEX_ON_TASK_STACK            3
   #define D_RTOSAL_S0_INDEX_ON_TASK_STACK            7
   #define D_RTOSAL_S1_INDEX_ON_TASK_STACK            8
   #define D_RTOSAL_A0_INDEX_ON_TASK_STACK            9
   #define D_RTOSAL_A1_INDEX_ON_TASK_STACK            10
   #define D_RTOSAL_A2_INDEX_ON_TASK_STACK            11
   #define D_RTOSAL_A3_INDEX_ON_TASK_STACK            12
   #define D_RTOSAL_A4_INDEX_ON_TASK_STACK            13
   #define D_RTOSAL_A5_INDEX_ON_TASK_STACK            14
   #define D_RTOSAL_A6_INDEX_ON_TASK_STACK            15
   #define D_RTOSAL_A7_INDEX_ON_TASK_STACK            16
   #define D_RTOSAL_S2_INDEX_ON_TASK_STACK            17
   #define D_RTOSAL_S3_INDEX_ON_TASK_STACK            18
   #define D_RTOSAL_S4_INDEX_ON_TASK_STACK            19
   #define D_RTOSAL_S5_INDEX_ON_TASK_STACK            20
   #define D_RTOSAL_S6_INDEX_ON_TASK_STACK            21
   #define D_RTOSAL_S7_INDEX_ON_TASK_STACK            22
   #define D_RTOSAL_S8_INDEX_ON_TASK_STACK            23
   #define D_RTOSAL_S9_INDEX_ON_TASK_STACK            24
   #define D_RTOSAL_S10_INDEX_ON_TASK_STACK           25
   #define D_RTOSAL_S11_INDEX_ON_TASK_STACK           26
   #define D_RTOSAL_T3_INDEX_ON_TASK_STACK            27
#ifdef D_COMRV
   #define D_RTOSAL_TP_INDEX_ON_TASK_STACK            28
#endif /* D_COMRV */
   #define D_RTOSAL_T5_INDEX_ON_TASK_STACK            29
#elif D_USE_THREADX
   #error "Add THREADX appropriate definitions"
#else
   #error "RTOS is not defined"
#endif /* D_USE_FREERTOS */
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
extern void (*fptrParamErrorNotification)(const void *pParam, u32_t uErrorCode);

/**
* global variables
*/


/**
 * API
 */

/**
* @brief Activated upon Timer-tick (and invokes context-switch in the OS)
*
* @param None
*
*/
void rtosalTick(void);

#endif /* __RTOSAL_H__ */
