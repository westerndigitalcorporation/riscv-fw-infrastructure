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
* @file   demo_app.h
* @author Nati Rapaport
* @date   29.10.2019
* @brief  internal api for demo applications
*/

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
* functions
*/

/**
 * demoInit
 *
 * Initialize the application:
 * - Register unhandled exceptions, Ecall exception and Timer ISR
 * - Create the messages queue
 * - Create the Tx and Rx tasks
 * - Create the Semaphore and the Sem-Task
 * - Create the software timer
 *
 * void *pMem - not in use
 */
void demoInit(void *pMem);
