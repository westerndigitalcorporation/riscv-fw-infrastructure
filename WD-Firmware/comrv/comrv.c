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

/**
* macros
*/

/**
* types
*/
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
/* reg x31 holds the address token */
register comrvOverlayTokenRegister_t g_uiCurrentOverlayToken asm("x31");
/* reg x30 holds the  address of comrv_entry function */
register u32_t g_reservedRegX30 asm("x30");
/* reg x29 holds caller token or 0  in case caller is a non-overlay function */
register u32_t g_reservedRegX29 asm("x29");

/**
* external prototypes
*/
void comrv_entry(void);

/**
* global variables
*/
static comrvOverlayTokenEntry_t overlayTokenList[D_COMRV_NUM_OF_OVERLAY_ENTRIES];

void * __OVERLAY_SEC_START__;
void * __OVERLAY_SEC_END__;

//extern void* arr[];

void comrvInit(void)
{
   /* initialize internal data base */
   memset(overlayTokenList, 0xFF, sizeof(overlayTokenList));

   /* clear reg x29 */
   g_reservedRegX29 = 0;

   /* we need to save the addresses of COMRV entry point */
   asm volatile ("la x30, %0" :  : "i"(comrv_entry));
}

/**
* Search if current overlay token is already loaded
*
* @param pRtosalEventGroupCb   - pointer to event group control block
*
* @return u32_t            -
*/
void* comrvSearchCurrentAddressToken(void)
{
   u32_t index;
   for (index = 0 ; index < D_COMRV_NUM_OF_OVERLAY_ENTRIES ; index++)
   {
      if (overlayTokenList[index].tokenReg.value == g_uiCurrentOverlayToken.value)
      {
         return overlayTokenList[index].actualAddress;
      }
   }

   return 0;
}

/**
* Load an overlay group according to address token in register x31
*
* @param pRtosalEventGroupCb   - pointer to event group control block
*
* @return u32_t            -
*/
void* comrvLoadCurrentAddressToken(void)
{
   overlayTokenList[0].tokenReg.value = g_uiCurrentOverlayToken.value;
#if 1
   overlayTokenList[0].actualAddress = (void*)(g_uiCurrentOverlayToken.value ^ 1);
   return overlayTokenList[0].actualAddress;
#else
   memcpy((void*)&__OVERLAY_SEC_START__, arr[(u32_t)g_uiCurrentOverlayToken.value], 100);
   overlayTokenList[0].actualAddress = (void*)&__OVERLAY_SEC_START__;
   return &__OVERLAY_SEC_START__;
#endif
}

/**
*
*
* @param pRtosalEventGroupCb   - pointer to event group control block
*
* @return u32_t            -
*/
#if 0
u32_t comrvGetCallerRaTokenAndOffset(u32_t* pReturnAddress, u32_t* pToken)
{
   /* check if the return address is an overlay function */
   if (pReturnAddress >= (u32_t*)&__OVERLAY_SEC_START__ && pReturnAddress < (u32_t*)&__OVERLAY_SEC_END__)
   {
      /* TODO: loop all overlay groups to find the caller group start address */
	  *pToken = (u32_t)((u08_t*)pReturnAddress - (u08_t*)overlayTokenList[0].actualAddress);
      return overlayTokenList[0].tokenReg.value;
   }
   *pToken = (u32_t)pReturnAddress;
   /* caller is a non-overlay function so offset will be the actual pReturnAddress */
   return (u32_t)pReturnAddress;
}
#endif
