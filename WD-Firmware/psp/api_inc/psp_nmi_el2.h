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
* @file   psp_nmi_el2.h
* @author Nati Rapaport
* @date   18.08.2020
* @brief  The file defines the psp NMI interfaces for features of SweRV EL2
* 
*/
#ifndef __PSP_NMI_EL2_H__
#define __PSP_NMI_EL2_H__

/**
* include files
*/

/**
* types
*/

/**
* definitions
*/
#define D_PSP_NMI_FAST_INT_DOUBLE_BIT_ECC_ERROR   0xF0001000
#define D_PSP_NMI_FAST_INT_DCCM_ACCESS_ERROR      0xF0001001
#define D_PSP_NMI_FAST_INT_NON_DCCM_REGION_ERROR  0xF0001002

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

#endif /* __PSP_NMI_EL2_H__ */
