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
* @file   demo_external_interrupts.c
* @author Nati Rapaport
* @date   25.03.2020
* @brief  Demo application for external interrupts (using SweRVolf FPGA for SweRV EH1)
*/

/**
* include files
*/
#include "psp_api.h"
#include "demo_platform_al.h"
#include "external_interrupts.h"  /* BSP file - for generation of external interrupt upon demo test call */
#include "demo_utils.h"

/**
* definitions
*/
#define D_DEMO_EXT_INT_TEST_1  1
#define D_DEMO_EXT_INT_TEST_2  2
#define D_DEMO_EXT_INT_TEST_3  3
#define D_DEMO_EXT_INT_TEST_4  4
#define D_DEMO_EXT_INT_TEST_5  5
#define D_DEMO_EXT_INT_TEST_6  6
#define D_DEMO_EXT_INT_TEST_7  7
#define D_DEMO_EXT_INT_TEST_8  8
#define D_DEMO_EXT_INT_TEST_9  9
#define D_DEMO_EXT_INT_TEST_10 10

/* Verification points at each ISR */
#define D_DEMO_INITIAL_STATE               0
#define D_DEMO_EXT_INT_ISR_JUMPED          1
#define D_DEMO_EXT_INT_PENDING_BIT_SET     2
#define D_DEMO_EXT_INT_CORRECT_PRIORITY    3

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
/* General vector table */
extern void psp_vect_table(void);

/**
* global variables
*/
/* Array with the bit-map expected value for each check-point of the test results */
u32_t g_uiDemoResultsBitMapToCompare[D_DEMO_EXT_INT_CORRECT_PRIORITY+1] = {0x1,0x3,0x7,0xF};

/* Array of test results. It is used repeatedly per test. Each place in the array represents the results of corresponding ISR in the current test.
 * Note - As only irq3 and irq4 are functional, then places 0, 1 and 2 in the array are not in use */
u32_t g_uiDemoExtIntsPassFailResult[D_BSP_LAST_IRQ_NUM+1];

/* Array of priority-level set by the test function for each external-interrupt source-id */
u32_t g_uiDemoPriorityLevelPerSourceId[D_BSP_LAST_IRQ_NUM+1];

/**
* functions
*/

/**
* @brief - Initialize (zero) the array of test results - to be called before each test
*
*/
void demoInitializeTestResults(void)
{
	u32_t uiTestResultsIndex;

	for(uiTestResultsIndex = D_BSP_FIRST_IRQ_NUM; uiTestResultsIndex <= D_BSP_LAST_IRQ_NUM; uiTestResultsIndex++)
	{
		/* Set test-result of all ISRs to "initial state" (bit0 = 1) */
		g_uiDemoExtIntsPassFailResult[uiTestResultsIndex] = M_PSP_BIT_MASK(D_DEMO_INITIAL_STATE);

		/* Make sure the external-interrupt triggers are cleared */
		bspClearExtInterrupt(uiTestResultsIndex);
	}
}


/**
 * @brief - Default initialization of PIC and Gateways. Called at the beginning of each test.
 *
 * @param - pIsr - pointer of ISR to register for this test
 */
void demoDefaultInitialization(pspInterruptHandler_t pTestIsr)
{
	u32_t uiSourceId;

    /* Set Standard priority order */
	pspExtInterruptSetPriorityOrder(D_PSP_EXT_INT_STANDARD_PRIORITY);

    /* Set interrupts threshold to minimal (== all interrupts should be served) */
	pspExtInterruptsSetThreshold(M_PSP_EXT_INT_THRESHOLD_UNMASK_ALL_VALUE);

	/* Set the nesting priority threshold to minimal (== all interrupts should be served) */
	pspExtInterruptsSetNestingPriorityThreshold(M_PSP_EXT_INT_THRESHOLD_UNMASK_ALL_VALUE);

	/* Initialize all Interrupt-sources & Register ISR for all */
	for (uiSourceId = D_BSP_FIRST_IRQ_NUM; uiSourceId <= D_BSP_LAST_IRQ_NUM; uiSourceId++)
	{
		/* Set Gateway Interrupt type (Level) */
		pspExtInterruptSetType(uiSourceId, D_PSP_EXT_INT_LEVEL_TRIG_TYPE);

		/* Set gateway Polarity (Active high) */
		pspExtInterruptSetPolarity(uiSourceId, D_PSP_EXT_INT_ACTIVE_HIGH);

		/* Clear the gateway */
		pspExtInterruptClearPendingInt(uiSourceId);

		/* Priority-level is checked after each test so store it here as "expected results"*/
		g_uiDemoPriorityLevelPerSourceId[uiSourceId] = M_PSP_EXT_INT_PRIORITY_SET_TO_HIGHEST_VALUE;
		/* Set the priority level to highest to all interrupt sources */
		pspExtInterruptSetPriority(uiSourceId, g_uiDemoPriorityLevelPerSourceId[uiSourceId]);

		/* Enable each one of the interrupts in the PIC */
		pspExternalInterruptEnableNumber(uiSourceId);

		/* Register ISRs to all interrupt sources (here we use same ISR to all of them) */
		pspExternalInterruptRegisterISR(uiSourceId, pTestIsr, 0);
	}

	/* Enable all interrupts in mstatus CSR */
	M_PSP_INTERRUPTS_ENABLE_IN_MACHINE_LEVEL();

	/* Enable external interrupts in mie CSR */
	M_PSP_SET_CSR(D_PSP_MIE_NUM, D_PSP_MIE_MEIE_MASK);
}


/**
 * @brief - Verify expected results for IRQ3 and IRQ4
 *
 * @param - uiTestNumber              - Test number
 * @param - uiExpectedResultFirstIRQ  - Expected result to verify for 1'st IRQ (IRQ3)
 * @param - uiExpectedResultSecondIRQ - Expected result to verify for 2'nd IRQ (IRQ4)
 */
void demoVerifyExpectedTestResults(u32_t uiTestNumber, u32_t uiExpectedResultFirstIRQ, u32_t uiExpectedResultSecondIRQ)
{
	/* Verify that both IRQ3 and IRQ4 passed through all checkpoints successfully */
	if ((g_uiDemoResultsBitMapToCompare[uiExpectedResultFirstIRQ] != g_uiDemoExtIntsPassFailResult[D_BSP_IRQ_3])
		||
		(g_uiDemoResultsBitMapToCompare[uiExpectedResultSecondIRQ] != g_uiDemoExtIntsPassFailResult[D_BSP_IRQ_4]))
	{
        /* Output a failure message */
        demoOutputMsg("External Interrupts, Test #%d Failed:\n", uiTestNumber);
        /* Break here to let debug */
        M_PSP_EBREAK();
    }
}


/**
 *
 * @brief - The function indicates/marks 3 things:
 * - ISR of the current claim-id has been occurred
 * - Whether corresponding "pending" bit is set or not
 * - Corresponding "priority" field is correct
 *
 * @return - ucIntSourceId - number of current interrupt
 *
 */
u08_t demoCheckInterruptStatusInISR(void)
{
	/* Get the claim-id (== source-id) of the current ISR */
	u08_t ucIntSourceId = pspExtInterruptGetClaimId();

	/* Indication of the ISR occurrence */
	g_uiDemoExtIntsPassFailResult[ucIntSourceId] |= M_PSP_BIT_MASK(D_DEMO_EXT_INT_ISR_JUMPED);

	/* Mark whether corresponding pending-bit is set or not */
	if (D_PSP_ON == pspExtInterruptIsPending(ucIntSourceId))
	{
		g_uiDemoExtIntsPassFailResult[ucIntSourceId] |= M_PSP_BIT_MASK(D_DEMO_EXT_INT_PENDING_BIT_SET);
	}

	/* Check correct priority-level (clidpri) field in meicidpl CSR */
	if (pspExtInterruptGetPriority() ==  g_uiDemoPriorityLevelPerSourceId[ucIntSourceId])
	{
		g_uiDemoExtIntsPassFailResult[ucIntSourceId] |= M_PSP_BIT_MASK(D_DEMO_EXT_INT_CORRECT_PRIORITY);
	}

	return (ucIntSourceId);
}

/**
 * ISR for test #1, #2, #3 ,#4 and #5 (see the tests details)
 *
 *
 * In addition, this ISR clears the interrupt at its source (BSP api)
 *
 * Tests 1,2,3,4 and 5 does different things each. However all of them use this ISR.
 */
void demoExtIntTest_1_2_3_4_5_ISR(void)
{
	u08_t ucIntSourceId;

	/* Check some interrupt's indication */
	ucIntSourceId = demoCheckInterruptStatusInISR();

	/* Stop the generation of the specific external interrupt */
	bspClearExtInterrupt(ucIntSourceId);
}


/**
 * ISR for test #6 (see the test details)
 *
 * The ISR indicates/marks 3 things:
 * - ISR of the current claim-id has been occurred
 * - Whether corresponding "pending" bit is set or not
 * - Corresponding "priority" field is correct
 *
 * In addition, this ISR clears the interrupt at the gateway
 *
 */
void demoExtIntTest_6_ISR(void)
{
	u08_t ucIntSourceId;

    /* Check some interrupt's indication */
	ucIntSourceId = demoCheckInterruptStatusInISR();

	/* Stop the generation of the specific external interrupt */
	pspExtInterruptClearPendingInt(ucIntSourceId);
}


/* TBD - following ISRs */

/**
 * Test # 1 - After full external interrupts initialization, disable external interrupts in mie CSR.
 *            Verify that external interrupts not occur at all.
 *
 */
void demoExtIntsTest1GlobalDisabled(void)
{
	u32_t uiTestNumber = D_DEMO_EXT_INT_TEST_1;

    /* Put the Generation-Register in its initial state (no external interrupts are generated) */
	bspInitializeGenerationRegister(D_PSP_EXT_INT_ACTIVE_HIGH);

    /* Initialize test results array and parameters */
    demoInitializeTestResults();

	/* Initialize PIC, gateways and other External-Interrupt related CSRs */
	demoDefaultInitialization(demoExtIntTest_1_2_3_4_5_ISR);

	/* Disable external interrupts in mie CSR */
	M_PSP_CLEAR_CSR(D_PSP_MIE_NUM, D_PSP_MIE_MEIE_MASK);

	/* Generate external interrupts 3 & 4 (with Active-High polarity, Level trigger type) */
	bspGenerateExtInterrupt(D_BSP_IRQ_3, D_PSP_EXT_INT_ACTIVE_HIGH, D_PSP_EXT_INT_LEVEL_TRIG_TYPE );
	bspGenerateExtInterrupt(D_BSP_IRQ_4, D_PSP_EXT_INT_ACTIVE_HIGH, D_PSP_EXT_INT_LEVEL_TRIG_TYPE );

	/* Expected results here are - No ISR has been occurred */
	demoVerifyExpectedTestResults(uiTestNumber, D_DEMO_INITIAL_STATE, D_DEMO_INITIAL_STATE);
}


/**
 *  Test # 2 - Enable IRQ3 and disable IRQ4 in meieS CSRs.
 *             Verify that only enabled external-interrupt did occurred
 *
 */
void demoExtIntsTest2SpecificDisabled(void)
{
	u32_t uiTestNumber = D_DEMO_EXT_INT_TEST_2;

	/* Put the Generation-Register in its initial state (no external interrupts are generated) */
	bspInitializeGenerationRegister(D_PSP_EXT_INT_ACTIVE_HIGH);

	/* Cleanup previous test results before proceeding */
	demoInitializeTestResults();

	/* Initialize PIC, gateways and other External-Interrupt related CSRs */
	demoDefaultInitialization(demoExtIntTest_1_2_3_4_5_ISR);

	/* Disable IRQ3 */
	pspExternalInterruptDisableNumber(D_BSP_IRQ_3);

	/* Generate external interrupts 3 & 4 (with Active-High, Level trigger type) */
	bspGenerateExtInterrupt(D_BSP_IRQ_3, D_PSP_EXT_INT_ACTIVE_HIGH, D_PSP_EXT_INT_LEVEL_TRIG_TYPE );
	bspGenerateExtInterrupt(D_BSP_IRQ_4, D_PSP_EXT_INT_ACTIVE_HIGH, D_PSP_EXT_INT_LEVEL_TRIG_TYPE );

	/* Expected results here are - IRQ3 did not occur, IRQ4 occurred */
	demoVerifyExpectedTestResults(uiTestNumber, D_DEMO_INITIAL_STATE, D_DEMO_EXT_INT_CORRECT_PRIORITY);
}

/**
 *  Test # 3 - Set priority & threshold in standard priority-order
 *             Verify external interrupts occurred only on sources with priority higher than threshold
 *
 */
void demoExtIntsTest3PriorityStandardOrder(void)
{
	u32_t uiTestNumber = D_DEMO_EXT_INT_TEST_3;

	/* Put the Generation-Register in its initial state (no external interrupts are generated) */
	bspInitializeGenerationRegister(D_PSP_EXT_INT_ACTIVE_HIGH);

	/* Cleanup previous test results before proceeding */
	demoInitializeTestResults();

	/* Initialize PIC, gateways and other External-Interrupt related CSRs */
	demoDefaultInitialization(demoExtIntTest_1_2_3_4_5_ISR);

	/* Part1: Set the priority of both IRQ3 and IRQ4 not higher than threshold. Expect no ISR to jump */

	/* Set interrupts threshold to 7 */
	pspExtInterruptsSetThreshold(D_PSP_EXT_INT_THRESHOLD_7);

	/* Set IRQ3 priority to 6 */
	g_uiDemoPriorityLevelPerSourceId[D_BSP_IRQ_3] = D_PSP_EXT_INT_PRIORITY_6;
	/* Priority-level is checked later so store it here as expected value */
	pspExtInterruptSetPriority(D_BSP_IRQ_3, g_uiDemoPriorityLevelPerSourceId[D_BSP_IRQ_3]);

	/* Set IRQ4 priority to 7 */
	g_uiDemoPriorityLevelPerSourceId[D_BSP_IRQ_4] = D_PSP_EXT_INT_PRIORITY_7;
	/* Priority-level is checked later so store it here as expected value */
	pspExtInterruptSetPriority(D_BSP_IRQ_4, g_uiDemoPriorityLevelPerSourceId[D_BSP_IRQ_4]);

	/* Generate external interrupts 3 & 4 (with Active-High, Level trigger type) */
	bspGenerateExtInterrupt(D_BSP_IRQ_3, D_PSP_EXT_INT_ACTIVE_HIGH, D_PSP_EXT_INT_LEVEL_TRIG_TYPE );
	bspGenerateExtInterrupt(D_BSP_IRQ_4, D_PSP_EXT_INT_ACTIVE_HIGH, D_PSP_EXT_INT_LEVEL_TRIG_TYPE );

	/* Expected results here are - No ISR has been occurred */
	demoVerifyExpectedTestResults(uiTestNumber, D_DEMO_INITIAL_STATE, D_DEMO_INITIAL_STATE);

	/* Part2: Set only IRQ3 priority higher than threshold. Expect only ISR3 to jump */

    /* Initialize test results array and parameters */
	demoInitializeTestResults();

	/* Set IRQ3 priority to 8 */
	g_uiDemoPriorityLevelPerSourceId[D_BSP_IRQ_3] = D_PSP_EXT_INT_PRIORITY_8;
	/* Priority-level is checked later so store it here as expected value */
	pspExtInterruptSetPriority(D_BSP_IRQ_3, g_uiDemoPriorityLevelPerSourceId[D_BSP_IRQ_3]);

	/* Generate external interrupts 3 & 4 (with Active-High, Level trigger type) */
	bspGenerateExtInterrupt(D_BSP_IRQ_3, D_PSP_EXT_INT_ACTIVE_HIGH, D_PSP_EXT_INT_LEVEL_TRIG_TYPE );
	bspGenerateExtInterrupt(D_BSP_IRQ_4, D_PSP_EXT_INT_ACTIVE_HIGH, D_PSP_EXT_INT_LEVEL_TRIG_TYPE );

	/* Expected results here are - IRQ3 occurred, IRQ4 did not occur */
	demoVerifyExpectedTestResults(uiTestNumber, D_DEMO_EXT_INT_CORRECT_PRIORITY, D_DEMO_INITIAL_STATE);
}

/**
 *  Test # 4 - Set priority & threshold in reversed priority-order
 *             Verify external interrupts occurred only on sources with priority lower than threshold
 *
 */
void demoExtIntsTest4PriorityReversedOrder(void)
{
	u32_t uiTestNumber = D_DEMO_EXT_INT_TEST_4;

	/* Put the Generation-Register in its initial state (no external interrupts are generated) */
	bspInitializeGenerationRegister(D_PSP_EXT_INT_ACTIVE_HIGH);

	/* Cleanup previous test results before proceeding */
	demoInitializeTestResults();

	/* Initialize PIC, gateways and other External-Interrupt related CSRs */
	demoDefaultInitialization(demoExtIntTest_1_2_3_4_5_ISR);

	/* Set Reversed priority order */
	pspExtInterruptSetPriorityOrder(D_PSP_EXT_INT_REVERSED_PRIORITY);

	/* Part1: Set the priority of both IRQ3 and IRQ4 not lower than threshold. Expect no ISR to jump */

	/* Set interrupts threshold to 5 */
	pspExtInterruptsSetThreshold(D_PSP_EXT_INT_THRESHOLD_5);

	/* As priority-order has been set to 'reversed', need to set again the nesting priority threshold to minimum (== all should be served)
	 * - now it should be '15' */
	pspExtInterruptsSetNestingPriorityThreshold(M_PSP_EXT_INT_THRESHOLD_UNMASK_ALL_VALUE);

	/* Set IRQ3 priority to 5 */
	g_uiDemoPriorityLevelPerSourceId[D_BSP_IRQ_3] = D_PSP_EXT_INT_PRIORITY_5;
	/* Priority-level is checked later so store it here as expected value */
	pspExtInterruptSetPriority(D_BSP_IRQ_3, g_uiDemoPriorityLevelPerSourceId[D_BSP_IRQ_3]);

	/* Set IRQ4 priority to 6 */
	g_uiDemoPriorityLevelPerSourceId[D_BSP_IRQ_4] = D_PSP_EXT_INT_PRIORITY_6;
	/* Priority-level is checked later so store it here as expected value */
	pspExtInterruptSetPriority(D_BSP_IRQ_4, g_uiDemoPriorityLevelPerSourceId[D_BSP_IRQ_4]);

	/* Generate external interrupts 3 & 4 (with Active-High, Level trigger type) */
	bspGenerateExtInterrupt(D_BSP_IRQ_3, D_PSP_EXT_INT_ACTIVE_HIGH, D_PSP_EXT_INT_LEVEL_TRIG_TYPE );
	bspGenerateExtInterrupt(D_BSP_IRQ_4, D_PSP_EXT_INT_ACTIVE_HIGH, D_PSP_EXT_INT_LEVEL_TRIG_TYPE );

	/* Expected result here is - No ISR has been occurred */
	demoVerifyExpectedTestResults(uiTestNumber, D_DEMO_INITIAL_STATE, D_DEMO_INITIAL_STATE);

	/* Part2: Set only IRQ3 priority lower than threshold. Expect only ISR3 to jump */

    /* Initialize test results array and parameters */
	demoInitializeTestResults();

	/* Set IRQ3 priority to 4 */
	g_uiDemoPriorityLevelPerSourceId[D_BSP_IRQ_3] = D_PSP_EXT_INT_PRIORITY_4;
	/* Priority-level is checked later so store it here as expected value */
	pspExtInterruptSetPriority(D_BSP_IRQ_3, g_uiDemoPriorityLevelPerSourceId[D_BSP_IRQ_3]);

	/* Generate external interrupts 3 & 4 (with Active-High, Level trigger type) */
	bspGenerateExtInterrupt(D_BSP_IRQ_3, D_PSP_EXT_INT_ACTIVE_HIGH, D_PSP_EXT_INT_LEVEL_TRIG_TYPE );
	bspGenerateExtInterrupt(D_BSP_IRQ_4, D_PSP_EXT_INT_ACTIVE_HIGH, D_PSP_EXT_INT_LEVEL_TRIG_TYPE );

	/* Expected results here are - IRQ3 occurred, IRQ4 did not occur */
	demoVerifyExpectedTestResults(uiTestNumber, D_DEMO_EXT_INT_CORRECT_PRIORITY, D_DEMO_INITIAL_STATE);
}


/**
 *  Test # 5 - Test correct gateway polarity (active-high / active-low) behavior
 *
 */
void demoExtIntsTest5GatweayPolarity(void)
{
	u32_t uiTestNumber = D_DEMO_EXT_INT_TEST_5;

    /* Part1 - Configure PIC to Active-High polarity. Trigger Interrupts in Active-Low polarity. Expect no ISR to occur  */

	/* Put the Generation-Register in its initial state (no external interrupts are generated) */
	bspInitializeGenerationRegister(D_PSP_EXT_INT_ACTIVE_HIGH);

	/* Cleanup previous test results before proceeding */
	demoInitializeTestResults();

	/* Initialize PIC, gateways and other External-Interrupt related CSRs */
	demoDefaultInitialization(demoExtIntTest_1_2_3_4_5_ISR);

	/* Generate external interrupts 3 & 4 (with Active-Low, Level trigger type) */
	bspGenerateExtInterrupt(D_BSP_IRQ_3, D_PSP_EXT_INT_ACTIVE_LOW, D_PSP_EXT_INT_LEVEL_TRIG_TYPE );
	bspGenerateExtInterrupt(D_BSP_IRQ_4, D_PSP_EXT_INT_ACTIVE_LOW, D_PSP_EXT_INT_LEVEL_TRIG_TYPE );

	/* Expected result here is - No ISR has been occurred */
	demoVerifyExpectedTestResults(uiTestNumber, D_DEMO_INITIAL_STATE, D_DEMO_INITIAL_STATE);

    /* Part2 - Configure PIC to Active-High polarity. Trigger Interrupts Active-High polarity. Expect ISRs to occur  */

	/* Put the Generation-Register in its initial state (no external interrupts are generated) */
	bspInitializeGenerationRegister(D_PSP_EXT_INT_ACTIVE_HIGH);

	/* Cleanup previous test results before proceeding */
	demoInitializeTestResults();

	/* Generate external interrupts 3 & 4 (with Active-High, Level trigger type) */
	bspGenerateExtInterrupt(D_BSP_IRQ_3, D_PSP_EXT_INT_ACTIVE_HIGH, D_PSP_EXT_INT_LEVEL_TRIG_TYPE );
	bspGenerateExtInterrupt(D_BSP_IRQ_4, D_PSP_EXT_INT_ACTIVE_HIGH, D_PSP_EXT_INT_LEVEL_TRIG_TYPE );

	/* Expected result here is - ISRs has been occurred */
	demoVerifyExpectedTestResults(uiTestNumber, D_DEMO_EXT_INT_CORRECT_PRIORITY, D_DEMO_EXT_INT_CORRECT_PRIORITY);

	/* TBD for the 2 following parts. Pending clarification of correct setting of trigger-register in Active-low mode */

	/* Part3 - Configure PIC to Active-Low polarity. Trigger Interrupts Active-High polarity. Expect no ISRs to occur  */

    /* Part4 - Configure PIC to Active-Low polarity. Trigger Interrupts Active-High polarity. Expect ISRs to occur  */
}


/**
 *  Test # 6 - Test correct gateway (level-triggered or edge-triggered) behavior
 *
 */
void demoExtIntsTest6GatweayConfiguration(void)
{
	u32_t uiSourceId, uiTestNumber = D_DEMO_EXT_INT_TEST_6;

	/* Put the Generation-Register in its initial state (no external interrupts are generated) */
	bspInitializeGenerationRegister(D_PSP_EXT_INT_ACTIVE_HIGH);

	/* Cleanup previous test results before proceeding */
	demoInitializeTestResults();

	/* Initialize PIC, gateways and other External-Interrupt related CSRs */
	demoDefaultInitialization(demoExtIntTest_6_ISR);

	/* Set Gateway Interrupt type (Edge) */
	for (uiSourceId = D_BSP_FIRST_IRQ_NUM; uiSourceId <= D_BSP_LAST_IRQ_NUM; uiSourceId++)
	{
		pspExtInterruptSetType(uiSourceId, D_PSP_EXT_INT_EDGE_TRIG_TYPE);
	}

	/* Generate external interrupts 3 & 4 (with Active-Low, Level trigger type) */
	bspGenerateExtInterrupt(D_BSP_IRQ_3, D_PSP_EXT_INT_ACTIVE_HIGH, D_PSP_EXT_INT_EDGE_TRIG_TYPE );
	bspGenerateExtInterrupt(D_BSP_IRQ_4, D_PSP_EXT_INT_ACTIVE_HIGH, D_PSP_EXT_INT_EDGE_TRIG_TYPE );

	/* Expected result here is - ISRs has been occurred */
	demoVerifyExpectedTestResults(uiTestNumber, D_DEMO_EXT_INT_CORRECT_PRIORITY, D_DEMO_EXT_INT_CORRECT_PRIORITY);
}


/**
 *  Test # 7 - Test external interrupts with lower priority sources do not preempt higher source interrupt
 *
 * @return - returns 0 (success). In case of a failure the function enters an endless loop
 */
void demoExtIntsTest7NestedInteeruptLowerPriority(void)
{
}


/**
 *  Test # 8 - Test external interrupts with same priority sources as current interrupt do not preempt it
 *
 * @return - returns 0 (success). In case of a failure the function enters an endless loop
 */
void demoExtIntsTest8NestedInteeruptSamePriority(void)
{
}


/**
 *  Test # 9 - Test external interrupts with higher priority sources then current interrupt do preempt it
 *
 * @return - returns 0 (success). In case of a failure the function enters an endless loop
 */
void demoExtIntsTest9NestedInteeruptHigherPriority(void)
{
}



/**
 * demoStart - startup point of the demo application. called from main function.
 *
 */
void demoStart(void)
{
   /* Register interrupt vector */
   pspInterruptsSetVectorTableAddress(&psp_vect_table);

   /* Test #1 - Global disable external interrupts */
   demoExtIntsTest1GlobalDisabled();

   /* Test #2 - Disable specific external interrupt */
   demoExtIntsTest2SpecificDisabled();

   /* Test #3 - Priority & Threshold - standard order */
   demoExtIntsTest3PriorityStandardOrder();

   /* Test #4 - Priority & Threshold - reversed order*/
   demoExtIntsTest4PriorityReversedOrder();

   /* Test #5 - Polarity - active-high/active-low */
   demoExtIntsTest5GatweayPolarity();

   /* Test #6 - Gateways Level/Edge setting*/
   demoExtIntsTest6GatweayConfiguration();

#if 0
   /* Test #7 - Nested interrupts - lower priority */
   demoExtIntsTest7NestedInteeruptLowerPriority();

   /* Test #8 - Nested interrupts - same priority */
   demoExtIntsTest8NestedInteeruptSamePriority();

   /* Test #9 - Nested interrupts - higher priority */
   demoExtIntsTest9NestedInteeruptHigherPriority();
#endif
   /* Arriving here means all tests passed successfully */
   demoOutputMsg("External Interrupts tests passed successfully\n");
}

