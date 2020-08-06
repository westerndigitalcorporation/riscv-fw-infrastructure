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
* @file   psp_timers_hifive1.h
* @author Nati Rapaport
* @date   28.01.2020
* @brief  The file defines timer-counter api services for HiFive1 core.
* 
*/
#ifndef  __PSP_TIMERS_HIFIVE1_H__
#define  __PSP_TIMERS_HIFIVE1_H__

/**
* include files
*/

/**
* definitions
*/

/**
* types
*/
typedef enum pspTimers
{
   E_MACHINE_TIMER  = 0,
   E_LAST_TIMER
} ePspTimers_t;

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
* @brief Setup and activate core's Timer
*
* @parameter - timer            - indicates which timer to setup and run
* @parameter - uiPeriodMseconds - defines the timer's period in mSeconds
*
***************************************************************************************************/
void pspTimerCounterSetupAndRun(u32_t uiTimer, u32_t uiPeriodMseconds);

/**
* @brief Get Timer counter value
*
* @parameter - timer - indicates which timer to setup and run
*
* @return u64_t      - Timer counter value
*
***************************************************************************************************/
u64_t pspTimerCounterGet(u32_t uiTimer);

/**
* @brief Get Time compare counter value
*
* @parameter - timer - indicates which timer to setup and run
*
* @return u64_t      – Time compare counter value
*
***************************************************************************************************/
u64_t pspTimeCompareCounterGet(u32_t uiTimer);

#endif /* __PSP_TIMERS_HIFIVE1_H__ */
