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
pspNmiHandler_t pspNmiRegisterHandler(pspNmiHandler_t fptrNmiHandler, u32_t uiNmiCause);



#endif /* __PSP_NMI_EH1_H__ */
