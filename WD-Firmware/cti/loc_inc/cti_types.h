/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2020 Western Digital Corporation or its affiliates.
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
* @file   cti_types.h
* @author Ronen Haen
* @date   01.07.2020
* @brief  The file defines cti (comrv testing infrastructure) types
*/
#ifndef __CTI_TYPES_H
#define __CTI_TYPES_H

/*
 * INCLUDES
 */

/*
 * DEFINITIONS
 */
#define M_CTI_5_NOPS \
   asm volatile ("nop");  \
   asm volatile ("nop");  \
   asm volatile ("nop");  \
   asm volatile ("nop");  \
   asm volatile ("nop");  \

#define M_CTI_10_NOPS \
   M_CTI_5_NOPS \
   M_CTI_5_NOPS \

#define M_CTI_50_NOPS \
   M_CTI_10_NOPS \
   M_CTI_10_NOPS \
   M_CTI_10_NOPS \
   M_CTI_10_NOPS \
   M_CTI_10_NOPS \

#define M_CTI_100_NOPS \
   M_CTI_50_NOPS \
   M_CTI_50_NOPS \

#define M_CTI_512B_NOPS \
   M_CTI_100_NOPS \
   M_CTI_100_NOPS \
   M_CTI_50_NOPS  \
   M_CTI_5_NOPS   \
   asm volatile ("nop");

/*
* TYPEDEFS
*/
typedef enum
{
   E_CB_TEST_OVL_GROUP_SANITY                   = 0x0,
   E_CB_TEST_OVL_GROUP_WITH_SAME_SIZE           = 0x1,
   E_CB_TEST_OVL_GROUP_WITH_DIFFERENT_SIZE      = 0x2,
   E_CB_TEST_OVL_GROUP_MULTIGROUP               = 0x3,
   E_CB_TEST_OVL_GROUP_LOCK_UNLOCK              = 0x4,
   E_CB_TEST_OVL_GROUP_DEFRAG_OVERLAY_MEMORY    = 0x5,
   E_CB_TEST_OVL_GROUP_OVLA                     = 0x6,
   E_CB_TEST_OVL_OVL_CRC_CHECK                  = 0x7,
   E_CB_TEST_OVL_CRITICAL_SECTION               = 0x8,
   E_CB_TEST_OVL_THREAD_SAFE                    = 0x9,
   E_CB_TEST_OVL_NONE,
   E_CB_TEST_OVL_MAX = E_CB_TEST_OVL_NONE
}E_CB_TEST_OVL;

typedef u08_t BOOL;

/* struct that holds the thread function parameters */
typedef struct
{
   u32_t  uiParam1;
   u32_t  uiParam2;
   u32_t  uiParam3;
}S_FW_CB_PARAM, *S_FW_CB_PARAM_PTR;

/* control block */
typedef struct
{
   /* holds the current test bitmap */
   u32_t        uiTestCurrent;
   /* holds the error bitmap */
   E_TEST_ERROR eTestErrorBitmap;
   /* generic parameter */
   u32_t        uiTestSharedMemory;
   /* used to know whether or not the variable has already been initialized */
   u32_t        uiIsValidData;
   u32_t        uiTestSyncPoint;
} S_FRAMEWORK_CONTROL_BLOCK, *S_FW_CB_PTR;

typedef  E_TEST_ERROR (*ctiTestFunctionPtr)(S_FW_CB_PTR frameWorkCB_p);

/*
* STRUCTS
*/

/*
* FUNCTIONS PROTOTYPES
*/

#endif /* __CTI_TYPES_H */

