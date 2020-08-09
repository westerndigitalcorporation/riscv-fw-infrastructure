/*
* Copyright (c) 2010-2016 Western Digital, Inc.
*
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
* @file    psp_cache_control_eh2.c
* @author  Nati Rapaport
* @date    05.07.2020
* @brief   Cache control services for SweRV EH2
*/

/**
* include files
*/
#include "psp_api.h"
#include"psp_internal_mutex_eh2.h"

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
* local prototypes
*/

/**
* external prototypes
*/

/**
* global variables
*/

/**
* APIs
*/

/* @brief - Enable I-Cache for a given memory-id
*
* @parameter - memory Id
*/
D_PSP_TEXT_SECTION void pspCacheControlEnableIcache(u32_t uiMemoryRegionId)
{
  M_PSP_ASSERT(D_CACHE_CONTROL_MAX_NUMBER_OF_REGIONS > uiMemoryRegionId);

  /* As this CSR is common to all harts, make sure that it will not be accessed simultaneously by more than single hart */
  M_PSP_INTERNAL_MUTEX_LOCK(E_MUTEX_INTERNAL_FOR_MEMORY_CONTROL);

  M_PSP_SET_CSR(D_PSP_MRAC_NUM, M_PSP_CACHE_CONTROL_ICACHE_VAL(uiMemoryRegionId));

  /* Remove the multi-harts access protection */
  M_PSP_INTERNAL_MUTEX_UNLOCK(E_MUTEX_INTERNAL_FOR_MEMORY_CONTROL);
}

/**
* @brief - Enable side-effect for a given memory-id
*
* @parameter - memory Id
*/
D_PSP_TEXT_SECTION void pspCacheControlEnableSideEfect(u32_t uiMemoryRegionId)
{
  M_PSP_ASSERT(D_CACHE_CONTROL_MAX_NUMBER_OF_REGIONS > uiMemoryRegionId);

  /* As this CSR is common to all harts, make sure that it will not be accessed simultaneously by more than single hart */
  M_PSP_INTERNAL_MUTEX_LOCK(E_MUTEX_INTERNAL_FOR_MEMORY_CONTROL);

  M_PSP_SET_CSR(D_PSP_MRAC_NUM, M_PSP_CACHE_CONTROL_SIDEEFFECT_VAL(uiMemoryRegionId));

  /* Remove the multi-harts access protection */
  M_PSP_INTERNAL_MUTEX_UNLOCK(E_MUTEX_INTERNAL_FOR_MEMORY_CONTROL);
}

/* @brief - Disable I-Cache for a given memory-id
*
* @parameter - memory Id
*/
D_PSP_TEXT_SECTION void pspCacheControlDisableIcache(u32_t uiMemoryRegionId)
{
  M_PSP_ASSERT(D_CACHE_CONTROL_MAX_NUMBER_OF_REGIONS > uiMemoryRegionId);

  /* As this CSR is common to all harts, make sure that it will not be accessed simultaneously by more than single hart */
  M_PSP_INTERNAL_MUTEX_LOCK(E_MUTEX_INTERNAL_FOR_MEMORY_CONTROL);

  M_PSP_CLEAR_CSR(D_PSP_MRAC_NUM, M_PSP_CACHE_CONTROL_ICACHE_VAL(uiMemoryRegionId));

  /* Remove the multi-harts access protection */
  M_PSP_INTERNAL_MUTEX_UNLOCK(E_MUTEX_INTERNAL_FOR_MEMORY_CONTROL);
}

/**
* @brief - Disable side-effect for a given memory-id
*
* @parameter - memory Id
*/
D_PSP_TEXT_SECTION void pspCacheControlDisableSideEfect(u32_t uiMemoryRegionId)
{
  M_PSP_ASSERT(D_CACHE_CONTROL_MAX_NUMBER_OF_REGIONS > uiMemoryRegionId);

  /* As this CSR is common to all harts, make sure that it will not be accessed simultaneously by more than single hart */
  M_PSP_INTERNAL_MUTEX_LOCK(E_MUTEX_INTERNAL_FOR_MEMORY_CONTROL);

  M_PSP_CLEAR_CSR(D_PSP_MRAC_NUM, M_PSP_CACHE_CONTROL_SIDEEFFECT_VAL(uiMemoryRegionId));

  /* Remove the multi-harts access protection */
  M_PSP_INTERNAL_MUTEX_UNLOCK(E_MUTEX_INTERNAL_FOR_MEMORY_CONTROL);
}
