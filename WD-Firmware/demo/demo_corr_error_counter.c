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
* @file   demo_corr_error_counter.c
* @author Nati Rapaport
* @date   25.03.2020
* @brief  Demo application for correctable error counter interrupt (using SweRVolf FPGA for SweRV EH1)
*/

/**
* include files
*/
#include "psp_api.h"

/**
* definitions
*/

/**
* macros
*/

/**
* types
*/

/**
* local prototypes
*/

/**
* external prototypes
*/
/* General vector table */
extern void psp_vect_table(void);

/**
* global variables
*/

/**
* functions
*/


void demoCorrErrIsr(void)
{
	M_PSP_EBREAK();
}

/**
 * demoStart - startup point of the demo application. called from main function.
 *
 */
void demoStart(void)
{
   /* Register interrupt vector */
   pspInterruptsSetVectorTableAddress(&psp_vect_table);

   /* Register correctable-error-counter ISR*/
   pspRegisterInterruptHandler(demoCorrErrIsr,E_MACHINE_CORRECTABLE_ERROR_CAUSE);

   /* Set correctable-error-counter threshold */
   pspCorErrCntSetThreshold(E_ICACHE_CORR_ERR_COUNTER ,1);

   while(1);
}
