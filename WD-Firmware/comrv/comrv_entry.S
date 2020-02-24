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
.include "psp_macros.inc"
#include "comrv_config.h"
#include "comrv_macros.inc"
#include "comrv_config.h"

/* disable warning for reserved registers use - we are using comrv
   reserved register and don't want to see these warnings. */
.option nowarnreservedreg

/* size of used stack */
.equ D_COMRV_STACK_FRAME_SIZE,              REGBYTES * 32

/* thunk indication upper mask */
.equ D_COMRV_THUNK_UPPER_MASK,              0x8000

.equ D_COMRV_TOKEN_INDICATION,              1
.equ D_COMRV_INVOKE_CALLEE_BIT_0,           1
.equ D_COMRV_THUNK_SHMT,                    4

/*
 local stack frame layout:
 offset 0 - a0
 offset 1 - a1
 offset 2 - a2
 offset 3 - a3
 offset 4 - a4
 offset 5 - a5
 offset 6 - a6
 offset 7 - a7
*/
.equ D_COMRV_A0_REG_STACK_OFFSET_TMP,       REGBYTES * 0
.equ D_COMRV_A1_REG_STACK_OFFSET_TMP,       REGBYTES * 1
.equ D_COMRV_A2_REG_STACK_OFFSET_TMP,       REGBYTES * 2
.equ D_COMRV_A3_REG_STACK_OFFSET_TMP,       REGBYTES * 3
.equ D_COMRV_A4_REG_STACK_OFFSET_TMP,       REGBYTES * 4
.equ D_COMRV_A5_REG_STACK_OFFSET_TMP,       REGBYTES * 5
.equ D_COMRV_A6_REG_STACK_OFFSET_TMP,       REGBYTES * 6
.equ D_COMRV_A7_REG_STACK_OFFSET_TMP,       REGBYTES * 7

/*
 COM-RV stack frame layout - see comrvStackFrame_t:
*/
.equ D_COMRV_CALLER_RA_STACK_OFFSET,        REGBYTES * 0
.equ D_COMRV_CALLEE_TOKEN_STACK_OFFSET,     REGBYTES * 1
.equ D_COMRV_PREV_STACK_FRAME,              REGBYTES * 2


.global comrvEntry
.global comrvEntryDisable
.global comrv_ret_from_callee
.global comrvNotifyDisabledError

/* TODO: need to set this code in COMRV_TEXT_SEC */
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
    /* Add the indication we are calling a new overlay function
       this indication is required to calculate the correct offset
       when returning to an overlay function */
    ori         t3, t3, D_COMRV_INVOKE_CALLEE_BIT_0
    /* save a0-a7 on stack */
    M_COMRV_SAVE_ARGS_REGS
    /* search if the token (t5) is already loaded */
    jal         comrvGetAddressFromToken
    /* save the loaded function address in t5 (instead of the token) */
    mv          t5, a0
    /* restore a0-a7 from stack */
    M_COMRV_RESTORE_ARGS_REGS

comrv_invoke_callee:
    /* call the function */
    jalr        t5
comrv_ret_from_callee:
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

comrv_igonr_caller_thunk_stack_frame:
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
    bltz        t2, comrv_handle_thunk_token
    /* save a0-a7 on stack */
    M_COMRV_SAVE_ARGS_REGS
    /* pass the ra as the function parameter -
       used for calculating the return offset */
    mv          a0, ra
    /* search if the token (t5) is already loaded */
    jal         comrvGetAddressFromToken
    /* an overlay function is now loaded and a0 holds the actual
       address we can keep it in ra */
    mv          ra, a0
    /* restore a0-a7 from stack */
    M_COMRV_RESTORE_ARGS_REGS

comrv_exit_ret_to_caller:
    /* return back to the actual caller - ra hold the actual address */
    ret

comrv_handle_thunk_token:
    /* remove the thunk indication */
    lui         t2, D_COMRV_THUNK_UPPER_MASK
    xor         t5, t5, t2
    /* load the offset to previous stack frame */
    lh          t2, D_COMRV_PREV_STACK_FRAME(t3)
    /* t2 will point to the previous stack frame */
    add         t2, t2, t3
    /* save the actual token to previous stack frame before
       we skip current stack frame */
    M_PSP_STORE t5, D_COMRV_CALLEE_TOKEN_STACK_OFFSET(t2)
    /* optimization to save 'lh' instruction above 'comrv_igonr_caller_thunk_stack_frame:'*/
    mv          t5, t2
    /* now we can skip to the previous stack frame */
    j comrv_igonr_caller_thunk_stack_frame

/* for testing purpose, we use the .weak so we won't get stuck
   this also allows the end user to reimplement comrvEntryDisable
   diffrently */
.weak comrvEntryDisable
/* we get here if the end user disabled comrv and tried
   calling/loading overlay function/data */
comrvEntryDisable:
1:
    la          t0, comrvNotifyDisabledError
    jr          t0

