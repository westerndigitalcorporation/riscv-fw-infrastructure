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
* @file   rtosal_misc.c
* @author Ronen Haen
* @date   21.01.2019 
* @brief  The file implements the RTOS AL misc API
* 
*/

/**
* include files
*/
#include "rtosal_util_api.h"
#include "rtosal.h"
#ifdef D_USE_FREERTOS
   #include "task.h"
#endif /* #ifdef D_USE_FREERTOS */

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
extern void rtosalParamErrorNotification(const void *pParam, u32_t uiErrorCode);


/**
* global variables
*/
rtosalParamErrorNotification_t fptrParamErrorNotification = rtosalParamErrorNotification;
rtosalApplicationInit_t  fptrAppInit = NULL;

/**
* This API initializes the RTOS and triggers the scheduler operation
*
* @param fptrInit - pointer to a function which creates the application
*                   tasks, mutexs, semaphores, queues, etc.
*
* @return calling this function will never return
*/
void rtosalStart(rtosalApplicationInit_t fptrInit)
{
#ifdef D_USE_FREERTOS
   fptrInit(NULL);
   vTaskStartScheduler();
#elif D_USE_THREADX
   fptrAppInit = fptrInit;
   tx_kernel_enter();
#endif /* #ifdef D_USE_FREERTOS */
}

/**
* Set param error notification function
*
* @param fptrRtosalParamErrorNotification - notification function
*
* @return none
*/
void rtosalParamErrorNotifyFuncRegister(rtosalParamErrorNotification_t fptrRtosalParamErrorNotification)
{
   fptrParamErrorNotification = fptrRtosalParamErrorNotification;
}

/**
*
*
* @param none
*
* @return none
*/
void rtosalContextSwitchIndicationSet(void)
{
}

/**
* This API is ThreadX specific. It is invoked during ThreadX init flow
*
* @param pMemory - pointer to memory used for the initial application init
*
* @return none
*/
#ifdef D_USE_THREADX
void tx_application_define(void *pMemory)
{
   fptrAppInit(pMemory);
}
#endif /* #ifdef D_USE_FREERTOS */

