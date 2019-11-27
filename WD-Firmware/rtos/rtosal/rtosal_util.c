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
* @file   rtosal_util.c
* @author Ronen Haen
* @date   21.01.2019 
* @brief  The file implements the RTOS AL misc API
* 
*/

/**
* include files
*/
#include "psp_types.h"
#include "rtosal_util.h"
#include "rtosal_macros.h"
#include "rtosal_task_api.h"
#ifdef D_USE_FREERTOS
   #include "FreeRTOS.h"
   #include "task.h"
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */
#include "psp_api.h"
#include "demo_platform_al.h"

/**
* definitions
*/
#define D_IDLE_TASK_SIZE      450
#define D_TIMER_TASK_SIZE     450

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
/* application specific timer-tick handler function. The function should be implemented and registered
 * by the application  */
extern rtosalTimerTickHandler_t fptrTimerTickHandler ;

/**
* global variables
*/
u32_t g_rtosalContextSwitch = 0;
u32_t g_rtosalIsInterruptContext = D_RTOSAL_NON_INT_CONTEXT;

#ifdef D_USE_FREERTOS
/* Idle-task and Timer-task are created by FreeRtos and not by this application */
rtosalTask_t stIdleTask;
rtosalStackType_t uIdleTaskStackBuffer[D_IDLE_TASK_SIZE];
rtosalTask_t stTimerTask;
rtosalStackType_t uTimerTaskStackBuffer[D_TIMER_TASK_SIZE];
#endif

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
* @brief check if in ISR context
*
* @param None
*
* @return u32_t            - D_NON_INT_CONTEXT
*                          - non zero value - interrupt context
*/
RTOSAL_SECTION u32_t rtosalIsInterruptContext(void)
{
   return (g_rtosalIsInterruptContext > 0);
}

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
   if (xTaskIncrementTick() == D_PSP_TRUE)
   {
	   vTaskSwitchContext();
   }
#elif D_USE_THREADX
   #error "Add THREADX appropriate definitions"
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */
}

#ifdef D_USE_FREERTOS
/**
 * vApplicationGetIdleTaskMemory - Called from FreeRTOS upon Idle task creation, to get task's memory buffers
 *
 * rtosalStaticTask_t **ppxIdleTaskTCBBuffer - pointer to Task's Control-Block buffer (pointer to pointer as it is output parameter)
 * rtosalStack_t **ppxIdleTaskStackBuffer - pointer to Task's stack buffer  (pointer to pointer as it is output parameter)
 * uint32_t *pulIdleTaskStackSize - Task's stack size (pointer, as it is output parameter)
 *
 */
void vApplicationGetIdleTaskMemory(rtosalStaticTask_t **ppxIdleTaskTCBBuffer, rtosalStack_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
  *ppxIdleTaskTCBBuffer = (rtosalStaticTask_t*)&stIdleTask;
  *ppxIdleTaskStackBuffer = (rtosalStack_t*)&uIdleTaskStackBuffer[0];
  *pulIdleTaskStackSize = D_IDLE_TASK_SIZE;
}

/**
 * vApplicationGetTimerTaskMemory - Called from FreeRTOS upon Timer task creation, to get task's memory buffers
 *
 * rtosalStaticTask_t **ppxTimerTaskTCBBuffer - pointer to Task's Control-Block buffer (pointer to pointer as it is output parameter)
 * rtosalStack_t **ppxTimerTaskStackBuffer - pointer to Task's stack buffer  (pointer to pointer as it is output parameter)
 * uint32_t *pulTimerTaskStackSize - Task's stack size (pointer, as it is output parameter)
 *
 */
void vApplicationGetTimerTaskMemory(rtosalStaticTask_t **ppxTimerTaskTCBBuffer, rtosalStack_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
{
  *ppxTimerTaskTCBBuffer = (rtosalStaticTask_t*)&stTimerTask;
  *ppxTimerTaskStackBuffer = (rtosalStack_t*)&uTimerTaskStackBuffer[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/**
 * vApplicationMallocFailedHook - Called from FreeRTOS upon malloc failure
 *
 * Not in use
 *
 */
void vApplicationMallocFailedHook( void )
{
    /* The malloc failed hook is enabled by setting
    configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

    Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues, software
    timers, and semaphores.  The size of the FreeRTOS heap is set by the
    configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	demoOutputMsg("malloc failed\n", 14);
    for( ;; );
}


/**
 * vApplicationStackOverflowHook - Called from FreeRTOS upon stack-overflow
 *
 * void* xTask - not in use
 * signed char *pcTaskName - not in use
 *
 */
void vApplicationStackOverflowHook(void* xTask, signed char *pcTaskName)
{
    ( void ) pcTaskName;
    ( void ) xTask;

    /* Run time stack overflow checking is performed if
    configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected.  pxCurrentTCB can be
    inspected in the debugger if the task name passed into this function is
    corrupt. */
    demoOutputMsg("Stack Overflow\n", 15);
    for( ;; );
}

/**
 * vApplicationStackOverflowHook - Called from FreeRTOS
 *
 * Currently empty function
 *
 */
void vApplicationIdleHook( void )
{
	/*demoOutputMsg("Idle Task Hook\n", 15);*/
}

/**
 * vApplicationTickHook - Called from FreeRTOS upon any timer's tick
 *
 */
//extern void rtosalContextSwitchIndicationClear(void); /* Temporarily here! */
void vApplicationTickHook( void )
{
	if (NULL != fptrTimerTickHandler)
	{
		fptrTimerTickHandler();
	}
}
#endif /* D_USE_FREERTOS */
