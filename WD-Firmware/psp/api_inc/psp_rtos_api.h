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
* @file   psp_rtos_api.h
* @author Nati Rapaport
* @date   19.09.2019
* @brief  The file defines the API of PSP to the specific RTOS in use
*/
#ifndef  __PSP_RTOS_API_H__
#define  __PSP_RTOS_API_H__

#ifdef D_USE_FREERTOS
/**
* include files
*/
#include "FreeRTOS.h"
#include "portable.h"

/**
* definitions
*/
#define pspBYTE_ALIGNMENT_MASK portBYTE_ALIGNMENT_MASK

#else
   #error "Define dependencies and includes appropriately for the RTOS in use"
#endif

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

#endif /* __PSP_RTOS_API_H__ */
