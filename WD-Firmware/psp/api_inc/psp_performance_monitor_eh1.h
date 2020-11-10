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
* @file    psp_performance_monitor.h
* @author  Nidal Faour
* @date    15.03.2020
* @brief   performance monitor api provider
*
*/

#ifndef _PSP_PERFORMANCE_MONITOR_H_
#define _PSP_PERFORMANCE_MONITOR_H_

/**
* include files
*/

/**
* definitions
*/
#define D_PSP_CYCLE_COUNTER     M_PSP_BIT_MASK(0)
#define D_PSP_TIME_COUNTER      M_PSP_BIT_MASK(1)
#define D_PSP_INSTRET_COUNTER   M_PSP_BIT_MASK(2)
#define D_PSP_COUNTER0          M_PSP_BIT_MASK(3)
#define D_PSP_COUNTER1          M_PSP_BIT_MASK(4)
#define D_PSP_COUNTER2          M_PSP_BIT_MASK(5)
#define D_PSP_COUNTER3          M_PSP_BIT_MASK(6)

/*
 * Performance monitoring events
*/
/* Event 0 is Reserved */
#define   D_CYCLES_CLOCKS_ACTIVE                     1
#define   D_I_CACHE_HITS                             2
#define   D_I_CACHE_MISSES                           3
#define   D_INSTR_COMMITTED_ALL                      4
#define   D_INSTR_COMMITTED_16BIT                    5
#define   D_INSTR_COMMITTED_32BIT                    6
#define   D_INSTR_ALLIGNED_ALL                       7
#define   D_INSTR_DECODED_ALL                        8
#define   D_MULS_COMMITTED                           9
#define   D_DIVS_COMMITTED                           10
#define   D_LOADS_COMMITED                           11
#define   D_STORES_COMMITTED                         12
#define   D_MISALIGNED_LOADS                         13
#define   D_MISALIGNED_STORES                        14
#define   D_ALUS_COMMITTED                           15
#define   D_CSR_READ                                 16
#define   D_CSR_READ_WRITE                           17
#define   D_WRITE_RD_0                               18
#define   D_EBREAK                                   19
#define   D_ECALL                                    20
#define   D_FENCE                                    21
#define   D_FENCE_I                                  22
#define   D_MRET                                     23
#define   D_BRANCHES_COMMITTED                       24
#define   D_BRANCHES_MISPREDICTED                    25
#define   D_BRANCHES_TAKEN                           26
#define   D_UNPREDICTABLE_BRANCHES                   27
#define   D_CYCLES_FETCH_STALLED                     28
#define   D_CYCLES_ALIGNER_STALLED                   29
#define   D_CYCLE_DECODE_STALLED                     30
#define   D_CYCLE_POSTSYNC_STALLED                   31
#define   D_CYCLE_PRESYNC_STALLED                    32
#define   D_CYCLE_FROZEN                             33
#define   D_CYCLES_SB_WB_STALLED                     34
#define   D_CYCLES_DMA_DCCM_TRANSACTION_STALLED      35
#define   D_CYCLES_DMA_ICCM_TRANSACTION_STALLED      36
#define   D_EXCEPTIONS_TAKEN                         37
#define   D_TIMER_INTERRUPTS_TAKEN                   38
#define   D_EXTERNAL_INTERRUPTS_TAKEN                39
#define   D_TLU_FLUSHES                              40
#define   D_BRANCH_FLUSHES                           41
#define   D_I_BUS_TRANSACTIONS_INSTR                 42
#define   D_D_BUD_TRANSACTIONS_LD_ST                 43
#define   D_D_BUS_TRANSACTIONS_MISALIGNED            44
#define   D_I_BUS_ERRORS                             45
#define   D_D_BUS_ERRORS                             46
#define   D_CYCLES_STALLED_DUE_TO_I_BUS_BUSY         47
#define   D_CYCLES_STALLED_DUE_TO_D_BUS_BUSY         48
#define   D_CYCLES_INTERRUPTS_DISABLED               49
#define   D_CYCLES_INTERRUPTS_STALLED_WHILE_DISABLED 50

/**
* macros
*/

/**
* types
*/

/*
 * Performance monitoring events
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
void pspMachinePerfMonitorDisableAll(void);

/**
* @brief The function enables all the performance monitors
*
*/
void pspMachinePerfMonitorEnableAll(void);

/**
* @brief The function pair a counter to an event
*
* @param uiCounter    – counter to be set
*                     – supported counters are:
*                         D_PSP_COUNTER0
*                         D_PSP_COUNTER1
*                         D_PSP_COUNTER2
*                         D_PSP_COUNTER3
* @param eEvent       – event to be paired to the selected counter
*
* @return No return value
*/
void pspMachinePerfCounterSet(u32_t uiCounter, u32_t uiEvent);

/**
* @brief The function gets the counter value (64 bit)
*
* @param eCounter     – counter index
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
u64_t pspMachinePerfCounterGet(u32_t uiCounter);

#endif /* _PSP_PERFORMANCE_MONITOR_H_ */
