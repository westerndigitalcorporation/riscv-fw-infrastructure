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
* @file   psp_version.c
* @author Nati Rapaport
* @date   13.05.2020
* @brief  The file maintains PSP version and suply it upon demand
*/


/**
* include files
*/
#include "psp_api.h"

/**
* definitions
*/
#define PSP_VERSION_MAJOR  1
#define PSP_VERSION_MINOR  1

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
* functions
*/
/**
* @brief - The function return PSP version 
*
* @param input: pointer to PSP version structure
*
*/
D_PSP_TEXT_SECTION void pspGetVersion(pspVersion_t *pPspVersion)
{
  pPspVersion->usMajor = (u16_t)PSP_VERSION_MAJOR;
  pPspVersion->usMinor = (u16_t)PSP_VERSION_MINOR;
}
