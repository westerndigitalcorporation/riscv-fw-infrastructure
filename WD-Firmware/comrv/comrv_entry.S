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

.include "psp_asm_macros.h"


/* total size of used stack */
.equ D_COMRV_STACK_SIZE,                    REGBYTES * 4
/*
 stack layout:
 offset 0 - caller ra or caller ra offset in case the caller is an overlay function
 offset 1 - 0 if caller is a regular function or caller token in case the caller is an overlay function
 offset 2 - callee token - may be a regular address in case callee in a regular function
 offset 3 - temporary storage for a0 as it is being used by comrv_entry
*/
.equ D_COMRV_CALLER_RA_STACK_OFFSET,        REGBYTES * 0
.equ D_COMRV_CALLER_TOKEN_STACK_OFFSET,     REGBYTES * 1
.equ D_COMRV_A0_REG_STACK_OFFSET,           REGBYTES * 2
.equ D_COMRV_A1_REG_STACK_OFFSET,           REGBYTES * 3

.global   comrv_entry

.func  .text.entry
.align 4
comrv_entry:
    /* TODO: what if we have more then 8 args to the overlay function ??? */
    /* new stack frame for comrv usage */
    m_ADDI  sp,  sp,-D_COMRV_STACK_SIZE
    /* if x29 is 0 it means caller is non-overlay function */
    bnez    x29,  comrv_save_caller_token
    /* copy ra to caller token register */
    mv      x29,  ra
comrv_save_caller_token:
    /* save caller token */
    m_STORE x29, D_COMRV_CALLER_TOKEN_STACK_OFFSET(sp)
    /* check if caller token is an overlay token (bit0 is set) */
    andi    x28,  x29, 1
    /* if x28 is 1 it means caller token is an overlay token */
    beqz    x28,  comrv_check_callee_token
    /* calculate and store caller ra offset */
    xori    x28,  x29, 1
    sub     x28,  ra, x28
    m_STORE x28,  D_COMRV_CALLER_RA_STACK_OFFSET(sp)

comrv_check_callee_token:
    /* update caller token register with the callee token */
    mv      x29,  x31
    /* check if address token is an overlay token (bit0 is set) */
    andi    x28,  x31, 1
    /* if x28 is 0 it means we call a non overlay function */
    beqz    x28,  comrv_invoke_callee
    /* search if the token (x31) is already loaded */
    jal     comrvSearchCurrentAddressToken
    /* if the overlay token is already loaded (a0 will hold the actual address) */
    bnez    a0,  comrv_skip_load
    /* load the overlay function */
    jal     comrvLoadCurrentAddressToken
comrv_skip_load:
    /* save the loaded function address in x31 */
    mv      x31, a0
    /* restore a0 - may contain an actual callee function argument */
    m_LOAD  a0, D_COMRV_A0_REG_STACK_OFFSET(sp)
comrv_invoke_callee:
    /* call the function */
    jalr    x31
    /* We need to load the offset to the return address relative to the overlay token */
    m_LOAD  x31, D_COMRV_CALLER_TOKEN_STACK_OFFSET(sp)
    m_LOAD  ra,  D_COMRV_CALLER_RA_STACK_OFFSET(sp)
    /* check if address token is an overlay token */
    andi    x28, x31, 1
    /* if x28 is 0 it means caller is non-overlay function */
    bnez    x28,  comrv_skip_clear_x29
    /* clear x29 as we are returning to a non-overlay function */
    mv      x29,  zero
comrv_skip_clear_x29:
    /* skip token serch and load as we are returning to a non-overlay function */
    beqz    x28, comrv_exit_ret_to_caller
    /* store caller token in x29 */
    mv      x29,  x31
    /* save a0 as we are going to use it */
    m_STORE a0,  D_COMRV_A0_REG_STACK_OFFSET(sp)
    /* search if the token (x31) is already loaded */
    jal     comrvSearchCurrentAddressToken
    m_LOAD  ra,  D_COMRV_CALLER_RA_STACK_OFFSET(sp)
    /* an overlay token is already loaded thwn a0 will hold the actual address */
    add     a0, ra, a0
    /* if a0 and ra are not equal it means the overlay is loaded */
    bne     a0, ra, comrv_exit_skip_load
    /* load the overlay group */
    jal     comrvLoadCurrentAddressToken
    m_LOAD  ra,  D_COMRV_CALLER_RA_STACK_OFFSET(sp)
    add     a0, ra, a0
comrv_exit_skip_load:
    /* save the loaded function address in x31 */
    mv      x31, a0
    /* restore a0 - may contain an actual callee function argument */
    m_LOAD  a0, D_COMRV_A0_REG_STACK_OFFSET(sp)
comrv_exit_ret_to_caller:
    /* restore sp */
    m_ADDI  sp,  sp, D_COMRV_STACK_SIZE
    /* return back to the actual caller */
    jr      x31
.endfunc
