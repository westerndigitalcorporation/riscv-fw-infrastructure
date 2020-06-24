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
* @file   comrv_defines.h
* @author Ronen Haen
* @date   11.06.2019
* @brief  This file provides COM-RV defines
* 
*/
#ifndef  __COMRV_DEFINES_H__
#define  __COMRV_DEFINES_H__

/**
* include files
*/
#include "psp_intrinsics.h"
#include "psp_attributes.h"
#include "comrv_config.h"

/**
* definitions
*/
// TODO: ronen - use psp defines
#define D_COMRV_INLINE     D_PSP_ALWAYS_INLINE
#define D_COMRV_NO_INLINE  D_PSP_NO_INLINE
#define _OVERLAY_          __attribute__((overlaycall))

#define D_COMRV_NUM_OF_CACHE_ENTRIES      (D_COMRV_OVL_CACHE_SIZE_IN_BYTES/D_COMRV_OVL_GROUP_SIZE_MIN)

/* profile selection */
#if D_COMRV_NUM_OF_CACHE_ENTRIES < 0xFF
   #define D_COMRV_PROFILE 1
#elif D_COMRV_NUM_OF_CACHE_ENTRIES < 0xFFFF
   #define D_COMRV_PROFILE 2
#else
   #define D_COMRV_PROFILE 3
#endif /* D_COMRV_PROFILE==0 */

#if D_COMRV_PROFILE == 1

  #ifdef D_COMRV_EVICTION_LRU
    typedef u08_t lru_t;
    typedef u16_t lruIndexes_t;
  #elif defined(D_COMRV_EVICTION_LFU)
  #elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
  #endif /* D_COMRV_EVICTION_LRU */

#elif D_COMRV_PROFILE == 2

  #ifdef D_COMRV_EVICTION_LRU
    typedef u16_t lru_t;
    typedef u32_t lruIndexes_t;
  #elif defined(D_COMRV_EVICTION_LFU)
  #elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
  #endif /* D_COMRV_EVICTION_LRU */

#elif D_COMRV_PROFILE == 3

  #ifdef D_COMRV_EVICTION_LRU
    typedef u32_t lru_t;
    typedef u64_t lruIndexes_t;
  #elif defined(D_COMRV_EVICTION_LFU)
  #elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
  #endif /* D_COMRV_EVICTION_LRU */

#endif /* D_COMRV_PROFILE_1 */

/* comrv errors */
#define D_COMRV_SUCCESS                      0
#define D_COMRV_INVALID_INIT_PARAMS_ERR      1
#define D_COMRV_NOT_ENOUGH_ENTRIES           2
#define D_COMRV_LOAD_ERR                     3
#define D_COMRV_OVL_DATA_DEFRAG_ERR          4
#define D_COMRV_CRC_CHECK_ERR                5
#define D_COMRV_TBL_LOAD_ERR                 6
#define D_COMRV_LOCK_UNLOCK_ERR              7
#define D_COMRV_INVOKED_WHILE_DISABLED       8
#define D_COMRV_ENTER_CRITICAL_SECTION_ERR   9
#define D_COMRV_EXIT_CRITICAL_SECTION_ERR    10
#define D_COMRV_INTERNAL_ERR                 11
#define D_COMRV_NOT_INITIALIZED_ERR          12


/* comrv defines */
#define D_COMRV_INSTRUMENTATION_BASE_IND     0xFC
#define D_COMRV_INSTRUMENTATION_INVOKE_BIT   1
#define D_COMRV_INSTRUMENTATION_LOAD_BIT     2
/* Load and invoke an overlay function */
#define D_COMRV_LOAD_AND_INVOKE_IND          (D_COMRV_INSTRUMENTATION_BASE_IND | D_COMRV_INSTRUMENTATION_LOAD_BIT | D_COMRV_INSTRUMENTATION_INVOKE_BIT)
/* invoke an overlay function already loaded */
#define D_COMRV_NO_LOAD_AND_INVOKE_IND       (D_COMRV_INSTRUMENTATION_BASE_IND | D_COMRV_INSTRUMENTATION_INVOKE_BIT)
/* load and return from an overlay function */
#define D_COMRV_LOAD_AND_RETURN_IND          (D_COMRV_INSTRUMENTATION_BASE_IND | D_COMRV_INSTRUMENTATION_LOAD_BIT)
/* return from an overlay function w/o loading */
#define D_COMRV_NO_LOAD_AND_RETURN_IND       (D_COMRV_INSTRUMENTATION_BASE_IND)
/* invalid token */
#define D_COMRV_INVALID_TOKEN                0xFFFFFFFF
/* tables token */
#define D_COMRV_TABLES_TOKEN                 0x00000001

#endif /* __COMRV_DEFINES_H__ */
