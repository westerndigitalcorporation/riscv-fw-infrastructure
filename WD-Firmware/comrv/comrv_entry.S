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

.section  .text.entry
.align 4
comrv_entry:
    /* TODO: what if we have more then 8 args to the overlay function ??? */
    /* new stack frame for comrv usage */
    m_ADDI  sp,  sp,-D_COMRV_STACK_SIZE
    /* save a0 and a1 as we are going to use them */
    m_STORE a0,  D_COMRV_A0_REG_STACK_OFFSET(sp)
    m_STORE a1,  D_COMRV_A1_REG_STACK_OFFSET(sp)
    /* prepare comrvGetCallerRaTokenAndOffset arguments */
    mv      a0,  ra
    m_ADDI  a1,  sp, D_COMRV_CALLER_RA_STACK_OFFSET
    /* save caller token and ra offset */
    jal comrvGetCallerRaTokenAndOffset
    /* save the caller token/address */
    m_STORE a0,  D_COMRV_CALLER_TOKEN_STACK_OFFSET(sp)
    /* store the return address */
    m_LOAD  a1,  D_COMRV_A1_REG_STACK_OFFSET(sp)
    /*m_STORE a0,  D_COMRV_CALLER_RA_STACK_OFFSET(sp)*/
comrv_check_token:
    /* check if address token is an overlay token (bit0 is set) */
    andi    x29,  x31, 1
    /* if x29 is 0 it means we call a non overlay function */
    beqz    x29,  comrv_invoke_callee
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
    andi    x29, x31, 1
    beqz    x29, comrv_exit_ret_to_caller
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
