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
* @file   psp_csrs_el2.h
* @author Nati Rapaport
* @date   09.8.2020
* @brief  Definitions of Swerv's (EL2 version) CSRs
* 
*/
#ifndef  __PSP_CSRS_EL2_H__
#define  __PSP_CSRS_EL2_H__

/**
* include files
*/

/**
* definitions
*/

/**********************************/
/* Non standard CSRs in SweRV EL2 */
/**********************************/
#define D_PSP_MDBB_NUM         0xBC2 /* D-Bus Barrier register */
#define D_PSP_MDBAC_NUM        0xBC1 /* D-Bus 64-Bit Access Control register */
#define D_PSP_MDBHD_NUM        0xBC7 /* D-Bus 64-Bit High Data register */
#define D_PSP_MSPCBA_NUM       0x7F4 /* Stack Pointer Checker Base Address register */
#define D_PSP_MSPCTA_NUM       0x7F5 /* Stack Pointer Checker Top Address register */
#define D_PSP_MSPCC_NUM        0x7F6 /* Stack Pointer Checker Control register */


/* mscause CSR */
#define D_PSP_MSCAUSE_NUM              0x7FF       /* Machine Secondary cause CSR  */
/* Instruction access fault */
#define D_PSP_I_SIDE_BUS_ERROR                                       0
#define D_PSP_I_SIDE_DOUBLE_BIT_ECC_ERROR                            1
#define D_PSP_I_SIDE_UNMAPPED_ADDRESS_ERROR                          2
#define D_PSP_I_SIDE_ACCESS_OUT_OF_MPU_RANGE                         3
/* Illegal instruction */
#define D_PSP_INSTRUCTION_DATA                                       0
/* Breakpoint */
#define D_PSP_EBREAK_NOT_TODEBUG_MODE                                0
#define D_PSP_TRIGGER_HIT_NOT_TO_DEBUG_MODE                          1
/* Load address misaligned */
#define D_PSP_D_SIDE_LOAD_ACROSS_REGION_BOUNDARY                     0
#define D_PSP_D_SIDE_SIZE_MISALIGNED_LOAD_TO_NON_IDEMPOTENT_ADDRESS  1
/* Load access fault */
#define D_PSP_D_SIDE_CORE_LOCAL_LOAD_UNMAPPED_ADDRESS_ERROR          0
#define D_PSP_D_SIDE_DCCM_LOAD_DOUBLE_BIT_ECC_ERROR                  1
#define D_PSP_D_SIDE_LOAD_STACK_CHECK_ERROR                          2
#define D_PSP_D_SIDE_LOAD_SIDE_LOAD_ACCESS_OUT_OF_MPU_RANGE          3
#define D_PSP_D_SIDE_64_LOAD_ACCESS_ERROR                            4
#define D_PSP_D_SIDE_LOAD_REGION_PREDICTION_ERROR                    5
#define D_PSP_D_SIDE_PIC_LOAD_ACCESS_ERROR                           6
/* Store/AMO address misaligned */
#define D_PSP_D_SIDE_STORE_ACROSS_REGION_BOUNDARY                    0
#define D_PSP_D_SIDE_SIZE_MISALIGNED_STORE_TO_NON_IDEMPOTENT_ADDRESS 1
/* Store/AMO access fault */
#define D_PSP_D_SIDE_CORE_LOCAL_STORE_UNMAPPED_ADDRESS_ERROR         0
#define D_PSP_D_SIDE_DCCM_STORE_DOUBLE_BIT_ECC_ERROR                 1
#define D_PSP_D_SIDE_STORE_STACK_CHECK_ERROR                         2
#define D_PSP_D_SIDE_STORE_SIDE_LOAD_ACCESS_OUT_OF_MPU_RANGE         3
#define D_PSP_D_SIDE_64_STORE_ACCESS_ERROR                           4
#define D_PSP_D_STORE_REGION_PREDICTION_ERROR                        5
#define D_PSP_D_SIDE_PIC_STORE_ACCESS_ERROR                          6
/* Environment call from M-mode */
#define D_PSP_ECALL                                                  0


/**************/
/* Force-Debug CSRs */
/**************/
#define  D_PSP_MFDHT_NUM             0x7CE      /* Forced Debug Halt Threshold register */
#define  D_PSP_MFDHT_ENABLE_MASK     0x00000001 /* bit 0 */
#define  D_PSP_MFDHT_THRESHOLD_MASK  0x0000003E /* Power-of-two exponent of timeout threshold - bits 1..5 */
#define  D_PSP_MFDHT_THRESHOLD_SHIFT 1

#define  D_PSP_MFDHS_NUM             0x7CF       /* Forced Debug Halt Status register */
#define  D_PSP_MFDS_LSU_STATUS_MASK  0x00000001  /* LSU bus transaction termination status - bit 0 */
#define  D_PSP_MFDS_IFU_STATUS_MASK  0x00000002  /* IFU bus transaction termination status - bit 1 */

/**************************************/
/* Performance monitoring control CSR */
/**************************************/
#define D_PSP_MCOUNTINHIBIT_NUM 0x320


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



#endif /* __PSP_CSRS_EL2_H__ */
