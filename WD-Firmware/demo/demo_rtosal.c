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

/******************************************************************************/

/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */
/***********************************************************************************/



/**
* @file   demo_rtosal_demo.c
* @author Nati Rapaport
* @date   23.10.2019
* @brief  application of RTOS abstraction layer basic demonstration
*/

/**
* include files
*/
#ifdef D_HI_FIVE1
    #include <stdlib.h>	
#endif
#include "common_types.h"
#include "demo_platform_al.h"
#include "rtosal_task_api.h"
#include "rtosal_semaphore_api.h"
#include "rtosal_task_api.h"
#include "rtosal_queue_api.h"
#include "rtosal_time_api.h"


/**
* definitions
*/

/* The rate the data is sent to the queue, specified in milliseconds, and
converted to ticks (using the definition of D_TICK_TIME_MS) */
#define D_MAIN_QUEUE_SEND_PERIOD_TICKS            200/D_TICK_TIME_MS

/* The period of the example software timer, specified in milliseconds, and
converted to ticks (using the definition of D_TICK_TIME_MS) */
#define D_MAIN_SOFTWARE_TIMER_PERIOD_TICKS        1000/D_TICK_TIME_MS

/* The number of items the queue can hold.  This is 1 as the receive task
has a higher priority than the send task, so will remove items as they are added,
meaning the send task should always find the queue empty. */
#define D_MAIN_QUEUE_LENGTH                    ( 1 )

/* Stack size of the tasks in this application */
#define D_RX_TASK_STACK_SIZE  600
#define D_TX_TASK_STACK_SIZE  450
#define D_SEM_TASK_STACK_SIZE 450

/**
* macros
*/

/**
* types
*/

/**
* local prototypes
*/
static void demoRtosalCreateTasks(void *pParameters);
static void demoRtosalReceiveMsgTask(void *pParameters);
static void demoRtosalSendMsgTask(void *pParameters);
static void demoRtosalSemaphoreTask(void *pParameters);
static void demoRtosalTimerCallback(void *pTimer);
static void demoRtosalcalculateTimerPeriod(void);
void demoRtosalTimerTickHandler(void);

/**
* external prototypes
*/
extern void pspExternalIntHandlerIsr(void);

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
static volatile u32_t uiCountOfTimerCallbackExecutions = 0;
static volatile u32_t uiCountOfItemsReceivedOnQueue = 0;
static volatile u32_t uiCountOfReceivedSemaphores = 0;

/* Variables related to the tasks in this application */
static rtosalTask_t stRxTask;
static rtosalTask_t stTxTask;
static rtosalTask_t stSemTask;
static rtosalTimer_t stLedTimer;
static rtosalStackType_t uiRxTaskStackBuffer[D_RX_TASK_STACK_SIZE];
static rtosalStackType_t uiTxTaskStackBuffer[D_TX_TASK_STACK_SIZE];
static rtosalStackType_t uiSemTaskStackBuffer[D_SEM_TASK_STACK_SIZE];
static s08_t cQueueBuffer[D_MAIN_QUEUE_LENGTH * sizeof(u32_t)];


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
void demoRtosalCreateTasks(void *pParameters)
{
	
    u32_t uiResult;
    ePspExceptionCause_t eCause;

    /* Disable the machine external & timer interrupts until setup is done. */
    pspDisableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_EXT);
	pspDisableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_TIMER);

	/* register exception handlers - at the beginning, register 'pspTrapUnhandled' to all exceptions */
    for (eCause = E_EXC_INSTRUCTION_ADDRESS_MISALIGNED ; eCause < E_EXC_LAST_COMMON ; eCause++)
    {
    	/* Skip ECALL entry as we already registered there a handler */
    	if (E_EXC_ENVIRONMENT_CALL_FROM_MMODE == eCause)
    	{
    		continue;
    	}
        pspRegisterExceptionHandler(pspTrapUnhandled, eCause);
    }
    /*TODO [AD]: Add external interrupts handlers array registration to meivt CSR */

    /* register external interrupt handler */
    pspRegisterInterruptHandler(pspExternalIntHandlerIsr, E_MACHINE_EXTERNAL_CAUSE);

    /* Enable the Machine-External interrupt */
    pspEnableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_EXT);

    /* Create the queue used by the send-msg and receive-msg tasks. */
    uiResult = rtosalMsgQueueCreate(&stMsgQueue, cQueueBuffer, D_MAIN_QUEUE_LENGTH, sizeof(u32_t), NULL);
    if (uiResult != D_RTOSAL_SUCCESS)
    {
    	demoOutputMsg("Msg-Q creation failed\n", 22);
        for(;;);
    }

    /* Create the queue-receive task */
    uiResult = rtosalTaskCreate(&stRxTask, (s08_t*)"RX", E_RTOSAL_PRIO_29,
    		      demoRtosalReceiveMsgTask, (u32_t)NULL, D_RX_TASK_STACK_SIZE, uiRxTaskStackBuffer,
                  0, D_RTOSAL_AUTO_START, 0);
	if (uiResult != D_RTOSAL_SUCCESS)
	{
		demoOutputMsg("Rx-Task creation failed\n", 24);
		for(;;);
	}

	/* Create the queue-send task in exactly the same way */
    uiResult = rtosalTaskCreate(&stTxTask, (s08_t*)"TX", E_RTOSAL_PRIO_30,
    		      demoRtosalSendMsgTask, (u32_t)NULL, D_TX_TASK_STACK_SIZE,
				  uiTxTaskStackBuffer, 0, D_RTOSAL_AUTO_START, 0);
	if (uiResult != D_RTOSAL_SUCCESS)
	{
		demoOutputMsg("Tx-Task creation failed\n", 24);
		for(;;);
	}

	/* Create the semaphore used by the FreeRTOS tick hook function and the
    event semaphore task.  NOTE: A semaphore is used for example purposes,
    using a direct to task notification will be faster! */
    uiResult = rtosalSemaphoreCreate(&stEventSemaphore, NULL, 0, 1);
	if (uiResult != D_RTOSAL_SUCCESS)
	{
		demoOutputMsg("Semaphore creation failed\n", 26);
		for(;;);
	}

	/* Create the task that is synchronized with an interrupt using the
    stEventSemaphore semaphore. */
    uiResult = rtosalTaskCreate(&stSemTask, (s08_t*)"SEM", E_RTOSAL_PRIO_29,
    		      demoRtosalSemaphoreTask, (u32_t)NULL, D_SEM_TASK_STACK_SIZE,
				  uiSemTaskStackBuffer, 0, D_RTOSAL_AUTO_START, 0);
	if (uiResult != D_RTOSAL_SUCCESS)
	{
		demoOutputMsg("Sem-Task creation failed\n", 25);
	    for(;;);
	}

    /* Create the software timer */
    uiResult = rtosTimerCreate(&stLedTimer, (s08_t*)"LEDTimer", demoRtosalTimerCallback, 0,
                         D_RTOSAL_AUTO_START, D_MAIN_SOFTWARE_TIMER_PERIOD_TICKS, pdTRUE);
    if (uiResult != D_RTOSAL_SUCCESS)
    {
    	demoOutputMsg("Timer creation failed\n", 22);
        for(;;);
    }

    /* Register Timer-Tick interrupt handler function */
    rtosalRegisterTimerTickHandler(demoRtosalTimerTickHandler);

    /* Calculates timer period */
    demoRtosalcalculateTimerPeriod();

}


/**
 * demoRtosalTimerTickHandler - called upon Timer-Tick interrupt. This module implements the handler and register
 *                              it in RTOS-AL so it will be called upon each tick-interrupt
 *
 */
void demoRtosalTimerTickHandler(void)
{
    static u32_t uiCount = 0;

    /* The RTOS tick hook function is enabled by setting configUSE_TICK_HOOK to
    1 in FreeRTOSConfig.h.

    "Give" the semaphore on every 500th tick interrupt. */
    uiCount++;
    if( uiCount >= 500UL )
    {
         /* This function is called from an interrupt context (the RTOS tick interrupt),
          * so only ISR safe API functions can be used (those that end in "FromISR()".

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
         uiCount = 0UL;

         demoOutputMsg("Giving Semaphore\n", 17);
         demoOutputToggelLed();
    }
}

/**
 * demoRtosalTimerCallback - called each time the timer expires
 *
 * void* xTimer - not in use
 *
 */
static void demoRtosalTimerCallback(void* pTimer)
{
    /* The timer has expired.  Count the number of times this happens.  The
    timer that calls this function is an auto re-load timer, so it will
    execute periodically. */
    uiCountOfTimerCallbackExecutions++;

    demoOutputToggelLed();
#ifdef D_HI_FIVE1
    demoOutputMsg("RTOS Timer Callback\n", 20);
#elif defined(D_NEXYS_A7)
    demoOutputMsg("RTOS Timer Callback\n");
#else
    /* Developer: please add here implementation that fits your environment */
    M_PSP_NOP();
    M_PSP_NOP();
    M_PSP_NOP();
    M_PSP_NOP();
#endif
}

/**
 * demoRtosalSendMsgTask - Tx task function
 *
 * void *pvParameters - not in use
 *
 */
static void demoRtosalSendMsgTask( void *pParameters )
{
    const u32_t uiValueToSend = 100UL;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    for( ;; )
    {
        /* Place this task in the blocked state until it is time to run again.
        The block time is specified in ticks.
        The task will not consume any CPU time while it is in the Blocked state. */
        rtosalTaskSleep(D_MAIN_QUEUE_SEND_PERIOD_TICKS);

        demoOutputMsg("Sending to queue\n", 17);
        /* Send to the queue - causing the queue receive task to unblock and
        increment its counter.  0 is used as the block time so the sending
        operation will not block - it shouldn't need to block as the queue
        should always be empty at this point in the code. */
        rtosalMsgQueueSend(&stMsgQueue, &uiValueToSend, 0, D_RTOSAL_FALSE);
    }
}

/**
 * demoRtosalReceiveMsgTask - Rx task function
 *
 * void *pvParameters - not in use
 *
 */
static void demoRtosalReceiveMsgTask( void *pParameters )
{
	u32_t uiReceivedValue;

    for( ;; )
    {
        rtosalMsgQueueRecieve(&stMsgQueue, &uiReceivedValue, portMAX_DELAY);
        /* Wait until something arrives in the queue - this task will block
        indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
        FreeRTOSConfig.h. */
#ifdef D_HI_FIVE1
        char cStringValue[10];
        itoa(uiReceivedValue,cStringValue, 10);
        demoOutputMsg("Received: ", 10);
        demoOutputMsg(cStringValue, 3);
        demoOutputMsg("\n",1);
#else
        demoOutputMsg("Received %d: ",uiReceivedValue);
#endif

        /*  To get here something must have been received from the queue, but
        is it the expected value?  If it is, increment the counter. */
        if( uiReceivedValue == 100UL )
        {
            /* Count the number of items that have been received correctly. */
            uiCountOfItemsReceivedOnQueue++;
        }
    }
}

/**
 * demoRtosalSemaphoreTask - Semaphore task function
 *
 * void *pvParameters - not in use
 *
 */
static void demoRtosalSemaphoreTask( void *pParameters )
{
   u32_t uiResult;

    for( ;; )
    {
        /* Block until the semaphore is 'given'.  NOTE:
        A semaphore is used for example purposes.  In a real application it might
        be preferable to use a direct to task notification, which will be faster
        and use less RAM. */
        uiResult = rtosalSemaphoreWait(&stEventSemaphore, portMAX_DELAY);
        if (uiResult == D_RTOSAL_SUCCESS)
        {
           /* Count the number of times the semaphore is received. */
           uiCountOfReceivedSemaphores++;
        }

        demoOutputMsg("Semaphore taken\n", 16);
    }
}


/**
 * demoRtosalcalculateTimerPeriod - Calculates Timer period
 *
 */
void demoRtosalcalculateTimerPeriod(void)
{
	u32_t uiTimerPeriod = 0;

    #if (0 == D_CLOCK_RATE) || (0 == D_TICK_TIME_MS)
        #error "Core frequency values definitions are missing"
    #endif

	uiTimerPeriod = (D_CLOCK_RATE * D_TICK_TIME_MS / D_PSP_MSEC);
	/* Store calculated timerPeriod for future use */
	rtosalTimerSetPeriod(uiTimerPeriod);
}

