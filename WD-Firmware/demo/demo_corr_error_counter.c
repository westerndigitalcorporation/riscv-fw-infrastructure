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
* @file   demo_corr_error_counter.c
* @author Nati Rapaport
* @date   25.03.2020
* @brief  Demo application for correctable error counter interrupt (using SweRVolf FPGA for SweRV EH1)
*
* *******************************************************************************************************************
* NOTE: Currently this test is not running as part of the tests package. This test cannot (and hence not intended to)
*       run automatically, but a manual activations are needed. Please see the explanations inside demoStart function
*       herein.
* *******************************************************************************************************************
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

/**
* global variables
*/

/**
* functions
*/


/**
 * @brief - Correctable-Error-Counter ISR
 *
 * This ISR does nothing but break. See the explanation about this test at demoStart function.
 * breaking here means that correctable-error-counter ISR did happen == success
 *
 */
void demoCorrErrIsr(void)
{
  M_PSP_EBREAK();
}

/**
 * demoStart - startup point of the demo application. called from main function.
 *
 *********************************************************************************************************************************************
 * >> Explanation about test run: There is no way to create a bit-flip scenario by the code. That means this test cannot and hence is not
 *                                intended to run automatically.
 * This is the way this test is to be done:
 * 1) Automatic part of the test - demoStart is running. It installs the vector table, register the correctable-error-counter ISR and set a threshold
 *    of '1' to the i-cache correctable-error counter. That means - after 1 correctable-error (bit-flip) case in i-cache memory the interrupt is raised.
 *    Then demoStart go to infinite loop.
 *
 * 2) Manual part of the test - In order to create a correctable error (bit-flip) in the i-cache, the user is required to activate dicawics, dicad0,
 *    dicad1 and dicago CSRs according the instructions in EH1 PRM. When a correctable error (bit-flip) is created, the expectation is that the
 *    correctable-error interrupt will happen and the registered ISR will be called. All ISR does is to break, so a success of the test is to break
 *    at the ISR rather than looping infinitely in demoStart function.
*********************************************************************************************************************************************/
void demoStart(void)
{
  /* Register interrupt vector */
  pspMachineInterruptsSetVecTableAddress(&M_PSP_VECT_TABLE);

   /* Register correctable-error-counter ISR*/
   pspMachineInterruptsRegisterIsr(demoCorrErrIsr,E_MACHINE_CORRECTABLE_ERROR_CAUSE);

   /* Set correctable-error-counter threshold of 1 (== after 1 correctable-error (bit-flip), an interrupt will be raised) */
   pspMachineCorErrCntSetThreshold(E_ICACHE_CORR_ERR_COUNTER ,1);

   while(1);
}
