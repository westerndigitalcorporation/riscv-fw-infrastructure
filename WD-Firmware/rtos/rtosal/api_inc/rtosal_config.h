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
* @file   rtosal_config.h
* @author Ronen Haen
* @date   21.01.2019 
* @brief  The file defines the RTOS AL configuration
*/
#ifndef  __RTOSAL_CONFIG_H__
#define  __RTOSAL_CONFIG_H__

#ifdef D_USE_FREERTOS

#define configSUPPORT_STATIC_ALLOCATION  1
#define configSUPPORT_DYNAMIC_ALLOCATION 0
/**
* include files
*/
   #include "FreeRTOSConfig.h"

#elif D_USE_THREADX
   #include "TBD: the root api"
#else
   #error *** RTOSAL: undefined RTOS core (use D_USE_FREERTOS/D_USE_THREADX) ***
#endif /* #ifdef D_USE_FREERTOS */

/**
* definitions
*/
#ifdef D_USE_FREERTOS
   #define D_RTOSAL_ERROR_CHECK                  0
#elif D_USE_THREADX
   #define D_RTOSAL_ERROR_CHECK                  TBD_TAKE_VAL_FROM_TX
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

#endif /* __RTOSAL_CONFIG_H__ */
