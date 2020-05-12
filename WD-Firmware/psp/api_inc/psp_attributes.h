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
* @file   psp_attributes.h
* @author Nati Rapaport
* @date   26.08.2019
* @brief  The file defines attribute directives that relevant to our PSP
* 
*/
#ifndef  __PSP_ATTRIBUTES_H__
#define  __PSP_ATTRIBUTES_H__

#if defined (__GNUC__) || defined (__clang__)

/**
* include files
*/

/**
* definitions
*/

#define D_PSP_NO_INLINE                                __attribute__((noinline))
#define D_PSP_ALWAYS_INLINE                            D_PSP_INLINE __attribute__((always_inline))
#define D_PSP_ALIGNED(x)                               __attribute__ ((aligned(x)))
#define D_PSP_WEAK                                     __attribute__(( weak ))
#define D_PSP_TEXT_SECTION                             __attribute__((section("PSP_TEXT_SEC")))
#define D_PSP_DATA_SECTION                             __attribute__((section("PSP_DATA_SEC")))
#define D_PSP_NO_RETURN                                __attribute__((noreturn))
#define D_PSP_USED                                     __attribute__((used))

#define D_PSP_CREATE_ATTR(name, val)                   __attribute__((section(#name),aligned(val)))
#define D_PSP_GENERAL_DATA_SECTION(name, align_avl)    D_PSP_CREATE_ATTR( (#name), align_avl )

/**
* macros
*/

/**
* types
*/

#endif /* defined (__GNUC__) || defined (__clang__) */

#endif /* __PSP_ATTRIBUTES_H__ */
