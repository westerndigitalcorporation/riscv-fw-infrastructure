/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the RISC-V RV32 port.
 *----------------------------------------------------------*/
/* Standard Includes */
#include <unistd.h>
#include <stdlib.h>

#include "psp_macros.h"
#include "psp_defines.h"
#include "psp_types.h"

extern void pspHandleEcall();


/***************************************************************************************************
*
* @brief Function that called upon unregistered Trap handler
*
***************************************************************************************************/
void pspTrapUnhandled(void)
{
	uint32_t local_mepc,local_mcause;
	//exit(M_READ_CSR(mcause));
	local_mepc = M_READ_CSR(mepc);
	local_mcause = M_READ_CSR(mcause);
	if (0 == local_mepc || 0 == local_mcause){}
	//write(1, "Unhandeled exc\n", 15);
	asm volatile ("ebreak" : : : );
}

/***************************************************************************************************
*
* @brief Handler of ECALL exception
*
***************************************************************************************************/
void pspEcallHandler(void)
{
	pspHandleEcall();
}


/***************************************************************************************************
*
* @brief Setup function for M-Timer in the CLINT (per priviliged spec)
*
***************************************************************************************************/
void pspSetupTimerSingleRun(const unsigned int enable)
{
	//demoOutputMsg("SETUP Timer\n", 12);

    #if !defined(D_MTIME_ADDRESS) || !defined(D_MTIMECMP_ADDRESS)
       #error "MTIME/MTIMECMP address definition is missing"
    #endif
    #if !defined(D_RTC_CLOCK_HZ) || !defined(D_TICK_RATE_HZ)
       #error "Core frequency values definitions are missing"
    #endif

     // Set the machine timer
    volatile uint64_t * mtime       = (uint64_t*)D_MTIME_ADDRESS;
    volatile uint64_t * mtimecmp    = (uint64_t*)D_MTIMECMP_ADDRESS;
    uint64_t now = *mtime;
    uint64_t then = now + (D_RTC_CLOCK_HZ / D_TICK_RATE_HZ);
    *mtimecmp = then;

    if (D_PSP_TRUE == enable)
    {
        // Enable the Machine-Timer bit in MIE
        M_SET_CSR(mie, D_MIP_MTIP);
    }
}




