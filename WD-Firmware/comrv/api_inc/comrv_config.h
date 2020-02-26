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
#endif /* D_COMRV_USE_MULTI_GROUP_SUPPORT */

/* enable instrumentation */
#ifdef D_COMRV_ENABLE_FW_INSTRUMENTATION_SUPPORT
   #define D_COMRV_FW_INSTRUMENTATION
#endif /* D_COMRV_ENABLE_FW_INSTRUMENTATION_SUPPORT */

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

#endif /* __COMRV_CONFIG_H__ */
