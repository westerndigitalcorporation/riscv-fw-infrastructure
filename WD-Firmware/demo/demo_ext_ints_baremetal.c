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

/* In SwreVolf we have the capability to create 6 different external interrupts */
#define D_DEMO_NUM_OF_EXT_INTS 9
#define D_DEMO_IRQ_0           0 /* Not in use */
#define D_DEMO_IRQ_1           1 /* Not in use */
#define D_DEMO_IRQ_2           2 /* Not in use */
#define D_DEMO_IRQ_3           3
#define D_DEMO_IRQ_4           4
#define D_DEMO_IRQ_5           5
#define D_DEMO_IRQ_6           6
#define D_DEMO_IRQ_7           7
#define D_DEMO_IRQ_8           8

#define D_DEMO_CREATE_IRQ3   (1 << (D_DEMO_IRQ_3-1)) /* In order to create irq3, set bit #2 */
#define D_DEMO_CREATE_IRQ4   (1 << (D_DEMO_IRQ_4-1)) /* In order to create irq4, set bit #3 */
#define D_DEMO_CREATE_IRQ5   (1 << (D_DEMO_IRQ_5-1)) /* In order to create irq5, set bit #4 */
#define D_DEMO_CREATE_IRQ6   (1 << (D_DEMO_IRQ_6-1)) /* In order to create irq6, set bit #5 */
#define D_DEMO_CREATE_IRQ7   (1 << (D_DEMO_IRQ_7-1)) /* In order to create irq7, set bit #6 */
#define D_DEMO_CREATE_IRQ8   (1 << (D_DEMO_IRQ_8-1)) /* In order to create irq8, set bit #7 */


/* Verification points at each ISR */
#define D_DEMO_INITIAL_STATE               0
#define D_DEMO_EXT_INT_ISR_JUMPED          1
#define D_DEMO_EXT_INT_PENDING_BIT_SET     2
#define D_DEMO_EXT_INT_CORRECT_PRIORITY    3

/* Specified RAM address for generation of external interrupts (SwerVolf special implementation) */
#if (0 != D_EXT_INTS_GENERATION_ADDRESS)
    #define D_DEMO_EXT_INTS_GENERATION_ADDRESS    D_EXT_INTS_GENERATION_ADDRESS
#else
    #error "External interrupts generation address is not defined"
#endif

/* In SwerVolf we have bits 3..8 at a specified address, available for generation of external interrupts */
#if (0 != D_EXT_INTS_GENERATION_BITS)
    #define D_DEMO_EXT_INTS_GENERATION_BITS    D_EXT_INTS_GENERATION_BITS
#else
    #error "External interrupts generation bits are not defined"
#endif

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
/* External interrupts vector table */
extern pspInterruptHandler_t G_Ext_Interrupt_Handlers[];


/**
* global variables
*/

/* Array of test results. It is used repeatedly per test. Each place in the array represents the results of corresponding
 * ISR in the current test */
u08_t g_ucDemoExtIntsPassFailResult[D_DEMO_NUM_OF_EXT_INTS];

/* Array of priority-level set by the test function for each external-interrupt source-id */
u16_t g_usPriorityLevelPerSourceId[D_DEMO_NUM_OF_EXT_INTS];



/**
* functions
*/


/**
* Initialize (zero) the array of test results - to be called before each test
*
*/
void demoInitializeTestResults(void)
{
	for (u32_t i=0; i<D_DEMO_NUM_OF_EXT_INTS; i++)
	{
		/* Set test-result of all ISRs to "initial state" (0) */
		g_ucDemoExtIntsPassFailResult[i] = D_DEMO_INITIAL_STATE;
	}
}


u32_t uiActivationMask; /* Nati - This is temporarily here for debug. will be removed later */
/**
* Generate external interrupt(s) - use mechanism in SwerVolf to create external interrupt by setting certain bit(s) in dedicated memory address
*
* @param - usExtInterruptBitMap - bitmap of ext-interrupts to generate
*
*/
void demoGenerateExtInterrupt(u32_t usExtInterruptBitMap)
{
/* Nati - I have still a debug work to do around this one. */
	uiActivationMask = usExtInterruptBitMap;
//	uiActivationMask &= D_DEMO_EXT_INTS_GENERATION_BITS ;

	/* D_DEMO_EXT_INTS_GENERATION_BITS is used to make sure we only affect bits 3..8 */
	M_PSP_WRITE_REGISTER_32(D_DEMO_EXT_INTS_GENERATION_ADDRESS, uiActivationMask );
	/* Nati - to do - check why |= is not working here (ext-ints not created) */
}


/**
* Clear generation of external interrupt(s)
*
* @param - usExtInterruptBitMap - bitmap of ext-interrupts to clear
*
*/
void demoClearGenerationExtInterrupt(u16_t usExtInterruptBitMap)
{
	/* D_DEMO_EXT_INTS_GENERATION_BITS is used to make sure we only affect bits 3..8 */
	M_PSP_WRITE_REGISTER_32(D_DEMO_EXT_INTS_GENERATION_ADDRESS, 0 );
	/* Nati - to do - expand usage for other mask besides '0'. For now - all ext-interrupts are ceased at once */
	/* Nati - to do - check why &= is not working here (ext-ints not stop) */
}


/**
 * ISR for test #1, #2, #3 ,#4 and #5 (see the tests later in this file)
 *
 * The ISR indicates/marks 3 things:
 * - ISR of the current claim-id has been occurred
 * - Whether corresponding "pending" bit is set or not
 * - Corresponding "priority" field is correct
 *
 * Tests 1,2,3,4 and 5 does different things each. However all of them use this ISR.
 */
void demoExtIntTest_1_2_3_4_5_ISR(void)
{
	/* Get the claim-id (== source-id) of the current ISR */
	u08_t ucIntSourceId = pspExtInterruptGetClaimId();

	/* Indication that this ISR has been occurred */
	g_ucDemoExtIntsPassFailResult[ucIntSourceId] = D_DEMO_EXT_INT_ISR_JUMPED ;

	/* Mark whether corresponding pending-bit is set or not */
	if (D_PSP_ON == pspExtInterruptIsPending(ucIntSourceId))
	{
		g_ucDemoExtIntsPassFailResult[ucIntSourceId] = D_DEMO_EXT_INT_PENDING_BIT_SET ;
	}

	/* Verify correct priority-level (clidpri) field in meicidpl CSR */
	if (pspExtInterruptGetPriority() ==  g_usPriorityLevelPerSourceId[ucIntSourceId])
	{
		g_ucDemoExtIntsPassFailResult[ucIntSourceId] = D_DEMO_EXT_INT_CORRECT_PRIORITY ;
	}

	/* Clear the gateway */
	/* pspExtInterruptClearGateway(ucIntSourceId); */ /* Nati - TBD see next comment */

	/* Nati - to do - still under investigation why when in edge mode, clearing the pending bit do not stop the appearance
	 * of the external interrupt. For the meantime - we stop ext-interrupts by "killing" the source */
	demoClearGenerationExtInterrupt((u32_t)D_DEMO_EXT_INTS_GENERATION_BITS);
}


/* TBD - following ISRs */


/**
 *  Test # 1 - After full external interrupts initialization, disable external interrupts in mie CSR.
 *             Verify that external interrupts not occur at all.
 *
 * @return - returns 0 (success). In case of a failure the function enters an endless loop
 */
u32_t demoExtIntsTest1GlobalDisabled(void)
{
	u32_t uiTestResult = 0 ;

#ifdef D_NEXYS_A7

    /* Set Standard priority order */
	pspExtInterruptSetPriorityOrder(D_PSP_EXT_INT_STANDARD_PRIORITY);

    /* Set interrupts threshold to minimal (== all interrupts should be served) */
	pspExtInterruptsSetThreshold(M_PSP_EXT_INT_THRESHOLD_UNMASK_ALL_VALUE);

	/* Initialize all Interrupt-sources & Register ISR for all */
	for (u32_t uiSourceId = 0; uiSourceId < D_DEMO_NUM_OF_EXT_INTS; uiSourceId++)
	{
		/* Set Gateway Interrupt type */
		pspExtInterruptSetType(uiSourceId, D_PSP_EXT_INT_LEVEL_TRIG_TYPE);

		/* Set gateway Polarity */
		pspExtInterruptSetPolarity(uiSourceId, D_PSP_EXT_INT_ACTIVE_HIGH);

		/* Clear the gateway */
		pspExtInterruptClearGateway(uiSourceId);

		/* Set the priority level to highest to all interrupt sources */
		pspExtInterruptSetPriority(uiSourceId, M_PSP_EXT_INT_PRIORITY_SET_TO_HIGHEST_VALUE);

		/* Enable each one of the interrupts in the PIC */
		pspExternalInterruptEnableNumber(uiSourceId);

		/* Register ISRs to all interrupt sources (here we use same ISR to all of them) */
		pspExternalInterruptRegisterISR(uiSourceId, demoExtIntTest_1_2_3_4_5_ISR, 0);
	}

	/* Enable interrupts in mstatus CSR */ /* Nati - to do - add specific api to do it  */
	M_PSP_SET_CSR(D_PSP_MSTATUS_NUM, D_PSP_MSTATUS_MIE_MASK);

	/* Disable external interrupts in mie CSR */ /* Nati - to do - add specific api to do it (along with machine-timer interrupt) */
	M_PSP_CLEAR_CSR(D_PSP_MIE_NUM, D_PSP_MIE_MEIE_MASK);

	/* Generate external interrupts - all sources */
	demoGenerateExtInterrupt((u32_t)D_DEMO_EXT_INTS_GENERATION_BITS);

	/* Loop for a while.. */
	demoLoopForDelay(0x10); /* Nati - TBD - maybe not needed here at all */

	/* Stop generation of external interrupts - all sources */
	demoClearGenerationExtInterrupt((u32_t)D_DEMO_EXT_INTS_GENERATION_BITS);

	/* Verify no external interrupts have been occurred */
	for (u32_t uiExtIntsIndex = 0 ; uiExtIntsIndex < D_DEMO_NUM_OF_EXT_INTS; uiExtIntsIndex++)
	{
		if (D_DEMO_INITIAL_STATE != g_ucDemoExtIntsPassFailResult[uiExtIntsIndex])
		{
			/* Output a failure message */
 			demoOutputMsg("External Interrupts, Test #1 Failed\n");
 			demoOutputMsg("ISR # %d unexpectedly jumped\n ",uiExtIntsIndex);

			/* Loop here to let debug */
			M_ENDLESS_LOOP();
		}
	}

#endif /* D_NEXYS_A7 */

    return uiTestResult;
}


/**
 *  Test # 2 - Disable specific external interrupts in meieS CSRs.
 *             Verify that only enabled external interrupt sources did occurred
 *
 * @return - returns 0 (success). In case of a failure the function enters an endless loop
 */
u32_t demoExtIntsTest2SpecificDisabled(void)
{
	u32_t uiTestResult = 0 ;

#if D_NEXYS_A7

	/* Initialize external interrupts */

	/* Set priority to each one of the external interrupt sources */

	/* Enable external interrupts # 5, 7 and 8 */

	/* Disable external interrupts # 3, 4 and 6 */

	/* Generate external interrupts - all sources */

	/* Loop for a while.. */
	demoLoopForDelay(0x100);

	/* Verify external interrupts occurred only on sources 5,7 and 8 */
	for (u32_t uiExtIntsIndex = 0 ; uiExtIntsIndex < D_DEMO_NUM_OF_EXT_INTS; uiExtIntsIndex++)
	{
	    switch (uiExtIntsIndex)
	    {
	        /* For source-ids 3, 4 ,6 the expected result is D_DEMO_INITIAL_STATE */
            case D_DEMO_IRQ_3:
            case D_DEMO_IRQ_4:
            case D_DEMO_IRQ_6:
            	if (D_DEMO_INITIAL_STATE != g_ucDemoExtIntsPassFailResult[uiExtIntsIndex])
            	{
        			/* Output a failure message */
         			demoOutputMsg("External Interrupts, Test #2 Failed\n");
         			demoOutputMsg("ISR # %d unexpectedly jumped\n",uiExtIntsIndex);
        			/* Loop here to let debug */
        			M_ENDLESS_LOOP();
            	}
                break;

            /* For source-ids 5, 7 ,8 the expected result is D_DEMO_EXT_INT_CORRECT_PRIORITY */
            case D_DEMO_IRQ_5:
            case D_DEMO_IRQ_7:
            case D_DEMO_IRQ_8:
            	if (D_DEMO_EXT_INT_CORRECT_PRIORITY != g_ucDemoExtIntsPassFailResult[uiExtIntsIndex])
            	{
        			/* Output a failure message */
         			demoOutputMsg("External Interrupts, Test #2 Failed\n",36);
         			demoOutputMsg("ISR # %d did not jump or it is wrong\n",uiExtIntsIndex);
        			/* Loop here to let debug */
        			M_ENDLESS_LOOP();
            	}
                break;

            /* Not in use */
            case D_DEMO_IRQ_0:
            case D_DEMO_IRQ_1:
            case D_DEMO_IRQ_2:
            default:
            	break;
	    } /* end of switch-case */
	} /* end of loop */

#endif /* D_NEXYS_A7 */

    return uiTestResult;
}

/**
 *  Test # 3 - Set priority & threshold in standard priority-order
 *             Verify external interrupts occurred only on sources with priority higher than threshold
 *
 * @return - returns 0 (success). In case of a failure the function enters an endless loop
 */
u32_t demoExtIntsTest3PriorityStandardOrder(void)
{
	u32_t uiTestResult = 0 ;

	/* Initialize external interrupts */

	/* Enable external interrupts # 3..8 */

	/* Set standard priority order */

	/* Set theshold to 7 */

	/* Set irq 3 priority to 5 */

	/* Set irq 4 priority to 6 */

	/* Set irq 5 priority to 7 */

	/* Set irq 6 priority to 8 */

	/* Set irq 7 priority to 9 */

	/* Set irq 8 priority to 10 */

    /* Generate external interrupt at sources 3..8 */

	/* Loop for a while.. Let all pending ISRs to run */
	demoLoopForDelay(0x100);

	/* Verify external interrupts occurred only on sources with priority higher than the threshold (i.e. 6,7,8) */

    return uiTestResult;
}

/**
 *  Test # 4 - Set priority & threshold in reversed priority-order
 *             Verify external interrupts occurred only on sources with priority lower than threshold
 *
 * @return - returns 0 (success). In case of a failure the function enters an endless loop
 */
u32_t demoExtIntsTest4PriorityReversedOrder(void)
{
	u32_t uiTestResult = 0 ;

	/* Initialize external interrupts */

	/* Enable external interrupts # 3..8 */

	/* Set reversed priority order */

	/* Set theshold to 7 */

	/* Set irq 3 priority to 5 */

	/* Set irq 4 priority to 6 */

	/* Set irq 5 priority to 7 */

	/* Set irq 6 priority to 8 */

	/* Set irq 7 priority to 9 */

	/* Set irq 8 priority to 10 */

    /* Generate external interrupt at sources 3..8 */

	/* Loop for a while.. Let all pending ISRs to run */
	demoLoopForDelay(0x100);

	/* Verify external interrupts occurred only on sources with priority less than the threshold (i.e. 3 and 4) */

    return uiTestResult;
}


/**
 *  Test # 5 - Change priority & threshold on the go
 *             Verify external interrupts occurred only on sources with priority higher than threshold
 *
 * @return - returns 0 (success). In case of a failure the function enters an endless loop
 */
u32_t demoExtIntsTest5ThresholdAndPriorityChanging(void)
{
	u32_t uiTestResult = 0 ;

	/* Loop for a while.. Let all pending ISRs to run */
	demoLoopForDelay(0x100);

    return uiTestResult;
}


/**
 *  Test # 6 - Test correct gateway (level-triggered or edge-triggered) behavior
 *
 * @return - returns 0 (success). In case of a failure the function enters an endless loop
 */
u32_t demoExtIntsTest6GatweayConfiguration(void)
{
	u32_t uiTestResult = 0 ;

	/* Loop for a while.. Let all pending ISRs to run */
	demoLoopForDelay(0x100);

	return uiTestResult;
}


/**
 *  Test # 7 - Test external interrupts with lower priority sources do not preempt higher source interrupt
 *
 * @return - returns 0 (success). In case of a failure the function enters an endless loop
 */
u32_t demoExtIntsTest7NestedInteeruptLowerPriority(void)
{
	u32_t uiTestResult = 0 ;

	/* Loop for a while.. Let all pending ISRs to run */
	demoLoopForDelay(0x100);

    return uiTestResult;
}


/**
 *  Test # 8 - Test external interrupts with same priority sources as current interrupt do not preempt it
 *
 * @return - returns 0 (success). In case of a failure the function enters an endless loop
 */
u32_t demoExtIntsTest8NestedInteeruptSamePriority(void)
{
	u32_t uiTestResult = 0 ;

	/* Loop for a while.. Let all pending ISRs to run */
	demoLoopForDelay(0x100);

    return uiTestResult;
}


/**
 *  Test # 9 - Test external interrupts with higher priority sources then current interrupt do preempt it
 *
 * @return - returns 0 (success). In case of a failure the function enters an endless loop
 */
u32_t demoExtIntsTest9NestedInteeruptHigherPriority(void)
{
	u32_t uiTestResult = 0 ;

	/* Loop for a while.. Let all pending ISRs to run */
	demoLoopForDelay(0x100);

    return uiTestResult;
}



/**
 * demoStart - startup point of the demo application. called from main function.
 *
 */
void demoStart(void)
{

#ifdef D_HI_FIVE1

	/* External interrupt testing is not relevant for Hifive1 */
	for( ;; );

#elif D_NEXYS_A7
   /* Register interrupt vector */
   M_PSP_WRITE_CSR(D_PSP_MTVEC_NUM, &psp_vect_table);

   /* Register external-interrupts vector */
   M_PSP_WRITE_CSR(D_PSP_MEIVT_NUM, G_Ext_Interrupt_Handlers);

   /* Register external interrupt ISR for source #0 - "no interrupt pending" case */
   /* TBD */

   /* Initialize test results array and parameters */
   demoInitializeTestResults();

   /* Test #1 - Global disable exernal interrups */
   demoExtIntsTest1GlobalDisabled();
   /* Test #2 - Disable specific exernal interrup */
   demoExtIntsTest2SpecificDisabled();
   /* Test #3 - Priority & Threshold - standard order */
   demoExtIntsTest3PriorityStandardOrder();
   /* Test #4 - Priority & Threshold - reversed order*/
   demoExtIntsTest4PriorityReversedOrder();
   /* Test #5 - Changing Priority & Threshold - */
   demoExtIntsTest5ThresholdAndPriorityChanging();
   /* Test #6 - Gateways Level/Edge setting*/
   demoExtIntsTest6GatweayConfiguration();
   /* Test #7 - Nested interrupts - lower priority */
   demoExtIntsTest7NestedInteeruptLowerPriority();
   /* Test #8 - Nested interrupts - same priority */
   demoExtIntsTest8NestedInteeruptSamePriority();
   /* Test #9 - Nested interrupts - higher priority */
   demoExtIntsTest9NestedInteeruptHigherPriority();

   /* Arriving here means all tests passed successfully */
   demoOutputMsg("External Interrupts tests passed successfully\n");

   /* Loop here to let debug */
   M_ENDLESS_LOOP();

#endif /* D_NEXYS_A7 */
}

