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
* @file   psp_int_vect_eh2.h
* @author Nati Rapaport
* @date   12.07.2020
* @brief  Define the interrupts vector tables of hart0 and hart1 in SweRV EH2
*/
#ifndef __PSP_INT_VECT_EH2_H__
#define __PSP_INT_VECT_EH2_H__

/**
* include files
*/

/**
* macros
*/

/**
* types
*/

/**
* definitions
*/
#ifdef M_PSP_VECT_TABLE
  #undef M_PSP_VECT_TABLE
  /* Interrupt vector table */
  /* In SweRV EH2 with single HW thread - only psp_vect_table_hart0 is used */
  #define M_PSP_VECT_TABLE psp_vect_table_hart0
#endif
/* In SweRV EH2 with 2 HW thread - both psp_vect_table_hart0 psp_vect_table_hart1 are used */
#define M_PSP_VECT_TABLE_HART0 psp_vect_table_hart0
#define M_PSP_VECT_TABLE_HART1 psp_vect_table_hart1

/**
* local prototypes
*/

/**
* external prototypes
*/
void psp_vect_table_hart0(void);
void psp_vect_table_hart1(void);

/**
* global variables
*/

/**
* APIs
*/

#endif /* __PSP_INT_VECT_EH2_H__ */
