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
 * printToTerminal - loops using while and print to terminal to consume some cycles and performance.
 *
 */
void printToTerminal(void)
{
  u32_t i = 0;
  while(1)
  {
    printfNexys("Don't mind me just wasting your time! %d", i);
    if (i >= 86)
      break;
    i++;
  }
  M_PSP_INST_FENCEI();
}

/**
 * demoStart - startup point of the demo application. called from main function.
 *
 */
void demoStart(void)
{
    u32_t           udIPC = 0,   udTotalBranches = 0;
    u32_t volatile  udCycle0,    udCycle1;
    u32_t volatile  udInstRet0,  udInstRet1;
    u32_t volatile  udBranch0,   udBranch1;


   /* Setting event counter (0) for event branches take (26) */
   pspPerformanceCounterSet(D_PSP_COUNTER0, E_BRANCHES_TAKEN);

   /* Get counters before the loop */

   /* Get the cycle counter*/
   udCycle0   = pspPerformanceCounterGet(D_PSP_CYCLE_COUNTER);

   /* Get the instruction retired counter */
   udInstRet0 = pspPerformanceCounterGet(D_PSP_INSTRET_COUNTER);

   /* Get the branches taken counter */
   udBranch0  = pspPerformanceCounterGet(D_PSP_COUNTER0);


   /* do something then get the counters values afterward */
   printToTerminal();


   /* Get counters after the loop */

   /* Get the cycle counter*/
   udCycle1   = pspPerformanceCounterGet(D_PSP_CYCLE_COUNTER);

   /* Get the instruction retired counter */
   udInstRet1 = pspPerformanceCounterGet(D_PSP_INSTRET_COUNTER);

   /* Get the branches taken counter */
   udBranch1  = pspPerformanceCounterGet(D_PSP_COUNTER0);

   /* calculating deltas between counters */
   udIPC           = (udCycle1 - udCycle0) / (udInstRet1 - udInstRet0);
   udTotalBranches = udBranch1 - udBranch0;

   printfNexys("Total cycles %d\nTotal Instructions %d\nInstructions per cycle %d\nBranches per loop %d", \
               (udCycle1 - udCycle0),     \
               (udInstRet1 - udInstRet0), \
               udIPC,                     \
               udTotalBranches);
}
