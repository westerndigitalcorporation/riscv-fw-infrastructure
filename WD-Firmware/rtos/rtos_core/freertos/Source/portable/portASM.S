/*
    FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution and was contributed
    to the project by Technolution B.V. (www.technolution.nl,
    freertos-riscv@technolution.eu) under the terms of the FreeRTOS
    contributors license.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry''s de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
   The file contents was modified by Western-Digital (Nati Rapaport) to support
   HiFive core with WD FW-infrastructure
*/

/*
This implementation supports riscv privileged v1.10
*/
#include "psp_core_base.inc"
.include "psp_macros.inc"
#include "rtosal_defines.inc"
#include "rtosal_macros.inc"

.global pxCurrentTCB
.global xPortStartScheduler
.global vPortEndScheduler
.global pxPortInitialiseStack

//.extern vTaskSwitchContext

xPortStartScheduler:
    jal pspTimerSetup
    M_RTOSAL_RESTORE_CONTEXT pxCurrentTCB, 0
    M_PSP_POP_REGFILE
    mret

vPortEndScheduler:
1:
    j 1b

.align 8
.func
pxPortInitialiseStack:
	csrr t0, mstatus					/* Obtain current mstatus value. */
    M_PSP_ADDI t1, x0, 0x188                /* Generate the value 0x1880, which are -   */
	slli t1, t1, 4                      /* the MPIE and MPP bits to set in mstatus. */
	or t0, t0, t1						/* Set MPIE and MPP bits in mstatus value. */

    M_PSP_ADDI a0, a0, -REGBYTES
    M_PSP_STORE t0, 0(a0)                         /* mstatus onto the stack. */
    M_PSP_ADDI a0, a0, -(22 * REGBYTES)           /* Space for registers x11-x31. */
    M_PSP_STORE a2, 0(a0)                         /* Task parameters (pvParameters parameter) goes into register X10/a0 on the stack. */
    M_PSP_ADDI a0, a0, -(6 * REGBYTES)            /* Space for registers x5-x9. */
    M_PSP_STORE x0, 0(a0)                         /* Return address onto the stack */
// The following is relevant only when there are "aditional registers" in the core to handle. For now irrelevant hence commented out */
// It was left here commented as a reference only
//    M_PSP_ADDI t0, x0, D_NUM_OF_ADDITIONAL_REGISTERS /* The number of chip specific additional registers. */
//chip_specific_stack_frame:                       /* First add any chip specific registers to the stack frame being created. */
//    beq t0, x0, 1f                               /* No more chip specific registers to save. */
//    M_PSP_ADDI a0, a0, -REGBYTES                     /* Make space for chip specific register. */
//    M_PSP_STORE x0, 0(a0)                            /* Give the chip specific register an initial value of zero. */
//    M_PSP_ADDI t0, t0, -1                            /* Decrement the count of chip specific registers remaining. */
//    j chip_specific_stack_frame                  /* Until no more chip specific registers. */
//1:                                               */
    M_PSP_ADDI a0, a0, -REGBYTES
    M_PSP_STORE a1, 0(a0)                         /* mret value (pxCode parameter) onto the stack. */
    ret
.endfunc

