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
* @file   demo_platform_al.h
* @author Nati Rapaport
* @date   23.10.2019
* @brief  initializations and api layer to the specific platform functions 
*/

/**
* include files
*/
#include "psp_pragmas.h"

#ifdef D_HI_FIVE1
   #include "encoding.h"
   #include "platform.h"
#endif
/**
* definitions
*/

/* LEDs outputs definition */
#ifdef D_HI_FIVE1
    #define D_LED_GREEN_ON 1
    #define D_LED_BLUE_ON  2
#else
    #define D_LED_GREEN_ON 'X' //unknown
    #define D_LED_BLUE_ON  'X' //unknown
   PRE_COMPILED_MSG("no platform was defined")
#endif

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
#ifdef D_HI_FIVE1
   extern void _init();
#endif


/**
* global variables
*/

/**
* global variables
*/

/**
* functions
*/

/**
* demoPlatformInit - Initialize board related stuff
*
* */
void demoPlatformInit(void);


/**
* demoOutputMsg - output (usually, but not necessarily - print it out) a given string,
*                 using the platform means for that.
*
* const void *str - pointer to a string to be printed out
*
* size_t size - number of characters to print
*
* */
void demoOutputMsg(const void *str, size_t size);

/**
* demoOutputLed - sets LED output according input request.
*
* const D_LED_ACTION ledAct - LED action to do
*
* The "LED action" is defined per each platform
*
* */
void demoOutputLed(const int ledAct);


