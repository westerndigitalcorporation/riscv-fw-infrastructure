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
* @file   comrv_asm_macros.h
* @author Ronen Haen
* @date   02.10.2019
* @brief  The defines COM-RV assembly macros
* 
*/

/**
* include files
*/

/**
* definitions
*/

/**
* macros
*/
#ifdef D_COMRV_RTOS_SUPPORT
.macro m_COMRV_ENABLE_INTS
   M_PSP_ENABLE_INTERRUPTS
.endm
#else
.macro m_COMRV_ENABLE_INTS
.endm
#endif /* D_COMRV_RTOS_SUPPORT */

#ifdef D_COMRV_RTOS_SUPPORT
.macro m_COMRV_DISABLE_INTS
   M_PSP_DISABLE_INTERRUPTS
.endm
#else
.macro m_COMRV_ENABLE_INTS
.endm
#endif /* D_COMRV_RTOS_SUPPORT */

.macro m_FENCE_I
   fence.i
.endm
