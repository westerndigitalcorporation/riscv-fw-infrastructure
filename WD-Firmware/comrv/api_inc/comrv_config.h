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
//#define D_COMRV_MULTI_GROUP_SUPPORT
/* enable instrumentation */
//#define D_COMRV_FW_INSTRUMENTATION
/* enable init vars run time validation */
//#define D_COMRV_VERIFY_INIT_ARGS
/* enable overlay data usage */
//#define D_COMRV_OVL_DATA_SUPPORT
/* enable CRC */
//#define D_COMRV_CRC
/* enable comrv debug */
//#define D_COMRV_DEBUG
/* enable the ability to enable/disable comrv */
//#define  D_COMRV_CONTROL_SUPPORT
/* enable comrv error notifications */
#define M_COMRV_ERROR_NOTIFICATIONS

/* minimum size of an overlay group */
#define D_COMRV_OVL_GROUP_SIZE_MIN                          512
/* maximum size of an overlay group */
#define D_COMRV_OVL_GROUP_SIZE_MAX                          4096
/* maximum number of overlay calls depth within the
   entire application; if the application contains several tasks
   this define must cover the max number at any given time */
#define D_COMRV_CALL_STACK_DEPTH                            10
/* size of the overlay cache - the size of the RAM provided
   for loading and executing the overlay groups; granularity
   expressed in bytes */
#define D_COMRV_OVL_CACHE_SIZE_IN_BYTES                     1536

#endif /* __COMRV_CONFIG_H__ */
