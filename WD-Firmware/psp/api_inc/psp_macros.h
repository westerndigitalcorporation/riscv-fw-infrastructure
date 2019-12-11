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

#if (D_PSP_ASSERT==1)
   #define M_PSP_ASSERT(checkedResult)
   /* TODO add assert call */
#else
   #define M_PSP_ASSERT(checkedResult)
#endif /* #if (D_PSP_ASSERT==1)  */

#define M_PSP_READ_CSR(reg) (	{ unsigned long __tmp; \
  asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
  __tmp; })
/* Use two-step definition in order to read Product-Specific (PS) CSR */
#define M_PSP_READ_PS_CSR(x)  M_PSP_READ_CSR_INTERMEDIATE(x)
#define M_PSP_READ_CSR_INTERMEDIATE(y) M_PSP_READ_CSR(y)
/*******************************************************/

#define M_PSP_WRITE_CSR(reg, val) ({ \
  if (__builtin_constant_p(val) && (unsigned long)(val) < 32) \
    asm volatile ("csrw " #reg ", %0" :: "i"(val)); \
  else \
    asm volatile ("csrw " #reg ", %0" :: "r"(val)); })
/* Use two-step definition in order to write Product-Specific (PS) CSR */
#define M_PSP_WRITE_PS_CSR(x, val)  M_PSP_WRITE_CSR_INTERMEDIATE(x, val)
#define M_PSP_WRITE_CSR_INTERMEDIATE(y, val) M_PSP_WRITE_CSR(y, val)
/*******************************************************/


#define M_PSP_SWAP_CSR(reg, val) ({ unsigned long __tmp; \
  if (__builtin_constant_p(val) && (unsigned long)(val) < 32) \
    asm volatile ("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "i"(val)); \
  else \
    asm volatile ("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "r"(val)); \
  __tmp; })
/* Use two-step definition in order to swap Product-Specific (PS) CSR */
#define M_PSP_SWAP_PS_CSR(x, val)  M_PSP_SWAP_CSR_INTERMEDIATE(x, val)
#define M_PSP_SWAP_CSR_INTERMEDIATE(y, val) M_PSP_SWAP_CSR(y, val)
/*******************************************************/


#define M_PSP_SET_CSR(reg, bit) ({ unsigned long __tmp; \
  if (__builtin_constant_p(bit) && (unsigned long)(bit) < 32) \
    asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "i"(bit)); \
  else \
    asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "r"(bit)); \
  __tmp; })
/* Use two-step definition in order to set Product-Specific (PS) CSR */
#define M_PSP_SET_PS_CSR(x, bit)  M_PSP_SET_CSR_INTERMEDIATE(x, bit)
#define M_PSP_SET_CSR_INTERMEDIATE(y, bit) M_PSP_SET_CSR(y, bit)
/*******************************************************/

#define M_PSP_CLEAR_CSR(reg, bit) ({ unsigned long __tmp; \
  if (__builtin_constant_p(bit) && (unsigned long)(bit) < 32) \
    asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "i"(bit)); \
  else \
    asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "r"(bit)); \
  __tmp; })
/* Use two-step definition in order to clear Product-Specific (PS) CSR */
#define M_PSP_CLEAR_PS_CSR(x, bit)  M_PSP_CLEAR_CSR_INTERMEDIATE(x, bit)
#define M_PSP_CLEAR_CSR_INTERMEDIATE(y, bit) M_PSP_CLEAR_CSR(y, bit)
/*******************************************************/

#define M_PSP_DISABLE_INTERRUPTS()	asm volatile( "csrc mstatus, 8" )
#define M_PSP_ENABLE_INTERRUPTS()	asm volatile( "csrs mstatus, 8" )
#define M_PSP_YIELD()               asm volatile( "ecall" )
#define M_PSP_NOP()                 asm volatile ("nop")
#define M_PSP_MEMORY_BARRIER()      asm volatile( "" ::: "memory" )



#endif /* __PSP_MACRO_H__ */
