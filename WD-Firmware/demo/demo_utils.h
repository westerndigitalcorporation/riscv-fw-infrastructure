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
* @file   demo_utils.h
* @author Nati Rapaport
* @date   03.06.2020
* @brief  Common utilities for the demo applications
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
#define M_DEMO_ENDLESS_LOOP()   while(1) \
                             { \
                                asm volatile ("nop"); \
                                asm volatile ("nop"); \
                             }

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
* global variables
*/

/**
* functions
*/
