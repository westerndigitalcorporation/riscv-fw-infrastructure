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
* @file   psp_timers_eh1.h
* @author Nati Rapaport
* @date   05.05.2020
* @brief  EH1 timers
* 
*/
#ifndef  __PSP_TIMERS_EH1_H__
#define  __PSP_TIMERS_EH1_H__

/**
* include files
*/

/**
* definitions
*/
#define D_PSP_INTERNAL_TIMER0 1
#define D_PSP_INTERNAL_TIMER1 2


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
* @brief Enable timer counting when core in sleep mode
*
* @parameter - uiTimer  - indicates which timer to setup
*
***************************************************************************************************/
void pspTimerEnableCountInSleepMode(u32_t uiTimer);

/**
* @brief Disable timer counting when core in sleep mode
*
* @parameter - uiTimer  - indicates which timer to setup
*
***************************************************************************************************/
void pspTimerDisableCountInSleepMode(u32_t uiTimer);

/**
* @brief Enable timer counting when core in in stall
*
* @parameter - uiTimer  - indicates which timer to setup
*
***************************************************************************************************/
void pspTimerEnableCountInStallMode(u32_t uiTimer);

/**
* @brief Disable timer counting when core in in stall
*
* @parameter - uiTimer  - indicates which timer to setup
*
***************************************************************************************************/
void pspTimerDisableCountInStallMode(u32_t uiTimer);


#endif /* __PSP_TIMERS_EH1_H__ */
