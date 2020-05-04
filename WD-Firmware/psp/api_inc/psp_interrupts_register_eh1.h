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
* @file   psp_interrupts_register_eh1.h
* @author Nati Rapaport
* @date   14.01.2020
* @brief  The file supplies specific EH1 information for registration of interrupts service routines on EH1 core.
*/
#ifndef __PSP_INTERRUPTS_REGISTER_EH1_H__
#define __PSP_INTERRUPTS_REGISTER_EH1_H__

/**
* include files
*/

/**
* macros
*/

/**
* types
*/

/* */
typedef enum pspInterruptCauseEh1
{
   E_MACHINE_INTERNAL_TIMER1_CAUSE   = 28,
   E_MACHINE_INTERNAL_TIMER0_CAUSE   = 29,
   E_MACHINE_CORRECTABLE_ERROR_CAUSE = 30,
   E_LAST_EH1_CAUSE
} pspInterruptCauseEh1_t;

/**
* definitions
*/
#define D_PSP_FIRST_EH1_INT_CAUSE = E_MACHINE_INTERNAL_TIMER1_CAUSE;

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

#endif /* __PSP_INTERRUPTS_REGISTER_EH1_H__ */
