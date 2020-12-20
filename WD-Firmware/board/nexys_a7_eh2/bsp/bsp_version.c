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
* @file   bsp_version.c
* @author Ofer Shinaar
* @date   24.02.2020
* @brief  Supplies SweRVolf version information
*
*/

/**
* include files
*/
#include "psp_api.h"
#include "bsp_mem_map.h"
#include "bsp_version.h"

/**
* definitions
*/

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
 *
* The function return version num
*
* @param inputs: ucRev, minor, major, sha, dirty
*
*/
void versionGetSwervolfVer(swervolfVersion_t *pSwervolfVersion)
{
  pSwervolfVersion->ucRev   = *(u08_t*)D_VERSION_REV;
  pSwervolfVersion->ucMajor = *(u08_t*)D_VERSION_MAJOR;
  pSwervolfVersion->ucMinor = *(u08_t*)D_VERSION_MINOR;
  pSwervolfVersion->ucDirty = *(u08_t*)D_VERSION_DIRTY;
  pSwervolfVersion->ucSha   = *(u08_t*)D_VERSION_SHA;
}

