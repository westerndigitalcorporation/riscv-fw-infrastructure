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
* @file   psp_attributes.h
* @author Nati Rapaport
* @date   26.08.2019
* @brief  The file defines attribute directives that relevant to our PSP
* 
*/

/**
* include files
*/

/**
* definitions
*/

#ifdef __GNUC__  /* NatiR - to do - check for LLVM too */
  #define D_PSP_NO_INLINE                         __attribute__((noinline))
  #define D_PSP_ALWAYS_INLINE                     __attribute__((always_inline))
  #define D_PSP_16_ALIGNED                        __attribute__ ((aligned(16)))
  #define D_PSP_WEAK                              __attribute__(( weak ))
  #define D_PSP_TEXT_SECTION                      __attribute__((section("PSP_TEXT_SEC")))
  #define D_PSP_DATA_SECTION                      __attribute__((section("PSP_DATA_SEC")))
#endif

/**
* macros
*/

/**
* types
*/

