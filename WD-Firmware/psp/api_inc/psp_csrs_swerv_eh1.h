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
* @file   psp_csrs_swerv_eh1.h
* @author Nati Rapaport
* @date   01.12.2019
* @brief  Definitions of Swerv's (EH1 version) CSRs
* 
*/
#ifndef  __PSP_CSRS_SWERV_EH1_H__
#define  __PSP_CSRS_SWERV_EH1_H__

/**
* include files
*/

/**
* definitions
*/
/* Region Access Control Register */
#define D_PSP_MRAC_NUM      (0x7c0)

/* mgpmc CSR */
#define D_PSP_MGPMC_NUM     0x7D0
#define D_PSP_MGMPC_MASK    0x00000001 /* Performance Monitor enable/disable */

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



#endif /* __PSP_CSRS_SWERV_EH1_H__ */
