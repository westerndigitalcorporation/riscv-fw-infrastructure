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
* @file   comrv.c
* @author Ronen Haen
* @date   21.06.2019
* @brief  The file implements the COM-RV interfaces
* 
*/

/**
* include files
*/
#include "common_types.h"
#include "string.h"
#include "comrv_api.h"

/**
* definitions
*/
#define D_COMRV_NUM_OF_OVERLAY_ENTRIES 1
#define D_COMRV_END_OF_STACK           0xDEAD

/**
* macros
*/
#define M_COMRV_READ_TOKEN_REG(x)       asm volatile ("mv %0, t5" : "=r" (x)  : );
#define M_COMRV_SET_ENTRY_ADDR(address) asm volatile ("la t6, "#address : : : );
#if __riscv_xlen == 64
 #define M_COMRV_SET_POOL_ADDR(address)  asm volatile ("la t4, "#address : : : ); \
                                         asm volatile ("ld t4, 0x0(t4)"  : : : );
 #define M_COMRV_SET_STACK_ADDR(address) asm volatile ("la t3, "#address : : : ); \
                                         asm volatile ("ld t3, 0x0(t3)"  : : : );
#elif __riscv_xlen == 32
 #define M_COMRV_SET_POOL_ADDR(address)  asm volatile ("la t4, "#address : : : ); \
                                         asm volatile ("lw t4, 0x0(t4)"  : : : );
 #define M_COMRV_SET_STACK_ADDR(address) asm volatile ("la t3, "#address : : : ); \
                                         asm volatile ("lw t3, 0x0(t3)"  : : : );
#endif
/**
* types
*/
typedef struct comrvStackFrame
{
  /* holds return address (caller is non overlay function) or return offset (caller is overlay function) */
  u32_t callerReturnAddressOffset;
  /* holds callee address (callee is non overlay function) or callee token (callee is overlay function) */
  u32_t calleeToken;
  /* holds the offset in bytes to the previous stack frame */
  s16_t offsetPrevFrame;
  /* size of the loaded overlay group - 1; used to avoid rereading this value from the overlay offset table */
  u16_t overlayGroupSize;
} comrvStackFrame_t;

#if 0
/* overlay token */
typedef struct comrvToken
{
  /* overlay token indication 0: address; 1: overlay token */
  u32_t overlayAddressToken:1;
  /* overlay group ID the function resides in */
  u32_t overlayGroupID:16;
  /* data/function offset within the overlay group – 4 bytes granularity */
  u32_t offset:10;
  /* 2 reserved bits */
  u32_t reserved:2;
  /* specify the heap ID this overlay group belongs to */
  u32_t heapID:2;
  /* multi group indication */
  u32_t multiGroup:1;
} comrvToken_t;
#endif

typedef struct comrvToken
{
  u32_t addressToken;
} comrvToken_t;

/* overlay token */
typedef union comrvOverlayTokenRegister
{
  u32_t        value;
  comrvToken_t token;
} comrvOverlayTokenRegister_t;

/* overlay token entry */
typedef struct comrvOverlayTokenEntry
{
  comrvOverlayTokenRegister_t tokenReg;
  void*                       actualAddress;
} comrvOverlayTokenEntry_t;


/**
* local prototypes
*/

/**
* external prototypes
*/
extern void comrv_entry(void);

/**
* global variables
*/
comrvOverlayTokenEntry_t overlayTokenList[D_COMRV_NUM_OF_OVERLAY_ENTRIES];
extern void *__OVERLAY_STACK_START__, *__OVERLAY_STACK_END__;
comrvStackFrame_t* pStackStartAddr = (comrvStackFrame_t*)&__OVERLAY_STACK_START__;
u32_t tmp;

/**
* COM-RV initialization function
*
* @param  none
*
* @return none
*/
void comrvInit(void)
{
   comrvStackFrame_t* pStackEndAddr = (comrvStackFrame_t*)&__OVERLAY_STACK_END__ - 1;

   /* mark the last frame */
   pStackStartAddr->offsetPrevFrame = D_COMRV_END_OF_STACK;
   /* initialize internal stack */
   for (pStackStartAddr++ ; pStackStartAddr < pStackEndAddr ; pStackStartAddr++)
   {
	   pStackStartAddr->offsetPrevFrame = (s16_t)-sizeof(comrvStackFrame_t);
	   pStackStartAddr->calleeToken = 0;
   }

   /* initialize internal data base */
   memset(overlayTokenList, 0xFF, sizeof(overlayTokenList));

   /* clear reg x29 */
   asm volatile ("mv t4, zero");

   /* set the address of COMRV entry point */
   M_COMRV_SET_ENTRY_ADDR(comrv_entry);

#ifndef __OS__
   /* set the address of COMRV stack and initialize it */
   pStackStartAddr--;
   M_COMRV_SET_STACK_ADDR(pStackStartAddr);
   pStackStartAddr->offsetPrevFrame = D_COMRV_END_OF_STACK;
#endif /* __OS__ */

   /* set the address of COMRV stack pool */
   pStackStartAddr--;
   M_COMRV_SET_POOL_ADDR(pStackStartAddr);
}

/**
* Search if current overlay token is already loaded
*
* @param  none
*
* @return void* - address of the overlay function or NULL if not loaded
*/
void* comrvSearchCurrentAddressToken(void)
{
   u32_t index, regValue;

   M_COMRV_READ_TOKEN_REG(regValue);

   for (index = 0 ; index < D_COMRV_NUM_OF_OVERLAY_ENTRIES ; index++)
   {
      if (overlayTokenList[index].tokenReg.value == regValue)
      {
         return overlayTokenList[index].actualAddress;
      }
   }

   return 0;
}

/**
* Load an overlay group according to address token in register t5
*
* @param pOverlayGroupSize - pointer output the overlay group size - 1
*
* @return void*            - address of the loaded overlay function
*/
void* comrvLoadCurrentAddressToken(u16_t* pOverlayGroupSize)
{
   u32_t regValue;

   M_COMRV_READ_TOKEN_REG(regValue);
   *pOverlayGroupSize = 512-1;
   overlayTokenList[0].tokenReg.value = regValue;
   overlayTokenList[0].actualAddress = (void*)(regValue ^ 1);
   return overlayTokenList[0].actualAddress;
}

