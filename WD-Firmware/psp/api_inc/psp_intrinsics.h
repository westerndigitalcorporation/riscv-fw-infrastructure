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
* @file   psp_intrinsics.h
* @author Nati Rapaport
* @date   12.11.2019
* @brief  The psp intrinsics
* 
*/
#ifndef  __PSP_INTRINSICS_H__
#define  __PSP_INTRINSICS_H__

#if defined (__GNUC__) || defined (__clang__)

/**
* include files
*/

/**
* definitions
*/
#ifdef __cplusplus
   #define  D_PSP_EXTERNC  extern "C"
#else
   #define  D_PSP_EXTERNC
#endif


#define D_PSP_INLINE    static inline

/* The following CSRs are known to be intrinsics values in GCC and LLVM. Those are not instructions */
#define D_PSP_MISA    misa
#define D_PSP_MTVEC   mtvec
#define D_PSP_MSTATUS mstatus
#define D_PSP_MIE     mie
#define D_PSP_MIP     mip
#define D_PSP_MCAUSE  mcause
#define D_PSP_MEPC    mepc

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

/**
* global variables
*/

/**
* APIs
*/

#endif /* defined (__GNUC__) || defined (__clang__) */

#endif /* __PSP_INTRINSICS_H__ */
