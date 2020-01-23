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
* @file   psp_interrupt_api_ehx1.h
* @author Nati Rapaport
* @date   14.01.2020
* @brief  The file defines the psp interrupt interfaces specific to SweRV-EHX1 core
*/
#ifndef __PSP_INTERRUPT_API_EHX1H__
#define __PSP_INTERRUPT_API_EHX1H__

/**
* include files
*/
#include "psp_interrupt_api.h"

/**
* macros
*/

/**
* types
*/

/* */
typedef enum pspInterruptCauseSweRVehx1
{
   E_MACHINE_INTERNAL_TIMER1_CAUSE   = 28,
   E_MACHINE_INTERNAL_TIMER0_CAUSE   = 29,
   E_MACHINE_CORRECTABLE_ERROR_CAUSE = 30,
   E_LAST_EHX1_CAUSE
} pspInterruptCauseSweRVehx1_t;

/**
* definitions
*/
#define D_PSP_FIRST_EHX1_INT_CAUSE = E_MACHINE_INTERNAL_TIMER1_CAUSE;

/* Exceptions */
typedef enum pspExceptionCauseSweRVehx1
{
   /* TBD how to handle MACHINE_D_BUS_STORE_ERR_NMI and MACHINE_D_BUS_NON_BLOCKING_LOAD_ERR  NMIs*/
   /*E_EXC_MACHINE_D_BUS_STORE_ERR_NMI              = ... */
   /*  E_EXC_MACHINE_D_BUS_NON_BLOCKING_LOAD_ERR    = ... */
   E_EXC_LAST_EHX1
} pspExceptionCauseSweRVehx1_t;


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

#endif /* __PSP_INTERRUPT_API_EHX1_H__ */
