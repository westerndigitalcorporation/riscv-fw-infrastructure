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
* @file   cti_utilities.c
* @author Ronen Haen
* @date   01.07.2020
* @brief  The file implements cti framework
*/

/**
* include files
*/
#include "common_types.h"
#include "cti.h"
#include "psp_api.h"

/**
* definitions
*/

/**
* macros
*/
#define M_CTI_INT_DISABLE(pPrevIntState) pspMachineInterruptsDisable(pPrevIntState)
#define M_CTI_INT_ENABLE(uiPrevIntState)  pspMachineInterruptsRestore(uiPrevIntState)

/**
* types
*/

/**
* local prototypes
*/

/**
* external prototypes
*/
extern stCtiFuncsHitCounter  g_stCtiOvlFuncsHitCounter;

/**
* global variables
*/

/**
* @brief secure setter for E_TEST_ERROR
*
* @param  pCtiFrameWorkCB - Value that we want to assign
* @param  eValue - Pointer to E_TEST_ERROR
*
* @return E_CTI_RESULT
*/
E_CTI_RESULT ctiSetErrorBit(S_FW_CB_PTR pCtiFrameWorkCB, E_TEST_ERROR eValue)
{
   u32_t uiPrevIntState;
   E_CTI_RESULT eRes;

   if (pCtiFrameWorkCB == NULL)
   {
      eRes = E_CTI_ERROR;
   }
   else
   {
      eRes = E_CTI_OK;
   }

   M_CTI_INT_DISABLE(&uiPrevIntState);

   pCtiFrameWorkCB->eTestErrorBitmap |= eValue;

   M_CTI_INT_ENABLE(uiPrevIntState);

   return E_CTI_OK;
}

/**
* @brief
*
* @param  pCtiFrameWorkCB
*
* @return E_CTI_RESULT
*/
E_TEST_ERROR ctiGetErrorBits(S_FW_CB_PTR pCtiFrameWorkCB)
{
   return pCtiFrameWorkCB->eTestErrorBitmap;
}

/**
* @brief reset the test ovl global struct and call reset
*
* @param  None
*
* @return None
*/
void ctiResetOvlGlobal(void)
{
   g_stCtiOvlFuncsHitCounter.uiComrvLoad = 0;
   g_stCtiOvlFuncsHitCounter.uiDefragCounter = 0;
   g_stCtiOvlFuncsHitCounter.uiErrorNum = 0;
}
