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
* include files
*/
#include "common_types.h"
#include "demo_platform_al.h"
#include "demo_utils.h"
#include "mati_api.h"
#include "comrv_api.h"

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
extern void psp_vect_table(void);

/**
* global variables
*/

/**
* functions
*/
void demoStart(void)
{
   comrvInitArgs_t stComrvInitArgs = { 1 };

   M_DEMO_START_PRINT();

   /* Register interrupt vector */
   pspInterruptsSetVectorTableAddress(&psp_vect_table);

   /* Init ComRV engine */
   comrvInit(&stComrvInitArgs);

   /* call the test environment */
   f_mati_main();
}

