/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2020-2021 Western Digital Corporation or its affiliates.
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
* @file   psp_internal_mutex_eh2.c
* @author Nati Rapaport
* @date   05.07.2020
* @brief  The file defines mutexs for internal PSP usage. It is relevant for SweRV EH2 (multi HW-threads core)
*         
*/

/**
* include files
*/
#include "psp_api.h"
#include "psp_internal_mutex_eh2.h"

/**
* definitions
*/
#define D_INTERNAL_MUTEX_SECTION __attribute__((section(".psp_internal_mutexes")))

#define D_PSP_NUM_OF_INTERNAL_MUTEXES      5

/**
* types
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
D_INTERNAL_MUTEX_SECTION u32_t g_uiInternalPspMutex[D_PSP_NUM_OF_INTERNAL_MUTEXES];
/**
* APIs
*/

/**
* @brief - Initialize (zero) the internal PSP mutexs. Used by PSP for multi-harts safe functionality
*          Note that the size of PSP internal mutex is 32bit (while the mutex for users is 64bits)
*
*/
D_PSP_TEXT_SECTION void pspInternalMutexInit(void)
{
  /* Set all mutexs used internally by PSP to "Unlocked" state */
  pspMemsetBytes((void*)g_uiInternalPspMutex, D_PSP_MUTEX_UNLOCKED, sizeof(u32_t)*D_PSP_NUM_OF_INTERNAL_MUTEXES);
}

/**
* @brief - Lock an internal-PSP mutex
*
* @parameter - mutex number
*
*/
void pspInternalMutexLock(u32_t uiMutexNumber)
{
  M_PSP_ASSERT(D_PSP_NUM_OF_INTERNAL_MUTEXES > uiMutexNumber);

  pspAtomicsEnterCriticalSection((u32_t*)&g_uiInternalPspMutex[uiMutexNumber]);
}

/**
* @brief - Unlock an internal-PSP mutex
*
* @parameter - mutex number
*
*/
void pspInternalMutexUnlock(u32_t uiMutexNumber)
{
  M_PSP_ASSERT(D_PSP_NUM_OF_INTERNAL_MUTEXES > uiMutexNumber);

  pspAtomicsExitCriticalSection((u32_t*)&g_uiInternalPspMutex[uiMutexNumber]);
}
