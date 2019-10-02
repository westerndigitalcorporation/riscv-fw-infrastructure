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
* @file   psp_specific_definitions.h
* @author Nati Rapaport
* @date   16.09.2019
* @brief  This file is used internally by PSP module for handling CSRs specific to a given core
*/
#ifndef  __PSP_SPECIFIC_DEFS_H__
#define  __PSP_SPECIFIC_DEFS_H__



/**
* include files
*/

/**
* APIs
*/
/* API to context-switch handler in the RTOS layer */

/**
* definitions
*/
#if defined (D_RV_HiFive1)
   #define D_pspHasCLINT 1
   #define D_pspAdditionalContextSize 0 /* Must be even number on 32-bit cores. */
#elif defined (D_Pulpino_Vega_RV32M1RM)
   #define D_pspHasCLINT 0
   #define D_pspAdditionalContextSize 6 /* Must be even number on 32-bit cores. */
#elif defined (D_RV32I_CLINT_no_extensions)
    #define D_pspHasCLINT 1
    #define D_pspAdditionalContextSize 0 /* Must be even number on 32-bit cores. */
#else
   #error "Specific core is not defined!"
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


#endif /* __PSP_SPECIFIC_DEFS_H__ */
