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

/**
* definitions
*/
#define M_DEMO_COMRV_RTOS_FENCE()   M_PSP_INST_FENCE(); \
                                    M_PSP_INST_FENCEI();

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

M_OVL_DUMMY_FUNCTION(16)

#define M_OVL_FUNCTIONS_CALL \
  OvlTestFunc_10_(); \
  OvlTestFunc_11_(); \
  OvlTestFunc_12_(); \
  OvlTestFunc_13_(); \
  OvlTestFunc_14_(); \
  OvlTestFunc_15_();

extern void* _OVERLAY_STORAGE_START_ADDRESS_;

#ifdef D_COMRV_FW_INSTRUMENTATION
comrvInstrumentationArgs_t g_stInstArgs;
#endif /* D_COMRV_FW_INSTRUMENTATION */

#define OVL_OverlayFunc0 _OVERLAY_
#define OVL_OverlayFunc1 _OVERLAY_
#define OVL_OverlayFunc2 _OVERLAY_
#define OVL_OverlayFunc3 _OVERLAY_

D_PSP_NO_INLINE void NonOverlayFunc(void);
void OVL_OverlayFunc0 OverlayFunc0(void);
void OVL_OverlayFunc1 OverlayFunc1(void);
void OVL_OverlayFunc2 OverlayFunc2(void);
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
   OvlTestFunc_16_();
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

M_OVL_FUNCTIONS_GENERATOR

void demoStart(void)
{
   comrvInitArgs_t stComrvInitArgs = { 1 };

   /* Register interrupt vector */
   M_PSP_WRITE_CSR(D_PSP_MTVEC_NUM, &psp_vect_table);

   /* Init ComRV engine */
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

   /* verify function calls where completed successfully */
   if (globalCount != 9 || gOverlayFunc0 != 48 ||
       gOverlayFunc1 != 7 || gOverlayFunc2 != 6)
   {
      /* loop forever */
      M_ENDLESS_LOOP();
   }

   /* check that the overlay group > 512B works */
   M_OVL_FUNCTIONS_CALL;
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
   comrvMemcpyHook(pLoadArgs->pDest, (u08_t*)&_OVERLAY_STORAGE_START_ADDRESS_ + pLoadArgs->uiGroupOffset, pLoadArgs->uiSizeInBytes);
   /* it is upto the end user of comrv to synchronize the instruction and data stream after
      overlay data has been written to destination memory */
   M_DEMO_COMRV_RTOS_FENCE();
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

