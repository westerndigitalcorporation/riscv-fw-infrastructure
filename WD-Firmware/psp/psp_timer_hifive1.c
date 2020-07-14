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
* @file   psp_timer_hifive1.c
* @author Nati Rapaport
* @date   13.11.2019
* @brief  This file implements core's timer-counter service functions (for HiFive1 core)
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
* APIs
*/

/**
* @brief Setup and activate core's Timer
*
* @parameter - timer         - indicates which timer to setup and run
* @parameter - uiPeriodCycles  - defines the timer's period in cycles 
*
***************************************************************************************************/
D_PSP_TEXT_SECTION void pspTimerCounterSetupAndRun(u32_t uiTimer, u32_t uiPeriodCycles)
{
  #if (0 == D_MTIME_ADDRESS) || (0 == D_MTIMECMP_ADDRESS)
    #error "D_MTIME_ADDRESS or D_MTIMECMP_ADDRESS is not defined"
  #endif

  /* Set the mtime and mtimecmp (memory-mapped registers) per privileged spec */
  volatile u64_t *pMtime       = (u64_t*)D_MTIME_ADDRESS;
  volatile u64_t *pMtimecmp    = (u64_t*)D_MTIMECMP_ADDRESS;
  u64_t udNow = *pMtime;
  u64_t udThen = udNow + uiPeriodCycles;
  *pMtimecmp = udThen;
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
  volatile u64_t *pMtime       = (u64_t*)D_MTIME_ADDRESS;
  return *pMtime;
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
  volatile u64_t *pMtimecmp    = (u64_t*)D_MTIMECMP_ADDRESS;
  return *pMtimecmp;
}


