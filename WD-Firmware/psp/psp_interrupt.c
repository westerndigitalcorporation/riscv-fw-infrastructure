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
* @file   psp_interrupt.c
* @author Ronen Haen
* @date   20.05.2019
* @brief  The file implements the PSP interrupt API
* 
*/

/**
* include files
*/
#include "psp_api.h"
#include "psp_interrupt_api.h"

/**
* definitions
*/

/**
* macros
*/

/**
* types
*/
u32_t g_uiIsInterruptContext = D_PSP_NON_INT_CONTEXT;

/**
* local prototypes
*/
void pspDefaultExceptionIntHandler(void);
void pspDefaultMSoftIntHandler(void);
void pspDefaultMTimerIntHandler(void);
void pspDefaultMExternIntHandler(void);
void pspDefaultEmptyIntHandler(void);

/**
* external prototypes
*/

/**
* global variables
*/

pspInterruptHandler_t  exceptions_ints[D_PSP_NUM_OF_INTS_EXCEPTIONS] = {
               pspDefaultEmptyIntHandler,
               pspDefaultEmptyIntHandler,
               pspDefaultEmptyIntHandler,
               pspDefaultEmptyIntHandler,
               pspDefaultEmptyIntHandler,
               pspDefaultEmptyIntHandler,
               pspDefaultEmptyIntHandler,
               pspDefaultEmptyIntHandler,
               pspDefaultEmptyIntHandler,
               pspDefaultEmptyIntHandler,
               pspDefaultEmptyIntHandler,
               pspDefaultEmptyIntHandler,
               pspDefaultEmptyIntHandler,
               pspDefaultEmptyIntHandler,
               pspDefaultEmptyIntHandler };

pspInterruptHandler_t fptrIntExceptionIntHandler = pspDefaultExceptionIntHandler;
pspInterruptHandler_t fptrIntSSoftIntHandler = pspDefaultEmptyIntHandler;
pspInterruptHandler_t fptrIntRsrvdSoftIntHandler = pspDefaultEmptyIntHandler;
pspInterruptHandler_t fptrIntMSoftIntHandler = pspDefaultMSoftIntHandler;
pspInterruptHandler_t fptrIntUTimerIntHandler = pspDefaultEmptyIntHandler;
pspInterruptHandler_t fptrIntSTimerIntHandler = pspDefaultEmptyIntHandler;
pspInterruptHandler_t fptrIntRsrvdTimerIntHandler = pspDefaultEmptyIntHandler;
pspInterruptHandler_t fptrIntMTimerIntHandler = pspDefaultMTimerIntHandler;
pspInterruptHandler_t fptrIntUExternIntHandler = pspDefaultEmptyIntHandler;
pspInterruptHandler_t fptrIntSExternIntHandler = pspDefaultEmptyIntHandler;
pspInterruptHandler_t fptrIntRsrvdExternIntHandler = pspDefaultEmptyIntHandler;
pspInterruptHandler_t fptrIntMExternIntHandler = pspDefaultMExternIntHandler;
pspInterruptHandler_t fptrIntUSoftIntHandler = pspDefaultEmptyIntHandler;

/**
* The function installs an interrupt service routine per risc-v cause
*
* @param fptrInterruptHandler     – function pointer to the interrupt service routine
* @param eIntCause                – interrupt source
*
* @return u32_t                   - D_PSP_SUCCESS
*                                 - D_PSP_PTR_ERROR - provided invalid function pointer
*                                 - D_PSP_FAIL - unsupported value in eIntCause
*/
PSP_SECTION u32_t pspRegisterIsrCauseHandler(pspInterruptHandler_t fptrInterruptHandler,
		                                     pspInterruptCause_t eIntCause)
{
   M_PSP_VALIDATE_FUNC_PARAM(fptrInterruptHandler, fptrInterruptHandler == NULL,
		                        D_PSP_PTR_ERROR);
   M_PSP_VALIDATE_FUNC_PARAM(&eIntCause, eIntCause >= E_LAST_CAUSE, D_PSP_FAIL);

   switch (eIntCause)
   {
      case E_USER_SOFTWARE_CAUSE:
    	  fptrIntUSoftIntHandler = fptrInterruptHandler;
    	  break;
      case E_SUPERVISOR_SOFTWARE_CAUSE:
    	  fptrIntSSoftIntHandler = fptrInterruptHandler;
          break;
      case E_RESERVED_SOFTWARE_CAUSE:
    	  fptrIntRsrvdSoftIntHandler = fptrInterruptHandler;
    	  break;
      case E_MACHINE_SOFTWARE_CAUSE:
    	  fptrIntMSoftIntHandler = fptrInterruptHandler;
          break;
      case E_USER_TIMER_CAUSE:
    	  fptrIntUTimerIntHandler = fptrInterruptHandler;
    	  break;
      case E_SUPERVISOR_TIMER_CAUSE:
    	  fptrIntSTimerIntHandler = fptrInterruptHandler;
    	  break;
      case E_RESERVED_TIMER_CAUSE:
    	  fptrIntRsrvdTimerIntHandler = fptrInterruptHandler;
    	  break;
      case E_MACHINE_TIMER_CAUSE:
    	  fptrIntMTimerIntHandler = fptrInterruptHandler;
    	  break;
      case E_USER_EXTERNAL_CAUSE:
    	  fptrIntUExternIntHandler = fptrInterruptHandler;
          break;
      case E_SUPERVISOR_EXTERNAL_CAUSE:
    	  fptrIntSExternIntHandler = fptrInterruptHandler;
          break;
      case E_RESERVED_EXTERNAL_CAUSE:
    	  fptrIntRsrvdExternIntHandler = fptrInterruptHandler;
    	  break;
      case E_MACHINE_EXTERNAL_CAUSE:
    	  fptrIntMExternIntHandler = fptrInterruptHandler;
    	  break;
      default:
    	  return D_PSP_FAIL;
   }

   return D_PSP_SUCCESS;
}

/**
* The function installs an exception handler per exception cause
*
* @param fptrInterruptHandler     – function pointer to the exception handler
* @param eExcCause                – exception cause
*
* @return u32_t                   - D_PSP_SUCCESS
*                                 - D_PSP_PTR_ERROR - provided invalid function pointer
*                                 - D_PSP_FAIL - unsupported value in eIntCause
*/
PSP_SECTION u32_t pspRegisterIsrExceptionHandler(pspInterruptHandler_t fptrInterruptHandler,
		                                         pspExceptionCause_t eExcCause)
{
   M_PSP_VALIDATE_FUNC_PARAM(fptrInterruptHandler, fptrInterruptHandler == NULL, D_PSP_PTR_ERROR);
   M_PSP_VALIDATE_FUNC_PARAM(&eExcCause, eExcCause >= E_EXC_LAST, D_PSP_FAIL);

   exceptions_ints[eExcCause] = fptrInterruptHandler;

   return D_PSP_SUCCESS;
}

/**
* default exception interrupt handler
*
* @param none
*
* @return none
*/
PSP_SECTION void pspDefaultExceptionIntHandler(void)
{
   /* get the exception cause */
   u32_t cause = M_PSP_READ_CSR(mcause);

   /* is it a valid cause */
   if (cause < D_PSP_NUM_OF_INTS_EXCEPTIONS)
   {
      /* call the specific exception handler */
      exceptions_ints[cause]();
   }
   /* Although cause not valid we still need to do something */
   else
   {
      pspDefaultEmptyIntHandler();
   }
}

/**
* default machine software interrupt handler
*
* @param none
*
* @return none
*/
PSP_SECTION void pspDefaultMSoftIntHandler(void)
{
}

/**
* default machine timer interrupt handler
*
* @param none
*
* @return none
*/
PSP_SECTION void pspDefaultMTimerIntHandler(void)
{
}

/**
* default machine external interrupt handler
*
* @param none
*
* @return none
*/
PSP_SECTION void pspDefaultMExternIntHandler(void)
{
}

/**
* default empty interrupt handler
*
* @param none
*
* @return none
*/
PSP_SECTION void pspDefaultEmptyIntHandler(void)
{
}

/**
* @brief check if in ISR context
*
* @param None
*
* @return u32_t            - D_NON_INT_CONTEXT
*                          - non zero value - interrupt context
*/
u32_t pspIsInterruptContext(void)
{
   return (g_uiIsInterruptContext > 0);
}
