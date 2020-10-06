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
* @file   psp_pmc.c
* @author Alex Dvoskin
* @date   March 2020
* @brief  This file implements core's power management control service functions
*
*/

/**
* include files
*/
#include "psp_api.h"


/**
* definitions
*/
#define D_PSP_INTERRUPTS_DISABLE_IN_HALT   0

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

#ifdef D_EHX1_VER_1_0 /* 'haltie' feature is added to SweRV EHX1 from version 1.0 only */
/**
* @brief Initiate core halt (i.e., transition to Halted (pmu/fw-halt, C3) state)
*
* @param uiEnableInterrupts - indication whether to (atomically) enable interrupts upon transition to 'halted' mode or not
*
* @return none
*/
D_PSP_TEXT_SECTION void pspPmcHalt(u32_t uiEnableInterrupts)
{
  if (D_PSP_INTERRUPTS_DISABLE_IN_HALT == uiEnableInterrupts)
  {
	  M_PSP_WRITE_CSR(D_PSP_MPMC_NUM, D_PSP_MPMC_HALT_MASK); /* Initiate 'Halted' mode. Don't enable interrupts upon initiation */
  }
  else
  {
	  M_PSP_WRITE_CSR(D_PSP_MPMC_NUM, (D_PSP_MPMC_HALT_MASK | D_PSP_MPMC_HALTIE_MASK)); /* Initiate 'Halted' mode. Atomically enable interrupts upon initiation */
  }

}
#else /* #ifdef D_EHX1_VER_0_9 does not contain 'haltie' feature */
/**
* @brief Initiate core halt (i.e., transition to Halted (pmu/fw-halt, C3) state)
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspPmcHalt(void)
{
  M_PSP_WRITE_CSR(D_PSP_MPMC_NUM, D_PSP_MPMC_HALT_MASK); /* Initiate 'Halted' mode. Don't enable interrupts upon initiation */
}
#endif /* D_EHX1_VER_1_0 or D_EHX1_VER_0_9 */

/**
* @brief The following function temporarily stop the core from executing instructions for given number of core clock cycles(ticks)
*
* @ticks - Number of core clock cycles
*
* @return none
*/
D_PSP_TEXT_SECTION void pspPmcStall(u32_t uiTicks)
{
  M_PSP_WRITE_CSR(D_PSP_MCPC_NUM, uiTicks);
}


