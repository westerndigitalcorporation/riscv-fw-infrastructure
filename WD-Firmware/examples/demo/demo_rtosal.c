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
* @file   demo_rtosal_demo.c
* @author Nati Rapaport
* @date   23.10.2019
* @brief  application of RTOS abstraction layer basic demonstration
*/

/**
* include files
*/
#include <stdlib.h>
#include <unistd.h>

#include "common_types.h"
#include "demo_platform_al.h"

#include "psp_api.h"

#include "rtosal_starter_api.h"
#include "rtosal_semaphore_api.h"
#include "rtosal_defines.h"
#include "rtosal_task_api.h"
#include "rtosal_queue_api.h"
#include "rtosal_time_api.h"
#include "rtosal_util_api.h"

/**
* definitions
*/

/* The rate at which data is sent to the queue, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define D_MAIN_QUEUE_SEND_PERIOD_MS            pdMS_TO_TICKS( 200 )

/* The period of the example software timer, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define D_MAIN_SOFTWARE_TIMER_PERIOD_MS        pdMS_TO_TICKS( 1000 )

/* The number of items the queue can hold.  This is 1 as the receive task
has a higher priority than the send task, so will remove items as they are added,
meaning the send task should always find the queue empty. */
#define D_MAIN_QUEUE_LENGTH                    ( 1 )

/* Stack size of the tasks in this application */
#define D_RX_TASK_STACK_SIZE  600
#define D_TX_TASK_STACK_SIZE  configMINIMAL_STACK_SIZE
#define D_SEM_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define D_IDLE_TASK_SIZE      configMINIMAL_STACK_SIZE

/**
* macros
*/

/**
* types
*/

/**
* local prototypes
*/
static void demoRtosalCreateTasks(void *pParam);
static void demoReceiveMsgTask( void *pvParameters );
static void demoSendMsgTask( void *pvParameters );
static void demoSemaphoreTask( void *pvParameters );
static void demoTimerCallback( void* xTimer );


/**
* external prototypes
*/
extern void pspTrapUnhandled(void);


/**
* global variables
*/
/* The queue used by the send-msg and receive-msg tasks. */
static rtosalMsgQueue_t stMsgQueue;

/* The semaphore (in this case binary) that is used by the FreeRTOS tick hook
 * function and the event semaphore task.
 */
static rtosalSemaphore_t stEventSemaphore;

/* The counters used by the various examples.  The usage is described in the
 * comments at the top of this file.
 */
static volatile uint32_t ulCountOfTimerCallbackExecutions = 0;
static volatile uint32_t ulCountOfItemsReceivedOnQueue = 0;
static volatile uint32_t ulCountOfReceivedSemaphores = 0;

/* Variables related to the tasks in this application */
static rtosalTask_t stRxTask;
static rtosalTask_t stTxTask;
static rtosalTask_t stSemTask;
static rtosalTimer_t stLedTimer;
static rtosalStackType_t uRxTaskStackBuffer[D_RX_TASK_STACK_SIZE];
static rtosalStackType_t uTxTaskStackBuffer[D_TX_TASK_STACK_SIZE];
static rtosalStackType_t uSemTaskStackBuffer[D_SEM_TASK_STACK_SIZE];
static s08_t cQueueBuffer[D_MAIN_QUEUE_LENGTH * sizeof(uint32_t)];
/* Idle-task and Timer-task are created by FreeRtos and not by this application */
static rtosalTask_t stIdleTask;
static rtosalStackType_t uIdleTaskStackBuffer[D_IDLE_TASK_SIZE];
static rtosalTask_t stTimerTask;
static rtosalStackType_t uTimerTaskStackBuffer[configTIMER_TASK_STACK_DEPTH];


/**
* functions
*/
/**
 * demoStart - startup point of the demo application. called from main function.
 *
 */
void demoStart(void)
{
	rtosalStart(demoRtosalCreateTasks);
}

/**
 * demoRtosalCreateTasks
 *
 * Initialize the application:
 * - Register unhandled exceptions, Ecall exception and Timer ISR
 * - Create the message queue
 * - Create the Tx and Rx tasks
 * - Create the Semaphore and the Sem-Task
 * - Create the software timer
 *
 * This function is called from RTOS abstraction layer. After its completion, the scheduler is kicked on
 * and the tasks are start to be active
 *
 */
void demoRtosalCreateTasks(void *pParam)
{
	
    u32_t res;
    pspExceptionCause_t cause;

    /* Disable the machine & timer interrupts until setup is done. */
    M_PSP_CLEAR_CSR(mie, D_PSP_MIP_MEIP);
    M_PSP_CLEAR_CSR(mie, D_PSP_MIP_MTIP);
    /* register exception handlers - at the beginning, register 'pspTrapUnhandled' to all exceptions */
    for (cause = E_EXC_INSTRUCTION_ADDRESS_MISALIGNED ; cause < E_EXC_LAST ; cause++)
    {
    	/* Skip ECALL entry as we already registered there a handler */
    	if (E_EXC_ENVIRONMENT_CALL_FROM_MMODE == cause)
    	{
    		continue;
    	}
        pspRegisterExceptionHandler(pspTrapUnhandled, cause);
    }

    /* register external interrupt handler */
    /* pspRegisterInterruptHandler(handle_interrupt, E_MACHINE_EXTERNAL_CAUSE); */

    /* Enable the Machine-External bit in MIE */
    M_PSP_SET_CSR(mie, D_PSP_MIP_MEIP);

    /* Create the queue used by the send-msg and receive-msg tasks. */
    res = rtosalMsgQueueCreate(&stMsgQueue, cQueueBuffer, D_MAIN_QUEUE_LENGTH, sizeof(uint32_t), NULL);
    if (res != D_RTOSAL_SUCCESS)
    {
    	demoOutputMsg("Msg-Q creation failed\n", 22);
        for(;;);
    }

    /* Create the queue-receive task */
    res = rtosalTaskCreate(&stRxTask, (s08_t*)"RX", E_RTOSAL_PRIO_29,
    		      demoReceiveMsgTask, (u32_t)NULL, D_RX_TASK_STACK_SIZE, uRxTaskStackBuffer,
                  0, D_RTOSAL_AUTO_START, 0);
	if (res != D_RTOSAL_SUCCESS)
	{
		demoOutputMsg("Rx-Task creation failed\n", 24);
		for(;;);
	}

	/* Create the queue-send task in exactly the same way */
    res = rtosalTaskCreate(&stTxTask, (s08_t*)"TX", E_RTOSAL_PRIO_30,
    		      demoSendMsgTask, (u32_t)NULL, D_TX_TASK_STACK_SIZE,
				  uTxTaskStackBuffer, 0, D_RTOSAL_AUTO_START, 0);
	if (res != D_RTOSAL_SUCCESS)
	{
		demoOutputMsg("Tx-Task creation failed\n", 24);
		for(;;);
	}

	/* Create the semaphore used by the FreeRTOS tick hook function and the
    event semaphore task.  NOTE: A semaphore is used for example purposes,
    using a direct to task notification will be faster! */
    res = rtosalSemaphoreCreate(&stEventSemaphore, NULL, 0, 1);
	if (res != D_RTOSAL_SUCCESS)
	{
		demoOutputMsg("Semaphore creation failed\n", 26);
		for(;;);
	}

	/* Create the task that is synchronized with an interrupt using the
    stEventSemaphore semaphore. */
    res = rtosalTaskCreate(&stSemTask, (s08_t*)"SEM", E_RTOSAL_PRIO_29,
    		      demoSemaphoreTask, (u32_t)NULL, D_SEM_TASK_STACK_SIZE,
				  uSemTaskStackBuffer, 0, D_RTOSAL_AUTO_START, 0);
	if (res != D_RTOSAL_SUCCESS)
	{
		demoOutputMsg("Sem-Task creation failed\n", 25);
	    for(;;);
	}

    /* Create the software timer */
    res = rtosTimerCreate(&stLedTimer, (s08_t*)"LEDTimer", demoTimerCallback, 0,
                         D_RTOSAL_AUTO_START, D_MAIN_SOFTWARE_TIMER_PERIOD_MS, pdTRUE);
    if (res != D_RTOSAL_SUCCESS)
    {
    	demoOutputMsg("Timer creation failed\n", 22);
        for(;;);
    }
}


/**
 * demoTimerCallback - called each time the timer expires
 *
 * void* xTimer - not in use
 *
 */
static void demoTimerCallback(void* xTimer)
{
#ifdef D_RV_HI_FIVE1
    /* The timer has expired.  Count the number of times this happens.  The
    timer that calls this function is an auto re-load timer, so it will
    execute periodically. */
    ulCountOfTimerCallbackExecutions++;

    demoOutputLed(D_LED_BLUE_ON);
    demoOutputMsg("RTOS Timer Callback\n", 20);
#else
    M_PSP_NOP();
    M_PSP_NOP();
    M_PSP_NOP();
    M_PSP_NOP();
#endif
}

/**
 * demoSendMsgTask - Tx task function
 *
 * void *pvParameters - not in use
 *
 */
static void demoSendMsgTask( void *pvParameters )
{
const uint32_t ulValueToSend = 100UL;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    for( ;; )
    {
        /* Place this task in the blocked state until it is time to run again.
        The block time is specified in ticks, the constant used converts ticks
        to ms.  The task will not consume any CPU time while it is in the
        Blocked state. */
        rtosalTaskSleep(D_MAIN_QUEUE_SEND_PERIOD_MS);

        demoOutputMsg("Sending to queue\n", 17);
        /* Send to the queue - causing the queue receive task to unblock and
        increment its counter.  0 is used as the block time so the sending
        operation will not block - it shouldn't need to block as the queue
        should always be empty at this point in the code. */
        rtosalMsgQueueSend(&stMsgQueue, &ulValueToSend, 0, D_RTOSAL_FALSE);
    }
}

/**
 * demoReceiveMsgTask - Rx task function
 *
 * void *pvParameters - not in use
 *
 */
static void demoReceiveMsgTask( void *pvParameters )
{
    uint32_t ulReceivedValue;
    char stringValue[10];
    for( ;; )
    {

        /* Wait until something arrives in the queue - this task will block
        indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
        FreeRTOSConfig.h. */
        rtosalMsgQueueRecieve(&stMsgQueue, &ulReceivedValue, portMAX_DELAY);
        itoa(ulReceivedValue,stringValue, 10);
        demoOutputMsg("Recieved: ", 10);
		demoOutputMsg(stringValue, 3);
		demoOutputMsg("\n",1);

        /*  To get here something must have been received from the queue, but
        is it the expected value?  If it is, increment the counter. */
        if( ulReceivedValue == 100UL )
        {
            /* Count the number of items that have been received correctly. */
            ulCountOfItemsReceivedOnQueue++;
        }
    }
}

/**
 * demoSemaphoreTask - Semaphore task function
 *
 * void *pvParameters - not in use
 *
 */
static void demoSemaphoreTask( void *pvParameters )
{
   u32_t res;

    for( ;; )
    {
        /* Block until the semaphore is 'given'.  NOTE:
        A semaphore is used for example purposes.  In a real application it might
        be preferable to use a direct to task notification, which will be faster
        and use less RAM. */
        res = rtosalSemaphoreWait(&stEventSemaphore, portMAX_DELAY);
        if (res == D_RTOSAL_SUCCESS)
        {
           /* Count the number of times the semaphore is received. */
           ulCountOfReceivedSemaphores++;
        }

        demoOutputMsg("Semaphore taken\n", 16);
    }
}

/**
 * vApplicationTickHook - Called from FreeRTOS upon any timer's tick
 *
 */
void vApplicationTickHook( void )
{
static uint32_t ulCount = 0;

    /* The RTOS tick hook function is enabled by setting configUSE_TICK_HOOK to
    1 in FreeRTOSConfig.h.

    "Give" the semaphore on every 500th tick interrupt. */
    ulCount++;
    if( ulCount >= 500UL )
    {
    	/* This function is called from an interrupt context (the RTOS tick
        interrupt),    so only ISR safe API functions can be used (those that end
        in "FromISR()".


        xHigherPriorityTaskWoken was initialized to pdFALSE, and will be set to
        pdTRUE by xSemaphoreGiveFromISR() if giving the semaphore unblocked a
        task that has equal or higher priority than the interrupted task.
        NOTE: A semaphore is used for example purposes.  In a real application it
        might be preferable to use a direct to task notification,
        which will be faster and use less RAM. */

      rtosalSemaphoreRelease(&stEventSemaphore);
      /* the rtosalSemaphoreRelease will automatically handle the xHigherPriorityTaskWoken
       * indication and in this case even if xHigherPriorityTaskWoken is true, we don't
       * need to perform a context switch (we are in a context of the tick interrupt which
       * is already handling context switch if required therefore we must clear the
       * rtos al 'context switch' indication)
       */
      rtosalContextSwitchIndicationClear();
      ulCount = 0UL;

      demoOutputMsg("Giving Semaphore\n", 17);
      demoOutputLed(D_LED_GREEN_ON);
    }
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

