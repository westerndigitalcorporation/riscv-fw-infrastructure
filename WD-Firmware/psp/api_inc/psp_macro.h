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

#define PSP_SECTION __attribute__((section("PSP_SEC")))

#define M_PSP_READ_CSR(reg) (	{ unsigned long __tmp; \
  asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
  __tmp; })

#define M_PSP_WRITE_CSR(reg, val) ({ \
  if (__builtin_constant_p(val) && (unsigned long)(val) < 32) \
    asm volatile ("csrw " #reg ", %0" :: "i"(val)); \
  else \
    asm volatile ("csrw " #reg ", %0" :: "r"(val)); })

#endif /* __PSP_MACRO_H__ */
