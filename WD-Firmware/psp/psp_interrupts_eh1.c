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
* @file   psp_interrupts_eh1.c
* @author Nati Rapaport
* @date   04.05.2020
* @brief  The file supplies registration API for interrupt and exception service routines on EH1 core.
*
*/

/**
* include files
*/
#include "psp_api.h"

/**
* definitions
*/

/**
* macros
*/

/**
* types
*/

/**
* local prototypes
*/
/* Default ISRs */
D_PSP_TEXT_SECTION void pspDefaultExceptionIntHandler_isr(void);
D_PSP_TEXT_SECTION void pspDefaultEmptyIntHandler_isr(void);

/**
* external prototypes
*/

/**
* global variables
*/
/* The stack used by interrupt service routines */
#if (0 == D_ISR_STACK_SIZE)
  #error "D_ISR_STACK_SIZE is not defined"
#else
  static /*D_PSP_DATA_SECTION*/ D_PSP_ALIGNED(16) pspStack_t udISRStack[ D_ISR_STACK_SIZE ] ;
  const pspStack_t xISRStackTop = ( pspStack_t ) &( udISRStack[ ( D_ISR_STACK_SIZE ) - 1 ] );
#endif

/* Exception handlers */
D_PSP_DATA_SECTION pspInterruptHandler_t  g_fptrExceptions_ints[D_PSP_NUM_OF_INTS_EXCEPTIONS] = {
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
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr };

/* Exceptions handler pointer */
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntExceptionIntHandler   = pspDefaultExceptionIntHandler_isr;

/* Interrupts handler pointers */
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntUSoftIntHandler       = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntSSoftIntHandler       = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntRsrvdSoftIntHandler   = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntMSoftIntHandler       = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntUTimerIntHandler      = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntSTimerIntHandler      = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntRsrvdTimerIntHandler  = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntMTimerIntHandler      = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntUExternIntHandler     = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntSExternIntHandler     = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntRsrvdExternIntHandler = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntMExternIntHandler     = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntMTimer0IntHandler     = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntMTimer1IntHandler     = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrCorrErrCntIntHandler     = pspDefaultEmptyIntHandler_isr;

/**
* APIs
*/

/**
* @brief - The function installs an interrupt service routine per risc-v cause
*
* @input parameter -  fptrInterruptHandler     - function pointer to the interrupt service routine
* @input parameter -  uiInterruptCause         - uiInterruptCause  – interrupt source
*
* @return u32_t                               - previously registered ISR. If NULL then registeration had an error
*/
D_PSP_TEXT_SECTION pspInterruptHandler_t pspRegisterInterruptHandler(pspInterruptHandler_t fptrInterruptHandler, u32_t uiInterruptCause)
{
   pspInterruptHandler_t fptrFunc;
   u32_t uiInterruptsState;

   M_PSP_ASSERT((NULL != fptrInterruptHandler) && (E_LAST_COMMON_CAUSE > uiInterruptCause) &&
            (E_FIRST_EH1_CAUSE < uiInterruptCause) && (E_LAST_EH1_CAUSE > uiInterruptCause));

   /* Disable interrupts */
   pspInterruptsDisable(&uiInterruptsState);

   switch (uiInterruptCause)
   {
      case E_USER_SOFTWARE_CAUSE:
        fptrFunc = g_fptrIntUSoftIntHandler;
        g_fptrIntUSoftIntHandler = fptrInterruptHandler;
        break;
      case E_SUPERVISOR_SOFTWARE_CAUSE:
        fptrFunc = g_fptrIntSSoftIntHandler;
        g_fptrIntSSoftIntHandler = fptrInterruptHandler;
        break;
      case E_RESERVED_SOFTWARE_CAUSE:
        fptrFunc = g_fptrIntRsrvdSoftIntHandler;
        g_fptrIntRsrvdSoftIntHandler = fptrInterruptHandler;
        break;
      case E_MACHINE_SOFTWARE_CAUSE:
        fptrFunc = g_fptrIntMSoftIntHandler;
        g_fptrIntMSoftIntHandler = fptrInterruptHandler;
        break;
      case E_USER_TIMER_CAUSE:
        fptrFunc = g_fptrIntUTimerIntHandler;
        g_fptrIntUTimerIntHandler = fptrInterruptHandler;
        break;
      case E_SUPERVISOR_TIMER_CAUSE:
        fptrFunc = g_fptrIntSTimerIntHandler;
        g_fptrIntSTimerIntHandler = fptrInterruptHandler;
        break;
      case E_RESERVED_TIMER_CAUSE:
        fptrFunc = g_fptrIntRsrvdTimerIntHandler;
        g_fptrIntRsrvdTimerIntHandler = fptrInterruptHandler;
        break;
      case E_MACHINE_TIMER_CAUSE:
        fptrFunc = g_fptrIntMTimerIntHandler;
        g_fptrIntMTimerIntHandler = fptrInterruptHandler;
        break;
      case E_USER_EXTERNAL_CAUSE:
        fptrFunc = g_fptrIntUExternIntHandler;
        g_fptrIntUExternIntHandler = fptrInterruptHandler;
        break;
      case E_SUPERVISOR_EXTERNAL_CAUSE:
        fptrFunc = g_fptrIntSExternIntHandler;
        g_fptrIntSExternIntHandler = fptrInterruptHandler;
        break;
      case E_RESERVED_EXTERNAL_CAUSE:
        fptrFunc = g_fptrIntRsrvdExternIntHandler;
        g_fptrIntRsrvdExternIntHandler = fptrInterruptHandler;
        break;
      case E_MACHINE_EXTERNAL_CAUSE:
        fptrFunc = g_fptrIntMExternIntHandler;
        g_fptrIntMExternIntHandler = fptrInterruptHandler;
        break;
      case E_MACHINE_INTERNAL_TIMER0_CAUSE:
        fptrFunc = g_fptrIntMTimer0IntHandler;
        g_fptrIntMTimer0IntHandler = fptrInterruptHandler;
        break;
      case E_MACHINE_INTERNAL_TIMER1_CAUSE:
        fptrFunc = g_fptrIntMTimer1IntHandler;
        g_fptrIntMTimer1IntHandler = fptrInterruptHandler;
        break;
      case E_MACHINE_CORRECTABLE_ERROR_CAUSE:
          fptrFunc = g_fptrCorrErrCntIntHandler;
          g_fptrCorrErrCntIntHandler = fptrInterruptHandler;
          break;
      default:
        fptrFunc = NULL;
        break;
   }

   /* Restore interrupts */
   pspInterruptsRestore(uiInterruptsState);

   return fptrFunc;
}

/**
* @brief - The function installs an exception handler per exception cause
*
* @input parameter -  fptrInterruptHandler     - function pointer to the exception handler
* @input parameter -  uiExceptionCause         - exception cause
*
* @return u32_t                               - previously registered ISR
*/
D_PSP_TEXT_SECTION pspInterruptHandler_t pspRegisterExceptionHandler(pspInterruptHandler_t fptrInterruptHandler, u32_t uiExceptionCause)
{
   pspInterruptHandler_t fptrFunc;

   M_PSP_ASSERT(fptrInterruptHandler != NULL && uiExceptionCause < E_EXC_LAST_CAUSE);

   fptrFunc = g_fptrExceptions_ints[uiExceptionCause];

   g_fptrExceptions_ints[uiExceptionCause] = fptrInterruptHandler;

   return fptrFunc;
}

/**
* @brief - default exception interrupt handler
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspDefaultExceptionIntHandler_isr(void)
{
   /* get the exception cause */
   u32_t uiCause = M_PSP_READ_CSR(D_PSP_MCAUSE_NUM);

   /* is it a valid cause */
   M_PSP_ASSERT(uiCause < D_PSP_NUM_OF_INTS_EXCEPTIONS);

   /* call the specific exception handler */
   g_fptrExceptions_ints[uiCause]();
}


/**
* @brief - default empty interrupt handler - used when no other handler is registered for the interrupt or exception
*          - Read mepc, mcause and mtval CSRs
*          - break
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspDefaultEmptyIntHandler_isr(void)
{
    volatile u32_t uiLocalMepc, uiLocalMcause, uiLocalmtval;

    uiLocalMepc   = M_PSP_READ_CSR(D_PSP_MEPC_NUM);
    uiLocalMcause = M_PSP_READ_CSR(D_PSP_MCAUSE_NUM);
    uiLocalmtval =  M_PSP_READ_CSR(D_PSP_MTVAL_NUM); /* Relevant for exceptions */

    if (0 == uiLocalMepc || 0 == uiLocalMcause || 0 == uiLocalmtval)
    {}

    M_PSP_EBREAK();
}

/**
* @brief - Set vector-table address at mtvec CSR
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspInterruptsSetVectorTableAddress(void* pVectTable)
{
  u32_t uiInterruptsState;

  /* Assert if vector-table address is NULL */
  M_PSP_ASSERT(NULL != pVectTable)

  /* Disable interrupts */
  pspInterruptsDisable(&uiInterruptsState);

  M_PSP_WRITE_CSR(D_PSP_MTVEC_NUM, pVectTable);

  /* Restore interrupts */
  pspInterruptsRestore(uiInterruptsState);
}

/**
* @brief - Disable interrupts and return the current interrupt state in each one of the privileged levels
*
* @output parameter - *pOutPrevIntState - Current (== before the 'disable') interrupts state in each one of the privileged levels (read from mstatus CSR)
*
* @return - none
*/
D_PSP_TEXT_SECTION void pspInterruptsDisable(u32_t *pOutPrevIntState)
{
  /* Disable interrupts (all privilege levels). Status of mstatus CSR before the 'disable' is saved in pOutPrevIntState */
  M_PSP_CLEAR_AND_READ_CSR(*pOutPrevIntState, D_PSP_MSTATUS_NUM, (D_PSP_MSTATUS_UIE_MASK | D_PSP_MSTATUS_SIE_MASK | D_PSP_MSTATUS_MIE_MASK) );
}

/**
* @brief - Restore the interrupts state in each one of the privileged levels.
*          i.e. if they were already disabled - they will stay disabled. If they were enabled - they will become enabled now.
*
* @input parameter - uiPrevIntState - Previous interrupts state in each one of the privileged levels
*
* @return - none
*/
D_PSP_TEXT_SECTION void pspInterruptsRestore(u32_t uiPrevIntState)
{
  M_PSP_SET_CSR(D_PSP_MSTATUS_NUM, uiPrevIntState);
}

/**
* @brief - Enable interrupts (in all privilege levels) regardless their previous state
*
* @param - none
*
* @return - none
*/
D_PSP_TEXT_SECTION void pspInterruptsEnable(void)
{
  M_PSP_SET_CSR(D_PSP_MSTATUS_NUM, (D_PSP_MSTATUS_UIE_MASK | D_PSP_MSTATUS_SIE_MASK | D_PSP_MSTATUS_MIE_MASK));
}



/**
* @brief - Disable specified interrupt when called in MACHINE-LEVEL
*                                                     *************
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values:
  *************** - D_PSP_INTERRUPTS_MACHINE_SW
                  - D_PSP_INTERRUPTS_MACHINE_TIMER
                  - D_PSP_INTERRUPTS_MACHINE_EXT
                  - D_PSP_INTERRUPTS_SUPERVISOR_SW
                  - D_PSP_INTERRUPTS_SUPERVISOR_TIMER
                  - D_PSP_INTERRUPTS_SUPERVISOR_EXT
                  - D_PSP_INTERRUPTS_USER_SW
                  - D_PSP_INTERRUPTS_USER_TIMER
                  - D_PSP_INTERRUPTS_USER_EXT
                  - D_PSP_INTERRUPTS_MACHINE_TIMER0
                  - D_PSP_INTERRUPTS_MACHINE_TIMER1
                  - D_PSP_INTERRUPTS_MACHINE_CORR_ERR_COUNTER
*
* @input parameter - Interrupt number to disable
*
* @return - none
*/
D_PSP_TEXT_SECTION void pspDisableInterruptNumberMachineLevel(u32_t uiInterruptNumber)
{
  M_PSP_CLEAR_CSR(D_PSP_MIE_NUM, M_PSP_BIT_MASK(uiInterruptNumber));
}



/**
* @brief - Enable specified interrupt when called in MACHINE-LEVEL
*                                                    *************
* IMPORTANT NOTE: When you call this function, you can use either one of the following defined values:
  *************** - D_PSP_INTERRUPTS_MACHINE_SW
                  - D_PSP_INTERRUPTS_MACHINE_TIMER
                  - D_PSP_INTERRUPTS_MACHINE_EXT
                  - D_PSP_INTERRUPTS_SUPERVISOR_SW
                  - D_PSP_INTERRUPTS_SUPERVISOR_TIMER
                  - D_PSP_INTERRUPTS_SUPERVISOR_EXT
                  - D_PSP_INTERRUPTS_USER_SW
                  - D_PSP_INTERRUPTS_USER_TIMER
                  - D_PSP_INTERRUPTS_USER_EXT
                  - D_PSP_INTERRUPTS_MACHINE_TIMER0
                  - D_PSP_INTERRUPTS_MACHINE_TIMER1
                  - D_PSP_INTERRUPTS_MACHINE_CORR_ERR_COUNTER
*
* @input parameter - Interrupt number to enable
*
* @return - none
*/
D_PSP_TEXT_SECTION void pspEnableInterruptNumberMachineLevel(u32_t uiInterruptNumber)
{
  M_PSP_SET_CSR(D_PSP_MIE_NUM, M_PSP_BIT_MASK(uiInterruptNumber));
}



/**
* @brief - Disable specified interrupt when called in USER-LEVEL
*                                                     **********
* IMPORTANT NOTE: When you call this function, use ONLY one of the following defined values:
  **************  - D_PSP_INTERRUPTS_USER_SW
                  - D_PSP_INTERRUPTS_USER_TIMER
                  - D_PSP_INTERRUPTS_USER_EXT
*
* @input parameter - Interrupt number to disable
*
* @return - none
*/
D_PSP_TEXT_SECTION void pspDisableInterruptNumberUserLevel(u32_t uiInterruptNumber)
{
  M_PSP_ASSERT((D_PSP_INTERRUPTS_USER_SW    == uiInterruptNumber)||
               (D_PSP_INTERRUPTS_USER_TIMER == uiInterruptNumber)||
               (D_PSP_INTERRUPTS_USER_EXT   == uiInterruptNumber));

  M_PSP_CLEAR_CSR(D_PSP_UIE_NUM, M_PSP_BIT_MASK(uiInterruptNumber));
}



/**
* @brief - Enable specified interrupt when called in USER-LEVEL
*                                                    **********
* IMPORTANT NOTE: When you call this function, use ONLY one of the following defined values:
  **************  - D_PSP_INTERRUPTS_USER_SW
                  - D_PSP_INTERRUPTS_USER_TIMER
                  - D_PSP_INTERRUPTS_USER_EXT
*
* @input parameter - Interrupt number to enable
*
* @return - none
*/
D_PSP_TEXT_SECTION void pspEnableInterruptNumberUserLevel(u32_t uiInterruptNumber)
{
  M_PSP_ASSERT((D_PSP_INTERRUPTS_USER_SW    == uiInterruptNumber)||
               (D_PSP_INTERRUPTS_USER_TIMER == uiInterruptNumber)||
               (D_PSP_INTERRUPTS_USER_EXT   == uiInterruptNumber));

  M_PSP_SET_CSR(D_PSP_UIE_NUM, M_PSP_BIT_MASK(uiInterruptNumber));
}
