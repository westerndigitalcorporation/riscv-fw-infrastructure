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
#define M_OVL_DUMMY_FUNCTION(x) \
  void _OVERLAY_ OvlTestFunc_##x##_() \
  { \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
    asm volatile ("nop");  \
  };

#define M_OVL_FUNCTIONS_GENERATOR \
  M_OVL_DUMMY_FUNCTION(10) \
  M_OVL_DUMMY_FUNCTION(11) \
  M_OVL_DUMMY_FUNCTION(12) \
  M_OVL_DUMMY_FUNCTION(13) \
  M_OVL_DUMMY_FUNCTION(14) \
  M_OVL_DUMMY_FUNCTION(15)

#define M_OVL_FUNCTIONS_CALL \
  OvlTestFunc_10_(); \
  OvlTestFunc_11_(); \
  OvlTestFunc_12_(); \
  OvlTestFunc_13_(); \
  OvlTestFunc_14_(); \
  OvlTestFunc_15_();

extern void* __OVERLAY_STORAGE_START__ADDRESS__;

#ifdef D_COMRV_FW_INSTRUMENTATION
comrvInstrumentationArgs_t g_stInstArgs;
#endif /* D_COMRV_FW_INSTRUMENTATION */

#define OVL_OverlayFunc0 _OVERLAY_
#define OVL_OverlayFunc1 _OVERLAY_
#define OVL_OverlayFunc2 _OVERLAY_

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

extern void psp_vect_table(void);

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

/* overlay function 2 */
void OVL_OverlayFunc2 OverlayFunc2(void)
{
   gOverlayFunc2+=3;
   /* lock the group holding OverlayFunc2 */
   comrvLockUnlockOverlayGroupByFunction(OverlayFunc2, D_COMRV_GROUP_STATE_LOCK);
}

/* non overlay function */
D_PSP_NO_INLINE void NonOverlayFunc(void)
{
   globalCount+=1;
   OverlayFunc2();
   globalCount+=2;
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
   gOverlayFunc0+=1;
   myFunc();
   gOverlayFunc0+=2;
}

//M_OVL_FUNCTIONS_GENERATOR

void demoStart(void)
{
   comrvInitArgs_t stComrvInitArgs = { 1 };

   /* Register interrupt vector */
   M_PSP_WRITE_CSR(mtvec, &psp_vect_table);

   /* Init ComRV engine */
   comrvInit(&stComrvInitArgs);

   /* demonstrate function pointer usage */
   myFunc = OverlayFunc1;

   globalCount+=1;
   OverlayFunc0();
   globalCount+=2;
   /* verify function calls where completed successfully */
   if (globalCount != 6 || gOverlayFunc0 != 3 ||
       gOverlayFunc1 != 7 || gOverlayFunc2 != 3)
   {
      /* loop forever */
      M_ENDLESS_LOOP();
   }
   /* unlock the group holding OverlayFunc2 */
   comrvLockUnlockOverlayGroupByFunction(OverlayFunc2, D_COMRV_GROUP_STATE_UNLOCK);
   // M_OVL_FUNCTIONS_CALL;
}

/**
* memory copy hook
*
* @param  none
*
* @return none
*/
void comrvMemcpyHook(void* pDest, void* pSrc, u32_t sizeInBytes)
{
   u32_t loopCount = sizeInBytes/(sizeof(u32_t)), i;
   /* copy dwords */
   for (i = 0; i < loopCount ; i++)
   {
      *((u32_t*)pDest + i) = *((u32_t*)pSrc + i);
   }
   loopCount = sizeInBytes - (loopCount*(sizeof(u32_t)));
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
   comrvMemcpyHook(pLoadArgs->pDest, (u08_t*)&__OVERLAY_STORAGE_START__ADDRESS__ + pLoadArgs->uiGroupOffset, pLoadArgs->uiSizeInBytes);
   return pLoadArgs->pDest;
}

/**
* notification hook
*
* @param  pInstArgs - pointer to instrumentation arguments
*
* @return none
*/
#ifdef D_COMRV_FW_INSTRUMENTATION
void comrvInstrumentationHook(const comrvInstrumentationArgs_t* pInstArgs)
{
   g_stInstArgs = *pInstArgs;
}
#endif /* D_COMRV_FW_INSTRUMENTATION */

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
   /* we can't continue so loop forever */
   while (1);
}

/**
* crc calculation hook (itt)
*
* @param pAddress         - memory address to calculate
*        memSizeInBytes   - number of bytes to calculate
*        uiExpectedResult - expected crc result
*
* @return calculated CRC
*/
u32_t comrvCrcCalcHook (const void* pAddress, u16_t usMemSizeInBytes, u32_t uiExpectedResult)
{
   return 0;
}

/******************** start temporary build issue workaround ****************/
void _kill(void)
{
}
void _sbrk(void)
{
}
void _getpid(void)
{
}
/******************** end temporary build issue workaround ****************/

