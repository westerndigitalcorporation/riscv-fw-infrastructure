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

/**
* include files
*/

/**
* macros
*/

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
   #define M_PSP_ASSERT(checkedResult) if (checkedResult) \
                                     { asm volatile ("ebreak" : : : );}
#else
   #define M_PSP_ASSERT(checkedResult)
#endif /* D_PSP_DEBUG */


/***** CSR read *****/
#define _READ_CSR_(reg) (	{ unsigned long __tmp; \
  asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
  __tmp; })
#define _READ_CSR_INTERMEDIATE_(y) _READ_CSR_(y)
/*************************************************************************/
#define M_PSP_READ_CSR(x)  _READ_CSR_INTERMEDIATE_(x)
/*************************************************************************/

/***** CSR write *****/
#define _WRITE_CSR_(reg, val) ({ \
  if (__builtin_constant_p(val) && (unsigned long)(val) < 32) \
    asm volatile ("csrw " #reg ", %0" :: "i"(val)); \
  else \
    asm volatile ("csrw " #reg ", %0" :: "r"(val)); })
#define _WRITE_CSR_INTERMEDIATE_(y, val) _WRITE_CSR_(y, val)
/*************************************************************************/
#define M_PSP_WRITE_CSR(x, val)  _WRITE_CSR_INTERMEDIATE_(x, val)
/*************************************************************************/

/***** CSR swap *****/
#define _SWAP_CSR_(reg, val) ({ unsigned long __tmp; \
  if (__builtin_constant_p(val) && (unsigned long)(val) < 32) \
    asm volatile ("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "i"(val)); \
  else \
    asm volatile ("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "r"(val)); \
  __tmp; })
#define _SWAP_CSR_INTERMEDIATE_(y, val) _SWAP_CSR_(y, val)
/*************************************************************************/
#define M_PSP_SWAP_CSR(x, val)  _SWAP_CSR_INTERMEDIATE_(x, val)
/*************************************************************************/

/***** CSR set *****/
#define _SET_CSR_(reg, bit) ({ unsigned long __tmp; \
  if (__builtin_constant_p(bit) && (unsigned long)(bit) < 32) \
    asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "i"(bit)); \
  else \
    asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "r"(bit)); \
  __tmp; })
#define _SET_CSR_INTERMEDIATE_(y, bit) _SET_CSR_(y, bit)
/*************************************************************************/
#define M_PSP_SET_CSR(x, bit)  _SET_CSR_INTERMEDIATE_(x, bit)
/*************************************************************************/

/***** CSR clear *****/
#define _CLEAR_CSR_(__tmp, reg, bit) ({ \
  if (__builtin_constant_p(bit) && (unsigned long)(bit) < 32) \
    asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "i"(bit)); \
  else \
    asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "r"(bit)); \
  __tmp; })
/*#define _CLEAR_CSR_INTERMEDIATE_(y, bit) _CLEAR_CSR_(y, bit)*/
#define _CLEAR_CSR_INTERMEDIATE_(__tmp, y, bit) _CLEAR_CSR_(__tmp, y, bit)
/*************************************************************************/
/*#define M_PSP_CLEAR_CSR(x, bit)  _CLEAR_CSR_INTERMEDIATE_(x, bit)*/
#define M_PSP_CLEAR_CSR(__tmp, x, bit)  _CLEAR_CSR_INTERMEDIATE_(__tmp, x, bit)
/*************************************************************************/

/** Nati - TO DO: add more macros as the above ones. using single instructions that does read & clear/set/write etc. **/



#define M_PSP_EBREAK()              asm volatile ("ebreak" : : : );
#define M_PSP_ECALL()               asm volatile ("ecall" : : : );
#define M_PSP_NOP()                 asm volatile ("nop")
#define M_PSP_MEMORY_BARRIER()      asm volatile( "" ::: "memory" )

/*******************************************************/

/* __builtin_expect instruction provides branch
   prediction information. The condition parameter is the expected
   comparison value. If it is equal to 1 (true), the condition
   is likely to be true, in other case condition is likely to be false.
   this provides us a way to take rare cases out of the critical execution path */
#define M_PSP_BUILTIN_EXPECT(condition, expected)  __builtin_expect(condition, expected)
/*******************************************************/


#endif /* __PSP_MACRO_H__ */
