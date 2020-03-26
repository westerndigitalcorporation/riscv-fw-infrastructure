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
* @file   demo_cache_control.c
* @author Ronen Haen
* @date   26.03.2020
* @brief  The file implements the cache control demo
*/

/**
* include files
*/
#include "common_types.h"
#include "psp_macros.h"
#include "demo_platform_al.h"
#include "psp_cache_control_eh1.h"
#include "psp_timers.h"

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

extern void psp_vect_table(void);

/**
* global variables
*/

/**
* functions
*/

void demoStart(void)
{
   u32_t uiIndex;
   volatile u64_t ulCounter1, ulCounter2, ulCounter3;

   /* Register interrupt vector */
   M_PSP_WRITE_CSR(mtvec, &psp_vect_table);

   /* Disable Machine-Timer interrupt so we won't get interrupted */
   pspDisableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_TIMER);

   /* Activates Core's timer with the calculated period */
   M_PSP_TIMER_COUNTER_ACTIVATE(D_PSP_CORE_TIMER,  0xFFFFFFFF);

   ulCounter1 = pspTimerCounterGet();

   M_PSP_DISABLE_MEM_REGION_ICACHE(0);

   for (uiIndex = 0 ; uiIndex < 65536 ; uiIndex++)
   {
      asm volatile ("nop");
   }

   ulCounter2 = pspTimerCounterGet();

   M_PSP_ENABLE_MEM_REGION_ICACHE(0);

   for (uiIndex = 0 ; uiIndex < 65536 ; uiIndex++)
   {
      asm volatile ("nop");
   }

   ulCounter3 = pspTimerCounterGet();

   asm volatile ("ebreak");
}

