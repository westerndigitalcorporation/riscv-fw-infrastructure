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

.extern xISRStackTop

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
.equ REGBYTES, 8
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
.equ REGBYTES, 4
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
    /* make room in stack for the starndard core registers.
     * In addition, make sure SP is aligned to 16 */
    m_ADDI    sp, sp, -REGBYTES * 32
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
    /* Restore core standard registers,
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
    /* Make sure SP is aligned to 16 */
    m_ADDI    sp, sp, REGBYTES * 32
.endm
