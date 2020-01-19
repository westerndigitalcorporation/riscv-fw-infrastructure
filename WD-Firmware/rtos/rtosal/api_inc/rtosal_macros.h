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
* @file   rtosal_macros.h
* @author Ronen Haen
* @date   07.02.2019
* @brief  The file defines the RTOS AL macros
*/
#ifndef __RTOSAL_MACRO_H__
#define __RTOSAL_MACRO_H__

/**
* include files
*/

/**
* macros
*/
/* error checking macro */
#if (D_RTOSAL_ERROR_CHECK==1)
   #define M_RTOSAL_VALIDATE_FUNC_PARAM(param, conditionMet, returnCode) \
      if (conditionMet) \
      { \
         fptrParamErrorNotification((const void*)(param), returnCode); \
         return (returnCode); \
      }
#else
   #define M_RTOSAL_VALIDATE_FUNC_PARAM(param, conditionMet, returnCode)
#endif /* #if (D_RTOSAL_ERROR_CHECK==1) */

#define RTOSAL_SECTION __attribute__((section("RTOSAL_SEC")))


#ifdef D_USE_FREERTOS
   /* set mepc to specific task stack - this macro is used by comrv only */
   #define M_RTOSAL_SAVE_MEPC_TO_APP_STACK(pStackTop, val) (*pStackTop = (rtosalStack_t)val)
   /* read mepc from specific task stack - this macro is used by comrv only */
   #define M_RTOSAL_READ_MEPC_FROM_APP_STACK(pStackTop) (*pStackTop)
#elif D_USE_THREADX
   #error *** TODO: need to define the TBD ***
   #define M_RTOSAL_SAVE_MEPC_TO_APP_STACK(pStackTop, val) TBD
   #error *** TODO: need to define the TBD ***
   #define M_RTOSAL_READ_MEPC_FROM_APP_STACK(pStackTop) TBD
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

#endif /* __RTOSAL_MACRO_H__ */
