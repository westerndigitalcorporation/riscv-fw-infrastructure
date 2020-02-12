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
#define D_PSP_INTS_DISABLE_ENABLE_BALANCED 0

/**
* macros
*/

/* Enable/Disable specific interrupt */
/* __mie_interrupt_ is D_PSP_MIE_USIE .. D_PSP_MIE_MEIE as defined in psp_csrs.h */
#define M_PSP_ENABLE_INTERRUPT(__mie_interrupt__)  M_PSP_SET_CSR(mie, __mie_interrupt__);
#define M_PSP_DISBLE_INTERRUPT(__mie_interrupt__)  M_PSP_CLEAR_CSR(D_PSP_MIE, __mie_interrupt__);

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
extern s32_t gIntsDisableEnableBalance;

/**
* APIs
*/

/**
* Enable interrupts and set the gIntsDisableEnableBalance in "balanced" state
*/
static D_PSP_INLINE void pspInterruptsEnable(void)
{
	/* Enable interrupts */
	M_PSP_SET_CSR(D_PSP_MSTATUS, D_PSP_MSTATUS_MIE);
	/* Set disable/enable counter in "balanced" state */
	gIntsDisableEnableBalance = D_PSP_INTS_DISABLE_ENABLE_BALANCED;
}

/**
* Disable interrupts and decrement the gIntsDisableEnableBalance counter
*/
static D_PSP_INLINE void pspInterruptsDisable(void)
{
	/* Disable interrupts */
    M_PSP_CLEAR_CSR(D_PSP_MSTATUS, D_PSP_MSTATUS_MIE);
	/* Decrement the disable/enable counter */
	gIntsDisableEnableBalance--;
}

/**
* Increment gIntsDisableEnableBalance counter and only if it is in "balanced" state - enable interrupts
*/
static D_PSP_INLINE void pspInterruptsRestore(void)
{
	/* Increment the disable/enable counter. Only if it is "balanced" do interrupts-enable */
	if (++gIntsDisableEnableBalance == D_PSP_INTS_DISABLE_ENABLE_BALANCED)
	{
		M_PSP_SET_CSR(D_PSP_MSTATUS, D_PSP_MSTATUS_MIE);
	}
}

#endif /* __PSP_INTERRUPTS_H__ */
