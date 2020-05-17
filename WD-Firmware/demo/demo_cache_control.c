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
#include "demo_platform_al.h"
#include "bsp_mem_map.h"
#include "psp_api.h"

/**
* definitions
*/
#define D_MAIN_MEM_INDEX                         0
#define D_DEMO_MAX_LOOP_COUNT                    65536
#define D_DEMO_EXPECTED_TIMER_VAL_WHEN_CACHE_ON  200000
#define D_DEMO_EXPECTED_TIMER_VAL_WHEN_CACHE_OFF 3000000
#define D_DEMO_OLOF_SWERV                        0xC1

/**
* macros
*/
#define M_DEMO_CACHE_CONTROL_CODE_TO_MEASURE() for (uiIndex = 0 ; uiIndex < D_DEMO_MAX_LOOP_COUNT ; uiIndex++) \
                                               { \
                                                  asm volatile ("nop"); \
                                               }

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
   volatile u32_t* pUartState;

   volatile u64_t ulCounter1, ulCounter2, ulCounter3;

   /* Register interrupt vector */
   M_PSP_WRITE_CSR(mtvec, &psp_vect_table);

   /* give us an indication if under whisper or not */
   pUartState = (u32_t*)(D_UART_BASE_ADDRESS+0x8);
   /* is swerv (not whisper) */
   if (*pUartState == D_DEMO_OLOF_SWERV)
   {
      /* clear all mrac bits - disable cache and sideeffect bits */
      for (uiIndex = 0 ; uiIndex < D_CACHE_CONTROL_MAX_NUMBER_OF_REGIONS ; uiIndex++)
      {
         M_PSP_DISABLE_MEM_REGION_ICACHE(uiIndex);
         M_PSP_DISABLE_MEM_REGION_SIDEEFFECT(uiIndex);
      }

      /* Disable Machine-Timer interrupt so we won't get interrupted
         timer interrupt not needed in this demo */
      pspDisableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_TIMER);

      /* Activates Core's timer */
      pspTimerCounterSetupAndRun(E_MACHINE_TIMER,  0xFFFFFFFF);

      /* sample the timer value */
      ulCounter1 = pspTimerCounterGet(E_MACHINE_TIMER);

      /* we disable (again) the cache just to have the same amount
         of measured instructions */
      M_PSP_DISABLE_MEM_REGION_ICACHE(D_MAIN_MEM_INDEX);

      /* execute some code */
      M_DEMO_CACHE_CONTROL_CODE_TO_MEASURE();

      /* sample the timer value */
      ulCounter2 = pspTimerCounterGet(E_MACHINE_TIMER);

      /* enable cache for the main memory so we can measure how much
         time execution takes */
      M_PSP_ENABLE_MEM_REGION_ICACHE(D_MAIN_MEM_INDEX);

      /* execute some code */
      M_DEMO_CACHE_CONTROL_CODE_TO_MEASURE();

      /* sample the timer value */
      ulCounter3 = pspTimerCounterGet(E_MACHINE_TIMER);

      /* verify we are within execution time limits when cache
         is enabled/disabled */
      ulCounter3 -= ulCounter2;
      ulCounter2 -= ulCounter1;
      if ((ulCounter3 > D_DEMO_EXPECTED_TIMER_VAL_WHEN_CACHE_ON) ||
          (ulCounter2 < D_DEMO_EXPECTED_TIMER_VAL_WHEN_CACHE_OFF))
      {
         asm volatile ("ebreak");
      }
   }
   else
   /* whisper */
   {
      printfNexys("This demo can't run under whisper");
   }
}

