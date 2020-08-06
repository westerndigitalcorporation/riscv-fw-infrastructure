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
* @file   psp_macros_eh2.h
* @author Nati Rapaport
* @date   08.07.2020
* @brief  Macros for Swerv EH2 - information and functionalities of core and harts (HW threads)
* 
*/
#ifndef  __PSP_MACROS_EH2_H__
#define  __PSP_MACROS_EH2_H__

/**
* include files
*/

/**
* definitions
*/
#define D_PSP_HART1_START_BIT 1

/**
* macros
*/
/* Redefine M_PSP_GET_CORE_ID for EH2 (per EH2 PRM redefinition of MHARTID CSR) */
#ifdef M_PSP_GET_CORE_ID
  #undef M_PSP_GET_CORE_ID
  #define M_PSP_GET_CORE_ID()  (M_PSP_READ_CSR(D_PSP_MHARTID_NUM) >> D_PSP_MHARTID_CORE_ID_SHIFT)
#endif

#define M_PSP_GET_HART_ID()       (M_PSP_READ_CSR(D_PSP_MHARTID_NUM) & D_PSP_MHARTID_HART_ID_MASK)
#define M_PSP_GET_NUM_OF_HARTS()  (M_PSP_READ_CSR(D_PSP_MHARTNUM_NUM) & D_PSP_MHARTNUM_TOTAL_MASK)
#define M_PSP_START_HART1()       (M_PSP_SET_CSR(D_PSP_MHARTSTART_NUM, (1 << D_PSP_HART1_START_BIT))

/**
* types
*/
typedef enum pspHartNum
{
  E_HART0     = 0,
  E_HART1     = 1,
  E_LAST_HART,
} ePspHartNum_t;

/**
* local prototypes
*/

/**
* external prototypes
*/

/**
* global variables
*/

/**
* APIs
*/

#endif /* __PSP_MACROS_EH2_H__ */
