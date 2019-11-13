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
* @file   rtosal_starter.c
* @author Nati Rapaport
* @date   13.11.2019
* @brief  The file implements the RTOS AL start & end functions
*
*/

/**
* include files
*/
#include "rtosal_starter_api.h"
#include "rtosal_macro.h"
#include "common_types.h"
#include "psp_api.h"
#ifdef D_USE_FREERTOS
   #include "FreeRTOS.h"
   #include "task.h"
#else
   #error "Add appropriate RTOS definitions"
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
extern void rtosalHandleEcall(void);
extern void rtosalTimerIntHandler(void);


/**
* global variables
*/
rtosalApplicationInit_t  fptrAppInit = NULL;


/**
* Initialization of the RTOS and starting the scheduler operation
*
* @param fptrInit - pointer to a function which creates the application
*                   tasks, mutexs, semaphores, queues, etc.
*
* @return calling this function will never return
*/
RTOSAL_SECTION void rtosalStart(rtosalApplicationInit_t fptrInit)
{
#ifdef D_USE_FREERTOS
    /* register E_CALL exception handler */
    pspRegisterExceptionHandler(rtosalHandleEcall, E_EXC_ENVIRONMENT_CALL_FROM_MMODE);

    /* register timer interrupt handler */
    pspRegisterInterruptHandler(rtosalTimerIntHandler, E_MACHINE_TIMER_CAUSE);

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
* End the activity of the scheduler
*
* @param  - none
*
* @return - none
* */
RTOSAL_SECTION void rtosalEndScheduler( void )
{
	/* Not implemented. */
	for( ;; );
}
