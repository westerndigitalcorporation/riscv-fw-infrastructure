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
.equ D_COMRV_STATE_ENTRY,                   1
.equ D_COMRV_STATE_EXIT,                    2

/*
 local stack frame layout:
 offset 0  - a0
 offset 1  - a1
 offset 2  - a2
 offset 3  - a3
 offset 4  - a4
 offset 5  - a5
 offset 6  - a6
 offset 7  - a7
 --- start 'RTOS support' specific layout
 offset 8  - sp
 offset 9  - ra
 offset 10 - t5
 offset 11 - comrv state indications - enter/exit, 'post search and load'
 --- end  'RTOS support' specific layout
*/
.equ D_COMRV_S0_REG_STACK_OFFSET_TMP,       REGBYTES * 0
.equ D_COMRV_S1_REG_STACK_OFFSET_TMP,       REGBYTES * 1
.equ D_COMRV_A0_REG_STACK_OFFSET_TMP,       REGBYTES * 2
.equ D_COMRV_A1_REG_STACK_OFFSET_TMP,       REGBYTES * 3
.equ D_COMRV_A2_REG_STACK_OFFSET_TMP,       REGBYTES * 4
.equ D_COMRV_A3_REG_STACK_OFFSET_TMP,       REGBYTES * 5
.equ D_COMRV_A4_REG_STACK_OFFSET_TMP,       REGBYTES * 6
.equ D_COMRV_A5_REG_STACK_OFFSET_TMP,       REGBYTES * 7
.equ D_COMRV_A6_REG_STACK_OFFSET_TMP,       REGBYTES * 8
.equ D_COMRV_A7_REG_STACK_OFFSET_TMP,       REGBYTES * 9
.equ D_COMRV_S2_REG_STACK_OFFSET_TMP,       REGBYTES * 10
.equ D_COMRV_S3_REG_STACK_OFFSET_TMP,       REGBYTES * 11
.equ D_COMRV_S4_REG_STACK_OFFSET_TMP,       REGBYTES * 12
.equ D_COMRV_S5_REG_STACK_OFFSET_TMP,       REGBYTES * 13
.equ D_COMRV_S6_REG_STACK_OFFSET_TMP,       REGBYTES * 14
.equ D_COMRV_S7_REG_STACK_OFFSET_TMP,       REGBYTES * 15
.equ D_COMRV_S8_REG_STACK_OFFSET_TMP,       REGBYTES * 16
.equ D_COMRV_S9_REG_STACK_OFFSET_TMP,       REGBYTES * 17
.equ D_COMRV_S10_REG_STACK_OFFSET_TMP,      REGBYTES * 18
.equ D_COMRV_S11_REG_STACK_OFFSET_TMP,      REGBYTES * 19
.equ D_COMRV_RA_REG_STACK_OFFSET_TMP,       REGBYTES * 20
.equ D_COMRV_T5_REG_STACK_OFFSET_TMP,       REGBYTES * 21
.equ D_COMRV_SP_REG_STACK_OFFSET_TMP,       REGBYTES * 22
.equ D_COMRV_STATE_STACK_OFFSET_TMP ,       REGBYTES * 23

/*
 COM-RV stack frame layout - see comrvStackFrame_t:
*/
.equ D_COMRV_CALLER_RA_STACK_OFFSET,        REGBYTES * 0
.equ D_COMRV_CALLEE_TOKEN_STACK_OFFSET,     REGBYTES * 1
.equ D_COMRV_PREV_STACK_FRAME,              REGBYTES * 2
.equ D_COMRV_ALIGN_VAL_STACK_OFFSET,        10
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
   #ifdef D_COMRV_MIN_NUM_OF_MULTI_GROUPS
      .equ D_COMRV_ALIGN_MG_STACK_OFFSET,         11
      .macro M_COMRV_STORE_MG operand1,operand2
          M_COMRV_STORE_BYTE \operand1, \operand2
      .endm
      .macro M_COMRV_LOAD_MG operand1,operand2
          M_COMRV_LOAD_BYTE \operand1, \operand2
      .endm
   #else
      .equ D_COMRV_ALIGN_MG_STACK_OFFSET,         14
      .macro M_COMRV_STORE_MG operand1,operand2
          M_COMRV_STORE_HALF \operand1, \operand2
      .endm
      .macro M_COMRV_LOAD_MG operand1,operand2
          M_COMRV_LOAD_HALF \operand1, \operand2
      .endm
   #endif /* D_COMRV_MIN_NUM_OF_MULTI_GROUPS */
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */

.global comrvEntry
.global comrvEntryDisable
.global comrv_ret_from_callee
.global comrvNotifyDisabledError
.global comrvEntry_context_switch
.global comrv_ret_from_callee_context_switch
.global comrv_pre_ret_to_asm_engine
.global comrvEntryNotInitialized
.global comrvNotifyNotInitialized

/*
   COMRV_TEXT_ASM section flags:
   a - ignored. (For compatibility with the ELF version)
   x - executable section
*/
.section  COMRV_TEXT_ASM, "ax"

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
    /* disable interrupts - save current status in t1*/
    M_COMRV_DISABLE_INTS t1
    /* RTOS support: make sure existing state is cleared */
    M_COMRV_CLEAR_STATE
    /* save the address of the next free stack frame in t2 - new stack frame
       for this engine call */
    mv            t2, t4
    /* load to t4 the offset to the previous free stack frame */
    lh            t4, D_COMRV_PREV_STACK_FRAME(t2)
    /* set in t4 the address of the next free stack frame - will be used in
        the next engine call */
    add           t4, t2, t4
    /* restore interrupts from t1 register */
    M_COMRV_RESTORE_INTS t1
    /* at this point t4 points to the next free frame, t3 points to the top
       of the com-rv stack and t2 holds the allocated stack frame. */
    /* link the allocated frame to its stack - we link the allocated frame
       to the stack pointed by t3. */
    /* calc the offset from the new stack frame (t2) to previous stack frame (t3) */
    sub           t3, t3, t2
    /* in current stack frame (t2) save the offset to previous stack frame */
    sh            t3, D_COMRV_PREV_STACK_FRAME(t2)
    /* link allocated stack frame */
    mv            t3, t2
    /* save callee token */
    M_COMRV_STORE t5, D_COMRV_CALLEE_TOKEN_STACK_OFFSET(t3)
    /* save the return address */
    M_COMRV_STORE ra, D_COMRV_CALLER_RA_STACK_OFFSET(t3)
    /* check if callee token is an overlay token (in t5 bit0 is set) */
    andi          t2, t5, D_COMRV_TOKEN_INDICATION
    /* if t2 is 0 it means callee is a non overlay function so we can
       skip search and load */
    beqz          t2, comrv_invoke_callee
    /* Add the indication we are calling a new overlay function
       this indication is required to calculate the correct offset
       when returning to an overlay function */
    ori           t3, t3, D_COMRV_INVOKE_CALLEE_BIT_0
    /* save a0-a7 on stack, in rtos support also: s0-s11 on stack + comrv state */
    M_COMRV_SAVE_REGS_AND_STATE D_COMRV_STATE_ENTRY
comrvEntry_context_switch:
    /* search if the token (t5) is already loaded */
    jal         comrvGetAddressFromToken
    /* save the loaded function address in t5 (instead of the token) */
    mv          t5, a0
    /* restore a0-a7 from stack */
    M_COMRV_RESTORE_A_REGS

comrv_invoke_callee:
    /* call the function */
    jalr        t5
comrv_ret_from_callee:
    /* disable interrupts - save current status in t1*/
    M_COMRV_DISABLE_INTS t1
    /* RTOS support: make sure existing state is cleared */
    M_COMRV_CLEAR_STATE
    /* load the return address */
    M_COMRV_LOAD  ra, D_COMRV_CALLER_RA_STACK_OFFSET(t3)
    /* we need to load the caller token to detrmine if the RA is to an overlay
       or an actual return address; first we need to unlink the top of the
       com-rv stack frame, return it back to the stack frame pool and get
       read the caller token (in previous frame it will be the callee token) */
    /* load the offset to previous stack frame */
    lh            t5, D_COMRV_PREV_STACK_FRAME(t3)
    /* unlink the top of the stack - t3 hold the unlinked stack frame address
       t5 will hold the address of the top of the stack */
    add           t5, t5, t3

comrv_igonr_caller_thunk_stack_frame:
    /* link the freed stack frame back to the stack frames pool;
       t2 will hold the offset to the previous free stack frame */
    sub           t4, t4, t3
    /* save the offset to the previous free stack frame */
    sh            t4, D_COMRV_PREV_STACK_FRAME(t3)
    /* link the free stack frame back to the pool */
    mv            t4, t3
    /* save the top of the stack to the stack register */
    mv            t3, t5
    /* restore interrupts from t1 register */
    M_COMRV_RESTORE_INTS t1
    /* read caller token (will be in the callee field now as we look at
       the previous stack frame) */
    M_COMRV_LOAD  t5, D_COMRV_CALLEE_TOKEN_STACK_OFFSET(t3)
    /* t2 will hold the token type - address/token */
    andi          t2, t5, D_COMRV_TOKEN_INDICATION
    /* t2 is 0 means return to a non-overlay function so we can skip
       token search and load */
    beqz          t2, comrv_exit_ret_to_caller
    /* place thunk bit in the msb */
    slli          t2, t5, D_COMRV_THUNK_SHMT
    /* if thunk bit set (t2 negative) we skip current comrv stack frame */
    bltz          t2, comrv_handle_thunk_token
    /* in rtos support save comrv state */
    M_COMRV_SAVE_STATE D_COMRV_STATE_EXIT
comrv_ret_from_callee_context_switch:
    /* pass the ra as the function parameter -
       used for calculating the return offset */
    mv            a0, ra
    /* search if the token (t5) is already loaded */
    jal           comrvGetAddressFromToken
    /* an overlay function is now loaded and a0 holds the actual
       address we can keep it in ra */
    mv            ra, a0
    /* restore a0-a7 from stack */
    M_COMRV_RESTORE_A_REGS

comrv_exit_ret_to_caller:
    /* return back to the actual caller - ra hold the actual address */
    ret

comrv_handle_thunk_token:
    /* remove the thunk indication */
    lui               t2, D_COMRV_THUNK_UPPER_MASK
    xor               t5, t5, t2
    /* load the offset to previous stack frame */
    lh                t2, D_COMRV_PREV_STACK_FRAME(t3)
    /* t2 will point to the previous stack frame */
    add               t2, t2, t3
    /* save the actual token to previous stack frame before
       we skip current stack frame */
    M_COMRV_STORE     t5, D_COMRV_CALLEE_TOKEN_STACK_OFFSET(t2)
    /* TODO: in case of MG with min num of groups, we can save the
       'load address alignmebt value' and the MG fields in one load/store
       instrunction */
    /* load from current stack frame into t5 the 'load address alignment value' */
    lb                t5, D_COMRV_ALIGN_VAL_STACK_OFFSET(t3)
    /* save the 'load address alignment value' in previous stack frame */
    sb                t5, D_COMRV_ALIGN_VAL_STACK_OFFSET(t2)
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
    /* load from current stack frame into t5 the 'multigroup table entry' */
    M_COMRV_LOAD_MG   t5, D_COMRV_ALIGN_MG_STACK_OFFSET(t3)
    /* save the 'multigroup table entry' in previous stack frame */
    M_COMRV_STORE_MG  t5, D_COMRV_ALIGN_MG_STACK_OFFSET(t2)
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */
    /* optimization to save 'lh' instruction above 'comrv_igonr_caller_thunk_stack_frame:'*/
    mv                t5, t2
    /* disable interrupts - save current status in t1*/
    M_COMRV_DISABLE_INTS t1
    /* now we can skip to the previous stack frame */
    j                 comrv_igonr_caller_thunk_stack_frame

#if defined(D_COMRV_RTOS_SUPPORT) && defined(D_COMRV_ALLOW_CALLS_AFTER_SEARCH_LOAD)
comrv_pre_ret_to_asm_engine:
    /* restore s0 - s11 regs */
    M_COMRV_RESTORE_SAVED_REGS tp
    /* restore original ra */
    M_COMRV_LOAD      ra, D_COMRV_RA_REG_STACK_OFFSET_TMP(tp)
    /* restore original t5 */
    M_COMRV_LOAD      t5, D_COMRV_T5_REG_STACK_OFFSET_TMP(tp)
    /* restore original sp */
    M_COMRV_LOAD      sp, D_COMRV_SP_REG_STACK_OFFSET_TMP(tp)
    /* enable interrupts */
    M_COMRV_RESTORE_INTS a0
    /* return back to the engine */
    jr                a1
#endif /* D_COMRV_RTOS_SUPPORT && D_COMRV_ALLOW_CALLS_AFTER_SEARCH_LOAD */

/* NOTE: no additional lable shall be placed below this line
   as it may break the COMRV RTOS support */

#ifdef D_COMRV_CONTROL_SUPPORT
/* for testing purpose, we use the .weak so we won't get stuck
   this also allows the end user to reimplement comrvEntryDisable
   diffrently */
.weak comrvEntryDisable
#endif /* D_COMRV_CONTROL_SUPPORT */
/* we get here if the end user disabled comrv and tried
   calling/loading overlay function/data */
comrvEntryDisable:
    la          t0, comrvNotifyDisabledError
    jr          t0

#ifdef D_COMRV_RTOS_SUPPORT
/* this function is being used to make sure that
   if an overlay function has been invoked while
   comrv hasn't been initialize and at least one task
   has been created, the end user won't get exception */
comrvEntryNotInitialized:
    la         t0, comrvNotifyNotInitialized
    jr         t0
#endif /* D_COMRV_RTOS_SUPPORT */

