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
* @file   psp_timer_swerv_ehx1.c
* @author Nati Rapaport
* @date   8.12.2019
* @brief  This file implements SweRV_EHX1 timer service functions
*
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the RISC-V RV32 port.
 *----------------------------------------------------------*/
#include "psp_types.h"
#include "psp_api.h"
#include "psp_swerv_ehx1_csrs.h"

/**
*
* @brief Setup function for M-Timer. Called upon initialization of the system
*
***************************************************************************************************/
void pspTimerSetup(void)
{
    D_PSP_SETUP_SINGLE_TIMER_RUN(D_PSP_TRUE);
}

/**
*
* @brief Setup function for Timer0 (per SweRV-EHX1 reference manual)
*
***************************************************************************************************/
void pspTimerSetupSingleRun(const unsigned int enableInterrupt)
{
	/* Set the machine timer0 */
    u32_t now = M_PSP_READ_PS_CSR(D_PSP_MITCNT0);
    u32_t then = now + (D_CLOCK_RATE * D_TICK_TIME_MS / D_PSP_MSEC);
    M_PSP_WRITE_PS_CSR(D_PSP_MITBND0, then);

    if (D_PSP_TRUE == enableInterrupt)
    {
	    /* Enable Timer0 counting */
	    M_PSP_SET_PS_CSR(D_PSP_MITCTL0, D_PSP_MITCTL_EN);

	    // Enable the Machine-Timer-0 interrupt bit in MIE CSR
	    M_PSP_ENABLE_TIMER0_INT();
    }
}




