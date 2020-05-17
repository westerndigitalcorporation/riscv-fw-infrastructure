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
* @file   bsp_timer.h
* @author Nati Rapaport
* @date   20.04.2020
* @brief  Timer in Nexys_A7 SweRVolf FPGA (used for tests purpose to generate NMI or External interrupt)
*/
#ifndef  __BSP_TIMER_H__
#define  __BSP_TIMER_H__

/**
* include files
*/

/**
* definitions
*/


/**
* types
*/
/* Options for timer routing - i.e. upon timer expiration, NMI-pin/IRQ3/IRQ4 will be asserted */
typedef enum timerRouting
{
  E_TIMER_TO_NMI        = 0,
  E_TIMER_TO_IRQ3       = 1,
  E_TIMER_TO_IRQ4       = 2,
}eTimerRouting_t;

/**
* local prototypes
*/

/**
* external prototypes
*/


/**
* macros
*/


/**
* global variables
*/

/**
* APIs
*/

/**
* Setup the routing of the timer of SweRVolf FPGA
*
* @param eTimerRouting - whether timer expiration will be routed to NMI-pin, IRQ3 or IRQ4
*/
void bspRoutTimer(eTimerRouting_t eTimerRouting);

/**
* Setting up the timer duration on SweRVolf FPGA
*
* @param uiTimerDuration - Timer duration in mili-seconds
*/
void bspSetTimerDurationMsec(u32_t uiTimerDurationMsec);

/**
* Start the timer on SweRVolf FPGA
*
*/
void bspStartTimer(void);

/**
* Stop the timer on SweRVolf FPGA
*
*/
void bspStopTimer(void);

#endif /* __BSP_TIMER_H__ */
