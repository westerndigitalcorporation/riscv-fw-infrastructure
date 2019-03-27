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
* @file   psp.c
* @author Ronen Haen
* @date   21.01.2019 
* @brief  The file implements the PSP API
* 
*/

/**
* include files
*/
#include "common_types.h"
#include "psp_api.h"

/**
* definitions
*/

/**
* macros
*/

/**
* types
*/
u32_t g_uiIsInterruptContext = D_NON_INT_CONTEXT;

/**
* local prototypes
*/

/**
* @brief check if in ISR context
*
* @param None
*
* @return u32_t            - D_NON_INT_CONTEXT
*                          - non zero value - interrupt context
*/
u32_t pspIsInterruptContext(void)
{
   return (g_uiIsInterruptContext > 0);
}
