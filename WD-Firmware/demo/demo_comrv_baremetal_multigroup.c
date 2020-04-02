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

/**
* definitions
*/
void _OVERLAY_ OverlayFunc0(void);
void _OVERLAY_ OverlayFunc1(void);
void _OVERLAY_ OverlayFunc2(void);
void _OVERLAY_ OverlayFunc3(void);

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

extern void psp_vect_table(void);

/**
* global variables
*/

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
   OverlayFunc2();
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
   comrvInitArgs_t stComrvInitArgs = { 1 };

   /* Register interrupt vector */
   pspInterruptsSetVectorTableAddress(&psp_vect_table);

   /* Init ComRV engine */
   comrvInit(&stComrvInitArgs);

   globalCount+=1;
   OverlayFunc0();
   globalCount+=1;

   /* verify function calls where completed successfully */
   if (globalCount != 2 || gOverlayFunc0 != 3 ||
       gOverlayFunc1 != 5 || gOverlayFunc2 != 7)
   {
      /* loop forever */
      M_ENDLESS_LOOP();
   }
}

