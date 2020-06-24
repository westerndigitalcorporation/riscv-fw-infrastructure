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

.equ D_PSP_MIE, 8

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
    M_PSP_STORE    x1,   3  * REGBYTES(sp)
    M_PSP_STORE    x5,   4  * REGBYTES(sp)
    M_PSP_STORE    x6,   5  * REGBYTES(sp)
    M_PSP_STORE    x7,   6  * REGBYTES(sp)
    M_PSP_STORE    x8,   7  * REGBYTES(sp)
    M_PSP_STORE    x9,   8  * REGBYTES(sp)
    M_PSP_STORE    x10,  9  * REGBYTES(sp)
    M_PSP_STORE    x11, 10  * REGBYTES(sp)
    M_PSP_STORE    x12, 11  * REGBYTES(sp)
    M_PSP_STORE    x13, 12  * REGBYTES(sp)
    M_PSP_STORE    x14, 13  * REGBYTES(sp)
    M_PSP_STORE    x15, 14  * REGBYTES(sp)
    M_PSP_STORE    x16, 15  * REGBYTES(sp)
    M_PSP_STORE    x17, 16  * REGBYTES(sp)
    M_PSP_STORE    x18, 17  * REGBYTES(sp)
    M_PSP_STORE    x19, 18  * REGBYTES(sp)
    M_PSP_STORE    x20, 19  * REGBYTES(sp)
    M_PSP_STORE    x21, 20  * REGBYTES(sp)
    M_PSP_STORE    x22, 21  * REGBYTES(sp)
    M_PSP_STORE    x23, 22  * REGBYTES(sp)
    M_PSP_STORE    x24, 23  * REGBYTES(sp)
    M_PSP_STORE    x25, 24  * REGBYTES(sp)
    M_PSP_STORE    x26, 25  * REGBYTES(sp)
    M_PSP_STORE    x27, 26  * REGBYTES(sp)
    M_PSP_STORE    x28, 27  * REGBYTES(sp)
    M_PSP_STORE    x30, 29  * REGBYTES(sp)
#ifndef D_COMRV
    M_PSP_STORE    x29, 28  * REGBYTES(sp)
    M_PSP_STORE    x31, 30  * REGBYTES(sp)
#else
    M_PSP_STORE    x4,  28  * REGBYTES(sp)
#endif /* D_COMRV */
.endm

/* Macro for restoring application context */
.macro M_PSP_POP_REGFILE
    /* Restore core standard registers,
    Skip global pointer because that does not change */
    M_PSP_LOAD    x1,   3 * REGBYTES(sp)
    M_PSP_LOAD    x5,   4 * REGBYTES(sp)
    M_PSP_LOAD    x6,   5 * REGBYTES(sp)
    M_PSP_LOAD    x7,   6 * REGBYTES(sp)
    M_PSP_LOAD    x8,   7 * REGBYTES(sp)
    M_PSP_LOAD    x9,   8 * REGBYTES(sp)
    M_PSP_LOAD    x10,  9 * REGBYTES(sp)
    M_PSP_LOAD    x11, 10 * REGBYTES(sp)
    M_PSP_LOAD    x12, 11 * REGBYTES(sp)
    M_PSP_LOAD    x13, 12 * REGBYTES(sp)
    M_PSP_LOAD    x14, 13 * REGBYTES(sp)
    M_PSP_LOAD    x15, 14 * REGBYTES(sp)
    M_PSP_LOAD    x16, 15 * REGBYTES(sp)
    M_PSP_LOAD    x17, 16 * REGBYTES(sp)
    M_PSP_LOAD    x18, 17 * REGBYTES(sp)
    M_PSP_LOAD    x19, 18 * REGBYTES(sp)
    M_PSP_LOAD    x20, 19 * REGBYTES(sp)
    M_PSP_LOAD    x21, 20 * REGBYTES(sp)
    M_PSP_LOAD    x22, 21 * REGBYTES(sp)
    M_PSP_LOAD    x23, 22 * REGBYTES(sp)
    M_PSP_LOAD    x24, 23 * REGBYTES(sp)
    M_PSP_LOAD    x25, 24 * REGBYTES(sp)
    M_PSP_LOAD    x26, 25 * REGBYTES(sp)
    M_PSP_LOAD    x27, 26 * REGBYTES(sp)
    M_PSP_LOAD    x28, 27 * REGBYTES(sp)
    M_PSP_LOAD    x30, 29 * REGBYTES(sp)
#ifndef D_COMRV
    M_PSP_LOAD    x29, 28 * REGBYTES(sp)
    M_PSP_LOAD    x31, 30 * REGBYTES(sp)
#else
   M_PSP_LOAD     x4,  28  * REGBYTES(sp)
#endif /* D_COMRV */
    /* Make sure SP is aligned to 16 */
    M_PSP_ADDI    sp, sp, REGBYTES * 32
.endm


/* Macro for setting SP to use stack dedicated to ISRs */
/* [NR] - To do: add stack check */
.macro M_PSP_CHANGE_SP_FROM_APP_TO_ISR_STACK
    /* Load sp register with the addres of current Task-CB */
    M_PSP_LOAD    sp, xISRStackTop
.endm

/* Macro for setting SP to use stack of current application */
/* [NR] - To do: add stack check */
.macro M_PSP_CHANGE_SP_FROM_ISR_TO_APP_STACK  pAppCB, spLocationInAppCB
    /* Load sp register with the address of current Application Control Block */
    M_PSP_LOAD    sp, \pAppCB
  /* Update sp regsiter to point to Task's stack*/
    M_PSP_LOAD    sp, \spLocationInAppCB(sp)
.endm

/* disable machine interrupts after reading the current status */
.macro M_PSP_DISABLE_MINTERRUPTS saveCurrentStatusToReg
    csrrci \saveCurrentStatusToReg, mstatus, D_PSP_MIE
.endm

/* restore machine interrupts status */
.macro M_PSP_RESTORE_MINTERRUPTS restorStatusFromReg
    csrs mstatus, \restorStatusFromReg
.endm

