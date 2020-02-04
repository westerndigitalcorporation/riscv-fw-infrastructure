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
* @file   psp_api.h
* @author Ronen Haen
* @date   21.01.2019
* @brief  The file defines the PSP API
*/
#ifndef  __PSP_API_H__
#define  __PSP_API_H__

/**
* include files
*/

#include "psp_intrinsics.h"
#include "psp_defines.h"
#include "psp_config.h"
#include "psp_interrupts.h"
#include "psp_macros.h"
#include "psp_pragmas.h"
#include "psp_attributes.h"
#include "psp_csrs.h"
#include "psp_timers.h"
#ifdef D_NEXYS_A7
    #include "psp_csrs_swerv_eh1.h"
#endif

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


/**
* The function installs an interrupt service routine per risc-v cause
*
* @param fptrInterruptHandler     – function pointer to the interrupt service routine
* @param interruptCause           – interrupt source
*
* @return u32_t                   - previously registered ISR
*/
pspInterruptHandler_t pspRegisterInterruptHandler(pspInterruptHandler_t fptrInterruptHandler, u32_t interruptCause);


/**
* The function installs an exception handler per exception cause
*
* @param fptrInterruptHandler     – function pointer to the exception handler
* @param exceptionCause           – exception cause
*
* @return u32_t                   - previously registered ISR
*/
pspInterruptHandler_t pspRegisterExceptionHandler(pspInterruptHandler_t fptrInterruptHandler, u32_t exceptionCause);

/**
*
* Function that called upon unregistered Trap handler
*/
void pspTrapUnhandled(void);




#endif /* __PSP_API_H__ */
