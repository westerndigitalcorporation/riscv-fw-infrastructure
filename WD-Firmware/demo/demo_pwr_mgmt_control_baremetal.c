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
* include files
*/
#include "psp_api.h"
#include "demo_platform_al.h"


/**
* definitions
*/
#define D_BEFORE_HALT 	 (0)
#define D_AFTER_HALT  	 (1)
#define D_IN_MTIMER_ISR  (2)
#define D_MTIMER_WAKEUP_TEST_RESULT  (M_PSP_BIT_MASK(D_BEFORE_HALT) | M_PSP_BIT_MASK(D_AFTER_HALT) | M_PSP_BIT_MASK(D_IN_MTIMER_ISR))
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
extern void psp_vect_table(void);
/**
* global variables
*/
u32_t gTestWayPoints = 0;

/**
* functions
*/

/**
 * demoMtimerIsrHandler - Handle machine timer ISR
 *
 */
D_PSP_TEXT_SECTION void demoMtimerIsrHandler(void)
{
	/* Disable Machine-Timer interrupt */
	pspDisableInterruptNumberMachineLevel(E_MACHINE_TIMER_CAUSE);

	/* Mark that ISR visited */
	gTestWayPoints |= M_PSP_BIT_MASK(D_IN_MTIMER_ISR);
}

/**
 * demoMtimerWakeupTest - verify that core wakes up upon machine timer interrupt
 *
 */
void demoMtimerWakeupTest(void)
{
	/* register trap handler */
	M_PSP_WRITE_CSR(D_PSP_MTVEC_NUM, &psp_vect_table);

	/* register Machine timer timer interrupt handler */
	pspRegisterInterruptHandler(demoMtimerIsrHandler, E_MACHINE_TIMER_CAUSE);

	/* Enable machine timer interrupt */
	pspEnableInterruptNumberMachineLevel(E_MACHINE_TIMER_CAUSE);

	/* Activates Core's timer with the calculated period */
	M_PSP_TIMER_COUNTER_ACTIVATE(D_PSP_CORE_TIMER, 200000);

	/* Enable all machine level interrupts */
	M_PSP_INTERRUPTS_ENABLE_IN_MACHINE_LEVEL();

	gTestWayPoints |= M_PSP_BIT_MASK(D_BEFORE_HALT);

	/* Halt the core */
	pspPmcHalt();

	gTestWayPoints |= M_PSP_BIT_MASK(D_AFTER_HALT);

	if(gTestWayPoints != D_MTIMER_WAKEUP_TEST_RESULT)
	{
		M_ENDLESS_LOOP();
	}
}

/**
 * demoStart - startup point of the demo application. called from main function.
 *
 */
void demoStart(void)
{

	demoMtimerWakeupTest();

	/* Arriving here means all tests passed successfully */
	demoOutputMsg("Power Management Control tests passed successfully\n");

	/* Loop here to let debug */
	M_ENDLESS_LOOP();

}

