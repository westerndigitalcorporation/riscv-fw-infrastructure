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
* @file   psp_timers_el2.c
* @author Nati Rapaport
* @date   19.08.2020
* @brief  This file implements EL2 timers service functions
*
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
 * Internal functions
 */

/**
* @brief Setup the countup of Machine Timer
*
* @parameter - udPeriodCycles - defines the timer's period in cycles
*
***************************************************************************************************/
D_PSP_TEXT_SECTION void pspMachineTimerSetupTimerCounter(u64_t udPeriodCycles)
{
  #if (0 == D_MTIME_ADDRESS) || (0 == D_MTIMECMP_ADDRESS)
    #error "D_MTIME_ADDRESS or D_MTIMECMP_ADDRESS is not defined"
  #endif

  volatile u64_t *pMtime    = (u64_t*)D_MTIME_ADDRESS;
  volatile u64_t *pMtimecmp = (u64_t*)D_MTIMECMP_ADDRESS;
  u64_t udNow, udThen;

  /* Set the mtime and mtimecmp (memory-mapped registers) per privileged spec */
  udNow = *pMtime;
  udThen = udNow + udPeriodCycles;
  *pMtimecmp = udThen;
}

/**
* @brief Cascade Timer0 and Timer1 to act as a single timer
*        In this mode, Timer1 counts up when Timer0 reachs its bound value. Timer1 interrupt raises when Timer1 reachs its bound.
*        **Note** In 'cascade' mode HALT-EN, and PAUSE-EN indications must be the set identically for both timers
*                 so part this function also set disable all of them here.
*
* @parameter - udPeriodCycles - defines the timer's period in cycles
*
***************************************************************************************************/
D_PSP_TEXT_SECTION void pspMachineTimerSetup64bitTimer(u64_t udPeriodCycles)
{
  u32_t uiNow, uiThen;

  /* Read Timer0 counter */
  uiNow = M_PSP_READ_CSR(D_PSP_MITCNT0_NUM);
  /* Add the lower 32bit of the input 'udPeriodCycles' parameter */
  uiThen = uiNow + (u32_t)udPeriodCycles;
  /* Set Timer0 bound */
  M_PSP_WRITE_CSR(D_PSP_MITBND0_NUM, uiThen);
  /* Read Timer1 counter */
  uiNow = M_PSP_READ_CSR(D_PSP_MITCNT1_NUM);
  /* Add the upper 32bit of the input 'udPeriodCycles' parameter */
  uiThen = uiNow + (u32_t)(udPeriodCycles >> D_PSP_SHIFT_32);
  /* Set Timer0 bound */
  M_PSP_WRITE_CSR(D_PSP_MITBND1_NUM, uiThen);

  /* In cascade mode, the HALT-EN, and PAUSE-EN bits must be the set identically for both timers - so disable all of them them now */
  M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
  M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
  M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_HALT_EN_MASK);
  M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_HALT_EN_MASK);

  /* Enable Timer0 and Timer1 counting */
  M_PSP_SET_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_EN_MASK);
  M_PSP_SET_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_EN_MASK);
}

/**
* APIs
*/

/**
* @brief Setup and activate Timer
*
* @parameter - timer            - indicates which timer to setup and run
* @parameter - uiPeriodCycles   - defines the timer's period in cycles
*
***************************************************************************************************/
D_PSP_TEXT_SECTION void pspMachineTimerCounterSetupAndRun(u32_t uiTimer, u64_t udPeriodCycles)
{
  u32_t uiNow, uiThen;

  M_PSP_ASSERT((D_PSP_MACHINE_TIMER == uiTimer) || (D_PSP_INTERNAL_TIMER0 == uiTimer) ||
           (D_PSP_INTERNAL_TIMER1 == uiTimer) || (D_PSP_INTERNAL_64BIT_TIMER == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_MACHINE_TIMER:
      pspMachineTimerSetupTimerCounter(udPeriodCycles);
      break;
    case D_PSP_INTERNAL_TIMER0:
      /* Read Timer0 counter */
      uiNow = M_PSP_READ_CSR(D_PSP_MITCNT0_NUM);
      uiThen = uiNow + (u32_t)udPeriodCycles;
      /* Set Timer0 bound */
      M_PSP_WRITE_CSR(D_PSP_MITBND0_NUM, uiThen);
      /* Enable Timer0 counting */
      M_PSP_SET_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      /* Read Timer1 counter */
      uiNow = M_PSP_READ_CSR(D_PSP_MITCNT1_NUM);
      uiThen = uiNow + (u32_t)udPeriodCycles;
      /* Set Timer1 bound */
      M_PSP_WRITE_CSR(D_PSP_MITBND1_NUM, uiThen);
      /* Enable Timer1 counting */
      M_PSP_SET_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_EN_MASK);
      break;
    case D_PSP_INTERNAL_64BIT_TIMER:
      pspMachineTimerSetup64bitTimer(udPeriodCycles);
      break;
    default:
      break;
  }
}

/**
* @brief Get Timer counter value
*
* @parameter - timer - indicates from which timer to get the counter value
*
* @return u64_t      - Timer counter value
*
***************************************************************************************************/
D_PSP_TEXT_SECTION u64_t pspMachineTimerCounterGet(u32_t uiTimer)
{
  u64_t udCounter = 0;
  u64_t udCounterTemp = 0;

  M_PSP_ASSERT((D_PSP_MACHINE_TIMER == uiTimer) || (D_PSP_INTERNAL_TIMER0 == uiTimer) ||
           (D_PSP_INTERNAL_TIMER1 == uiTimer) || (D_PSP_INTERNAL_64BIT_TIMER == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_MACHINE_TIMER:
      udCounter = *(u64_t*)D_MTIME_ADDRESS;
      break;
    case D_PSP_INTERNAL_TIMER0:
      udCounter = (u64_t)M_PSP_READ_CSR(D_PSP_MITCNT0_NUM);
      break;
    case D_PSP_INTERNAL_TIMER1:
      udCounter = (u64_t)M_PSP_READ_CSR(D_PSP_MITCNT1_NUM);
      break;
    case D_PSP_INTERNAL_64BIT_TIMER:
    udCounterTemp = M_PSP_READ_CSR(D_PSP_MITCNT1_NUM);
    udCounter = udCounterTemp << D_PSP_SHIFT_32;
    udCounter |= M_PSP_READ_CSR(D_PSP_MITCNT0_NUM);
      break;
    default:
      break;
  }

  return (udCounter);
}

/**
* @brief Get Time compare counter value
*
* @parameter - timer - indicates from which timer to get the compare-counter value
*
* @return u64_t      – Time compare counter value
*
***************************************************************************************************/
D_PSP_TEXT_SECTION u64_t pspMachineTimerCompareCounterGet(u32_t uiTimer)
{
  u64_t udCounterCompare = 0;
  u64_t udCounterCompareTemp = 0;

  M_PSP_ASSERT((D_PSP_MACHINE_TIMER == uiTimer) || (D_PSP_INTERNAL_TIMER0 == uiTimer) ||
           (D_PSP_INTERNAL_TIMER1 == uiTimer) || (D_PSP_INTERNAL_64BIT_TIMER == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_MACHINE_TIMER:
      udCounterCompare = *(u64_t*)D_MTIMECMP_ADDRESS;
      break;
    case D_PSP_INTERNAL_TIMER0:
      udCounterCompare = (u64_t)M_PSP_READ_CSR(D_PSP_MITBND0_NUM);
      break;
    case D_PSP_INTERNAL_TIMER1:
      udCounterCompare = (u64_t)M_PSP_READ_CSR(D_PSP_MITBND1_NUM);
      break;
    case D_PSP_INTERNAL_64BIT_TIMER:
      udCounterCompareTemp = M_PSP_READ_CSR(D_PSP_MITBND1_NUM);
      udCounterCompare = udCounterCompareTemp << D_PSP_SHIFT_32;
      udCounterCompare |= M_PSP_READ_CSR(D_PSP_MITBND0_NUM);
      break;
    default:
      break;
  }

  return (udCounterCompare);
}

/**
* @brief Enable timer counting when core in sleep mode
*
* @parameter - uiTimer  - indicates which timer to setup
*
***************************************************************************************************/
D_PSP_TEXT_SECTION void pspMachineTimerEnableCountInSleepMode(u32_t uiTimer)
{
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer) || (D_PSP_INTERNAL_64BIT_TIMER == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      M_PSP_SET_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_HALT_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      M_PSP_SET_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_HALT_EN_MASK);
      break;
    case D_PSP_INTERNAL_64BIT_TIMER:
      M_PSP_SET_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_HALT_EN_MASK);
      M_PSP_SET_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_HALT_EN_MASK);
      break;
    default:
      break;
  }
}

/**
* @brief Disable timer counting when core in sleep mode
*
* @parameter - uiTimer  - indicates which timer to setup
*
***************************************************************************************************/
D_PSP_TEXT_SECTION void pspMachineTimerDisableCountInSleepMode(u32_t uiTimer)
{
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer) || (D_PSP_INTERNAL_64BIT_TIMER == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_HALT_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_HALT_EN_MASK);
      break;
    case D_PSP_INTERNAL_64BIT_TIMER:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_HALT_EN_MASK);
      M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_HALT_EN_MASK);
      break;
    default:
      break;
  }
}

/**
* @brief Enable timer counting when core in in stall
*
* @parameter - uiTimer  - indicates which timer to setup
*
***************************************************************************************************/
D_PSP_TEXT_SECTION void pspMachineTimerEnableCountInStallMode(u32_t uiTimer)
{
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer) || (D_PSP_INTERNAL_64BIT_TIMER == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      M_PSP_SET_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      M_PSP_SET_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    case D_PSP_INTERNAL_64BIT_TIMER:
      M_PSP_SET_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      M_PSP_SET_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    default:
      break;
  }
}

/**
* @brief Disable timer counting when core in in stall
*
* @parameter - uiTimer  - indicates which timer to setup
*
***************************************************************************************************/
D_PSP_TEXT_SECTION void pspMachineTimerDisableCountInStallMode(u32_t uiTimer)
{
  M_PSP_ASSERT((D_PSP_INTERNAL_TIMER0 == uiTimer) || (D_PSP_INTERNAL_TIMER1 == uiTimer) || (D_PSP_INTERNAL_64BIT_TIMER == uiTimer));

  switch (uiTimer)
  {
    case D_PSP_INTERNAL_TIMER0:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    case D_PSP_INTERNAL_TIMER1:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    case D_PSP_INTERNAL_64BIT_TIMER:
        M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
        M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    default:
      break;
  }
}
