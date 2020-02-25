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
#if defined(D_HI_FIVE1)
   #include <unistd.h>
#elif defined(D_NEXYS_A7)
   #include "printf.h"
#else
   PRE_COMPILED_MSG("no platform was defined")
#endif

#include "demo_platform_al.h"
#include "psp_api.h"

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
#ifdef D_HI_FIVE1
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
#ifdef D_HI_FIVE1
   /* Empty implementation */
#endif
#ifdef D_NEXYS_A7
   /* Whisper bypass - force UART state to be "non-busy" (== 0) so print via UART will be displayed on console
    * when running with Whisper */
    u32_t* pUartState = (u32_t*)(UART_BASE_ADDRESS+0x8);
   *pUartState = 0 ;

   printfNexys("------------------------------------------");
   printfNexys("Hello from SweRV_EH1 core running on NexysA7  ");
   printfNexys("Following: Demo RTOSAL on FreeRTOS kernel" );
   printfNexys("------------------------------------------");
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
#ifdef D_HI_FIVE1
	_init();
#elif D_NEXYS_A7
	// Nada for now
#endif
    demoLedsInit();
    demoUartInit();

}

/**
* demoOutputMsg - output (usually, but not necessarily - print it out) a given string,
*                 using the platform means for that.
*
* const void *pStr - pointer to a string to be printed out
*
* u32_t uiSize - number of characters to print
*
* */
void demoOutputMsg(const void *pStr, u32_t uiSize)
{
#ifdef D_HI_FIVE1
	write(1, pStr, uiSize);
#elif D_NEXYS_A7
	printfNexys(pStr);
#endif
}


/**
* demoOutputLed - sets LED output according input request.
*
* const s32_t - LED action to do
*
* The "LED action" is defined per each platform
*
* */
void demoOutputLed(const s32_t siLedAct)
{
#ifdef D_HI_FIVE1
   switch (siLedAct)
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

