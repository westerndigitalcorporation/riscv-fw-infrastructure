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
* @file   rtosal_interrupt_api.h
* @author Ronen Haen
* @date   07.02.2019
* @brief  The file defines the RTOS AL interrupt interfaces
*/
#ifndef __RTOSAL_INTERRUPT_API_H__
#define __RTOSAL_INTERRUPT_API_H__

/**
* include files
*/
#include "rtosal_config.h"
#include "rtosal_defines.h"
#include "rtosal_types.h"

/**
* definitions
*/

/**
* macros
*/

/**
* types
*/
/* */
typedef enum rtosalInterruptSource
{
   E_USER_SOFTWARE_SOURCE_INT = 0,
   E_SUPERVISOR_SOFTWARE_SOURCE_INT = 1,
   E_RESERVED_SOFTWARE_SOURCE_INT = 2,
   E_MACHINE_SOFTWARE_SOURCE_INT = 3,
   E_USER_TIMER_SOURCE_INT = 4,
   E_SUPERVISOR_TIMER_SOURCE_INT = 5,
   E_RESERVED_TIMER_SOURCE_INT = 6,
   E_MACHINE_TIMER_SOURCE_INT = 7,
   E_USER_EXTERNAL_SOURCE_INT = 8,
   E_SUPERVISOR_EXTERNAL_SOURCE_INT = 9,
   E_RESERVED_EXTERNAL_SOURCE_INT = 10,
   E_MACHINE_EXTERNAL_SOURCE_INT = 11,
   E_LAST_SOURCE_INT
} rtosalInterruptSource_t;

/* interrupt handler definition */
typedef void (*rtosalInterruptHandler_t)(void);
typedef unsigned long (*rtosalExceptionHandler_t)(unsigned long mcause, unsigned long sp, unsigned long ecallArg);

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

void rtosalTick(void);

u32_t rtosalInstallIsr(rtosalInterruptHandler_t fptrRtosalInterruptHandler, rtosalInterruptSource_t eSourceInt);
u32_t rtosalInstallExceptionIsr(rtosalExceptionHandler_t fptrRtosalExceptionHandler);

#endif /* __RTOSAL_INTERRUPT_API_H__ */
