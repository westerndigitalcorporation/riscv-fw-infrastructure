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
* @file   rtosal_starter_api.h
* @author Nati Rapaport
* @date   13.11.2019
* @brief  The file defines the RTOS AL start and end interfaces
*/
#ifndef __RTOSAL_STARTER_API_H__
#define __RTOSAL_STARTER_API_H__

/**
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

/* application specific initialization function */
typedef void (*rtosalApplicationInit_t)(void *pParam);

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
* Initialization of the RTOS and starting the scheduler operation
*/
void rtosalStart(rtosalApplicationInit_t fptrInit);

/**
* Ending of scheduler operation
*/
void rtosalEndScheduler(void);



#endif // __RTOSAL_STARTER_API_H__
