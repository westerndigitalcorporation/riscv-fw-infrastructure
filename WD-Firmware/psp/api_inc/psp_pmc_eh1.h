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
* @file   psp_pmc_eh1.h
* @author Alex Dvoskin
* @date   March 2020
* @brief  The file defines power management control(PMC) api services
* 
*/
#ifndef  __PSP_PMC_EH1_H__
#define  __PSP_PMC_EH1_H__

/**
* include files
*/

/**
* definitions
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
* macros
*/

/**
* APIs
*/

/**
* @brief Initiate core halt (i.e., transition to Halted (pmu/fw-halt, C3) state)
*
* @param none
*
* @return none
*/
void pspPmcHalt(void);

/**
* @brief The following function temporarily stop the core from executing instructions for given number of core clock cycles(ticks)
*
* @ticks - Number of core clock cycles
*
* @return none
*/
void pspPmcStall(u32_t uiTicks);

#endif /* __PSP_PMC_EH1_H__ */
