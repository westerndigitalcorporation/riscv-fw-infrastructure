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
* @brief  This file implements core's timer-counter service functions
*
*/

/**
* include files
*/
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
void pspTimerCounterActivate(u32_t uiTimer, u32_t uiPeriod);

/**
* external prototypes
*/

/**
* global variables
*/
void (*fptrPspTimerCounterActivate)(u32_t uiTimer, u32_t uiPeriod) = pspTimerCounterActivate;

/**
*
* @brief Activate core's Timer
*
* @ timer  - indicates which timer (actually it is a counter) to setup
* @ period - defines the timer's period
*
***************************************************************************************************/
void pspTimerCounterActivate(u32_t uiTimer, u32_t uiPeriod)
{

    #if (0 == D_MTIME_ADDRESS) || (0 == D_MTIMECMP_ADDRESS)
        #error "MTIME/MTIMECMP address definition is missing"
    #endif

	/* Set the mtime and mtimecmp (memory-mapped registers) per privileged spec */
    volatile u64_t *pMtime       = (u64_t*)D_MTIME_ADDRESS;
    volatile u64_t *pMtimecmp    = (u64_t*)D_MTIMECMP_ADDRESS;
    u64_t udNow = *pMtime;
    u64_t udThen = udNow + uiPeriod;
    *pMtimecmp = udThen;
}

/**
*
* @brief Get Timer counter value
*
* @return u64_t      – Timer counter value
*
***************************************************************************************************/
u64_t pspTimerCounterGet(void)
{
	#if (0 == D_MTIME_ADDRESS)
        #error "MTIME address definition is missing"
    #endif
	volatile u64_t *pMtime       = (u64_t*)D_MTIME_ADDRESS;
	return *pMtime;
}

/**
*
* @brief Get Time compare counter value
*
* @return u64_t      – Time compare counter value
*
***************************************************************************************************/
u64_t pspTimeCompareCounterGet(void)
{
	#if (0 == D_MTIMECMP_ADDRESS)
        #error "MTIMECMP address definition is missing"
    #endif
	volatile u64_t *pMtimecmp    = (u64_t*)D_MTIMECMP_ADDRESS;
	return *pMtimecmp;
}


