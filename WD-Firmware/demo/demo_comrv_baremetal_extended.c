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
* include files
*/
#include "common_types.h"
#include "comrv_api.h"
#include "psp_api.h"
#include "demo_platform_al.h"
#include "demo_utils.h"

/**
* definitions
*/
#define M_5_NOPS() \
   asm volatile ("nop");  \
   asm volatile ("nop");  \
   asm volatile ("nop");  \
   asm volatile ("nop");  \
   asm volatile ("nop");  \

#define M_10_NOPS() \
   M_5_NOPS() \
   M_5_NOPS()

#define M_50_NOPS() \
   M_10_NOPS() \
   M_10_NOPS() \
   M_10_NOPS() \
   M_10_NOPS() \
   M_10_NOPS()

#define M_100_NOPS() \
   M_50_NOPS() \
   M_50_NOPS()

#define M_512B_NOPS() \
   M_100_NOPS() \
   M_100_NOPS() \
   M_50_NOPS()  \
   M_5_NOPS()   \
   asm volatile ("nop");

#define M_1K_NOPS() \
   M_512B_NOPS() \
   M_512B_NOPS()

/**
* macros
*/

/**
* types
*/
typedef void (*funcPtr)(void);

/**
* local prototypes
*/
void NoneOverlayFunc(void);
void _OVERLAY_ OvlTestFunc512B1(void);
void _OVERLAY_ OvlTestFunc512B2(void);
void _OVERLAY_ OvlTestFuncSameGroup512B1(void);
void _OVERLAY_ OvlTestFunc4K(void);
void _OVERLAY_ OvlTestFuncSameGroup512B2(void);
void _OVERLAY_ OvlTestFunc6(void);
void _OVERLAY_ OvlTestFunc7(void);

/**
* external prototypes
*/

/**
* global variables
*/
/* demonstrate function pointer usage with multigroups */
funcPtr pOvlFunc = OvlTestFunc6;

/**
* functions
*/
void NoneOverlayFunc(void)
{
   /* call overlay function - step in multi-group  */
   OvlTestFunc512B2();
}

void _OVERLAY_ OvlTestFunc512B1(void)
{
   /* step out of overlay function */
   asm volatile ("nop");
}

void _OVERLAY_ OvlTestFunc512B2(void)
{/* check the call stack and step out */
   asm volatile ("nop");
   asm volatile ("nop");
}

void _OVERLAY_ OvlTestFunc4K(void)
{
   M_1K_NOPS();
   M_1K_NOPS();
   M_1K_NOPS();
   M_512B_NOPS();
   asm volatile ("nop");
   /* resume and break */
}

void _OVERLAY_ OvlTestFunc1K(void)
{
   M_512B_NOPS();
   asm volatile ("nop");
}

void _OVERLAY_ OvlTestFunc6(void)
{
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
}

void _OVERLAY_ OvlTestFunc7(void)
{
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
}

void _OVERLAY_ OvlTestFuncSameGroup512B1(void)
{
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
}

void _OVERLAY_ OvlTestFuncSameGroup512B2(void)
{
   /* call overlay function -  step in large group (set BP in loaded
	  function which is about to be evicted) */
   OvlTestFunc4K();
   /* call non-overlay function - step in */
   NoneOverlayFunc();
   /* call overlay function -  step in via function pointer */
   pOvlFunc();
}

/* overlay function */
void _OVERLAY_ OvlTestFunc(void)
{
   /* call overlay function - step in (skip disabled) */
   OvlTestFunc512B1();
   /* call overlay function - step over (skip enabled) */
   OvlTestFuncSameGroup512B1();
   /* call overlay function - resume (set BP in loaded function) */
   OvlTestFunc1K();
   /* call overlay function - step in (already loaded) */
   OvlTestFuncSameGroup512B2();
}

void demoStart(void)
{
   comrvInitArgs_t stComrvInitArgs;

   M_DEMO_START_PRINT();

   /* Register interrupt vector */
   pspMachineInterruptsSetVecTableAddress(&M_PSP_VECT_TABLE);

   /* mark that comrv init shall load comrv tables */
   stComrvInitArgs.ucCanLoadComrvTables = 1;

   /* init comrv */
   comrvInit(&stComrvInitArgs);
   /* comrv static data view */
   /* comrv loaded groups view */
   /* call overlay function - step in (skip enabled) */
   OvlTestFunc();

   M_DEMO_END_PRINT();
}
