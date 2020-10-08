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
* @file   cti_errors.h
* @author Ronen Haen
* @date   01.07.2020
* @brief  The file defines cti (comrv testing infrastructure) errors
*/
#ifndef __CTI_ERRORS_H
#define __CTI_ERRORS_H

/*
* includes
*/

/*
* defines
*/

/*
* macros
*/

/*
* globals
*/

/*
* functions
*/

/*
* types
*/
typedef enum 
{
  E_TEST_ERROR_NO_ERROR                             = 0x0,
  E_TEST_ERROR_OVL_SANITY_FAILED                    = (0x1 << 1),
  E_TEST_ERROR_OVL_UNIFORM_GROUPS_FAILED            = (0x1 << 2),
  E_TEST_ERROR_OVL_NON_UNIFORM_GROUPS_FAILED        = (0x1 << 3),
  E_TEST_ERROR_OVL_MULTI_GROUPS_FAILED              = (0x1 << 4),
  E_TEST_ERROR_OVL_LOCK_UNLOCK_FAILED               = (0x1 << 5),
  E_TEST_ERROR_OVL_DEFRAG_FAILED                    = (0x1 << 6),
  E_TEST_ERROR_OVL_CRC_FAILED                       = (0x1 << 7),
  E_TEST_ERROR_OVL_CRITICAL_SECTION_FAILED          = (0x1 << 8),
  E_TEST_ERROR_OVL_THREAD_SAFE_READ_BACK_STORE      = (0x1 << 9),
  E_TEST_ERROR_OVL_FAILED                           = (0x1 << 10),
}E_TEST_ERROR;

#endif /* EOF */

