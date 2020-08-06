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
* @file   psp_nmi_eh1.c
* @author Nati Rapaport
* @date   13.04.2020
* @brief  The file contains default NMI (Non-maskable Interrupt) handlers and registration service (relevant to SweRV EH1)
*/


/**
* include files
*/
#include "psp_api.h"

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
* macros
*/

/**
* types
*/

/**
* local prototypes
*/
D_PSP_NO_RETURN void pspNmiPinAssertionDefaultHandler(void);    /* Default handler for pin-asserted NMI */
D_PSP_NO_RETURN void pspNmiDbusLoadErrorDefaultHandler(void);   /* Default handler for D-bus load error NMI */
D_PSP_NO_RETURN void pspNmiDbusStoreErrorDefaultHandler(void);  /* Default handler for D-bus store error NMI */

/**
* external prototypes
*/

/**
* global variables
*/
/* NMI handler pointers */
D_PSP_DATA_SECTION pspNmiHandler_t g_fptrNmiExtPinAssrtHandler    = pspNmiPinAssertionDefaultHandler;
D_PSP_DATA_SECTION pspNmiHandler_t g_fptrNmiDbusLoadErrHandler    = pspNmiDbusLoadErrorDefaultHandler;
D_PSP_DATA_SECTION pspNmiHandler_t g_fptrNmiDbusStoreErrHandler   = pspNmiDbusStoreErrorDefaultHandler;

/**
* functions
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
*
* @return u32_t      - previously registered ISR. If NULL then registration is erroneous.
*/
D_PSP_TEXT_SECTION pspNmiHandler_t pspNmiRegisterHandler(pspNmiHandler_t fptrNmiHandler, u32_t uiNmiCause)
{
  pspNmiHandler_t fptrNmiFunc;

  M_PSP_ASSERT((NULL != fptrNmiHandler) && ( (D_PSP_NMI_EXT_PIN_ASSERTION == uiNmiCause) || (D_PSP_NMI_D_BUS_STORE_ERROR == uiNmiCause)
            || (D_PSP_NMI_D_BUS_LOAD_ERROR == uiNmiCause) ) )

    switch (uiNmiCause)
  {
    case D_PSP_NMI_EXT_PIN_ASSERTION:
      fptrNmiFunc = g_fptrNmiExtPinAssrtHandler;
      g_fptrNmiExtPinAssrtHandler = fptrNmiHandler;
      break;
    case D_PSP_NMI_D_BUS_STORE_ERROR:
      fptrNmiFunc = g_fptrNmiDbusStoreErrHandler;
      g_fptrNmiDbusStoreErrHandler = fptrNmiHandler;
      break;
    case D_PSP_NMI_D_BUS_LOAD_ERROR:
      fptrNmiFunc = g_fptrNmiDbusLoadErrHandler;
      g_fptrNmiDbusLoadErrHandler = fptrNmiHandler;
      break;
    default:
      fptrNmiFunc = NULL;
      break;
  }

     return fptrNmiFunc;

}
/**
* @brief - Default handler for pin assertion NMI
*
*/
D_PSP_TEXT_SECTION D_PSP_NO_RETURN void pspNmiPinAssertionDefaultHandler(void)
{
  M_PSP_EBREAK();
  while(1);
}

/**
* @brief - Default handler for D-Bus load error NMI
*
*/
D_PSP_TEXT_SECTION D_PSP_NO_RETURN void pspNmiDbusLoadErrorDefaultHandler(void)
{
  M_PSP_EBREAK();
  while(1);
}

/**
* @brief - Default handler for D-Bus store error NMI
*
*/
D_PSP_TEXT_SECTION D_PSP_NO_RETURN void pspNmiDbusStoreErrorDefaultHandler(void)
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
    default:
      break;
  }

  /* No return from NMI handler. Loop here forever */
  while(1);
}


