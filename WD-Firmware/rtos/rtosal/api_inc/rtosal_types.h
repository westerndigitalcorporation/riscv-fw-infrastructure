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
* @file   rtosal_types.h
* @author Ronen Haen
* @date   21.01.2019 
* @brief  The defines RTOS AL specific types
* 
*/
#ifndef  __RTOSAL_TYPES_H__
#define  __RTOSAL_TYPES_H__

/**
* include files
*/
#include "common_types.h"

/**
* types
*/
#ifdef D_USE_FREERTOS
   typedef u32_t rtosalStackType_t;
#elif D_USE_THREADX
#error *** TODO: need to define the TBD ***
   typedef TBD   rtosalStackType_t;
#endif /* #ifdef D_USE_FREERTOS */


#endif /* __RTOSAL_TYPES_H__ */
