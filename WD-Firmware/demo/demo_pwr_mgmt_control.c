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
* @file   demo_pwr_mgmt_control.c
* @author Alexander Dvoskin
* @date   26.03.2020
* @brief  Demo application for power-management unit. Contains tests that does the following:
*         (1) Set Core to sleep mode ('Halt', per EH1 PRM definition) and wake it up with MTIME interrupt and with EXTERNAL interrupt.
*         (2) Stall the core for a given period ('Pause', per EH1 PRM definition) and resumes it with a count-down, with MTIME interrupt and with EXTERNAL interrupt.
*/

/**
* include files
*/
#include "psp_api.h"
#include "demo_platform_al.h"
#include "demo_utils.h"
#include "bsp_external_interrupts.h"
#include "bsp_timer.h"

/**
* definitions
*/
#define D_BEFORE_SLEEP    (0)
#define D_AFTER_SLEEP     (1)
#define D_IN_MTIMER_ISR  (2)
#define D_IN_EXT_INT_ISR (3)
/* Expected test results for sleep until MTIME interrupt */
#define D_MTIMER_WAKEUP_TEST_RESULT    (M_PSP_BIT_MASK(D_BEFORE_SLEEP) | M_PSP_BIT_MASK(D_AFTER_SLEEP) | M_PSP_BIT_MASK(D_IN_MTIMER_ISR))
/* Expected test results for sleep until EXTERNAL interrupt */
#define D_EXT_INT_WAKEUP_TEST_RESULT   (M_PSP_BIT_MASK(D_BEFORE_SLEEP) | M_PSP_BIT_MASK(D_AFTER_SLEEP) | M_PSP_BIT_MASK(D_IN_EXT_INT_ISR))

#define D_BEFORE_STALL  (0)
#define D_AFTER_STALL   (1)
/* Expected test results for stall until count down complete */
#define D_STALL_COUNTDOWN_TEST_RESULT (M_PSP_BIT_MASK(D_BEFORE_STALL) | M_PSP_BIT_MASK(D_AFTER_STALL))
/* Expected test results for stall until MTIME interrupt */
#define D_STALL_MTIME_INT_TEST_RESULT (M_PSP_BIT_MASK(D_BEFORE_STALL) | M_PSP_BIT_MASK(D_AFTER_STALL) | M_PSP_BIT_MASK(D_IN_MTIMER_ISR))
/* Expected test results for stall until EXTERNAL interrupt */
#define D_STALL_EXT_INT_TEST_RESULT   (M_PSP_BIT_MASK(D_BEFORE_STALL) | M_PSP_BIT_MASK(D_AFTER_STALL) | M_PSP_BIT_MASK(D_IN_EXT_INT_ISR))

/* Sleep and Stall duration for the tests */
#define D_SLEEP_TIME          10
#define D_STALL_TIME          10
#define D_LONG_STALL_TIME     20 * D_STALL_TIME

/* These definitions enables us to easily switch between IRQ3 and IRQ4 when using external interrupts as wakeup from halt or pause */
#define D_DEMO_IRQ           D_BSP_IRQ_3
#define D_DEMO_TIMER_TO_IRQ  E_TIMER_TO_IRQ3

/**
* macros
*/
/* Convert mSec duration to number of clock cycles */
#define M_DEMO_MSEC_TO_CYCLES(duration)   (duration * (D_CLOCK_RATE / D_PSP_MSEC))

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
volatile u32_t g_uiTestWayPoints;

/**
* functions
*/

/**
 * @brief - External interrupt ISR
 *
 */
void demoExternalInterruptIsr(void)
{
    /* Stop the generation of the specific external interrupt */
    bspClearExtInterrupt(D_DEMO_IRQ);

  /* Disable External-interrupts */
  pspDisableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_EXT);

  /* Mark that ISR visited */
  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_IN_EXT_INT_ISR);
}

/**
 * @brief - Initialize and setup external-interrupt
 *
 */
void demoSetupExternalInterrupts(void)
{
    /* Set Standard priority order */
  pspExtInterruptSetPriorityOrder(D_PSP_EXT_INT_STANDARD_PRIORITY);

    /* Set interrupts threshold to minimal level (== all interrupts should be served) */
  pspExtInterruptsSetThreshold(M_PSP_EXT_INT_THRESHOLD_UNMASK_ALL_VALUE);

  /* Set the nesting priority threshold to minimal level (== all interrupts should be served) */
  pspExtInterruptsSetNestingPriorityThreshold(M_PSP_EXT_INT_THRESHOLD_UNMASK_ALL_VALUE);

  /* Set Gateway Interrupt type (Level) */
  pspExtInterruptSetType(D_DEMO_IRQ, D_PSP_EXT_INT_LEVEL_TRIG_TYPE);

  /* Set gateway Polarity (Active high) */
  pspExtInterruptSetPolarity(D_DEMO_IRQ, D_PSP_EXT_INT_ACTIVE_HIGH);

  /* Set the priority level to highest */
  pspExtInterruptSetPriority(D_DEMO_IRQ, M_PSP_EXT_INT_PRIORITY_SET_TO_HIGHEST_VALUE);

  /* Enable IRQ3 interrupt in the PIC */
  pspExternalInterruptEnableNumber(D_DEMO_IRQ);

  /* Enable external interrupts */
  pspEnableInterruptNumberMachineLevel(E_MACHINE_EXTERNAL_CAUSE);
}

/**
 * @brief - Set core to Sleep (pmu/fw-halt) mode and wake it up with external interrupt
 *          In this test we use the SweRVolf FPGA timer to trigger external-interrupt
 *
 */
void demoSleepAndWakeupByExternalInterrupt(void)
{
  u64_t udTimeBeforeSleep;
  u64_t udTimeAfterSleep;

  /* Zero the test results variable */
  g_uiTestWayPoints = 0;

  /* register external interrupt handler */
  pspExternalInterruptRegisterISR(D_DEMO_IRQ, demoExternalInterruptIsr, 0);

  /* Rout SweRVolf FPGA timer to IRQ3 assertion - i.e. when the timer expires, IRQ3 external interrupt is asserted */
  bspRoutTimer(D_DEMO_TIMER_TO_IRQ);

  /* Set SweRVolf FPGA Timer duration (upon expiration, it will trigger an external interrupt) */
  bspSetTimerDurationMsec(D_SLEEP_TIME);

  /* Setup external interrupts */
    demoSetupExternalInterrupts();

  /* Enable all machine level interrupts */
    pspInterruptsEnable();

  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_BEFORE_SLEEP);

  udTimeBeforeSleep = pspTimerCounterGet(E_MACHINE_TIMER);

  /* Let the SweRVolf FPGA timer to start running */
  bspStartTimer();

  /* Halt the core */
  pspPmcHalt();

  /* This line , and the following are executed only when core is not in 'Sleep' */
  udTimeAfterSleep = pspTimerCounterGet(E_MACHINE_TIMER);

  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_AFTER_SLEEP);

  /* verify all test way points were visited */
  if(g_uiTestWayPoints != D_EXT_INT_WAKEUP_TEST_RESULT)
  {
    /* Test failed */
    M_DEMO_ENDLESS_LOOP();
  }

  /* verify that core was indeed halted */
  if(udTimeAfterSleep - udTimeBeforeSleep < D_SLEEP_TIME)
  {
    /* Test failed */
    M_DEMO_ENDLESS_LOOP();
  }
}


/**
 * @brief - Machine timer ISR
 *
 */
void demoMtimerIsrHandler(void)
{
  /* Disable Machine-Timer interrupt */
  pspDisableInterruptNumberMachineLevel(E_MACHINE_TIMER_CAUSE);

  /* Mark that ISR visited */
  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_IN_MTIMER_ISR);
}


/**
 * @brief - Set core to sleep (halt/fw-halt) mode and wake it up with machine timer interrupt
 *
 */
void demoSleepAndWakeupByMtimer(void)
{
  u64_t udTimeBeforeSleep;
  u64_t udTimeAfterSleep;

  /* Zero the test results variable */
  g_uiTestWayPoints = 0;

  /* Register Machine timer interrupt handler */
  pspRegisterInterruptHandler(demoMtimerIsrHandler, E_MACHINE_TIMER_CAUSE);

  /* Enable Machine timer interrupt */
  pspEnableInterruptNumberMachineLevel(E_MACHINE_TIMER_CAUSE);

  /* Activate Machine timer */
  pspTimerCounterSetupAndRun(E_MACHINE_TIMER, M_DEMO_MSEC_TO_CYCLES(D_SLEEP_TIME));

  /* Enable all Machine level interrupts */
  pspInterruptsEnable();

  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_BEFORE_SLEEP);

  udTimeBeforeSleep = pspTimerCounterGet(E_MACHINE_TIMER);

  /* Sets core to Sleep (pmu/fw-halt) mode */
  pspPmcHalt();

  /* This line , and the following are executed only when core is not in 'Sleep' */
  udTimeAfterSleep = pspTimerCounterGet(E_MACHINE_TIMER);

  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_AFTER_SLEEP);

  /* verify all test way points were visited */
  if(g_uiTestWayPoints != D_MTIMER_WAKEUP_TEST_RESULT)
  {
    /* Test failed */
    M_DEMO_ENDLESS_LOOP();
  }

  /* verify that core was indeed halted */
  if(udTimeAfterSleep - udTimeBeforeSleep < D_SLEEP_TIME)
  {
    /* Test failed */
    M_DEMO_ENDLESS_LOOP();
  }
}


/**
 * @brief - Stall the core ('pause', per EH1 PRM) and resume it when count down expires
 *
 */
void demoStallAndResumeByCountdown(void)
{
  u64_t udTimeBeforeStall;
  u64_t udTimeAfterStall;

  /* Zero the test results variable */
  g_uiTestWayPoints = 0;

  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_BEFORE_STALL);

  udTimeBeforeStall = pspTimerCounterGet(E_MACHINE_TIMER);

  /* Pause core */
  pspPmcStall(M_DEMO_MSEC_TO_CYCLES(D_STALL_TIME));

  /* This line , and the following are executed only when core is not in 'Pause' */
  udTimeAfterStall = pspTimerCounterGet(E_MACHINE_TIMER);

  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_AFTER_STALL);

  /* Verify all test way points were visited */
  if(g_uiTestWayPoints != D_STALL_COUNTDOWN_TEST_RESULT)
  {
    /* Test failed */
    M_DEMO_ENDLESS_LOOP();
  }

  /* Verify that core was indeed paused */
  if(udTimeAfterStall - udTimeBeforeStall < D_STALL_TIME)
  {
    /* Test failed */
    M_DEMO_ENDLESS_LOOP();
  }
}

/**
 * @brief - Stall the core ('pause', per EH1 PRM) and resume it with  machine timer interrupt
 *
 */
void demoStallAndResumeByMtimeInterrupt(void)
{
  u64_t udTimeBeforeStall;
  u64_t udTimeAfterStall;
  u64_t udStallTime;

  /* Zero the test results variable */
  g_uiTestWayPoints = 0;

  /* Register Machine timer interrupt handler */
  pspRegisterInterruptHandler(demoMtimerIsrHandler, E_MACHINE_TIMER_CAUSE);

  /* Enable Machine timer interrupt */
  pspEnableInterruptNumberMachineLevel(E_MACHINE_TIMER_CAUSE);

  /* Activate Machine timer */
  pspTimerCounterSetupAndRun(E_MACHINE_TIMER, M_DEMO_MSEC_TO_CYCLES(D_STALL_TIME));

  /* Enable all Machine level interrupts */
  pspInterruptsEnable();

  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_BEFORE_STALL);

  udTimeBeforeStall = pspTimerCounterGet(E_MACHINE_TIMER);

  /* Stall the core - for longer time than Machine timer duration */
  pspPmcStall(M_DEMO_MSEC_TO_CYCLES(D_LONG_STALL_TIME));

  /* This line , and the following are executed only when core is not in 'Stall' */
  udTimeAfterStall = pspTimerCounterGet(E_MACHINE_TIMER);

  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_AFTER_STALL);

  /* Verify all test way points were visited */
  if(g_uiTestWayPoints != D_STALL_MTIME_INT_TEST_RESULT)
  {
    /* Test failed */
    M_DEMO_ENDLESS_LOOP();
  }

  /* Verify that core (1) was indeed stalled and (2) then resumed not due to count down but due to mtimer interrupt */
  udStallTime = udTimeAfterStall - udTimeBeforeStall;
      /* That means core was not in stall at all */
  if( (udStallTime < M_DEMO_MSEC_TO_CYCLES(D_STALL_TIME)) ||
        /* That means core resumed upon count-down and not upon interrupt */
        (udStallTime > M_DEMO_MSEC_TO_CYCLES(D_LONG_STALL_TIME)) )
  {
    /* Test failed */
    M_DEMO_ENDLESS_LOOP();
  }
}


/**
 * @brief - Stall the core ('pause', per EH1 PRM) and resume it with  external interrupt
 *
 */
void demoStallAndResumeByExternalInterrupt(void)
{
  u64_t udTimeBeforeStall;
  u64_t udTimeAfterStall;
  u64_t udStallTime;

  /* Zero the test results variable */
  g_uiTestWayPoints = 0;

  /* Register external interrupt handler */
  pspExternalInterruptRegisterISR(D_DEMO_IRQ, demoExternalInterruptIsr, 0);

  /* Rout SweRVolf FPGA timer to IRQ3 assertion - i.e. when the timer expires, IRQ3 external interrupt is asserted */
  bspRoutTimer(D_DEMO_TIMER_TO_IRQ);

  /* Set SweRVolf FPGA Timer duration (upon expiration, it will trigger an external interrupt) */
  bspSetTimerDurationMsec(D_STALL_TIME);

  /* Setup external interrupts */
    demoSetupExternalInterrupts();

  /* Enable all machine level interrupts */
    pspInterruptsEnable();

  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_BEFORE_STALL);

  udTimeBeforeStall = pspTimerCounterGet(E_MACHINE_TIMER);

  /* Let the SweRVolf FPGA timer to start running */
  bspStartTimer();

  /* Pause core - for longer time than machine timer duration */
  pspPmcStall(M_DEMO_MSEC_TO_CYCLES(D_LONG_STALL_TIME));

  /* This line , and the following are executed only when core is not in 'Stall' */
  udTimeAfterStall = pspTimerCounterGet(E_MACHINE_TIMER);

  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_AFTER_STALL);

  /* Verify all test way points were visited */
  if(g_uiTestWayPoints != D_STALL_EXT_INT_TEST_RESULT)
  {
    /* Test failed */
    M_DEMO_ENDLESS_LOOP();
  }

  /* Verify that core (1) was indeed stalled and (2) then resumed not due to count down but due to external interrupt */
  udStallTime = udTimeAfterStall - udTimeBeforeStall;
      /* That means core was not in stall at all */
  if( (udStallTime < M_DEMO_MSEC_TO_CYCLES(D_STALL_TIME)) ||
        /* That means core resumed upon count-down and not upon interrupt */
        (udStallTime > M_DEMO_MSEC_TO_CYCLES(D_LONG_STALL_TIME)) )
  {
    /* Test failed */
    M_DEMO_ENDLESS_LOOP();
  }
}


/**
 * @brief - startup point of the demo application. called from main function.
 *
 */
void demoStart(void)
{
  u32_t uiPrevIntState;

  /* Register interrupt vector */
  pspInterruptsSetVectorTableAddress(&psp_vect_table);

  /************************/
  /* Part1 - Sleep tests  */
  /************************/
  /* Set core to sleep (halt/fw-halt) mode and wake it up with machine timer interrupt */
  demoSleepAndWakeupByMtimer();
  /* Set core to Sleep (pmu/fw-halt) mode and wake it up with external interrupt */
  demoSleepAndWakeupByExternalInterrupt();
    /* Disable interrupts at the end of part 1*/
  pspInterruptsDisable(&uiPrevIntState);

  /************************/
  /* Part2 - Stall tests  */
  /************************/
  /* Stall the core ('pause', per EH1 PRM) and resume it when count down expires */
  demoStallAndResumeByCountdown();
  /* Stall the core ('pause', per EH1 PRM) and resume it with  machine timer interrupt */
  demoStallAndResumeByMtimeInterrupt();
  /* Stall the core ('pause', per EH1 PRM) and resume it with  external interrupt */
  demoStallAndResumeByExternalInterrupt();

  /* Arriving here means all tests passed successfully */
  demoOutputMsg("Power Management Control tests passed successfully\n",52);

}
