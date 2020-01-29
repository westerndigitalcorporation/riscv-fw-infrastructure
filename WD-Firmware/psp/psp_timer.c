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
* @file   psp_timer.c
* @author Nati Rapaport
* @date   13.11.2019
* @brief  This file implements core's timer service functions
*
*/

/**
* include files
*/
#include "psp_types.h"
#include "psp_api.h"

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
void pspTimerActivate(u32_t timer, u32_t period);

/**
* external prototypes
*/

/**
* global variables
*/
void (*fptrPspTimerActivate)(u32_t timer, u32_t period) = pspTimerActivate;

/**
*
* @brief Activate core's Timer
*
* @ timer  - indicates which timer to setup
* @ period - defines the timer's period
*
***************************************************************************************************/
void pspTimerActivate(u32_t timer, u32_t period)
{
	//demoOutputMsg("SETUP Timer\n", 12);

    #if !defined(D_MTIME_ADDRESS) || !defined(D_MTIMECMP_ADDRESS)
        #error "MTIME/MTIMECMP address definition is missing"
    #endif

	/* Set the mtime and mtimecmp (memory-mapped registers) per privileged spec */
    volatile u64_t * mtime       = (u64_t*)D_MTIME_ADDRESS;
    volatile u64_t * mtimecmp    = (u64_t*)D_MTIMECMP_ADDRESS;
    u64_t now = *mtime;
    u64_t then = now + period;
    *mtimecmp = then;
}

