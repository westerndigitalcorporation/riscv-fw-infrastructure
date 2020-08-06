/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2020 Western Digital Corporation or its affiliates.
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
#ifdef D_HI_FIVE1
  #include <string.h>
  #include <stdlib.h>
  #define M_DEMO_START_PRINT()    demoOutputMsg("->> ",4);                  \
                                  demoOutputMsg(__FILE__,strlen(__FILE__)); \
                                  demoOutputMsg("     start\n",11);

  #define M_DEMO_END_PRINT()      demoOutputMsg("->> ",4);                  \
                                  demoOutputMsg(__FILE__,strlen(__FILE__)); \
                                  demoOutputMsg("     end\n",11);

  #define M_DEMO_ERR_PRINT()      char cStringValue[10];                     \
                                  itoa(__LINE__,cStringValue, 10);           \
                                  demoOutputMsg("->> ",4);                   \
                                  demoOutputMsg(__FILE__,strlen(__FILE__));  \
                                  demoOutputMsg("     error -> line",18);    \
                                  demoOutputMsg(cStringValue,strlen(cStringValue));\
                                  demoOutputMsg("\n",1);

#else
  #define M_DEMO_START_PRINT()    demoOutputMsg("->> '%s' .... start", __FILE__);
  #define M_DEMO_END_PRINT()      demoOutputMsg("->> '%s' .... ends", __FILE__);
  #define M_DEMO_ERR_PRINT()      demoOutputMsg("->> %s  ..error -> line %d", __FILE__, __LINE__);
#endif

#define M_DEMO_ENDLESS_LOOP() M_DEMO_ERR_PRINT()       \
                              while(1)                 \
                              {                        \
                                 asm volatile ("nop"); \
                                 asm volatile ("nop"); \
                              }

/* Convert mSec duration to number of clock cycles */
#define M_DEMO_MSEC_TO_CYCLES(duration)   (duration * (D_CLOCK_RATE / D_PSP_MSEC))

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
