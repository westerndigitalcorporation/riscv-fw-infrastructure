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
* @file   comrv_asm_macros.h
* @author Ronen Haen
* @date   02.10.2019
* @brief  The defines COM-RV assembly macros
* 
*/

/**
* include files
*/

/**
* definitions
*/

/**
* macros
*/
.macro M_COMRV_SAVE_ARGS_REGS reg
   M_PSP_STORE a0, D_COMRV_A0_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE a1, D_COMRV_A1_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE a2, D_COMRV_A2_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE a3, D_COMRV_A3_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE a4, D_COMRV_A4_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE a5, D_COMRV_A5_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE a6, D_COMRV_A6_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE a7, D_COMRV_A7_REG_STACK_OFFSET_TMP(\reg)
.endm

.macro M_COMRV_RESTORE_ARGS_REGS reg
   M_PSP_LOAD  a0, D_COMRV_A0_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_LOAD  a1, D_COMRV_A1_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_LOAD  a2, D_COMRV_A2_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_LOAD  a3, D_COMRV_A3_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_LOAD  a4, D_COMRV_A4_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_LOAD  a5, D_COMRV_A5_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_LOAD  a6, D_COMRV_A6_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_LOAD  a7, D_COMRV_A7_REG_STACK_OFFSET_TMP(\reg)
.endm

#ifdef D_COMRV_RTOS_SUPPORT
.macro M_COMRV_SAVE_SAVED_REGS reg
   M_PSP_STORE s0, D_COMRV_S0_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE s1, D_COMRV_S1_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE s2, D_COMRV_S2_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE s3, D_COMRV_S3_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE s4, D_COMRV_S4_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE s5, D_COMRV_S5_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE s6, D_COMRV_S6_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE s7, D_COMRV_S7_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE s8, D_COMRV_S8_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE s9, D_COMRV_S9_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE s10, D_COMRV_S10_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE s11, D_COMRV_S11_REG_STACK_OFFSET_TMP(\reg)
.endm

.macro M_COMRV_RESTORE_SAVED_REGS reg
   M_PSP_LOAD  s0, D_COMRV_S0_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_LOAD  s1, D_COMRV_S1_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_LOAD  s2, D_COMRV_S2_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_LOAD  s3, D_COMRV_S3_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_LOAD  s4, D_COMRV_S4_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_LOAD  s5, D_COMRV_S5_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_LOAD  s6, D_COMRV_S6_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_LOAD  s7, D_COMRV_S7_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_LOAD  s8, D_COMRV_S8_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_LOAD  s9, D_COMRV_S9_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_LOAD  s10, D_COMRV_S10_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_LOAD  s11, D_COMRV_S11_REG_STACK_OFFSET_TMP(\reg)
.endm

.macro M_COMRV_SAVE_RTOS_STATE_REGS reg, state
   li          t2, \state
   M_PSP_STORE t2, D_COMRV_STATE_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE sp, D_COMRV_SP_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE ra, D_COMRV_RA_REG_STACK_OFFSET_TMP(\reg)
   M_PSP_STORE t5, D_COMRV_T5_REG_STACK_OFFSET_TMP(\reg)
.endm

.macro M_COMRV_SAVE_STATE state
   M_COMRV_DISABLE_INTS         t1
   M_COMRV_SAVE_ARGS_REGS       tp
   M_COMRV_SAVE_SAVED_REGS      tp
   M_COMRV_SAVE_RTOS_STATE_REGS tp, \state
   M_COMRV_RESTORE_INTS         t1
.endm
.macro M_COMRV_RESTORE_STATE
   M_COMRV_DISABLE_INTS         t1
   M_COMRV_RESTORE_ARGS_REGS    tp
   M_COMRV_RESTORE_INTS         t1
.endm
.macro M_COMRV_CLEAR_STATE
   M_PSP_STORE zero, D_COMRV_STATE_STACK_OFFSET_TMP(tp)
.endm
#else
.macro M_COMRV_SAVE_STATE state
   M_PSP_ADDI  sp, sp, -D_COMRV_STACK_FRAME_SIZE
   M_COMRV_SAVE_ARGS_REGS    sp
.endm
.macro M_COMRV_RESTORE_STATE
   M_COMRV_RESTORE_ARGS_REGS sp
   M_PSP_ADDI  sp, sp, D_COMRV_STACK_FRAME_SIZE
.endm
.macro M_COMRV_CLEAR_STATE
.endm
#endif /* D_COMRV_RTOS_SUPPORT */

#ifdef D_COMRV_RTOS_SUPPORT
.macro M_COMRV_RESTORE_INTS restoreStatusFromReg
   M_PSP_RESTORE_MINTERRUPTS \restoreStatusFromReg
.endm
#else
.macro M_COMRV_RESTORE_INTS restoreStatusFromReg
.endm
#endif /* D_COMRV_RTOS_SUPPORT */

#ifdef D_COMRV_RTOS_SUPPORT
.macro M_COMRV_DISABLE_INTS saveStatusInReg
   M_PSP_DISABLE_MINTERRUPTS \saveStatusInReg
.endm
#else
.macro M_COMRV_DISABLE_INTS saveStatusInReg
.endm
#endif /* D_COMRV_RTOS_SUPPORT */

