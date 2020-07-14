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
* @file   psp_csrs_eh2.h
* @author Nati Rapaport
* @date   01.12.2019
* @brief  Definitions of Swerv's (EH2 version) CSRs
* 
*/
#ifndef  __PSP_CSRS_EH2_H__
#define  __PSP_CSRS_EH2_H__

/**
* include files
*/

/**
* definitions
*/

/** Non standard CSRs in SweRV EH2 **/
#define  D_PSP_MHARTSTART_NUM  0x7FC   /* Hart Start Control register */
#define  D_PSP_MNMIPDEL_NUM    0x7FE   /* NMI Pin Delegation register */
#define  D_PSP_MNMIPDEL_MASK   0x00000003 /* Only bits 0 and 1 are relevant */

#define  D_PSP_MHARTNUM_NUM         0xFC4      /* Total Number of Harts register */
#define  D_PSP_MHARTNUM_TOTAL_MASK  0x00000003 /* Total number of Harts in this core - bits 0..1 */

/**************/
/* Debug CSRs */
/**************/
#define  D_PSP_MFDHT_NUM             0x7CE      /* Forced Debug Halt Threshold register */
#define  D_PSP_MFDHT_ENABLE_MASK     0x00000001 /* bit 0 */
#define  D_PSP_MFDHT_THRESHOLD_MASK  0x0000003E /* Power-of-two exponent of timeout threshold - bits 1..5 */
#define  D_PSP_MFDHT_THRESHOLD_SHIFT 1

#define  D_PSP_MFDHS_NUM             0x7CF       /* Forced Debug Halt Status register */
#define  D_PSP_MFDS_LSU_STATUS_MASK  0x00000001  /* LSU bus transaction termination status - bit 0 */
#define  D_PSP_MFDS_IFU_STATUS_MASK  0x00000002  /* IFU bus transaction termination status - bit 1 */

/*************************************/
/* PIC memory mapped registers       */
/*************************************/
/* meidel CSR */
#define  D_PSP_PIC_MEIDEL_OFFSET       0x6000
#define  D_PSP_PIC_MEIDEL_ADDR         PSP_PIC_BASE_ADDRESS + D_PSP_PIC_MEIDEL_OFFSET  /* External interrupts delegation register */
#define  D_PSP_MEIDEL_DELEGATION_MASK  0x00000001 /* bit 0 */

/* meitp CSR */
#define D_PSP_PIC_MEITP_OFFSET       0x1800
#define D_PSP_MEITP_ADDR             PSP_PIC_BASE_ADDRESS + D_PSP_PIC_MEITP_OFFSET  /* External interrupts per-hart pending  */

/*****************************************/
/* EH2 specific fields in standard CSRs  */
/*****************************************/
/* mhartid CSR */
//#define D_PSP_MHARTID_NUM     0xF14
#define  D_PSP_MHARTID_CORE_ID_MASK    0xFFFFFFF0  /* Core ID field - bits 4..31 */
#define  D_PSP_MHARTID_CORE_ID_SHIFT   4           /* Core ID field shift - 4    */
#define  D_PSP_MHARTID_HART_ID_MASK    0x0000000F  /* Hart ID field - bits 0..3  */
#define  D_PSP_MHARTID_HART_ID_SHIFT   0           /* Hart ID field shift - 0    */


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



#endif /* __PSP_CSRS_EH2_H__ */
