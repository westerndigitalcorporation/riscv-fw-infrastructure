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
* @file   rtosal_error.c
* @author Nati Rapaport
* @date   13.11.2019
* @brief  The file supply service to handle error notification in RTOSAL
* 
*/

/**
* include files
*/
#include "rtosal_error_api.h"
#include "rtosal_macro.h"

/**
* definitions
*/

/**
* macros
*/

/**
* types
*/

/**
* local prototypes
*/
RTOSAL_SECTION void rtosalParamErrorNotification(const void *pParam, u32_t uiErrorCode);

/**
* external prototypes
*/


/**
* global variables
*/
rtosalParamErrorNotification_t fptrParamErrorNotification = rtosalParamErrorNotification;

/**
* functions
*/

/**
* default 'param error' notification function
* The user should define and register a param-notification function of his own
*
* @param pParam      pointer of the invalid parameter
* @param uiErrorCode  error code
*
* @return none
*/
RTOSAL_SECTION void rtosalParamErrorNotification(const void *pParam, u32_t uiErrorCode)
{
   (void)pParam;
   (void)uiErrorCode;
}

/**
* Register notification function for a case of param error
*
* @param fptrRtosalParamErrorNotification - pointer to a notification function
*
* @return none
*/
RTOSAL_SECTION void rtosalParamErrorNotifyFuncRegister(rtosalParamErrorNotification_t fptrRtosalParamErrorNotification)
{
   fptrParamErrorNotification = fptrRtosalParamErrorNotification;
}
