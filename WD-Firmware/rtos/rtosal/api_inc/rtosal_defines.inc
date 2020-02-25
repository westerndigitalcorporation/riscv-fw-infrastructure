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
* @file   rtosal_defines.inc
* @author Nati rapaport
* @date   13.11.2019
* @brief  RTOS-AL various definitions to be used in assembly code
* 
*/

/**
* include files
*/

/**
* definitions
*/
.if D_USE_FREERTOS
    .equ D_RTOSAL_MEPC_LOC_IN_STK,    0
    .equ D_RTOSAL_MSTATUS_LOC_IN_STK, 31
.else
    /* [NR] - TO DO - define appropriate numbers for THREADX */
    .error "RTOS is not defined!"
.endif


