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
* @file   comrv_api.h
* @author Ronen Haen
* @date   11.06.2019
* @brief  The file defines the COM-RV interfaces
*/
#ifndef __COMRV_TASK_API_H__
#define __COMRV_TASK_API_H__

/**
* include files
*/

/**
* definitions
*/
#define D_COMRV_NO_AVAILABLE_ENTRY_ERR    0
#define D_COMRV_LOAD_ERR                  1
#define D_COMRV_OVL_DATA_DEFRAG_ERR       2
#define D_COMRV_CRC_CHECK_ERR             3

/**
* macros
*/
/* this macro is used to imitate the compiler and it replaces
 * the call to any overlay func() with saving the address of func() to x31
 * and jumping to x31 (which is set to comrv entry point).
 * The addi is for making the address an 'address token' differentiating it
 * from regular address
 */
/* TODO:
 * asm volatile ("la t6, "#func :  :  : );
 * is a workaround, should be
 * asm volatile ("la t6, %0" :  : "i"(func) : );
 * This is due to a bug in clang
 */
#define _OVERLAY_  __attribute__((overlaycall)) __attribute__((noinline))

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
* APIs
*/
void   comrvInit(void);
void   comrvErrorInddicationHook(u32_t errorNum);
void   comrvMemcpyHook(void* pDest, void* pSrc, u32_t sizeInBytes);
void*  comrvLoadOvlayGroupHook(u32_t groupOffset, void* pDest, u32_t sizeInBytes);
void   comrvNotificationHook(void);
u32_t comrvCrcCalcHook(void* pAddress, u16_t memprySize);

#endif /* __COMRV_TASK_API_H__ */
