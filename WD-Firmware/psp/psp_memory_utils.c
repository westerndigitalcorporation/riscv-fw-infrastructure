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
* @file   psp_utils.c
* @author Nati Rapaport
* @date   24.06.2020
* @brief  Utilities for PSP usage
*
*/ 

/**
* include files
*/
#include "psp_api.h"

/**
* types
*/

/**
* definitions
*/

/**
* local prototypes
*/

/**
* external prototypes
*/

/**
* macros
*/

/**
* global variables
*/

/**
* APIs
*/

/**
* @brief - PSP implementation of memset function - bytes setting
* 
* @parameter - address of the memory to be set
* @parameter - pattern to set
* @parameter - number of bytes to set
*
* @return - address of the memory that has been set
*/
D_PSP_TEXT_SECTION void* pspMemsetBytes(void* pMemory, s08_t siVal, u32_t uiSizeInBytes)
{
   u32_t uiIndex;

   M_PSP_ASSERT(NULL != pMemory);

   /* Loop on the memory and set it according the inputs */
   for (uiIndex = 0 ; uiIndex < uiSizeInBytes ; uiIndex++)
   {
      *(u08_t*)(pMemory + uiIndex) = siVal;
   }

   return pMemory;
}


