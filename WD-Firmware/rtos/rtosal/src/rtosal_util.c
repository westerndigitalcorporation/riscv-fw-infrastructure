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
#include "rtosal_macro.h"
#ifdef D_USE_FREERTOS
   #include "task.h"
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */
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
extern void rtosalParamErrorNotification(const void *pParam, u32_t uiErrorCode);
extern void rtosalTimerIntHandler(void);
extern void rtosalHandleEcall(void);


/**
* global variables
*/
rtosalParamErrorNotification_t fptrParamErrorNotification = rtosalParamErrorNotification;
rtosalApplicationInit_t  fptrAppInit = NULL;
u32_t g_rtosalContextSwitch = 0;

/**
* This API initializes the RTOS and triggers the scheduler operation
*
* @param fptrInit - pointer to a function which creates the application
*                   tasks, mutexs, semaphores, queues, etc.
*
* @return calling this function will never return
*/
RTOSAL_SECTION void rtosalStart(rtosalApplicationInit_t fptrInit)
{
    /* register E_CALL exception handler */
    pspRegisterIsrExceptionHandler(rtosalHandleEcall, E_EXC_ENVIRONMENT_CALL_FROM_MMODE);

    /* register timer interrupt handler */
    pspRegisterIsrCauseHandler(rtosalTimerIntHandler, E_MACHINE_TIMER_CAUSE);


#ifdef D_USE_FREERTOS
   fptrInit(NULL);
   vTaskStartScheduler();
#elif D_USE_THREADX
   fptrAppInit = fptrInit;
   tx_kernel_enter();
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */
}

/**
* Set param error notification function
*
* @param fptrRtosalParamErrorNotification - notification function
*
* @return none
*/
RTOSAL_SECTION void rtosalParamErrorNotifyFuncRegister(rtosalParamErrorNotification_t fptrRtosalParamErrorNotification)
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
RTOSAL_SECTION void rtosalContextSwitchIndicationSet(void)
{
	g_rtosalContextSwitch = 1;
}

/**
*
*
* @param none
*
* @return none
*/
RTOSAL_SECTION void rtosalContextSwitchIndicationClear(void)
{
	g_rtosalContextSwitch = 0;
}

/**
* This API is ThreadX specific. It is invoked during ThreadX init flow
*
* @param pMemory - pointer to memory used for the initial application init
*
* @return none
*/
#ifdef D_USE_THREADX
RTOSAL_SECTION void tx_application_define(void *pMemory)
{
   fptrAppInit(pMemory);
}
#endif /* #ifdef D_USE_THREADX */

/**
* This function is invoked by the system timer interrupt
*
* @param  none
*
* @return none
*/
RTOSAL_SECTION void rtosalTick(void)
{
#ifdef D_USE_FREERTOS
   if (xTaskIncrementTick() == pdTRUE)
   {
	   contextSwitchFunc();
   }
#elif D_USE_THREADX
   #error "Add THREADX appropriate definitions"
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */
}


RTOSAL_SECTION void rtosalEndScheduler( void )
{
	/* Not implemented. */
	for( ;; );
}

