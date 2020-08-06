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
* @file   psp_macro_eh1.h
* @author Ofer Shinaar
* @date   03/2020
* @brief  The file defines the psp macros for SweRV EH1
*/
#ifndef __PSP_MACRO_EH1_H__
#define __PSP_MACRO_EH1_H__

/**
* include files
*/

/**
* macros
*/


/* enable all cache regions without "side effects"
 * for more info please read the PRM for "Region Access Control Register"  */
#define M_PSP_ICACHE_ENABLE()      M_PSP_WRITE_CSR(D_PSP_MRAC,0x55555555)
#define M_PSP_ICACHE_DISABLE()     M_PSP_WRITE_CSR(D_PSP_MRAC,0x0)


#endif /* __PSP_MACRO_EH1_H__ */
