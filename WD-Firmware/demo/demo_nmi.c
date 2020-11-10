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
* @file   demo_nmi.c
* @author Nati Rapaport
* @date   20.04.2020
* @brief  Demo application for NMI testing (using SweRVolf FPGA for SweRV EH1)
*/

/**
* include files
*/
#include "psp_api.h"
#include "bsp_timer.h"
#include "demo_platform_al.h"
#include "demo_utils.h"

/**
* definitions
*/
#define D_DEMO_DURATION_MSEC    500                            /* Create a delay of 0.5 second  */
#define D_NUM_OF_ITERATIONS_IN_DELAY_LOOP    D_CLOCK_RATE * 3 /* ~(3*Const) seconds of any Freq */
/**
* macros
*/

/**
* types
*/


/**
* local prototypes
*/
void demoNmiPinAssertionHandler(void); /* Pin-assertion-NMI handler for this test */

/**
* external prototypes
*/

/**
* global variables
*/
volatile u32_t g_uiReturnAddress;    /* Store the RA register contents */

/**
* functions
*/

/**
 * @brief -  demoStart - startup point of the demo application. called from main function.
 *
 *  Flow of the test:
 *  1.  Store RA register in a global parameter
 *  2.  Set address of pspMachineNmiHandlerSelector in nmi_vec
 *  3.  Register NMI handler for External-Pin-Assertion NMI
 *  4.  Set the timer to generate External-Pin-Assertion NMI upon expiration
 *  5.  Set 0.5 second duration in the timer
 *  6.  Let the timer to run (at its expiration - we expect to receive NMI)
 *  7.  Delay in a loop (10 seconds)
 *  8.  Upon NMI (after 0.5 second), the NMI handler is called
 *  9.  NMI handler restore RA register from the global parameter and return to 'main' - Test completed successfully
 *  10. If NMI did not occur, The 10 seconds loop expires and function breaks. Test failed.
 */
void demoStart(void)
{
  u32_t uiIterationCounter;

  M_DEMO_START_PRINT();

  /* Run this demo only if target is Swerv. Cannot run on Whisper */
  if (D_PSP_TRUE == demoIsSwervBoard())
  {
    /* ** Store RA register contents in a global parameter here **
     * This is required because the general NMI handler might do stack 'push' operations, hence changes the SP,
     * but it does not do equivalent 'pop' operations because it is not returned anywhere.
     * So, in order to be able to return from here to 'main', the RA register is stored here in a global parameter
     * and it is restored in the Pin-Asserted-NMI handler */
    asm volatile ("mv %0, ra" : "=r" (g_uiReturnAddress)  : );

    /* Register the initial NMI handler in nmi_vec register */
    pspMachineNmiSetVec(D_NMI_VEC_ADDRESSS, pspMachineNmiHandlerSelector);

    /* Register External-Pin-Asserted NMI handler function */
    pspMachineNmiRegisterHandler(demoNmiPinAssertionHandler, D_PSP_NMI_EXT_PIN_ASSERTION);

    /* Rout timer to NMI pin assertion - i.e. when the timer expires, an NMI will be asserted */
    bspRoutTimer(E_TIMER_TO_NMI);

    /* Initialize Timer (at its expiration, it will create an NMI) */
    bspSetTimerDurationMsec(D_DEMO_DURATION_MSEC);

    /* Enable the timer to start running */
    bspStartTimer();
  
    /* Delay here in a loop */
    for(uiIterationCounter=0; uiIterationCounter<D_NUM_OF_ITERATIONS_IN_DELAY_LOOP; uiIterationCounter++)
    {
      M_PSP_NOP();
      M_PSP_NOP();
    }
    /* Arriving here means test failed, as the NMI should have been occurred already */
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }
  else
  {
    /* whisper */
    printfNexys("This demo can't run under whisper");
    M_DEMO_END_PRINT();
  }
}


/**
 * @brief -  demoNmiPinAssertionHandler - NMI handler
 *
 */
void demoNmiPinAssertionHandler()
{
  /* Arriving here means the test passed successfully */

  /* do the print before restoring ra since compiler will optimize the return (leaf)*/
  M_DEMO_END_PRINT();

  /* Restore RA from the global parameter here */
  asm volatile ("mv ra, %0" : : "r" (g_uiReturnAddress) );

  /* From here we return directly back to 'main', as we restored the RA register here */
  return;
}


