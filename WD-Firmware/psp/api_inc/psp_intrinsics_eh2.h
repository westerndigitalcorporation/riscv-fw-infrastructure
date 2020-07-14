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
* @file   psp_intrinsics_eh2.h
* @author Nati Rapaport
* @date   12.11.2019
* @brief  psp intrinsics for eh2 - atomic built-in operations
* 
*/
#ifndef  __PSP_INTRINSICS_EH2_H__
#define  __PSP_INTRINSICS_EH2_H__

#if defined (__GNUC__) || defined (__clang__)

/**
* include files
*/

/**
* definitions
*/

/* You can operate atomic commands only on variables in the DCCM. Do not define the atomic operation intrinsics if DCCM does not exist in the system */
#ifdef D_PSP_DCCM_SECTION_ADDRESS
  /* SweRV EH2 takes the most conservative approach to atomic and implements .aq znd .rl semantics with all atomic instructions. So we could afford using __ATOMIC_RELAXED */ 
  #define PSP_ATOMIC_SWAP(pAddress, value)  __atomic_exchange_n(pAddress, value, __ATOMIC_RELAXED); 
  #define PSP_ATOMIC_ADD(pAddress, value)   __atomic_add_fetch(pAddress, value, __ATOMIC_RELAXED);
  #define PSP_ATOMIC_AND(pAddress, value)   __atomic_and_fetch(pAddress, value, __ATOMIC_RELAXED);
  #define PSP_ATOMIC_XOR(pAddress, value)   __atomic_xor_fetch(pAddress, value, __ATOMIC_RELAXED);
  #define PSP_ATOMIC_OR(pAddress, value)    __atomic_or_fetch(pAddress, value, __ATOMIC_RELAXED);
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
* macros
*/

/**
* global variables
*/

/**
* APIs
*/

#endif /* defined (__GNUC__) || defined (__clang__) */

#endif /* __PSP_INTRINSICS_EH2_H__ */
