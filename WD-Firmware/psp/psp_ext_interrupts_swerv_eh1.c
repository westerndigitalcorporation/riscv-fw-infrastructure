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
* @file   psp_ext_interrupts_swerv_eh1.c
* @author Nati Rapaport
* @date   15.03.2020
* @brief  The file supplies external interrupt services. The file is specific to SweRV EH1 specifications
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
    #error "Definition of number of External interrupts in PIC is missing"
#else
    #define PSP_PIC_NUM_OF_EXT_INTERRUPTS D_PIC_NUM_OF_EXT_INTERRUPTS
#endif

/* 1'st Ext-Interrupt source could be any number (including 0) so there's no point to check with #if.. */
#define PSP_EXT_INTERRUPT_FIRST_SOURCE_USED    D_EXT_INTERRUPT_FIRST_SOURCE_USED

/* Number of last External interrupt source in the PIC */
#if (0 == D_EXT_INTERRUPT_LAST_SOURCE_USED)
    #error "Definition of last External interrupt source is missing"
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

D_PSP_TEXT_SECTION void pspExternalInterruptDisableNumber(u32_t uiIntNum);
D_PSP_TEXT_SECTION void pspExternalInterruptEnableNumber(u32_t uiIntNum);
D_PSP_TEXT_SECTION void pspExternalInterruptSetPriority(u32_t uiIntNum, u32_t uiPriority);
D_PSP_TEXT_SECTION void pspExternalInterruptsSetThreshold(u32_t uiThreshold);
D_PSP_TEXT_SECTION pspInterruptHandler_t pspExternalInterruptRegisterISR(u32_t uiVectorNumber, pspInterruptHandler_t pIsr, void* pParameter);


// NatiR - continue with these. Check what they are
//void pspExtIntSetInterruptMode(u32_t uiInterruptNum, u32_t uiInterruptMode);
//void PSP_intStartup_vect (void);


/**
* external prototypes
*/

/**
* global variables
*/
D_PSP_DATA_SECTION void (*g_fptrPspExternalInterruptDisableNumber)(u32_t uiIntNum)                 = pspExternalInterruptDisableNumber;
D_PSP_DATA_SECTION void (*g_fptrPspExternalInterruptEnableNumber)(u32_t uiIntNum)                  = pspExternalInterruptEnableNumber;
D_PSP_DATA_SECTION void (*g_fptrPspExternalInterruptSetPriority)(u32_t uiIntNum, u32_t uiPriority) = pspExternalInterruptSetPriority;
D_PSP_DATA_SECTION void (*g_fptrPspExternalInterruptSetThreshold)(u32_t uiThreshold)               = pspExternalInterruptsSetThreshold;
D_PSP_DATA_SECTION pspInterruptHandler_t (*g_fptrPspExternalInterruptRegisterISR)(u32_t uiVectorNumber, pspInterruptHandler_t pIsr, void* pParameter) = pspExternalInterruptRegisterISR;


/* External interrupt handlers Global Table */
D_PSP_DATA_SECTION pspInterruptHandler_t G_Ext_Interrupt_Handlers[PSP_PIC_NUM_OF_EXT_INTERRUPTS];



/**
* This function disables a specified external interrupt in the PIC
*
* @param intNum = the number of the external interrupt to disable
* @return None
*/
D_PSP_TEXT_SECTION void pspExternalInterruptDisableNumber(u32_t uiIntNum)
{
	/* Clear Int-Enable bit in meie register, corresponds to given source (interrupt-number) */
	M_PSP_WRITE_REGISTER_32(D_PSP_PIC_MEIE_ADDR + D_PSP_MULT_BY_4(uiIntNum) , 0);
}

/*
* This function enables a specified external interrupt in the PIC
*
* @param intNum = the number of the external interrupt to enable
* @return None
*/
D_PSP_TEXT_SECTION void pspExternalInterruptEnableNumber(u32_t uiIntNum)
{
	/* Set Int-Enable bit in meie register, corresponds to given source (interrupt-number) */
	M_PSP_WRITE_REGISTER_32((D_PSP_PIC_MEIE_ADDR + D_PSP_MULT_BY_4(uiIntNum)), D_PSP_MEIE_INT_EN_MASK);
}

/*
*  This function sets the priority of a specified external interrupt
*
*  @param intNum = the number of the external interrupt to disable
*  @param priority = priority to be set
* @return None
*/
D_PSP_TEXT_SECTION void pspExternalInterruptSetPriority(u32_t uiIntNum, u32_t uiPriority)
{
	/* Set priority in meipl register, corresponds to given source (interrupt-number) */
	M_PSP_WRITE_REGISTER_32((D_PSP_MEIPL_ADDR + D_PSP_MULT_BY_4(uiIntNum)), uiPriority);
}

/*
* This function sets the priority threshold of the external interrupts in the PIC
*
* @param threshold = priority threshold to be programmed to PIC
* @return None
*/
D_PSP_TEXT_SECTION void pspExternalInterruptsSetThreshold(u32_t uiThreshold)
{
	/* Set in meipt CSR, the priority-threshold */
	M_PSP_WRITE_CSR(D_PSP_MEIPT_NUM, uiThreshold);
}

/*
* This function registers external interrupt handler
*
* @param uiVectorNumber = the number of the external interrupt to register
*        pIsr = the ISR to register
*        pParameter = NOT IN USED for baremetal implementation
* @return pOldIsr = pointer to the previously registered ISR
*/
D_PSP_TEXT_SECTION pspInterruptHandler_t pspExternalInterruptRegisterISR(u32_t uiVectorNumber, pspInterruptHandler_t pIsr, void* pParameter)
{
   pspInterruptHandler_t fptrPrevIsr = NULL;

   /* Assert if uiVectorNumber is beyond first or last interrupts-in-use */
   M_PSP_ASSERT((PSP_EXT_INTERRUPT_FIRST_SOURCE_USED <= uiVectorNumber)  && (PSP_EXT_INTERRUPT_LAST_SOURCE_USED >= uiVectorNumber))

   /* Register the interrupt */
   fptrPrevIsr = G_Ext_Interrupt_Handlers[uiVectorNumber];
   G_Ext_Interrupt_Handlers[uiVectorNumber] = pIsr;

   /* Make sure changes are synced */
   M_PSP_INST_FENCEI();

   return(fptrPrevIsr);
}

/**
* default empty external interrupt handler
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspExternalInterruptDefaultEmptyIsr(void)
{
	M_PSP_EBREAK();
}

/**
* External interrupt handler
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspExternalIntHandlerIsr(void)
{
	fptrFunction fptrExtIntHandler = NULL;
	u32_t* pClaimId;

	/* Trigger capture of the interrupt source ID(handler address), write '1' to meicpct */
	M_PSP_WRITE_CSR(D_PSP_MEICPCT_NUM, 0x1);

	/* Obtain external interrupt handler address from meihap register */
	pClaimId = (u32_t*)M_PSP_READ_CSR(D_PSP_MEIHAP_NUM);

	fptrExtIntHandler = *((fptrFunction)pClaimId);

	M_PSP_ASSERT(pExtIntHandler != NULL);

	fptrExtIntHandler();
}


