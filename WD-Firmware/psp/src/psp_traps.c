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

#include "../api_inc/psp_macros.h"
/* Scheduler includes. */
//#include "FreeRTOS.h"
#include "psp_rtos_api.h"
#include "psp_defines.h"
#include "rtosal_services_api.h"
#include "psp_specific_definitions.h"

extern void pspHandleEcall();


#ifndef configCLINT_BASE_ADDRESS
	#warning configCLINT_BASE_ADDRESS must be defined in FreeRTOSConfig.h.  If the target chip includes a Core Local Interrupter (CLINT) then set configCLINT_BASE_ADDRESS to the CLINT base address.  Otherwise set configCLINT_BASE_ADDRESS to 0.
#endif

/* Let the user override the pre-loading of the initial LR with the address of
prvTaskExitError() in case it messes up unwinding of the stack in the
debugger. */
#ifdef configTASK_RETURN_ADDRESS
	#define pspTASK_RETURN_ADDRESS	configTASK_RETURN_ADDRESS
#else
	#define pspTASK_RETURN_ADDRESS	prvTaskExitError
#endif

/* The stack used by interrupt service routines.  Set configISR_STACK_SIZE_WORDS
to use a statically allocated array as the interrupt stack.  Alternative leave
configISR_STACK_SIZE_WORDS undefined and update the linker script so that a
linker variable names __freertos_irq_stack_top has the same value as the top
of the stack used by main.  Using the linker script method will repurpose the
stack that was used by main before the scheduler was started for use as the
interrupt stack after the scheduler has started. */
#ifdef configISR_STACK_SIZE_WORDS
	static __attribute__ ((aligned(16))) StackType_t xISRStack[ configISR_STACK_SIZE_WORDS ] = { 0 };
	const StackType_t xISRStackTop = ( StackType_t ) &( xISRStack[ ( configISR_STACK_SIZE_WORDS /*& ~pspBYTE_ALIGNMENT_MASK*/ ) - 1 ] );
#else
	extern const uint32_t __freertos_irq_stack_top[];
	const StackType_t xISRStackTop = ( StackType_t ) __freertos_irq_stack_top;
#endif


/* Used to program the machine timer compare register. */
uint64_t ullNextTime = 0ULL;
const uint64_t *pullNextTime = &ullNextTime;
const size_t uxTimerIncrementsForOneTick = ( size_t ) ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ); /* Assumes increment won't go over 32-bits. */

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
	/*write(1,"ECALL\n", 6);*/
	pspHandleEcall();
}

/***************************************************************************************************
*
* @brief Handler of M-Timer interrupt
*
***************************************************************************************************/
void pspMTimerIntHandler(void)
{
	static uint64_t then = 0;

	//write(1,"MTIMER int\n", 11);
	M_CLEAR_CSR(mie, D_MIP_MTIP); /* clear timer interrupt indication */
    volatile uint64_t * mtime       = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIME); /* mtime - pointer to mtime field in CLINT */
    volatile uint64_t * mtimecmp    = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIMECMP); /* mtimecmp - pointer to mtimecmp field in CLINT */

    /* Configure CLINT for the next timer interrupt: */
	if(then != 0)  {
		//next timer irq is 1 second from previous
		then += (configRTC_CLOCK_HZ / configTICK_RATE_HZ);
	} else{ //first time setting the timer
		uint64_t now = *mtime;
		then = now + (configRTC_CLOCK_HZ / configTICK_RATE_HZ);
	}
	*mtimecmp = then;

   /* Increment the RTOS tick. */
#ifndef D_USE_RTOSAL
   #ifdef D_USE_FREERTOS
	   if( xTaskIncrementTick() != pdFALSE )
	   {
	      vTaskSwitchContext();
	   }
   #else
	   #error "Need to add a direct RTOS call herein"
   #endif  /* D_USE_FREERTOS */
#else
	rtosalTick();
#endif /* D_USE_RTOSAL */
	M_SET_CSR(mie, D_MIP_MTIP);
}

/***************************************************************************************************
*
* @brief Setup function for M-Timer in the CLINT (per priviliged spec)
*
***************************************************************************************************/
void pspSetupTimer(void)
{
#ifdef D_pspHasCLINT
	write(1,"SETUP Timer\n", 12);

    // Set the machine timer
    volatile uint64_t * mtime       = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIME);
    volatile uint64_t * mtimecmp    = (uint64_t*) (CLINT_CTRL_ADDR + CLINT_MTIMECMP);
    uint64_t now = *mtime;
    uint64_t then = now + (configRTC_CLOCK_HZ / configTICK_RATE_HZ);
    *mtimecmp = then;

    // Enable the Machine-Timer bit in MIE
    M_SET_CSR(mie, D_MIP_MTIP);
#else
    #error "Need to define timer interrupt implementation when no CLINT in the core"
    /* TBD for future specs*/
#endif
}





