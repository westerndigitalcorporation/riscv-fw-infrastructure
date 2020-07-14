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
* @file   psp_macro.h
* @author Ronen Haen
* @date   20.05.2019
* @brief  The file defines the psp macros
*/
#ifndef __PSP_MACRO_H__
#define __PSP_MACRO_H__

/* TODO: this #ifdef is temporarily here. Should replace it with proper definitions of statements in this file (e.g. "asm volatile")
 *       and wrapping them with this protection */
 #if defined (__GNUC__) || defined (__clang__)

/**
* include files
*/

/**
* macros
*/

#define M_PSP_BIT_MASK(num) (1 << (num))
#define M_PSP_MULT_BY_4(val) ((val) << 2)


/* error checking macro */
#if (D_PSP_ERROR_CHECK==1)
   /* TODO: need to add default function */
   #define M_PSP_VALIDATE_FUNC_PARAM(param, conditionMet, returnCode) \
      if (conditionMet) \
      { \
         fptrParamErrorNotification((const void*)(param), returnCode); \
         return (returnCode); \
      }
#else
   #define M_PSP_VALIDATE_FUNC_PARAM(param, conditionMet, returnCode)
#endif /* #if (D_PSP_ERROR_CHECK==1) */

/* assert macro */
#ifdef D_PSP_DEBUG
   #define M_PSP_ASSERT(checkedResult) if (M_PSP_BUILTIN_EXPECT(checkedResult, 0)) \
                                          M_PSP_EBREAK()
#else
   #define M_PSP_ASSERT(checkedResult)
#endif /* D_PSP_DEBUG */


/***** CSR read *****/
#define _READ_CSR_(reg) (  { unsigned long val; \
  asm volatile ("csrr %0, " #reg : "=r"(val)); \
  val; })
#define _READ_CSR_INTERMEDIATE_(reg) _READ_CSR_(reg)
/*************************************************************************/
/***** CSR write *****/
#define _WRITE_CSR_(reg, val) ({ \
  if (__builtin_constant_p(val) && (unsigned long)(val) < 32) \
    asm volatile ("csrw " #reg ", %0" :: "i"(val)); \
  else \
    asm volatile ("csrw " #reg ", %0" :: "r"(val)); })
#define _WRITE_CSR_INTERMEDIATE_(reg, val) _WRITE_CSR_(reg, val)
/*************************************************************************/
/***** CSR set *****/
#define _SET_CSR_(reg, bits) ({\
  if (__builtin_constant_p(bits) && (unsigned long)(bits) < 32) \
    asm volatile ("csrs " #reg ", %0" :: "i"(bits)); \
  else \
    asm volatile ("csrs " #reg ", %0" :: "r"(bits)); })
#define _SET_CSR_INTERMEDIATE_(reg, bits) _SET_CSR_(reg, bits)
/*************************************************************************/
/***** CSR clear *****/
#define _CLEAR_CSR_(reg, bits) ({\
  if (__builtin_constant_p(bits) && (unsigned long)(bits) < 32) \
    asm volatile ("csrc " #reg ", %0" :: "i"(bits)); \
  else \
    asm volatile ("csrc " #reg ", %0" :: "r"(bits)); })
#define _CLEAR_CSR_INTERMEDIATE_(reg, bits) _CLEAR_CSR_(reg, bits)
/*************************************************************************/
/***** CSR swap (read & write) *****/
#define _SWAP_CSR_(read_val, reg, write_val) ({ \
  if (__builtin_constant_p(write_val) && (unsigned long)(write_val) < 32) \
    asm volatile ("csrrw %0, " #reg ", %1" : "=r"(read_val) : "i"(write_val)); \
  else \
    asm volatile ("csrrw %0, " #reg ", %1" : "=r"(read_val) : "r"(write_val)); \
  read_val; })
#define _SWAP_CSR_INTERMEDIATE_(read_val, reg, write_val) _SWAP_CSR_(read_val, reg, write_val)
/*************************************************************************/
/***** CSR set & read *****/
#define _SET_AND_READ_CSR_(read_val, reg, bits) ({ \
  if (__builtin_constant_p(bits) && (unsigned long)(bits) < 32) \
    asm volatile ("csrrs %0, " #reg ", %1" : "=r"(read_val) : "i"(bits)); \
  else \
    asm volatile ("csrrs %0, " #reg ", %1" : "=r"(read_val) : "r"(bits)); \
  read_val; })
#define _SET_AND_READ_CSR_INTERMEDIATE_(read_val, reg, bits) _SET_AND_READ_CSR_(read_val, reg, bits)
/*************************************************************************/
/***** CSR clear & read *****/
#define _CLEAR_AND_READ_CSR_(read_val, reg, bits) ({ \
  if (__builtin_constant_p(bits) && (unsigned long)(bits) < 32) \
    asm volatile ("csrrc %0, " #reg ", %1" : "=r"(read_val) : "i"(bits)); \
  else \
    asm volatile ("csrrc %0, " #reg ", %1" : "=r"(read_val) : "r"(bits)); \
  read_val; })
#define _CLEAR_AND_READ_CSR_INTERMEDIATE_(read_val, reg, bits) _CLEAR_AND_READ_CSR_(read_val, reg, bits)
/*************************************************************************/


/******************************** Macros of CSRs handling ********************************/
#define M_PSP_READ_CSR(csr)                           _READ_CSR_INTERMEDIATE_(csr)
#define M_PSP_WRITE_CSR(csr, val)                     _WRITE_CSR_INTERMEDIATE_(csr, val)
#define M_PSP_SET_CSR(csr, bits)                      _SET_CSR_INTERMEDIATE_(csr, bits)
#define M_PSP_CLEAR_CSR(csr, bits)                    _CLEAR_CSR_INTERMEDIATE_(csr, bits)
#define M_PSP_SWAP_CSR(read_val, csr, write_val)      _SWAP_CSR_INTERMEDIATE_(csr, val)
#define M_PSP_SET_AND_READ_CSR(read_val, csr, bits)   _SET_AND_READ_CSR_INTERMEDIATE_(read_val, csr, bits)
#define M_PSP_CLEAR_AND_READ_CSR(read_val, csr, bits) _CLEAR_AND_READ_CSR_INTERMEDIATE_(read_val, csr, bits)
/*****************************************************************************************/

#define M_PSP_EBREAK()              asm volatile ("ebreak" : : : );
#define M_PSP_ECALL()               asm volatile ("ecall" : : : );
#define M_PSP_NOP()                 asm volatile ("nop")
#define M_PSP_MEMORY_BARRIER()      asm volatile ( "" ::: "memory" )

/*******************************************************/

/* __builtin_expect instruction provides branch
   prediction information. The condition parameter is the expected
   comparison value. If it is equal to 1 (true), the condition
   is likely to be true, in other case condition is likely to be false.
   this provides us a way to take rare cases out of the critical execution path */
#define M_PSP_BUILTIN_EXPECT(condition, expected)  __builtin_expect(condition, expected)

/*******************************************************/

/* order device I/O and memory accesses */
#define M_PSP_INST_FENCE()          asm volatile( "fence rw,rw" )
/* synchronize the instruction and data streams */
#define M_PSP_INST_FENCEI()         asm volatile( "fence.i" )



/*******************************************************/


#define M_PSP_WRITE_REGISTER_32(reg, value)  ((*(volatile u32_t *)(void*)(reg)) = (value))  //need to use _Uncached u32_t if we have d$
#define M_PSP_READ_REGISTER_32(reg)          ((*(volatile u32_t *)(void*)(reg)))
#define M_PSP_SET_REGISTER_32(reg, bits)     ((*(volatile u32_t *)(void*)(reg)) |= (bits))  //need to use _Uncached u32_t if we have d$
#define M_PSP_CLEAR_REGISTER_32(reg, bits)   ((*(volatile u32_t *)(void*)(reg)) &= (~bits)) //need to use _Uncached u32_t if we have d$

/* Get Core-Id */
#define M_PSP_GET_CORE_ID()    M_PSP_READ_CSR(D_PSP_MHARTID_NUM)

/* TODO: - this #ifdef is temporarily here. Should replace it with proper definitions of statements in this file (e.g. "asm volatile")
 *         and wrapping them with this protection */
#endif /* (__GNUC__) || defined (__clang__)*/

#endif /* __PSP_MACRO_H__ */
