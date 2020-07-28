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
* @file   comrv_api.h
* @author Ronen Haen
* @date   11.06.2019
* @brief  The file defines the COM-RV interfaces
*/
#ifndef __COMRV_TASK_API_H__
#define __COMRV_TASK_API_H__

#ifndef __clang__
#error comrv can compile only with llvm
#endif // #ifndef __clang__

/**
* include files
*/
#include "comrv_types.h"

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
* APIs
*/
void comrvEnable(void);
void comrvDisable(void);
void comrvLoadTables(void);
const comrvCB_t* comrvGetDatabase(void);
void comrvReset(comrvResetType_t eResetType);
void comrvInit(comrvInitArgs_t* pInitParams);
void comrvGetStatus(comrvStatus_t* pComrvStatus);
D_COMRV_NO_INLINE u32_t comrvInitApplicationStack(void);
u32_t comrvLockUnlockOverlayGroupByFunction(void* pAddress, comrvLockState_t eLockState);
#ifdef D_COMRV_RTOS_SUPPORT
   u32_t* comrvSaveContextSwitch(volatile u32_t* pMepc, volatile u32_t* pRegisterT3,
                               comrvTaskStackRegsVal_t** pComrvTaskStackRegs);
#endif /* D_COMRV_RTOS_SUPPORT */

#endif /* __COMRV_TASK_API_H__ */
