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

/**
* external prototypes
*/

/**
* global variables
*/


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
D_PSP_TEXT_SECTION void pspExtInterruptSetPriority(u32_t uiIntNum, u32_t uiPriority)
{
	/* TO DO: add functionality here when relevant for HiFive*/
}

/*
*  This function sets the priority threshold of the external interrupts (empty function)
*/
D_PSP_TEXT_SECTION void pspExtInterruptsSetThreshold(u32_t uiThreshold)
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
* External interrupt handler
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspExternalIntHandlerIsr(void)
{
	/* TODO: add functionality here when relevant for HiFive*/
}

