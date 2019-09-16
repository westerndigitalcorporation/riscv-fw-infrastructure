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
#include "psp_asm_macros.h"

/* total size of used stack */
.equ D_COMRV_STACK_SIZE,                    REGBYTES * 4

/* shift amount to get the function offset in the lsb */
.equ D_COMRV_FUNC_OFFSET_SMNT,              17

/* mask for function ofset in the token */
.equ D_COMRV_FUNC_OFFSET_MASK,              0x3FF

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


.global   comrv_entry

.section  .text.entry
.align 4
/* This is the function responsible for invoking the overlay function
   Whenever there is a call to an overlay function, the compiler will plant instead
   a call to this entry point (its address is fixed in t6) and set the appropriate
   address token in register t5 */
comrv_entry:
    /* allocate a new stack frame from the stack frame pool - we unlink the next
       free frame from the stack frame pool (t4) and assign its address to t2.
       t4 is updated with the next free stack frame */
    /* disable interrupts */
    /* TODO */
    /* save the address of the next free stack frame in t2 - new stack frame
       for this engine call */
    mv      t2, t4
    /* load to t4 the offset to the previous free stack frame */
    lh      t4, D_COMRV_PREV_STACK_FRAME(t2)
    /* set in t4 the address of the next free stack frame - will be used in
        the next engine call */
    add     t4, t2, t4
    /* enable interrupts */
    /* TODO */
    /* at this point t4 points to the next free frame, t3 points to the top
       of the com-rv stack and t2 holds the allocated stack frame. */
    /* link the allocated frame to its stack - we link the allocated frame
       to the stack pointed by t3. */
    /* calc the offset from the new stack frame (t2) to previous stack frame (t3) */
    sub     t3, t2, t3
    /* the sign helps us in clculating the offsets between frames */
    neg     t3, t3
    /* in current stack frame (t2r) save the offset to previous stack frame */
    sh      t3, D_COMRV_PREV_STACK_FRAME(t2)
    /* link allocated stack frame */
    mv      t3, t2
    /* save callee token */
    m_STORE t5, D_COMRV_CALLEE_TOKEN_STACK_OFFSET(t3)
    /* save the return address */
    m_STORE ra, D_COMRV_CALLER_RA_STACK_OFFSET(t3)
    /* check if callee token is an overlay token (in t5 bit0 is set) */
    andi    t2, t5, 1
    /* if t2 is 0 it means callee is a non overlay function so we can
       skip search and load */
    beqz    t2, comrv_invoke_callee
    /* change sp for temporary storage */
    m_ADDI  sp, sp, -D_COMRV_STACK_SIZE
    /* temporary save a0 in the next free stack frame */
    m_STORE a0, D_COMRV_A0_REG_STACK_OFFSET_TMP(sp)
    /* search if the token (t5) is already loaded */
    jal     comrvSearchCurrentAddressToken
    /* if the overlay token is already loaded a0 holds the actual address */
    bnez    a0, comrv_skip_load
    /* prepare address of comrvLoadCurrentAddressToken output arg - overlay loaded
       to comrv table entry */
    m_ADDI  a0, t3, D_COMRV_GROUP_SIZE_STACK_OFFSET
    /* load the overlay function according to the t5 token */
    jal     comrvLoadCurrentAddressToken
comrv_skip_load:
    /* save the loaded function address in t5 (instead of the token) */
    mv      t5, a0
    /* restore a0 - may contain an actual callee function argument */
    m_LOAD  a0, D_COMRV_A0_REG_STACK_OFFSET_TMP(sp)
    /* adjust back sp - memory not needed anymore */
    m_ADDI  sp, sp, D_COMRV_STACK_SIZE
comrv_invoke_callee:
    /* call the function */
    jalr    t5
    /* load the return address */
    m_LOAD  ra, D_COMRV_CALLER_RA_STACK_OFFSET(t3)
    /* we need to load the caller token to detrmine if the RA is an offset
       or an actual return address; first we need to unlink the top of the
       com-rv stack frame, return it back to the stack frame pool and get
       read the caller token (in previous frame it will bethe  callee token) */
    /* clear the callee token field */
    m_STORE zero, D_COMRV_CALLEE_TOKEN_STACK_OFFSET(t3)
    /* load the offset to previous stack frame */
    lh      t5, D_COMRV_PREV_STACK_FRAME(t3)
    /* unlink the top of the stack - t3 hold the unlinked stack frame address
       t5 will hold the address of the top of the stack */
    add     t5, t5, t3
    /* disable interrupts */
    /* TODO */
    /* link the freed stack frame back to the stack frames pool;
       t2 will hold the offset to the previous free stack frame */
    sub     t4, t3, t4
    /* the sign helps us in clculating the offsets between frames */
    neg     t4, t4
    /* save the offset to the previous free stack frame */
    sh      t4, D_COMRV_PREV_STACK_FRAME(t3)
    /* link the free stack frame back to the pool */
    mv      t4, t3
    /* enable interrupts */
    /* TODO */
    /* save the top of the stack to the stack register */
    mv      t3, t5
    /* read caller token (will be in the callee field now as we look at
       the previous stack frame) */
    m_LOAD  t5, D_COMRV_CALLEE_TOKEN_STACK_OFFSET(t3)
    /* t2 will hold the token type - address/token */
    andi    t2, t5, 1
    /* t2 is 0 means return to a non-overlay function so we can skip
       token search and load */
    beqz    t2, comrv_exit_ret_to_caller
    /* change sp for temporary storage */
    m_ADDI  sp, sp, -D_COMRV_STACK_SIZE
    /* temporary save a0 as it is being used */
    m_STORE a0, D_COMRV_A0_REG_STACK_OFFSET_TMP(sp)
    /* temporary save ra as it is being used */
    m_STORE ra, D_COMRV_RA_REG_STACK_OFFSET_TMP(sp)
    /* search if the token (t5) is already loaded */
    jal     comrvSearchCurrentAddressToken
    /* if a0 not equals 0 it means the overlay is loaded  and a0 contains the
       start address of the loaded overlay function */
    bgtu    a0, zero, comrv_exit_skip_load
    /* the arg passed to comrvLoadCurrentAddressToken isn't used now so
       we just provide it a temp location on the stack */
    m_ADDI  a0, sp, D_COMRV_ARG_STACK_OFFSET_TMP
    /* load the overlay group */
    jal     comrvLoadCurrentAddressToken
comrv_exit_skip_load:
    /* restore ra - load the saved address */
    m_LOAD  ra, D_COMRV_RA_REG_STACK_OFFSET_TMP(sp)
    /* get the function offset from the token */
    srli    t2, t5, D_COMRV_FUNC_OFFSET_SMNT
    andi    t2, t2, D_COMRV_FUNC_OFFSET_MASK
    /* subtract the function offset from the saved RA */
    sub     ra, ra, zero /* FIXME when we get the actual offset in the token change zero to t2 */
    /* read the group size */
    lh      t1, D_COMRV_GROUP_SIZE_STACK_OFFSET(t3)
    /* get the actual ra offset */
    and     ra, ra, t1
    /* an overlay token is already loaded then a0 will hold the actual
       address we can add to it the RA offset */
    add     ra, ra, a0
    /* restore a0 */
    m_LOAD  a0, D_COMRV_A0_REG_STACK_OFFSET_TMP(sp)
    /* adjust back sp - memory not needed anymore */
    m_ADDI  sp,  sp, D_COMRV_STACK_SIZE
comrv_exit_ret_to_caller:
    /* return back to the actual caller - ra hold the actual address */
    ret
