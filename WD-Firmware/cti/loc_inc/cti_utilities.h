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
* @file   cti_utilities.h
* @author Ronen Haen
* @date   01.07.2020
* @brief  The file defines cti (comrv testing infrastructure) utils api
*/
#ifndef __CTI_UTILITIES_H
#define __CTI_UTILITIES_H

/**
 * INCLUDES
 */

/**
 * DEFINITIONS
 */

/**
 * MACROS
 */

/**
 * TYPEDEFS
 */

/**
 * EXPORTED GLOBALS
 */

/**
 * ENUM
 */

/**
 * FUNCTIONS PROTOTYPES
 */
E_CTI_RESULT ctiSetErrorBit(S_FW_CB_PTR pCtiFrameWorkCB, E_TEST_ERROR eValue);
E_TEST_ERROR ctiGetErrorBits(S_FW_CB_PTR pCtiFrameWorkCB);
void         ctiResetOvlGlobal();

#endif /* __CTI_UTILITIES_H */

