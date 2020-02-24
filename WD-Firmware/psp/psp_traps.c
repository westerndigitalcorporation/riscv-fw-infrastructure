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
* @file   psp_traps.c
* @author Ronen Haen
* @date   10.05.2019
* @brief  This file implements trap functions
*
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the RISC-V RV32 port.
 *----------------------------------------------------------*/
#include "psp_api.h"

#if defined (__GNUC__) || defined (__clang__)

/**
*
* @brief Function that called upon unregistered Trap handler
*
***************************************************************************************************/
void pspTrapUnhandled(void)
{
	u32_t local_mepc,local_mcause;
	//exit(M_PSP_READ_CSR(D_PSP_MCAUSE_ADDR));
	local_mepc = M_PSP_READ_CSR(mepc);
	local_mcause = M_PSP_READ_CSR(D_PSP_MCAUSE_ADDR);
	if (0 == local_mepc || 0 == local_mcause){}
	//write(1, "Unhandeled exc\n", 15);
	   asm volatile ("ebreak" : : : );
}

#endif /* defined (__GNUC__) || defined (__clang__) */



