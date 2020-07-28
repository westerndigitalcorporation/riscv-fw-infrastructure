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
#include "psp_csrs.h"
#include "demo_utils.h"

/**
* definitions
*/
#define M_DEMO_COMRV_RTOS_FENCE()   M_PSP_INST_FENCE(); \
                                    M_PSP_INST_FENCEI();

#define M_DEMO_5_NOPS()    \
   asm volatile ("nop");  \
   asm volatile ("nop");  \
   asm volatile ("nop");  \
   asm volatile ("nop");  \
   asm volatile ("nop");

#define M_DEMO_10_NOPS()  \
   M_DEMO_5_NOPS();       \
   M_DEMO_5_NOPS();

#define M_DEMO_50_NOPS()  \
   M_DEMO_10_NOPS();      \
   M_DEMO_10_NOPS();      \
   M_DEMO_10_NOPS();      \
   M_DEMO_10_NOPS();      \
   M_DEMO_10_NOPS();

#define M_OVL_DUMMY_FUNCTION(x,y) \
  void _OVERLAY_ OvlTestFunc_##x##_() \
   { \
      M_DEMO_50_NOPS();     \
      M_DEMO_10_NOPS();     \
      OvlTestFunc_##y##_(); \
      M_DEMO_10_NOPS();     \
   };

#define M_OVL_DUMMY_FUNCTION_LEAF(x) \
   void _OVERLAY_ OvlTestFunc_##x##_() \
   { \
      M_DEMO_50_NOPS();     \
      M_DEMO_10_NOPS();     \
   };

#define M_OVL_FUNCTIONS_GENERATOR \
  M_OVL_DUMMY_FUNCTION_LEAF(11) \
  M_OVL_DUMMY_FUNCTION_LEAF(12) \
  M_OVL_DUMMY_FUNCTION_LEAF(13) \
  M_OVL_DUMMY_FUNCTION(14,11)   \
  M_OVL_DUMMY_FUNCTION(15,12)   \
  M_OVL_DUMMY_FUNCTION(16,13)


M_OVL_DUMMY_FUNCTION_LEAF(10)
M_OVL_FUNCTIONS_GENERATOR

#define M_OVL_FUNCTIONS_CALL \
  OvlTestFunc_14_(); \
  OvlTestFunc_15_(); \
  OvlTestFunc_16_();

#ifdef D_COMRV_FW_INSTRUMENTATION
comrvInstrumentationArgs_t g_stInstArgs;
#endif /* D_COMRV_FW_INSTRUMENTATION */

#define OVL_OverlayFunc0 _OVERLAY_
#define OVL_OverlayFunc1 _OVERLAY_
#define OVL_OverlayFunc2 _OVERLAY_
#define OVL_OverlayFunc3 _OVERLAY_
#define OVL_OverlayFunc4 _OVERLAY_

D_PSP_NO_INLINE void NonOverlayFunc(void);
void OVL_OverlayFunc0 OverlayFunc0(void);
void OVL_OverlayFunc1 OverlayFunc1(void);
void OVL_OverlayFunc2 OverlayFunc2(void);
void OVL_OverlayFunc4 OverlayFunc4(void);
u32_t OVL_OverlayFunc3 OverlayFunc3(u32_t uiVal1, u32_t uiVal2, u32_t uiVal3, u32_t uiVal4,
                                    u32_t uiVal5, u32_t uiVal6, u32_t uiVal7, u32_t uiVal8,
                                    u32_t uiVal9);

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
/* overlay function 4 */
void OVL_OverlayFunc4 OverlayFunc4(void)
{
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
}

/* non overlay function */
D_PSP_NO_INLINE void NonOverlayFunc(void)
{
   globalCount+=1;
   OverlayFunc2();
   globalCount+=2;
}

/* overlay function 3 - 8 args through regs + 1 through the stack*/
u32_t OVL_OverlayFunc3 OverlayFunc3(u32_t uiVal1, u32_t uiVal2, u32_t uiVal3, u32_t uiVal4,
                                    u32_t uiVal5, u32_t uiVal6, u32_t uiVal7, u32_t uiVal8,
                                    u32_t uiVal9)
{
   /* unlock the group holding OverlayFunc2 (we need 1K for
      overlay group containing OvlTestFunc_16_) */
   comrvLockUnlockOverlayGroupByFunction(OverlayFunc1, D_COMRV_GROUP_STATE_UNLOCK);
   /* call other overlay function to make sure args remain valid */
   OvlTestFunc_10_();
   return uiVal1+uiVal2+uiVal3+uiVal4+uiVal5+uiVal6+uiVal7+uiVal8+uiVal9;
}

/* overlay function 2 */
void OVL_OverlayFunc2 OverlayFunc2(void)
{
   gOverlayFunc2+=3;
   /* lock the group holding OverlayFunc2 */
   comrvLockUnlockOverlayGroupByFunction(OverlayFunc1, D_COMRV_GROUP_STATE_LOCK);
}

/* overlay function 1 */
void OVL_OverlayFunc1 OverlayFunc1(void)
{
   gOverlayFunc1+=3;
   NonOverlayFunc();
   gOverlayFunc1+=4;
}

/* overlay function 0 */
void OVL_OverlayFunc0 OverlayFunc0(void)
{
   OverlayFunc4();
   gOverlayFunc0+=1;
   myFunc();
   gOverlayFunc0+=2;
   NonOverlayFunc();
   /* check 9 args (8 will pass via regs + one additional via stack) */
   gOverlayFunc0+=OverlayFunc3(1,2,3,4,5,6,7,8,9);
}

#ifdef D_COMRV_CONTROL_SUPPORT
/* override comrv implementation */
void comrvEntryDisable(void)
{
}
#endif /* D_COMRV_CONTROL_SUPPORT */

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

#ifdef D_COMRV_CONTROL_SUPPORT
   /* check the disable API */
   comrvDisable();
   /* try to call an overlay function */
   OverlayFunc0();
   /* enable comrv */
   comrvEnable();
#endif /* D_COMRV_CONTROL_SUPPORT */

   /* demonstrate function pointer usage */
   myFunc = OverlayFunc1;

   globalCount+=1;
   OverlayFunc0();
   globalCount+=2;
   OverlayFunc4();

   /* verify function calls where completed successfully */
   if (globalCount != 9 || gOverlayFunc0 != 48 ||
       gOverlayFunc1 != 7 || gOverlayFunc2 != 6)
   {
      /* loop forever */
      M_DEMO_ENDLESS_LOOP();
   }

   /* check that the overlay group > 512B works */
   M_OVL_FUNCTIONS_CALL;

   /* at this point the entire cache is taken with 1 loaded group,
      lets add a call to a none loaded function which will break the loaded
      group */
   OverlayFunc4();

   M_DEMO_END_PRINT();
}

