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

/**
* macros
*/

/**
* types
*/

/**
* local prototypes
*/
void rtosalContextSwitchIndicationSet(void);

/**
* This function is invoked by the system timer interrupt
*/
void rtosalTick(void);

/**
* external prototypes
*/
extern void (*fptrParamErrorNotification)(const void *pParam, u32_t uErrorCode);

/**
* global variables
*/

#endif /* __RTOSAL_H__ */
