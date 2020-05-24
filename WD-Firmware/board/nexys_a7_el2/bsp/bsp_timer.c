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
* @file   bsp_timer.c
* @author Nati Rapaport
* @date   20.04.2020
* @brief  Timer in Nexys_A7 SweRVolf FPGA (used for tests purpose to generate NMI or External interrupt)
*/

/**
* include files
*/
#include "psp_api.h"
#include "bsp_external_interrupts.h"
#include "bsp_timer.h"

/**
* definitions
*/
/* Specified RAM address for timer duration setup (SweRVolf special implementation) */
#if (0 != D_TIMER_DURATION_SETUP_ADDRESS)
    #define D_BSP_TIMER_DURATION_SETUP_REGISTER   D_TIMER_DURATION_SETUP_ADDRESS
#else
    #error "Timer setup address is not defined"
#endif

/* Specified RAM address for timer activation (SweRVolf special implementation) */
#if (0 != D_TIMER_ACTIVATION_ADDRESS)
    #define D_BSP_TIMER_ACTIVATION_REGISTER   D_TIMER_ACTIVATION_ADDRESS
#else
    #error "Timer activation address is not defined"
#endif

#define D_TIMER_START   1
#define D_TIMER_STOP    0
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
* macros
*/
/* SweRVolf EH1 rate is 50Mhz, EL2 = 25Mhz. Convert duration (in MSec) to number of timer cycles */
#define M_MSEC_DURATION_TO_CYCLES(duration)   (duration * (D_CLOCK_RATE / D_PSP_MSEC))

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
void bspRoutTimer(eTimerRouting_t eTimerRouting)
{
  u32_t uiRoutingValue = M_PSP_READ_REGISTER_32(D_BSP_EXT_INTS_GENERATION_REGISTER);

  switch (eTimerRouting)
  {
  case E_TIMER_TO_NMI:
    uiRoutingValue &= ~((1 << D_BSP_ROUT_TIMER_TO_IRQ3)|(1 << D_BSP_ROUT_TIMER_TO_IRQ4));
    break;
  case E_TIMER_TO_IRQ3:
    uiRoutingValue |= (1 << D_BSP_ROUT_TIMER_TO_IRQ3);
    break;
  case E_TIMER_TO_IRQ4:
    uiRoutingValue |= (1 << D_BSP_ROUT_TIMER_TO_IRQ4);
    break;
    default:
      break;
  }

  /* Set the routing selection */
  M_PSP_SET_REGISTER_32(D_BSP_EXT_INTS_GENERATION_REGISTER, uiRoutingValue);
}

/**
* Setting up the timer duration on SweRVolf FPGA
*
* @param uiTimerDuration - Timer duration in mili-seconds
*/
void bspSetTimerDurationMsec(u32_t uiTimerDurationMsec)
{
  /* Convert duration to timer cycles */
  u32_t uiTimerSet = M_MSEC_DURATION_TO_CYCLES(uiTimerDurationMsec);

  M_PSP_WRITE_REGISTER_32(D_BSP_TIMER_DURATION_SETUP_REGISTER, uiTimerSet);
}

/**
* Start the timer on SweRVolf FPGA
*
*/
void bspStartTimer(void)
{
  M_PSP_WRITE_REGISTER_32(D_BSP_TIMER_ACTIVATION_REGISTER, D_TIMER_START);
}

/**
* Stop the timer on SweRVolf FPGA
*
*/
void bspStopTimer(void)
{
  M_PSP_WRITE_REGISTER_32(D_BSP_TIMER_ACTIVATION_REGISTER, D_TIMER_STOP);
}

