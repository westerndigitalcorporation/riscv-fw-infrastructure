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
#include <stdlib.h>
#include "common_types.h"
#include "psp_api.h"
#include "psp_swerv_ehx1_csrs.h"
#include "psp_interrupt_api_ehx1.h"
#include "demo_platform_al.h"

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

extern void psp_vect_table(void);

/**
* global variables
*/
volatile u32_t g_uDemoTimer0InterruptCount = 0;
volatile u32_t g_uDemoTimer0Count = 0;

volatile u32_t g_uDemoTimer1InterruptCount = 0;
volatile u32_t g_uDemoTimer1Count = 0;

/**
* functions
*/
/* Timer0 functions */
void demoSetupTimer0(void);
void demoTimer0IntHandler(void);

/* Timer1 functions */
void demoSetupTimer1(void);
void demoTimer1IntHandler(void);

/**
 * demoStart - startup point of the demo application. called from main function.
 *
 */
void demoStart(void)
{
   /* Register interrupt vector */
   M_PSP_WRITE_CSR(mtvec, &psp_vect_table);

   demoSetupTimer0();
   demoSetupTimer1();

   while(1)
   {
	   g_uDemoTimer0Count = M_PSP_READ_PS_CSR(D_PSP_MITCNT0_NUM);
	   g_uDemoTimer1Count = M_PSP_READ_PS_CSR(D_PSP_MITCNT1_NUM);

   };
}


/**************************************************************************************
 Timer0 PSP test
***************************************************************************************/
void demoSetupTimer0(void)
{
	g_uDemoTimer0Count = M_PSP_READ_PS_CSR(D_PSP_MITCNT0_NUM);

	/* Register Timer0 interrupt handler */
    pspRegisterInterruptHandler(demoTimer0IntHandler, E_MACHINE_INTERNAL_TIMER0_CAUSE);

	/* Setup Timer0 to start running */

    /* Disable Timer0 interrupt and countup */
    M_PSP_DISABLE_TIMER0_INT();
    M_PSP_DISABLE_TIMER0_CNT();
    /* Zero Timer0 count register */
    M_PSP_WRITE_PS_CSR(D_PSP_MITCNT0_NUM,0);
    /* Set Timer0 limit countup */
    M_PSP_WRITE_PS_CSR(D_PSP_MITBND0_NUM, 0xFF0000);
    /* Enable Timer0 interrupt and countup */
    M_PSP_ENABLE_TIMER0_INT();
    M_PSP_ENABALE_TIMER0_CNT();
}


void demoTimer0IntHandler()
{
	char strMsg[] = "Timer0 interrupt fired: " ;
	char numberToDisplay[2];

	g_uDemoTimer0InterruptCount++;

	itoa(g_uDemoTimer0InterruptCount, numberToDisplay, sizeof(numberToDisplay));
	demoOutputMsg(strMsg,sizeof(strMsg));
	demoOutputMsg(numberToDisplay,2);
	demoOutputMsg("\n",1);
}


/**************************************************************************************
 Timer1 PSP test
***************************************************************************************/
void demoSetupTimer1(void)
{
	g_uDemoTimer1Count = M_PSP_READ_PS_CSR(D_PSP_MITCNT1_NUM);

	/* Register Timer1 interrupt handler */
    pspRegisterInterruptHandler(demoTimer1IntHandler, E_MACHINE_INTERNAL_TIMER1_CAUSE);

	/* Setup Timer1 to start running */

    /* Disable Timer1 interrupt and countup */
    M_PSP_DISABLE_TIMER1_INT();
    M_PSP_DISABLE_TIMER1_CNT();
    /* Zero Timer1 count register */
    M_PSP_WRITE_PS_CSR(D_PSP_MITCTL1_NUM,0);
    /* Set Timer1 limit countup */
    M_PSP_WRITE_PS_CSR(D_PSP_MITBND1_NUM, 0xFF0000);
    /* Enable Timer1 interrupt and countup */
    M_PSP_ENABLE_TIMER1_INT();
    M_PSP_ENABALE_TIMER1_CNT();
}


void demoTimer1IntHandler()
{
	char strMsg[] = "Timer1 interrupt fired: " ;
	char numberToDisplay[2];

	g_uDemoTimer1InterruptCount++;

	itoa(g_uDemoTimer1InterruptCount, numberToDisplay, sizeof(numberToDisplay));
	demoOutputMsg(strMsg,sizeof(strMsg));
	demoOutputMsg(numberToDisplay,2);
	demoOutputMsg("\n",1);
}
