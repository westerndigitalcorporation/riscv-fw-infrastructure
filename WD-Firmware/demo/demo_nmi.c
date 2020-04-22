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
extern void demoNmiPostTest(void);            /* Called at the end of the test to be able to return to caller */
extern void demoNmiPinAssertionHandler(void); /* Pin-assertion-NMI handler for this test */

/**
* global variables
*/
u32_t uiDelayAtPostTest = 1000000000; /* 1 Billion iterations = 20 seconds (SweRVolf FGPA rate is 5Mhz) */
u32_t uiJumpHereFromNmi;  /* Address to jump after NMI occurs in order to be able to return back to 'main' function */

/**
* functions
*/

/**
 * @brief -  demoStart - startup point of the demo application. called from main function.
 *
 *  Flow of the test:
 *  1. Set address of pspNmiHandlerSelector in nmi_vec
 *  2. Register NMI handler for External-Pin-Assertion NMI
 *  3. Set the timer to generate External-Pin-Assertion NMI upon expiration
 *  4. Set 0.5 second duration in the timer
 *  5. Let the timer to run (at its expiration - we expect to receive NMI)
 *  6. Call NmiPostTest assembly function, that does the following:
 *      -   Get current PC address into register
 *      -   Add to that register offset - to the end of the function
 *      -   Loop for 20 seconds
 *      -   If loop expires - test failed (as no NMI occurred)
 *      -   If NMI did occurred, the handler will instruct PC to jump to the offset address - from that address return here
 *  7. Return to main
 */
void demoStart(void)
{
	/* Register the initial NMI handler in nmi_vec register */
	pspNmiSetVec(D_NMI_VEC_ADDRESSS, pspNmiHandlerSelector);

	/* Register External-Pin-Asserted NMI handler function */
	pspNmiRegisterHandler(demoNmiPinAssertionHandler, D_PSP_NMI_EXT_PIN_ASSERTION);

	/* Rout timer to NMI pin assertion - i.e. when the timer expires, an NMI will be asserted */
	bspRoutTimer(E_TIMER_TO_NMI);
	
	/* Initialize Timer (at its expiration, it will create an NMI) */
	bspSetTimerDurationMsec(500000);

	/* Enable the timer to start running */
	bspStartTimer();

	/* Special handling to resolve the fact we cannot return from NMI handler, and yet be able to
	 * return to the 'main' function in order to maintain common test structure */
	demoNmiPostTest();

	/* Arriving here means the test passed successfully */
	demoOutputMsg("** NMI test passed successfully **\n");

    return;
}

