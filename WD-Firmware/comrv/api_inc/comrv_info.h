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
* @file   comrv_info.h
* @author Ronen Haen
* @date   10.05.2020
* @brief  The file defines the COM-RV info for the end user
*/
#ifndef __COMRV_INFO_H__
#define __COMRV_INFO_H__

/**
* include files
*/
#include "common_types.h"
#include "comrv_config.h"

/**
* definitions
*/
/* comrv version */
#define D_COMRV_VERSION_MINOR    1
#define D_COMRV_VERSION_MAJOR    0

/* multi-group field offset in comrv stack */
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
   #ifdef D_COMRV_MIN_NUM_OF_MULTI_GROUPS
      /* field size is one bytes */
      #define D_COMRV_MULTIGROUP_OFFSET   11
   #else
      /* field size is two bytes */
      #define D_COMRV_MULTIGROUP_OFFSET   14
   #endif /* D_COMRV_MIN_NUM_OF_MULTI_GROUPS */
#else
   /* field doesn't exist */
   #define D_COMRV_MULTIGROUP_OFFSET   0
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */

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

/**
* APIs
*/

#endif /* __COMRV_INFO_H__ */
