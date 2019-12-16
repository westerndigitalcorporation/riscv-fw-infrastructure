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
* @file   comrv_entry.S
* @author Ronen Haen
* @date   21.06.2019
* @brief  The file implements the COM-RV entry function
*
*/
#include "psp_core_base.inc"
.include "comrv_macros.inc"

/* total size of used stack */
.equ D_COMRV_STACK_FRAME_SIZE,              REGBYTES * 4

/* shift amount to get the function offset in the lsb */
.equ D_COMRV_FUNC_OFFSET_SMNT,              17

/* mask for function ofset in the token */
.equ D_COMRV_FUNC_OFFSET_MASK,              0x3FF

.equ D_COMRV_TOKEN_INDICATION,              1
.equ D_COMRV_INVOKE_CALLEE_BIT_0,           1
.equ D_COMRV_RET_CALLER_BIT_0,              0
.equ D_COMRV_THUNK_SHMT,                    4

/*
 local stack frame layout:
 offset 0 - RA
 offset 1 - A0
 offset 2 - function arg
 offset 3 - not used
*/
.equ D_COMRV_RA_REG_STACK_OFFSET_TMP,       REGBYTES * 0
.equ D_COMRV_A0_REG_STACK_OFFSET_TMP,       REGBYTES * 1
.equ D_COMRV_ARG_STACK_OFFSET_TMP,          REGBYTES * 2

/*
 COM-RV stack frame layout - see comrvStackFrame_t:
*/
.equ D_COMRV_CALLER_RA_STACK_OFFSET,        REGBYTES * 0
.equ D_COMRV_CALLEE_TOKEN_STACK_OFFSET,     REGBYTES * 1
.equ D_COMRV_PREV_STACK_FRAME,              REGBYTES * 2
.equ D_COMRV_GROUP_SIZE_STACK_OFFSET,       D_COMRV_PREV_STACK_FRAME + 2


.global comrvEntry

.section  .text
.align 4
/* This is the function responsible for invoking the overlay function
   Whenever there is a call to an overlay function, the compiler will plant instead
   a call to this entry point (its address is fixed in t6) and set the appropriate
   address token in register t5;
   this code uses fixed registers t3, t4 and t5 -
   t3 - points to current stack top (per task if OS involved)
   t4 - points to the free pool of stack frames
   t5 - holds the token we need to invoke (can be an actual address if callee
        is a non-overlay function) */

comrvEntry:
    /* allocate a new stack frame from the stack frame pool - we unlink the next
       free frame from the stack frame pool (t4) and assign its address to t2.
       t4 is updated with the next free stack frame */
    /* disable interrupts */
    m_COMRV_DISABLE_INTS
    /* workaround for HiFive1 */
    fence.i
    /* save the address of the next free stack frame in t2 - new stack frame
       for this engine call */
    mv          t2, t4
    /* load to t4 the offset to the previous free stack frame */
    lh          t4, D_COMRV_PREV_STACK_FRAME(t2)
    /* set in t4 the address of the next free stack frame - will be used in
        the next engine call */
    add         t4, t2, t4
    /* enable interrupts */
    m_COMRV_ENABLE_INTS
    /* at this point t4 points to the next free frame, t3 points to the top
       of the com-rv stack and t2 holds the allocated stack frame. */
    /* link the allocated frame to its stack - we link the allocated frame
       to the stack pointed by t3. */
    /* calc the offset from the new stack frame (t2) to previous stack frame (t3) */
    sub         t3, t3, t2
    /* in current stack frame (t2) save the offset to previous stack frame */
    sh          t3, D_COMRV_PREV_STACK_FRAME(t2)
    /* link allocated stack frame */
    mv          t3, t2
    /* save callee token */
    M_PSP_STORE t5, D_COMRV_CALLEE_TOKEN_STACK_OFFSET(t3)
    /* save the return address */
    M_PSP_STORE ra, D_COMRV_CALLER_RA_STACK_OFFSET(t3)
    /* check if callee token is an overlay token (in t5 bit0 is set) */
    andi        t2, t5, D_COMRV_TOKEN_INDICATION
    /* if t2 is 0 it means callee is a non overlay function so we can
       skip search and load */
    beqz        t2, comrv_invoke_callee
    /* change sp for temporary storage */
    M_PSP_ADDI  sp, sp, -D_COMRV_STACK_FRAME_SIZE
    /* temporary save a0 in the next free stack frame */
    M_PSP_STORE a0, D_COMRV_A0_REG_STACK_OFFSET_TMP(sp)
    /* Add the indication we are calling a new overlay function
       this indication is required to calculate the correct offset
       when returning to an overlay function */
    ori         t3, t3, D_COMRV_INVOKE_CALLEE_BIT_0
    /* search if the token (t5) is already loaded */
    jal         comrvGetAddressFromToken
    /* save the loaded function address in t5 (instead of the token) */
    mv          t5, a0
    /* restore a0 - may contain an actual callee function argument */
    M_PSP_LOAD  a0, D_COMRV_A0_REG_STACK_OFFSET_TMP(sp)
    /* adjust back sp - memory not needed anymore */
    M_PSP_ADDI  sp, sp, D_COMRV_STACK_FRAME_SIZE

comrv_invoke_callee:
    /* call the function */
    jalr        t5

comrv_igonr_caller_thunk_stack_frame:
    /* load the return address */
    M_PSP_LOAD  ra, D_COMRV_CALLER_RA_STACK_OFFSET(t3)
    /* we need to load the caller token to detrmine if the RA is to an overlay
       or an actual return address; first we need to unlink the top of the
       com-rv stack frame, return it back to the stack frame pool and get
       read the caller token (in previous frame it will be the callee token) */
    /* load the offset to previous stack frame */
    lh          t5, D_COMRV_PREV_STACK_FRAME(t3)
    /* unlink the top of the stack - t3 hold the unlinked stack frame address
       t5 will hold the address of the top of the stack */
    add         t5, t5, t3
    /* disable interrupts */
    m_COMRV_DISABLE_INTS
    /* link the freed stack frame back to the stack frames pool;
       t2 will hold the offset to the previous free stack frame */
    sub         t4, t4, t3
    /* save the offset to the previous free stack frame */
    sh          t4, D_COMRV_PREV_STACK_FRAME(t3)
    /* link the free stack frame back to the pool */
    mv          t4, t3
    /* enable interrupts */
    m_COMRV_ENABLE_INTS
    /* save the top of the stack to the stack register */
    mv          t3, t5
    /* read caller token (will be in the callee field now as we look at
       the previous stack frame) */
    M_PSP_LOAD  t5, D_COMRV_CALLEE_TOKEN_STACK_OFFSET(t3)
    /* t2 will hold the token type - address/token */
    andi        t2, t5, D_COMRV_TOKEN_INDICATION
    /* t2 is 0 means return to a non-overlay function so we can skip
       token search and load */
    beqz        t2, comrv_exit_ret_to_caller
    /* place thunk bit in the msb */
    slli        t2, t5, D_COMRV_THUNK_SHMT
    /* if thunk bit set (t2 negative) we skip current comrv stack frame */
    bltz        t2, comrv_igonr_caller_thunk_stack_frame
    /* change sp for temporary storage */
    M_PSP_ADDI  sp, sp, -D_COMRV_STACK_FRAME_SIZE
    /* temporary save a0 as it is being used */
    M_PSP_STORE a0, D_COMRV_A0_REG_STACK_OFFSET_TMP(sp)
    /* save ra as in the token as a placeholder - this field
       is no longer in use */
    M_PSP_STORE ra, D_COMRV_CALLEE_TOKEN_STACK_OFFSET(t3)
    /* search if the token (t5) is already loaded */
    jal         comrvGetAddressFromToken
    /* an overlay function is now loaded and a0 holds the actual
       address we can keep it in ra */
    mv          ra, a0
    /* restore a0 */
    M_PSP_LOAD  a0, D_COMRV_A0_REG_STACK_OFFSET_TMP(sp)
    /* adjust back sp - memory not needed anymore */
    M_PSP_ADDI  sp,  sp, D_COMRV_STACK_FRAME_SIZE

comrv_exit_ret_to_caller:
    /* return back to the actual caller - ra hold the actual address */
    ret
