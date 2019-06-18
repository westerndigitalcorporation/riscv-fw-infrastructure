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
#include "psp_macro_asm.inc"

.macro SET_INT_CONTEXT
    /* save address of g_uiIsInterruptContext -> t0 */
    la      t0, g_uiIsInterruptContext
    /* load the value g_uiIsInterruptContext -> t1 */
    LOAD    t1, 0x0(t0)
    /* increment t1 by 1 */
    ADDI    t1, t1, 1
    /* store new value to g_uiIsInterruptContext */
    STORE   t1, 0x0(t0)
.endm

.macro CLEAR_INT_CONTEXT
    /* save address of g_uiIsInterruptContext -> t0 */
    la      t0, g_uiIsInterruptContext
    /* load the value g_uiIsInterruptContext -> t1 */
    LOAD    t1, 0x0(t0)
    /* decrement t1 by 1 */
    ADDI    t1, t1, -1
    /* store new value to g_uiIsInterruptContext */
    STORE   t1, 0x0(t0)
.endm

/*saves mstatus and tcb    */
.macro portSAVE_CONTEXT
    .global    pxCurrentTCB
    /* Store mstatus */
    csrr    t0, mstatus    //pointer
    STORE   t0, 32 * REGBYTES(sp)
    /* Store current stackpointer in task control block (TCB) */
    LOAD    t0, pxCurrentTCB    //pointer
    STORE   sp, 0x0(t0)
.endm

 /*restore mstatus and tcb    */
.macro portRESTORE_CONTEXT
    .global    pxCurrentTCB
    /* Load stack pointer from the current TCB */
    LOAD    sp, pxCurrentTCB
    LOAD    sp, 0x0(sp)
    /* Load task program counter */
    LOAD    t0, 33 * REGBYTES(sp)
    csrw    mepc, t0
    /* Load saved mstatus */
    LOAD    t0, 32 * REGBYTES(sp)
    csrw    mstatus, t0
.endm

.macro portEND_SWITCHING_ISR brunch_label
    /* save address of g_rtosalContextSwitch -> t0 */
    la      t0, g_rtosalContextSwitch
    /* load the value g_rtosalContextSwitch -> t1 */
    LOAD    t1, 0x0(t0)
    /* check if g_rtosalContextSwitch is set - need to do context switch */
    beqz    t1, \brunch_label
    /* clear g_rtosalContextSwitch */
    STORE   zero, 0x0(t0)
    /* perform context switch */
    j       vPortYieldFromInterrupt
.endm

.section  .text.entry
.align 4
.global   vect_table_

.ifndef D_RTOSAL_VECT_TABLE
.global   vect_table
vect_table:
    pushREGFILE
    SET_INT_CONTEXT
    csrr  t0, mcause
    bge      t0, zero, vect_table_
    slli  t0, t0, 2
    la    t1, vect_table_
    add   t0, t0, t1
    jr    t0
.endif /* #if D_RTOSAL_VECT_TABLE */

.ifndef D_RTOSAL_VECT_TABLE
vect_table_:
.else
vect_table:
.endif /* #if D_RTOSAL_VECT_TABLE */
    j exceptions_int
    .align 2
    j reserved_int
    .align 2
    j reserved_int
    .align 2
    j m_soft_int
    .align 2
    j reserved_int
    .align 2
    j reserved_int
    .align 2
    j reserved_int
    .align 2
    j m_timer_int
    .align 2
    j reserved_int
    .align 2
    j reserved_int
    .align 2
    j reserved_int
    .align 2
    j m_external_int
    .align 2

exceptions_int:
.ifdef D_RTOSAL_VECT_TABLE
    pushREGFILE
    SET_INT_CONTEXT
.endif /* D_RTOSAL_VECT_TABLE */
    /* pass mcause in a0 */
    /* csrr a0, mcause */
    /* pass sp in a1 */
    /* mv   a1,sp */
    /* call the exception handler */
    CALL_INT_HANDLER fptrIntExceptionIntHandler
    /* adjust stack pointer back to
       where it was prior to ulSynchTrap  */
    /*mv    sp,a0*/
    CLEAR_INT_CONTEXT
    popREGFILE
    mret

m_soft_int:
.ifdef D_RTOSAL_VECT_TABLE
    pushREGFILE
    SET_INT_CONTEXT
.endif /* D_RTOSAL_VECT_TABLE */
    CALL_INT_HANDLER fptrIntMSoftIntHandler
    portEND_SWITCHING_ISR m_soft_int_no_cs
m_soft_int_no_cs:
    CLEAR_INT_CONTEXT
    popREGFILE
    mret

m_timer_int:
.ifdef D_RTOSAL_VECT_TABLE
    pushREGFILE
    SET_INT_CONTEXT
.endif /* D_RTOSAL_VECT_TABLE */
    portSAVE_CONTEXT
    portSAVE_EPC
    CALL_INT_HANDLER fptrIntMTimerIntHandler
    portRESTORE_CONTEXT
    CLEAR_INT_CONTEXT
    popREGFILE
    mret

m_external_int:
.ifdef D_RTOSAL_VECT_TABLE
    pushREGFILE
    SET_INT_CONTEXT
.endif /* D_RTOSAL_VECT_TABLE */
    CALL_INT_HANDLER fptrIntMExternIntHandler
    portRESTORE_X2
    portEND_SWITCHING_ISR m_external_int_no_cs
m_external_int_no_cs:
    CLEAR_INT_CONTEXT
    popREGFILE
    mret

.weak reserved_int
reserved_int:
1:
  j 1b
