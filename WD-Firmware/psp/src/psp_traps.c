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


/* The stack used by interrupt service routines.  Set configISR_STACK_SIZE_WORDS
to use a statically allocated array as the interrupt stack.  Alternative leave
configISR_STACK_SIZE_WORDS undefined and update the linker script so that a
linker variable names __freertos_irq_stack_top has the same value as the top
of the stack used by main.  Using the linker script method will repurpose the
stack that was used by main before the scheduler was started for use as the
interrupt stack after the scheduler has started. */
#ifdef D_ISR_STACK_SIZE_WORDS
	static __attribute__ ((aligned(16))) pspStack_t xISRStack[ D_ISR_STACK_SIZE_WORDS ] = { 0 };
	const pspStack_t xISRStackTop = ( pspStack_t ) &( xISRStack[ ( D_ISR_STACK_SIZE_WORDS ) - 1 ] );
#else
	extern const uint32_t __freertos_irq_stack_top[];
	const pspStack_t xISRStackTop = ( pspStack_t ) __freertos_irq_stack_top;
#endif


/* Set configCHECK_FOR_STACK_OVERFLOW to 3 to add ISR stack checking to task
stack checking.  A problem in the ISR stack will trigger an assert, not call the
stack overflow hook function (because the stack overflow hook is specific to a
task stack, not the ISR stack). */
#if( configCHECK_FOR_STACK_OVERFLOW > 2 )
	#warning This path not tested, or even compiled yet.
	/* Don't use 0xa5 as the stack fill bytes as that is used by the kernerl for
	the task stacks, and so will legitimately appear in many positions within
	the ISR stack. */
	#define pspISR_STACK_FILL_BYTE	0xee

	static const uint8_t ucExpectedStackBytes[] = {
									pspISR_STACK_FILL_BYTE, pspISR_STACK_FILL_BYTE, pspISR_STACK_FILL_BYTE, pspISR_STACK_FILL_BYTE,		\
									pspISR_STACK_FILL_BYTE, pspISR_STACK_FILL_BYTE, pspISR_STACK_FILL_BYTE, pspISR_STACK_FILL_BYTE,		\
									pspISR_STACK_FILL_BYTE, pspISR_STACK_FILL_BYTE, pspISR_STACK_FILL_BYTE, pspISR_STACK_FILL_BYTE,		\
									pspISR_STACK_FILL_BYTE, pspISR_STACK_FILL_BYTE, pspISR_STACK_FILL_BYTE, pspISR_STACK_FILL_BYTE,		\
									pspISR_STACK_FILL_BYTE, pspISR_STACK_FILL_BYTE, pspISR_STACK_FILL_BYTE, pspISR_STACK_FILL_BYTE };	\

	#define pspCHECK_ISR_STACK() configASSERT( ( memcmp( ( void * ) xISRStack, ( void * ) ucExpectedStackBytes, sizeof( ucExpectedStackBytes ) ) == 0 ) )
#else
	/* Define the function away. */
	#define pspCHECK_ISR_STACK()
#endif /* configCHECK_FOR_STACK_OVERFLOW > 2 */


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




