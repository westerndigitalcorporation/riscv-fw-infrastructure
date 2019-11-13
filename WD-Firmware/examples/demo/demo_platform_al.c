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
* @file   demo_platform_al.c
* @author Nati Rapaport
* @date   23.10.2019
* @brief  initializations and api layer to the specific platform functions 
*/

/**
* include files
*/
#include "common_types.h"
#include "demo_platform_al.h"
#include "psp_api.h"

#ifdef D_RV_HI_FIVE1
   #include <unistd.h>
#else
   PRE_COMPILED_MSG("no platform was defined")
#endif

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
* global variables
*/

/**
* internal functions
*/


/**
* demoLedsInit - LEDs initialization per board's specifications
*
* */
void demoLedsInit(void)
{
#ifdef D_RV_HI_FIVE1
	GPIO_REG(GPIO_INPUT_EN)    &= ~((0x1<< RED_LED_OFFSET) | (0x1<< GREEN_LED_OFFSET) | (0x1 << BLUE_LED_OFFSET)) ;
	GPIO_REG(GPIO_OUTPUT_EN)   |=  ((0x1<< RED_LED_OFFSET)| (0x1<< GREEN_LED_OFFSET) | (0x1 << BLUE_LED_OFFSET)) ;
	GPIO_REG(GPIO_OUTPUT_VAL)  &= ~((0x1<< RED_LED_OFFSET) | (0x1<< GREEN_LED_OFFSET) | (0x1 << BLUE_LED_OFFSET)) ;

#endif
}

/**
* demoUartInit - URAT initialization per board's specifications
*
* */

void demoUartInit(void)
{
#ifdef D_RV_HI_FIVE1
	/* Empty implementation */
#endif
}

/**
* api functions
*/

/**
* demoPlatformInit - Initialize board related stuff
*
* */
void demoPlatformInit(void)
{
#ifdef D_RV_HI_FIVE1
	_init();
#endif
    demoLedsInit();
    demoUartInit();

}

/**
* demoOutputMsg - output (usually, but not necessarily - print it out) a given string,
*                 using the platform means for that.
*
* const void *str - pointer to a string to be printed out
*
* size_t size - number of characters to print
*
* */
void demoOutputMsg(const void *str, size_t size)
{
#ifdef D_RV_HI_FIVE1
	write(1, str, size);
#endif
}


/**
* demoOutputLed - sets LED output according input request.
*
* const D_LED_ACTION ledAct - LED action to do
*
* The "LED action" is defined per each platform
*
* */
void demoOutputLed(const int ledAct)
{
#ifdef D_RV_HI_FIVE1
	switch (ledAct)
	{
        case D_LED_GREEN_ON:
           GPIO_REG(GPIO_OUTPUT_VAL)  ^=   (0x1 << GREEN_LED_OFFSET) ;
 	       break;
        case D_LED_BLUE_ON:
           GPIO_REG(GPIO_OUTPUT_VAL)  ^=   (0x1 << BLUE_LED_OFFSET) ;
 	       break;
        default:
           break;
	}

#endif
}



