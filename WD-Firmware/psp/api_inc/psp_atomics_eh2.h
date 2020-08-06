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
* @file   psp_atomics_eh2.h
* @author Nati Rapaport
* @date   21.06.2020
* @brief  The file defines APIs for atomic commands on SweRV EH2  
*/
#ifndef  __PSP_ATOMICS_EH2_H__
#define  __PSP_ATOMICS_EH2_H__


/**
* include files
*/

/**
* definitions
*/
/* Address of DCCM area that PSP is use for safe handling of multi-harts functionalities */
#ifdef D_DCCM_SECTION_START_ADDRESS
    #define D_PSP_DCCM_SECTION_ADDRESS  D_DCCM_SECTION_START_ADDRESS
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
/* Atomic operations macros */
#ifdef D_PSP_DCCM_SECTION_ADDRESS
  #define M_PSP_ATOMIC_COMPARE_AND_SET(pAddress, uiExpectedValue, uiDesiredValue)   pspAtomicsCompareAndSet(pAddress, uiExpectedValue, uiDesiredValue);
  #define M_PSP_ATOMIC_ENTER_CRITICAL_SECTION(pAddress)                             pspAtomicsEnterCriticalSection(pAddress);
  #define M_PSP_ATOMIC_EXIT_CRITICAL_SECTION(pAddress)                              pspAtomicsExitCriticalSection(pAddress);
  #define M_PSP_ATOMIC_AMO_SWAP(pAddress, uiValueToSwap)                            pspAtomicsAmoSwap(pAddress, uiValueToSwap);
  #define M_PSP_ATOMIC_AMO_ADD(pAddress, uiValueToAdd)                              pspAtomicsAmoAdd(pAddress, uiValueToAdd);
  #define M_PSP_ATOMIC_AMO_AND(pAddress, uiValueToAndWith)                          pspAtomicsAmoAnd(pAddress, uiValueToAndWith);
  #define M_PSP_ATOMIC_AMO_OR(pAddress, uiValueToOrWith)                            pspAtomicsAmoOr(pAddress, uiValueToOrWith);
  #define M_PSP_ATOMIC_AMO_XOR(pAddress, uiValueToXorWith)                          pspAtomicsAmoXor(pAddress, uiValueToXorWith);
  #define M_PSP_ATOMIC_AMO_MIN(pAddress, uiValueToCompare)                          pspAtomicsAmoMin(pAddress, uiValueToCompare);
  #define M_PSP_ATOMIC_AMO_MAX(pAddress, uiValueToCompare)                          pspAtomicsAmoMax(pAddress, uiValueToCompare);
  #define M_PSP_ATOMIC_AMO_MIN_UNSIGINED(pAddress, uiValueToCompare)                pspAtomicsAmoMinUnsigned(pAddress, uiValueToCompare);
  #define M_PSP_ATOMIC_AMO_MAX_UNSIGNED(pAddress, uiValueToCompare)                 pspAtomicsAmoMaxUnsigned(pAddress, uiValueToCompare);
#else
  /* You can operate atomic commands only on variables in the DCCM. If DCCM is not defined, then these macros are empty */
  #define M_PSP_ATOMIC_COMPARE_AND_SET(pAddress, uiExpectedValue, uiDesiredValue)
  #define M_PSP_ATOMIC_ENTER_CRITICAL_SECTION(pAddress)
  #define M_PSP_ATOMIC_EXIT_CRITICAL_SECTION(pAddress)
  #define M_PSP_ATOMIC_AMO_SWAP(pAddress, uiValueToSwap)
  #define M_PSP_ATOMIC_AMO_ADD(pAddress, uiValueToAdd)
  #define M_PSP_ATOMIC_AMO_AND(pAddress, uiValueToAndWith)
  #define M_PSP_ATOMIC_AMO_OR(pAddress, uiValueToOrWith)
  #define M_PSP_ATOMIC_AMO_XOR(pAddress, uiValueToXorWith)
  #define M_PSP_ATOMIC_AMO_MIN(pAddress, uiValueToCompare)
  #define M_PSP_ATOMIC_AMO_MAX(pAddress, uiValueToCompare)
  #define M_PSP_ATOMIC_AMO_MIN_UNSIGINED(pAddress, uiValueToCompare)
  #define M_PSP_ATOMIC_AMO_MAX_UNSIGNED(pAddress, uiValueToCompare)
#endif

/**
* global variables
*/

/**
* APIs
*/

/**
* @brief - Returns the start address in the DCCM that can be used for atomic operations
*
* @return - address in the DCCM area, free for usage
*/
u32_t pspAtomicsGetAddressForAtomicOperations(void);

/**
* @brief - compare and set a value in the memory using atomic commands
*
* @parameter - address in the memory to compare and set the value there
* @parameter - expected value in the memory address
* @parameter - new value to set at that memory address
*
* @return    - 0 - success. Otherwise - failure
*/
D_PSP_NO_INLINE u32_t pspAtomicsCompareAndSet(volatile u32_t* pAddress, u32_t uiExpectedValue, u32_t uiDesiredValue);

/**
* @brief - spin-lock on a parameter in a given memory address
*
* @parameter - (a0) address in the memory with the parameter to check
*/
D_PSP_NO_INLINE void pspAtomicsEnterCriticalSection(volatile u32_t* pAddress);

/**
* @brief - release the lock by parameter in a given memory address using atomic command
*
* @parameter - (a0) address in the memory with the parameter to set
*/
D_PSP_NO_INLINE void pspAtomicsExitCriticalSection(volatile u32_t* pAddress);

/**
* @brief - AMO (Atomic Memory Operation) Swap command
*
* @parameter - (a0) address in the memory with the parameter to check and add
* @parameter - (a1) value to load to that address
*
* @return    - (a0) previous value in the address
*/
u32_t pspAtomicsAmoSwap(u32_t* pAddress, u32_t uiValueToSwap);

/**
* @brief - AMO (Atomic Memory Operation) Add command
*
* @parameter - (a0) address in the memory with the parameter to check and add
* @parameter - (a1) value to add
*
* @return    - (a0) previous value in the address
*/
u32_t pspAtomicsAmoAdd(u32_t* pAddress, u32_t uiValueToAdd);

/**
* @brief - AMO (Atomic Memory Operation) bitwise AND command
*
* @parameter - (a0) address in the memory with the parameter to check and do bitwise-AND with
* @parameter - (a1) value to do bitwise-AND with the content of the given memory
*
* @return    - (a0) previous value in the address
*/
u32_t pspAtomicsAmoAnd(u32_t* pAddress, u32_t uiValueToAndWith);

/**
* @brief - AMO (Atomic Memory Operation) bitwise OR command
*
* @parameter - (a0) address in the memory with the parameter to check and do bitwise-OR with
* @parameter - (a1) value to do bitwise-OR with the content of the given memory
*
* @return    - (a0) previous value in the address
*/
u32_t pspAtomicsAmoOr(u32_t* pAddress, u32_t uiValueToOrWith);

/**
* @brief - AMO (Atomic Memory Operation) bitwise XOR command
*
* @parameter - (a0) address in the memory with the parameter to check and do bitwise-XOR with
* @parameter - (a1) value to do bitwise-XOR with the content of the given memory
*
* @return    - (a0) previous value in the address
*/
u32_t pspAtomicsAmoXor(u32_t* pAddress, u32_t uiValueToXorWith);

/**
* @brief - AMO (Atomic Memory Operation) min command
*
* @parameter - (a0) address in the memory with the parameter to compare
* @parameter - (a1) value to compare with the content of the given memory, to determine the minimum
*
* @return    - (a0) previous value in the address
*/
u32_t pspAtomicsAmoMin(u32_t* pAddress, u32_t uiValueToCompare);

/**
* @brief - AMO (Atomic Memory Operation) max command
*
* @parameter - (a0) address in the memory with the parameter to compare
* @parameter - (a1) value to compare with the content of the given memory, to determine the maximum
*
* @return    - (a0) previous value in the address
*/
u32_t pspAtomicsAmoMax(u32_t* pAddress, u32_t uiValueToCompare);

/**
* @brief - AMO (Atomic Memory Operation) min command - unsigned
*
* @parameter - (a0) address in the memory with the parameter to compare (unsigned)
* @parameter - (a1) unsigned value to compare with the content of the given memory, to determine the minimum
*
* @return    - (a0) previous value in the address
*/
u32_t pspAtomicsAmoMinUnsigned(u32_t* pAddress, u32_t uiValueToCompare);

/**
* @brief - AMO (Atomic Memory Operation) max command - unsigned
*
* @parameter - (a0) address in the memory with the parameter to compare (unsigned)
* @parameter - (a1) unsigned value to compare with the content of the given memory, to determine the maximum
*
* @return    - (a0) previous value in the address
*/
u32_t pspAtomicsAmoMaxUnsigned(u32_t* pAddress, u32_t uiValueToCompare);

#endif /* __PSP_ATOMICS_EH2_H__ */
