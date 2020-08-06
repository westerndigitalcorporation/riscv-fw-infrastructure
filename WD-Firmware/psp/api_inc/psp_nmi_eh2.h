/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2020 Western Digital Corporation or its affiliates.
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
* @file   psp_nmi_eh2.h
* @author Nati Rapaport
* @date   19.05.2020
* @brief  The file defines the psp NMI interfaces for features of SweRV EH2
* 
*/
#ifndef __PSP_NMI_EH2_H__
#define __PSP_NMI_EH2_H__

/**
* include files
*/

/**
* types
*/

/**
* definitions
*/
#define D_PSP_NMI_FAST_INT_DUOBLE_BIT_ECC_ERROR   0xF0001000
#define D_PSP_NMI_FAST_INT_DCCM_ACCESS_ERROR      0xF0001001
#define D_PSP_NMI_FAST_INT_NON_DCCM_REGION_ERROR  0xF0001002

/**
* local prototypes
*/

/**
* external prototypes
*/

/**
* macros
*/

/**
* global variables
*/

/**
* APIs
*/

/**
 * @brief - delegate pin-asserted NMI to a given Hart (HW thread)
 *          That means - upon occurence of the pin-asserted NMI, it will be handelled by the given Hart    
 *
 * @parameter - Hart number to delegate the NMI to
 */
void pspNmiSetDelegation(u32_t uiHartNumber);

/**
 * @brief - clear delegation of pin-asserted NMI for a given Hart (HW thread)
 *          That means - upon occurence of the pin-asserted NMI, the given Hart will not handle the NMI    
 *
 * @parameter - Hart number to clear NMI delegation from
 */
void pspNmiClearDelegation(u32_t uiHartNumber);

/**
 * @brief - check whether pin-asserted NMI handling is delegated to the given hart (HW thread) or not
 *
 * @parameter - Hart number
 * @return    - 0/1 to indicate whether the NMI handling is delegated to the given hart-number or not
 */
u32_t pspIsNmiDelegatedToHart(u32_t uiHartNumber);

#endif /* __PSP_NMI_EH2_H__ */
