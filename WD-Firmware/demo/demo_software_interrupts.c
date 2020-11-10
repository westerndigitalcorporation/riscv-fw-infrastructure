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
#include "demo_platform_al.h"
#include "demo_utils.h"



/**
* definitions
*/
#define D_DEMO_NUM_OF_SW_INTERRUPTS 15

/**
* external prototypes
*/
u32_t* pSwIntCtrl = (u32_t*)D_SW_INT_ADDRESS;

/**
* global variables
*/
u32_t g_uiSwIsrCount1 = 0;
u32_t g_uiSwIsrCount2 = 0;
u32_t g_uiSyncPoint   = 0;

/**
* macros
*/
/* Macros to trigger and clear software interrupt (relevant to Whisper only) */
#define M_DEMO_TRIGGER_SW_INTERRUPT()  *pSwIntCtrl = 1;
#define M_DEMO_CLEAR_SW_INTERRUPT()    *pSwIntCtrl = 0;

#define M_DEMO_SET_SYNC_POINT()   g_uiSyncPoint = 1;
#define M_DEMO_CLEAR_SYNC_POINT() g_uiSyncPoint = 0;
#define M_DEMO_IS_SYNC_POINT_SET() (g_uiSyncPoint != 0)
/**
* types
*/

/**
* local prototypes
*/

/**
* functions
*/
void demoSoftwareIsr1()
{
  g_uiSwIsrCount1++;
  M_DEMO_SET_SYNC_POINT();
  M_DEMO_CLEAR_SW_INTERRUPT();
}

void demoSoftwareIsr2()
{
  g_uiSwIsrCount2++;
  M_DEMO_SET_SYNC_POINT();
  M_DEMO_CLEAR_SW_INTERRUPT();
}
/**
 * demoSoftwareInterrupts - demo for software interrupt setup and ISRs
 *                          - part1: enable software interrupts and verify they occur
 *                          - part2: disable software interrupts and verify they do not occur
 */
void demoSoftwareInterrupts(void)
{
  u32_t uiInterruptsStatus;
  u32_t uiNumberOfSwInterrupts;

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiInterruptsStatus);

  /* Register software ISR number 1 */
  pspMachineInterruptsRegisterIsr(demoSoftwareIsr1, E_MACHINE_SOFTWARE_CAUSE);

  /* Enable software interrupts */
  pspMachineInterruptsEnableIntNumber(D_PSP_INTERRUPTS_MACHINE_SW);
  /* Enable interrupts*/
  pspMachineInterruptsEnable();

  /* Create a series of software interrupts */
  for(uiNumberOfSwInterrupts = 0; uiNumberOfSwInterrupts < D_DEMO_NUM_OF_SW_INTERRUPTS; uiNumberOfSwInterrupts++)
  {
    M_DEMO_TRIGGER_SW_INTERRUPT();
    while(M_DEMO_IS_SYNC_POINT_SET())
    {
      M_DEMO_CLEAR_SYNC_POINT();
    };
  }

  /* Check that expected number of software ISRs have been called */
  if (D_DEMO_NUM_OF_SW_INTERRUPTS != g_uiSwIsrCount1)
  {
    M_DEMO_ENDLESS_LOOP();
  }

  /* Disable software interrupts */
  pspMachineInterruptsDisableIntNumber(D_PSP_INTERRUPTS_MACHINE_SW);

  /* Register software ISR number 2 */
  pspMachineInterruptsRegisterIsr(demoSoftwareIsr2, E_MACHINE_SOFTWARE_CAUSE);

  /* Create a series of software interrupts */
  for(uiNumberOfSwInterrupts = 0; uiNumberOfSwInterrupts < D_DEMO_NUM_OF_SW_INTERRUPTS; uiNumberOfSwInterrupts++)
  {
    M_DEMO_TRIGGER_SW_INTERRUPT();
    while(M_DEMO_IS_SYNC_POINT_SET())
    {
      M_DEMO_CLEAR_SYNC_POINT();
    };
  }

  /* Check that no software ISRs have been called */
  if (g_uiSwIsrCount2 > 0)
  {
    M_DEMO_ENDLESS_LOOP();
  }
}


/**
 * demoStart - startup point of the demo application. called from main function.
 *
 */
void demoStart(void)
{
  M_DEMO_START_PRINT();

  /* Register interrupt vector */
  pspMachineInterruptsSetVecTableAddress(&M_PSP_VECT_TABLE);

  /* Run this demo only if target is Whisper. Cannot run on SweRV */
  if (D_PSP_FALSE == demoIsSwervBoard())
  {
    /* Software interrupts demo function */
    demoSoftwareInterrupts();
  }
  else
  {
    /* SweRV */
    printfNexys("This demo is currently not supported in SweRV FPGA Board");
  }

  M_DEMO_END_PRINT();
}
