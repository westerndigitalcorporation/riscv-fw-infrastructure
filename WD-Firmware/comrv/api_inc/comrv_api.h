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
#define D_COMRV_INVALID_INIT_PARAMS_ERR   0
#define D_COMRV_NO_AVAILABLE_ENTRY_ERR    1
#define D_COMRV_LOAD_ERR                  2
#define D_COMRV_OVL_DATA_DEFRAG_ERR       3
#define D_COMRV_CRC_CHECK_ERR             4

#define D_COMRV_PROFILE_BASE_IND          0xFC
#define D_COMRV_PROFILING_INVOKE_VAL      1
#define D_COMRV_PROFILING_LOAD_VAL        2
/* Load and invoke an overlay function */
#define D_COMRV_LOAD_AND_INVOKE_IND       (D_COMRV_PROFILE_BASE_IND | D_COMRV_PROFILING_LOAD_BIT | D_COMRV_PROFILING_INVOKE_BIT)
/* invoke an overlay function already loaded */
#define D_COMRV_NO_LOAD_AND_INVOKE_IND    (D_COMRV_PROFILE_BASE_IND | D_COMRV_PROFILING_INVOKE_BIT)
/* load and return from an overlay function */
#define D_COMRV_LOAD_AND_RETURN_IND       (D_COMRV_PROFILE_BASE_IND | D_COMRV_PROFILING_LOAD_VAL)
/* return from an overlay function w/o loading */
#define D_COMRV_NO_LOAD_AND_RETURN_IND    (D_COMRV_PROFILE_BASE_IND)

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
typedef struct comrvInitArgs
{
   /* address the memory region which shall be used
      for loading and executing overlay groups */
   void* pCacheMemoeyAddress;
   /* size in bytes of the memory pointed by pCacheMemoeyAddress
      the size must be aligned to D_COMRV_OVL_GROUP_SIZE_MIN */
   u32_t uiCacheSizeInBytes;
}comrvInitArgs_t;

/* comrv load arguments */
typedef struct comrvLoadArgs
{
   /* offset in bytes of the group to be loaded */
   u32_t uiGroupOffset;
   /* size in bytes of the group to be loaded */
   u32_t uiSizeInBytes;
   /* destination address to copy to the loaded group */
   void* pDest;
}comrvLoadArgs_t;

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
void comrvInit(comrvInitArgs_t* pInitParams);
void comrvGetStatus(void);
// TODO: ronen - use define for no inline
__attribute__((noinline)) void comrvInitApplicationStack(void);

#endif /* __COMRV_TASK_API_H__ */
