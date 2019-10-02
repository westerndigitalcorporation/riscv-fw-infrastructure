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
This implementation supports riscv privileged v1.10
*/
<<<<<<< HEAD:WD-Firmware/rtos/rtosal/src/rtosal_int_vect.S
#include "psp_asm_macros.h"
#include "psp_specific_definitions.h"
#include "psp_specific_macros.h"



/* The following macros were here before. I moved them to psp_asm_macros.h
//.macro m_RESTORE_CONTEXT
//.macro m_END_SWITCHING_ISR brunch_label
//.macro m_SAVE_EPC
//.macro m_SAVE_RA
//.macro m_RESTORE_SP
*/
=======
.include "psp_asm_macros.h"

/* saves mstatus and tcb */
.macro m_portSAVE_CONTEXT
    .global    pxCurrentTCB
    /* Store mstatus */
    csrr      t0, mstatus
    m_STORE   t0, 32 * REGBYTES(sp)
    /* Store current stackpointer in task control block (TCB) */
    m_LOAD    t0, pxCurrentTCB
    m_STORE   sp, 0x0(t0)
.endm

 /* restore mstatus and tcb */
.macro m_portRESTORE_CONTEXT
    .global    pxCurrentTCB
    /* Load stack pointer from the current TCB */
    m_LOAD    sp, pxCurrentTCB
    m_LOAD    sp, 0x0(sp)
    /* Load task program counter */
    m_LOAD    t0, 33 * REGBYTES(sp)
    csrw      mepc, t0
    /* Load saved mstatus */
    m_LOAD    t0, 32 * REGBYTES(sp)
    csrw      mstatus, t0
.endm

.macro m_portEND_SWITCHING_ISR brunch_label
    /* save address of g_rtosalContextSwitch -> a0 */
    la        a0, g_rtosalContextSwitch
    /* load the value g_rtosalContextSwitch -> a1 */
    m_LOAD    a1, 0x0(a0)
    /* check if g_rtosalContextSwitch is set - need to do context switch */
    beqz      a1, \brunch_label
    /* clear g_rtosalContextSwitch */
    /* TODO: if bitmanip exist add bit set */
    m_STORE   zero, 0x0(a0)
    /* perform context switch */
.if D_USE_FREERTOS
    m_portSAVE_CONTEXT
    jal     vTaskSwitchContext
    m_portRESTORE_CONTEXT
.elseif D_USE_THREADX
.endif /* .if D_USE_FREERTOS */
.endm

/* Saves current Machine Exception Program Counter (MEPC) as task program counter */
.macro m_portSAVE_EPC
    csrr      t0, mepc
    m_STORE   t0, 33 * REGBYTES(sp)
.endm

/* Saves current return adress (RA) as task program counter */
.macro m_portSAVE_RA
    LOAD      t0, 1 * REGBYTES(sp)
    m_STORE   t0, 33 * REGBYTES(sp)
.endm

/*/not called on a context switch*/
.macro m_portRESTORE_SP
    m_LOAD    x2, 2 * REGBYTES(sp)
.endm
>>>>>>> 02376fdecd7bf2f9adf18afb271eca43c9ee6872:WD-Firmware/rtos/rtosal/rtosal_int_vect.S

.section  .text.entry
.align 4
.global   rtosal_vect_table

.ifndef D_RTOSAL_VECT_TABLE
rtosal_vect_table:
    m_pushREGFILE
    m_SET_INT_CONTEXT
    m_pushAdditionalRegs
    m_SAVE_CONTEXT
    m_SET_SP_TO_ISR_STACK /* After RegFile is pushed onto Task's-Stack, we change sp to point to ISR-Stack */
    csrr    t0, mcause
    bge     t0, zero, rtosal_vect_table_
    slli    t0, t0, 2
    la      t1, rtosal_vect_table_
    add     t0, t0, t1
    jr      t0
.endif /* #if D_RTOSAL_VECT_TABLE */

.align 4
.ifndef D_RTOSAL_VECT_TABLE
rtosal_vect_table_:
.else
rtosal_vect_table:
.endif /* #if D_RTOSAL_VECT_TABLE */
    j       rtosa_exceptions_int       /* User software interrupt & exceptions */
    .align 2
    j       rtosa_reserved_int         /* Supervisor software interrupt        */
    .align 2
    j       rtosa_reserved_int         /* Reserved for future standard use     */
    .align 2
    j       rtosa_m_soft_int           /* Machine software interrupt           */
    .align 2
    j       rtosa_reserved_int         /* User timer interrupt                 */
    .align 2
    j       rtosa_reserved_int         /* Supervisor timer interrupt           */
    .align 2
    j       rtosa_reserved_int         /* Reserved for future standard use     */
    .align 2
    j       rtosa_m_timer_int          /* Machine timer interrupt              */
    .align 2
    j       rtosa_reserved_int         /* User external interrupt              */
    .align 2
    j       rtosa_reserved_int         /* Supervisor external interrupt        */
    .align 2
    j       rtosa_reserved_int         /* Reserved for future standard use     */
    .align 2
    j       rtosa_m_external_int       /* Machine external interrupt           */
    .align 2

rtosa_exceptions_int:
.ifdef D_RTOSAL_VECT_TABLE
    m_pushREGFILE                                     /* Push registers of current task onto stack */
    m_SET_INT_CONTEXT                                 /* Mark we are in interrupt context */
    m_pushAdditionalRegs
    m_SAVE_CONTEXT
    m_SET_SP_TO_ISR_STACK                             /* After RegFile is pushed onto task's-stack, we change sp to point to ISR-Stack */
.endif /* D_RTOSAL_VECT_TABLE */
    m_CALL_INT_HANDLER g_fptrIntExceptionIntHandler   /* Call the exception handler */
    m_SET_SP_TO_TASK_STACK                            /* Just before pop RegFile of the interrupted task - change sp to point to current Task-Stack */
    m_CLEAR_INT_CONTEXT                               /* Clear interrupt context indication */
    m_popREGFILE                                      /* Restore the registers of current task from the stack */
    m_popAdditionalRegs
    mret

rtosa_m_soft_int:
.ifdef D_RTOSAL_VECT_TABLE
    m_pushREGFILE                                     /* Push registers of current task onto stack */
    m_SET_INT_CONTEXT                                 /* Mark we are in interrupt context */
    m_pushAdditionalRegs
    m_SAVE_CONTEXT
    m_SET_SP_TO_ISR_STACK                             /* After RegFile is pushed onto task's-stack, we change sp to point to ISR-Stack */
.endif /* D_RTOSAL_VECT_TABLE */
    m_CALL_INT_HANDLER g_fptrIntMSoftIntHandler        /* Call the interrupt handler */
    m_END_SWITCHING_ISR rtosa_m_soft_int_no_cs         /* Check if context switch is required now. If yes - handle it now */
rtosa_m_soft_int_no_cs:
    m_CLEAR_INT_CONTEXT                                /* Clear interrupt context indication */
    m_SET_SP_TO_TASK_STACK                             /* Just before pop RegFile of the interrupted Task - change sp to point to current Task-Stack */
    m_popREGFILE                                       /* Restore the registers of current task from the stack */
    m_popAdditionalRegs
    mret

rtosa_m_timer_int:
.ifdef D_RTOSAL_VECT_TABLE
    m_pushREGFILE                                     /* Push registers of current task onto stack */
    m_SET_INT_CONTEXT                                 /* Mark we are in interrupt context */
    m_pushAdditionalRegs
    m_SAVE_CONTEXT
    m_SET_SP_TO_ISR_STACK                             /* After RegFile is pushed onto task's-stack, we change sp to point to ISR-Stack */
.endif /* D_RTOSAL_VECT_TABLE */
// natiR - remove    m_SAVE_EPC                                         /* Save MEPC CSR */
    m_CALL_INT_HANDLER g_fptrIntMTimerIntHandler       /* Call the interrupt handler */
    m_RESTORE_CONTEXT                                  /* Restore mstatus, mepc and sp */
    m_SET_SP_TO_TASK_STACK                             /* Just before pop RegFile of the interrupted task - change sp to point to current Task-Stack */
    m_CLEAR_INT_CONTEXT                                /* Clear interrupt context indication */
    m_popREGFILE                                       /* Restore the registers of current task from the stack */
    m_popAdditionalRegs
    mret

rtosa_m_external_int:
.ifdef D_RTOSAL_VECT_TABLE
    m_pushREGFILE                                     /* Push registers of current task onto stack */
    m_SET_INT_CONTEXT                                 /* Mark we are in interrupt context */
    m_pushAdditionalRegs
    m_SAVE_CONTEXT
    m_SET_SP_TO_ISR_STACK                             /* After RegFile is pushed onto task's-stack, we change sp to point to ISR-Stack */
.endif /* D_RTOSAL_VECT_TABLE */
    m_CALL_INT_HANDLER g_fptrIntMExternIntHandler      /* Call the interrupt handler */
    m_RESTORE_SP                                       /* Restore sp with the stack address of the current task */
    m_END_SWITCHING_ISR rtosa_m_external_int_no_cs     /* Check if context switch is required now. If yes - handle it now */
rtosa_m_external_int_no_cs:
    m_CLEAR_INT_CONTEXT                                /* Clear the indication of interupt context */
    m_SET_SP_TO_TASK_STACK                             /* Just before pop RegFile of the interrupted task - change sp to point to current Task-Stack */
    m_popREGFILE                                       /* Restore the registers of current task from the stack */
    m_popAdditionalRegs
    mret

.weak rtosa_reserved_int
rtosa_reserved_int:
1:
    nop
    nop
    j 1b
