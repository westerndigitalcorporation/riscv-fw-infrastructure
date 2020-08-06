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
* @file   psp_atomics_eh2.c
* @author Nati Rapaport
* @date   06.07.2020
* @brief  The file contains APIs for atomic operations on SweRV EH2
*/

/**
* include files
*/
#include "psp_api.h"
#include "psp_internal_mutex_eh2.h"

/**
* types
*/

/**
* definitions
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

/**
* @brief - Returns the start address in the DCCM that can be used for atomic operations
*
* @return - address in the DCCM area, free for usage
*/
D_PSP_TEXT_SECTION u32_t pspAtomicsGetAddressForAtomicOperations(void)
{
  u32_t uiDccmAddress = M_PSP_END_OF_PSP_AREA_IN_DCCM();

  return (uiDccmAddress);
}

/**
* @brief - compare and set a value in the memory using atomic commands
*
* @parameter - address in the memory to compare and set the value there
* @parameter - expected value in the memory address
* @parameter - new value to set at that memory address
*
* @return    - 0 - success. Otherwise - failure
*/
D_PSP_NO_INLINE D_PSP_TEXT_SECTION u32_t pspAtomicsCompareAndSet(volatile u32_t* pAddress, u32_t uiExpectedValue, u32_t uiDesiredValue)
{
  M_PSP_ASSERT(NULL != pAddress);

  __asm__(
     "lr.w.aq    t0, (a0);"           /* Load original value from memory (address given by a0) to t0 */
     "bne        t0, a1, skip_out;"   /* If the value there is not as expected - return non-zero (== failure) */
     "sc.w.rl    a0, a2, (a0);"       /* Otherwise update memory with value from a2. If update succeeds - a0 = 0, otherwise - non-zero */
  "skip_out:"
  );
  return (u32_t)(pAddress);
}

/**
* @brief - spin-lock on a parameter in a given memory address
*
* @parameter - (a0) address in the memory with the parameter to check
*/
D_PSP_NO_INLINE D_PSP_TEXT_SECTION void pspAtomicsEnterCriticalSection(volatile u32_t* pAddress)
{
  M_PSP_ASSERT(NULL != pAddress);

  __asm__(
     "li           t0, 1;"         /* '1' is the locking value */
  "spin:"
     "amoswap.w.aq t0, t0, (a0);"  /* set '1' in the given address and set the previous value in t0 (atomic command) */
     "bnez         t0, spin;"      /* spin until t0=0 --> meaning the lock was free and now we locked it  */
     "ret;"                        /* lock is done */
    );
}

/**
* @brief - release the lock by parameter in a given memory address using atomic command
*
* @parameter - (a0) address in the memory with the parameter to set
*/
D_PSP_NO_INLINE D_PSP_TEXT_SECTION void pspAtomicsExitCriticalSection(volatile u32_t* pAddress)
{
  M_PSP_ASSERT(NULL != pAddress);

  __asm__(
     "amoswap.w.rl x0, x0, (a0);"  /* clear the lock */
     "ret;"
  );
}

/**
* @brief - AMO (Atomic Memory Operation) Swap command
*
* @parameter - (a0) address in the memory with the parameter to check and add
* @parameter - (a1) value to load to that address
*
* @return    - (a0) previous value in the address
*/
D_PSP_TEXT_SECTION u32_t pspAtomicsAmoSwap(u32_t* pAddress, u32_t uiValueToSwap)
{
  M_PSP_ASSERT(NULL != pAddress);

  __asm__(
     "amoswap.w a0, a1, (a0);"   /* put the value in a1 in the given address in a0 and set the previous content of that address in a0 (atomic command) */
  );
  return ((u32_t)pAddress);         /* return the previous value in the given address (pAddress = a0) */
}

/**
* @brief - AMO (Atomic Memory Operation) Add command
*
* @parameter - (a0) address in the memory with the parameter to check and add
* @parameter - (a1) value to add
*
* @return    - (a0) previous value in the address
*/
D_PSP_TEXT_SECTION u32_t pspAtomicsAmoAdd(u32_t* pAddress, u32_t uiValueToAdd)
{
  M_PSP_ASSERT(NULL != pAddress);

  __asm__(
     "amoadd.w a0, a1, (a0);"   /* add the value in a1 to the given address in a0 and set the previous content of that address in a0 (atomic command) */
  );
  return ((u32_t)pAddress);        /* return the previous value in the given address (pAddress = a0) */
}

/**
* @brief - AMO (Atomic Memory Operation) bitwise AND command
*
* @parameter - (a0) address in the memory with the parameter to check and do bitwise-AND with
* @parameter - (a1) value to do bitwise-AND with the content of the given memory
*
* @return    - (a0) previous value in the address
*/
D_PSP_TEXT_SECTION u32_t pspAtomicsAmoAnd(u32_t* pAddress, u32_t uiValueToAndWith)
{
  M_PSP_ASSERT(NULL != pAddress);

  __asm__(
     "amoand.w a0, a1, (a0);"   /* Do bitwise-AND with the value in a1 and the content given address in a0 and set the previous content of that address in a0 (atomic command) */
  );
  return ((u32_t)pAddress);        /* return the previous value in the given address (pAddress = a0) */
}

/**
* @brief - AMO (Atomic Memory Operation) bitwise OR command
*
* @parameter - (a0) address in the memory with the parameter to check and do bitwise-OR with
* @parameter - (a1) value to do bitwise-OR with the content of the given memory
*
* @return    - (a0) previous value in the address
*/
D_PSP_TEXT_SECTION u32_t pspAtomicsAmoOr(u32_t* pAddress, u32_t uiValueToOrWith)
{
  M_PSP_ASSERT(NULL != pAddress);

  __asm__(
     "amoor.w a0, a1, (a0);"   /* Do bitwise-OR with the value in a1 and the content given address in a0 and set the previous content of that address in a0 (atomic command) */
  );
  return ((u32_t)pAddress);       /* return the previous value in the given address (pAddress = a0) */
}

/**
* @brief - AMO (Atomic Memory Operation) bitwise XOR command
*
* @parameter - (a0) address in the memory with the parameter to check and do bitwise-XOR with
* @parameter - (a1) value to do bitwise-XOR with the content of the given memory
*
* @return    - (a0) previous value in the address
*/
D_PSP_TEXT_SECTION u32_t pspAtomicsAmoXor(u32_t* pAddress, u32_t uiValueToXorWith)
{
  M_PSP_ASSERT(NULL != pAddress);

  __asm__(
     "amoxor.w a0, a1, (a0);"   /* Do bitwise-XOR with the value in a1 and the content given address in a0 and set the previous content of that address in a0 (atomic command) */
  );
  return ((u32_t)pAddress);        /* return the previous value in the given address (pAddress = a0) */
}

/**
* @brief - AMO (Atomic Memory Operation) min command
*
* @parameter - (a0) address in the memory with the parameter to compare
* @parameter - (a1) value to compare with the content of the given memory, to determine the minimum
*
* @return    - (a0) previous value in the address
*/
D_PSP_TEXT_SECTION u32_t pspAtomicsAmoMin(u32_t* pAddress, u32_t uiValueToCompare)
{
  M_PSP_ASSERT(NULL != pAddress);

  __asm__(
     "amomin.w a0, a1, (a0);"   /* Compare contents of memory in a0 with the value in a1 and take the minimum of them. Set the previous content of the address in a0 (atomic command) */
  );
  return ((u32_t)pAddress);        /* return the previous value in the given address (pAddress = a0) */
}

/**
* @brief - AMO (Atomic Memory Operation) max command
*
* @parameter - (a0) address in the memory with the parameter to compare
* @parameter - (a1) value to compare with the content of the given memory, to determine the maximum
*
* @return    - (a0) previous value in the address
*/
D_PSP_TEXT_SECTION u32_t pspAtomicsAmoMax(u32_t* pAddress, u32_t uiValueToCompare)
{
  M_PSP_ASSERT(NULL != pAddress);

  __asm__(
     "amomax.w a0, a1, (a0);"   /* Compare contents of memory in a0 with the value in a1 and take the maximum of them. Set the previous content of the address in a0 (atomic command) */
  );
  return ((u32_t)pAddress);        /* return the previous value in the given address (pAddress = a0) */
}

/**
* @brief - AMO (Atomic Memory Operation) min command - unsigned
*
* @parameter - (a0) address in the memory with the parameter to compare (unsigned)
* @parameter - (a1) unsigned value to compare with the content of the given memory, to determine the minimum
*
* @return    - (a0) previous value in the address
*/
D_PSP_TEXT_SECTION u32_t pspAtomicsAmoMinUnsigned(u32_t* pAddress, u32_t uiValueToCompare)
{
  M_PSP_ASSERT(NULL != pAddress);

  __asm__(
     "amominu.w a0, a1, (a0);"   /* Unsigned-compare contents of memory in a0 with the value in a1 and take the minimum of them. Set the previous content of the address in a0 (atomic command) */
  );
  return ((u32_t)pAddress);         /* return the previous value in the given address (pAddress = a0) */
}

/**
* @brief - AMO (Atomic Memory Operation) max command - unsigned
*
* @parameter - (a0) address in the memory with the parameter to compare (unsigned)
* @parameter - (a1) unsigned value to compare with the content of the given memory, to determine the maximum
*
* @return    - (a0) previous value in the address
*/
D_PSP_TEXT_SECTION u32_t pspAtomicsAmoMaxUnsigned(u32_t* pAddress, u32_t uiValueToCompare)
{
  M_PSP_ASSERT(NULL != pAddress);

  __asm__(
     "amomaxu.w a0, a1, (a0);"   /* Unsigned-compare contents of memory in a0 with the value in a1 and take the maximum of them. Set the previous content of the address in a0 (atomic command) */
  );
  return ((u32_t)pAddress);         /* return the previous value in the given address (pAddress = a0) */
}
