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
* @file   psp_int_vect.h
* @author Nati Rapaport
* @date   12.07.2020
* @brief  Define the interrupts vector table
*/
#ifndef __PSP_INT_VECT_H__
#define __PSP_INT_VECT_H__

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
#define M_PSP_VECT_TABLE psp_vect_table

/**
* local prototypes
*/

/**
* external prototypes
*/
void psp_vect_table(void);

/**
* global variables
*/

/**
* APIs
*/

#endif /* __PSP_INT_VECT_H__ */
