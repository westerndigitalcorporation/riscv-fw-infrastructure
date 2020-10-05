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
* include files
*/
#include "common_types.h"
#include "demo_platform_al.h"
#include "demo_utils.h"
#include "cti_api.h"
#include "comrv_api.h"
#include "psp_api.h"

/**
* definitions
*/
#define M_DEMO_COMRV_RTOS_FENCE()   M_PSP_INST_FENCE(); \
                                    M_PSP_INST_FENCEI();

/**
* macros
*/

/**
* types
*/
typedef u32_t (*DemoExternalErrorHook_t)(const comrvErrorArgs_t* pErrorArgs);

/**
* local prototypes
*/

/**
* external prototypes
*/
extern void* _OVERLAY_STORAGE_START_ADDRESS_;
extern u32_t xcrc32(const u08_t *pBuf, s32_t siLen, u32_t uiInit);

/**
* global variables
*/
DemoExternalErrorHook_t fptrDemoExternalErrorHook = NULL;

/**
* functions
*/
void demoStart(void)
{
   comrvInitArgs_t stComrvInitArgs = { 1 };

   M_DEMO_START_PRINT();

   /* Register interrupt vector */
   pspInterruptsSetVectorTableAddress(&M_PSP_VECT_TABLE);

   /* Init ComRV engine */
   comrvInit(&stComrvInitArgs);

   /* run the tests */
   ctiMain();

   M_DEMO_END_PRINT();
}

/**
* memory copy hook
*
* @param  none
*
* @return none
*/
void comrvMemcpyHook(void* pDest, void* pSrc, u32_t uiSizeInBytes)
{
   u32_t loopCount = uiSizeInBytes/(sizeof(u32_t)), i;
   /* copy dwords */
   for (i = 0; i < loopCount ; i++)
   {
      *((u32_t*)pDest + i) = *((u32_t*)pSrc + i);
   }
   loopCount = uiSizeInBytes - (loopCount*(sizeof(u32_t)));
   /* copy bytes */
   for (i = (i-1)*(sizeof(u32_t)) ; i < loopCount ; i++)
   {
      *((u08_t*)pDest + i) = *((u08_t*)pSrc + i);
   }
}

/**
* load overlay group hook
*
* @param pLoadArgs - refer to comrvLoadArgs_t for exact args
*
* @return loaded address or NULL if unable to load
*/
void* comrvLoadOvlayGroupHook(comrvLoadArgs_t* pLoadArgs)
{
   g_stCtiOvlFuncsHitCounter.uiComrvLoad++;
   comrvMemcpyHook(pLoadArgs->pDest, (u08_t*)&_OVERLAY_STORAGE_START_ADDRESS_ + pLoadArgs->uiGroupOffset, pLoadArgs->uiSizeInBytes);
   /* it is upto the end user of comrv to synchronize the instruction and data stream after
      overlay data has been written to destination memory */
   M_DEMO_COMRV_RTOS_FENCE();
   return pLoadArgs->pDest;
}

/**
* set a function pointer to be called by comrvErrorHook
*
* @param  pErrorArgs - pointer to error arguments
*
* @return none
*/
void demoComrvSetErrorHandler(void* fptrAddress)
{
   fptrDemoExternalErrorHook = fptrAddress;
}

/**
* error hook
*
* @param  pErrorArgs - pointer to error arguments
*
* @return none
*/
void comrvErrorHook(const comrvErrorArgs_t* pErrorArgs)
{
   comrvStatus_t stComrvStatus;
   comrvGetStatus(&stComrvStatus);
   /* if external error handler was set e.g. cti */
   if (fptrDemoExternalErrorHook == NULL)
   {
      /* we can't continue so loop forever */
      M_DEMO_ERR_PRINT();
      M_DEMO_ENDLESS_LOOP();
   }
   else
   {
      fptrDemoExternalErrorHook(pErrorArgs);
   }
}

/**
* Invalidate data cache hook
*
* @param pAddress         - memory address to invalidate
*        uiNumSizeInBytes - number of bytes to invalidate
*
* @return none
*/
void comrvInvalidateDataCacheHook(const void* pAddress, u32_t uiNumSizeInBytes)
{
   (void)pAddress;
   (void)uiNumSizeInBytes;
}

/**
* crc calculation hook
*
* @param pAddress         - memory address to calculate
*        memSizeInBytes   - number of bytes to calculate
*        uiExpectedResult - expected crc result
*
* @return calculated CRC
*/
#ifdef D_COMRV_ENABLE_CRC_SUPPORT
u32_t comrvCrcCalcHook(const void* pAddress, u16_t usMemSizeInBytes, u32_t uiExpectedResult)
{
   volatile u32_t uiCrc;
   uiCrc = xcrc32(pAddress, usMemSizeInBytes, 0xffffffff);
   return !(uiExpectedResult == uiCrc);
}
#endif /* D_COMRV_ENABLE_CRC_SUPPORT */

