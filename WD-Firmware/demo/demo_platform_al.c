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
#elif defined(D_SWERV_EH1) || defined(D_SWERV_EH2) ||defined(D_SWERV_EL2)
   #include "bsp_printf.h"
   #include "bsp_mem_map.h"
   #include "bsp_version.h"
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
#ifdef D_SWERV_EH1
  swervolfVersion_t stSwervolfVersion;

  versionGetSwervolfVer(&stSwervolfVersion);

 /* Whisper bypass - force UART state to be "non-busy" (== 0) so print via UART will be displayed on console
  * when running with Whisper */
  u32_t* pUartState = (u32_t*)(D_UART_BASE_ADDRESS+0x8);
  *pUartState = 0 ;

  /* init uart */
  uartInit();


  printfNexys("------------------------------------------");
  printfNexys("Hello from SweRV core running on NexysA7  ");
  printfNexys("Core list:");
  printfNexys("\t EH1 = 11 ");
  printfNexys("\t EL2 = 16 ");
  printfNexys("Running demo on core %d...", M_PSP_READ_CSR(D_PSP_MARCHID_NUM));
  printfNexys("------------------------------------------");
  printfNexys("SweRVolf version %d.%d%d (SHA %08x) (dirty %d)",
                   stSwervolfVersion.ucMajor,
                   stSwervolfVersion.ucMinor,
                   stSwervolfVersion.ucRev,
                   stSwervolfVersion.ucSha,
                   stSwervolfVersion.ucDirty);
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
#elif defined(D_SWERV_EH1) || defined(D_SWERV_EH2) ||defined(D_SWERV_EL2)
  // Nada for now
#endif
  /* init LED GPIO*/
  demoLedsInit();

  /* init Uart for 115200 baud, 8 data bits, 1 stop bit, no parity */
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
#ifdef D_HI_FIVE1
void demoOutputMsg(const void *pStr, u32_t uiSize)
{
  write(1, pStr, uiSize);
}
#endif

/**
* demoOutputToggelLed - sets LED output according input request.
*
*
* The "LED action" is defined per each platform, it is the led color
*
* */
void demoOutputToggelLed(void)
{
  static u08_t ucLedAct;
#ifdef D_HI_FIVE1
  switch (ucLedAct)
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
#elif defined(D_SWERV_EH1) || defined(D_SWERV_EH2) ||defined(D_SWERV_EL2)
  demoOutpuLed(ucLedAct);
#endif

  ucLedAct = !ucLedAct;
}

/**
* demoOutputLed - sets LED output on/off
*
* const uiOnOffMode = 0/1
*
*
* */
void demoOutpuLed(const u08_t ucOnOffMode)
{
#ifdef D_SWERV_EH1
  M_PSP_ASSERT(ucOnOffMode>1);
  M_PSP_WRITE_REGISTER_32(D_LED_BASE_ADDRESS, ucOnOffMode);
#endif
}

