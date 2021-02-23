/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2021 Western Digital Corporation or its affiliates.
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
#include "psp_macros.h"
#include "comrv_api.h"
#include "demo_platform_al.h"
#include "psp_csrs.h"
#include "demo_utils.h"

/**
* definitions
*/
#define D_DEMO_ARR_SIZE 10
/**
* macros
*/
#define M_DEMO_5_NOPS()    \
   asm volatile ("nop");  \
   asm volatile ("nop");  \
   asm volatile ("nop");  \
   asm volatile ("nop");  \
   asm volatile ("nop");

#define OVL_OverlayFunc0 _OVERLAY_
#define OVL_OverlayFunc1 _OVERLAY_
#define OVL_OverlayFunc2 _OVERLAY_

/**
* types
*/


/**
* local prototypes
*/
void OVL_OverlayFunc0 OverlayFunc0(void);
void OVL_OverlayFunc1 OverlayFunc1(void);
void OVL_OverlayFunc1 OverlayFunc2(void);

/**
* external prototypes
*/

/**
* global variables
*/
_DATA_OVERLAY_ const u32_t uiSomeOverlayData[D_DEMO_ARR_SIZE] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

/**
* functions
*/
/* overlay function 2 */
void OVL_OverlayFunc2 OverlayFunc2(void)
{
   M_DEMO_5_NOPS();
}

/* overlay function 1 */
void OVL_OverlayFunc1 OverlayFunc1(void)
{
   M_DEMO_5_NOPS();
}

/* overlay function 0 */
void OVL_OverlayFunc0 OverlayFunc0(void)
{
   M_DEMO_5_NOPS();
}

void AdditionalDataOverlayExamples(void)
{
   u32_t uiIndex;
   const u32_t *pToSameOvlData, *pToOvlData;

   /* allocate the same data overlay (to simulate a call from a different code location) */
   pToSameOvlData = comrvDataOverlayAllocation(uiSomeOverlayData);

   /* release the RO data overlay uiSomeOverlayData - since there are 2 pointers
      referencing the same data overlay, it will still be available after releasing
      it once */
   comrvDataOverlayRelease(uiSomeOverlayData);

   /* call overlay functions - data overlay shouldn't be evicted */
   OverlayFunc0();
   OverlayFunc1();

   /* verify that the data didn't change and still loaded */
   for (uiIndex = 0 ; uiIndex < D_DEMO_ARR_SIZE ; uiIndex++)
   {
      if (pToSameOvlData[uiIndex] != uiIndex)
      {
         M_DEMO_ENDLESS_LOOP();
      }
   }

   /* re allocate the RO data overlay to make it MRU */
   pToOvlData = comrvDataOverlayAllocation(uiSomeOverlayData);

   /* release of RO data overlay */
   comrvDataOverlayRelease(uiSomeOverlayData);

   /* second release of RO data overlay */
   comrvDataOverlayRelease(uiSomeOverlayData);

   /* call overlay functions - since the data overlay isn't being referrenced anymore
      the following 2 calls shall take the entire cache (overriding the occupied data overlay memory)*/
   OverlayFunc0();
   OverlayFunc1();

   /* verify that the data overlay isn't available */
   for (uiIndex = 0 ; uiIndex < D_DEMO_ARR_SIZE ; uiIndex++)
   {
      if (pToSameOvlData[uiIndex] == uiIndex)
      {
         M_DEMO_ENDLESS_LOOP();
      }
   }

}

void demoStart(void)
{
   u32_t uiIndex;
   const u32_t *pToOvlData;
   comrvInitArgs_t stComrvInitArgs;

   M_DEMO_START_PRINT();

   /* Register interrupt vector */
   pspMachineInterruptsSetVecTableAddress(&M_PSP_VECT_TABLE);

   /* mark that comrv init shall load comrv tables */
   stComrvInitArgs.ucCanLoadComrvTables = 1;

   /* init comrv */
   comrvInit(&stComrvInitArgs);

   /* call overlay function */
   OverlayFunc0();
   OverlayFunc1();

   /* load the RO data overlay */
   pToOvlData = comrvDataOverlayAllocation(uiSomeOverlayData);

   /* verify the read data content */
   for (uiIndex = 0 ; uiIndex < D_DEMO_ARR_SIZE ; uiIndex++)
   {
      if (pToOvlData[uiIndex] != uiIndex)
      {
         M_DEMO_ENDLESS_LOOP();
      }
   }

   /* call OverlayFunc2 - same group as uiSomeOverlayData */
   OverlayFunc2();

   /* more examples */
   AdditionalDataOverlayExamples();

   M_DEMO_END_PRINT();
}
