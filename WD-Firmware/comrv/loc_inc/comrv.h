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
* @file   comrv.h
* @author Ronen Haen
* @date   11.06.2019
* @brief  The defines COM-RV private interfaces
* 
*/
#ifndef  __COMRV_H__
#define  __COMRV_H__

/**
* include files
*/
#include "psp_api.h"

/**
* definitions
*/

/**
* macros
*/
#define D_COMRV_TEXT_SECTION     __attribute__((section("COMRV_TEXT_SEC")))
#define D_COMRV_DATA_SECTION     __attribute__((section("COMRV_DATA_SEC")))
#define D_COMRV_RODATA_SECTION   __attribute__((section("COMRV_RODATA_SEC")))

#ifdef D_COMRV_ERROR_NOTIFICATIONS
   #define M_COMRV_ERROR(stError,errorNum,token)   stError.uiErrorNum = errorNum; \
                                                   stError.uiToken    = token; \
                                                   comrvErrorHook(&stError);
#else
   #define M_COMRV_ERROR(stError,errorNum,token)
#endif /* D_COMRV_ERROR_NOTIFICATIONS */

#ifdef D_COMRV_ASSERT_ENABLED
   #ifdef D_COMRV_ERROR_NOTIFICATIONS
      #define M_COMRV_ASSERT_ACTION(error,tokenVal)      M_COMRV_ERROR(stErrArgs,error,tokenVal);
   #else
      #define M_COMRV_ASSERT_ACTION(error,tokenVal)      while(1);
   #endif /* D_COMRV_ERROR_NOTIFICATIONS */
   #define M_COMRV_ASSERT(expression, expectedVlue, error, tokenVal)  if (M_COMRV_BUILTIN_EXPECT((expression) != (expectedVlue), 0)) \
                                                                      { \
                                                                         M_COMRV_ASSERT_ACTION(error,tokenVal); \
                                                                      }
#else
   #define M_COMRV_ASSERT(expression, expectedVlue, error, tokenVal)
#endif /* D_COMRV_ASSERT_ENABLED */

#ifdef D_COMRV_RTOS_SUPPORT

   #define M_COMRV_DISABLE_INTS(uiOutPrevIntState) M_PSP_CLEAR_AND_READ_CSR(uiOutPrevIntState, D_PSP_MSTATUS_NUM, (D_PSP_MSTATUS_UIE_MASK | D_PSP_MSTATUS_SIE_MASK | D_PSP_MSTATUS_MIE_MASK) );
   #define M_COMRV_ENABLE_INTS(uiOutPrevIntState)  M_PSP_SET_CSR(D_PSP_MSTATUS_NUM, uiOutPrevIntState)
   #define M_COMRV_ENTER_CRITICAL_SECTION()  ret = comrvEnterCriticalSectionHook(); \
                                             M_COMRV_ASSERT(ret, D_COMRV_SUCCESS, D_COMRV_ENTER_CRITICAL_SECTION_ERR, unToken.uiValue);
   #define M_COMRV_EXIT_CRITICAL_SECTION()   ret = comrvExitCriticalSectionHook(); \
                                             M_COMRV_ASSERT(ret, D_COMRV_SUCCESS, D_COMRV_EXIT_CRITICAL_SECTION_ERR, unToken.uiValue);
#else
   #define M_COMRV_ENTER_CRITICAL_SECTION()
   #define M_COMRV_EXIT_CRITICAL_SECTION()
   #define M_COMRV_DISABLE_INTS(uiOutPrevIntState) (void)uiOutPrevIntState
   #define M_COMRV_ENABLE_INTS(uiOutPrevIntState)  (void)uiOutPrevIntState
#endif /* D_COMRV_RTOS_SUPPORT */

/* M_PSP_BUILTIN_EXPECT instruction provides branch
   prediction information. The condition parameter is the expected
   comparison value. If it is equal to 1 (true), the condition
   is likely to be true, in other case condition is likely to be false.
   this provides us a way to take rare cases out of the critical execution path */
#define M_COMRV_BUILTIN_EXPECT(condition, expected)  M_PSP_BUILTIN_EXPECT(condition, expected)

/* invalidate data cache */
#define M_COMRV_DCACHE_FLUSH(pAddress, uiNumOfBytes) comrvInvalidateDataCacheHook(pAddress, uiNumOfBytes);

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

#endif /* __COMRV_H__ */
