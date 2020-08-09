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
* @file    psp_performance_monitor.c
* @author  Nidal Faour
* @date    15.03.2020
* @brief   performance monitor service provider
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
* @brief The function enable/disable the group performance monitor
*
* @param uiMonitorEnBit           – monitor enable/disable bit
*
* @return No return value
*/
D_PSP_TEXT_SECTION void pspEnableAllPerformanceMonitor(u32_t uiMonitorEnBit)
{
  M_PSP_SET_CSR(D_PSP_MGPMC_NUM, uiMonitorEnBit & D_PSP_MGMPC_MASK);
}


/**
* @brief The function pair a counter to an event
*
* @param eCounter     – counter to be set
*                     – supported counters are:
*                         D_PSP_COUNTER0
*                         D_PSP_COUNTER1
*                         D_PSP_COUNTER2
*                         D_PSP_COUNTER3
* @param eEvent       – event to be paired to the selected counter
*
* @return No return value
*/
D_PSP_TEXT_SECTION void pspPerformanceCounterSet(u32_t eCounter, ePspPerformanceMonitorEvents_t eEvent)
{
  switch (eCounter)
  {
    case D_PSP_COUNTER0:
        M_PSP_WRITE_CSR(D_PSP_MHPMEVENT3_NUM, eEvent);
        break;
    case D_PSP_COUNTER1:
        M_PSP_WRITE_CSR(D_PSP_MHPMEVENT4_NUM, eEvent);
        break;
    case D_PSP_COUNTER2:
        M_PSP_WRITE_CSR(D_PSP_MHPMEVENT5_NUM, eEvent);
        break;
    case D_PSP_COUNTER3:
        M_PSP_WRITE_CSR(D_PSP_MHPMEVENT6_NUM, eEvent);
        break;
    default:
        M_PSP_ASSERT(1);
        break;
  }
}
/**
* @brief The function gets the counter value
*
* @param eCounter    – counter index
*                     – supported counters are:
*                         D_PSP_CYCLE_COUNTER
*                         D_PSP_TIME_COUNTER
*                         D_PSP_INSTRET_COUNTER
*                         D_PSP_COUNTER0
*                         D_PSP_COUNTER1
*                         D_PSP_COUNTER2
*                         D_PSP_COUNTER3
*
* @return u32_t      – Counter value
*/
D_PSP_TEXT_SECTION u32_t pspPerformanceCounterGet(u32_t eCounter)
{
  u32_t uiCounterVal = 0;
  switch (eCounter)
  {
    case D_PSP_CYCLE_COUNTER:
      uiCounterVal = M_PSP_READ_CSR(D_PSP_MCYCLE_NUM);
      break;
    case D_PSP_TIME_COUNTER:
      uiCounterVal = M_PSP_READ_CSR(D_PSP_TIME_NUM);
      break;
    case D_PSP_INSTRET_COUNTER:
      uiCounterVal = M_PSP_READ_CSR(D_PSP_MINSTRET_NUM);
      break;
    case D_PSP_COUNTER0:
      uiCounterVal = M_PSP_READ_CSR(D_PSP_MHPMCOUNTER3_NUM);
      break;
    case D_PSP_COUNTER1:
      uiCounterVal = M_PSP_READ_CSR(D_PSP_MHPMCOUNTER4_NUM);
      break;
    case D_PSP_COUNTER2:
      uiCounterVal = M_PSP_READ_CSR(D_PSP_MHPMCOUNTER5_NUM);
      break;
    case D_PSP_COUNTER3:
      uiCounterVal = M_PSP_READ_CSR(D_PSP_MHPMCOUNTER6_NUM);
      break;
    default:
      M_PSP_ASSERT(1);
      break;
  }
  return uiCounterVal;
}

