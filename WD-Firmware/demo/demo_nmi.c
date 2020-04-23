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
* @file   demo_nmi.c
* @author Nati Rapaport
* @date   20.04.2020
* @brief  Demo application for NMI testing (using SweRVolf FPGA for SweRV EH1)
*/

/**
* include files
*/
#include "psp_api.h"
#include "demo_platform_al.h"
#include "timer.h"

/**
* definitions
*/
#define D_NUM_OF_ITERATIONS_IN_DELAY_LOOP 500000000  /* 500M iterations = 10 seconds (SweRVolf FGPA rate is 50Mhz) */

/**
* macros
*/

/**
* types
*/
typedef void (*funcPtr)(void);


/**
* local prototypes
*/
void demoNmiPinAssertionHandler(void); /* Pin-assertion-NMI handler for this test */

/**
* external prototypes
*/

/**
* global variables
*/
volatile u32_t g_uiJumpHereFromNmi;  /* Store the address to jump after NMI occurs in order to be able to return back to 'main' function */
volatile u32_t g_uiStackPointer;     /* Store the stack-pointer contents */

/**
* functions
*/

/**
 * @brief -  demoStart - startup point of the demo application. called from main function.
 *
 *  Flow of the test:
 *  1.  Set address of pspNmiHandlerSelector in nmi_vec
 *  2.  Register NMI handler for External-Pin-Assertion NMI
 *  3.  Set the timer to generate External-Pin-Assertion NMI upon expiration
 *  4.  Set 0.5 second duration in the timer
 *  5.  Let the timer to run (at its expiration - we expect to receive NMI)
 *  6.  Get current PC, add to it a delta of 0x30 bytes and save the result in a global parameter (g_uiJumpHereFromNmi)
 *  7.  Delay in a loop (10 seconds)
 *  8.  Upon NMI (after 0.5 second), the NMI handler is called
 *  9.  NMI handler does a jump to the address that stored in g_uiJumpHereFromNmi parameter
 *  10. Test completed successfully and function returns to 'main'
 *  11. If NMI did not occur, The 10 seconds loop expires and function breaks. Test failed.
 */
void demoStart(void)
{
	u32_t uiIterationCounter;

	/* Register the initial NMI handler in nmi_vec register */
	pspNmiSetVec(D_NMI_VEC_ADDRESSS, pspNmiHandlerSelector);

	/* Register External-Pin-Asserted NMI handler function */
	pspNmiRegisterHandler(demoNmiPinAssertionHandler, D_PSP_NMI_EXT_PIN_ASSERTION);

	/* Rout timer to NMI pin assertion - i.e. when the timer expires, an NMI will be asserted */
	bspRoutTimer(E_TIMER_TO_NMI);
	
	/* Initialize Timer (at its expiration, it will create an NMI) */
	bspSetTimerDurationMsec(500000);

	/* ** Store SP in a global parameter here **
	 * This is required here because when NMI handler is called, there might be
	 * 'push' operations on the stack at the function entrance. However, the NMI handler is not
	 * returning here, but instead we do a "jump" from the NMI handler back to here.
	 * (just to be able to return to 'main'. Normally, NMI does not return anywhere)
	 * That means there is no 'pop' from the stack at the NMI handler.
	 * So, in order to maintain reliable stack in our demo application, we save SP before triggering the NMI
	 * and we restore it at the NMI handler.
	 * This way, when this function returns to 'main', the SP is correct.
	 */
	asm volatile ("mv %0, sp" : "=r" (g_uiStackPointer)  : );

    /* Get the PC value, add 0x30 and store the result in g_uiJumpHereFromNmi
     * The result address is beyond the loop herein, and within the sequence of NOPs */
	asm volatile ("auipc %0, 0x0" : "=r" (g_uiJumpHereFromNmi)  : );
	g_uiJumpHereFromNmi += 0x30;

	/* Enable the timer to start running */
	bspStartTimer();

	/* Delay here in a loop */
	for(uiIterationCounter=0; uiIterationCounter<D_NUM_OF_ITERATIONS_IN_DELAY_LOOP; uiIterationCounter++)
	{
		M_PSP_NOP();
		M_PSP_NOP();
	}
	/* Arriving here means test failed, as the NMI should have been occurred already */
	M_PSP_EBREAK();
	/* Add some 'nop' instructions here to make sure the skip address is safe  */
	M_PSP_NOP();
	M_PSP_NOP();
	M_PSP_NOP();
	M_PSP_NOP();
	M_PSP_NOP();
	M_PSP_NOP();
	M_PSP_NOP();
	M_PSP_NOP();
	M_PSP_NOP();
	M_PSP_NOP();
	M_PSP_NOP();
	M_PSP_NOP();
	M_PSP_NOP();
	M_PSP_NOP();
	M_PSP_NOP();
	M_PSP_NOP();

	/* Arriving here means the test passed successfully */
	demoOutputMsg("** NMI test passed successfully **\n");

    return;
}


/**
 * @brief -  demoNmiPinAssertionHandler - NMI handler
 *
 */
void demoNmiPinAssertionHandler()
{
	/* Restore SP from the global parameter here */
	asm volatile ("mv sp, %0" : : "r" (g_uiStackPointer) );

	/* Jump to the address that stored in g_uiJumpHereFromNmi parameter */
	((funcPtr)g_uiJumpHereFromNmi)();
}


