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
* @file   comrv_config.h
* @author Ronen Haen
* @date   11.06.2019
* @brief  The file defines the COM-RV configuration
*/
#ifndef __COMRV_CONFIG_H__
#define __COMRV_CONFIG_H__

/**
* include files
*/

/**
* definitions
*/
/* specify eviction policy */
//#define D_COMRV_EVICTION_POLICY=1

/* enable multi-group support */
#ifdef D_COMRV_ENABLE_MULTI_GROUP_SUPPORT
   #define D_COMRV_MULTI_GROUP_SUPPORT
   /* this will use upto 2^8 -1 multi groups; if not defined,
      the maximum number of groups will be used (2^16 - 1) */
   #ifdef D_COMRV_ENABLE_MIN_NUM_OF_MULTI_GROUP_SUPPORT
      #define D_COMRV_MIN_NUM_OF_MULTI_GROUPS
   #endif /* D_COMRV_ENABLE_MAX_MULTI_GROUP_SIZE_SUPPORT */
#endif /* D_COMRV_USE_MULTI_GROUP_SUPPORT */

/* enable instrumentation */
#ifdef D_COMRV_ENABLE_FW_INSTRUMENTATION_SUPPORT
   #define D_COMRV_FW_INSTRUMENTATION
#endif /* D_COMRV_ENABLE_FW_INSTRUMENTATION_SUPPORT */

#ifdef D_COMRV_ALLOW_CALLS_AFTER_SEARCH_LOAD_SUPPORT
   /* Mark we allow non comrv function calls after search and load
      indication - applies only for rtos support to handle
      cases where we get a context switch during calls to such functions */
   #define D_COMRV_ALLOW_CALLS_AFTER_SEARCH_LOAD
#endif /* D_COMRV_ENABLE_FW_INSTRUMENTATION_NON_ATOMIC_SUPPORT */

/* enable init vars run time validation */
#ifdef D_COMRV_ENABLE_VERIFY_INIT_ARGS_SUPPORT
   #define D_COMRV_VERIFY_INIT_ARGS
#endif /* D_COMRV_ENABLE_VERIFY_INIT_ARGS_SUPPORT */

/* enable overlay data usage */
#ifdef D_COMRV_ENABLE_OVL_DATA_SUPPORT
   #define D_COMRV_OVL_DATA_SUPPORT
#endif /* D_COMRV_ENABLE_OVL_DATA_SUPPORT */

/* enable CRC */
#ifdef D_COMRV_ENABLE_CRC_SUPPORT
   #define D_COMRV_CRC
#endif /* D_COMRV_ENABLE_CRC_SUPPORT */

#ifdef D_COMRV_ENABLE_DEBUG_SUPPORT
   #define D_COMRV_DEBUG
#endif /* D_COMRV_ENABLE_DEBUG_SUPPORT */

/* enable the ability to enable/disable comrv */
#ifdef D_COMRV_ENABLE_CONTROL_SUPPORT
   #define  D_COMRV_CONTROL_SUPPORT
#endif /* D_COMRV_ENABLE_CONTROL_SUPPORT */

/* enable comrv error notifications */
#ifdef D_COMRV_ENABLE_ERROR_NOTIFICATIONS
   #define D_COMRV_ERROR_NOTIFICATIONS
#endif /* M_COMRV_ENABLE_ERROR_NOTIFICATIONS */

/* enable rtos support */
#ifdef D_COMRV_ENABLE_RTOS_SUPPORT
   #define D_COMRV_RTOS_SUPPORT
#endif /* D_COMRV_ENABLE_RTOS_SUPPORT */

/* enable comrv asserts */
#ifdef D_COMRV_ENABLE_ASSERT_ENABLED
   #define D_COMRV_ASSERT_ENABLED
#endif /* D_COMRV_ENABLE_ASSERT_ENABLED */

/* minimum size of an overlay group */
#if D_COMRV_MIN_GROUP_SIZE_IN_BYTES > 0
   #if D_COMRV_MIN_GROUP_SIZE_IN_BYTES < 512
      #error "unsupported size D_COMRV_MIN_GROUP_SIZE_IN_BYTES provided"
   #endif /* D_COMRV_MIN_GROUP_SIZE_IN_BYTES < 512 */
   #define D_COMRV_OVL_GROUP_SIZE_MIN       D_COMRV_MIN_GROUP_SIZE_IN_BYTES
#else
   #define D_COMRV_OVL_GROUP_SIZE_MIN       512
#endif /* D_COMRV_MIN_GROUP_SIZE_IN_BYTES */

/* maximum size of an overlay group */
#if D_COMRV_MAX_GROUP_SIZE_IN_BYTES > 0
   #if D_COMRV_MAX_GROUP_SIZE_IN_BYTES > 4096
      #error "unsupported size D_COMRV_MAX_GROUP_SIZE_IN_BYTES provided"
   #endif /* D_COMRV_MAX_GROUP_SIZE_IN_BYTES > 4096 */
   #define D_COMRV_OVL_GROUP_SIZE_MAX       D_COMRV_MAX_GROUP_SIZE_IN_BYTES
#else
   #define D_COMRV_OVL_GROUP_SIZE_MAX       4096
#endif /* D_COMRV_MAX_GROUP_SIZE_IN_BYTES */

/* maximum number of overlay calls depth within the
   entire application; if the application contains several tasks
   this define must cover the max number at any given time */
#if D_COMRV_MAX_CALL_STACK_DEPTH > 0
   #define D_COMRV_CALL_STACK_DEPTH         D_COMRV_MAX_CALL_STACK_DEPTH
#else
   #define D_COMRV_CALL_STACK_DEPTH         10
#endif /* D_COMRV_MAX_CALL_STACK_DEPTH */

/* size of the overlay cache - the size of the RAM provided
   for loading and executing the overlay groups; granularity
   expressed in bytes */
#if D_COMRV_MAX_OVL_CACHE_SIZE_IN_BYTES > 0
   #define D_COMRV_OVL_CACHE_SIZE_IN_BYTES  D_COMRV_MAX_OVL_CACHE_SIZE_IN_BYTES
#else
   #define D_COMRV_OVL_CACHE_SIZE_IN_BYTES  1536
#endif /* D_COMRV_MAX_OVL_CACHE_SIZE_IN_BYTES */

/* eviction algorithm definition */
#if (D_COMRV_EVICTION_POLICY == 0)
   #ifndef D_COMRV_EVICTION_POLICY
      /* can be that D_COMRV_EVICTION_POLICY wasn't defined - same as if it was
         set to 0 so set it to 1 - 0 and 1 are the same - lru */
      #define D_COMRV_EVICTION_POLICY 1
   #endif /* D_COMRV_EVICTION_POLICY */
   #define D_COMRV_EVICTION_LRU
#elif (D_COMRV_EVICTION_POLICY == 1)
   #define D_COMRV_EVICTION_LRU
#elif (D_COMRV_EVICTION_POLICY == 2)
   #define D_COMRV_EVICTION_LFU
#elif (D_COMRV_EVICTION_POLICY == 3)
   #define D_COMRV_EVICTION_MIX_LRU_LFU
#endif /* */

#endif /* __COMRV_CONFIG_H__ */
