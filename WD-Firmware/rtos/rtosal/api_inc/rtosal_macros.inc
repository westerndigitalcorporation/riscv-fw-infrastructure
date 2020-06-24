/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2019 Western Digital Corporation or its affiliates.
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
* @file   rtosal_macros.inc
* @author Nati Rapaport
* @date   29.10.2019
* @brief  The file defines the RTOS AL assembly macros
*/

/**
* include files
*/

/**
* macros
*/

.if D_USE_FREERTOS
   .global pxCurrentTCB
.else
   .error "RTOS is not defined!"
.endif // D_USE_FREERTOS


/* Macro that sets indication of interrupt context */
.macro M_RTOSAL_SET_INT_CONTEXT
    /* save address of g_rtosalIsInterruptContext -> t0 */
    la            a0, g_rtosalIsInterruptContext
    /* load the value g_rtosalIsInterruptContext -> t1 */
    M_PSP_LOAD    a1, 0x0(a0)
    /* increment t1 by 1 */
    M_PSP_ADDI    a1, a1, 1
    /* store new value to g_rtosalIsInterruptContext */
    M_PSP_STORE   a1, 0x0(a0)
.endm

/* Macro that clears indication of interrupt context */
.macro M_RTOSAL_CLEAR_INT_CONTEXT
    /* save address of g_rtosalIsInterruptContext -> t0 */
    la            a0, g_rtosalIsInterruptContext
    /* load the value g_rtosalIsInterruptContext -> t1 */
    M_PSP_LOAD    a1, 0x0(a0)
    /* decrement t1 by 1 */
    M_PSP_ADDI    a1, a1, -1
    /* store new value to g_rtosalIsInterruptContext */
    M_PSP_STORE   a1, 0x0(a0)
.endm

/* this macro save mstatus and mepc CSRs on stack then store sp in the Application control block */
.macro M_RTOSAL_SAVE_CONTEXT  pAppCB, spLocationInAppCB
    /* Store current stackpointer in task control block (TCB) */
    M_PSP_LOAD    t0, \pAppCB
    M_PSP_STORE   sp, \spLocationInAppCB(t0)
    /* Store mepc */
    csrr          t0, mepc
    M_PSP_STORE   t0, D_RTOSAL_MEPC_LOC_IN_STK(sp)
.endm

 /* this macro restore sp from the Application control block then restore mstatus and mepc CSRs from stack */
.macro M_RTOSAL_RESTORE_CONTEXT  pAppCB, spLocationInAppCB
    /* Load stack pointer from the current TCB */
    M_PSP_LOAD    sp, \pAppCB
    M_PSP_LOAD    sp, \spLocationInAppCB(sp)
    /* Load task program counter */
    M_PSP_LOAD    t0, D_RTOSAL_MEPC_LOC_IN_STK * REGBYTES(sp)
    csrw          mepc, t0
.endm

/* This macro is activated at the end of ISR handling. It does the following:
 * (1) checks whether a context-switch is required.
 * (2) If no - jump to 'branch_label'
 * (2) if yes - then
 *    (a) it clears the context-switch indication and
 *    (b) it calls M_RTOSAL_SWITCH_CONTEXT (macro that calls OS function to do context-switch)
 */
.macro M_RTOSAL_END_CONTEXT_SWITCH_FROM_ISR branch_label
    /* save address of g_rtosalContextSwitch -> a0 */
    la            a0, g_rtosalContextSwitch
    /* load the value g_rtosalContextSwitch -> a1 */
    M_PSP_LOAD    a1, 0x0(a0)
    /* check if g_rtosalContextSwitch is set - need to do context switch */
    beqz          a1, \branch_label
    /* clear g_rtosalContextSwitch */
    /* TODO: if bitmanip exist add bit set */
    M_PSP_STORE   zero, 0x0(a0)
    /* call OS to perform context switch */
    M_RTOSAL_SWITCH_CONTEXT
.endm

.macro M_RTOSAL_SWITCH_CONTEXT
.if D_USE_FREERTOS
   jal            vTaskSwitchContext
.endif
.endm

