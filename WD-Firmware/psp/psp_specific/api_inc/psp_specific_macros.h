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

#elif defined (D_Pulpino_Vega_RV32M1RM)

   /* Constants to define the additional registers found on the Pulpino RI5KY. */
   #define lpstart0 	0x7b0
   #define lpend0 		0x7b1
   #define lpcount0 	0x7b2
   #define lpstart1 	0x7b4
   #define lpend1 		0x7b5
   #define lpcount1 	0x7b6

   /* Save additional registers found on the Pulpino. */
   .macro m_pushAdditionalRegs
      addi sp, sp, -(D_pspAdditionalContextSize * D_pspWORD_SIZE) /* Make room for the additional registers. */
      csrr t0, lpstart0							 /* Load additional registers into accessible temporary registers. */
      csrr t1, lpend0
      csrr t2, lpcount0
      csrr t3, lpstart1
      csrr t4, lpend1
      csrr t5, lpcount1
      sw t0, 1 * D_pspWORD_SIZE( sp )
      sw t1, 2 * D_pspWORD_SIZE( sp )
      sw t2, 3 * D_pspWORD_SIZE( sp )
      sw t3, 4 * D_pspWORD_SIZE( sp )
      sw t4, 5 * D_pspWORD_SIZE( sp )
      sw t5, 6 * D_pspWORD_SIZE( sp )
   .endm

   /* Restore the additional registers found on the Pulpino. */
   .macro m_popAdditionalRegs
      lw t0, 1 * D_pspWORD_SIZE( sp )			/* Load additional registers into accessible temporary registers. */
      lw t1, 2 * D_pspWORD_SIZE( sp )
      lw t2, 3 * D_pspWORD_SIZE( sp )
      lw t3, 4 * D_pspWORD_SIZE( sp )
      lw t4, 5 * D_pspWORD_SIZE( sp )
      lw t5, 6 * D_pspWORD_SIZE( sp )
      csrw lpstart0, t0
      csrw lpend0, t1
      csrw lpcount0, t2
      csrw lpstart1, t3
      csrw lpend1, t4
      csrw lpcount1, t5
      addi sp, sp, (D_pspAdditionalContextSize * D_pspWORD_SIZE )/* Remove space added for additional registers. */
   .endm

#elif defined (D_RV32I_CLINT_no_extensions)

   .macro m_pushAdditionalRegs
      /* No additional registers to save, so this macro does nothing. */
   .endm

   .macro m_popAdditionalRegs
      /* No additional registers to restore, so this macro does nothing. */
   .endm

#else
   // NatiR #error "Specific core is not defined!"
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
