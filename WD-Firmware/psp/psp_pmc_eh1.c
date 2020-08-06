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
#define D_PSP_PMC_INITIATE_HALT (0x1)
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
* @brief Initiate core halt (i.e., transition to Halted (pmu/fw-halt, C3) state)
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspPmcHalt(void)
{
  M_PSP_WRITE_CSR(D_PSP_MPMC_NUM, D_PSP_PMC_INITIATE_HALT);
}

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


