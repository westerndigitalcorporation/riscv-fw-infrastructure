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

/**
* global variables
*/

/**
* functions
*/

/**
 * @brief - Handler of NMI in this demo
 *
 */
void demoNmiHandler(void)
{
	/* Arriving here means all tests passed successfully */
	demoOutputMsg("NMI test passed successfully\n");
}

/**
 * @brief -  demoStart - startup point of the demo application. called from main function.
 *
 */
void demoStart(void)
{
	/* Rout timer to NMI pin assertion - i.e. when the timer expires, an NMI will be asserted */
	bspRoutTimer(E_TIMER_TO_NMI);
	
	/* Initialize Timer (at its expiration, it will create an NMI) */
	bspSetTimerDurationMsec(1000000);

	/* Enable the timer */
	bspStartTimer();
}
