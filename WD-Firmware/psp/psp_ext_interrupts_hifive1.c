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
* @file   psp_ext_interrupts_hifive.c
* @author Nati Rapaport
* @date   15.03.2020
* @brief  The file supplies external interrupt services. The file is specific to hifive1 specifications
* 
*/

/**
* include files
*/
#include "psp_api.h"

/**
* definitions
*/


/**
* macros
*/

/**
* types
*/

/**
* local prototypes
*/

/* External interrupts functions (empty functions for HiFive) */
D_PSP_TEXT_SECTION void pspExternalInterruptDisableNumber(u32_t uiIntNum);
D_PSP_TEXT_SECTION void pspExternalInterruptEnableNumber(u32_t uiIntNum);
D_PSP_TEXT_SECTION void pspExternalInterruptSetPriority(u32_t uiIntNum, u32_t uiPriority);
D_PSP_TEXT_SECTION void pspExternalInterruptsSetThreshold(u32_t uiThreshold);
D_PSP_TEXT_SECTION pspInterruptHandler_t pspExternalInterruptRegisterISR(u32_t uiVectorNumber, pspInterruptHandler_t pIsr, void* pParameter);
D_PSP_TEXT_SECTION void pspExternalInterruptDefaultEmptyIsr(void);

/**
* external prototypes
*/

/**
* global variables
*/

/* External-interrupts function pointers */
D_PSP_DATA_SECTION void (*g_fptrPspExternalInterruptDisableNumber)(u32_t uiIntNum)                 = pspExternalInterruptDisableNumber;
D_PSP_DATA_SECTION void (*g_fptrPspExternalInterruptEnableNumber)(u32_t uiIntNum)                  = pspExternalInterruptEnableNumber;
D_PSP_DATA_SECTION void (*g_fptrPspExternalInterruptSetPriority)(u32_t uiIntNum, u32_t uiPriority) = pspExternalInterruptSetPriority;
D_PSP_DATA_SECTION void (*g_fptrPspExternalInterruptSetThreshold)(u32_t uiThreshold)               = pspExternalInterruptsSetThreshold;
D_PSP_DATA_SECTION pspInterruptHandler_t (*g_fptrPspExternalInterruptRegisterISR)(u32_t uiVectorNumber, pspInterruptHandler_t pIsr, void* pParameter) = pspExternalInterruptRegisterISR;




/**
* This function disables a specified external interrupt (empty function)
*/
D_PSP_TEXT_SECTION void pspExternalInterruptDisableNumber(u32_t uiIntNum)
{
	/* TO DO: add functionality here when relevant for HiFive*/
}

/*
* This function enables a specified external interrupt (empty function)
*/
D_PSP_TEXT_SECTION void pspExternalInterruptEnableNumber(u32_t uiIntNum)
{
	/* TO DO: add functionality here when relevant for HiFive*/
}

/*
*  This function sets the priority of a specified external interrupts (empty function)
*/
D_PSP_TEXT_SECTION void pspExternalInterruptSetPriority(u32_t uiIntNum, u32_t uiPriority)
{
	/* TO DO: add functionality here when relevant for HiFive*/
}

/*
*  This function sets the priority threshold of the external interrupts (empty function)
*/
D_PSP_TEXT_SECTION void pspExternalInterruptsSetThreshold(u32_t uiThreshold)
{
	/* TO DO: add functionality here when relevant for HiFive*/
}

/*
* This function register external function handler (empty function)
*/
D_PSP_TEXT_SECTION pspInterruptHandler_t pspExternalInterruptRegisterISR(u32_t uiVectorNumber, pspInterruptHandler_t pIsr, void* pParameter)
{
  /* TO DO: add functionality here when relevant for HiFive*/
  return 0 ;
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
