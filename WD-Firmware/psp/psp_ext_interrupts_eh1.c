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
* @file   psp_ext_interrupts_eh1.c
* @author Nati Rapaport
* @date   15.03.2020
* @brief  The file supplies external interrupt services (vector table installment, ISRs registration, PIC control API etc)
*         The file is specific to SweRV EH1 specifications.
* 
*/

/**
* include files
*/
#include "psp_api.h"

/**
* definitions
*/

/* Number of external interrupt sources in the PIC */
#if (0 == D_PIC_NUM_OF_EXT_INTERRUPTS)
    #error "D_PIC_NUM_OF_EXT_INTERRUPTS is not defined"
#else
    #define PSP_PIC_NUM_OF_EXT_INTERRUPTS D_PIC_NUM_OF_EXT_INTERRUPTS
#endif

/* Number of first External interrupt source in the PIC (0 is a valid number) */
#ifndef  D_EXT_INTERRUPT_FIRST_SOURCE_USED
    #error "D_EXT_INTERRUPT_FIRST_SOURCE_USED is not defined"
#else
    #define PSP_EXT_INTERRUPT_FIRST_SOURCE_USED    D_EXT_INTERRUPT_FIRST_SOURCE_USED
#endif

/* Number of last External interrupt source in the PIC */
#if (0 == D_EXT_INTERRUPT_LAST_SOURCE_USED)
    #error "D_EXT_INTERRUPT_LAST_SOURCE_USED is not defined"
#else
    #define PSP_EXT_INTERRUPT_LAST_SOURCE_USED     D_EXT_INTERRUPT_LAST_SOURCE_USED
#endif

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

/* External interrupt handlers Global Table */
D_PSP_DATA_SECTION D_PSP_ALIGNED(1024) pspInterruptHandler_t G_Ext_Interrupt_Handlers[PSP_PIC_NUM_OF_EXT_INTERRUPTS];


/**
* functions
*/

/**
* @brief - Set external interrupts vector-table address at MEIVT CSR
*/
D_PSP_TEXT_SECTION void pspExternalInterruptSetVectorTableAddress(void* pExtIntVectTable)
{
  u32_t uiInterruptsState;

  /* Assert if vector-table address is NULL */
  M_PSP_ASSERT(NULL != pExtIntVectTable)

  /* Disable interrupts */
  pspInterruptsDisable(&uiInterruptsState);

  M_PSP_WRITE_CSR(D_PSP_MEIVT_NUM, pExtIntVectTable);

  /* Restore interrupts */
  pspInterruptsRestore(uiInterruptsState);
}

/*
* This function registers external interrupt handler
*
* @param uiVectorNumber = the number of the external interrupt to register
*        pIsr = the ISR to register
*        pParameter = NOT IN USE for baremetal implementation
* @return pOldIsr = pointer to the previously registered ISR
*/
D_PSP_TEXT_SECTION pspInterruptHandler_t pspExternalInterruptRegisterISR(u32_t uiVectorNumber, pspInterruptHandler_t pIsr, void* pParameter)
{
   u32_t uiInterruptsState;
   pspInterruptHandler_t fptrPrevIsr = NULL;

   /* Assert if uiVectorNumber is beyond first or last interrupts-in-use */
   M_PSP_ASSERT((PSP_EXT_INTERRUPT_FIRST_SOURCE_USED <= uiVectorNumber)  && (PSP_EXT_INTERRUPT_LAST_SOURCE_USED >= uiVectorNumber))

   /* Disable interrupts */
   pspInterruptsDisable(&uiInterruptsState);

   /* Set external-interrupts vector-table address in MEIVT CSR */
   pspExternalInterruptSetVectorTableAddress(G_Ext_Interrupt_Handlers);

   /* Register the interrupt */
   fptrPrevIsr = G_Ext_Interrupt_Handlers[uiVectorNumber];
   G_Ext_Interrupt_Handlers[uiVectorNumber] = pIsr;

   /* Restore interrupts */
   pspInterruptsRestore(uiInterruptsState);

   return(fptrPrevIsr);
}

/**
* This function disables a specified external interrupt in the PIC
*
* @param intNum = the number of the external interrupt to disable
* @return None
*/
D_PSP_TEXT_SECTION void pspExternalInterruptDisableNumber(u32_t uiIntNum)
{
  u32_t uiInterruptsState;

  /* Assert if uiVectorNumber is beyond first or last interrupts-in-use */
  M_PSP_ASSERT((PSP_EXT_INTERRUPT_FIRST_SOURCE_USED <= uiIntNum)  && (PSP_EXT_INTERRUPT_LAST_SOURCE_USED >= uiIntNum))

  /* Disable interrupts */
  pspInterruptsDisable(&uiInterruptsState);

  /* Clear Int-Enable bit in meie register, corresponds to given source (interrupt-number) */
  M_PSP_WRITE_REGISTER_32((D_PSP_PIC_MEIE_ADDR + M_PSP_MULT_BY_4(uiIntNum)) , 0);

  /* Restore interrupts */
  pspInterruptsRestore(uiInterruptsState);
}

/*
* This function enables a specified external interrupt in the PIC
*
* @param intNum = the number of the external interrupt to enable
* @return None
*/
D_PSP_TEXT_SECTION void pspExternalInterruptEnableNumber(u32_t uiIntNum)
{
  u32_t uiInterruptsState;

  /* Assert if uiVectorNumber is beyond first or last interrupts-in-use */
  M_PSP_ASSERT((PSP_EXT_INTERRUPT_FIRST_SOURCE_USED <= uiIntNum)  && (PSP_EXT_INTERRUPT_LAST_SOURCE_USED >= uiIntNum))

  /* Disable interrupts */
  pspInterruptsDisable(&uiInterruptsState);

  /* Set Int-Enable bit in meie register, corresponds to given source (interrupt-number) */
  M_PSP_WRITE_REGISTER_32((D_PSP_PIC_MEIE_ADDR + M_PSP_MULT_BY_4(uiIntNum)), D_PSP_MEIE_INT_EN_MASK);

  /* Restore interrupts */
  pspInterruptsRestore(uiInterruptsState);
}

/*
*  This function sets the priority of a specified external interrupt
*
*  @param intNum = the number of the external interrupt to disable
*  @param priority = priority to be set
* @return None
*/
D_PSP_TEXT_SECTION void pspExtInterruptSetPriority(u32_t uiIntNum, u32_t uiPriority)
{
  u32_t uiInterruptsState;

  /* Disable interrupts */
  pspInterruptsDisable(&uiInterruptsState);

  /* Set priority in meipl register, corresponds to given source (interrupt-number) */
  M_PSP_WRITE_REGISTER_32((D_PSP_MEIPL_ADDR + M_PSP_MULT_BY_4(uiIntNum)), uiPriority);

  /* Restore interrupts */
  pspInterruptsRestore(uiInterruptsState);
}

/*
* This function sets the priority threshold of the external interrupts in the PIC
*
* @param threshold = priority threshold to be programmed to PIC
* @return None
*/
D_PSP_TEXT_SECTION void pspExtInterruptsSetThreshold(u32_t uiThreshold)
{
  u32_t uiInterruptsState;

  /* Disable interrupts */
  pspInterruptsDisable(&uiInterruptsState);

  /* Set in meipt CSR, the priority-threshold */
  M_PSP_WRITE_CSR(D_PSP_MEIPT_NUM, uiThreshold);

  /* Restore interrupts */
  pspInterruptsRestore(uiInterruptsState);
}

/*
* This function sets the nesting priority threshold of the external interrupts in the PIC
*
* @param threshold = nesting priority threshold to be programmed to PIC
* @return None
*/
D_PSP_TEXT_SECTION void  pspExtInterruptsSetNestingPriorityThreshold(u32_t uiNestingPriorityThreshold)
{
  u32_t uiInterruptsState;

  /* Disable interrupts */
  pspInterruptsDisable(&uiInterruptsState);

  /* Set in meicidpl CSR, the nesting priority priority-threshold */
  M_PSP_WRITE_CSR(D_PSP_MEICIDPL_NUM, uiNestingPriorityThreshold);

  /* Set in meicurpl CSR, the nesting priority priority-threshold */
  M_PSP_WRITE_CSR(D_PSP_MEICURPL_NUM, uiNestingPriorityThreshold);

  /* Restore interrupts */
  pspInterruptsRestore(uiInterruptsState);
}

/*
* This function checks whether a given external interrupt is pending or not
*
* @param uiExtInterrupt = Number of external interrupt
* @return = pending (1) or not (0)
*/
D_PSP_TEXT_SECTION u32_t pspExtInterruptIsPending(u32_t uiExtInterrupt)
{
  u32_t uiRegister, uiBit, uiResult;

  /* Calculate the meipX register to access to check the input interrupt number */
  uiRegister = D_PSP_MEIP_ADDR + D_PSP_REG32_BYTE_WIDTH * (uiExtInterrupt >> D_PSP_SHIFT_5);

  /* Calculate the bit in meipX register to access to check the input interrupt number */
  uiBit = uiExtInterrupt - (uiRegister * D_PSP_REG32_BIT_WIDTH);

  /* Check the specific bit */
  uiResult = !!( M_PSP_READ_REGISTER_32(uiRegister) & (1 << uiBit) );

  return (uiResult);
}


/*
* This function sets external-interrupt type (Level-triggered or Edge-triggered)
*
* @param uiIntNum  = Number of external interrupt
* @param uiIntType = Type of the interrupt (level or edge)
*
*/
D_PSP_TEXT_SECTION void pspExtInterruptSetType(u32_t uiIntNum, u32_t uiIntType)
{
  u32_t uiInterruptsState;

  /* Assert on interrupt-type value */
  M_PSP_ASSERT((D_PSP_EXT_INT_LEVEL_TRIG_TYPE == uiIntType) || (D_PSP_EXT_INT_EDGE_TRIG_TYPE == uiIntType));

  /* Disable interrupts */
  pspInterruptsDisable(&uiInterruptsState);

  /* Set interrupt type */
  M_PSP_SET_REGISTER_32(D_PSP_PIC_MEIGWCTRL_ADDR + M_PSP_MULT_BY_4(uiIntNum), uiIntType << D_PSP_MEIGWCTRL_TYPE_BIT_OFFSET);

  /* Restore interrupts */
  pspInterruptsRestore(uiInterruptsState);
}


/*
* This function sets external-interrupt polarity (active-high or active-low)
*
* @param uiIntNum   = Number of external interrupt
* @param uiPolarity = active-high or active-low
*
*/
D_PSP_TEXT_SECTION void pspExtInterruptSetPolarity(u32_t uiIntNum, u32_t uiPolarity)
{
  u32_t uiInterruptsState;

  /* Assert on interrupt-type value */
  M_PSP_ASSERT((D_PSP_EXT_INT_ACTIVE_HIGH == uiPolarity) || (D_PSP_EXT_INT_ACTIVE_LOW == uiPolarity));

  /* Disable interrupts */
  pspInterruptsDisable(&uiInterruptsState);

  /* Set interrupt type */
  M_PSP_SET_REGISTER_32(D_PSP_PIC_MEIGWCTRL_ADDR + M_PSP_MULT_BY_4(uiIntNum), uiPolarity << D_PSP_MEIGWCTRL_POLARITY_BIT_OFFSET);

  /* Restore interrupts */
  pspInterruptsRestore(uiInterruptsState);
}

/*
* This function clears the indication of pending interrupt
*
* @param uiIntNum   = Number of external interrupt
*/
D_PSP_TEXT_SECTION void  pspExtInterruptClearPendingInt(u32_t uiIntNum)
{
  u32_t uiInterruptsState;

  /* Disable interrupts */
  pspInterruptsDisable(&uiInterruptsState);

  /* Clear the gwateway */
  M_PSP_WRITE_REGISTER_32(D_PSP_PIC_MEIGWCLR_ADDR + M_PSP_MULT_BY_4(uiIntNum), 0);

  /* Restore interrupts */
  pspInterruptsRestore(uiInterruptsState);
}

/*
* This function sets Priority Order (Standard or Reserved)
*
* @param uiPriorityOrder = Standard or Reserved
*
*/
D_PSP_TEXT_SECTION void pspExtInterruptSetPriorityOrder(u32_t uiPriorityOrder)
{
  u32_t uiInterruptsState;

  /* Assert on priority-order value */
  M_PSP_ASSERT((D_PSP_EXT_INT_STANDARD_PRIORITY == uiPriorityOrder) || (D_PSP_EXT_INT_REVERSED_PRIORITY == uiPriorityOrder));

  /* Disable interrupts */
  pspInterruptsDisable(&uiInterruptsState);

  /* Set Priority Order */
  M_PSP_WRITE_REGISTER_32(D_PSP_PIC_MPICCFG_ADDR, uiPriorityOrder);

  /* Restore interrupts */
  pspInterruptsRestore(uiInterruptsState);
}

/*
* This function gets the current selected external interrupt (claim-id)
*
* @return - claim-id number
*/
D_PSP_TEXT_SECTION u32_t pspExtInterruptGetClaimId(void)
{
  u32_t uiClaimId = (M_PSP_READ_CSR(D_PSP_MEIHAP_NUM) & D_PSP_MEIHAP_CLAIMID_MASK)  ;
  uiClaimId = uiClaimId >> D_PSP_SHIFT_2;

  return uiClaimId;
}


/*
* This function gets the priority of currently selected external interrupt
*
* @return - priority level
*/
D_PSP_TEXT_SECTION u32_t pspExtInterruptGetPriority(void )
{
  u32_t uiPriorityLevel = (M_PSP_READ_CSR(D_PSP_MEICIDPL_NUM) & D_PSP_MEICIDPL_CLIDPRI_MASK) ;

  return uiPriorityLevel;
}

/*
* External interrupt handler
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspExtInterruptIsr(void)
{
  fptrFunction fptrExtIntHandler = NULL;
  u32_t* pClaimId;

  /* Trigger capture of the interrupt source ID(handler address), write '1' to meicpct */
  M_PSP_WRITE_CSR(D_PSP_MEICPCT_NUM, D_PSP_MEICPCT_CAPTURE_MASK);

  /* Obtain external interrupt handler address from meihap register */
  pClaimId = (u32_t*)M_PSP_READ_CSR(D_PSP_MEIHAP_NUM);

  fptrExtIntHandler = *((fptrFunction)pClaimId);

  M_PSP_ASSERT(fptrExtIntHandler != NULL);

  fptrExtIntHandler();
}

