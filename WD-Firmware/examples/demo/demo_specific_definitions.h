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
* @file   demo_specific_definitions.h
* @author Nati Rapaport
* @date   3.11.2019
* @brief  Contains definitions that specific to the current platform/core/system
*/

/**
* include files
*/

/**
* definitions
*/
#if defined (D_RV_HI_FIVE1)
   #define D_pspHasCLINT 1
   #define D_NUM_OF_ADDITIONAL_REGISTERS 0 /* Must be even number on 32-bit cores. */
#else
   #error "Specific core is not defined!"
#endif

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
* global variables
*/

/**
* functions
*/


