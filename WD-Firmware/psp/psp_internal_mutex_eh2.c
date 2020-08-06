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

/**
* APIs
*/

/**
* @brief - Initialize (zero) the internal PSP mutexs. Used by PSP for multi-harts safe functionality
*
*/
D_PSP_TEXT_SECTION void pspInternalMutexInit(void)
{
  /* Set all mutexs used internally by PSP to "Unlocked" state */
  pspMemsetBytes((void*)D_PSP_DCCM_SECTION_ADDRESS, D_PSP_MUTEX_UNLOCKED, D_PSP_NUM_OF_INTERNAL_MUTEXS);
}
