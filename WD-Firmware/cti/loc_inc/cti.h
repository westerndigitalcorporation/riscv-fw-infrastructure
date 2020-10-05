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
* @file   cti.h
* @author Ronen Haen
* @date   01.07.2020
* @brief  The file defines cti (comrv testing infrastructute) local interface
*/
#ifndef __CTI_H
#define __CTI_H

/**
 * INCLUDES
 */
#include "cti_api.h"
#include "cti_errors.h"
#include "cti_types.h"
#include "cti_utilities.h"
#include "cti_tests.h"

/**
 * DEFINITIONS
 */

/**
 * MACROS
 */

/**
 * ENUM
 */

/**
 * TYPEDEFS
 */

/**
 * EXPORTED GLOBALS
 */
extern S_FW_CB_PTR g_pCtiFrameWorkCB;

/**
 * FUNCTIONS PROTOTYPES
 */
void ctiTaskBTest(void);

/**
 * EXTERNAL PROTOTYPES
 */

#endif /* __CTI_H */

