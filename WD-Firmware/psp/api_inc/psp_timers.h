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
* @file   psp_timers.h
* @author Nati Rapaport
* @date   28.01.2020
* @brief  The file defines timer-counter api services
* 
*/
#ifndef  __PSP_TIMER_COUNTERS_H__
#define  __PSP_TIMER_COUNTERS_H__

/**
* include files
*/

/**
* definitions
*/
#define D_PSP_CORE_TIMER 0


/**
* types
*/

/**
* local prototypes
*/

/**
* external prototypes
*/
extern void (*fptrPspTimerCounterActivate)(u32_t uiTimer, u32_t uiPeriod);

/**
* global variables
*/

/**
* macros
*/
#define M_PSP_TIMER_COUNTER_ACTIVATE(timer, period)   fptrPspTimerCounterActivate(timer, period)

/**
* APIs
*/

#endif /* __PSP_TIMER_COUNTERS_H__ */
