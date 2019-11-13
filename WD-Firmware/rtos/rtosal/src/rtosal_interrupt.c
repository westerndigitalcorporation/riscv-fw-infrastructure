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
* @file   rtosal_interrupt.c
* @author Ronen Haen
* @date   21.01.2019 
* @brief  The file implements the RTOS AL interrupt API
* 
*/

/**
* include files
*/
#include "rtosal_interrupt_api.h"
#include "rtosal.h"

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
* The function installs an interrupt service routine per risc-v cause
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
                       rtosalInterruptCause_t stCauseIndex)
{
   return D_RTOSAL_SUCCESS;
}
