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
* @file   psp_interrupts.h
* @author Ronen Haen
* @date   20.05.2019
* @brief  The file defines the psp interrupt interfaces
*/
#ifndef __PSP_INTERRUPTS_H__
#define __PSP_INTERRUPTS_H__

/**
* include files
*/

/**
* definitions
*/

/**
* macros
*/

/* Disable/Enable specific interrupt */
/* mie_interrupt is one of D_PSP_MIE_USIE .. D_PSP_MIE_MEIE as defined in psp_csrs.h */
#define M_PSP_M_DISBLE_INTERRUPT_ID(mie_interrupt)  u32_t uiCsrVal; \
	                                                M_PSP_CLEAR_AND_READ_CSR(uiCsrVal, D_PSP_MIE_NUM, mie_interrupt);
#define M_PSP_M_ENABLE_INTERRUPT_ID(mie_interrupt)  M_PSP_SET_CSR(D_PSP_MIE_NUM, mie_interrupt);

/* Disable Interrupts (all privilege levels) */
#define M_PSP_INTERRUPTS_DISABLE_IN_MACHINE_LEVEL(mask)  pspInterruptsDisable(mask)
/* Restore interrupts to their previous state */
#define M_PSP_INTERRUPTS_RESTORE_IN_MACHINE_LEVEL(mask)  pspInterruptsRestore(mask)
/* Enable interrupts regardless their previous state */
#define M_PSP_INTERRUPTS_ENABLE_IN_MACHINE_LEVEL()       pspInterruptsEnable()

/**
* types
*/

/* */
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
} pspInterruptCause_t;

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
} pspExceptionCause_t;

typedef enum pspExternIntHandlerPrivilege
{
   E_EXT_USER_INT_HNDLR       = E_USER_EXTERNAL_CAUSE,
   E_EXT_SUPERVISOR_INT_HNDLR = E_SUPERVISOR_EXTERNAL_CAUSE,
   E_EXT_MACHINE_INT_HNDLR    = E_MACHINE_EXTERNAL_CAUSE,
   E_EXT_INT_HNDLR_LAST
} pspExternIntHandlerPrivilege_t;

/* interrupt handler definition */
typedef void (*pspInterruptHandler_t)(void);

/**
* local prototypes
*/

/**
* external prototypes
*/

/**
* global variables
*/

/**
* APIs
*/

/**
* Disable interrupts and return the current (== before the 'disable') interrupt state
*/
void pspInterruptsDisable(u32_t  *pOutPrevIntState);

/**
* Restore the interrupts state
*/
void pspInterruptsRestore(u32_t uiPrevIntState);
/**
* Enable interrupts regardless their previous state
*/
void pspInterruptsEnable(void);




#endif /* __PSP_INTERRUPTS_H__ */
