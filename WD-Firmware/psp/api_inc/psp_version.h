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
* @file   psp_version.h
* @author Nati Rapaport
* @date   13.05.2020
* @brief  The file defines the PSP version and api to fetch it
*/
#ifndef  __PSP_VERSION_H__
#define  __PSP_VERSION_H__

/***
* include files
*/

/**
* definitions
*/


/**
* macros
*/


/**
* types
*/
/* version sturct */
typedef struct pspVersion
{
  u16_t usMajor;
  u16_t usMinor;
} pspVersion_t;

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
/**
* @brief - The function return PSP version 
*
* @param input: pointer to PSP version structure
*
*/
void pspGetVersion(pspVersion_t *pPspVersion);




#endif /* __PSP_VERSION_H__ */
