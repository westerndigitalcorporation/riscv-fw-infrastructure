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

/***************************************************************************/
/**         MULTIPLE INCLUSION                                            **/
/***************************************************************************/
#ifndef __MATI_UTILITIES_H
#define __MATI_UTILITIES_H


/***************************************************************************/
/**         INCLUDES                                                      **/
/***************************************************************************/
#include "mati_errors.h"
#include "mati_types.h"

/***************************************************************************/
/**         DEFINITIONS                                                   **/
/***************************************************************************/
#define D_TIMER0_ID      0x0
#define D_TIMER0_COUNT   0x21
#define D_TIMER0_CONTROL 0x22
#define D_TIMER0_LIMIT   0x23
#define D_TIMER1_ID      0x1
#define D_TIMER1_COUNT   0x100
#define D_TIMER1_CONTROL 0x101
#define D_TIMER1_LIMIT   0x102

/***************************************************************************/
/**    MACROS                                                             **/
/***************************************************************************/

/********************************************************************************************/
/* merge  test code, test number and error code to 32 bit                                   */
/*                                                                                          */
/*    31                23                15                7                0   bit number */
/*    |   group number   |    test number  |    error code  |    error code  |   data       */
/*                                                                                          */
/********************************************************************************************/
#define D_SET_MERGE_ERROR_CODE(group_number, test_number, test_error)            (group_number << 0x18 | test_number << 0x10 | test_error)

/***************************************************************************/
/**         TYPEDEFS                                                      **/
/***************************************************************************/

/***************************************************************************/
/**    EXPORTED GLOBALS                                                   **/
/***************************************************************************/

/***************************************************************************/
/**         ENUM                                                          **/
/***************************************************************************/


/***************************************************************************/
/**         FUNCTIONS PROTOTYPES                                          **/
/***************************************************************************/

E_MATI_Result  f_mati_frameworkStop( S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
E_MATI_Result  f_mati_frameworkIsRunning( S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
E_MATI_Result  f_mati_frameworkStart( S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
E_MATI_Result  f_mati_frameworkWaitForCompletion( S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);

E_MATI_Result  f_mati_testInProgress(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
E_MATI_Result  f_mati_setTestStarted(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
E_MATI_Result  f_mati_setTestCompleted(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);

u32_t           f_mati_busyWaitSleep(u32_t* pWaitVariable, u32_t uiWaitValue, u32_t uiTimeOut);
u32_t           f_mati_busyWaitYield(u32_t* pWaitVariable, u32_t uiWaitValue, u32_t uiTimeOut);
E_MATI_Result  f_mati_secureSet(u32_t* pResource, u32_t uiValue);
E_MATI_Result  f_mati_setErrorBit(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB, E_TEST_ERROR eValue);
E_MATI_Result  f_mati_clearErrorBits(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
E_TEST_ERROR   f_mati_getErrorBits(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
u32_t           f_mati_getRandom();
void           f_mati_startTestAndWait(u08_t ucUsedThreadsBitmask);
void           f_mati_resetAomGlobal();
void           f_mati_validateTestResult(E_TEST_CONFIG testType_p, S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
void           f_mati_reset_aom_area();
void           f_mati_initializeBtstInputParams(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
S_FRAMEWORK_BTSTS_PARAMS_PTR f_mati_getInputParams(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
u08_t           f_mati_resetTimer(u08_t ucTimerToReset);
void           f_mati_getTime(u64_t *ulCounter_p);
void           f_mati_advanceULCounter(u64_t* ulCounter);
void           f_mati_resetULCounter(u64_t* ulCounter);
void*          f_mati_taskGetStackBase(void);
void*          f_mati_taskGetStackTop(void);

// stub
void SET(u32_t setMsg);


/***************************************************************************/
/**         END OF FILE                                                   **/
/***************************************************************************/
#endif /* EOF */

