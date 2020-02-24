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
typedef enum rtosalInterruptCause
{
   userSoftwareInterrupt = 0,
} rtosalInterruptCause_t;

/* interrupt handler definition */
typedef void (*rtosalInterruptHandler_t)(void);


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
* The function installs an interrupt service routine per risc-v cuase
*
* @param fptrRtosalInterruptHandler – function pointer to the interrupt
*                                   service routine
* @param uiInterruptId             – interrupt ID number
* @param uiInterruptPriority       – interrupt priority
* @param stCauseIndex               – value of the mcuase register this
*                                   interrupt is assigned to
* @return u32_t                   - D_RTOSAL_SUCCESS
*                                 - D_RTOSAL_CALLER_ERROR
*/
u32_t rtosalInstallIsr(rtosalInterruptHandler_t fptrRtosalInterruptHandler,
                       u32_t uiInterruptId , u32_t uiInterruptPriority,
                       rtosalInterruptCause_t stCauseIndex);

/**
* @brief check if in ISR context
*
* @param None
*
* @return u32_t            - D_NON_INT_CONTEXT
*                          - non zero value - interrupt context
*/
u32_t rtosalIsInterruptContext(void);

#endif /* __RTOSAL_INTERRUPT_API_H__ */
