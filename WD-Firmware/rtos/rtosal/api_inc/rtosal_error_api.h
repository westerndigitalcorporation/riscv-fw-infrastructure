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
* @file   rtosal_error_api.h
* @author Nati Rapaport
* @date   13.11.2019
* @brief  The file supply service to handle error notification in RTOSAL
*/
#ifndef __RTOSAL_ERROR_API_H__
#define __RTOSAL_ERROR_API_H__

/**
* include files
*/
#include "rtosal_config.h"
#include "rtosal_defines.h"
#include "rtosal_types.h"
#include "common_types.h"

/**
* definitions
*/

/**
* macros
*/

/**
* types
*/

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
* Set param error notification function
*/
//void rtosalParamErrorNotifyFuncSet(rtosalParamErrorNotification_t fptrRtosalParamErrorNotification);

/**
* Register notification function for a case of param error
*
* @param fptrRtosalParamErrorNotification - pointer to a notification function
*
* @return none
*/
void rtosalParamErrorNotifyFuncRegister(rtosalParamErrorNotification_t fptrRtosalParamErrorNotification);


#endif /* __RTOSAL_ERROR_API_H__ */
