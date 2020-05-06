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
* @file   psp_traps_interrupts.c
* @author Nati Rapaport
* @date   19.02.2020
* @brief  The file contains the psp common parts of traps and interrupts-related functions
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

/**
* external prototypes
*/

/**
* global variables
*/
/* The stack used by interrupt service routines */
#if (0 == D_ISR_STACK_SIZE)
    #error "ISR Stack size is not defined"
#else
	static /*D_PSP_DATA_SECTION*/ D_PSP_ALIGNED(16) pspStack_t udISRStack[ D_ISR_STACK_SIZE ] ;
	const pspStack_t xISRStackTop = ( pspStack_t ) &( udISRStack[ ( D_ISR_STACK_SIZE ) - 1 ] );
#endif

/**
* APIs
*/

/**
* @brief - Set vector-table address at mtvec CSR
*/
void pspInterruptsSetVectorTableAddress(void* pVectTable)
{
	u32_t uiInterruptsState;

	/* Disable interrupts */
	pspInterruptsDisable(&uiInterruptsState);

    M_PSP_WRITE_CSR(D_PSP_MTVEC_NUM, pVectTable);

    /* Restore interrupts */
    pspInterruptsRestore(uiInterruptsState);
}

/**
* @brief - Disable interrupts and return the current interrupt state in each one of the privileged levels
*
* output parameter - *pOutPrevIntState - Current (== before the 'disable') interrupts state in each one of the privileged levels (read from mstatus CSR)
*/
void pspInterruptsDisable(u32_t *pOutPrevIntState)
{
	/* Disable interrupts (all privilege levels). Status of mstatus CSR before the 'disable' is saved in pOutPrevIntState */
    M_PSP_CLEAR_AND_READ_CSR(*pOutPrevIntState, D_PSP_MSTATUS_NUM, (D_PSP_MSTATUS_UIE_MASK | D_PSP_MSTATUS_SIE_MASK | D_PSP_MSTATUS_MIE_MASK) );
}

/**
* @brief - Restore the interrupts state in each one of the privileged levels.
*          i.e. if they were already disabled - they will stay disabled. If they were enabled - they will become enabled now.
*
* input parameter - uiPrevIntState - Previous interrupts state in each one of the privileged levels
*/
void pspInterruptsRestore(u32_t uiPrevIntState)
{
	M_PSP_SET_CSR(D_PSP_MSTATUS_NUM, uiPrevIntState);
}

/**
* @brief - Enable interrupts (in all privilege levels) regardless their previous state
*
*/
void pspInterruptsEnable(void)
{
	M_PSP_SET_CSR(D_PSP_MSTATUS_NUM, (D_PSP_MSTATUS_UIE_MASK | D_PSP_MSTATUS_SIE_MASK | D_PSP_MSTATUS_MIE_MASK));
}



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
                  - other interrupts, specific to the core in use
******************************************************************
* input parameter - Interrupt number to disable
*/
void pspDisableInterruptNumberMachineLevel(u32_t uiInterruptNumber)
{
	M_PSP_CLEAR_CSR(D_PSP_MIE_NUM, M_PSP_BIT_MASK(uiInterruptNumber));
}



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
                  - other interrupts, specific to the core in use
******************************************************************
* input parameter - Interrupt number to enable
*/
void pspEnableInterruptNumberMachineLevel(u32_t uiInterruptNumber)
{
	M_PSP_SET_CSR(D_PSP_MIE_NUM, M_PSP_BIT_MASK(uiInterruptNumber));
}



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
void pspDisableInterruptNumberUserLevel(u32_t uiInterruptNumber)
{
	M_PSP_ASSERT((D_PSP_INTERRUPTS_USER_SW    == uiInterruptNumber)||
			     (D_PSP_INTERRUPTS_USER_TIMER == uiInterruptNumber)||
				 (D_PSP_INTERRUPTS_USER_EXT   == uiInterruptNumber));

	M_PSP_CLEAR_CSR(D_PSP_UIE_NUM, M_PSP_BIT_MASK(uiInterruptNumber));
}



/**************************************************************
* @brief - Enable specified interrupt when called in USER-LEVEL
*                                                    **********
* IMPORTANT NOTE: When you call this function, use ONLY one of the following defined values:
  **************  - D_PSP_INTERRUPTS_USER_SW
                  - D_PSP_INTERRUPTS_USER_TIMER
                  - D_PSP_INTERRUPTS_USER_EXT
******************************************************************
*
* input parameter - Interrupt number to enable
*/
void pspEnableInterruptNumberUserLevel(u32_t uiInterruptNumber)
{
	M_PSP_ASSERT((D_PSP_INTERRUPTS_USER_SW    == uiInterruptNumber)||
			     (D_PSP_INTERRUPTS_USER_TIMER == uiInterruptNumber)||
				 (D_PSP_INTERRUPTS_USER_EXT   == uiInterruptNumber));

	M_PSP_SET_CSR(D_PSP_UIE_NUM, M_PSP_BIT_MASK(uiInterruptNumber));
}







#if defined (__GNUC__) || defined (__clang__)

/**
*
* @brief Function that called upon unregistered Trap handler
*
***************************************************************************************************/
void pspTrapUnhandled(void)
{
	volatile u32_t uiLocalMepc, uiLocalMcause, uiLocalmtval;

	uiLocalMepc   = M_PSP_READ_CSR(D_PSP_MEPC_NUM);
	uiLocalMcause = M_PSP_READ_CSR(D_PSP_MCAUSE_NUM);
	uiLocalmtval =  M_PSP_READ_CSR(D_PSP_MTVAL_NUM);

	if (0 == uiLocalMepc || 0 == uiLocalMcause || 0 == uiLocalmtval)
	{}

	M_PSP_EBREAK();
}

#endif /* defined (__GNUC__) || defined (__clang__) */


