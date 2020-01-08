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

#include "psp_core_base.inc"
#include "psp_macros.inc"
#include "rtosal_defines.inc"
#include "rtosal_macros.inc"

#ifdef __clang__
/* disable warning for reserved registers use - we are using comrv
   reserved register and don't want to see these warnings. */
.option nowarnreservedreg
#endif /* __clang__ */

.global    pxCurrentTCB
.global    rtosalHandleEcall

.section  .text.entry
.align 4
.global   rtosal_vect_table

.ifndef D_RTOSAL_VECT_TABLE
rtosal_vect_table:
    M_PSP_PUSH_REGFILE
    M_PSP_SET_INT_CONTEXT
    M_RTOSAL_SAVE_CONTEXT pxCurrentTCB, 0
    M_PSP_CHANGE_SP_FROM_APP_TO_ISR_STACK  /* After RegFile is pushed onto application's-Stack, we change sp to point to ISR-Stack */
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
    j       rtosal_exceptions_int       /* User software interrupt & exceptions */
    .align 2
    j       rtosal_reserved_int         /* Supervisor software interrupt        */
    .align 2
    j       rtosal_reserved_int         /* Reserved for future standard use     */
    .align 2
    j       rtosal_m_soft_int           /* Machine software interrupt           */
    .align 2
    j       rtosal_reserved_int         /* User timer interrupt                 */
    .align 2
    j       rtosal_reserved_int         /* Supervisor timer interrupt           */
    .align 2
    j       rtosal_reserved_int         /* Reserved for future standard use     */
    .align 2
    j       rtosal_m_timer_int          /* Machine timer interrupt              */
    .align 2
    j       rtosal_reserved_int         /* User external interrupt              */
    .align 2
    j       rtosal_reserved_int         /* Supervisor external interrupt        */
    .align 2
    j       rtosal_reserved_int         /* Reserved for future standard use     */
    .align 2
    j       rtosal_m_external_int       /* Machine external interrupt           */
    .align 2

rtosal_exceptions_int:
.ifdef D_RTOSAL_VECT_TABLE
    M_PSP_PUSH_REGFILE                                    /* Push registers of current application onto stack */
    M_PSP_SET_INT_CONTEXT                                 /* Mark we are in interrupt context */
    M_RTOSAL_SAVE_CONTEXT pxCurrentTCB, 0                 /* Save MEPC & MSTATUS on stack. Save SP in currunt application CB */
    M_PSP_CHANGE_SP_FROM_APP_TO_ISR_STACK                 /* After RegFile is pushed onto application's-stack, we change sp to point to ISR-Stack */
.endif /* D_RTOSAL_VECT_TABLE */
    M_PSP_CALL_INT_HANDLER g_fptrIntExceptionIntHandler   /* Call the exception handler - if it is ECALL handler, then it also does RESTORE-CONTEXT*/
    M_RTOSAL_RESTORE_CONTEXT pxCurrentTCB, 0              /* Restore SP (application's one) and MEPC & MSTATUS */
    M_PSP_CLEAR_INT_CONTEXT                               /* Clear interrupt context indication */
    M_PSP_POP_REGFILE                                     /* Restore the registers of current application from the stack */
    mret

rtosal_m_soft_int:
.ifdef D_RTOSAL_VECT_TABLE
    M_PSP_PUSH_REGFILE                                       /* Push registers of current application onto stack */
    M_PSP_SET_INT_CONTEXT                                    /* Mark we are in interrupt context */
    M_RTOSAL_SAVE_CONTEXT pxCurrentTCB, 0                    /* Save MEPC & MSTATUS on stack. Save SP in currunt application CB */
    M_PSP_CHANGE_SP_FROM_APP_TO_ISR_STACK                    /* After RegFile is pushed onto application's-stack, we change sp to point to ISR-Stack */
.endif /* D_RTOSAL_VECT_TABLE */
    M_PSP_CALL_INT_HANDLER g_fptrIntMSoftIntHandler          /* Call the interrupt handler */
    M_RTOSAL_END_CONTEXT_SWITCH_FROM_ISR rtosal_m_soft_int_no_cs /* Check if context switch is required now. If yes - handle it now */
rtosal_m_soft_int_no_cs:
    M_RTOSAL_RESTORE_CONTEXT pxCurrentTCB, 0                    /* Restore SP (application's one) and MEPC & MSTATUS */
    M_PSP_CLEAR_INT_CONTEXT                                  /* Clear interrupt context indication */
    M_PSP_POP_REGFILE                                        /* Restore the registers of current application from the stack */
    mret

rtosal_m_timer_int:
.ifdef D_RTOSAL_VECT_TABLE
    M_PSP_PUSH_REGFILE                                       /* Push registers of current application onto stack */
    M_PSP_SET_INT_CONTEXT                                    /* Mark we are in interrupt context */
    M_RTOSAL_SAVE_CONTEXT pxCurrentTCB, 0                    /* Save MEPC & MSTATUS on stack. Save SP in currunt application CB */
    M_PSP_CHANGE_SP_FROM_APP_TO_ISR_STACK                    /* After RegFile is pushed onto application's-stack, we change sp to point to ISR-Stack */
.endif /* D_RTOSAL_VECT_TABLE */
    M_PSP_CALL_INT_HANDLER g_fptrIntMTimerIntHandler         /* Call the interrupt handler */
    M_RTOSAL_RESTORE_CONTEXT pxCurrentTCB, 0                 /* Restore SP (application's one) and MEPC & MSTATUS */
    M_PSP_CLEAR_INT_CONTEXT                                  /* Clear interrupt context indication */
    M_PSP_POP_REGFILE                                        /* Restore the registers of current application from the stack */
    mret

rtosal_m_external_int:
.ifdef D_RTOSAL_VECT_TABLE
    M_PSP_PUSH_REGFILE                                            /* Push registers of current application onto stack */
    M_PSP_SET_INT_CONTEXT                                         /* Mark we are in interrupt context */
    M_RTOSAL_SAVE_CONTEXT pxCurrentTCB, 0                         /* Save MEPC & MSTATUS on stack. Save SP in currunt application CB */
    M_PSP_CHANGE_SP_FROM_APP_TO_ISR_STACK                         /* After RegFile is pushed onto application's-stack, we change sp to point to ISR-Stack */
.endif /* D_RTOSAL_VECT_TABLE */
    M_PSP_CALL_INT_HANDLER g_fptrIntMExternIntHandler                /* Call the interrupt handler */
    M_RTOSAL_END_CONTEXT_SWITCH_FROM_ISR rtosal_m_external_int_no_cs  /* Check if context switch is required now. If yes - handle it now */
rtosal_m_external_int_no_cs:
    M_RTOSAL_RESTORE_CONTEXT pxCurrentTCB, 0                         /* Restore SP (application's one) and MEPC & MSTATUS */
    M_PSP_CLEAR_INT_CONTEXT                                          /* Clear interrupt context indication */
    M_PSP_POP_REGFILE                                                /* Restore the registers of current application from the stack */
    mret


.weak rtosal_reserved_int
rtosal_reserved_int:
1:
    nop
    nop
    j 1b

.align 8
/* Implements context-switch as a result of ECALL exception */
rtosalHandleEcall:
    /* Update & save MEPC contents on the application's stack: */
    /* 1. Use application's stack (rather than ISR's stack) */
    M_PSP_CHANGE_SP_FROM_ISR_TO_APP_STACK  pxCurrentTCB, 0
    /* 2. Read MEPC content as saved on stack (location 0) */
    M_PSP_LOAD  a0, D_RTOSAL_MEPC_LOC_IN_STK(sp)
    /* 3. Advance the address by 4 in order to return the next instraction right after the ECALL yielding */
    M_PSP_ADDI  a0, a0, 4
    /* 4. Write back the number to application's stack (location 0) */
    M_PSP_STORE a0, D_RTOSAL_MEPC_LOC_IN_STK(sp)
    M_PSP_CHANGE_SP_FROM_APP_TO_ISR_STACK
    M_RTOSAL_SWITCH_CONTEXT
    M_RTOSAL_RESTORE_CONTEXT pxCurrentTCB, 0    /* This macro restors sp to the one used by current application, and restores MEPC and MSTATUS CSRs*/
    M_PSP_CLEAR_INT_CONTEXT
    M_PSP_POP_REGFILE
    mret

