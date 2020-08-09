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

/**
* macros
*/

/**
* types
*/


/*
 * Performance monitoring events
*/
typedef enum pspPerformanceMonitorEvents
{
   /* Event 0 Reserved */
   E_CYCLES_CLOCKS_ACTIVE                     = 1,
   E_I_CACHE_HITS                             = 2,
   E_I_CACHE_MISSES                           = 3,
   E_INSTR_COMMITTED_ALL                      = 4,
   E_INSTR_COMMITTED_16BIT                    = 5,
   E_INSTR_COMMITTED_32BIT                    = 6,
   E_INSTR_ALLIGNED_ALL                       = 7,
   E_INSTR_DECODED_ALL                        = 8,
   E_MULS_COMMITTED                           = 9,
   E_DIVS_COMMITTED                           = 10,
   E_LOADS_COMMITED                           = 11,
   E_STORES_COMMITTED                         = 12,
   E_MISALIGNED_LOADS                         = 13,
   E_MISALIGNED_STORES                        = 14,
   E_ALUS_COMMITTED                           = 15,
   E_CSR_READ                                 = 16,
   E_CSR_READ_WRITE                           = 17,
   E_WRITE_RD_0                               = 18,
   E_EBREAK                                   = 19,
   E_ECALL                                    = 20,
   E_FENCE                                    = 21,
   E_FENCE_I                                  = 22,
   E_MRET                                     = 23,
   E_BRANCHES_COMMITTED                       = 24,
   E_BRANCHES_MISPREDICTED                    = 25,
   E_BRANCHES_TAKEN                           = 26,
   E_UNPREDICTABLE_BRANCHES                   = 27,
   E_CYCLES_FETCH_STALLED                     = 28,
   E_CYCLES_ALIGNER_STALLED                   = 29,
   E_CYCLE_DECODE_STALLED                     = 30,
   E_CYCLE_POSTSYNC_STALLED                   = 31,
   E_CYCLE_PRESYNC_STALLED                    = 32,
   E_CYCLE_FROZEN                             = 33,
   E_CYCLES_SB_WB_STALLED                     = 34,
   E_CYCLES_DMA_DCCM_TRANSACTION_STALLED      = 35,
   E_CYCLES_DMA_ICCM_TRANSACTION_STALLED      = 36,
   E_EXCEPTIONS_TAKEN                         = 37,
   E_TITMER_INTERRUPTS_TAKEN                  = 38,
   E_EXTERNAL_INTERRUPTS_TAKEN                = 39,
   E_TLU_FLUSHES                              = 40,
   E_BRANCH_FLUSHES                           = 41,
   E_I_BUS_TRANSACTIONS_INSTR                 = 42,
   E_D_BUD_TRANSACTIONS_LD_ST                 = 43,
   E_D_BUS_TRANSACTIONS_MISALIGNED            = 44,
   E_I_BUS_ERRORS                             = 45,
   E_D_BUS_ERRORS                             = 46,
   E_CYCLES_STALLED_DUE_TO_I_BUS_BUSY         = 47,
   E_CYCLES_STALLED_DUE_TO_D_BUS_BUSY         = 48,
   E_CYCLES_INTERRUPTS_DISABLED               = 49,
   E_CYCLES_INTERRUPTS_STALLED_WHILE_DISABLED = 50,
   E_END
} ePspPerformanceMonitorEvents_t;


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
* @brief The function enable/disable the group performance monitor
*
* @param uiMonitorEn           – monitor enable disable
*
* @return No return value
*/
void pspEnableAllPerformanceMonitor(u32_t uiMonitorEn);

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
void pspPerformanceCounterSet(u32_t eCounter, ePspPerformanceMonitorEvents_t eEvent);

/**
* @brief The function gets the counter value
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
u32_t pspPerformanceCounterGet(u32_t eCounter);

#endif /* _PSP_PERFORMANCE_MONITOR_H_ */
