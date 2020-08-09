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
* @file   psp_ext_interrupts_eh1.h
* @author Nati Rapaport
* @date   16.03.2020
* @brief  The file defines the psp external interrupts interfaces for SweRV EH1
*/
#ifndef __PSP_EXT_INTERRUPTS_EH1_H__
#define __PSP_EXT_INTERRUPTS_EH1_H__

/**
* include files
*/

/**
* definitions
*/

/* External-Interrupts type */
#define D_PSP_EXT_INT_LEVEL_TRIG_TYPE  0
#define D_PSP_EXT_INT_EDGE_TRIG_TYPE   1

/* External-Interrupts polarity */
#define D_PSP_EXT_INT_ACTIVE_HIGH  0
#define D_PSP_EXT_INT_ACTIVE_LOW   1

/* External-Interrupts priority-order */
#define D_PSP_EXT_INT_STANDARD_PRIORITY  0
#define D_PSP_EXT_INT_REVERSED_PRIORITY  1


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


/**
* macros
*/

/* Set the highest priority level in meiplS (External Interrupt Priority Level Register)
 * In case of standard-priority-order the priority is 15, in case of reversed priority order - 0 */
#define M_PSP_EXT_INT_PRIORITY_SET_TO_HIGHEST_VALUE \
    (M_PSP_READ_REGISTER_32(D_PSP_PIC_MPICCFG_ADDR) & D_PSP_MPICCFG_PRIORD_MASK) == D_PSP_EXT_INT_STANDARD_PRIORITY \
                                                                          ? D_PSP_EXT_INT_PRIORITY_15 : D_PSP_EXT_INT_PRIORITY_0

/* When set this priority level in meiplS (External Interrupt Priority Level Register) the corresponding interrupt will
 * not be served.
 * In case of standard-priority-order the priority is 0, in case of reversed priority order - 15 */
#define M_PSP_EXT_INT_PRIORITY_SET_TO_MASKED_VALUE \
    (M_PSP_READ_REGISTER_32(D_PSP_PIC_MPICCFG_ADDR) & D_PSP_MPICCFG_PRIORD_MASK) == D_PSP_EXT_INT_STANDARD_PRIORITY \
                                                                          ? D_PSP_EXT_INT_PRIORITY_0 : D_PSP_EXT_INT_PRIORITY_15

/* When set this threshold level in meipt (External Interrupt Priority Threshold Register) all interrupts will be masked (not served)
 * In case of standard priority order - 15, in case of reversed priority order - 0 */
#define M_PSP_EXT_INT_THRESHOLD_MASK_ALL_VALUE \
    (M_PSP_READ_REGISTER_32(D_PSP_PIC_MPICCFG_ADDR) & D_PSP_MPICCFG_PRIORD_MASK) == D_PSP_EXT_INT_STANDARD_PRIORITY \
                                                                          ? D_PSP_EXT_INT_THRESHOLD_15 : D_PSP_EXT_INT_THRESHOLD_0

/* When set this threshold level in meipt (External Interrupt Priority Threshold Register) no interrupt will be masked (all served)
 * In case of standard priority order - 0, in case of reversed priority order - 15 */
#define M_PSP_EXT_INT_THRESHOLD_UNMASK_ALL_VALUE \
    (M_PSP_READ_REGISTER_32(D_PSP_PIC_MPICCFG_ADDR) & D_PSP_MPICCFG_PRIORD_MASK) == D_PSP_EXT_INT_STANDARD_PRIORITY \
                                                                          ? D_PSP_EXT_INT_THRESHOLD_0 : D_PSP_EXT_INT_THRESHOLD_15

/**
* global variables
*/

/**
* APIs
*/

/**
* @brief - Set external interrupts vector-table address at MEIVT CSR
*/
void pspExternalInterruptSetVectorTableAddress(void* pExtIntVectTable);

/*
* This function registers external interrupt handler
*
* @param uiVectorNumber = the number of the external interrupt to register
*        pIsr = the ISR to register
*        pParameter = NOT IN USE for baremetal implementation
* @return pOldIsr = pointer to the previously registered ISR (Null in case of a failure)
*/
pspInterruptHandler_t pspExternalInterruptRegisterISR(u32_t uiVectorNumber, pspInterruptHandler_t pIsr, void* pParameter);


/*
* This function disables a specified external interrupt in the PIC
*
* @param intNum = the number of the external interrupt to disable
* @return None
*/
void pspExternalInterruptDisableNumber(u32_t uiIntNum);


/*
* This function enables a specified external interrupt in the PIC
*
* @param intNum = the number of the external interrupt to enable
* @return None
*/
void pspExternalInterruptEnableNumber(u32_t uiIntNum);


/*
* This function checks whether a given external interrupt is pending or not
*
* @param uiExtInterrupt = Number of external interrupt
* @return = pending (1) or not (0)
*/
u32_t pspExtInterruptIsPending(u32_t uiExtInterrupt);


/*
* This function set external-interrupt type (Level-triggered or Edge-triggered)
*
* @param uiIntNum  = Number of external interrupt
* @param uiIntType = Type of the interrupt (level or edge)
*/
void  pspExtInterruptSetType(u32_t uiIntNum, u32_t uiIntType);


/*
* This function set external-interrupt polarity (active-high or active-low)
*
* @param uiIntNum   = Number of external interrupt
* @param uiPolarity = active-high or active-low
*/
void  pspExtInterruptSetPolarity(u32_t uiIntNum, u32_t uiPolarity);


/*
* This function clears the indication of pending interrupt
*
* @param uiIntNum   = Number of external interrupt
*/
void  pspExtInterruptClearPendingInt(u32_t uiIntNum);


/*
* This function set Priority Order (Standard or Reserved)
*
* @param uiPriorityOrder = Standard or Reserved
*/
void  pspExtInterruptSetPriorityOrder(u32_t uiPriorityOrder);

/*
*  This function sets the priority of a specified external interrupt
*
*  @param intNum = the number of the external interrupt to disable
*  @param priority = priority to be set
* @return None
*/
void  pspExtInterruptSetPriority(u32_t uiIntNum, u32_t uiPriority);

/*
* This function sets the priority threshold of the external interrupts in the PIC
*
* @param threshold = priority threshold to be programmed to PIC
* @return None
*/
void  pspExtInterruptsSetThreshold(u32_t uiThreshold);

/*
* This function sets the nesting priority threshold of the external interrupts in the PIC
*
* @param threshold = nesting priority threshold to be programmed to PIC
* @return None
*/
void  pspExtInterruptsSetNestingPriorityThreshold(u32_t uiNestingPriorityThreshold);

/*
* This function get the current selected external interrupt (claim-id)
*
* @return - claim-id number
*/
u32_t pspExtInterruptGetClaimId(void);


/*
* This function get the priority of currently selected external interrupt
*
* @return - priority level
*/
u32_t pspExtInterruptGetPriority(void);



#endif /* __PSP_EXT_INTERRUPTS_EH1_H__ */
