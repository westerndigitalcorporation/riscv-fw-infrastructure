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
* @file   psp_timer.c
* @author Nati Rapaport
* @date   13.11.2019
* @brief  This file implaments core's timer service functions
*
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the RISC-V RV32 port.
 *----------------------------------------------------------*/
#include "psp_types.h"
#include "psp_api.h"


/**
*
* @brief Setup function for M-Timer in the CLINT (per priviliged spec)
*
***************************************************************************************************/
void pspTimerSetupSingleRun(const unsigned int enableInterrupt)
{
	//demoOutputMsg("SETUP Timer\n", 12);

    #if !defined(D_MTIME_ADDRESS) || !defined(D_MTIMECMP_ADDRESS)
       #error "MTIME/MTIMECMP address definition is missing"
    #endif
    #if !defined(D_CLOCK_RATE) || !defined(D_TICK_TIME_MS)
       #error "Core frequency values definitions are missing"
    #endif

     // Set the machine timer
    volatile u64_t * mtime       = (u64_t*)D_MTIME_ADDRESS;
    volatile u64_t * mtimecmp    = (u64_t*)D_MTIMECMP_ADDRESS;
    u64_t now = *mtime;
    u64_t then = now + (D_CLOCK_RATE * D_TICK_TIME_MS / D_PSP_MSEC);
    *mtimecmp = then;

    if (D_PSP_TRUE == enableInterrupt)
    {
        // Enable the Machine-Timer interrupt bit in MIE CSR
        M_PSP_SET_CSR(mie, D_PSP_MIP_MTIP);
    }
}




