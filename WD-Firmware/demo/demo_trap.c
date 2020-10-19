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
#include "psp_api.h"
#include "demo_platform_al.h"
#include "demo_utils.h"


/**
* definitions
*/
#define D_IN_EXCEPTION_HANDLER      (0)
#define D_TRAPS_TEST_RESULT          (M_PSP_BIT_MASK(D_IN_EXCEPTION_HANDLER))

#define D_ILLEGAL_CSR_ADDRESS        (0x7)

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
u32_t g_uiTestWayPoints = 0;

/**
* functions
*/

/**
 * demoIllegalInstructionExceptionHandler - Handle exception
 *
 */
void demoIllegalInstructionExceptionHandler(void)
{
  u32_t uiMepc =  M_PSP_READ_CSR(D_PSP_MEPC_NUM);

  /* Mark that exception handler visited */
  g_uiTestWayPoints |= M_PSP_BIT_MASK(D_IN_EXCEPTION_HANDLER);

  /* move return address from exception one forward in order to avoid infinitely repeating exception */
  M_PSP_WRITE_CSR(D_PSP_MEPC_NUM, uiMepc + 4);
}

/**
 * demoIllegalInstructionExceptionHandlingTest - verify exception handled properly
 *
 */
void demoIllegalInstructionExceptionHandlingTest(void)
{
  /* register trap handler */
  /* TODO: Replace trap handler registration with dedicated PSP API */
  M_PSP_WRITE_CSR(D_PSP_MTVEC_NUM, &M_PSP_VECT_TABLE);

  pspMachineInterruptsRegisterExcpHandler(demoIllegalInstructionExceptionHandler, E_EXC_ILLEGAL_INSTRUCTION);

  /* create illegal instruction exception by writing to non valid CSR address */
  M_PSP_WRITE_CSR(D_ILLEGAL_CSR_ADDRESS, 0);

  /* verify all test way points were visited */
  if(g_uiTestWayPoints != D_TRAPS_TEST_RESULT)
  {
    /* Test failed */
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }
}

/**
 * demoStart - startup point of the demo application. called from main function.
 *
 */
void demoStart(void)
{
  M_DEMO_START_PRINT();
  /* verify proper exception handling */
  demoIllegalInstructionExceptionHandlingTest();

  M_DEMO_END_PRINT();

}
