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

/**
* definitions
*/

/**
* macros
*/
/* Get region index*/
#define M_CACHE_CONTROL_ICACHE_OFFSET(regionIndex)     (regionIndex << 1)
#define M_CACHE_CONTROL_SIDEEFFECT_OFFSET(regionIndex) ((regionIndex << 1) + 1)

/* MRAC registers - If more regions has to be cached in future keep or'ing to cacheable regions*/
#define M_PSP_CACHE_CONTROL_ENABLE_ICACHE(regionIndex)     (1 << M_CACHE_CONTROL_ICACHE_OFFSET(regionIndex))
/* if more regions has to be marked as sideeffect in future keep or'ing to following macro */
#define M_PSP_CACHE_CONTROL_ENABLE_SIDEEFFECT(regionIndex) (1 << M_CACHE_CONTROL_SIDEEFFECT_OFFSET(regionIndex))

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
