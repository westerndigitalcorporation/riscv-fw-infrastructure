/*
 SPDX-License-Identifier: Apache-2.0
 Copyright 2019 Western Digital Corporation or its affiliates.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http:www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/
#ifndef __PSP_ASM_MACROS_H__
#define __PSP_ASM_MACROS_H__

#include "psp_specific_rtos.h"


.extern xISRStackTop

/*.if __riscv_xlen == 64 */
#if __riscv_xlen == 64
.macro m_STORE operand1,operand2
    sd \operand1, \operand2
.endm
.macro m_LOAD operand1,operand2
    ld \operand1, \operand2
.endm
.macro m_ADDI operand1,operand2,operand3
    addiw \operand1, \operand2, \operand3
.endm
/*.equ REGBYTES, 8   --  NatiR - resolve why it gives here “expected symbol name” compilation error */
/*.else */
#elif __riscv_xlen == 32
.macro m_STORE operand1,operand2
    sw \operand1, \operand2
.endm
.macro m_LOAD operand1,operand2
    lw \operand1, \operand2
.endm
.macro m_ADDI operand1,operand2,operand3
    addi \operand1, \operand2, \operand3
.endm
/*.equ REGBYTES, 4   --  NatiR - resolve why it gives here “expected symbol name” compilation error */
/*.endif */
#endif

#ifndef REGBYTES
#define REGBYTES 4
#endif






.macro m_SET_INT_CONTEXT
    /* save address of g_uiIsInterruptContext -> t0 */
    la        a0, g_uiIsInterruptContext
    /* load the value g_uiIsInterruptContext -> t1 */
    m_LOAD    a1, 0x0(a0)
    /* increment t1 by 1 */
    m_ADDI    a1, a1, 1
    /* store new value to g_uiIsInterruptContext */
    m_STORE   a1, 0x0(a0)
.endm

.macro m_CLEAR_INT_CONTEXT
    /* save address of g_uiIsInterruptContext -> t0 */
    la        a0, g_uiIsInterruptContext
    /* load the value g_uiIsInterruptContext -> t1 */
    m_LOAD    a1, 0x0(a0)
    /* decrement t1 by 1 */
    m_ADDI    a1, a1, -1
    /* store new value to g_uiIsInterruptContext */
    m_STORE   a1, 0x0(a0)
.endm

.macro m_CALL_INT_HANDLER fptIntHandler
    /* load the address of provided fptIntHandler */
    la        a0, \fptIntHandler
    /* load the actual handler address */
    m_LOAD    a0, 0x0(a0)
    /* invoke the interrupt handler */
    jalr      a0
.endm

/* Macro for saving task context */
.macro m_pushREGFILE
    /* make room in stack */
    m_ADDI    sp, sp, -REGBYTES * 30
    /* Save Context */
    m_STORE    x1,   1  * REGBYTES(sp)
    m_STORE    x5,   2  * REGBYTES(sp)
    m_STORE    x6,   3  * REGBYTES(sp)
    m_STORE    x7,   4  * REGBYTES(sp)
    m_STORE    x8,   5  * REGBYTES(sp)
    m_STORE    x9,   6  * REGBYTES(sp)
    m_STORE    x10,  7  * REGBYTES(sp)
    m_STORE    x11,  8  * REGBYTES(sp)
    m_STORE    x12,  9  * REGBYTES(sp)
    m_STORE    x13, 10  * REGBYTES(sp)
    m_STORE    x14, 11  * REGBYTES(sp)
    m_STORE    x15, 12  * REGBYTES(sp)
    m_STORE    x16, 13  * REGBYTES(sp)
    m_STORE    x17, 14  * REGBYTES(sp)
    m_STORE    x18, 15  * REGBYTES(sp)
    m_STORE    x19, 16  * REGBYTES(sp)
    m_STORE    x20, 17  * REGBYTES(sp)
    m_STORE    x21, 18  * REGBYTES(sp)
    m_STORE    x22, 19  * REGBYTES(sp)
    m_STORE    x23, 20  * REGBYTES(sp)
    m_STORE    x24, 21  * REGBYTES(sp)
    m_STORE    x25, 22  * REGBYTES(sp)
    m_STORE    x26, 23  * REGBYTES(sp)
    m_STORE    x27, 24  * REGBYTES(sp)
    m_STORE    x28, 25  * REGBYTES(sp)
    m_STORE    x29, 26  * REGBYTES(sp)
    m_STORE    x30, 27  * REGBYTES(sp)
    m_STORE    x31, 28  * REGBYTES(sp)
.endm

/* Macro for restoring task context */
.macro m_popREGFILE
    /* Restore registers,
    Skip global pointer because that does not change */
    m_LOAD    x1,   1 * REGBYTES(sp)
    m_LOAD    x5,   2 * REGBYTES(sp)
    m_LOAD    x6,   3 * REGBYTES(sp)
    m_LOAD    x7,   4 * REGBYTES(sp)
    m_LOAD    x8,   5 * REGBYTES(sp)
    m_LOAD    x9,   6 * REGBYTES(sp)
    m_LOAD    x10,  7 * REGBYTES(sp)
    m_LOAD    x11,  8 * REGBYTES(sp)
    m_LOAD    x12,  9 * REGBYTES(sp)
    m_LOAD    x13, 10 * REGBYTES(sp)
    m_LOAD    x14, 11 * REGBYTES(sp)
    m_LOAD    x15, 12 * REGBYTES(sp)
    m_LOAD    x16, 13 * REGBYTES(sp)
    m_LOAD    x17, 14 * REGBYTES(sp)
    m_LOAD    x18, 15 * REGBYTES(sp)
    m_LOAD    x19, 16 * REGBYTES(sp)
    m_LOAD    x20, 17 * REGBYTES(sp)
    m_LOAD    x21, 18 * REGBYTES(sp)
    m_LOAD    x22, 19 * REGBYTES(sp)
    m_LOAD    x23, 20 * REGBYTES(sp)
    m_LOAD    x24, 21 * REGBYTES(sp)
    m_LOAD    x25, 22 * REGBYTES(sp)
    m_LOAD    x26, 23 * REGBYTES(sp)
    m_LOAD    x27, 24 * REGBYTES(sp)
    m_LOAD    x28, 25 * REGBYTES(sp)
    m_LOAD    x29, 26 * REGBYTES(sp)
    m_LOAD    x30, 27 * REGBYTES(sp)
    m_LOAD    x31, 28 * REGBYTES(sp)
    m_ADDI    sp, sp, REGBYTES * 30
.endm

/* Macro for setting SP to use stack dedicated to ISRs */
.macro m_SET_SP_TO_ISR_STACK
    /* Load sp register with the addres of current Task-CB */
    m_LOAD    sp, xISRStackTop
.endm

/* Macro for setting SP to use stack of current Task */
.macro m_SET_SP_TO_TASK_STACK
    .global    pxCurrentTCB
    /* Load sp register with the addres of current Task-CB */
    m_LOAD    sp, pxCurrentTCB
	/* Update sp regsiter to point to Task's stack*/
    m_LOAD    sp, 0x0(sp)
.endm

/*
///////////////////////////////////////////////////////////////////////
// I moved the following macros from rtosal_int_vect.S
///////////////////////////////////////////////////////////////////////
*/


/* saves mstatus and tcb */
.macro m_SAVE_CONTEXT
    .global    pxCurrentTCB
    /* Store mstatus */
    csrr      t0, mstatus
    m_STORE   t0, 29 * REGBYTES(sp)
    /* Store current stackpointer in task control block (TCB) */
    m_LOAD    t0, pxCurrentTCB
    m_STORE   sp, 0x0(t0)
    /* NatiR - doc it*/
	csrr      t0, mepc
    m_STORE   t0, 0(sp) // NatiR - use macro for location

.endm

 /* restore mstatus and tcb */
.macro m_RESTORE_CONTEXT
    .global    pxCurrentTCB
    /* Load stack pointer from the current TCB */
    m_LOAD    sp, pxCurrentTCB
    m_LOAD    sp, 0x0(sp)
    /* Load task program counter */
    m_LOAD    t0, 0 * REGBYTES(sp)
    csrw      mepc, t0
    /* Load saved mstatus */
    m_LOAD    t0, 29 * REGBYTES(sp)
    csrw      mstatus, t0
.endm

.macro m_END_SWITCHING_ISR branch_label
    /* save address of g_rtosalContextSwitch -> a0 */
    la        a0, g_rtosalContextSwitch
    /* load the value g_rtosalContextSwitch -> a1 */
    m_LOAD    a1, 0x0(a0)
    /* check if g_rtosalContextSwitch is set - need to do context switch */
    beqz      a1, \branch_label
    /* clear g_rtosalContextSwitch */
    /* TODO: if bitmanip exist add bit set */
    m_STORE   zero, 0x0(a0)
    /* perform context switch */
#ifdef D_USE_FREERTOS
    m_SAVE_CONTEXT
    jal     vTaskSwitchContext
    m_RESTORE_CONTEXT
#else
    -- Add appropriate RTOS definitions
#endif /* .if D_USE_FREERTOS */
.endm

/* Saves current Machine Exception Program Counter (MEPC) as task program counter */
.macro m_SAVE_EPC_
    csrr      t0, mepc
    m_STORE   t0, 0 * REGBYTES(sp)
.endm

/* Saves current return adress (RA) as task program counter */
.macro m_SAVE_RA // NatiR - remove it ?
    LOAD      t0, 1 * REGBYTES(sp)
    m_STORE   t0, 33 * REGBYTES(sp)
.endm

/* not called on a context switch*/
.macro m_RESTORE_SP
    m_LOAD    x2, 2 * REGBYTES(sp)
.endm

#endif /* __PSP_ASM_MACROS_H__ */
