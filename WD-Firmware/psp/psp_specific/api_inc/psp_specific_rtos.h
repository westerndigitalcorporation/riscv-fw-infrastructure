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
* @file   psp_specific_rtos.h
* @author Nati Rapaport
* @date   16.09.2019
* @brief  This file is used internally by PSP module and serves as an api layer to specific core.
*/
#ifndef  __PSP_SPECIFIC_RTOS_H__
#define  __PSP_SPECIFIC_RTOS_H__

/**
* include files
*/

/**
* APIs
*/
/* API to context-switch handler in the RTOS layer */
#ifdef D_USE_FREERTOS
   .extern void vTaskSwitchContext(void);
#else
   #error "Define APIs per the RTOS in use"
#endif

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

/**
* external prototypes
*/


/**
* global variables
*/


#endif /* __PSP_SPECIFIC_RTOS_H__ */
