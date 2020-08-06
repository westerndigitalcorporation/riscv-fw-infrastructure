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
* @file   psp_interrupts_hifive1.h
* @author Nati Rapaport
* @date   04.05.2020
* @brief  The file supplies information and registration API for interrupt and exception service routines on HiFive1 core.
*/
#ifndef __PSP_INTERRUPTS_HIFIVE1_H__
#define __PSP_INTERRUPTS_HIFIVE1_H__

/**
* include files
*/

/**
* types
*/
typedef enum pspInterruptCause
{
   E_USER_SOFTWARE_CAUSE             = 0,
   E_SUPERVISOR_SOFTWARE_CAUSE       = 1,
   E_RESERVED_SOFTWARE_CAUSE         = 2,
   E_MACHINE_SOFTWARE_CAUSE          = 3,
   E_USER_TIMER_CAUSE                = 4,
   E_SUPERVISOR_TIMER_CAUSE          = 5,
   E_RESERVED_TIMER_CAUSE            = 6,
   E_MACHINE_TIMER_CAUSE             = 7,
   E_USER_EXTERNAL_CAUSE             = 8,
   E_SUPERVISOR_EXTERNAL_CAUSE       = 9,
   E_RESERVED_EXTERNAL_CAUSE         = 10,
   E_MACHINE_EXTERNAL_CAUSE          = 11,
   E_LAST_CAUSE
} ePspInterruptCause_t;

/* Exceptions */
typedef enum pspExceptionCause
{
   E_EXC_INSTRUCTION_ADDRESS_MISALIGNED           = 0,
   E_EXC_INSTRUCTION_ACCESS_FAULT                 = 1,
   E_EXC_ILLEGAL_INSTRUCTION                      = 2,
   E_EXC_BREAKPOINT                               = 3,
   E_EXC_LOAD_EXC_ADDRESS_MISALIGNED              = 4,
   E_EXC_LOAD_EXC_ACCESS_FAULT                    = 5,
   E_EXC_STORE_AMO_ADDRESS_MISALIGNED             = 6,
   E_EXC_STORE_AMO_ACCESS_FAULT                   = 7,
   E_EXC_ENVIRONMENT_CALL_FROM_UMODE              = 8,
   E_EXC_ENVIRONMENT_CALL_FROM_SMODE              = 9,
   E_EXC_RESERVED                                 = 10,
   E_EXC_ENVIRONMENT_CALL_FROM_MMODE              = 11,
   E_EXC_INSTRUCTION_PAGE_FAULT                   = 12,
   E_EXC_LOAD_EXC_PAGE_FAULT                      = 13,
   E_EXC_RESERVEE_EXC_FOR_FUTURE_STANDARE_EXC_USE = 14,
   E_EXC_STORE_AMO_PAGE_FAULT                     = 15,
   E_EXC_LAST_CAUSE
} ePspExceptionCause_t;

/* interrupt handler definition */
typedef void (*pspInterruptHandler_t)(void);

/**
* definitions
*/
/* Enable/Disable bits of SW, Timer and External interrupts in Machine privilege level */
#define D_PSP_INTERRUPTS_MACHINE_SW        E_MACHINE_SOFTWARE_CAUSE
#define D_PSP_INTERRUPTS_MACHINE_TIMER     E_MACHINE_TIMER_CAUSE
#define D_PSP_INTERRUPTS_MACHINE_EXT       E_MACHINE_EXTERNAL_CAUSE
/* Enable/Disable bits of SW, Timer and External interrupts in Supervisor privilege level */
#define D_PSP_INTERRUPTS_SUPERVISOR_SW     E_SUPERVISOR_SOFTWARE_CAUSE
#define D_PSP_INTERRUPTS_SUPERVISOR_TIMER  E_SUPERVISOR_TIMER_CAUSE
#define D_PSP_INTERRUPTS_SUPERVISOR_EXT    E_SUPERVISOR_EXTERNAL_CAUSE
/* Enable/Disable bits of SW, Timer and External interrupts in User privilege level */
#define D_PSP_INTERRUPTS_USER_SW           E_USER_SOFTWARE_CAUSE
#define D_PSP_INTERRUPTS_USER_TIMER        E_USER_TIMER_CAUSE
#define D_PSP_INTERRUPTS_USER_EXT          E_USER_EXTERNAL_CAUSE

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
* @brief - The function installs an interrupt service routine per risc-v cause
*
* @parameter - fptrInterruptHandler     - function pointer to the interrupt service routine
* @parameter - interruptCause           - interrupt source
* @return    - u32_t                    - previously registered ISR
*/
pspInterruptHandler_t pspRegisterInterruptHandler(pspInterruptHandler_t fptrInterruptHandler, u32_t uiInterruptCause);

/**
* @brief - The function installs an exception handler per exception cause
*
* @parameter -  fptrInterruptHandler     - function pointer to the exception handler
* @parameter -  exceptionCause           - exception cause
* @return    -  u32_t                    - previously registered ISR
*/
pspInterruptHandler_t pspRegisterExceptionHandler(pspInterruptHandler_t fptrInterruptHandler, u32_t uiExceptionCause);

/**
* @brief - Set vector-table address at mtvec CSR
*
* @param - pointer to the vector table
*
* @return - none
*/
void pspInterruptsSetVectorTableAddress(void* pVectTable);

/**
* @brief - default empty interrupt handler
*
* @param - none
*
* @return - none
*/
void pspDefaultEmptyIntHandler_isr(void);

/**
* @brief - Disable interrupts and return the current interrupt state in each one of the privileged levels
*
* @output parameter - Current (== before the 'disable') interrupts state in each one of the privileged levels (read from mstatus CSR)
*
* @return - none
*/
void pspInterruptsDisable(u32_t  *pOutPrevIntState);

/**
* @brief - Restore the interrupts state in each one of the privileged levels.
*          i.e. if they were already disabled - they will stay disabled. If they were enabled - they will become enabled now.
*
* @input parameter - Previous interrupts state in each one of the privileged levels
*
* @return - none
*/
void pspInterruptsRestore(u32_t uiPrevIntState);

/**
* @brief - Enable interrupts (in all privilege levels) regardless their previous state
*
* @param - none
*
* @return - none
*/
void pspInterruptsEnable(void);


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
*
* @input parameter - Interrupt number to disable
*
* @return - none
*/
void pspDisableInterruptNumberMachineLevel(u32_t uiInterruptNumber);

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
*
* @input parameter - Interrupt number to enable
*
* @return - none
*/
void pspEnableInterruptNumberMachineLevel(u32_t uiInterruptNumber);

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
void pspDisableInterruptNumberUserLevel(u32_t uiInterruptNumber);

/**
* @brief - Enable specified interrupt when called in USER-LEVEL
*                                                     **********
* IMPORTANT NOTE: When you call this function, use ONLY one of the following defined values:
  **************  - D_PSP_INTERRUPTS_USER_SW
                  - D_PSP_INTERRUPTS_USER_TIMER
                  - D_PSP_INTERRUPTS_USER_EXT
*
* @input parameter - Interrupt number to enable
*
* @return - none
*/
void pspEnableInterruptNumberUserLevel(u32_t uiInterruptNumber);


#endif /* __PSP_INTERRUPTS_HIFIVE1_H__ */
