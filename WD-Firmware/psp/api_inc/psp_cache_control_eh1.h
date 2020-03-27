/*
* Copyright (c) 2010-2016 Western Digital, Inc.
*
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
* @file    psp_cache_control.h
* @author  Ronen Haen
* @date    26.03.2020
* @brief   Cache control interface
*/

#ifndef _PSP_CACHE_CONTROL_H_
#define _PSP_CACHE_CONTROL_H_

/**
* include files
*/
#include "psp_csrs_swerv_eh1.h"

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
/* enable icache for a specific memory region */
#define M_PSP_ENABLE_MEM_REGION_ICACHE(memRegionId)      (M_PSP_SET_CSR(D_PSP_MRAC_NUM, M_PSP_CACHE_CONTROL_ICACHE_VAL(memRegionId)))
/* enable sideeffect for a specific memory region */
#define M_PSP_ENABLE_MEM_REGION_SIDEEFFECT(memRegionId)  (M_PSP_SET_CSR(D_PSP_MRAC_NUM, M_PSP_CACHE_CONTROL_SIDEEFFECT_VAL(memRegionId)))
/* disable icache for a specific memory region */
#define M_PSP_DISABLE_MEM_REGION_ICACHE(memRegionId)     (M_PSP_CLEAR_CSR(D_PSP_MRAC_NUM, M_PSP_CACHE_CONTROL_ICACHE_VAL(memRegionId)))
/* disable sideeffect for a specific memory region */
#define M_PSP_DISABLE_MEM_REGION_SIDEEFFECT(memRegionId) (M_PSP_CLEAR_CSR(D_PSP_MRAC_NUM, M_PSP_CACHE_CONTROL_SIDEEFFECT_VAL(memRegionId)))

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

#endif /* _PSP_CACHE_CONTROL_H_ */
