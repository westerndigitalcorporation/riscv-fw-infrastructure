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
* @file   psp_traps_interrupts.h
* @author Ronen Haen
* @date   20.05.2019
* @brief  The file defines the psp traps & interrupt interfaces
*/
#ifndef __PSP_TRAPS_INTERRUPTS_H__
#define __PSP_TRAPS_INTERRUPTS_H__

/**
* include files
*/

/* Interrupt context indication values */
#define D_PSP_NON_INT_CONTEXT              0
#define D_PSP_INT_CONTEXT                  1


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
   E_LAST_COMMON_CAUSE
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
   E_EXC_LAST_COMMON
} ePspExceptionCause_t;

typedef enum pspExternIntHandlerPrivilege
{
   E_EXT_USER_INT_HNDLR       = E_USER_EXTERNAL_CAUSE,
   E_EXT_SUPERVISOR_INT_HNDLR = E_SUPERVISOR_EXTERNAL_CAUSE,
   E_EXT_MACHINE_INT_HNDLR    = E_MACHINE_EXTERNAL_CAUSE,
   E_EXT_INT_HNDLR_LAST
} ePspExternIntHandlerPrivilege_t;


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

/* Disable all Interrupts (in all privilege levels) in mstatus CSR */
#define M_PSP_INTERRUPTS_DISABLE_IN_MACHINE_LEVEL(pMask) pspInterruptsDisable(pMask);
/* Restore interrupts to their previous state */
#define M_PSP_INTERRUPTS_RESTORE_IN_MACHINE_LEVEL(mask)  pspInterruptsRestore(mask);
/* Enable all interrupts regardless their previous state */
#define M_PSP_INTERRUPTS_ENABLE_IN_MACHINE_LEVEL()       pspInterruptsEnable();


/**
* global variables
*/

/**
* APIs
*/

/**
* @brief - Set vector-table address at mtvec CSR
*/
void pspInterruptsSetVectorTableAddress(void* pVectTable);


/**
* @brief - The function installs an interrupt service routine per risc-v cause
*
* input parameter  fptrInterruptHandler     - function pointer to the interrupt service routine
* input parameter  interruptCause           - interrupt source
* return u32_t                              - previously registered ISR
*/
pspInterruptHandler_t pspRegisterInterruptHandler(pspInterruptHandler_t fptrInterruptHandler, u32_t uiInterruptCause);


/**
* @brief - The function installs an exception handler per exception cause
*
* input parameter fptrInterruptHandler     - function pointer to the exception handler
* input parameter exceptionCause           - exception cause
* return u32_t                             - previously registered ISR
*/
pspInterruptHandler_t pspRegisterExceptionHandler(pspInterruptHandler_t fptrInterruptHandler, u32_t uiExceptionCause);


/**
* @brief - default empty interrupt handler
*/
void pspDefaultEmptyIntHandler_isr(void);


/**
* @brief - Function that called upon unregistered Trap handler
*/
void pspTrapUnhandled(void);


/**
* @brief - Disable interrupts and return the current interrupt state in each one of the privileged levels
*
* output parameter - Current (== before the 'disable') interrupts state in each one of the privileged levels (read from mstatus CSR)
*/
void pspInterruptsDisable(u32_t  *pOutPrevIntState);


/**
* @brief - Restore the interrupts state in each one of the privileged levels.
*          i.e. if they were already disabled - they will stay disabled. If they were enabled - they will become enabled now.
*
* input parameter - Previous interrupts state in each one of the privileged levels
*/
void pspInterruptsRestore(u32_t uiPrevIntState);


/**
* @brief - Enable interrupts (in all privilege levels) regardless their previous state
*/
void pspInterruptsEnable(void);



/******************************************************************
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
******************************************************************
* input parameter - Interrupt number to disable
*/
void pspDisableInterruptNumberMachineLevel(u32_t uiInterruptNumber);



/*****************************************************************
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
******************************************************************
* input parameter - Interrupt number to enable
*/
void pspEnableInterruptNumberMachineLevel(u32_t uiInterruptNumber);



/***************************************************************
* @brief - Disable specified interrupt when called in USER-LEVEL
*                                                     **********
* IMPORTANT NOTE: When you call this function, use ONLY one of the following defined values:
  **************  - D_PSP_INTERRUPTS_USER_SW
                  - D_PSP_INTERRUPTS_USER_TIMER
                  - D_PSP_INTERRUPTS_USER_EXT
******************************************************************
*
* input parameter - Interrupt number to disable
*/
void pspDisableInterruptNumberUserLevel(u32_t uiInterruptNumber);



/***************************************************************
* @brief - Enable specified interrupt when called in USER-LEVEL
*                                                     **********
* IMPORTANT NOTE: When you call this function, use ONLY one of the following defined values:
  **************  - D_PSP_INTERRUPTS_USER_SW
                  - D_PSP_INTERRUPTS_USER_TIMER
                  - D_PSP_INTERRUPTS_USER_EXT
******************************************************************
*
* input parameter - Interrupt number to enable
*/
void pspEnableInterruptNumberUserLevel(u32_t uiInterruptNumber);




#endif /* __PSP_TRAPS_INTERRUPTS_H__ */
