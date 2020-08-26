/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2020 Western Digital Corporation or its affiliates.
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
* @file   psp_bitmanip_eh2.h
* @author Nati Rapaport
* @date   23.07.2020
* @brief  The file contains APIs for bit-manipulation operations on SweRV EH2
*/
#ifndef  __PSP_BITMANIP_EH2_H__
#define  __PSP_BITMANIP_EH2_H__


/**
* include files
*/

/**
* definitions
*/
#define D_PSP_HIGHEST_BIT_NUMBER   __riscv_xlen-1

/**
* types
*/

/**
* local prototypes
*/

/**
* external prototypes
*/

/**
* macros
*/
/* clz (count leading zeros) command  - count number of zero-bits up to the most significant '1' bit */
#define M_PSP_BITMANIP_CLZ(argument , result)  asm volatile("clz %0, %1" : "=r"(result): "r"(argument) : );

/* ctz (count trailing zeros) command - count number of zero-bits from the least significant '1' bit */
#define M_PSP_BITMANIP_CTZ(argument , result)  asm volatile("ctz %0, %1" : "=r"(result): "r"(argument) : );

/* pcnt (population count of '1' bits) command - count number of '1' bits in the argument */
#define M_PSP_BITMANIP_PCNT(argument, number_of_ones)  asm volatile("pcnt %0, %1" : "=r"(number_of_ones) : "r"(argument) : );

/* andn command - bitwise AND of 1'st argument and inverted 2'nd argument */
#define M_PSP_BITMANIP_ANDN(argument_to_and, argument_to_invert, result) asm volatile("andn %0, %1, %2" : "=r"(result) : "r"(argument_to_and), "r"(argument_to_invert) : );

/* orn command - bitwise OR of 1'st argument and inverted 2'nd argument */
#define M_PSP_BITMANIP_ORN(argument_to_or, argument_to_invert, result) asm volatile("orn %0, %1, %2" : "=r"(result) : "r"(argument_to_or), "r"(argument_to_invert) : );

/* xnor command - bitwise XOR of 1'st argument and inverted 2'nd argument */
#define M_PSP_BITMANIP_XNOR(argument_to_xor, argument_to_invert, result) asm volatile("xnor %0, %1, %2" : "=r"(result) : "r"(argument_to_xor), "r"(argument_to_invert) : );

/* min command - minimum of the 1'st and 2'nd arguments, compared as signed integers */
#define M_PSP_BITMANIP_MIN(argument1, argument2, smallest_argument) asm volatile("min %0, %1, %2" : "=r"(smallest_argument) : "r"(argument1), "r"(argument2) : );

/* max command - maximum of the 1'st and 2'nd arguments, compared as signed integers */
#define M_PSP_BITMANIP_MAX(argument1, argument2, largest_argument) asm volatile("max %0, %1, %2" : "=r"(largest_argument) : "r"(argument1), "r"(argument2) : );

/* minu command - minimum of the 1'st and 2'nd arguments, compared as unsigned integers */
#define M_PSP_BITMANIP_MINU(argument1, argument2, smallest_argument) asm volatile("minu %0, %1, %2" : "=r"(smallest_argument) : "r"(argument1), "r"(argument2) : );

/* maxu command - maximum of the 1'st and 2'nd arguments, compared as unsigned integers */
#define M_PSP_BITMANIP_MAXU(argument1, argument2, largest_argument) asm volatile("maxu %0, %1, %2" : "=r"(largest_argument) : "r"(argument1), "r"(argument2) : );

/* sextb - sign extend byte in a given 32bit argument */
#define M_PSP_BITMANIP_SEXTB(argument , result)  asm volatile("sext.b %0, %1" : "=r"(result): "r"(argument) : );

/* sexth - sign extend half-word (16 bits) in a given 32bit argument */
#define M_PSP_BITMANIP_SEXTH(argument , result)  asm volatile("sext.h %0, %1" : "=r"(result): "r"(argument) : );

/* pack command - pack lower halves of 2 arguments into one, with 1'st argument-half in the lower output-half and 2'nd argument-half in the upper output-half */
#define M_PSP_BITMANIP_PACK(argument1, argument2, result) asm volatile("pack %0, %1, %2" : "=r"(result) : "r"(argument1), "r"(argument2) : );

/* packu command - pack upper halves of 2 arguments into one, with 1'st argument-half in the lower output-half and 2'nd argument-half in the upper output-half */
#define M_PSP_BITMANIP_PACKU(argument1, argument2, result) asm volatile("packu %0, %1, %2" : "=r"(result) : "r"(argument1), "r"(argument2) : );

/* packh command - pack least-significant BYTES of 2 input arguments into 16 least-significant BITS of returned result, zero extending the rest of the returned result */
#define M_PSP_BITMANIP_PACKH(argument1, argument2, result) asm volatile("packh %0, %1, %2" : "=r"(result) : "r"(argument1), "r"(argument2) : );

/* rol command - Rotate left the first argument, N times (2'nd argument). This operation is similar to shift-left operation from the base spec, except it shift in
*                the values from the opposite side of the register, in order. This is also called ‘circular shift’. */
#define M_PSP_BITMANIP_ROL(argument_to_rotate, number_of_rtoations, result) asm volatile("rol %0, %1, %2" : "=r"(result) : "r"(argument_to_rotate), "r"(number_of_rtoations) : );

/* ror command - Rotate right the first argument, N times (2'nd argument) */
#define M_PSP_BITMANIP_ROR(argument_to_rotate, number_of_rtoations, result) asm volatile("ror %0, %1, %2" : "=r"(result) : "r"(argument_to_rotate), "r"(number_of_rtoations) : );

/* rori command - same as ror command but assembly uses 2'nd parameter as immediate number rather than using a register */
#define M_PSP_BITMANIP_RORI(argument_to_rotate, number_of_rtoations, result) asm volatile("rori %0, %1, %2" : "=r"(result) : "r"(argument_to_rotate), "i"(number_of_rtoations) : );

/* rev command - reverse the bits in the given argument (i.e. swaps bits 0 and 31, 1 and 30 etc.) */
#define M_PSP_BITMANIP_REV(argument_to_reverse, swapped_result) asm volatile("rev %0, %1" : "=r"(swapped_result) : "r"(argument_to_reverse) : );

/* rev8 command - swaps bytes in the given 32bit argument */
#define M_PSP_BITMANIP_REV8(argument_to_reverse, swapped_result) asm volatile("rev8 %0, %1" : "=r"(swapped_result) : "r"(argument_to_reverse) : );

/* orc.b command  */
#define M_PSP_BITMANIP_ORCB(argument_to_or, or_result) asm volatile("orc.b %0, %1" : "=r"(or_result) : "r"(argument_to_or) : );

/* orc.16 command  */
#define M_PSP_BITMANIP_ORC16(argument_to_or, or_result) asm volatile("orc16 %0, %1" : "=r"(or_result) : "r"(argument_to_or) : );

/* sbset command - single bit set, in a given position */
#define M_PSP_BITMANIP_SBSET(argument, bit_position, bit_set_result) asm volatile("sbset %0, %1, %2" : "=r"(bit_set_result) : "r"(argument), "r"(bit_position) : );

/* sbseti command - same as sbset command but assembly uses 2'nd parameter as immediate number rather than using a register */
#define M_PSP_BITMANIP_SBSETI(argument, bit_position, bit_set_result) asm volatile("sbseti %0, %1, %2" : "=r"(bit_set_result) : "r"(argument), "i"(bit_position) : );

/* sbclr command - single bit clear, in a given position */
#define M_PSP_BITMANIP_SBCLR(argument, bit_position, bit_clr_result) asm volatile("sbclr %0, %1, %2" : "=r"(bit_clr_result) : "r"(argument), "r"(bit_position) : );

/* sbclri command - same as sbclr command but assembly uses 2'nd parameter as immediate number rather than using a register */
#define M_PSP_BITMANIP_SBCLRI(argument, bit_position, bit_clr_result) asm volatile("sbclri %0, %1, %2" : "=r"(bit_clr_result) : "r"(argument), "i"(bit_position) : );

/* sbinv command - single bit invert, in a given position */
#define M_PSP_BITMANIP_SBINV(argument, bit_position, bit_inv_result) asm volatile("sbinv %0, %1, %2" : "=r"(bit_inv_result) : "r"(argument), "r"(bit_position) : );

/* sbinvi command - same as sbinv command but assembly uses 2'nd parameter as immediate number rather than using a register */
#define M_PSP_BITMANIP_SBINVI(argument, bit_position, bit_inv_result) asm volatile("sbinvi %0, %1, %2" : "=r"(bit_inv_result) : "r"(argument), "i"(bit_position) : );

/* sbext command - single bit extract, from a given position */
#define M_PSP_BITMANIP_SBEXT(argument, bit_position, bit_ext_result) asm volatile("sbext %0, %1, %2" : "=r"(bit_ext_result) : "r"(argument), "r"(bit_position) : );

/* sbexti command - same as sbext command but assembly uses 2'nd parameter as immediate number rather than using a register */
#define M_PSP_BITMANIP_SBEXTI(argument, bit_position, bit_ext_result) asm volatile("sbexti %0, %1, %2" : "=r"(bit_ext_result) : "r"(argument), "i"(bit_position) : );

/* Find First Set macro - returns the position of the 1'st '1' bit in the input argument */
/*
 * Note - This macro does not support a case of input argument = 0.
 *         In case of input=0 the returned value is -1
 */
#define M_PSP_BITMANIP_FFS(argument, result)   M_PSP_BITMANIP_CLZ(argument, result); \
                                               result = D_PSP_HIGHEST_BIT_NUMBER - result;

/* Find Last Set macro - returns the position of the last '1' in the input argument */
/*
 * Note - This macro does not support a case of input argument = 0.
 *         In case of input=0 the returned value is -1
 */
#define M_PSP_BITMANIP_FLS(argument, result)   M_PSP_BITMANIP_CTZ(argument, result); \
                                               result = D_PSP_HIGHEST_BIT_NUMBER - result;

#if __riscv_xlen == 64

/* addwu command - add arg2 to arg1 and then zero upper 32 bits */
#define M_PSP_BITMANIP_ADDWU(argument1, argument2, result) asm volatile("addwu %0, %1, %2" : "=r"(result) : "r"(argument1), "r"(argument2) : );

/* addiwu command - same as addwu command but assembly uses 2'nd parameter as immediate number rather than using a register */
#define M_PSP_BITMANIP_ADDIWU(argument1, argument2, result) asm volatile("addiwu %0, %1, %2" : "=r"(result) : "r"(argument1), "i"(argument2) : );

/* subwu command - subtract arg2 from arg1 and then zero upper 32 bits */
#define M_PSP_BITMANIP_SUBWU(argument1, argument2, result) asm volatile("subwu %0, %1, %2" : "=r"(result) : "r"(argument1), "r"(argument2) : );

/* addu.w command - zero upper 32 bits of arg2 and then add it to arg1 */
#define M_PSP_BITMANIP_ADDUW(argument1, argument2, result) asm volatile("addu.w %0, %1, %2" : "=r"(result) : "r"(argument1), "r"(argument2) : );

/* subu.w command - zero upper 32 bits of arg2 and then subtract it from arg1 */
#define M_PSP_BITMANIP_SUBUW(argument1, argument2, result) asm volatile("subu.w %0, %1, %2" : "=r"(result) : "r"(argument1), "r"(argument2) : );

/* slliu.w command - zero upper 32 bits of input argument and then shift-left it num_of_shifts times */
#define M_PSP_BITMANIP_SLLIUW(argument_to_shift, num_of_shifts, result) asm volatile("slliu.w %0, %1, %2" : "=r"(result) : "r"(argument_to_shift), "i"(num_of_shifts) : );

#endif /* __riscv_xlen == 64 */

/**
* global variables
*/

/**
* APIs
*/

#endif /* __PSP_BITMANIP_EH2_H__ */
