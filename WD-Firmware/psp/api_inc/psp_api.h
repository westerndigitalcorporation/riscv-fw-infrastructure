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

#include "psp_config.h"
#include "psp_defines.h"
#include "psp_interrupt_api.h"
#include "psp_macros.h"

/**
* definitions
*/

/**
* macros
*/
#define	M_PSP_CLR_TIMER_INT()      M_PSP_CLEAR_CSR(mie, D_MIP_MTIP);
#define M_PSP_ENABLE_TIMER_INT()   M_PSP_SET_CSR(mie, D_MIP_MTIP);
// NatiR - add additional macros for more CSRs

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
u32_t pspIsInterruptContext(void);
pspInterruptHandler_t pspRegisterIsrCauseHandler(pspInterruptHandler_t fptrRtosalInterruptHandler, pspInterruptCause_t eIntCause);
pspInterruptHandler_t pspRegisterIsrExceptionHandler(pspInterruptHandler_t fptrRtosalInterruptHandler, pspExceptionCause_t eExcCause);



#endif /* __PSP_API_H__ */
