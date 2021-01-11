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
* @file   demo_comrv_instrumentation.c
* @author Ronen Haen
* @date   06.01.2021
* @brief  Demo application for COMRV instrumentation
*/

/**
* include files
*/
#include "common_types.h"
#include "psp_api.h"
#include "demo_platform_al.h"
#include "demo_utils.h"
#include "comrv_api.h"

/**
* definitions
*/

/**
* macros
*/
#define M_DEMO_5_NOPS()     \
   asm volatile ("nop");    \
   asm volatile ("nop");    \
   asm volatile ("nop");    \
   asm volatile ("nop");    \
   asm volatile ("nop");

#define M_DEMO_10_NOPS()    \
      M_DEMO_5_NOPS()	    \
      M_DEMO_5_NOPS()

#define M_DEMO_50_NOPS()    \
      M_DEMO_10_NOPS()    \
      M_DEMO_10_NOPS()    \
      M_DEMO_10_NOPS()    \
      M_DEMO_10_NOPS()    \
      M_DEMO_10_NOPS()

#define M_DEMO_100_NOPS()   \
      M_DEMO_50_NOPS()    \
      M_DEMO_50_NOPS()    \

#define M_DEMO_260_NOPS()   \
      M_DEMO_100_NOPS()   \
      M_DEMO_100_NOPS()   \
      M_DEMO_50_NOPS()    \
      M_DEMO_10_NOPS()    \

/**
* types
*/

/**
* local prototypes
*/
void _OVERLAY_ OverlayFunc0(u32_t uiValidateInstrumentationValue);
void _OVERLAY_ OverlayFunc1(void);
void _OVERLAY_ OverlayFunc2(void);

/**
* external prototypes
*/

/**
* global variables
*/
extern comrvInstrumentationArgs_t g_stInstArgs;

/**
* functions
*/

/* overlay function 2 - 1024 bytes */
void _OVERLAY_ OverlayFunc2(void)
{
   /* verify load and invoke for OverlayFunc2() */
   if (g_stInstArgs.uiInstNum != D_COMRV_LOAD_AND_INVOKE_IND)
   {
      M_DEMO_ENDLESS_LOOP();
   }

   /* verify load and invoke for OverlayFunc0() */
   OverlayFunc0(D_COMRV_LOAD_AND_INVOKE_IND);

   /* verify load and return for OverlayFunc2() -
      OverlayFunc2 was evicted when OverlayFunc0 was called  */
   if (g_stInstArgs.uiInstNum != D_COMRV_LOAD_AND_RETURN_IND)
   {
      M_DEMO_ENDLESS_LOOP();
   }

   /* this is to force OverlayFunc2() to be in a 1024 bytes overlay group */
   M_DEMO_260_NOPS();
}

/* overlay function 1 - no load and invoke */
void _OVERLAY_ OverlayFunc1(void)
{
   /* verify load and invoke - call to OverlayFunc1() */
   if (g_stInstArgs.uiInstNum != D_COMRV_LOAD_AND_INVOKE_IND)
   {
      M_DEMO_ENDLESS_LOOP();
   }

   /* call a loaded overlay function - verify no load and invoke
      OverlayFunc0 already loaded */
   OverlayFunc0(D_COMRV_NO_LOAD_AND_INVOKE_IND);

   /* verify no load and return - returned to  OverlayFunc1 */
   if (g_stInstArgs.uiInstNum != D_COMRV_NO_LOAD_AND_RETURN_IND)
   {
      M_DEMO_ENDLESS_LOOP();
   }
}

/* overlay function 0 */
void _OVERLAY_ OverlayFunc0(u32_t uiExpectedInstrumentationValue)
{
   /* verify expected */
   if (g_stInstArgs.uiInstNum != uiExpectedInstrumentationValue)
   {
      M_DEMO_ENDLESS_LOOP();
   }
}

void demoStart(void)
{
   comrvInitArgs_t stComrvInitArgs;

   M_DEMO_START_PRINT();

   /* mark that comrv init shall load comrv tables */
   stComrvInitArgs.ucCanLoadComrvTables = 1;

   /* init comrv */
   comrvInit(&stComrvInitArgs);

   /* Register interrupt vector */
   pspMachineInterruptsSetVecTableAddress(&M_PSP_VECT_TABLE);

   /* call overlay function 0 - verify load and invoke */
   OverlayFunc0(D_COMRV_LOAD_AND_INVOKE_IND);

   /* call overlay function 1 - verify no load and invoke, no load and return */
   OverlayFunc1();

   /* call overlay function 2 - verify load and return */
   OverlayFunc2();

   /* demo completed */
   M_DEMO_END_PRINT();
}
