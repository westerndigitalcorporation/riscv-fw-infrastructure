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

/***
* include files
*/
#include "psp_types.h"
#include "psp_intrinsics.h"
#include "psp_defines.h"
#include "psp_config.h"
#include "psp_csrs.h"
#include "psp_macros.h"
#include "psp_pragmas.h"
#include "psp_attributes.h"
#ifdef D_HI_FIVE1
    #include "psp_timers_hifive1.h"
    #include "psp_interrupts_hifive1.h"
#elif D_NEXYS_A7  /* Nati TO DO - change D_NEXYS_A7 all over the place to EH1 */
    #include "psp_csrs_eh1.h"
    #include "psp_timers_eh1.h"
    #include "psp_interrupts_eh1.h"
    #include "psp_ext_interrupts_eh1.h"
	#include "psp_pmc_eh1.h"
	#include "psp_performance_monitor_eh1.h"
    #include "psp_nmi_eh1.h"
    #include "psp_cache_control_eh1.h"
#endif
#include "psp_traps_interrupts.h"

/**
* definitions
*/


/**
* macros
*/

/* What is the current privilege-level */
#define M_PSP_GET_CURRENT_PRIVILEGE_LEVEL() /* Nati - to be implemented - use ebreak, then check MPP/SPP fields in mstatus, then return */

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




#endif /* __PSP_API_H__ */
