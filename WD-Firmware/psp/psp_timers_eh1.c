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
* @file   psp_timers_eh1.c
* @author Nati Rapaport
* @date   8.12.2019
* @brief  This file implements EH1 timers service functions
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
* @parameter - uiPeriodCycles - defines the timer's period in cycles
*
***************************************************************************************************/
D_PSP_TEXT_SECTION void pspTimerSetupMachineTimer(u32_t uiPeriodCycles)
{
  #if (0 == D_MTIME_ADDRESS) || (0 == D_MTIMECMP_ADDRESS)
    #error "D_MTIME_ADDRESS or D_MTIMECMP_ADDRESS is not defined"
  #endif

  volatile u64_t *pMtime    = (u64_t*)D_MTIME_ADDRESS;
  volatile u64_t *pMtimecmp = (u64_t*)D_MTIMECMP_ADDRESS;
  u64_t udNow, udThen;

  /* Set the mtime and mtimecmp (memory-mapped registers) per privileged spec */
  udNow = *pMtime;
  udThen = udNow + uiPeriodCycles;
  *pMtimecmp = udThen;
}


/**
* APIs
*/

/**
* @brief Setup and activate Timer
*
* @parameter - timer            - indicates which timer to setup and run
* @parameter - uiPeriodCycles - defines the timer's period in cycles
*
***************************************************************************************************/
D_PSP_TEXT_SECTION void pspTimerCounterSetupAndRun(u32_t uiTimer, u32_t uiPeriodCycles)
{
  u32_t uiNow, uiThen;

  M_PSP_ASSERT((E_MACHINE_TIMER == uiTimer) || (E_INTERNAL_TIMER0 == uiTimer) || (E_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case E_MACHINE_TIMER:
      pspTimerSetupMachineTimer(uiPeriodCycles);
      break;
    case E_INTERNAL_TIMER0:
      /* Read Timer0 counter */
      uiNow = M_PSP_READ_CSR(D_PSP_MITCNT0_NUM);
      uiThen = uiNow + uiPeriodCycles;
      /* Set Timer0 bound */
      M_PSP_WRITE_CSR(D_PSP_MITBND0_NUM, uiThen);
      /* Enable Timer0 counting */
      M_PSP_SET_CSR(D_PSP_MITCTL0_NUM, D_PSP_MITCTL_EN_MASK);
      break;
    case E_INTERNAL_TIMER1:
      /* Read Timer1 counter */
      uiNow = M_PSP_READ_CSR(D_PSP_MITCNT1_NUM);
      uiThen = uiNow + uiPeriodCycles;
      /* Set Timer1 bound */
      M_PSP_WRITE_CSR(D_PSP_MITBND1_NUM, uiThen);
      /* Enable Timer1 counting */
      M_PSP_SET_CSR(D_PSP_MITCTL1_NUM, D_PSP_MITCTL_EN_MASK);
      break;
    default:
      break;
  }
}

/**
* @brief Get Timer counter value
*
* @parameter - timer - indicates which timer to setup and run
*
* @return u64_t      - Timer counter value
*
***************************************************************************************************/
D_PSP_TEXT_SECTION u64_t pspTimerCounterGet(u32_t uiTimer)
{
  u64_t udCounter = 0;

  M_PSP_ASSERT((E_MACHINE_TIMER == uiTimer) || (E_INTERNAL_TIMER0 == uiTimer) || (E_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case E_MACHINE_TIMER:
      udCounter = *(u64_t*)D_MTIME_ADDRESS;
      break;
    case E_INTERNAL_TIMER0:
      udCounter = (u64_t)M_PSP_READ_CSR(D_PSP_MITCNT0_NUM);
      break;
    case E_INTERNAL_TIMER1:
      udCounter = (u64_t)M_PSP_READ_CSR(D_PSP_MITCNT1_NUM);
      break;
    default:
      break;
  }

  return (udCounter);
}

/**
* @brief Get Time compare counter value
*
* @parameter - timer - indicates which timer to setup and run
*
* @return u64_t      – Time compare counter value
*
***************************************************************************************************/
D_PSP_TEXT_SECTION u64_t pspTimeCompareCounterGet(u32_t uiTimer)
{
  u64_t udCounterCompare = 0;

  M_PSP_ASSERT((E_MACHINE_TIMER == uiTimer) || (E_INTERNAL_TIMER0 == uiTimer) || (E_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case E_MACHINE_TIMER:
      udCounterCompare = *(u64_t*)D_MTIMECMP_ADDRESS;
      break;
    case E_INTERNAL_TIMER0:
      udCounterCompare = (u64_t)M_PSP_READ_CSR(D_PSP_MITBND0_NUM);
      break;
    case E_INTERNAL_TIMER1:
      udCounterCompare = (u64_t)M_PSP_READ_CSR(D_PSP_MITBND1_NUM);
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
D_PSP_TEXT_SECTION void pspTimerEnableCountInSleepMode(u32_t uiTimer)
{
  M_PSP_ASSERT((E_INTERNAL_TIMER0 == uiTimer) || (E_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case E_INTERNAL_TIMER0:
      M_PSP_SET_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_HALT_EN_MASK);
      break;
    case E_INTERNAL_TIMER1:
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
D_PSP_TEXT_SECTION void pspTimerDisableCountInSleepMode(u32_t uiTimer)
{
  M_PSP_ASSERT((E_INTERNAL_TIMER0 == uiTimer) || (E_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case E_INTERNAL_TIMER0:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_HALT_EN_MASK);
      break;
    case E_INTERNAL_TIMER1:
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
D_PSP_TEXT_SECTION void pspTimerEnableCountInStallMode(u32_t uiTimer)
{
  M_PSP_ASSERT((E_INTERNAL_TIMER0 == uiTimer) || (E_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case E_INTERNAL_TIMER0:
      M_PSP_SET_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    case E_INTERNAL_TIMER1:
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
D_PSP_TEXT_SECTION void pspTimerDisableCountInStallMode(u32_t uiTimer)
{
  M_PSP_ASSERT((E_INTERNAL_TIMER0 == uiTimer) || (E_INTERNAL_TIMER1 == uiTimer));

  switch (uiTimer)
  {
    case E_INTERNAL_TIMER0:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL0_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    case E_INTERNAL_TIMER1:
      M_PSP_CLEAR_CSR(D_PSP_MITCTL1_NUM,D_PSP_MITCTL_PAUSE_EN_MASK);
      break;
    default:
      break;
  }
}



