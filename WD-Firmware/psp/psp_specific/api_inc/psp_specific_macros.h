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
* @file   psp_specific_macros.h
* @author Nati Rapaport
* @date   18.09.2019
* @brief  This file is used internally by PSP module for macros, defined specificalley per core type
*/
#ifndef  __PSP_SPECIFIC_MACROS_H__
#define  __PSP_SPECIFIC_MACROS_H__



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

/**
* macros
*/
#if defined (D_RV_HiFive1)

   .macro m_pushAdditionalRegs
      /* No additional registers to save, so this macro does nothing. */
   .endm

   .macro m_popAdditionalRegs
	   /* No additional registers to restore, so this macro does nothing. */
   .endm

#elif defined (D_RV32I_CLINT_no_extensions)

   .macro m_pushAdditionalRegs
      /* No additional registers to save, so this macro does nothing. */
   .endm

   .macro m_popAdditionalRegs
      /* No additional registers to restore, so this macro does nothing. */
   .endm

#else
   #error "Specific configuration is not defined!"
#endif




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


#endif /* __PSP_SPECIFIC_MACROS_H__ */
