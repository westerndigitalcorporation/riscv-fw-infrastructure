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
.macro M_COMRV_SAVE_ARGS_REGS
   M_PSP_ADDI  sp, sp, -D_COMRV_STACK_FRAME_SIZE
   M_PSP_STORE a0, D_COMRV_A0_REG_STACK_OFFSET_TMP(sp)
   M_PSP_STORE a1, D_COMRV_A1_REG_STACK_OFFSET_TMP(sp)
   M_PSP_STORE a2, D_COMRV_A2_REG_STACK_OFFSET_TMP(sp)
   M_PSP_STORE a3, D_COMRV_A3_REG_STACK_OFFSET_TMP(sp)
   M_PSP_STORE a4, D_COMRV_A4_REG_STACK_OFFSET_TMP(sp)
   M_PSP_STORE a5, D_COMRV_A5_REG_STACK_OFFSET_TMP(sp)
   M_PSP_STORE a6, D_COMRV_A6_REG_STACK_OFFSET_TMP(sp)
   M_PSP_STORE a7, D_COMRV_A7_REG_STACK_OFFSET_TMP(sp)
.endm

.macro M_COMRV_RESTORE_ARGS_REGS
   M_PSP_LOAD  a0, D_COMRV_A0_REG_STACK_OFFSET_TMP(sp)
   M_PSP_LOAD  a1, D_COMRV_A1_REG_STACK_OFFSET_TMP(sp)
   M_PSP_LOAD  a2, D_COMRV_A2_REG_STACK_OFFSET_TMP(sp)
   M_PSP_LOAD  a3, D_COMRV_A3_REG_STACK_OFFSET_TMP(sp)
   M_PSP_LOAD  a4, D_COMRV_A4_REG_STACK_OFFSET_TMP(sp)
   M_PSP_LOAD  a5, D_COMRV_A5_REG_STACK_OFFSET_TMP(sp)
   M_PSP_LOAD  a6, D_COMRV_A6_REG_STACK_OFFSET_TMP(sp)
   M_PSP_LOAD  a7, D_COMRV_A7_REG_STACK_OFFSET_TMP(sp)
   M_PSP_ADDI  sp, sp, D_COMRV_STACK_FRAME_SIZE
.endm

#ifdef D_COMRV_RTOS_SUPPORT
.macro m_COMRV_RESTORE_INTS restoreStatusFromReg
   M_PSP_RESTORE_MINTERRUPTS \restoreStatusFromReg
.endm
#else
.macro m_COMRV_RESTORE_INTS restoreStatusFromReg
.endm
#endif /* D_COMRV_RTOS_SUPPORT */

#ifdef D_COMRV_RTOS_SUPPORT
.macro m_COMRV_DISABLE_INTS saveStatusInReg
   M_PSP_DISABLE_MINTERRUPTS \saveStatusInReg
.endm
#else
.macro m_COMRV_DISABLE_INTS saveStatusInReg
.endm
#endif /* D_COMRV_RTOS_SUPPORT */

