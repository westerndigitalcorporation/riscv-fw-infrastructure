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
* @file   psp_internal_mutex_eh2.h
* @author Nati Rapaport
* @date   05.07.2020
* @brief  The file defines mutexs for internal PSP usage. It is relevant for SweRV EH2 (multi HW-threads core)
*         
*/
#ifndef __PSP_INTERNAL_MUTEX_EH2_H__
#define __PSP_INTERNAL_MUTEX_EH2_H__

/**
* include files
*/

/**
* types
*/
/* Mutexs for PSP internal usage */
typedef enum pspInternalMutex
{
  E_MUTEX_INTERNAL_FOR_MUTEX_HEAP_MNG    = 0,
  E_MUTEX_INTERNAL_FOR_EXT_INTERRUPTS    = 1,
  E_MUTEX_INTERNAL_FOR_CORR_ERR_COUNTERS = 2,
  E_MUTEX_INTERNAL_FOR_NMI_DELEGATION    = 3,
  E_MUTEX_INTERNAL_FOR_MEMORY_CONTROL    = 4,
  E_MUTEX_INTERNAL_LAST
}ePspInternalMutex_t;

/**
* definitions
*/
#define D_PSP_NUM_OF_INTERNAL_MUTEXS  E_MUTEX_INTERNAL_LAST

/**
* local prototypes
*/

/**
* external prototypes
*/

/**
* macros
*/
#define M_PSP_END_OF_PSP_AREA_IN_DCCM()             (D_PSP_DCCM_SECTION_ADDRESS + D_PSP_NUM_OF_INTERNAL_MUTEXS*sizeof(D_PSP_SIZE_OF_MUTEX));

#define M_PSP_INTERNAL_MUTEX_LOCK(internal_mutex)    M_PSP_ATOMIC_ENTER_CRITICAL_SECTION((volatile u32_t*)(D_PSP_DCCM_SECTION_ADDRESS + (internal_mutex*sizeof(pspMutex_t))));
#define M_PSP_INTERNAL_MUTEX_UNLOCK(internal_mutex)  M_PSP_ATOMIC_EXIT_CRITICAL_SECTION((volatile u32_t*)(D_PSP_DCCM_SECTION_ADDRESS + (internal_mutex*sizeof(pspMutex_t))));

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
void pspInternalMutexInit(void);

#endif /* __PSP_INTERNAL_MUTEX_EH2_H__ */
