/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2020-2021 Western Digital Corporation or its affiliates.
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

#ifdef D_EHX1_VER_1_0 /* 'haltie' feature is added to SweRV EHX1 from version 1.0 only */
  /* Upon initiating 'Halted' mode - whether to atomically enable interrupts or not */
  #define D_DEMO_DO_NOT_ENABLE_INTERRUPTS_UPON_HALT     0
  #define D_DEMO_ATOMICALLY_ENABLE_INTERRUPTS_UPON_HALT 1
#endif

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
volatile u32_t g_uiTestWayPoints;

/**
* functions
*/

/**
 * @brief - External interrupt ISR
 *
* NOTE: In EH2 the ISR function should be defined with 'interrupt' attribute because EH2 works in 'fast-interrupt'
* mode, meaning that upon external-interrupt the PC arrives directly to the ISR rather than to the trap.
* Setting the 'interrupt' attribute here causes the compiler to do the prologue & epilogue of interrupt and also calls 'mret'
* at the end of the ISR (in the other cores, where 'fast-interrupt' mode is not in use all that is done by the trap code)
*
*/
#ifdef D_SWERV_FAST_INT
D_PSP_INTERRUPT void demoExternalInterruptIsr(void)
#else
void demoExternalInterruptIsr(void)
#endif
{
    /* Stop the generation of the specific external interrupt */
    bspClearExtInterrupt(D_DEMO_IRQ);

  /* Disable External-interrupts */
  pspMachineInterruptsDisableIntNumber(D_PSP_INTERRUPTS_MACHINE_EXT);

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
  pspMachineExtInterruptSetPriorityOrder(D_PSP_EXT_INT_STANDARD_PRIORITY);

    /* Set interrupts threshold to minimal level (== all interrupts should be served) */
  pspMachineExtInterruptsSetThreshold(M_PSP_MACHINE_EXT_INT_THRESHOLD_UNMASK_ALL_VALUE);

  /* Set the nesting priority threshold to minimal level (== all interrupts should be served) */
  pspMachineExtInterruptsSetNestingPriorityThreshold(M_PSP_MACHINE_EXT_INT_THRESHOLD_UNMASK_ALL_VALUE);

  /* Set Gateway Interrupt type (Level) */
  pspMachineExtInterruptSetType(D_DEMO_IRQ, D_PSP_EXT_INT_LEVEL_TRIG_TYPE);

  /* Set gateway Polarity (Active high) */
  pspMachineExtInterruptSetPolarity(D_DEMO_IRQ, D_PSP_EXT_INT_ACTIVE_HIGH);

  /* Set the priority level to highest */
  pspMachineExtInterruptSetPriority(D_DEMO_IRQ, M_PSP_MACHINE_EXT_INT_PRIORITY_SET_TO_HIGHEST_VALUE);

  /* Enable IRQ3 interrupt in the PIC */
  pspMachineExternalInterruptEnableNumber(D_DEMO_IRQ);

  /* Enable external interrupts */
  pspMachineInterruptsEnableIntNumber(E_MACHINE_EXTERNAL_CAUSE);
}

/**
 * @brief - Set core to Sleep (pmu/fw-halt) mode and wake it up with external interrupt
 *          In this test we use the SweRVolf FPGA timer to trigger external-interrupt
 *
 */
void demoSleepAndWakeupByExternalInterrupt(void)
{
  u32_t uiPrevIntState;
  u64_t udTimeBeforeSleep;
  u64_t udTimeAfterSleep;

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiPrevIntState);

  /* Zero the test results variable */
  g_uiTestWayPoints = 0;

  /* register external interrupt handler */
  pspMachineExternalInterruptRegisterISR(D_DEMO_IRQ, demoExternalInterruptIsr, 0);

  /* Rout SweRVolf FPGA timer to IRQ3 assertion - i.e. when the timer expires, IRQ3 external interrupt is asserted */
  bspRoutTimer(D_DEMO_TIMER_TO_IRQ);

  /* Set SweRVolf FPGA Timer duration (upon expiration, it will trigger an external interrupt) */
  bspSetTimerDurationMsec(D_SLEEP_TIME);

  /* Setup external interrupts */
  demoSetupExternalInterrupts();

  /* Enable all machine level interrupts */
  pspMachineInterruptsEnable();

  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_BEFORE_SLEEP);

  udTimeBeforeSleep = pspMachineTimerCounterGet(D_PSP_MACHINE_TIMER);

  /* Let the SweRVolf FPGA timer to start running */
  bspStartTimer();

#ifdef D_EHX1_VER_1_0 /* 'haltie' feature is added to SweRV EHX1 from version 1.0 only */
  /* Halt the core - do not activate the "interrupt-enable" atomically upon 'Halted' initiation */
  pspMachinePowerMngCtrlHalt(D_DEMO_DO_NOT_ENABLE_INTERRUPTS_UPON_HALT);
#else /* D_EHX1_VER_0_9 - does not contain 'haltie' feature */
  /* Halt the core */
  pspMachinePowerMngCtrlHalt();
#endif

  /* This line , and the following are executed only when core is not in 'Sleep' */
  udTimeAfterSleep = pspMachineTimerCounterGet(D_PSP_MACHINE_TIMER);

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
  pspMachineInterruptsDisableIntNumber(E_MACHINE_TIMER_CAUSE);

  /* Mark that ISR visited */
  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_IN_MTIMER_ISR);
}


/**
 * @brief - Set core to sleep (halt/fw-halt) mode and wake it up with machine timer interrupt
 *
 */
void demoSleepAndWakeupByMtimer(void)
{
  u32_t uiPrevIntState;
  u64_t udTimeBeforeSleep;
  u64_t udTimeAfterSleep;

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiPrevIntState);

  /* Zero the test results variable */
  g_uiTestWayPoints = 0;

  /* Register Machine timer interrupt handler */
  pspMachineInterruptsRegisterIsr(demoMtimerIsrHandler, E_MACHINE_TIMER_CAUSE);

  /* Enable Machine timer interrupt */
  pspMachineInterruptsEnableIntNumber(E_MACHINE_TIMER_CAUSE);

  /* Activate Machine timer */
  pspMachineTimerCounterSetupAndRun(D_PSP_MACHINE_TIMER, M_DEMO_MSEC_TO_CYCLES(D_SLEEP_TIME));

  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_BEFORE_SLEEP);

  udTimeBeforeSleep = pspMachineTimerCounterGet(D_PSP_MACHINE_TIMER);

  /* Enable all Machine level interrupts */
  pspMachineInterruptsEnable();

#ifdef D_EHX1_VER_1_0 /* 'haltie' feature is added to SweRV EHX1 from version 1.0 only */
  /* Sets core to Sleep (pmu/fw-halt) mode - do not activate the "interrupt-enable" atomically upon 'Halted' initiation */
  pspMachinePowerMngCtrlHalt(D_DEMO_DO_NOT_ENABLE_INTERRUPTS_UPON_HALT);
#else /* D_EHX1_VER_0_9 - does not contain 'haltie' feature */
  /* Halt the core */
  pspMachinePowerMngCtrlHalt();
#endif

  /* This line , and the following are executed only when core is not in 'Sleep' */
  udTimeAfterSleep = pspMachineTimerCounterGet(D_PSP_MACHINE_TIMER);

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

#ifdef D_EHX1_VER_1_0  /* 'haltie' feature is added to SweRV EHX1 from version 1.0 only */
/**
 * @brief - Set core to Sleep (pmu/fw-halt) mode with 'haltie' option enabled and wake it up with timer interrupt.
 *          This test is like demoSleepAndWakeupByMtimer test with one difference - Interrupts are enabled not via explicit write
 *          to MSTATUS CSR (mie bit) but via activating 'haltie' option. That means interrupts are enabled atomically only upon 'Halt' initiation.
 */
void demoSleepHaltIeOption(void)
{
  u64_t udTimeBeforeSleep;
  u64_t udTimeAfterSleep;
  u32_t uiIntStatus;

  /* Zero the test results variable */
  g_uiTestWayPoints = 0;

  /* Disable all machine level interrupts (interrupts will be enabled using the 'halt-ie' option, upon 'Halted' initiation */
  pspMachineInterruptsDisable(&uiIntStatus);

  /* Register Machine timer interrupt handler */
  pspMachineInterruptsRegisterIsr(demoMtimerIsrHandler, E_MACHINE_TIMER_CAUSE);

  /* Enable Machine timer interrupt */
  pspMachineInterruptsEnableIntNumber(E_MACHINE_TIMER_CAUSE);

  /* Activate Machine timer */
  pspMachineTimerCounterSetupAndRun(D_PSP_MACHINE_TIMER, M_DEMO_MSEC_TO_CYCLES(D_SLEEP_TIME));

  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_BEFORE_SLEEP);

  udTimeBeforeSleep = pspMachineTimerCounterGet(D_PSP_MACHINE_TIMER);

  /* Sets core to Sleep (pmu/fw-halt) mode - do not activate the "interrupt-enable" atomically upon 'Halted' initiation */
  pspMachinePowerMngCtrlHalt(D_DEMO_ATOMICALLY_ENABLE_INTERRUPTS_UPON_HALT);

  /* This line , and the following are executed only when core is not in 'Sleep' */
  udTimeAfterSleep = pspMachineTimerCounterGet(D_PSP_MACHINE_TIMER);

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
#endif /* D_EHX1_VER_1_0 */

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

  udTimeBeforeStall = pspMachineTimerCounterGet(D_PSP_MACHINE_TIMER);

  /* Pause core */
  pspMachinePowerMngCtrlStall(M_DEMO_MSEC_TO_CYCLES(D_STALL_TIME));

  /* This line , and the following are executed only when core is not in 'Pause' */
  udTimeAfterStall = pspMachineTimerCounterGet(D_PSP_MACHINE_TIMER);

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
  u32_t uiPrevIntState;
  u64_t udTimeBeforeStall;
  u64_t udTimeAfterStall;
  u64_t udStallTime;

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiPrevIntState);

  /* Zero the test results variable */
  g_uiTestWayPoints = 0;

  /* Register Machine timer interrupt handler */
  pspMachineInterruptsRegisterIsr(demoMtimerIsrHandler, E_MACHINE_TIMER_CAUSE);

  /* Enable Machine timer interrupt */
  pspMachineInterruptsEnableIntNumber(E_MACHINE_TIMER_CAUSE);

  /* Activate Machine timer */
  pspMachineTimerCounterSetupAndRun(D_PSP_MACHINE_TIMER, M_DEMO_MSEC_TO_CYCLES(D_STALL_TIME));

  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_BEFORE_STALL);

  udTimeBeforeStall = pspMachineTimerCounterGet(D_PSP_MACHINE_TIMER);

  /* Enable all Machine level interrupts */
  pspMachineInterruptsEnable();

  /* Stall the core - for longer time than Machine timer duration */
  pspMachinePowerMngCtrlStall(M_DEMO_MSEC_TO_CYCLES(D_LONG_STALL_TIME));

  /* This line , and the following are executed only when core is not in 'Stall' */
  udTimeAfterStall = pspMachineTimerCounterGet(D_PSP_MACHINE_TIMER);

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
  u32_t uiPrevIntState;
  u64_t udTimeBeforeStall;
  u64_t udTimeAfterStall;
  u64_t udStallTime;

  /* Disable interrupts */
  pspMachineInterruptsDisable(&uiPrevIntState);

  /* Zero the test results variable */
  g_uiTestWayPoints = 0;

  /* Register external interrupt handler */
  pspMachineExternalInterruptRegisterISR(D_DEMO_IRQ, demoExternalInterruptIsr, 0);

  /* Rout SweRVolf FPGA timer to IRQ3 assertion - i.e. when the timer expires, IRQ3 external interrupt is asserted */
  bspRoutTimer(D_DEMO_TIMER_TO_IRQ);

  /* Set SweRVolf FPGA Timer duration (upon expiration, it will trigger an external interrupt) */
  bspSetTimerDurationMsec(D_STALL_TIME);

  /* Setup external interrupts */
  demoSetupExternalInterrupts();

  /* Enable all machine level interrupts */
  pspMachineInterruptsEnable();

  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_BEFORE_STALL);

  udTimeBeforeStall = pspMachineTimerCounterGet(D_PSP_MACHINE_TIMER);

  /* Let the SweRVolf FPGA timer to start running */
  bspStartTimer();

  /* Pause core - for longer time than machine timer duration */
  pspMachinePowerMngCtrlStall(M_DEMO_MSEC_TO_CYCLES(D_LONG_STALL_TIME));

  /* This line , and the following are executed only when core is not in 'Stall' */
  udTimeAfterStall = pspMachineTimerCounterGet(D_PSP_MACHINE_TIMER);

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
  M_DEMO_START_PRINT();

  /* Register interrupt vector */
  pspMachineInterruptsSetVecTableAddress(&M_PSP_VECT_TABLE);

  /* Run this demo only if target is Swerv. Cannot run on Whisper */
  if (D_PSP_TRUE == demoIsSwervBoard())
  {
#ifdef D_SWERV_EH2
    /* Initialize PSP mutexs */
    pspMutexInitPspMutexs();
#endif
    /************************/
    /* Part1 - Sleep tests  */
    /************************/
    /* Set core to sleep (halt/fw-halt) mode and wake it up with machine timer interrupt */
    demoSleepAndWakeupByMtimer();
    /* Set core to Sleep (pmu/fw-halt) mode and wake it up with external interrupt */
    demoSleepAndWakeupByExternalInterrupt();

#ifdef D_EHX1_VER_1_0 /* 'haltie' feature is added to SweRV EHX1 from version 1.0 only */
    /* Set core to Sleep (pmu/fw-halt) mode , with 'haltie' (Atomically interrupts-enable upon 'Halt' initiation) and wake it up with timer interrupt */
    demoSleepHaltIeOption();
#endif

    /************************/
    /* Part2 - Stall tests  */
    /************************/
    /* Stall the core ('pause', per EH1 PRM) and resume it when count down expires */
    demoStallAndResumeByCountdown();
    /* Stall the core ('pause', per EH1 PRM) and resume it with  machine timer interrupt */
    demoStallAndResumeByMtimeInterrupt();
    /* Stall the core ('pause', per EH1 PRM) and resume it with  external interrupt */
    demoStallAndResumeByExternalInterrupt();
  }
  else
  {
    /* whisper */
    printfNexys("This demo can't run under whisper");
  }

  M_DEMO_END_PRINT();
}
