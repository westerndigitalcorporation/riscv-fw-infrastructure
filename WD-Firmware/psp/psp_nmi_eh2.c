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
* @file   psp_nmi_eh2.c
* @author Nati Rapaport
* @date   19.05.2020
* @brief  The file contains default NMI (Non-maskable Interrupt) handlers, registration services and interface for NMI features of SweRV EH2
*
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
/* Specified RAM address of nmi_vec */
#if (0 != D_NMI_VEC_ADDRESSS)
    #define D_PSP_NMI_VEC_ADDRESSS   D_NMI_VEC_ADDRESSS
#else
    #error "D_NMI_VEC_ADDRESSS is not defined"
#endif

/**
* local prototypes
*/
D_PSP_NO_RETURN void pspNmiDefaultHandler(void);          /* Default NMI handler */

/**
* external prototypes
*/

/**
* macros
*/

/**
* global variables
*/
/* NMI handler pointers */
D_PSP_DATA_SECTION pspNmiHandler_t g_fptrNmiExtPinAssrtHandler            = pspNmiDefaultHandler;
D_PSP_DATA_SECTION pspNmiHandler_t g_fptrNmiDbusLoadErrHandler            = pspNmiDefaultHandler;
D_PSP_DATA_SECTION pspNmiHandler_t g_fptrNmiDbusStoreErrHandler           = pspNmiDefaultHandler;
D_PSP_DATA_SECTION pspNmiHandler_t g_fptrNmiFastIntEccErrHandler          = pspNmiDefaultHandler;
D_PSP_DATA_SECTION pspNmiHandler_t g_fptrNmiFastIntDccmAccessErrHandler   = pspNmiDefaultHandler;
D_PSP_DATA_SECTION pspNmiHandler_t g_fptrNmiFastIntNonDccmErrHandler      = pspNmiDefaultHandler;

/**
* APIs
*/

/**
 * @brief - set address of NMI initial handler in nmi_vec
 *
 * @parameter - uiNmiVecAddress - address of NMI_VEC register
 * @parameter - fptrNmiSelector - address of NMI initial handler
 */
D_PSP_TEXT_SECTION void pspNmiSetVec(u32_t uiNmiVecAddress, pspNmiHandler_t fptrNmiSelector)
{
    M_PSP_WRITE_REGISTER_32(uiNmiVecAddress, (u32_t)fptrNmiSelector);
}

/**
* @brief - Register a Non-Maskable Interrupt (NMI) service routine
*
* input parameter -  fptrNmiHandler     - function pointer to the NMI service routine
* input parameter -  uiNmiCause         - NMI source
*                    Note that the value of this input parameter could be only one of these:
*                    - D_PSP_NMI_EXT_PIN_ASSERTION
*                    - D_PSP_NMI_D_BUS_STORE_ERROR
*                    - D_PSP_NMI_D_BUS_LOAD_ERROR
*                    - D_PSP_NMI_FAST_INT_DUOBLE_BIT_ECC_ERROR
*                    - D_PSP_NMI_FAST_INT_DCCM_ACCESS_ERROR
*                    - D_PSP_NMI_FAST_INT_NON_DCCM_REGION_ERROR
*
* @return u32_t      - previously registered ISR. If NULL then registration is erroneous.
*/
D_PSP_TEXT_SECTION pspNmiHandler_t pspNmiRegisterHandler(pspNmiHandler_t fptrNmiHandler, u32_t uiNmiCause)
{
  pspNmiHandler_t fptrNmiFunc;
  u32_t uiHartNumber;

  M_PSP_ASSERT((NULL != fptrNmiHandler) && ( (D_PSP_NMI_EXT_PIN_ASSERTION == uiNmiCause) || (D_PSP_NMI_D_BUS_STORE_ERROR == uiNmiCause)
            || (D_PSP_NMI_D_BUS_LOAD_ERROR == uiNmiCause) ) )

  switch (uiNmiCause)
  {
     case D_PSP_NMI_EXT_PIN_ASSERTION:
       fptrNmiFunc = g_fptrNmiExtPinAssrtHandler;
       g_fptrNmiExtPinAssrtHandler = fptrNmiHandler;
       /* Delegate pin-asserted NMI handling to the current Hart (HW thread) */
       uiHartNumber = M_PSP_GET_HART_ID();
       pspNmiSetDelegation(uiHartNumber);
       break;
     case D_PSP_NMI_D_BUS_STORE_ERROR:
       fptrNmiFunc = g_fptrNmiDbusStoreErrHandler;
       g_fptrNmiDbusStoreErrHandler = fptrNmiHandler;
       break;
     case D_PSP_NMI_D_BUS_LOAD_ERROR:
       fptrNmiFunc = g_fptrNmiDbusLoadErrHandler;
       g_fptrNmiDbusLoadErrHandler = fptrNmiHandler;
       break;
     case D_PSP_NMI_FAST_INT_DUOBLE_BIT_ECC_ERROR:
       fptrNmiFunc = g_fptrNmiFastIntEccErrHandler;
       g_fptrNmiFastIntEccErrHandler = fptrNmiHandler;
       break;
     case D_PSP_NMI_FAST_INT_DCCM_ACCESS_ERROR:
       fptrNmiFunc = g_fptrNmiFastIntDccmAccessErrHandler;
       g_fptrNmiFastIntDccmAccessErrHandler = fptrNmiHandler;
       break;
     case D_PSP_NMI_FAST_INT_NON_DCCM_REGION_ERROR:
       fptrNmiFunc = g_fptrNmiFastIntNonDccmErrHandler;
       g_fptrNmiFastIntNonDccmErrHandler = fptrNmiHandler;
       break;
     default:
       fptrNmiFunc = NULL;
       break;
  }

     return fptrNmiFunc;

}

/**
* @brief - Default NMI handler
*
*/
D_PSP_TEXT_SECTION D_PSP_NO_RETURN void pspNmiDefaultHandler(void)
{
  M_PSP_EBREAK();
  while(1);
}

/**
* @brief - This function is called upon NMI and selects the appropriate handler
*
*/
D_PSP_NO_RETURN D_PSP_TEXT_SECTION  void pspNmiHandlerSelector(void)
{
  u32_t uiNmiCode;

  /* Select the correct handler according mcause CSR contents */
  uiNmiCode = M_PSP_READ_CSR(D_PSP_MCAUSE_NUM);

  switch (uiNmiCode)
  {
    case D_PSP_NMI_EXT_PIN_ASSERTION:
      g_fptrNmiExtPinAssrtHandler();
      break;
    case D_PSP_NMI_D_BUS_LOAD_ERROR:
      g_fptrNmiDbusLoadErrHandler();
      break;
    case D_PSP_NMI_D_BUS_STORE_ERROR:
      g_fptrNmiDbusStoreErrHandler();
      break;
    case D_PSP_NMI_FAST_INT_DUOBLE_BIT_ECC_ERROR:
      g_fptrNmiFastIntEccErrHandler();
      break;
    case D_PSP_NMI_FAST_INT_DCCM_ACCESS_ERROR:
      g_fptrNmiFastIntDccmAccessErrHandler();
      break;
    case D_PSP_NMI_FAST_INT_NON_DCCM_REGION_ERROR:
      g_fptrNmiFastIntNonDccmErrHandler();
      break;
    default:
      break;
  }

  /* No return from NMI handler. Loop here forever */
  while(1);
}

/**
 * @brief - delegate pin-asserted NMI to a given Hart (HW thread)
 *          That means - upon occurence of the pin-asserted NMI, it will be handelled by the given Hart    
 *
 * @parameter - Hart number to delegate the NMI to
 */
D_PSP_TEXT_SECTION void pspNmiSetDelegation(u32_t uiHartNumber)
{
  /* Assert on Hart number */
  M_PSP_ASSERT(E_LAST_HART > uiHartNumber);

  /* As this CSR is common to all harts, make sure that it will not be accessed simultaneously by more than single hart */
  M_PSP_INTERNAL_MUTEX_LOCK(E_MUTEX_INTERNAL_FOR_NMI_DELEGATION);

  /* Delegate NMI handling to the given Hart number */
  M_PSP_SET_CSR(D_PSP_MNMIPDEL_NUM, 1<<uiHartNumber);

  /* Remove the multi-harts access protection */
  M_PSP_INTERNAL_MUTEX_UNLOCK(E_MUTEX_INTERNAL_FOR_NMI_DELEGATION);
}

/**
 * @brief - clear delegation of pin-asserted NMI for a given Hart (HW thread)
 *          That means - upon occurence of the pin-asserted NMI, the given Hart will not handle the NMI    
 *
 * @parameter - Hart number to clear NMI delegation from
 */
D_PSP_TEXT_SECTION void pspNmiClearDelegation(u32_t uiHartNumber)
{
  /* Assert on Hart number */
  M_PSP_ASSERT(E_LAST_HART > uiHartNumber);

  /* As this CSR is common to all harts, make sure that it will not be accessed simultaneously by more than single hart */
  M_PSP_INTERNAL_MUTEX_LOCK(E_MUTEX_INTERNAL_FOR_NMI_DELEGATION);

  /* Clear delegation of NMI handling from the given Hart number */
  M_PSP_CLEAR_CSR(D_PSP_MNMIPDEL_NUM, 1<<uiHartNumber);

  /* Remove the multi-harts access protection */
  M_PSP_INTERNAL_MUTEX_UNLOCK(E_MUTEX_INTERNAL_FOR_NMI_DELEGATION);
}

/**
 * @brief - check whether pin-asserted NMI handling is delegated to the given hart (HW thread) or not
 *
 * @parameter - Hart number
 * @return    - 0/1 to indicate whether the NMI handling is delegated to the given hart-number or not
 */
D_PSP_TEXT_SECTION u32_t pspIsNmiDelegatedToHart(u32_t uiHartNumber)
{
  u32_t uiIsDelegated = 0;

  /* Assert on Hart number */
  M_PSP_ASSERT(E_LAST_HART > uiHartNumber);

  uiIsDelegated = !!(M_PSP_READ_CSR(D_PSP_MNMIPDEL_NUM) & (uiHartNumber+1));

  return (uiIsDelegated);
}

