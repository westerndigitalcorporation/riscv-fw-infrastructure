/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2020 Western Digital Corporation or its affiliates.
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
* @file   psp_ext_interrupts_eh2.h
* @author Nati Rapaport
* @date   18.05.2020
* @brief  The file defines the psp external interrupts interfaces for features of SweRV EH2 
*/
#ifndef __PSP_EXT_INTERRUPTS_EH2_H__
#define __PSP_EXT_INTERRUPTS_EH2_H__

/**
* include files
*/

/**
* definitions
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
* macros
*/

/**
* global variables
*/

/**
* APIs
*/

/**
* @brief - Delegate external interrupt to a Hart (HW thread)
*          When delegated, the given external-interrupt number will be visible only to the given hart
* 
* @parameter - external interrupt number
* @parameter - Hart number
*/
void pspExternalInterruptDelegateToHart(u32_t uiExtIntNumber, u32_t uiHartNumber);

/*
* This function checks whether a given external interrupt is pending or not per the current Hart (HW thread)
* Via this api, only the external-interrupts that are delegated to current hart, could be seen, whether they are pending or not.
*
* @param uiExtInterrupt = Number of external interrupt
* @return = pending (1) or not (0)
*/
u32_t pspExtInterruptIsPendingOnHart(u32_t uiExtInterrupt);


#endif /* __PSP_EXT_INTERRUPTS_EH2_H__ */
