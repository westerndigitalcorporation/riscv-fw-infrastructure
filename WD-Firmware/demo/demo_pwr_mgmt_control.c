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
* @file   demo_pwr_mgmt_control.c
* @author Alexander Dvoskin
* @date   26.03.2020
* @brief  Demo application for power-management unit. Tests that core is set to 'Halt' mode and then wakeup upon MTIME and EXTERNAL interrupts
*
*/

/**
* include files
*/
#include "psp_api.h"
#include "demo_platform_al.h"
#include "demo_utils.h"
#include "external_interrupts.h"
#include "timer.h"

/**
* definitions
*/
#define D_BEFORE_HALT 	 (0)
#define D_AFTER_HALT  	 (1)
#define D_IN_MTIMER_ISR  (2)
#define D_IN_EXT_INT_ISR (3)
#define D_MTIMER_WAKEUP_TEST_RESULT    (M_PSP_BIT_MASK(D_BEFORE_HALT) | M_PSP_BIT_MASK(D_AFTER_HALT) | M_PSP_BIT_MASK(D_IN_MTIMER_ISR))
#define D_EXT_INT_WAKEUP_TEST_RESULT   (M_PSP_BIT_MASK(D_BEFORE_HALT) | M_PSP_BIT_MASK(D_AFTER_HALT) | M_PSP_BIT_MASK(D_IN_EXT_INT_ISR))

#define D_HALT_TIME  (10 * (D_CLOCK_RATE / D_PSP_MSEC))/*number of clock cycles equivalent to 10 msec*/
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
u32_t g_uiTestWayPoints;

/**
* functions
*/

/**
 * @brief - demoExternalInterruptIsr - Handle External interrupt ISR
 *
 */
void demoExternalInterruptIsr(void)
{
	/* Disable External-interrupts */
	pspDisableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_EXT);

	/* Mark that ISR visited */
	g_uiTestWayPoints |= M_PSP_BIT_MASK(D_IN_EXT_INT_ISR);
}

/**
 * @brief - demoSetupExternalInterrupts - Initialize and setup external-interrupt (IRQ3)
 *
 */
void demoSetupExternalInterrupts(void)
{
    /* Set Standard priority order */
	pspExtInterruptSetPriorityOrder(D_PSP_EXT_INT_STANDARD_PRIORITY);

    /* Set interrupts threshold to minimal level (== all interrupts should be served) */
	pspExtInterruptsSetThreshold(M_PSP_EXT_INT_THRESHOLD_UNMASK_ALL_VALUE);

	/* Set the nesting priority threshold to minimal level (== all interrupts should be served) */
	pspExtInterruptsSetNestingPriorityThreshold(M_PSP_EXT_INT_THRESHOLD_UNMASK_ALL_VALUE);

	/* Set Gateway Interrupt type (Level) */
	pspExtInterruptSetType(D_BSP_IRQ_3, D_PSP_EXT_INT_LEVEL_TRIG_TYPE);

	/* Set gateway Polarity (Active high) */
	pspExtInterruptSetPolarity(D_BSP_IRQ_3, D_PSP_EXT_INT_ACTIVE_HIGH);

	/* Set the priority level to highest */
	pspExtInterruptSetPriority(D_BSP_IRQ_3, M_PSP_EXT_INT_PRIORITY_SET_TO_HIGHEST_VALUE);

	/* Enable IRQ3 interrupt in the PIC */
	pspExternalInterruptEnableNumber(D_BSP_IRQ_3);

	/* Enable external interrupts */
	pspEnableInterruptNumberMachineLevel(E_MACHINE_EXTERNAL_CAUSE);
}

/**
 * @brief - demoExtInterruptsWakeupTest - verify that core wakes up upon external interrupt
 *          In this test we use the SweRVolf FPGA timer to trigger external-interrupt (IRQ3)
 *
 */
void demoExtInterruptsWakeupTest(void)
{
	u64_t udTimeBeforeSleep;
	u64_t udTimeAfterSleep;

	/* Zero the test results variable */
	g_uiTestWayPoints = 0;

	/* register external interrupt handler */
	pspExternalInterruptRegisterISR(D_BSP_IRQ_3, demoExternalInterruptIsr, 0);

	/* Rout SweRVolf FPGA timer to IRQ3 assertion - i.e. when the timer expires, IRQ3 external interrupt is asserted */
	bspRoutTimer(E_TIMER_TO_IRQ3);

	/* Set SweRVolf FPGA Timer duration (upon expiration, it will trigger an external interrupt) */
	bspSetTimerDurationMsec(500000);

	/* Setup external interrupts */
    demoSetupExternalInterrupts();

	/* Enable all machine level interrupts */
	M_PSP_INTERRUPTS_ENABLE_IN_MACHINE_LEVEL();

	g_uiTestWayPoints |= M_PSP_BIT_MASK(D_BEFORE_HALT);

	udTimeBeforeSleep = pspTimerCounterGet();

	/* Let the SweRVolf FPGA timer to start running */
	bspStartTimer();

	/* Halt the core */
	pspPmcHalt();

	/* This line , and the following are executed only when core is not in 'Halt' */
	udTimeAfterSleep = pspTimerCounterGet();

	g_uiTestWayPoints |= M_PSP_BIT_MASK(D_AFTER_HALT);

	/* verify all test way points wore visited */
	if(g_uiTestWayPoints != D_EXT_INT_WAKEUP_TEST_RESULT)
	{
		/* Test failed */
		M_DEMO_ENDLESS_LOOP();
	}

	/* verify that core was indeed halted */
	if(udTimeAfterSleep - udTimeBeforeSleep < D_HALT_TIME)
	{
		/* Test failed */
		M_DEMO_ENDLESS_LOOP();
	}
}


/**
 * @brief - demoMtimerIsrHandler - Handle machine timer ISR
 *
 */
void demoMtimerIsrHandler(void)
{
	/* Disable Machine-Timer interrupt */
	pspDisableInterruptNumberMachineLevel(E_MACHINE_TIMER_CAUSE);

	/* Mark that ISR visited */
	g_uiTestWayPoints |= M_PSP_BIT_MASK(D_IN_MTIMER_ISR);
}


/**
 * @brief - demoMtimerWakeupTest - verify that core wakes up upon machine timer interrupt
 *
 */
void demoMtimerWakeupTest(void)
{
	u64_t udTimeBeforeSleep;
	u64_t udTimeAfterSleep;

	/* Zero the test results variable */
	g_uiTestWayPoints = 0;

	/* register Machine timer interrupt handler */
	pspRegisterInterruptHandler(demoMtimerIsrHandler, E_MACHINE_TIMER_CAUSE);

	/* Enable machine timer interrupt */
	pspEnableInterruptNumberMachineLevel(E_MACHINE_TIMER_CAUSE);

	/* Activate machine timer */
	M_PSP_TIMER_COUNTER_ACTIVATE(D_PSP_CORE_TIMER, D_HALT_TIME);

	/* Enable all machine level interrupts */
	M_PSP_INTERRUPTS_ENABLE_IN_MACHINE_LEVEL();

	g_uiTestWayPoints |= M_PSP_BIT_MASK(D_BEFORE_HALT);

	udTimeBeforeSleep = pspTimerCounterGet();

	/* Halt the core */
	pspPmcHalt();

	/* This line , and the following are executed only when core is not in 'Halt' */
	udTimeAfterSleep = pspTimerCounterGet();

	g_uiTestWayPoints |= M_PSP_BIT_MASK(D_AFTER_HALT);

	/* verify all test way points wore visited */
	if(g_uiTestWayPoints != D_MTIMER_WAKEUP_TEST_RESULT)
	{
		/* Test failed */
		M_DEMO_ENDLESS_LOOP();
	}

	/* verify that core was indeed halted */
	if(udTimeAfterSleep - udTimeBeforeSleep < D_HALT_TIME)
	{
		/* Test failed */
		M_DEMO_ENDLESS_LOOP();
	}
}

/**
 * @brief - demoStart - startup point of the demo application. called from main function.
 *
 */
void demoStart(void)
{
	/* Register interrupt vector */
	pspInterruptsSetVectorTableAddress(&psp_vect_table);

	/* verify core wakes up from halt(C3) state upon machine timer interrupt */
	demoMtimerWakeupTest();
	/* verify core wakes up from halt(C3) state upon external interrupt */
	demoExtInterruptsWakeupTest();

	/* Arriving here means all tests passed successfully */
	demoOutputMsg("Power Management Control tests passed successfully\n",52);

}
