/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2021 Western Digital Corporation or its affiliates.
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
* @file   demo_getchar.c
* @author Ofer Shinaar
* @date   23.02.2021
* @brief  This demo show getchar usage
*/

/**
* include files
*/
#include "common_types.h"
#include "demo_platform_al.h"
#include "demo_utils.h"

/**
* definitions
*/

#define D_MAX_STR_SIZE 64

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

void demoStart(void)
{

  M_DEMO_START_PRINT();

#ifdef D_NONE_AUTOMATION

  u08_t ucIdx=0;
  u08_t ucStringArr[D_MAX_STR_SIZE] = {0};

  printfNexys("please write any string following [enter]. limited characters=64. \n\r");

  do
  {
    ucStringArr[ucIdx] = uartGetchar();
    ucIdx++;
    if (ucIdx == D_MAX_STR_SIZE)
    {
      printfNexys("Too many characters pressed -> terminating ");
      return;
    }
  }while(ucStringArr[ucIdx-1] != 0x0D && ucStringArr[ucIdx-1] != 0x0a); //user press 'enter' or new LF

  printfNexys("Echoing your input: %s",ucStringArr);

#endif

  M_DEMO_END_PRINT();
}

