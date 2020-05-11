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
* @file   psp_core_base.inc
* @author Nati Rapaport
* @date   13.11.2019
* @brief  The file defines psp macros that are used in Assembly code. These macros depend on the core definition of 32/64 bits
*/


/* Macros that depend on whether the core is 32bits or 64bits */
.if __riscv_xlen == 32
    .equ REGBYTES, 4

    .macro M_PSP_STORE operand1,operand2
        sw \operand1, \operand2
    .endm

    .macro M_PSP_LOAD operand1,operand2
        lw \operand1, \operand2
    .endm

    .macro M_PSP_ADDI operand1,operand2,operand3
        addi \operand1, \operand2, \operand3
    .endm

.elseif __riscv_xlen == 64
    .error "You must verify it is work correctly for 64 bit"

    .equ REGBYTES, 8

    .macro M_PSP_STORE operand1,operand2
        sd \operand1, \operand2
    .endm

    .macro M_PSP_LOAD operand1,operand2
        ld \operand1, \operand2
    .endm

    .macro M_PSP_ADDI operand1,operand2,operand3
        addiw \operand1, \operand2, \operand3
    .endm
.endif

.macro M_PSP_STORE_BYTE operand1,operand2
    sb \operand1, \operand2
.endm

.macro M_PSP_LOAD_BYTE operand1,operand2
    lb \operand1, \operand2
.endm

.macro M_PSP_STORE_HALF operand1,operand2
    sh \operand1, \operand2
.endm

.macro M_PSP_LOAD_HALF operand1,operand2
    lh \operand1, \operand2
.endm
