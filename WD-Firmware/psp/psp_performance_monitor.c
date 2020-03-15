/* 
* Copyright (c) 2010-2016 Western Digital, Inc.
*
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
* @file
* @author
* @date
* @brief
* 
*/

/**
* include files
*/
#include "psp_macros.h"
#include "psp_api.h"
#include "psp_csrs_swerv_eh1.h"
#include "psp_timers.h"
#include "psp_performance_monitor.h"

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
* @param uiMonitorEn           – monitor enable/disable
*
* @return No return value
*/
void pspEnableAllPerformanceMonitor(u32_t uiMonitorEn)
{
	M_PSP_SET_CSR(D_PSP_MGPMC_NUM, uiMonitorEn & D_PSP_MGMPC_MASK);
}


/**
* @brief The function enables/disable the performance counters
*
* @param uiCountersEn     – counters to enable disable
*
* @return No return value
*/
void pspEnablePerfCounters(u32_t uiCountersEn)
{
	M_PSP_SET_CSR(D_PSP_MCOUNTEREN_NUM, uiCountersEn & D_PSP_MCOUNTEREN_MASK);
}

/**
* @brief The function pair a counter to an event
*
* @param tCounter     – counter to be set
* @param tEvent       – event to be paired to the selected counter
*
* @return No return value
*/
void pspPerfCounterSet(pspPerfMntrCounter_t tCounter, pspPerfMntrEvents_t tEvent)
{
	switch (tCounter)
		{
		case E_COUNTER0:
				M_PSP_WRITE_CSR(D_PSP_MHPMEVENT3_NUM, tEvent); // 0x323
				break;
		case E_COUNTER1:
				M_PSP_WRITE_CSR(D_PSP_MHPMEVENT4_NUM, tEvent); // 0x324
				break;
		case E_COUNTER2:
				M_PSP_WRITE_CSR(D_PSP_MHPMEVENT5_NUM, tEvent); // 0x325
				break;
		case E_COUNTER3:
				M_PSP_WRITE_CSR(D_PSP_MHPMEVENT6_NUM, tEvent); // 0x326
				break;
		}
}
/**
* @brief The function gets the counter value
*
* @param tCounter    – counter index
*
* @return u32_t      – Counter value
*/
u64_t pspPerfCounterGet(pspPerfMntrCounter_t tCounter)
{
	u64_t tCounterVal = 0;
	switch (tCounter)
	{
		case E_CYCLE_COUNTER:
			tCounterVal = M_PSP_READ_CSR(D_PSP_MCYCLE_NUM); // 0xB00
			break;
		case E_TIME_COUNTER:
			tCounterVal = pspTimerCounterGet();
			break;
		case E_INSTRET_COUNTER:
			tCounterVal = M_PSP_READ_CSR(D_PSP_MINSTRET_NUM); // 0xB02
			break;
		case E_COUNTER0:
			tCounterVal = M_PSP_READ_CSR(D_PSP_MHPMCOUNTER3_NUM); // 0xB03
			break;
		case E_COUNTER1:
			tCounterVal = M_PSP_READ_CSR(D_PSP_MHPMCOUNTER4_NUM); // 0xB04
			break;
		case E_COUNTER2:
			tCounterVal = M_PSP_READ_CSR(D_PSP_MHPMCOUNTER5_NUM); // 0xB06
			break;
		case E_COUNTER3:
			tCounterVal = M_PSP_READ_CSR(D_PSP_MHPMCOUNTER6_NUM); // 0xB06
			break;
		case E_TIME_CMP_COUNTER:
			tCounterVal = pspTimeCompareCounterGet();
			break;
	}
	return tCounterVal;
}

