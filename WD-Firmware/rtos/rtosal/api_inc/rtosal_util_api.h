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
* @file   rtosal_util_api.h
* @author Ronen Haen
* @date   07.02.2019
* @brief  The file defines the RTOS AL util interfaces
*/
#ifndef __RTOSAL_UTIL_API_H__
#define __RTOSAL_UTIL_API_H__

/**
* include files
*/
#include "rtosal_config.h"
#include "rtosal_defines.h"
#include "rtosal_types.h"

/**
* definitions
*/

/**
* macros
*/

/**
* types
*/
/* application specific initialization function */
typedef void (*rtosalApplicationInit_t)(void *pParam);

/* param error notification function */
typedef void (*rtosalParamErrorNotification_t)(const void *pParam, u32_t uiErrorCode);

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
/**
* This API initializes the RTOS and triggers the scheduler operation
*
* @param None
*
* @return calling this function will never return
*/
void rtosalStart(rtosalApplicationInit_t fptrInit);

/**
* Set param error notification function
*
* @param fptrRtosalParamErrorNotification - notification function
*
* @return none
*/
void rtosalParamErrorNotifyFuncSet(rtosalParamErrorNotification_t fptrRtosalParamErrorNotification);

#endif /* __RTOSAL_UTIL_API_H__ */
