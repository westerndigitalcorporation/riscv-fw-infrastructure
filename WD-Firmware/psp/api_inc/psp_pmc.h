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
* @file   psp_pmc.h
* @author Alex Dvoskin
* @date   March 2020
* @brief  The file defines power management control(PMC) api services
* 
*/
#ifndef  __PSP_PMC_H__
#define  __PSP_PMC_H__

/**
* include files
*/

/**
* definitions
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
extern void (*fptrPspPmcHalt)(void);
extern void (*fptrPspPmcPause)(u32_t ticks);

/**
* global variables
*/

/**
* macros
*/
#define M_PSP_HALT_CORE()   	 fptrPspPmcHalt()
#define M_PSP_PAUSE_CORE(ticks)  fptrPspPmcPause(ticks)

/**
* APIs
*/



#endif /* __PSP_PMC_H__ */
