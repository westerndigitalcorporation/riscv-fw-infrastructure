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
* @file   psp_defines.h
* @author Ronen Haen
* @date   20.05.2019
* @brief  The psp defines
* 
*/
#ifndef  __PSP_DEFINES_H__
#define  __PSP_DEFINES_H__

/**
* include files
*/

/**
* definitions
*/

/* there are 1000 msec in one second */
#define D_MSEC 1000

/* interrupt context indication values */
#define D_PSP_NON_INT_CONTEXT              0
#define D_PSP_INT_CONTEXT                  1

/* function return codes */
#define D_PSP_SUCCESS                      0x00
#define D_PSP_FAIL                         0x01
#define D_PSP_PTR_ERROR                    0x02

#define D_PSP_TRUE                         1
#define D_PSP_FALSE                        0



/* Definitions for the various fields in the CSRs */

#define D_MSTATUS_UIE         0x00000001
#define D_MSTATUS_SIE         0x00000002
#define D_MSTATUS_HIE         0x00000004
#define D_MSTATUS_MIE         0x00000008
#define D_MSTATUS_UPIE        0x00000010
#define D_MSTATUS_SPIE        0x00000020
#define D_MSTATUS_HPIE        0x00000040
#define D_MSTATUS_MPIE        0x00000080
#define D_MSTATUS_SPP         0x00000100
#define D_MSTATUS_HPP         0x00000600
#define D_MSTATUS_MPP         0x00001800
#define D_MSTATUS_FS          0x00006000
#define D_MSTATUS_XS          0x00018000
#define D_MSTATUS_MPRV        0x00020000
#define D_MSTATUS_PUM         0x00040000
#define D_MSTATUS_MXR         0x00080000
#define D_MSTATUS_VM          0x1F000000
#define D_MSTATUS32_SD        0x80000000
#define D_MSTATUS64_SD        0x8000000000000000

#define D_SSTATUS_UIE         0x00000001
#define D_SSTATUS_SIE         0x00000002
#define D_SSTATUS_UPIE        0x00000010
#define D_SSTATUS_SPIE        0x00000020
#define D_SSTATUS_SPP         0x00000100
#define D_SSTATUS_FS          0x00006000
#define D_SSTATUS_XS          0x00018000
#define D_SSTATUS_PUM         0x00040000
#define D_SSTATUS32_SD        0x80000000
#define D_SSTATUS64_SD        0x8000000000000000

#define D_CSR_XDEBUGVER      (3U<<30)
#define D_CSR_NDRESET        (1<<29)
#define D_CSR_FULLRESET      (1<<28)
#define D_CSR_EBREAKM        (1<<15)
#define D_CSR_EBREAKH        (1<<14)
#define D_CSR_EBREAKS        (1<<13)
#define D_CSR_EBREAKU        (1<<12)
#define D_CSR_STOPCYCLE      (1<<10)
#define D_CSR_STOPTIME       (1<<9)
#define D_CSR_CAUSE          (7<<6)
#define D_CSR_DEBUGINT       (1<<5)
#define D_CSR_HALT           (1<<3)
#define D_CSR_STEP           (1<<2)
#define D_CSR_PRV            (3<<0)

#define D_CSR_CAUSE_NONE     0
#define D_CSR_CAUSE_SWBP     1
#define D_CSR_CAUSE_HWBP     2
#define D_CSR_CAUSE_DEBUGINT 3
#define D_CSR_CAUSE_STEP     4
#define D_CSR_CAUSE_HALT     5

#define D_MCONTROL_TYPE(xlen)    (0xfULL<<((xlen)-4))
#define D_MCONTROL_DMODE(xlen)   (1ULL<<((xlen)-5))
#define D_MCONTROL_MASKMAX(xlen) (0x3fULL<<((xlen)-11))

#define D_MCONTROL_SELECT     (1<<19)
#define D_MCONTROL_TIMING     (1<<18)
#define D_MCONTROL_ACTION     (0x3f<<12)
#define D_MCONTROL_CHAIN      (1<<11)
#define D_MCONTROL_MATCH      (0xf<<7)
#define D_MCONTROL_M          (1<<6)
#define D_MCONTROL_H          (1<<5)
#define D_MCONTROL_S          (1<<4)
#define D_MCONTROL_U          (1<<3)
#define D_MCONTROL_EXECUTE    (1<<2)
#define D_MCONTROL_STORE      (1<<1)
#define D_MCONTROL_LOAD       (1<<0)

#define D_MCONTROL_TYPE_NONE      0
#define D_MCONTROL_TYPE_MATCH     2

#define D_MCONTROL_ACTION_DEBUG_EXCEPTION   0
#define D_MCONTROL_ACTION_DEBUG_MODE        1
#define D_MCONTROL_ACTION_TRACE_START       2
#define D_MCONTROL_ACTION_TRACE_STOP        3
#define D_MCONTROL_ACTION_TRACE_EMIT        4

#define D_MCONTROL_MATCH_EQUAL     0
#define D_MCONTROL_MATCH_NAPOT     1
#define D_MCONTROL_MATCH_GE        2
#define D_MCONTROL_MATCH_LT        3
#define D_MCONTROL_MATCH_MASK_LOW  4
#define D_MCONTROL_MATCH_MASK_HIGH 5


#define D_IRQ_S_SOFT   1
#define D_IRQ_H_SOFT   2
#define D_IRQ_M_SOFT   3
#define D_IRQ_S_TIMER  5
#define D_IRQ_H_TIMER  6
#define D_IRQ_M_TIMER  7
#define D_IRQ_S_EXT    9
#define D_IRQ_H_EXT    10
#define D_IRQ_M_EXT    11
#define D_IRQ_COP      12
#define D_IRQ_HOST     13

#define D_MIP_SSIP            (1 << D_IRQ_S_SOFT)
#define D_MIP_HSIP            (1 << D_IRQ_H_SOFT)
#define D_MIP_MSIP            (1 << D_IRQ_M_SOFT)
#define D_MIP_STIP            (1 << D_IRQ_S_TIMER)
#define D_MIP_HTIP            (1 << D_IRQ_H_TIMER)
#define D_MIP_MTIP            (1 << D_IRQ_M_TIMER)
#define D_MIP_SEIP            (1 << D_IRQ_S_EXT)
#define D_MIP_HEIP            (1 << D_IRQ_H_EXT)
#define D_MIP_MEIP            (1 << D_IRQ_M_EXT)

#define D_MSTATUS_OFFSET      29
#define D_MEPC_OFFSET         0 // ??
// NatiR - other registers too to define



#endif /* __PSP_DEFINES_H__ */
