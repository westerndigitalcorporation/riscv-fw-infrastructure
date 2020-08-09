/* 
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
* @file   psp_csrs.h
* @author Nati Rapaport
* @date   28.01.2020
*                          ###############################################################
* @brief  CSRs definitions ########## (ACCORDING SPEC RISCV-PRIVILEGED-V1.10) ############
*                          ###############################################################
* 
*/
#ifndef  __PSP_CSRS_H__
#define  __PSP_CSRS_H__

/**
* include files
*/

/**
* definitions
*/

/* Definitions for the CSR addresses */

/* Machine information registers */
#define D_PSP_MVENDOID_NUM    0xF11
#define D_PSP_MARCHID_NUM     0xF12
#define D_PSP_MIMPID_NUM      0xF13
#define D_PSP_MHARTID_NUM     0xF14

/* Machine Trap setup registers */

#define D_PSP_MIDELEG_NUM     0x301
#define D_PSP_MEDELEG_NUM     0x302


/* Machine Trap handling registers */
#define D_PSP_MSCRATCH_NUM    0x340
#define D_PSP_MEPC_NUM        0x341
#define D_PSP_MTVAL_NUM       0x343

/* Machine Protection and Translation registers */
#define D_PSP_PMPCFG0_NUM     0x3A0
#define D_PSP_PMPCFG1_NUM     0x3A1
#define D_PSP_PMPCFG2_NUM     0x3A2
#define D_PSP_PMPCFG3_NUM     0x3A3
#define D_PSP_PMPADDR0_NUM    0x3B0
#define D_PSP_PMPADDR1_NUM    0x3B1
#define D_PSP_PMPADDR2_NUM    0x3B2
#define D_PSP_PMPADDR3_NUM    0x3B3
#define D_PSP_PMPADDR4_NUM    0x3B4
#define D_PSP_PMPADDR5_NUM    0x3B5
#define D_PSP_PMPADDR6_NUM    0x3B6
#define D_PSP_PMPADDR7_NUM    0x3B7
#define D_PSP_PMPADDR8_NUM    0x3B8
#define D_PSP_PMPADDR9_NUM    0x3B9
#define D_PSP_PMPADDR10_NUM   0x3BA
#define D_PSP_PMPADDR11_NUM   0x3BB
#define D_PSP_PMPADDR12_NUM   0x3BC
#define D_PSP_PMPADDR13_NUM   0x3BD
#define D_PSP_PMPADDR14_NUM   0x3BE
#define D_PSP_PMPADDR15_NUM   0x3BF

/* Machine Counter/Timers registers */
#define D_PSP_MCYCLE_NUM           0xB00
#define D_PSP_MINSTRET_NUM         0xB02
#define D_PSP_MHPMCOUNTER3_NUM     0xB03
#define D_PSP_MHPMCOUNTER4_NUM     0xB04
#define D_PSP_MHPMCOUNTER5_NUM     0xB05
#define D_PSP_MHPMCOUNTER6_NUM     0xB06
#define D_PSP_MHPMCOUNTER7_NUM     0xB07
#define D_PSP_MHPMCOUNTER8_NUM     0xB08
#define D_PSP_MHPMCOUNTER9_NUM     0xB09
#define D_PSP_MHPMCOUNTER10_NUM    0xB0A
#define D_PSP_MHPMCOUNTER11_NUM    0xB0B
#define D_PSP_MHPMCOUNTER12_NUM    0xB0C
#define D_PSP_MHPMCOUNTER13_NUM    0xB0D
#define D_PSP_MHPMCOUNTER14_NUM    0xB0E
#define D_PSP_MHPMCOUNTER15_NUM    0xB0F
#define D_PSP_MHPMCOUNTER16_NUM    0xB10
#define D_PSP_MHPMCOUNTER17_NUM    0xB11
#define D_PSP_MHPMCOUNTER18_NUM    0xB12
#define D_PSP_MHPMCOUNTER19_NUM    0xB13
#define D_PSP_MHPMCOUNTER20_NUM    0xB14
#define D_PSP_MHPMCOUNTER21_NUM    0xB15
#define D_PSP_MHPMCOUNTER22_NUM    0xB16
#define D_PSP_MHPMCOUNTER23_NUM    0xB17
#define D_PSP_MHPMCOUNTER24_NUM    0xB18
#define D_PSP_MHPMCOUNTER25_NUM    0xB19
#define D_PSP_MHPMCOUNTER26_NUM    0xB1A
#define D_PSP_MHPMCOUNTER27_NUM    0xB1B
#define D_PSP_MHPMCOUNTER28_NUM    0xB1C
#define D_PSP_MHPMCOUNTER29_NUM    0xB1D
#define D_PSP_MHPMCOUNTER30_NUM    0xB1E
#define D_PSP_MHPMCOUNTER31_NUM    0xB1F
#define D_PSP_TIME_NUM             0xC01
#if __riscv_xlen == 32
    #define D_PSP_MCYCLEH_NUM           0xB80
    #define D_PSP_MINSTRETH_NUM         0xB82
    #define D_PSP_MHPMCOUNTER3H_NUM     0xB83
    #define D_PSP_MHPMCOUNTER4HH_NUM    0xB84
    #define D_PSP_MHPMCOUNTER5H_NUM     0xB85
    #define D_PSP_MHPMCOUNTER6H_NUM     0xB86
    #define D_PSP_MHPMCOUNTER7H_NUM     0xB87
    #define D_PSP_MHPMCOUNTER8H_NUM     0xB88
    #define D_PSP_MHPMCOUNTER9H_NUM     0xB89
    #define D_PSP_MHPMCOUNTER10H_NUM    0xB8A
    #define D_PSP_MHPMCOUNTER11H_NUM    0xB8B
    #define D_PSP_MHPMCOUNTER12H_NUM    0xB8C
    #define D_PSP_MHPMCOUNTER13H_NUM    0xB8D
    #define D_PSP_MHPMCOUNTER14H_NUM    0xB8E
    #define D_PSP_MHPMCOUNTER15H_NUM    0xB8F
    #define D_PSP_MHPMCOUNTER16H_NUM    0xB90
    #define D_PSP_MHPMCOUNTER17H_NUM    0xB91
    #define D_PSP_MHPMCOUNTER18H_NUM    0xB92
    #define D_PSP_MHPMCOUNTER19H_NUM    0xB93
    #define D_PSP_MHPMCOUNTER20H_NUM    0xB94
    #define D_PSP_MHPMCOUNTER21H_NUM    0xB95
    #define D_PSP_MHPMCOUNTER22H_NUM    0xB96
    #define D_PSP_MHPMCOUNTER23H_NUM    0xB97
    #define D_PSP_MHPMCOUNTER24H_NUM    0xB98
    #define D_PSP_MHPMCOUNTER25H_NUM    0xB99
    #define D_PSP_MHPMCOUNTER26H_NUM    0xB9A
    #define D_PSP_MHPMCOUNTER27H_NUM    0xB9B
    #define D_PSP_MHPMCOUNTER28H_NUM    0xB9C
    #define D_PSP_MHPMCOUNTER29H_NUM    0xB9D
    #define D_PSP_MHPMCOUNTER30H_NUM    0xB9E
    #define D_PSP_MHPMCOUNTER31H_NUM    0xB9F
    #define D_PSP_TIMEH_NUM             0xC81
#endif /* __riscv_xlen == 32 */



/* Machine Counter setup registers */
#define D_PSP_MHPMEVENT3_NUM     0x323
#define D_PSP_MHPMEVENT4_NUM     0x324
#define D_PSP_MHPMEVENT5_NUM     0x325
#define D_PSP_MHPMEVENT6_NUM     0x326
#define D_PSP_MHPMEVENT7_NUM     0x327
#define D_PSP_MHPMEVENT8_NUM     0x328
#define D_PSP_MHPMEVENT9_NUM     0x329
#define D_PSP_MHPMEVENT10_NUM    0x32A
#define D_PSP_MHPMEVENT11_NUM    0x32B
#define D_PSP_MHPMEVENT12_NUM    0x32C
#define D_PSP_MHPMEVENT13_NUM    0x32D
#define D_PSP_MHPMEVENT14_NUM    0x32E
#define D_PSP_MHPMEVENT15_NUM    0x32F
#define D_PSP_MHPMEVENT16_NUM    0x330
#define D_PSP_MHPMEVENT17_NUM    0x331
#define D_PSP_MHPMEVENT18_NUM    0x332
#define D_PSP_MHPMEVENT19_NUM    0x333
#define D_PSP_MHPMEVENT20_NUM    0x334
#define D_PSP_MHPMEVENT21_NUM    0x335
#define D_PSP_MHPMEVENT22_NUM    0x336
#define D_PSP_MHPMEVENT23_NUM    0X337
#define D_PSP_MHPMEVENT24_NUM    0x338
#define D_PSP_MHPMEVENT25_NUM    0x339
#define D_PSP_MHPMEVENT26_NUM    0x33A
#define D_PSP_MHPMEVENT27_NUM    0x33B
#define D_PSP_MHPMEVENT28_NUM    0x33C
#define D_PSP_MHPMEVENT29_NUM    0x33D
#define D_PSP_MHPMEVENT30_NUM    0x33E
#define D_PSP_MHPMEVENT31_NUM    0x33F

/* Debug/Trace registers */
#define D_PSP_TSELECT_NUM    0x7A0
#define D_PSP_TDATA1_NUM     0x7A1
#define D_PSP_TDATA2_NUM     0x7A2
#define D_PSP_TDATA3_NUM     0x7A3

/* Debug mode registers */
#define D_PSP_DCSR_NUM       0x7B0
#define D_PSP_DPC_NUM        0x7B1
#define D_PSP_DSCRATCH_NUM   0x7B2





/* Definitions for the various fields in CSRs */

/* counteren CSR */
#define D_PSP_MCOUNTEREN_NUM  0x306
#define D_PSP_MCOUNTEREN_MASK 0x0000007F

/* misa CSR */
#define D_PSP_MISA_NUM             0x301
#define D_PSP_MISA_EXTENSIONS_MASK 0x03FFFFFF
#define D_PSP_MISA_MXL_MASK        0x0C000000

/*  mtvec CSR */
#define D_PSP_MTVEC_NUM            0x305
#define D_PSP_MTVEC_MODE_MASK      0x00000003
#define D_PSP_MTVEC_BASE_MASK      0xFFFFFFFC

/* mstatus CSR */
#define D_PSP_MSTATUS_NUM          0x300
#define D_PSP_MSTATUS_UIE_MASK     0x00000001   /* Interrupts enable/disable */ /* User mode */
#define D_PSP_MSTATUS_SIE_MASK     0x00000002                                   /* Supervisor mode */
#define D_PSP_MSTATUS_MIE_MASK     0x00000008                                   /* Machine mode */
#define D_PSP_MSTATUS_UPIE_MASK    0x00000010
#define D_PSP_MSTATUS_SPIE_MASK    0x00000020
#define D_PSP_MSTATUS_MPIE_MASK    0x00000080
#define D_PSP_MSTATUS_SPP_MASK     0x00000100
#define D_PSP_MSTATUS_MPP_MASK     0x00001800
#define D_PSP_MSTATUS_FS_MASK      0x00006000
#define D_PSP_MSTATUS_XS_MASK      0x00018000
#define D_PSP_MSTATUS_MPRV_MASK    0x00020000
#define D_PSP_MSTATUS_SUM_MASK     0x00040000
#define D_PSP_MSTATUS_MXR_MASK     0x00080000
#define D_PSP_MSTATUS_TVM_MASK     0x00100000
#define D_PSP_MSTATUS_TW_MASK      0x00200000
#define D_PSP_MSTATUS_TSR_MASK     0x00400000
#define D_PSP_MSTATUS_UXL_MASK     0x0000000300000000
#define D_PSP_MSTATUS_SXL_MASK     0x0000000C00000000
#define D_PSP_MSTATUS32_SD_MASK    0x80000000
#define D_PSP_MSTATUS64_SD_MASK    0x8000000000000000

/* mip CSR */
#define D_PSP_MIP_NUM          0x344
#define D_PSP_MIP_USIP_MASK    0x00000001  /* Software Interrupt pending */ /* User mode */
#define D_PSP_MIP_SSIP_MASK    0x00000002                                   /* Supervisor mode */
#define D_PSP_MIP_MSIP_MASK    0x00000008                                   /* Machine mode */
#define D_PSP_MIP_UTIP_MASK    0x00000010  /* Timer Interrupt pending */    /* User mode */
#define D_PSP_MIP_STIP_MASK    0x00000020                                   /* Supervisor mode */
#define D_PSP_MIP_MTIP_MASK    0x00000080                                   /* Machine mode */
#define D_PSP_MIP_UEIP_MASK    0x00000100  /* External Interrupt pending */ /* User mode */
#define D_PSP_MIP_SEIP_MASK    0x00000200                                   /* Supervisor mode */
#define D_PSP_MIP_MEIP_MASK    0x00000800                                   /* Machine mode */

/* mie CSR */
#define D_PSP_MIE_NUM          0x304
#define D_PSP_MIE_USIE_MASK    0x00000001  /* Software Interrupt enable/disable */ /* User mode */
#define D_PSP_MIE_SSIE_MASK    0x00000002                                          /* Supervisor mode */
#define D_PSP_MIE_MSIE_MASK    0x00000008                                          /* Machine mode */
#define D_PSP_MIE_UTIE_MASK    0x00000010  /* Timer Interrupt enable/disable */    /* User mode */
#define D_PSP_MIE_STIE_MASK    0x00000020                                          /* Supervisor mode */
#define D_PSP_MIE_MTIE_MASK    0x00000080                                          /* Machine mode */
#define D_PSP_MIE_UEIE_MASK    0x00000100  /* External Interrupt enable/disable */ /* User mode */
#define D_PSP_MIE_SEIE_MASK    0x00000200                                          /* Supervisor mode */
#define D_PSP_MIE_MEIE_MASK    0x00000800                                          /* Machine mode */

/* uie CSR */
#define D_PSP_UIE_NUM          0x004
#define D_PSP_UIE_USIE_MASK    0x00000001  /* Software Interrupt enable/disable */ /* User mode */
#define D_PSP_UIE_UTIE_MASK    0x00000010  /* Timer Interrupt enable/disable */    /* User mode */
#define D_PSP_UIE_UEIE_MASK    0x00000100  /* External Interrupt enable/disable */ /* User mode */

/* mcause CSR */
#define D_PSP_MCAUSE_NUM                         0x342
#define D_PSP_MCAUSE_U_SW_INT_MASK               0x80000000
#define D_PSP_MCAUSE_S_SW_INT_MASK               0x80000001
#define D_PSP_MCAUSE_M_SW_INT_MASK               0x80000003
#define D_PSP_MCAUSE_U_TIMER_INT_MASK            0x80000004
#define D_PSP_MCAUSE_S_TIMER_INT_MASK            0x80000005
#define D_PSP_MCAUSE_M_TIMER_INT_MASK            0x80000007
#define D_PSP_MCAUSE_U_EXT_INT_MASK              0x80000008
#define D_PSP_MCAUSE_S_EXT_INT_MASK              0x80000009
#define D_PSP_MCAUSE_M_EXT_INT_MASK              0x80000011
#define D_PSP_MCAUSE_INSTR_ADDR_MIS_EXC_MASK     0x00000000
#define D_PSP_MCAUSE_INSTR_ACCS_FLT_EXC_MASK     0x00000001
#define D_PSP_MCAUSE_ILGL_INSTRCTION_EXC_MASK    0x00000002
#define D_PSP_MCAUSE_BREAKPOINT_EXC_MASK         0x00000003
#define D_PSP_MCAUSE_LD_ADDR_MISALGN_EXC_MASK    0x00000004
#define D_PSP_MCAUSE_LD_ACCESS_FLT_EXC_MASK      0x00000005
#define D_PSP_MCAUSE_ST_ADDR_MISALGN_EXC_MASK    0x00000006
#define D_PSP_MCAUSE_ST_ACCESS_FLT_EXC_MASK      0x00000007
#define D_PSP_MCAUSE_ENV_CALL_U_MD_EXC_MASK      0x00000008
#define D_PSP_MCAUSE_ENV_CALL_S_MD_EXC_MASK      0x00000009
#define D_PSP_MCAUSE_ENV_CALL_M_MD_EXC_MASK      0x00000011
#define D_PSP_MCAUSE_INSTR_PAGE_FLT_EXC_MASK     0x00000012
#define D_PSP_MCAUSE_LD_PAGE_FLT_EXC_MASK        0x00000013
#define D_PSP_MCAUSE_ST_PAGE_FLT_EXC_MASK        0x00000015


#endif /* __PSP_CSRS_H__ */
