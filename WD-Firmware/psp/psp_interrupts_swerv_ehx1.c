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
* @file   psp_interrupts_swerv_ehx1
* @author Nati Rapapoort
* @date   16.01.2020
* @brief  The file supplies interrupt services such as - registration, default handlers and indication whether we are in interrupt context.
*         The file is specific to swerv_ehx1 specifications
* 
*/

/**
* include files
*/
#include "psp_types.h"
#include "psp_api.h"
#include "psp_interrupt_api_ehx1.h"
/**
* definitions
*/

/* The stack used by interrupt service routines */
#ifdef D_ISR_STACK_SIZE
	static /*D_PSP_DATA_SECTION*/ D_PSP_16_ALIGNED pspStack_t xISRStack[ D_ISR_STACK_SIZE ] ;
	const pspStack_t xISRStackTop = ( pspStack_t ) &( xISRStack[ ( D_ISR_STACK_SIZE ) - 1 ] );
#else
    #error "ISR Stack size is not defined"
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
void pspDefaultExceptionIntHandler_isr(void);
void pspDefaultMSoftIntHandler_isr(void);
void pspDefaultMTimerIntHandler_isr(void);
void pspDefaultMExternIntHandler_isr(void);
void pspDefaultEmptyIntHandler_isr(void);
void pspDefaultMTimer0IntHandler_isr(void);
void pspDefaultMTimer1IntHandler_isr(void);

/**
* external prototypes
*/

/**
* global variables
*/
D_PSP_DATA_SECTION pspInterruptHandler_t  gExceptions_ints[D_PSP_NUM_OF_INTS_EXCEPTIONS] = {
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr };

D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntExceptionIntHandler   = pspDefaultExceptionIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntSSoftIntHandler       = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntRsrvdSoftIntHandler   = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntMSoftIntHandler       = pspDefaultMSoftIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntUTimerIntHandler      = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntSTimerIntHandler      = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntRsrvdTimerIntHandler  = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntMTimerIntHandler      = pspDefaultMTimerIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntUExternIntHandler     = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntSExternIntHandler     = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntRsrvdExternIntHandler = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntMExternIntHandler     = pspDefaultMExternIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntUSoftIntHandler       = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntMTimer0IntHandler     = pspDefaultMTimer0IntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntMTimer1IntHandler     = pspDefaultMTimer1IntHandler_isr;

/**
* The function installs an interrupt service routine per risc-v cause
*
* @param fptrInterruptHandler     – function pointer to the interrupt service routine
* @param eIntCause                – interrupt source
*
* @return u32_t                   - previously registered ISR
*/
D_PSP_TEXT_SECTION pspInterruptHandler_t pspRegisterInterruptHandler(pspInterruptHandler_t fptrInterruptHandler, u32_t uiInterruptCause)
{
   pspInterruptHandler_t pFptr;

   M_PSP_ASSERT(fptrInterruptHandler == NULL && uiInterruptCause >= E_LAST_COMMON_CAUSE &&
		        uiInterruptCause < D_PSP_FIRST_EHX1_INT_CAUSE && uiInterruptCause >= E_LAST_EHX1_CAUSE);

   switch (uiInterruptCause)
   {
      case E_USER_SOFTWARE_CAUSE:
    	  pFptr = g_fptrIntUSoftIntHandler;
    	  g_fptrIntUSoftIntHandler = fptrInterruptHandler;
    	  break;
      case E_SUPERVISOR_SOFTWARE_CAUSE:
    	  pFptr = g_fptrIntSSoftIntHandler;
    	  g_fptrIntSSoftIntHandler = fptrInterruptHandler;
          break;
      case E_RESERVED_SOFTWARE_CAUSE:
    	  pFptr = g_fptrIntRsrvdSoftIntHandler;
    	  g_fptrIntRsrvdSoftIntHandler = fptrInterruptHandler;
    	  break;
      case E_MACHINE_SOFTWARE_CAUSE:
    	  pFptr = g_fptrIntMSoftIntHandler;
    	  g_fptrIntMSoftIntHandler = fptrInterruptHandler;
          break;
      case E_USER_TIMER_CAUSE:
    	  pFptr = g_fptrIntUTimerIntHandler;
    	  g_fptrIntUTimerIntHandler = fptrInterruptHandler;
    	  break;
      case E_SUPERVISOR_TIMER_CAUSE:
    	  pFptr = g_fptrIntSTimerIntHandler;
    	  g_fptrIntSTimerIntHandler = fptrInterruptHandler;
    	  break;
      case E_RESERVED_TIMER_CAUSE:
    	  pFptr = g_fptrIntRsrvdTimerIntHandler;
    	  g_fptrIntRsrvdTimerIntHandler = fptrInterruptHandler;
    	  break;
      case E_MACHINE_TIMER_CAUSE:
    	  pFptr = g_fptrIntMTimerIntHandler;
    	  g_fptrIntMTimerIntHandler = fptrInterruptHandler;
    	  break;
      case E_USER_EXTERNAL_CAUSE:
    	  pFptr = g_fptrIntUExternIntHandler;
    	  g_fptrIntUExternIntHandler = fptrInterruptHandler;
          break;
      case E_SUPERVISOR_EXTERNAL_CAUSE:
    	  pFptr = g_fptrIntSExternIntHandler;
    	  g_fptrIntSExternIntHandler = fptrInterruptHandler;
          break;
      case E_RESERVED_EXTERNAL_CAUSE:
    	  pFptr = g_fptrIntRsrvdExternIntHandler;
    	  g_fptrIntRsrvdExternIntHandler = fptrInterruptHandler;
    	  break;
      case E_MACHINE_EXTERNAL_CAUSE:
    	  pFptr = g_fptrIntMExternIntHandler;
    	  g_fptrIntMExternIntHandler = fptrInterruptHandler;
    	  break;
      case E_MACHINE_INTERNAL_TIMER0_CAUSE:
	      pFptr = g_fptrIntMTimer0IntHandler;
	      g_fptrIntMTimer0IntHandler = fptrInterruptHandler;
	      break;
      case E_MACHINE_INTERNAL_TIMER1_CAUSE:
 	      pFptr = g_fptrIntMTimer1IntHandler;
 	      g_fptrIntMTimer1IntHandler = fptrInterruptHandler;
 	      break;
      default:
    	  pFptr = NULL;
    	  break;
   }

   return pFptr;
}

/**
* The function installs an exception handler per exception cause
*
* @param fptrInterruptHandler     – function pointer to the exception handler
* @param eExcCause                – exception cause
*
* @return u32_t                   - previously registered ISR
*/
D_PSP_TEXT_SECTION pspInterruptHandler_t pspRegisterExceptionHandler(pspInterruptHandler_t fptrInterruptHandler, u32_t uiExceptionCause)
{
   pspInterruptHandler_t pFptr;

   M_PSP_ASSERT(fptrInterruptHandler == NULL && uiExceptionCause >= E_EXC_LAST_COMMON);

   pFptr = gExceptions_ints[uiExceptionCause];

   gExceptions_ints[uiExceptionCause] = fptrInterruptHandler;

   return pFptr;
}

/**
* default exception interrupt handler
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspDefaultExceptionIntHandler_isr(void)
{
   /* get the exception cause */
   u32_t cause = M_PSP_READ_CSR(mcause);

   /* is it a valid cause */
   M_PSP_ASSERT(cause < D_PSP_NUM_OF_INTS_EXCEPTIONS);

   /* call the specific exception handler */
   gExceptions_ints[cause]();
}

/**
* default machine software interrupt handler
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspDefaultMSoftIntHandler_isr(void)
{
}

/**
* default machine timer interrupt handler
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspDefaultMTimerIntHandler_isr(void)
{
}

/**
* default machine external interrupt handler
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspDefaultMExternIntHandler_isr(void)
{
}

/**
* default empty interrupt handler
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspDefaultEmptyIntHandler_isr(void)
{
}

/**
* default internal timer0 interrupt handler
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspDefaultMTimer0IntHandler_isr(void)
{
}

/**
* default internal timer1 interrupt handler
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspDefaultMTimer1IntHandler_isr(void)
{
}
