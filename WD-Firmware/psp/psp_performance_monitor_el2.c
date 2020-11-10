/* 
* Copyright (c) 2010-2016 Western Digital, Inc.
*
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
* @file    psp_performance_monitor_el2.c
* @author  Nati Rapaport
* @date    20.08.2020
* @brief   performance monitor service provider for SweRV EL2
*          comply with The RISC-V Instruction Set Manual Volume II: Privileged Architecture 20190405-Priv-MSU-Ratification
* 
*/

/**
* include files
*/
#include "psp_api.h"

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

/**
* global variables
*/

/**
* APIs
*/

/**
* @brief The function disables all the performance monitors
*        ** Note ** Only Performance-Monitor counters 3..6 are disabled by this setting.
*                   The instruction-retired, cycles and time counters stay enabled.
*
*/
D_PSP_TEXT_SECTION void pspMachinePerfMonitorDisableAll(void)
{
  /* In MCOUNTINHIBIT CSR, '1' means counter is disabled */
  M_PSP_SET_CSR(D_PSP_MCOUNTINHIBIT_NUM, D_PSP_CYCLE_COUNTER|D_PSP_INSTRET_COUNTER|D_PSP_COUNTER0|D_PSP_COUNTER1|D_PSP_COUNTER2|D_PSP_COUNTER3);
}

/**
* @brief The function enables all the performance monitors
*
*/
D_PSP_TEXT_SECTION void pspMachinePerfMonitorEnableAll(void)
{
  /* In MCOUNTINHIBIT CSR, '0' means counter is enabled */
  M_PSP_CLEAR_CSR(D_PSP_MCOUNTINHIBIT_NUM, D_PSP_CYCLE_COUNTER|D_PSP_INSTRET_COUNTER|D_PSP_COUNTER0|D_PSP_COUNTER1|D_PSP_COUNTER2|D_PSP_COUNTER3);
}

/**
* @brief The function disable a specific performance monitor counter
*
* @param uiPerfMonCounter- Performance-Monitor counter to disable/enable
*                          supported counters to enable/disbale are:
*                             D_PSP_CYCLE_COUNTER
*                             D_PSP_INSTRET_COUNTER
*                             D_PSP_COUNTER0
*                             D_PSP_COUNTER1
*                             D_PSP_COUNTER2
*                             D_PSP_COUNTER3
*/
D_PSP_TEXT_SECTION void pspMachinePerfMonitorDisableCounter(u32_t uiPerfMonCounter)
{
  /* In MCOUNTINHIBIT CSR, '1' means counter is disabled */
  M_PSP_SET_CSR(D_PSP_MCOUNTINHIBIT_NUM, uiPerfMonCounter);
}

/**
* @brief The function enable a specific performance monitor counter
*
* @param uiPerfMonCounter- Performance-Monitor counter to disable/enable
*                          supported counters to enable/disbale are:
*                             D_PSP_CYCLE_COUNTER
*                             D_PSP_INSTRET_COUNTER
*                             D_PSP_COUNTER0
*                             D_PSP_COUNTER1
*                             D_PSP_COUNTER2
*                             D_PSP_COUNTER3
*/
D_PSP_TEXT_SECTION void pspMachinePerfMonitorEnableCounter(u32_t uiPerfMonCounter)
{
  /* In MCOUNTINHIBIT CSR, '0' means counter is enabled */
  M_PSP_CLEAR_CSR(D_PSP_MCOUNTINHIBIT_NUM, uiPerfMonCounter);
}

/**
* @brief The function pair a counter to an event
*
* @param uiCounter    – counter to be set
*                     – supported counters are:
*                         D_PSP_COUNTER0
*                         D_PSP_COUNTER1
*                         D_PSP_COUNTER2
*                         D_PSP_COUNTER3
* @param uiEvent       – event to be paired to the selected counter
*
* @return No return value
*/
D_PSP_TEXT_SECTION void pspMachinePerfCounterSet(u32_t uiCounter, u32_t uiEvent)
{
  switch (uiCounter)
  {
    case D_PSP_COUNTER0:
        M_PSP_WRITE_CSR(D_PSP_MHPMEVENT3_NUM, uiEvent);
        break;
    case D_PSP_COUNTER1:
        M_PSP_WRITE_CSR(D_PSP_MHPMEVENT4_NUM, uiEvent);
        break;
    case D_PSP_COUNTER2:
        M_PSP_WRITE_CSR(D_PSP_MHPMEVENT5_NUM, uiEvent);
        break;
    case D_PSP_COUNTER3:
        M_PSP_WRITE_CSR(D_PSP_MHPMEVENT6_NUM, uiEvent);
        break;
    default:
        M_PSP_ASSERT(1);
        break;
  }
}

/**
* @brief The function gets the counter value (64 bit)
*
* @param uiCounter    – counter index
*                     – supported counters are:
*                         D_PSP_CYCLE_COUNTER
*                         D_PSP_TIME_COUNTER
*                         D_PSP_INSTRET_COUNTER
*                         D_PSP_COUNTER0
*                         D_PSP_COUNTER1
*                         D_PSP_COUNTER2
*                         D_PSP_COUNTER3
*
* @return u64_t      – Counter value
*/
D_PSP_TEXT_SECTION u64_t pspMachinePerfCounterGet(u32_t uiCounter)
{
  u64_t uiCounterVal = 0xDEAFBEEFDEAFBEEF;

  switch (uiCounter)
  {
    case D_PSP_CYCLE_COUNTER:
      uiCounterVal  = (u64_t)M_PSP_READ_CSR(D_PSP_MCYCLE_NUM);                    /* read low 32 bits */
      uiCounterVal |= (u64_t)M_PSP_READ_CSR(D_PSP_MCYCLEH_NUM) << D_PSP_SHIFT_32; /* read high 32 bits */
      break;
    case D_PSP_TIME_COUNTER:
      uiCounterVal = pspMachineTimerCounterGet(D_PSP_MACHINE_TIMER);
      break;
    case D_PSP_INSTRET_COUNTER:
      uiCounterVal  = (u64_t)M_PSP_READ_CSR(D_PSP_MINSTRET_NUM);                    /* read low 32 bits */
      uiCounterVal |= (u64_t)M_PSP_READ_CSR(D_PSP_MINSTRETH_NUM) << D_PSP_SHIFT_32; /* read high 32 bits */
      break;
    case D_PSP_COUNTER0:
      uiCounterVal  = (u64_t)M_PSP_READ_CSR(D_PSP_MHPMCOUNTER3_NUM);                    /* read low 32 bits */
      uiCounterVal |= (u64_t)M_PSP_READ_CSR(D_PSP_MHPMCOUNTER3H_NUM) << D_PSP_SHIFT_32; /* read high 32 bits */
      break;
    case D_PSP_COUNTER1:
      uiCounterVal  = (u64_t)M_PSP_READ_CSR(D_PSP_MHPMCOUNTER4_NUM);                    /* read low 32 bits */
      uiCounterVal |= (u64_t)M_PSP_READ_CSR(D_PSP_MHPMCOUNTER4H_NUM) << D_PSP_SHIFT_32; /* read high 32 bits */
      break;
    case D_PSP_COUNTER2:
      uiCounterVal  = (u64_t)M_PSP_READ_CSR(D_PSP_MHPMCOUNTER5_NUM);                    /* read low 32 bits */
      uiCounterVal |= (u64_t)M_PSP_READ_CSR(D_PSP_MHPMCOUNTER5H_NUM) << D_PSP_SHIFT_32; /* read high 32 bits */
      break;
    case D_PSP_COUNTER3:
      uiCounterVal  = (u64_t)M_PSP_READ_CSR(D_PSP_MHPMCOUNTER6_NUM);                    /* read low 32 bits */
      uiCounterVal |= (u64_t)M_PSP_READ_CSR(D_PSP_MHPMCOUNTER6H_NUM) << D_PSP_SHIFT_32; /* read high 32 bits */
      break;
    default:
      M_PSP_ASSERT(1);
      break;
  }
  return uiCounterVal;
}
