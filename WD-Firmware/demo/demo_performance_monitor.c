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
#define D_NUMBER_OF_TIMER_INTERRUPTS  15

/**
* macros
*/

/**
* types
*/

/**
* local prototypes
*/
void demoTimerIsr(void);

/**
* external prototypes
*/

/**
* global variables
*/
u32_t g_uiNumberOfTimerInterrupts;
u64_t g_uiTimerCounter0, g_uiTimerCounter1;
u64_t g_uiCycle0, g_uiCycle1;
u64_t g_uiInstRet0, g_uiInstRet1;
u64_t g_uiTimerInt0, g_uiTimerInt1;
u64_t g_uiBranch0, g_uiBranch1;
u64_t g_uiFenci0, g_uiFenci1;
u64_t g_uiCsrRead0, g_uiCsrRead1;

/**
* functions
*/

/**
 * @brief - Performance-Monitors preparations before the test
 *
 */
void demoPrepareEventCounters()
{
  /* Register machine-timer ISR */
  pspRegisterInterruptHandler(demoTimerIsr, E_MACHINE_TIMER_CAUSE);
  /* Setting event counter (0) for event "timer-interrupts-taken" (38) */
  pspPerformanceCounterSet(D_PSP_COUNTER0, D_TIMER_INTERRUPTS_TAKEN);
  /* Setting event counter (1) for event "branches-taken" (26) */
  pspPerformanceCounterSet(D_PSP_COUNTER1, D_BRANCHES_TAKEN);
  /* Setting event counter (2) for event "fence.i" (22) */
  pspPerformanceCounterSet(D_PSP_COUNTER2, D_FENCE_I);
  /* Setting event counter (3) for event "CSR read" (16) */
  pspPerformanceCounterSet(D_PSP_COUNTER3, D_CSR_READ);
}

/**
 * @brief - Get all the timers we use in the test before the activity
 *
 */
void demoGetCountersBeforeActivity(void)
{
  /* Get the time counter (machine timer-counter) */
  g_uiTimerCounter0 = pspPerformanceCounterGet(D_PSP_TIME_COUNTER);
  /* Get the cycle counter*/
  g_uiCycle0 = pspPerformanceCounterGet(D_PSP_CYCLE_COUNTER);
  /* Get the instruction retired counter */
  g_uiInstRet0 = pspPerformanceCounterGet(D_PSP_INSTRET_COUNTER);
  /* Get the timer-interrupts taken counter */
  g_uiTimerInt0 = pspPerformanceCounterGet(D_PSP_COUNTER0);
  /* Get the branches taken counter */
  g_uiBranch0 = pspPerformanceCounterGet(D_PSP_COUNTER1);
  /* Get the fence.i taken counter */
  g_uiFenci0 = pspPerformanceCounterGet(D_PSP_COUNTER2);
  /* Get the csr-read counter */
  g_uiCsrRead0 = pspPerformanceCounterGet(D_PSP_COUNTER3);
}

/**
 * @brief - Get all the timers we use in the test after the activity
 *
 */
void demoGetCountersAfterActivity(void)
{
  /* Get the time counter*/
  g_uiTimerCounter1 = pspPerformanceCounterGet(D_PSP_TIME_COUNTER);
  /* Get the cycle counter*/
  g_uiCycle1 = pspPerformanceCounterGet(D_PSP_CYCLE_COUNTER);
  /* Get the instruction retired counter */
  g_uiInstRet1 = pspPerformanceCounterGet(D_PSP_INSTRET_COUNTER);
  /* Get the timer-interrupts taken counter */
  g_uiTimerInt1 = pspPerformanceCounterGet(D_PSP_COUNTER0);
  /* Get the branches taken counter */
  g_uiBranch1 = pspPerformanceCounterGet(D_PSP_COUNTER1);
  /* Get the fence.i taken counter */
  g_uiFenci1 = pspPerformanceCounterGet(D_PSP_COUNTER2);
  /* Get the csr-read counter */
  g_uiCsrRead1 = pspPerformanceCounterGet(D_PSP_COUNTER3);
}

/**
 * @brief - Timer ISR
 *
 */
void demoTimerIsr(void)
{
  /* Disable Machine-Timer interrupt */
  pspDisableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_TIMER);

  /* Increment the number of timer ISR counter */
  g_uiNumberOfTimerInterrupts++;

  /* Setup another timer interrupt, until a defined number of timer interrupts has already happened */
  if(D_NUMBER_OF_TIMER_INTERRUPTS > g_uiNumberOfTimerInterrupts)
  {
    /* Enable timer interrupt */
    pspEnableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_TIMER);

    pspTimerCounterSetupAndRun(D_PSP_MACHINE_TIMER, M_DEMO_MSEC_TO_CYCLES(5));
  }
}

/**
 * @brief - do some activities in order to test performance-monitor counters functionality
 *
 */
void demoDummyActivityFunction(void)
{
  u32_t i = 0;
  while(i <= 86)
  {
    demoOutputMsg("Don't mind me just wasting your time! %d", i);
    i++;
    M_PSP_READ_CSR(D_PSP_MVENDOID_NUM);
  }
  /* making sure the print has been sent out before continuing with the program */
  M_PSP_INST_FENCEI();
}

/**
 * @brief - Disable performance-monitor and make sure that performance-monitor counters do not function
 *          (note that in SweRV EH1 - cycles and instructions-retired counters keep working because there's no way to stop
 *           these counters. In EL2/EH2 cycles and instructions-retired could be stopped)
 *
 */
void demoPerfMonitorDisableAll(void)
{
  u32_t uiInterruptsStatus;

  /* Disable interrupts */
  pspInterruptsDisable(&uiInterruptsStatus);

  /* Disable Performance_monitor counters */
  pspPerformanceMonitorDisableAll();

  /* pre-test preparations */
  demoPrepareEventCounters();

  /* Get counters before the activity */
  demoGetCountersBeforeActivity();

  /* Setup and enable machine-timer interrupt */
  pspTimerCounterSetupAndRun(D_PSP_MACHINE_TIMER, M_DEMO_MSEC_TO_CYCLES(5));
  pspEnableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_TIMER);
  pspInterruptsEnable();

  /* Do something then get the counter values afterwards */
  demoDummyActivityFunction();

  /* Disable machine-timer interrupts */
  pspDisableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_TIMER);

  /* Get counters after the activity */
  demoGetCountersAfterActivity();

  /* Verify Performance-Monitor counters 3..6 are not functioning */
  if ((g_uiTimerInt0 != g_uiTimerInt1) || (g_uiBranch0 != g_uiBranch1) || (g_uiFenci0 != g_uiFenci1) || (g_uiCsrRead0 != g_uiCsrRead1))
  {
    M_DEMO_ENDLESS_LOOP();
  }

  demoOutputMsg("Part1:\nTotal timer cycles: %d\nTotal cycles: %d\nTotal Retired-instructions: %d\n", \
               (g_uiTimerCounter1 - g_uiTimerCounter0), (g_uiCycle1 - g_uiCycle0), (g_uiInstRet1 - g_uiInstRet0));
}

/**
 * @brief - Enable performance-monitor and make sure that performance-monitor counters and time, cycles and instructions-retired
 *          counters are functioning
 *
 */
void demoPerfMonitorEnableAll(void)
{
  u32_t uiIPC = 0; /* Instructions-per-cycle */
  u32_t uiTimerCounter, uiCycles, uiInstRet, uiBranches, uiFenci, uiCsrReads ;

  /* Disable Performance_monitor counters */
  pspPerformanceMonitorDisableAll();

  /* pre-test preparations */
  demoPrepareEventCounters();

  /* Get counters before the activity */
  demoGetCountersBeforeActivity();

  /* Enable Performance_monitor counters */
  pspPerformanceMonitorEnableAll();

  /* Setup and enable machine-timer interrupt */
  pspTimerCounterSetupAndRun(D_PSP_MACHINE_TIMER, M_DEMO_MSEC_TO_CYCLES(5));
  pspEnableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_TIMER);
  pspInterruptsEnable();

  /* Do something then get the counter values afterwards */
  demoDummyActivityFunction();

  /* Disable machine-timer interrupts */
  pspDisableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_TIMER);

  /* Get counters after the activity */
  demoGetCountersAfterActivity();

  /* Not all performance-monitor features are supported in Whisper */
  if (D_PSP_TRUE == demoIsSwervBoard())
  {
    /* Verify time, cycles and retired-instructions counters are functioning */
    if ((g_uiTimerCounter0 == g_uiTimerCounter1) || (g_uiCycle0 == g_uiCycle1) || (g_uiInstRet0 == g_uiInstRet1))
    {
      M_DEMO_ENDLESS_LOOP();
    }
  }
  else
  {
    /* In Whisper - verify cycles counter is functioning */
    if (g_uiCycle0 == g_uiCycle1)
    {
      M_DEMO_ENDLESS_LOOP();
    }
  }


  /* Verify Performance-Monitor counters 3..6 are functioning */
  if ((g_uiTimerInt0 == g_uiTimerInt1) || (g_uiBranch0 == g_uiBranch1) || (g_uiFenci0 == g_uiFenci1) || (g_uiCsrRead0 == g_uiCsrRead1))
  {
    M_DEMO_ENDLESS_LOOP();
  }

  /* Verify that Timer-interrupts counter counted correctly number of ISRs */
  if(g_uiNumberOfTimerInterrupts != (g_uiTimerInt1 - g_uiTimerInt0))
  {
    M_DEMO_ENDLESS_LOOP();
  }

  uiTimerCounter = g_uiTimerCounter1 - g_uiTimerCounter0;
  uiCycles = g_uiCycle1 - g_uiCycle0;
  uiInstRet = g_uiInstRet1 - g_uiInstRet0;
  uiBranches = g_uiBranch1 - g_uiBranch0;
  uiFenci = g_uiFenci1 - g_uiFenci0;
  uiCsrReads = g_uiCsrRead1 - g_uiCsrRead0 ;

  /* Divide number-of-cycles with number-of-instructionto get IPC */
  uiIPC = uiCycles/uiInstRet;

  /* In Swerv - Output the monitored parameters and calculated instructions-per-cycle */
  if (D_PSP_TRUE == demoIsSwervBoard())
  {
    demoOutputMsg("Part2:\nTotal timer cycles: %d\nTotal cycles: %d\nTotal Retired-instructions: %d\nInstructions-per-cycle: %d\n", \
                 uiTimerCounter, uiCycles, uiInstRet, uiIPC);
    demoOutputMsg("Number of Timer ISRs: %d\nNumber of branches: %d\nNumber of FENCE.I instructions: %d\nNumber of CSR reads: %d\n", \
                  g_uiNumberOfTimerInterrupts, uiBranches, uiFenci, uiCsrReads);
  }
  /* In Whisper - Output the monitored parameters (the supported ones)  */
  else
  {
    demoOutputMsg("Part2:Total cycles: %d\n",uiCycles);
    demoOutputMsg("Number of Timer ISRs: %d\nNumber of branches: %d\nNumber of FENCE.I instructions: %d\nNumber of CSR reads: %d\n", \
                  g_uiNumberOfTimerInterrupts, uiBranches, uiFenci, uiCsrReads);
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
  pspInterruptsSetVectorTableAddress(&M_PSP_VECT_TABLE);

  /* Part1 - disable all performance-monitor counters and verify that relevant counters stopped counting */
  demoPerfMonitorDisableAll();

  /* Zero timer-interrupts counter before next part */
  g_uiNumberOfTimerInterrupts = 0;

  /* Part2 - enable all performance-monitor counters and verify that relevant counters count up */
  demoPerfMonitorEnableAll();

  M_DEMO_END_PRINT();
}
