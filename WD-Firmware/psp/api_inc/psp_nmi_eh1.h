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
* @file   psp_nmi_eh1.h
* @author Nati Rapaport
* @date   13.04.2020
* @brief  The file contains NMI handlers registration service (relevant to SweRV EH1)
*/
#ifndef __PSP_NMI_EH1_H__
#define __PSP_NMI_EH1_H__

/**
* include files
*/

/**
* types
*/
/* NMI handler definition */
typedef void (*pspNmiHandler_t)(void);

/**
* definitions
*/
#define D_PSP_NMI_EXT_PIN_ASSERTION   0
#define D_PSP_NMI_D_BUS_STORE_ERROR   0xF0000000
#define D_PSP_NMI_D_BUS_LOAD_ERROR    0xF0000001

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
 * @brief - set address of NMI initial handler in nmi_vec
 *
 * @parameter - uiNmiVecAddress - address of NMI_VEC register
 * @parameter - fptrNmiSelector - address of NMI initial handler
 */
void pspNmiSetVec(u32_t uiNmiVecAddress, pspNmiHandler_t fptrNmiSelector);

/**
* @brief - The function installs an Non-Maskable Interrupt (NMI) service routine
*
* input parameter -  fptrNmiHandler     - function pointer to the NMI service routine
* input parameter -  uiNmiCause         - NMI source
*                    Note that the value of this input parameter could be only one of these:
*                    - D_PSP_NMI_EXT_PIN_ASSERTION
*                    - D_PSP_NMI_D_BUS_STORE_ERROR
*                    - D_PSP_NMI_D_BUS_LOAD_ERROR
*
* @return u32_t                               - previously registered ISR. If NULL then registeration had an error
*/
pspNmiHandler_t pspNmiRegisterHandler(pspNmiHandler_t fptrNmiHandler, u32_t uiNmiCause);


/**
* @brief - This function is called upon NMI and selects the appropriate handler
*
*/
D_PSP_NO_RETURN void pspNmiHandlerSelector(void);


#endif /* __PSP_NMI_EH1_H__ */
