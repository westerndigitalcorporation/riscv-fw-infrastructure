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
* @file   rtosal.h
* @author Ronen Haen
* @date   21.01.2019 
* @brief  The defines rtos-al private interfaces 
* 
*/
#ifndef  __RTOSAL_H__
#define  __RTOSAL_H__

/**
* include files
*/

/**
* definitions
*/
/* interrupt context indication values */
#define D_RTOSAL_NON_INT_CONTEXT              0
#define D_RTOSAL_INT_CONTEXT                  1

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
extern void (*fptrParamErrorNotification)(const void *pParam, u32_t uErrorCode);

/**
* global variables
*/


/**
 * API
 */

/**
* @brief Activated upon Timer-tick (and invokes context-switch in the OS)
*
* @param None
*
*/
void rtosalTick(void);

/**
* @brief check if in ISR context
*
* @param None
*
* @return u32_t            - D_NON_INT_CONTEXT
*                          - non zero value - interrupt context
*/
u32_t rtosalIsInterruptContext(void);

#endif /* __RTOSAL_H__ */
