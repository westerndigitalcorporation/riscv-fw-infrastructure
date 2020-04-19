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
* include files
*/
#include "psp_api.h"
#include "psp_macros.h"
#include "printf.h"
#include "demo_platform_al.h"


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
extern void psp_vect_table(void);

/**
* global variables
*/

/**
* functions
*/

/**
 * printToTerminal - loops using while and print to terminal to consume some cycles and performance.
 *
 */
void printToTerminal(void)
{
  u32_t i = 0;
  while(i <= 86)
  {
    printfNexys("Don't mind me just wasting your time! %d", i);
    i++;
  }
  /* making sure the print has been sent out before continuing with the program */
  M_PSP_INST_FENCEI();
}

/**
 * demoStart - startup point of the demo application. called from main function.
 *
 */
void demoStart(void)
{
  u32_t           uiIPC = 0,   uiTotalBranches = 0;
  u32_t volatile  uiCycle0,    uiCycle1;
  u32_t volatile  uiInstRet0,  uiInstRet1;
  u32_t volatile  uiBranch0,   uiBranch1;

  /* register trap handler */
  M_PSP_WRITE_CSR(D_PSP_MTVEC_NUM, &psp_vect_table);

  /* Setting event counter (0) for event branches take (26) */
  pspPerformanceCounterSet(D_PSP_COUNTER0, E_BRANCHES_TAKEN);

  /* Get counters before the loop */

  /* Get the cycle counter*/
  uiCycle0   = pspPerformanceCounterGet(D_PSP_CYCLE_COUNTER);

  /* Get the instruction retired counter */
  uiInstRet0 = pspPerformanceCounterGet(D_PSP_INSTRET_COUNTER);

  /* Get the branches taken counter */
  uiBranch0  = pspPerformanceCounterGet(D_PSP_COUNTER0);


  /* do something then get the counters values afterward */
  printToTerminal();


  /* Get counters after the loop */

  /* Get the cycle counter*/
  uiCycle1   = pspPerformanceCounterGet(D_PSP_CYCLE_COUNTER);

  /* Get the instruction retired counter */
  uiInstRet1 = pspPerformanceCounterGet(D_PSP_INSTRET_COUNTER);

  /* Get the branches taken counter */
  uiBranch1  = pspPerformanceCounterGet(D_PSP_COUNTER0);

  /* calculating deltas between counters */
  uiIPC           = (uiCycle1 - uiCycle0) / (uiInstRet1 - uiInstRet0);
  uiTotalBranches = uiBranch1 - uiBranch0;

  /* verify counters where read successfully */
  if (uiCycle0 == 0 || uiCycle1 == 0 || uiInstRet0 == 0 || uiInstRet1 == 0)
  {
    /* endless loop */
    M_ENDLESS_LOOP();
  }

   printfNexys("Total cycles %d\nTotal Instructions %d\nInstructions per cycle %d\nBranches per loop %d", \
               (uiCycle1 - uiCycle0),     \
               (uiInstRet1 - uiInstRet0), \
               uiIPC,                     \
               uiTotalBranches);
}
