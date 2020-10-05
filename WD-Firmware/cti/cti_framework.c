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
* @file   cti_framework.c
* @author Ronen Haen
* @date   01.07.2020
* @brief  The file implements cti framework
*/

/**
* include files
*/
#include "common_types.h"
#include "psp_api.h"
#include "cti.h"
#include "comrv_api.h"

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

S_FRAMEWORK_CONTROL_BLOCK g_stCtiFrameWorkCB;
S_FW_CB_PTR g_pCtiFrameWorkCB = &g_stCtiFrameWorkCB;

/**
* @brief
*
* @param  None
*
* @return None
*/
void ctiInitializeGFrameWorkCB(void)
{
   g_stCtiFrameWorkCB.uiIsValidData = 0;
   g_stCtiFrameWorkCB.uiTestCurrent = 0;
   g_stCtiFrameWorkCB.uiTestSharedMemory = 0;
   g_stCtiFrameWorkCB.eTestErrorBitmap = E_TEST_ERROR_NO_ERROR;
   g_stCtiFrameWorkCB.uiTestSyncPoint = 0;
}

/**
* @brief
*
* @param  None
*
* @return None
*/
u32_t ctiGetCurrentSyncPoint(void)
{
   return g_stCtiFrameWorkCB.uiTestSyncPoint;
}

/**
* @brief
*
* @param  None
*
* @return None
*/
void ctiSetCurrentSyncPoint(u32_t uiNewTestSyncPointVal)
{
   g_stCtiFrameWorkCB.uiTestSyncPoint = uiNewTestSyncPointVal;
}

/**
* @brief main COMRV test functions multiplexer
*
* @param  uiPparam
*
* @return None
*/
static void ctiMainOvlMuxTask(u32_t uiParam)
{
   E_TEST_ERROR eTestError;
   E_CB_TEST_OVL eTestId = g_pCtiFrameWorkCB->uiTestCurrent;
   if (E_CB_TEST_OVL_MAX > eTestId )
   {
      ctiResetOvlGlobal();
      eTestError = g_pLookupTableCtiTestOvl[eTestId](g_pCtiFrameWorkCB);
      if (eTestError != E_TEST_ERROR_NO_ERROR)
      {
         M_PSP_EBREAK();
      }
      g_pCtiFrameWorkCB->uiTestCurrent++;
   }
   else
   {
      // we need to reset the global variable in the last iteration
      ctiInitializeGFrameWorkCB();
   }
}

/**
* @brief COMRV TI main/task a entry point
*
* @param  None
*
* @return E_CTI_RESULT
*/
void ctiMain()
{
   /* if this is the first time we are using this global, it would need to be initialized */
   ctiInitializeGFrameWorkCB();

   while (g_pCtiFrameWorkCB->uiTestCurrent < E_CB_TEST_OVL_MAX)
   {
      ctiMainOvlMuxTask(0);
      /* Re init comrv cache control block - excluding offset/multi-group entry */
      comrvReset(E_RESET_TYPE_CACHE);
   }
}

/**
* @brief COMRV TI task b entry point
*
* @param  None
*
* @return E_CTI_RESULT
*/
void f_cti_task()
{
   ctiTaskBTest();
}
