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

/**
* include files
*/

/**
* macros
*/

.if D_USE_FREERTOS

.global    pxCurrentTCB

/* Macro for setting SP to use stack dedicated to ISRs */
.macro M_SET_SP_FROM_APP_TO_ISR_STACK
    /* Load sp register with the addres of current Task-CB */
    M_LOAD    sp, xISRStackTop
.endm

/* Macro for setting SP to use stack of current Task */
.macro M_REVERT_SP_FROM_ISR_TO_APP_STACK  pTaskCB, spLocInTcb
    /* Load sp register with the address of current Task-CB */
    M_LOAD    sp, \pTaskCB
	/* Update sp regsiter to point to Task's stack*/
    M_LOAD    sp, \spLocInTcb(sp)
.endm

/* saves mstatus and tcb */
.macro M_SAVE_CONTEXT  pTaskCB, spLocInTcb
    /* Store mstatus */
    csrr      t0, mstatus
    M_STORE   t0, D_MSTATUS_LOC_IN_STK * REGBYTES(sp)
    /* Store current stackpointer in task control block (TCB) */
    M_LOAD    t0, \pTaskCB
    M_STORE   sp, \spLocInTcb(t0)
    /* Store mepc */
	csrr      t0, mepc
    M_STORE   t0, D_MEPC_LOC_IN_STK(sp)

.endm

 /* restore mstatus and tcb */
.macro M_RESTORE_CONTEXT  pTaskCB, spLocInTcb
    /* Load stack pointer from the current TCB */
    M_LOAD    sp, \pTaskCB
    M_LOAD    sp, \spLocInTcb(sp)
    /* Load task program counter */
    M_LOAD    t0, D_MEPC_LOC_IN_STK * REGBYTES(sp)
    csrw      mepc, t0
    /* Load saved mstatus */
    M_LOAD    t0, D_MSTATUS_LOC_IN_STK * REGBYTES(sp)
    csrw      mstatus, t0
.endm

.macro M_END_SWITCHING_ISR branch_label
    /* save address of g_rtosalContextSwitch -> a0 */
    la        a0, g_rtosalContextSwitch
    /* load the value g_rtosalContextSwitch -> a1 */
    M_LOAD    a1, 0x0(a0)
    /* check if g_rtosalContextSwitch is set - need to do context switch */
    beqz      a1, \branch_label
    /* clear g_rtosalContextSwitch */
    /* TODO: if bitmanip exist add bit set */
    M_STORE   zero, 0x0(a0)
    /* perform context switch */
.if D_USE_FREERTOS
    jal     vTaskSwitchContext
.else
    -- Add appropriate RTOS definitions
.endif /* .if D_USE_FREERTOS */
.endm

/* Saves current Machine Exception Program Counter (MEPC) as task program counter */
.macro M_SAVE_EPC_
    csrr      t0, mepc
    M_STORE   t0, 0 * REGBYTES(sp)
.endm

/* Saves current return adress (RA) as task program counter */
.macro M_SAVE_RA
    LOAD      t0, 1 * REGBYTES(sp)
    M_STORE   t0, 33 * REGBYTES(sp)
.endm

/* not called on a context switch*/
.macro M_RESTORE_SP
    M_LOAD    x2, 2 * REGBYTES(sp)
.endm

.else
   .error "RTOS is not defined!"
.endif // D_USE_FREERTOS

