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
#define M_COMRV_READ_TOKEN_REG(x)       asm volatile ("mv %0, t6" : "=r" (x)  : );

/**
* macros
*/
#define M_COMRV_SET_ENTRY(func)           asm volatile ("la x30, "#func : : : );

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

/**
* external prototypes
*/
extern void comrv_entry(void);

/**
* global variables
*/
static comrvOverlayTokenEntry_t overlayTokenList[D_COMRV_NUM_OF_OVERLAY_ENTRIES];

void comrvInit(void)
{
   /* initialize internal data base */
   memset(overlayTokenList, 0xFF, sizeof(overlayTokenList));

   /* clear reg x29 */
   asm volatile ("mv t4, zero");

   /* we need to save the addresses of COMRV entry point */
   M_COMRV_SET_ENTRY(comrv_entry);
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
* Load an overlay group according to address token in register x31
*
* @param pRtosalEventGroupCb   - pointer to event group control block
*
* @return u32_t            -
*/
void* comrvLoadCurrentAddressToken(void)
{
   u32_t regValue;

   M_COMRV_READ_TOKEN_REG(regValue);

   overlayTokenList[0].tokenReg.value = regValue;
   overlayTokenList[0].actualAddress = (void*)(regValue ^ 1);
   return overlayTokenList[0].actualAddress;
}

