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

/**
* types
*/


/**
* local prototypes
*/
void OVL_OverlayFunc0 OverlayFunc0(void);
void OVL_OverlayFunc1 OverlayFunc1(void);

/**
* external prototypes
*/

/**
* global variables
*/

/**
* functions
*/

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

void demoStart(void)
{
   comrvInitArgs_t stComrvInitArgs;
   comrvStatus_t stComrvStatus;

   M_DEMO_START_PRINT();

   /* Register interrupt vector */
   pspMachineInterruptsSetVecTableAddress(&M_PSP_VECT_TABLE);

   /* mark that comrv init shall load comrv tables */
   stComrvInitArgs.ucCanLoadComrvTables = 1;

   /* init comrv */
   comrvInit(&stComrvInitArgs);

   /* get comrv CB */
   comrvGetStatus(&stComrvStatus);

   OverlayFunc0();
   OverlayFunc1();

#ifdef D_COMRV_EVICTION_LRU

   /* verify lru/mru are correct */
   if (stComrvStatus.pComrvCB->ucLruIndex != 0 &&
       stComrvStatus.pComrvCB->ucMruIndex != 1)
   {
      M_DEMO_ENDLESS_LOOP();
   }

   /* call a loaded overlay function */
   OverlayFunc0();

   /* verify lru/mru are correct - we expect them to change */
   if (stComrvStatus.pComrvCB->ucLruIndex != 1 &&
       stComrvStatus.pComrvCB->ucMruIndex != 0)
   {
      M_DEMO_ENDLESS_LOOP();
   }

#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */

   /* reset comrv, keep the 'offset' and 'multi-group' tables loaded and "clean" history of loaded groups */
   comrvReset(E_RESET_TYPE_LOADED_GROUPS);

   OverlayFunc1();
   OverlayFunc0();

#ifdef D_COMRV_EVICTION_LRU
   /* verify lru/mru are correct - we expect them to change */
   if (stComrvStatus.pComrvCB->ucLruIndex != 0 &&
       stComrvStatus.pComrvCB->ucMruIndex != 1)
   {
      M_DEMO_ENDLESS_LOOP();
   }
#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */

   M_DEMO_END_PRINT();
}
