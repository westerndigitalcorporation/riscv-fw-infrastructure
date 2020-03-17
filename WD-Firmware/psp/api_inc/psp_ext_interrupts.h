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
* @file   psp_ext_interrupts.h
* @author Nati Rapaport
* @date   16.03.2020
* @brief  The file defines the psp external interrupts interfaces
*/
#ifndef __PSP_EXT_INTERRUPTS_H__
#define __PSP_EXT_INTERRUPTS_H__

/**
* include files
*/

/**
* definitions
*/

/* External-Interrupts priority-order */
#define D_PSP_STANDARD_PRIORITY_ORDER 0
#define D_PSP_REVERSE_PRIORITY_ORDER  1


/* External Interrupt Priorities */
#define D_PSP_EXT_INT_PRIORITY_0            0 /* In standard order - lowest level. In reversed order - highest level */
#define D_PSP_EXT_INT_PRIORITY_1            1
#define D_PSP_EXT_INT_PRIORITY_2            2
#define D_PSP_EXT_INT_PRIORITY_3            3
#define D_PSP_EXT_INT_PRIORITY_4            4
#define D_PSP_EXT_INT_PRIORITY_5            5
#define D_PSP_EXT_INT_PRIORITY_6            6
#define D_PSP_EXT_INT_PRIORITY_7            7
#define D_PSP_EXT_INT_PRIORITY_8            8
#define D_PSP_EXT_INT_PRIORITY_9            9
#define D_PSP_EXT_INT_PRIORITY_10           10
#define D_PSP_EXT_INT_PRIORITY_11           11
#define D_PSP_EXT_INT_PRIORITY_12           12
#define D_PSP_EXT_INT_PRIORITY_13           13
#define D_PSP_EXT_INT_PRIORITY_14           14
#define D_PSP_EXT_INT_PRIORITY_15           15 /* In standard order - highest level. In reversed order - lowest level */


/* External Interrupt Threshold - Only external interrupt with a priority HIGHER than the threshold will be served  */
#define D_PSP_EXT_INT_THRESHOLD_0       0
#define D_PSP_EXT_INT_THRESHOLD_1       1
#define D_PSP_EXT_INT_THRESHOLD_2       2
#define D_PSP_EXT_INT_THRESHOLD_3       3
#define D_PSP_EXT_INT_THRESHOLD_4       4
#define D_PSP_EXT_INT_THRESHOLD_5       5
#define D_PSP_EXT_INT_THRESHOLD_6       6
#define D_PSP_EXT_INT_THRESHOLD_7       7
#define D_PSP_EXT_INT_THRESHOLD_8       8
#define D_PSP_EXT_INT_THRESHOLD_9       9
#define D_PSP_EXT_INT_THRESHOLD_10      10
#define D_PSP_EXT_INT_THRESHOLD_11      11
#define D_PSP_EXT_INT_THRESHOLD_12      12
#define D_PSP_EXT_INT_THRESHOLD_13      13
#define D_PSP_EXT_INT_THRESHOLD_14      14
#define D_PSP_EXT_INT_THRESHOLD_15      15


/**
* types
*/


/**
* local prototypes
*/

/**
* external prototypes
*/
extern void (*g_fptrPspExternalInterruptDisableNumber)(u32_t uiIntNum);
extern void (*g_fptrPspExternalInterruptEnableNumber)(u32_t uiIntNum);
extern void (*g_fptrPspExternalInterruptSetPriority)(u32_t uiIntNum, u32_t uiPriority);
extern void (*g_fptrPspExternalInterruptSetThreshold)(u32_t uiThreshold);
extern pspInterruptHandler_t (*g_fptrPspExternalInterruptRegisterISR)(u32_t uiVectorNumber, pspInterruptHandler_t pIsr, void* pParameter);


/**
* macros
*/

#define M_PSP_EXT_INT_DISBLE_ID(ext_int_id)              g_fptrPspExternalInterruptDisableNumber(ext_int_id);
#define M_PSP_EXT_INT_ENSBLE_ID(ext_int_id)              g_fptrPspExternalInterruptEnableNumber(ext_int_id);
#define M_PSP_EXT_INT_SET_PRIORITY(ext_int_id, priority) g_fptrPspExternalInterruptSetPriority(ext_int_id, priority);
#define M_PSP_EXT_INT_SET_THRESHOLD(threshold)           g_fptrPspExternalInterruptsSetThreshold(threshold);
#define M_PSP_EXT_INT_REGISTER_HANDLER(vect_number, pIsr, pParameters) \
		                                                       g_fptrPspExternalInterruptRegisterISR(vect_number, pIsr, pParameters);

/* When set this priority level in meiplS (External Interrupt Priority Level Register) the corresponding interrupt will
 * not be served.
 * In case of standard-priority-order the priority is 0, in case of reversed priority order - 15 */
#define M_PSP_EXT_INT_PRIORITY_SET_TO_MASKED \
		(M_PSP_READ_REGISTER_32(D_PSP_PIC_MPICCFG_ADDR) & D_PSP_MPICCFG_PRIORD_MASK) == D_PSP_STANDARD_PRIORITY_ORDER \
		                                                                      ? D_PSP_EXT_INT_PRIORITY_0 : D_PSP_EXT_INT_PRIORITY_15

/* Mask all interrupts in meipt (External Interrupt Priority Threshold Register) -
 * In case of standard priority order - 15, in case of reversed priority order - 0 */
#define M_PSP_EXT_INT_THRESHOLD_MASK_ALL \
		(M_PSP_READ_REGISTER_32(D_PSP_PIC_MPICCFG_ADDR) & D_PSP_MPICCFG_PRIORD_MASK) == D_PSP_STANDARD_PRIORITY_ORDER \
		                                                                      ? D_PSP_THRESHOLD_15 : D_PSP_THRESHOLD_0

/* Unmask all interrupts in meipt (External Interrupt Priority Threshold Register) -
 * In case of standard priority order - 0, in case of reversed priority order - 15 */
#define M_PSP_EXT_INT_THRESHOLD_UNMASK_ALL \
		(M_PSP_READ_REGISTER_32(D_PSP_PIC_MPICCFG_ADDR) & D_PSP_MPICCFG_PRIORD_MASK) == D_PSP_STANDARD_PRIORITY_ORDER \
		                                                                      ? D_PSP_THRESHOLD_0 : D_PSP_THRESHOLD_15

/**
* global variables
*/

/**
* APIs
*/


#endif /* __PSP_EXT_INTERRUPTS_H__ */
