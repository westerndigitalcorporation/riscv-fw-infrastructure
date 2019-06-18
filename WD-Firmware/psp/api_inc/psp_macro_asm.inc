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

#include "psp_defines.h"

.macro CALL_INT_HANDLER fptIntHandler
    /* load the address of provided fptIntHandler */
    la t0, \fptIntHandler
    /* load the actual handler address */
    LOAD t0, 0x0(t0)
    /* invoke the interrupt handler */
    jalr t0
.endm

/* Macro for saving task context */
.macro pushREGFILE
    /* make room in stack */
    ADDI	sp, sp, -REGBYTES * 34
    /* Save Context */
    STORE	x1, 1 * REGBYTES(sp)
    STORE	x2, 2 * REGBYTES(sp)
    /* STORE	x3, 3 * REGBYTES(sp) */
    STORE	x4, 4 * REGBYTES(sp)
    STORE	x5, 5 * REGBYTES(sp)
    STORE	x6, 6 * REGBYTES(sp)
    STORE	x7, 7 * REGBYTES(sp)
    STORE	x8, 8 * REGBYTES(sp)
    STORE	x9, 9 * REGBYTES(sp)
    STORE	x10, 10 * REGBYTES(sp)
    STORE	x11, 11 * REGBYTES(sp)
    STORE	x12, 12 * REGBYTES(sp)
    STORE	x13, 13 * REGBYTES(sp)
    STORE	x14, 14 * REGBYTES(sp)
    STORE	x15, 15 * REGBYTES(sp)
    STORE	x16, 16 * REGBYTES(sp)
    STORE	x17, 17 * REGBYTES(sp)
    STORE	x18, 18 * REGBYTES(sp)
    STORE	x19, 19 * REGBYTES(sp)
    STORE	x20, 20 * REGBYTES(sp)
    STORE	x21, 21 * REGBYTES(sp)
    STORE	x22, 22 * REGBYTES(sp)
    STORE	x23, 23 * REGBYTES(sp)
    STORE	x24, 24 * REGBYTES(sp)
    STORE	x25, 25 * REGBYTES(sp)
    STORE	x26, 26 * REGBYTES(sp)
    STORE	x27, 27 * REGBYTES(sp)
    STORE	x28, 28 * REGBYTES(sp)
    STORE	x29, 29 * REGBYTES(sp)
    STORE	x30, 30 * REGBYTES(sp)
    STORE	x31, 31 * REGBYTES(sp)
.endm

/* Saves current error program counter (EPC) as task program counter */
.macro portSAVE_EPC
    csrr	t0, mepc
    STORE	t0, 33 * REGBYTES(sp)
.endm

/* Saves current return adress (RA) as task program counter */
.macro portSAVE_RA
    LOAD	t0, 1 * REGBYTES(sp)
    STORE	t0, 33 * REGBYTES(sp)
.endm

/*/not called on a context switch*/
.macro portRESTORE_X2
    LOAD 	x2, 2 * REGBYTES(sp)
    //LOAD 	x3, 3 * REGBYTES(sp)
.endm

/* Macro for restoring task context */
.macro popREGFILE
    /* Restore registers,
    Skip global pointer because that does not change */
    LOAD	x1, 1 * REGBYTES(sp)
    /* LOAD 	x3, 3 * REGBYTES(sp) */
    LOAD	x4, 4 * REGBYTES(sp)
    LOAD	x5, 5 * REGBYTES(sp)
    LOAD	x6, 6 * REGBYTES(sp)
    LOAD	x7, 7 * REGBYTES(sp)
    LOAD	x8, 8 * REGBYTES(sp)
    LOAD	x9, 9 * REGBYTES(sp)
    LOAD	x10, 10 * REGBYTES(sp)
    LOAD	x11, 11 * REGBYTES(sp)
    LOAD	x12, 12 * REGBYTES(sp)
    LOAD	x13, 13 * REGBYTES(sp)
    LOAD	x14, 14 * REGBYTES(sp)
    LOAD	x15, 15 * REGBYTES(sp)
    LOAD	x16, 16 * REGBYTES(sp)
    LOAD	x17, 17 * REGBYTES(sp)
    LOAD	x18, 18 * REGBYTES(sp)
    LOAD	x19, 19 * REGBYTES(sp)
    LOAD	x20, 20 * REGBYTES(sp)
    LOAD	x21, 21 * REGBYTES(sp)
    LOAD	x22, 22 * REGBYTES(sp)
    LOAD	x23, 23 * REGBYTES(sp)
    LOAD	x24, 24 * REGBYTES(sp)
    LOAD	x25, 25 * REGBYTES(sp)
    LOAD	x26, 26 * REGBYTES(sp)
    LOAD	x27, 27 * REGBYTES(sp)
    LOAD	x28, 28 * REGBYTES(sp)
    LOAD	x29, 29 * REGBYTES(sp)
    LOAD	x30, 30 * REGBYTES(sp)
    LOAD	x31, 31 * REGBYTES(sp)
    addi	sp, sp, REGBYTES * 34
.endm