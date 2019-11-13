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

.global xISRStackTop

/* Macro that sets indication of interrupt context */
.macro M_PSP_SET_INT_CONTEXT
    /* save address of g_uiIsInterruptContext -> t0 */
    la        a0, g_uiIsInterruptContext
    /* load the value g_uiIsInterruptContext -> t1 */
    M_PSP_LOAD    a1, 0x0(a0)
    /* increment t1 by 1 */
    M_PSP_ADDI    a1, a1, 1
    /* store new value to g_uiIsInterruptContext */
    M_PSP_STORE   a1, 0x0(a0)
.endm

/* Macro that clears indication of interrupt context */
.macro M_PSP_CLEAR_INT_CONTEXT
    /* save address of g_uiIsInterruptContext -> t0 */
    la        a0, g_uiIsInterruptContext
    /* load the value g_uiIsInterruptContext -> t1 */
    M_PSP_LOAD    a1, 0x0(a0)
    /* decrement t1 by 1 */
    M_PSP_ADDI    a1, a1, -1
    /* store new value to g_uiIsInterruptContext */
    M_PSP_STORE   a1, 0x0(a0)
.endm

.macro M_PSP_CALL_INT_HANDLER fptIntHandler
    /* load the address of provided fptIntHandler */
    la        a0, \fptIntHandler
    /* load the actual handler address */
    M_PSP_LOAD    a0, 0x0(a0)
    /* invoke the interrupt handler */
    jalr      a0
.endm

/* Macro for saving application context */
.macro M_PSP_PUSH_REGFILE
    /* make room in stack for the starndard core registers.
     * In addition, make sure SP is aligned to 16 */
    M_PSP_ADDI    sp, sp, -REGBYTES * 32
    /* Save Context */
    M_PSP_STORE    x1,   1  * REGBYTES(sp)
    M_PSP_STORE    x5,   2  * REGBYTES(sp)
    M_PSP_STORE    x6,   3  * REGBYTES(sp)
    M_PSP_STORE    x7,   4  * REGBYTES(sp)
    M_PSP_STORE    x8,   5  * REGBYTES(sp)
    M_PSP_STORE    x9,   6  * REGBYTES(sp)
    M_PSP_STORE    x10,  7  * REGBYTES(sp)
    M_PSP_STORE    x11,  8  * REGBYTES(sp)
    M_PSP_STORE    x12,  9  * REGBYTES(sp)
    M_PSP_STORE    x13, 10  * REGBYTES(sp)
    M_PSP_STORE    x14, 11  * REGBYTES(sp)
    M_PSP_STORE    x15, 12  * REGBYTES(sp)
    M_PSP_STORE    x16, 13  * REGBYTES(sp)
    M_PSP_STORE    x17, 14  * REGBYTES(sp)
    M_PSP_STORE    x18, 15  * REGBYTES(sp)
    M_PSP_STORE    x19, 16  * REGBYTES(sp)
    M_PSP_STORE    x20, 17  * REGBYTES(sp)
    M_PSP_STORE    x21, 18  * REGBYTES(sp)
    M_PSP_STORE    x22, 19  * REGBYTES(sp)
    M_PSP_STORE    x23, 20  * REGBYTES(sp)
    M_PSP_STORE    x24, 21  * REGBYTES(sp)
    M_PSP_STORE    x25, 22  * REGBYTES(sp)
    M_PSP_STORE    x26, 23  * REGBYTES(sp)
    M_PSP_STORE    x27, 24  * REGBYTES(sp)
    M_PSP_STORE    x28, 25  * REGBYTES(sp)
    M_PSP_STORE    x29, 26  * REGBYTES(sp)
    M_PSP_STORE    x30, 27  * REGBYTES(sp)
    M_PSP_STORE    x31, 28  * REGBYTES(sp)
.endm

/* Macro for restoring application context */
.macro M_PSP_POP_REGFILE
    /* Restore core standard registers,
    Skip global pointer because that does not change */
    M_PSP_LOAD    x1,   1 * REGBYTES(sp)
    M_PSP_LOAD    x5,   2 * REGBYTES(sp)
    M_PSP_LOAD    x6,   3 * REGBYTES(sp)
    M_PSP_LOAD    x7,   4 * REGBYTES(sp)
    M_PSP_LOAD    x8,   5 * REGBYTES(sp)
    M_PSP_LOAD    x9,   6 * REGBYTES(sp)
    M_PSP_LOAD    x10,  7 * REGBYTES(sp)
    M_PSP_LOAD    x11,  8 * REGBYTES(sp)
    M_PSP_LOAD    x12,  9 * REGBYTES(sp)
    M_PSP_LOAD    x13, 10 * REGBYTES(sp)
    M_PSP_LOAD    x14, 11 * REGBYTES(sp)
    M_PSP_LOAD    x15, 12 * REGBYTES(sp)
    M_PSP_LOAD    x16, 13 * REGBYTES(sp)
    M_PSP_LOAD    x17, 14 * REGBYTES(sp)
    M_PSP_LOAD    x18, 15 * REGBYTES(sp)
    M_PSP_LOAD    x19, 16 * REGBYTES(sp)
    M_PSP_LOAD    x20, 17 * REGBYTES(sp)
    M_PSP_LOAD    x21, 18 * REGBYTES(sp)
    M_PSP_LOAD    x22, 19 * REGBYTES(sp)
    M_PSP_LOAD    x23, 20 * REGBYTES(sp)
    M_PSP_LOAD    x24, 21 * REGBYTES(sp)
    M_PSP_LOAD    x25, 22 * REGBYTES(sp)
    M_PSP_LOAD    x26, 23 * REGBYTES(sp)
    M_PSP_LOAD    x27, 24 * REGBYTES(sp)
    M_PSP_LOAD    x28, 25 * REGBYTES(sp)
    M_PSP_LOAD    x29, 26 * REGBYTES(sp)
    M_PSP_LOAD    x30, 27 * REGBYTES(sp)
    M_PSP_LOAD    x31, 28 * REGBYTES(sp)
    /* Make sure SP is aligned to 16 */
    M_PSP_ADDI    sp, sp, REGBYTES * 32
.endm


/* Macro for setting SP to use stack dedicated to ISRs */
.macro M_PSP_CHANGE_SP_FROM_APP_TO_ISR_STACK
    /* Load sp register with the addres of current Task-CB */
    M_PSP_LOAD    sp, xISRStackTop
.endm

/* Macro for setting SP to use stack of current application */
.macro M_PSP_CHANGE_SP_FROM_ISR_TO_APP_STACK  pAppCB, spLocationInAppCB
    /* Load sp register with the address of current Application Control Block */
    M_PSP_LOAD    sp, \pAppCB
	/* Update sp regsiter to point to Task's stack*/
    M_PSP_LOAD    sp, \spLocationInAppCB(sp)
.endm
