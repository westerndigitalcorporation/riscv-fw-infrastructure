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
* @file    psp_cache_control_eh1.h
* @author  Ronen Haen
* @date    26.03.2020
* @brief   Cache control interface
*/

#ifndef _PSP_CACHE_CONTROL_H_
#define _PSP_CACHE_CONTROL_H_

/**
* include files
*/

/**
* definitions
*/
#define D_CACHE_CONTROL_MAX_NUMBER_OF_REGIONS            16

/**
* macros
*/
/* convert region number to mrac offset */
#define M_CACHE_CONTROL_ICACHE_OFFSET(memRegionId)       (memRegionId << 1)
#define M_CACHE_CONTROL_SIDEEFFECT_OFFSET(memRegionId)   ((memRegionId << 1) + 1)
/* mrac icache value of a specific memory region */
#define M_PSP_CACHE_CONTROL_ICACHE_VAL(memRegionId)      (1 << M_CACHE_CONTROL_ICACHE_OFFSET(memRegionId))
/* mrac sideeffect value of a specific region */
#define M_PSP_CACHE_CONTROL_SIDEEFFECT_VAL(memRegionId)  (1 << M_CACHE_CONTROL_SIDEEFFECT_OFFSET(memRegionId))

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
/**
* @brief - Enable I-Cache for a given memory-id
*
* @parameter - memory Id
*/
void pspCacheControlEnableIcache(u32_t uiMemoryRegionId);

/**
* @brief - Enable side-effect for a given memory-id
*
* @parameter - memory Id
*/
void pspCacheControlEnableSideEfect(u32_t uiMemoryRegionId);

/* @brief - Disable I-Cache for a given memory-id
*
* @parameter - memory Id
*/
void pspCacheControlDisableIcache(u32_t uiMemoryRegionId);

/**
* @brief - Disable side-effect for a given memory-id
*
* @parameter - memory Id
*/
void pspCacheControlDisableSideEfect(u32_t uiMemoryRegionId);

#endif /* _PSP_CACHE_CONTROL_H_ */
