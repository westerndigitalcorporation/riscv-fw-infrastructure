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
* @file   psp_swerv_ehx1_csrs.h
* @author Nati Rapaport
* @date   01.12.2019
* @brief  Definitions of Swerv's (EHX1 version) CSRs
* 
*/
#ifndef  __PSP_SWERV_EHX1_CSRS_H__
#define  __PSP_SWERV_EHX1_CSRS_H__

/**
* include files
*/

/**
* definitions
*/
/** SweRV-EHX1, Non standard CSRs  **/
#define		D_PSP_MRAC 		0x7C0 	/* Region access control  */
#define		D_PSP_MCPC 		0x7C2 	/* Core pause control  */
#define		D_PSP_DMST 		0x7C4 	/* Memory synchronization trigger (debug mode only)  */
#define		D_PSP_MPMC 		0x7C6 	/* Power management control  */
#define		D_PSP_DICAWICS	0x7C8 	/* I-cache array/way/index selection (debug mode only)  */
#define		D_PSP_DICAD0    0x7C9 	/* I-cache array data 0 (debug mode only)  */
#define		D_PSP_DICAD1    0x7CA 	/* I-cache array data 1 (debug mode only)  */
#define		D_PSP_DICAGO    0x7CB 	/* I-cache array go (debug mode only)  */
#define		D_PSP_MGPMC     0x7D0 	/* Group performance monitor control  */
#define		D_PSP_MITCNT0   0x7D2 	/* Internal timer counter 0  */
#define		D_PSP_MITBND0 	0x7D3 	/* Internal timer bound 0  */
#define		D_PSP_MITCTL0 	0x7D4 	/* Internal timer control 0  */
#define		D_PSP_MITCNT1 	0x7D5 	/* Internal timer counter 1  */
#define		D_PSP_MITBND1 	0x7D6 	/* Internal timer bound 1  */
#define		D_PSP_MITCTL1   0x7D7 	/* Internal timer control 1  */
#define		D_PSP_MICECT    0x7F0 	/* I-cache error counter/threshold */
#define		D_PSP_MICCMECT  0x7F1 	/* ICCM correctable error counter/threshold  */
#define		D_PSP_MDCCMECT  0x7F2 	/* DCCM correctable error counter/threshold  */
#define		D_PSP_MCGC      0x7F8 	/* Clock gating control  */
#define		D_PSP_MFDC      0x7F9 	/* Feature disable control  */
#define		D_PSP_MDEAU     0xBC0 	/* D-Bus error address unlock  */
#define		D_PSP_MEIVT     0xBC8 	/* External interrupt vector table  */
#define		D_PSP_MEIPT     0xBC9 	/* External interrupt priority threshold  */
#define		D_PSP_MEICPCT   0xBCA 	/* External interrupt claim ID / priority level capture trigger  */
#define		D_PSP_MEICIDPL  0xBCB 	/* External interrupt claim ID�s priority level  */
#define		D_PSP_MEICURPL  0xBCC 	/* External interrupt current priority level  */
#define		D_PSP_MDSEAC    0xFC0 	/* D-bus first error address capture  */
#define		D_PSP_MEIHAP    0xFC8 	/* External interrupt handler address pointer  */

/** SweRV-EHX1, Core-Specific, Standard RISC-V Machine Information CSRs **/
#define		D_PSP_MISA		0x301		/* ISA and extensions */
#define		D_PSP_MVENDORID 0xF11		/* Vendor ID */
#define		D_PSP_MARCHID   0xF12		/* Architecture ID */
#define		D_PSP_MIMPID    0xF13		/* Implementation ID */
#define		D_PSP_MHARTID   0xF14		/* Hardware thread ID */


/* Definitions of certain fields in various SweRV-EHX1 CSRs */
/* MITCTL CSR fields */
#define D_PSP_MITCTL_PAUSE_EN   0x00000004 /* bit #2 */
#define D_PSP_MITCTL_HALT_EN    0x00000002 /* bit #1 */
#define D_PSP_MITCTL_EN         0x00000001 /* bit #0 */


/* MIE and MIP CSRs, SweRV-EHX1 specific fields */
#define D_PSP_IRQ_M_TIMER1          28
#define D_PSP_IRQ_M_TIMER0          29
#define D_PSP_IRQ_CORRECTABLE_ERROR 30

#define D_PSP_MIE_TIMER1_INT_ENABLE            (1 << D_PSP_IRQ_M_TIMER1)
#define D_PSP_MIE_TIMER0_INT_ENABLE            (1 << D_PSP_IRQ_M_TIMER0)
#define D_PSP_MIE_CORR_ERR_INT_ENABLE          (1 << D_PSP_IRQ_CORRECTABLE_ERROR)

#define D_PSP_MIP_TIMER1_INT_PENDING           (1 << D_PSP_IRQ_M_TIMER1)
#define D_PSP_MIP_TIMER0_INT_PENDING           (1 << D_PSP_IRQ_M_TIMER0)
#define D_PSP_MIE_CORR_ERR_INT_PENDING         (1 << D_PSP_IRQ_CORRECTABLE_ERROR)


/**
* macros
*/

/* Disable & Enable of Timer0 interrupt (via specific SweRV-EHX1 field in mie CSR)*/
#define M_PSP_DISABLE_TIMER0_INT()	    M_PSP_CLEAR_CSR(mie, D_PSP_MIE_TIMER0_INT_ENABLE);
#define M_PSP_ENABLE_TIMER0_INT()	    M_PSP_SET_CSR(mie, D_PSP_MIE_TIMER0_INT_ENABLE);
/* Disable & Enable of Timer0 count (via MITCTL0 - specific SweRV-EHX1 CSR)*/
#define M_PSP_DISABLE_TIMER0_CNT()  M_PSP_CLEAR_PS_CSR(D_PSP_MITCTL0, D_PSP_MITCTL_EN);
#define M_PSP_ENABALE_TIMER0_CNT()  M_PSP_SET_PS_CSR(D_PSP_MITCTL0, D_PSP_MITCTL_EN);

/* Disable & Enable of Timer1 interrupt (via specific SweRV-EHX1 field in mie CSR)*/
#define M_PSP_DISABLE_TIMER1_INT()	    M_PSP_CLEAR_CSR(mie, D_PSP_MIE_TIMER1_INT_ENABLE);
#define M_PSP_ENABLE_TIMER1_INT()	    M_PSP_SET_CSR(mie, D_PSP_MIE_TIMER1_INT_ENABLE);
/* Disable & Enable of Timer1 count (via MITCTL1 - specific SweRV-EHX1 CSR)*/
#define M_PSP_DISABLE_TIMER1_CNT()  M_PSP_CLEAR_PS_CSR(D_PSP_MITCTL1, D_PSP_MITCTL_EN);
#define M_PSP_ENABALE_TIMER1_CNT()  M_PSP_SET_PS_CSR(D_PSP_MITCTL1, D_PSP_MITCTL_EN);

#ifdef M_PSP_DISABLE_SWERV_TIMER
    #undef M_PSP_DISABLE_SWERV_TIMER
#endif
#define M_PSP_DISABLE_SWERV_TIMER() M_PSP_DISABLE_TIMER0_INT();

#ifdef M_PSP_ENABLE_SWERV_TIMER
    #undef M_PSP_ENABLE_SWERV_TIMER
#endif
#define M_PSP_ENABLE_SWERV_TIMER()  M_PSP_ENABLE_TIMER0_INT();

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



#endif /* __PSP_SWERV_EHX1_CSRS_H__ */
