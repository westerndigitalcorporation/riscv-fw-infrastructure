/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2020 Western Digital Corporation or its affiliates.
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
* @file   psp_int_vect_eh1.S
* @author Dvoskin Alexander
* @date   March 2020
* @brief  The file supplies interrupt services. The file is specific to SweRV EH1 specifications
*
*/


#include "psp_core_base.inc"
#include "psp_macros.inc"
#include "psp_int_vect_eh1.inc"

#ifdef __clang__
/* disable warning for reserved registers use - we are using comrv
   reserved register and don't want to see these warnings. */
.option nowarnreservedreg
#endif /* __clang__ */

/*
This implementation supports riscv privileged v1.10
*/

.section  .text
.align 4
.global   psp_vect_table

.ifndef D_PSP_VECT_TABLE
psp_vect_table:
    M_PSP_PUSH_REGFILE
    /* NatiR - TO DO - add indication of enter to Interrupt context (call it M_PSP_SET_INT_CONTEXT) */
    csrr    t0, mcause
    bge     t0, zero, psp_vect_table_
    slli    t0, t0, 2
    la      t1, psp_vect_table_
    add     t0, t0, t1
    jr      t0
.endif /* D_PSP_VECT_TABLE */

.align 4
.ifndef D_PSP_VECT_TABLE
psp_vect_table_:
.else
psp_vect_table:
.endif
    j psp_exceptions_int       /* User software interrupt & exceptions */
    .align 2
    j psp_reserved_int         /* Supervisor software interrupt    */
    .align 2
    j psp_reserved_int         /* Reserved for future standard use */
    .align 2
    j psp_m_soft_int           /* Machine software interrupt       */
    .align 2
    j psp_reserved_int         /* User timer interrupt             */
    .align 2
    j psp_reserved_int         /* Supervisor timer interrupt       */
    .align 2
    j psp_reserved_int         /* Reserved for future standard use */
    .align 2
    j psp_m_timer_int          /* Machine timer interrupt          */
    .align 2
    j psp_reserved_int         /* User external interrupt          */
    .align 2
    j psp_reserved_int         /* Supervisor external interrupt    */
    .align 2
    j psp_reserved_int         /* Reserved for future standard use */
    .align 2
    j psp_m_external_int       /* Machine external interrupt       */
    .align 2

psp_exceptions_int:
.ifdef D_PSP_VECT_TABLE
    M_PSP_PUSH_REGFILE                                  /* Push registers of current task onto stack */
    /* NatiR - TO DO - add indication of enter to Interrupt context (call it M_PSP_SET_INT_CONTEXT) */
    M_PSP_CHANGE_SP_FROM_APP_TO_ISR_STACK               /* After RegFile is pushed onto task's-stack, we change sp to point to ISR-Stack */
.endif /* D_PSP_VECT_TABLE */
    M_PSP_CALL_INT_HANDLER g_fptrIntExceptionIntHandler /* call the exception handler */
   // NatiR - replace with appropriate macro M_PSP_CHANGE_SP_FROM_ISR_TO_APP_STACK               /* Just before pop RegFile of the interrupted task - change sp to point to current Task-Stack */
    /* NatiR - TO DO - add indication of exit from Interrupt context (call it M_PSP_CLEAR_INT_CONTEXT) */
    M_PSP_POP_REGFILE                                   /* Restore the registers of current task from the stack */
    mret

psp_m_soft_int:
.ifdef D_PSP_VECT_TABLE
    M_PSP_PUSH_REGFILE                                  /* Push registers of current task onto stack */
    /* NatiR - TO DO - add indication of enter to Interrupt context (call it M_PSP_SET_INT_CONTEXT) */
    M_PSP_CHANGE_SP_FROM_APP_TO_ISR_STACK               /* After RegFile is pushed onto application's-Stack, we change sp to point to ISR-Stack */
.endif /* D_PSP_VECT_TABLE */
    M_PSP_CALL_INT_HANDLER g_fptrIntMSoftIntHandler     /* Call the interrupt handler */
   // Nati - replace with appropriate macro     M_PSP_CHANGE_SP_FROM_ISR_TO_APP_STACK               /* Just before pop RegFile of the interrupted Task - change sp to point to current Task-Stack */
    /* NatiR - TO DO - add indication of exit from Interrupt context (call it M_PSP_CLEAR_INT_CONTEXT) */
    M_PSP_POP_REGFILE                                    /* Restore the registers of current task from the stack */
    mret

psp_m_timer_int:
.ifdef D_PSP_VECT_TABLE
    M_PSP_PUSH_REGFILE                                   /* Push registers of current task onto stack */
    /* NatiR - TO DO - add indication of enter to Interrupt context (call it M_PSP_SET_INT_CONTEXT) */
    M_PSP_CHANGE_SP_FROM_APP_TO_ISR_STACK                  /* After RegFile is pushed onto application's-Stack, we change sp to point to ISR-Stack */
.endif /* D_PSP_VECT_TABLE */
    M_PSP_CALL_INT_HANDLER g_fptrIntMTimerIntHandler    /* Call the interrupt handler */
    // Nati - replace with appropriate macro     M_PSP_CHANGE_SP_FROM_ISR_TO_APP_STACK               /* Just before pop RegFile of the interrupted task - change sp to point to current Task-Stack */
    /* NatiR - TO DO - add indication of exit from Interrupt context (call it M_PSP_CLEAR_INT_CONTEXT) */
    M_PSP_POP_REGFILE                                    /* Restore the registers of current task from the stack */
    mret

psp_m_external_int:
.ifdef D_PSP_VECT_TABLE
    M_PSP_PUSH_REGFILE                                   /* Push registers of current task onto stack */
    /* NatiR - TO DO - add indication of enter to Interrupt context (call it M_PSP_SET_INT_CONTEXT) */
    M_PSP_CHANGE_SP_FROM_APP_TO_ISR_STACK                  /* After RegFile is pushed onto task's-stack, we change sp to point to ISR-Stack */
.endif /* D_PSP_VECT_TABLE */
    M_PSP_CALL_EXT_INT_HANDLER
    // Nati - replace with appropriate macro     M_PSP_CHANGE_SP_FROM_ISR_TO_APP_STACK               /* Just before pop RegFile of the interrupted task - change sp to point to current Task-Stack */
    /* NatiR - TO DO - add indication of exit from Interrupt context (call it M_PSP_CLEAR_INT_CONTEXT) */
    M_PSP_POP_REGFILE                                    /* Restore the registers of current task from the stack */
    mret


.weak psp_reserved_int
psp_reserved_int:
1:
    nop
    nop
    j 1b
