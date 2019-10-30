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
* @file   rtosal_asm_macros.h
* @author Nati Rapaport
* @date   29.10.2019
* @brief  The file defines the RTOS AL assembly macros
*/
#ifndef __RTOSAL_ASM_MACROS_H__
#define __RTOSAL_ASM_MACROS_H__

/**
* include files
*/

/**
* macros
*/

#ifdef D_USE_FREERTOS

.global    pxCurrentTCB

/* Macro for setting SP to use stack dedicated to ISRs */
.macro m_SET_SP_FROM_APP_TO_ISR_STACK
    /* Load sp register with the addres of current Task-CB */
    m_LOAD    sp, xISRStackTop
.endm

/* Macro for setting SP to use stack of current Task */
.macro m_REVERT_SP_FROM_ISR_TO_APP_STACK
    .global    pxCurrentTCB
    /* Load sp register with the addres of current Task-CB */
    m_LOAD    sp, pxCurrentTCB
	/* Update sp regsiter to point to Task's stack*/
    m_LOAD    sp, 0x0(sp)
.endm

/* saves mstatus and tcb */
.macro m_SAVE_CONTEXT
    .global    pxCurrentTCB
    /* Store mstatus */
    csrr      t0, mstatus
    m_STORE   t0, D_MSTATUS_LOC_IN_STK * REGBYTES(sp)
    /* Store current stackpointer in task control block (TCB) */
    m_LOAD    t0, pxCurrentTCB
    m_STORE   sp, 0x0(t0)
    /* Store mepc */
	csrr      t0, mepc
    m_STORE   t0, D_MEPC_LOC_IN_STK(sp)

.endm

 /* restore mstatus and tcb */
.macro m_RESTORE_CONTEXT
    .global    pxCurrentTCB
    /* Load stack pointer from the current TCB */
    m_LOAD    sp, pxCurrentTCB
    m_LOAD    sp, 0x0(sp)
    /* Load task program counter */
    m_LOAD    t0, D_MEPC_LOC_IN_STK * REGBYTES(sp)
    csrw      mepc, t0
    /* Load saved mstatus */
    m_LOAD    t0, D_MSTATUS_LOC_IN_STK * REGBYTES(sp)
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
    jal     vTaskSwitchContext
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
.macro m_SAVE_RA
    LOAD      t0, 1 * REGBYTES(sp)
    m_STORE   t0, 33 * REGBYTES(sp)
.endm

/* not called on a context switch*/
.macro m_RESTORE_SP
    m_LOAD    x2, 2 * REGBYTES(sp)
.endm

#else
   #error "Define an RTOS"
#endif // D_USE_FREERTOS




#endif /* __RTOSAL_ASM_MACROS_H__ */
