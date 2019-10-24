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
* @file   comrv_hooks.c
* @author Ronen Haen
* @date   20.10.2019
* @brief  The file implements the COM-RV hooks interfaces
* 
*/

/**
* include files
*/
#include "common_types.h"
#include "string.h"

/**
* definitions
*/

/**
* macros
*/

/**
* types
*/

/**
* globals
*/
extern void* __OVERLAY_START__;

/**
* Search if current overlay token is already loaded
*
* @param  none
*
* @return void* - address of the overlay function/data or NULL if not loaded
*/
void comrvErrorInddicationHook(u32_t errorNum)
{
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
   memcpy(pDest, pSrc, sizeInBytes);
}

/**
* load overlay group hook
*
* @param
*
* @return none
*/
void* comrvLoadOvlayGroupHook(u32_t groupOffset, void* pDest, u32_t sizeInBytes)
{
   memcpy(pDest, (u08_t*)&__OVERLAY_START__ + groupOffset, sizeInBytes);
   return pDest;
}

/**
* notification hook
*
* @param  none
*
* @return none
*/
void comrvNotificationHook(void)
{
}


/**
* crc calculation hook
*
* @param
*
* @return calculated CRC
*/
u32_t comrvCrcCalcHook(void* pAddress, u16_t memprySize)
{
   return 1;
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

