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
* @file   psp_csrs.h
* @author Nati Rapaport
* @date   28.01.2020
* @brief  CSRs definitions (per privileged spec)
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
#define D_PSP_MTVEC   mtvec
#define D_PSP_MSTATUS mstatus
#define D_PSP_MIE     mie
#define D_PSP_MIP     mip
#define D_PSP_MCAUSE  mcause

/* Definitions for the various fields in the CSRs */
/* misa CSR */
#define D_PSP_MISA_EXTENSIONS     0x03FFFFFF
#define D_PSP_MISA_MXL            0x0C000000
/*  mtvec CSR */
#define D_PSP_MTVEC_MODE          0x00000003
/* mstatus CSR */
#define D_PSP_MSTATUS_UIE         0x00000001   /* Interrupts enable/disable */ /* User mode */
#define D_PSP_MSTATUS_SIE         0x00000002                                   /* Supervisor mode */
#define D_PSP_MSTATUS_MIE         0x00000008                                   /* Machine mode */
#define D_PSP_MSTATUS_UPIE        0x00000010
#define D_PSP_MSTATUS_SPIE        0x00000020
#define D_PSP_MSTATUS_MPIE        0x00000080
#define D_PSP_MSTATUS_SPP         0x00000100
#define D_PSP_MSTATUS_MPP         0x00001800
#define D_PSP_MSTATUS_FS          0x00006000
#define D_PSP_MSTATUS_XS          0x00018000
#define D_PSP_MSTATUS_MPRV        0x00020000
#define D_PSP_MSTATUS_SUM         0x00040000
#define D_PSP_MSTATUS_MXR         0x00080000
#define D_PSP_MSTATUS_TVM         0x00100000
#define D_PSP_MSTATUS_TW          0x00200000
#define D_PSP_MSTATUS_TSR         0x00400000
#define D_PSP_MSTATUS_UXL         0x0000000300000000
#define D_PSP_MSTATUS_SXL         0x0000000C00000000
#define D_PSP_MSTATUS32_SD        0x80000000
#define D_PSP_MSTATUS64_SD        0x8000000000000000
/* mip CSR */
#define D_PSP_MIP_USIP                0x00000001  /* Software Interrupt pending */ /* User mode */
#define D_PSP_MIP_SSIP                0x00000002                                   /* Supervisor mode */
#define D_PSP_MIP_MSIP                0x00000008                                   /* Machine mode */
#define D_PSP_MIP_UTIP                0x00000010  /* Timer Interrupt pending */    /* User mode */
#define D_PSP_MIP_STIP                0x00000020                                   /* Supervisor mode */
#define D_PSP_MIP_MTIP                0x00000080                                   /* Machine mode */
#define D_PSP_MIP_UEIP                0x00000100  /* External Interrupt pending */ /* User mode */
#define D_PSP_MIP_SEIP                0x00000200                                   /* Supervisor mode */
#define D_PSP_MIP_MEIP                0x00000800                                   /* Machine mode */
/* mie CSR */
#define D_PSP_MIE_USIE                0x00000001  /* Software Interrupt enable/disable */ /* User mode */
#define D_PSP_MIE_SSIE                0x00000002                                          /* Supervisor mode */
#define D_PSP_MIE_MSIE                0x00000008                                          /* Machine mode */
#define D_PSP_MIE_UTIE                0x00000010  /* Timer Interrupt enable/disable */    /* User mode */
#define D_PSP_MIE_STIE                0x00000020                                          /* Supervisor mode */
#define D_PSP_MIE_MTIE                0x00000080                                          /* Machine mode */
#define D_PSP_MIE_UEIE                0x00000100  /* External Interrupt enable/disable */ /* User mode */
#define D_PSP_MIE_SEIE                0x00000200                                          /* Supervisor mode */
#define D_PSP_MIE_MEIE                0x00000800                                          /* Machine mode */
/* mcause CSR */
#define D_PSP_MCAUSE_U_SW_INT            0x80000000
#define D_PSP_MCAUSE_S_SW_INT            0x80000001
#define D_PSP_MCAUSE_M_SW_INT            0x80000003
#define D_PSP_MCAUSE_U_TIMER_INT         0x80000004
#define D_PSP_MCAUSE_S_TIMER_INT         0x80000005
#define D_PSP_MCAUSE_M_TIMER_INT         0x80000007
#define D_PSP_MCAUSE_U_EXT_INT           0x80000008
#define D_PSP_MCAUSE_S_EXT_INT           0x80000009
#define D_PSP_MCAUSE_M_EXT_INT           0x80000011
#define D_PSP_MCAUSE_INSTR_ADDR_MIS_EXC  0x00000000
#define D_PSP_MCAUSE_INSTR_ACCS_FLT_EXC  0x00000001
#define D_PSP_MCAUSE_ILGL_INSTRCTION_EXC 0x00000002
#define D_PSP_MCAUSE_BREAKPOINT_EXC      0x00000003
#define D_PSP_MCAUSE_LD_ADDR_MISALGN_EXC 0x00000004
#define D_PSP_MCAUSE_LD_ACCESS_FLT_EXC   0x00000005
#define D_PSP_MCAUSE_ST_ADDR_MISALGN_EXC 0x00000006
#define D_PSP_MCAUSE_ST_ACCESS_FLT_EXC   0x00000007
#define D_PSP_MCAUSE_ENV_CALL_U_MD_EXC   0x00000008
#define D_PSP_MCAUSE_ENV_CALL_S_MD_EXC   0x00000009
#define D_PSP_MCAUSE_ENV_CALL_M_MD_EXC   0x00000011
#define D_PSP_MCAUSE_INSTR_PAGE_FLT_EXC  0x00000012
#define D_PSP_MCAUSE_LD_PAGE_FLT_EXC     0x00000013
#define D_PSP_MCAUSE_ST_PAGE_FLT_EXC     0x00000015


#endif /* __PSP_CSRS_H__ */
