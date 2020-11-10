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
* @file    psp_performance_monitor_el2.h
* @author  Nati Rapaport
* @date    20.08.2020
* @brief   performance monitor api provider for SweRV EL2
*
*/

#ifndef _PSP_PERFORMANCE_MONITOR_EL2_H_
#define _PSP_PERFORMANCE_MONITOR_EL2_H_

/**
* include files
*/

/**
* definitions
*/
/*
 * Performance monitoring events
*/
#define   D_BITMANIP_COMMITED        54
#define   D_BUS_LOADS_COMMITED       55
#define   D_BUS_STORES_COMMITED      56
#define   D_CYCLES_IN_SLEEP_C3_STATE 512
#define   D_DMA_READS_ALL            513
#define   D_DMA_WRITES_ALL           514
#define   D_DMA_READS_TO_DCCM        515
#define   D_DMA_WRITESS_TO_DCCM      516

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
void pspMachinePerfMonitorDisableCounter(u32_t uiPerfMonCounter);

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
void pspMachinePerfMonitorEnableCounter(u32_t uiPerfMonCounter);

#endif /* _PSP_PERFORMANCE_MONITOR_EL2_H_ */
