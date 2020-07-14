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
#include "psp_macros.h"
#include "comrv_api.h"
#include "demo_platform_al.h"
#include "demo_utils.h"

/**
* definitions
*/
void _OVERLAY_ OverlayFunc0(void);
void _OVERLAY_ OverlayFunc1(void);
void _OVERLAY_ OverlayFunc2(void);
void _OVERLAY_ OverlayFunc3(void);
void _OVERLAY_ OvlTestFunc0(void);
void _OVERLAY_ OvlTestFunc1(void);
void _OVERLAY_ OvlTestFunc2(void);
void _OVERLAY_ OvlTestFunc3(void);
void _OVERLAY_ OvlTestFunc4(void);
void _OVERLAY_ OvlTestFunc5(void);
void _OVERLAY_ OvlTestFunc6(void);
void _OVERLAY_ OvlTestFunc7(void);

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

/**
* external prototypes
*/

/**
* global variables
*/
funcPtr myFunc;
volatile u32_t globalCount = 0;
volatile u32_t gOverlayFunc0 = 0;
volatile u32_t gOverlayFunc1 = 0;
volatile u32_t gOverlayFunc2 = 0;

/**
* functions
*/
void _OVERLAY_ OverlayFunc3(void)
{
   gOverlayFunc2+=3;
}

/* overlay function 2 */
void _OVERLAY_ OverlayFunc2(void)
{
   gOverlayFunc2+=4;
   OverlayFunc3();
}

/* overlay function 1 */
void _OVERLAY_ OverlayFunc1(void)
{
   gOverlayFunc1+=3;
   myFunc();
   gOverlayFunc1+=2;
}

/* overlay function 0 */
void _OVERLAY_ OverlayFunc0(void)
{
   gOverlayFunc0+=1;
   OverlayFunc1();
   gOverlayFunc0+=2;
}

void demoStart(void)
{
   comrvInitArgs_t stComrvInitArgs;

   M_DEMO_START_PRINT();

   /* Register interrupt vector */
   pspInterruptsSetVectorTableAddress(&M_PSP_VECT_TABLE);

   /* mark that comrv init shall load comrv tables */
   stComrvInitArgs.ucCanLoadComrvTables = 1;

   /* init comrv */
   comrvInit(&stComrvInitArgs);

   /* demonstrate function pointer usage with multigroups */
   myFunc = OverlayFunc2;

   globalCount+=1;
   OverlayFunc0();
   globalCount+=1;

   /* verify function calls where completed successfully */
   if (globalCount != 2 || gOverlayFunc0 != 3 ||
       gOverlayFunc1 != 5 || gOverlayFunc2 != 7)
   {
      /* loop forever */
      M_DEMO_ENDLESS_LOOP();
   }

   /* GDB test1 */
   OvlTestFunc1();
   OvlTestFunc0();
   OvlTestFunc2();
   OvlTestFunc3();
   OvlTestFunc0();
   OvlTestFunc0();

   /* GDB test2 */
   OvlTestFunc0();
   OvlTestFunc1();
   OvlTestFunc2();
   OvlTestFunc0();
   OvlTestFunc4();
   OvlTestFunc5();
   OvlTestFunc6();
   OvlTestFunc7();
   OvlTestFunc4();
   OvlTestFunc5();
   OvlTestFunc6();
   OvlTestFunc7();

   /* GDB test4 */
   OvlTestFunc2();
   OvlTestFunc3();
   OvlTestFunc4();
   OvlTestFunc7();
   OvlTestFunc5();
   OvlTestFunc2();
   OvlTestFunc3();
   OvlTestFunc1();
   OvlTestFunc7();
   OvlTestFunc4();
   OvlTestFunc5();

   M_DEMO_END_PRINT();
}

void _OVERLAY_ OvlTestFunc0(void)
{
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
}

void _OVERLAY_ OvlTestFunc1(void)
{
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
}

void _OVERLAY_ OvlTestFunc2(void)
{
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
}

void _OVERLAY_ OvlTestFunc3(void)
{
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
}

void _OVERLAY_ OvlTestFunc4(void)
{
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
}

void _OVERLAY_ OvlTestFunc5(void)
{
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
}

void _OVERLAY_ OvlTestFunc6(void)
{
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
}
