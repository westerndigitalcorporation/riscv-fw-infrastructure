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
#include "psp_macros.inc"
#include "rtosal_defines.inc"
#include "rtosal_macros.inc"

#ifdef __clang__
/* disable warning for reserved registers use - we are using comrv
   reserved register and don't want to see these warnings. */
.option nowarnreservedreg
#endif /* __clang__ */

.global pxCurrentTCB
.global xPortStartScheduler
.global vPortEndScheduler
.global pxPortInitialiseStack
.global rtosalUpdateStackPriorContextSwitch
.global vPortUpdateStackPriorContextSwitch

xPortStartScheduler:
    jal rtosalTimerSetup
    M_RTOSAL_RESTORE_CONTEXT pxCurrentTCB, 0
    M_PSP_POP_REGFILE
    /* TODO: replace trap handler to rtosal handler + change csrrsi
       to be disable ints and restore ints */
    /* this is the first task being executed
       so take the task entry address from mepc */
    csrr   ra, mepc
    /* set MIE to enable interrupts - must be the last
       instruction before the ret */
    csrrsi zero, mstatus, 8
    ret

vPortEndScheduler:
1:
    j 1b

.align 8
pxPortInitialiseStack:
#ifdef D_COMRV
   M_PSP_ADDI  sp, sp, -16
   M_PSP_STORE ra, 0(sp)                  /* save ra */
   M_PSP_STORE a0, 4(sp)                  /* save a0 */
   M_PSP_STORE a1, 8(sp)                  /* save a1 */
   jal comrvInitApplicationStack          /* we need to initialize t3 register (comrv stack) per task */
   M_PSP_LOAD  ra, 0(sp)                  /* restore ra */
   M_PSP_LOAD  a0, 4(sp)                  /* restore a0 */
   M_PSP_LOAD  a1, 8(sp)                  /* restore a1 */
   M_PSP_ADDI  sp, sp, 16
   mv          tp, a0                     /* tp holds the stack used by COMRV per task to
                                             store/restore callee args + comrv status */
   M_PSP_STORE zero, 0(tp)                /* clear the comrv status */
   M_PSP_ADDI  tp, tp, -(27 * REGBYTES)   /* 24 * REGBYTES - stack reserved per task for COMRV */
   M_PSP_ADDI  a0, a0, -(28 * REGBYTES)   /* sp address */
   M_PSP_STORE t3, -(5 * REGBYTES)(a0)    /* save t3 - comrv stack per task */
   M_PSP_STORE tp, -(4 * REGBYTES)(a0)    /* save tp */
#endif /* D_COMRV */

   M_PSP_STORE a2, -(23 * REGBYTES)(a0)   /* Task parameters (pvParameters parameter) goes into register X10/a0 on the stack. */
   M_PSP_STORE x0, -(29 * REGBYTES)(a0)   /* Return address onto the stack */
// The following is relevant only when there are "aditional registers" in the core to handle. For now irrelevant hence commented out */
// It was left here commented as a reference only
//    M_PSP_ADDI t0, x0, D_NUM_OF_ADDITIONAL_REGISTERS /* The number of chip specific additional registers. */
//chip_specific_stack_frame:              /* First add any chip specific registers to the stack frame being created. */
//    beq t0, x0, 1f                      /* No more chip specific registers to save. */
//    M_PSP_ADDI a0, a0, -REGBYTES        /* Make space for chip specific register. */
//    M_PSP_STORE x0, 0(a0)               /* Give the chip specific register an initial value of zero. */
//    M_PSP_ADDI t0, t0, -1               /* Decrement the count of chip specific registers remaining. */
//    j chip_specific_stack_frame         /* Until no more chip specific registers. */
//1:

   M_PSP_ADDI  a0, a0, -(32 * REGBYTES)    /* a0 to be returned to caller */
   M_PSP_STORE a1, 0(a0)                  /* mret value (pxCode parameter) onto the stack. */

   ret

vPortUpdateStackPriorContextSwitch:
   j  rtosalUpdateStackPriorContextSwitch
