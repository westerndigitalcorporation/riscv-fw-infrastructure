/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/
#ifdef D_USE_RTOSAL
   #include "rtosal_types.h"
   #include "rtosal_semaphore_api.h"
   #include "rtosal_task_api.h"
   #include "rtosal_mutex_api.h"
   #include "rtosal_queue_api.h"
   #include "rtosal_time_api.h"
   #include "rtosal_util_api.h"
   #include "psp_api.h"
   #include "task.h"     /* for tskIDLE_PRIORITY */
#else
   #ifdef D_USE_FREERTOS
      /* Kernel includes. */
      #include "FreeRTOS.h" /* Must come first. */
      #include "task.h"     /* RTOS task related API prototypes. */
      #include "queue.h"    /* RTOS queue related API prototypes. */
      #include "timers.h"   /* Software timer related API prototypes. */
      #include "semphr.h"   /* Semaphore related API prototypes. */
  #else
     #error "Define RTOS appropriate includes"
  #endif /* D_USE_FREERTOS */
#endif /* D_USE_RTOSAL */
/* TODO Add any manufacture supplied header files can be included
here. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "platform.h"
#include "encoding.h"
#include "plic_driver.h"

/* Priorities at which the tasks are created.  The event semaphore task is
given the maximum priority of ( configMAX_PRIORITIES - 1 ) to ensure it runs as
soon as the semaphore is given. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY     ( tskIDLE_PRIORITY + 2 )
#define mainQUEUE_SEND_TASK_PRIORITY        ( tskIDLE_PRIORITY + 1 )
#define mainEVENT_SEMAPHORE_TASK_PRIORITY   ( configMAX_PRIORITIES - 1 )

/* The rate at which data is sent to the queue, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainQUEUE_SEND_PERIOD_MS            pdMS_TO_TICKS( 200 )

/* The period of the example software timer, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainSOFTWARE_TIMER_PERIOD_MS        pdMS_TO_TICKS( 1000 )

/* The number of items the queue can hold.  This is 1 as the receive task
has a higher priority than the send task, so will remove items as they are added,
meaning the send task should always find the queue empty. */
#define mainQUEUE_LENGTH                    ( 1 )

/*-----------------------------------------------------------*/

/*
 * TODO: Implement this function for any hardware specific clock configuration
 * that was not already performed before main() was called.
 */
static void prvSetupHardware( void );

/*
 * The queue send and receive tasks as described in the comments at the top of
 * this file.
 */
static void prvQueueReceiveTask( void *pvParameters );
static void prvQueueSendTask( void *pvParameters );

/*
 * The callback function assigned to the example software timer as described at
 * the top of this file.
 */
#ifndef D_USE_RTOSAL
static void vExampleTimerCallback( TimerHandle_t xTimer );
#else
static void vExampleTimerCallback( void* xTimer );
#endif /* D_USE_RTOSAL */
/*
 * The event semaphore task as described at the top of this file.
 */
static void prvEventSemaphoreTask( void *pvParameters );

/*-----------------------------------------------------------*/

/* The queue used by the queue send and queue receive tasks. */
#ifndef D_USE_RTOSAL
static QueueHandle_t xQueue = NULL;
#else
static rtosalMsgQueue_t xQueue;
#endif /* D_USE_RTOSAL */

/* The semaphore (in this case binary) that is used by the FreeRTOS tick hook
 * function and the event semaphore task.
 */
#ifndef D_USE_RTOSAL
static SemaphoreHandle_t xEventSemaphore = NULL;
#else
static rtosalSemaphore_t xEventSemaphore;
#endif /* D_USE_RTOSAL */

/* The counters used by the various examples.  The usage is described in the
 * comments at the top of this file.
 */
static volatile uint32_t ulCountOfTimerCallbackExecutions = 0;
static volatile uint32_t ulCountOfItemsReceivedOnQueue = 0;
static volatile uint32_t ulCountOfReceivedSemaphores = 0;

/*-----------------------------------------------------------*/

// Structures for registering different interrupt handlers
// for different parts of the application.
typedef void (*function_ptr_t) (void);
void no_interrupt_handler (void) {};
function_ptr_t g_ext_interrupt_handlers[PLIC_NUM_INTERRUPTS];

// Instance data for the PLIC.
plic_instance_t g_plic;

#ifdef D_USE_RTOSAL
#define D_RX_TASK_STACK_SIZE  600
#define D_TX_TASK_STACK_SIZE  configMINIMAL_STACK_SIZE
#define D_SEM_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define D_IDLE_TASK_SIZE      configMINIMAL_STACK_SIZE
static rtosalTask_t rxTask;
static rtosalTask_t txTask;
static rtosalTask_t semTask;
static rtosalTimer_t ledTimer;
static rtosalStackType_t rxTaskStackBuffer[D_RX_TASK_STACK_SIZE];
static rtosalStackType_t txTaskStackBuffer[D_TX_TASK_STACK_SIZE];
static rtosalStackType_t semTaskStackBuffer[D_SEM_TASK_STACK_SIZE];
static s08_t queueBuffer[mainQUEUE_LENGTH * sizeof(uint32_t)];
static rtosalTask_t xIdleTaskTCBBuffer;
static rtosalStackType_t xIdleStack[D_IDLE_TASK_SIZE];
static rtosalTask_t xTimerTaskTCBBuffer;
static rtosalStackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];
void handle_interrupt(void);
extern void pspMTimerIntHandler(void);
extern void pspEcallHandler(void);
extern void pspTrapUnhandled(void);
#endif /* D_USE_RTOSAL */

/* Workaround to issue that raised when we moved to GCC-8.3 version regarding association
 * of FW init function to the startup code created by libc.
 * We explicitly call our init function from the beginning of our demo_init function */
extern void _init();


void demo_init(void* pMem);

int main(void)
{
#ifndef D_USE_RTOSAL
   demo_init(NULL);
   /* Start the tasks and timer running. */
   vTaskStartScheduler();
#else
   rtosalStart(demo_init);
#endif /* D_USE_RTOSAL */

   /* If all is well, the scheduler will now be running, and the following line
   will never be reached.  If the following line does execute, then there was
   insufficient FreeRTOS heap memory available for the idle and/or timer tasks
   to be created.  See the memory management section on the FreeRTOS web site
   for more details.  */
   for( ;; );
}

void demo_init(void *pMem)
{
#ifndef D_USE_RTOSAL
	TimerHandle_t xExampleSoftwareTimer = NULL;
#else
	u32_t res;
	pspExceptionCause_t cause;
#endif /* D_USE_RTOSAL */

	/* Explicitly call init function from here */
	_init();

    /* Configure the system ready to run the demo.  The clock configuration
    can be done here if it was not done before main() was called. */
    prvSetupHardware();

// NatiR: what happens when we are not in D_USE_RTOSAL? where the ISRs are installed?
#ifdef D_USE_RTOSAL
    /* Disable the machine & timer interrupts until setup is done. */
    clear_csr(mie, MIP_MEIP);
    clear_csr(mie, MIP_MTIP);
    /* register exception handlers */
    for (cause = E_EXC_INSTRUCTION_ADDRESS_MISALIGNED ; cause < E_EXC_LAST ; cause++)
    {
        pspRegisterExceptionHandler(pspTrapUnhandled, cause);
    }
    /* register E_CALL exception handler */
    pspRegisterExceptionHandler(pspEcallHandler, E_EXC_ENVIRONMENT_CALL_FROM_MMODE);
    /* install timer interrupt handler */
    pspRegisterInterruptHandler(pspMTimerIntHandler, E_MACHINE_TIMER_CAUSE);
    /* install external interrupt handler */
    pspRegisterInterruptHandler(handle_interrupt, E_MACHINE_EXTERNAL_CAUSE);
    /* Enable the Machine-External bit in MIE */
    set_csr(mie, MIP_MEIP); // NatiR: why not re-enable MIP_MTIP too?
#endif /* D_USE_RTOSAL */

    /* Create the queue used by the queue send and queue receive tasks. */
#ifndef D_USE_RTOSAL
    xQueue = xQueueCreate(     /* The number of items the queue can hold. */
                            mainQUEUE_LENGTH,
                            /* The size of each item the queue holds. */
                            sizeof( uint32_t ) );

    if(xQueue == NULL)	{
    	for(;;);
    }
#else
    res = rtosalMsgQueueCreate(&xQueue, queueBuffer, mainQUEUE_LENGTH, sizeof(uint32_t), NULL);
    if (res != D_RTOSAL_SUCCESS)
    {
        for(;;);
    }
#endif /* D_USE_RTOSAL */

    /* Create the queue receive task as described in the comments at the top
    of this file. */
#ifndef D_USE_RTOSAL
    xTaskCreate(     /* The function that implements the task. */
                    prvQueueReceiveTask,
                    /* Text name for the task, just to help debugging. */
                    ( const char * ) "Rx",
                    /* The size (in words) of the stack that should be created
                    for the task. */
					600, /* printf requires a much deeper stack*/
                    /* A parameter that can be passed into the task.  Not used
                    in this simple demo. */
                    NULL,
                    /* The priority to assign to the task.  tskIDLE_PRIORITY
                    (which is 0) is the lowest priority.  configMAX_PRIORITIES - 1
                    is the highest priority. */
                    mainQUEUE_RECEIVE_TASK_PRIORITY,
                    /* Used to obtain a handle to the created task.  Not used in
                    this simple demo, so set to NULL. */
                    NULL );
#else
    res = rtosalTaskCreate(&rxTask, (s08_t*)"RX", E_RTOSAL_PRIO_29,
    		      prvQueueReceiveTask, (u32_t)NULL, D_RX_TASK_STACK_SIZE, rxTaskStackBuffer,
                  0, D_RTOSAL_AUTO_START, 0);
	if (res != D_RTOSAL_SUCCESS)
	{
		for(;;);
	}
#endif /* D_USE_RTOSAL */

	/* Create the queue send task in exactly the same way.  Again, this is
    described in the comments at the top of the file. */
#ifndef D_USE_RTOSAL
    xTaskCreate(     prvQueueSendTask,
                    ( const char * ) "TX",
					configMINIMAL_STACK_SIZE,
                    NULL,
                    mainQUEUE_SEND_TASK_PRIORITY,
                    NULL );
#else
    res = rtosalTaskCreate(&txTask, (s08_t*)"TX", E_RTOSAL_PRIO_30,
    		      prvQueueSendTask, (u32_t)NULL, D_TX_TASK_STACK_SIZE,
				  txTaskStackBuffer, 0, D_RTOSAL_AUTO_START, 0);
	if (res != D_RTOSAL_SUCCESS)
	{
		for(;;);
	}
#endif /* D_USE_RTOSAL */

	/* Create the semaphore used by the FreeRTOS tick hook function and the
    event semaphore task.  NOTE: A semaphore is used for example purposes,
    using a direct to task notification will be faster! */
#ifndef D_USE_RTOSAL
    xEventSemaphore = xSemaphoreCreateBinary();
    if(xEventSemaphore == NULL)	{
    	for(;;);
    }
#else
    res = rtosalSemaphoreCreate(&xEventSemaphore, NULL, 0, 1);
	if (res != D_RTOSAL_SUCCESS)
	{
		for(;;);
	}
#endif /* D_USE_RTOSAL */

	/* Create the task that is synchronized with an interrupt using the
    xEventSemaphore semaphore. */
#ifndef D_USE_RTOSAL
    xTaskCreate(     prvEventSemaphoreTask,
                    ( const char * ) "Sem",
					configMINIMAL_STACK_SIZE,
                    NULL,
                    mainEVENT_SEMAPHORE_TASK_PRIORITY,
                    NULL );
#else
    res = rtosalTaskCreate(&semTask, (s08_t*)"SEM", E_RTOSAL_PRIO_29,
    		      prvEventSemaphoreTask, (u32_t)NULL, D_SEM_TASK_STACK_SIZE,
				  semTaskStackBuffer, 0, D_RTOSAL_AUTO_START, 0);
	if (res != D_RTOSAL_SUCCESS)
	{
		for(;;);
	}
#endif /* D_USE_RTOSAL */

    /* Create the software timer as described in the comments at the top of
    this file. */
#ifndef D_USE_RTOSAL
    xExampleSoftwareTimer = xTimerCreate(     /* A text name, purely to help
                                            debugging. */
                                            ( const char * ) "LEDTimer",
                                            /* The timer period, in this case
                                            1000ms (1s). */
                                            mainSOFTWARE_TIMER_PERIOD_MS,
                                            /* This is a periodic timer, so
                                            xAutoReload is set to pdTRUE. */
                                            pdTRUE,
                                            /* The ID is not used, so can be set
                                            to anything. */
                                            ( void * ) 0,
                                            /* The callback function that switches
                                            the LED off. */
                                            vExampleTimerCallback
                                        );
    /* Start the created timer.  A block time of zero is used as the timer
    command queue cannot possibly be full here (this is the first timer to
    be created, and it is not yet running). */
    xTimerStart( xExampleSoftwareTimer, 0 );
#else
    res = rtosTimerCreate(&ledTimer, (s08_t*)"LEDTimer", vExampleTimerCallback, 0,
                         D_RTOSAL_AUTO_START, mainSOFTWARE_TIMER_PERIOD_MS, pdTRUE);
    if (res != D_RTOSAL_SUCCESS)
    {
       for(;;);
    }
#endif /* D_USE_RTOSAL */
}
/*-----------------------------------------------------------*/

#ifndef D_USE_RTOSAL
static void vExampleTimerCallback( TimerHandle_t xTimer )
#else
static void vExampleTimerCallback(void* xTimer)
#endif /* D_USE_RTOSAL */
{
    /* The timer has expired.  Count the number of times this happens.  The
    timer that calls this function is an auto re-load timer, so it will
    execute periodically. */
    ulCountOfTimerCallbackExecutions++;

    GPIO_REG(GPIO_OUTPUT_VAL)  ^=   (0x1 << BLUE_LED_OFFSET) ;
    write(1,"RTOS Timer Callback\n", 20);

}
/*-----------------------------------------------------------*/

static void prvQueueSendTask( void *pvParameters )
{
#ifndef D_USE_RTOSAL
TickType_t xNextWakeTime;
#endif /* D_USE_RTOSAL */
const uint32_t ulValueToSend = 100UL;

    /* Initialise xNextWakeTime - this only needs to be done once. */
#ifndef D_USE_RTOSAL
    xNextWakeTime = xTaskGetTickCount();
#endif /* D_USE_RTOSAL */

    for( ;; )
    {
        /* Place this task in the blocked state until it is time to run again.
        The block time is specified in ticks, the constant used converts ticks
        to ms.  The task will not consume any CPU time while it is in the
        Blocked state. */
#ifndef D_USE_RTOSAL
       vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_PERIOD_MS );
#else
       rtosalTaskSleep(mainQUEUE_SEND_PERIOD_MS);
#endif /* D_USE_RTOSAL */

        write(1, "Sending to queue\n", 17);
        /* Send to the queue - causing the queue receive task to unblock and
        increment its counter.  0 is used as the block time so the sending
        operation will not block - it shouldn't need to block as the queue
        should always be empty at this point in the code. */
#ifndef D_USE_RTOSAL
        xQueueSend( xQueue, &ulValueToSend, 0 );
#else
        rtosalMsgQueueSend(&xQueue, &ulValueToSend, 0, D_RTOSAL_FALSE);
#endif /* D_USE_RTOSAL */
    }
}

/*-----------------------------------------------------------*/

static void prvQueueReceiveTask( void *pvParameters )
{
    uint32_t ulReceivedValue;
    char stringValue[10];
    for( ;; )
    {

        /* Wait until something arrives in the queue - this task will block
        indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
        FreeRTOSConfig.h. */
#ifndef D_USE_RTOSAL
        xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );
#else
        rtosalMsgQueueRecieve(&xQueue, &ulReceivedValue, portMAX_DELAY);
#endif /* D_USE_RTOSAL */
        itoa(ulReceivedValue,stringValue, 10);
        write(1,"Recieved: ", 10);
        write(1,stringValue, 3);
        write(1,"\n",1);

        /*  To get here something must have been received from the queue, but
        is it the expected value?  If it is, increment the counter. */
        if( ulReceivedValue == 100UL )
        {
            /* Count the number of items that have been received correctly. */
            ulCountOfItemsReceivedOnQueue++;
        }
    }
}

/*-----------------------------------------------------------*/

static void prvEventSemaphoreTask( void *pvParameters )
{
#ifdef D_USE_RTOSAL
   u32_t res;
#endif /* D_USE_RTOSAL */

    for( ;; )
    {
        /* Block until the semaphore is 'given'.  NOTE:
        A semaphore is used for example purposes.  In a real application it might
        be preferable to use a direct to task notification, which will be faster
        and use less RAM. */
#ifndef D_USE_RTOSAL
        xSemaphoreTake( xEventSemaphore, portMAX_DELAY );
        /* Count the number of times the semaphore is received. */
        ulCountOfReceivedSemaphores++;
#else
        res = rtosalSemaphoreWait(&xEventSemaphore, portMAX_DELAY);
        if (res == D_RTOSAL_SUCCESS)
        {
           /* Count the number of times the semaphore is received. */
           ulCountOfReceivedSemaphores++;
        }
#endif /* D_USE_RTOSAL */

        write(1, "Semaphore taken\n", 16);
    }
}

/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
#ifndef D_USE_RTOSAL
BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#endif /* #if defined(D_USE_RTOSAL) */
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
#ifndef D_USE_RTOSAL
      xSemaphoreGiveFromISR( xEventSemaphore, &xHigherPriorityTaskWoken );
#else
      rtosalSemaphoreRelease(&xEventSemaphore);
      /* the rtosalSemaphoreRelease will automatically handle the xHigherPriorityTaskWoken
       * indication and in this case even if xHigherPriorityTaskWoken is true, we don't
       * need to perform a context switch (we are in a context of the tick interrupt which
       * is already handling context switch if required therefore we must clear the
       * rtos al 'context switch' indication)
       */
      rtosalContextSwitchIndicationClear();
#endif
      ulCount = 0UL;

    	GPIO_REG(GPIO_OUTPUT_VAL)  ^=   (0x1 << GREEN_LED_OFFSET) ;
    	write(1, "Giving Semaphore\n", 17);

    }

    /* If xHigherPriorityTaskWoken is pdTRUE then a context switch should
    normally be performed before leaving the interrupt (because during the
    execution of the interrupt a task of equal or higher priority than the
    running task was unblocked).  The syntax required to context switch from
    an interrupt is port dependent, so check the documentation of the port you
    are using.

    In this case, the function is running in the context of the tick interrupt,
    which will automatically check for the higher priority task to run anyway,
    so no further action is required. */
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
    /* The malloc failed hook is enabled by setting
    configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

    Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues, software
    timers, and semaphores.  The size of the FreeRTOS heap is set by the
    configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	write(1,"malloc failed\n", 14);
    for( ;; );
}
/*-----------------------------------------------------------*/
#ifndef D_USE_RTOSAL
void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName )
#else
void vApplicationStackOverflowHook(void* xTask, signed char *pcTaskName)
#endif /* #ifndef D_USE_RTOSAL */
{
    ( void ) pcTaskName;
    ( void ) xTask;

    /* Run time stack overflow checking is performed if
    configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected.  pxCurrentTCB can be
    inspected in the debugger if the task name passed into this function is
    corrupt. */
    write(1, "Stack Overflow\n", 15);
    for( ;; );
}
/*-----------------------------------------------------------*/
void vApplicationIdleHook( void )
{
#ifndef D_USE_RTOSAL
volatile size_t xFreeStackSpace;

    /* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
    FreeRTOSConfig.h.

    This function is called on each cycle of the idle task.  In this case it
    does nothing useful, other than report the amount of FreeRTOS heap that
    remains unallocated. */
    xFreeStackSpace = xPortGetFreeHeapSize();
    if( xFreeStackSpace > 100 )
    {
        /* By now, the kernel has allocated everything it is going to, so
        if there is a lot of heap remaining unallocated then
        the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
        reduced accordingly. */
    }
#endif /* USE_FREERTOS */

}

/*ISR triggered by connecting Wake and GPIO pin 2 then pressing
the wake button */
void wake_ISR( )    {
    const uint32_t ulValueToSend = 555UL;
    
    GPIO_REG(GPIO_OUTPUT_VAL)  ^=   (0x1 << RED_LED_OFFSET) ;
#ifndef D_USE_RTOSAL
    xQueueSendFromISR( xQueue, &ulValueToSend, 0 );
#else
    rtosalMsgQueueSend(&xQueue, &ulValueToSend, 0, D_RTOSAL_FALSE);
#endif
    write(1,"---------->\n",13);
    //clear irq - interrupt pending register is write 1 to clear
    GPIO_REG(GPIO_FALL_IP) |= (1<<PIN_2_OFFSET);
}
/*-----------------------------------------------------------*/
#ifndef D_USE_RTOSAL
/*Entry Point for Interrupt Handler*/
void handle_interrupt(unsigned long mcause){
// NatiR: I need to understand the different behavior between D_USE_RTOSAL yes/no
// NatiR: It seems that there is no real ISR installed for any external interrupt. Am I wrong?
  /* check if global*/
  if(((mcause & MCAUSE_CAUSE) == IRQ_M_EXT))
  {
    plic_source int_num  = PLIC_claim_interrupt(&g_plic);
    g_ext_interrupt_handlers[int_num]();
    PLIC_complete_interrupt(&g_plic, int_num);
  }

}
#else
void handle_interrupt(void)
{
    plic_source int_num  = PLIC_claim_interrupt(&g_plic);
    g_ext_interrupt_handlers[int_num]();
    PLIC_complete_interrupt(&g_plic, int_num);
}
#endif /* #ifndef D_USE_RTOSAL */

/*-----------------------------------------------------------*/
//enables interrupt and assigns handler
void enable_interrupt(uint32_t int_num, uint32_t int_priority, function_ptr_t handler) {
    g_ext_interrupt_handlers[int_num] = handler;
    PLIC_set_priority(&g_plic, int_num, int_priority);
    PLIC_enable_interrupt (&g_plic, int_num);
}
/*-----------------------------------------------------------*/

/*
 *enables the plic and programs handlers
**/
void interrupts_init(  ) {

    // Disable the machine & timer interrupts until setup is done.
    clear_csr(mie, MIP_MEIP);
    clear_csr(mie, MIP_MTIP); //NatiR - why you disable Timer int here? and why you don't re-enable it at the end of this function?


  //setup PLIC
  PLIC_init(&g_plic,
	    PLIC_CTRL_ADDR,
	    PLIC_NUM_INTERRUPTS,
	    PLIC_NUM_PRIORITIES);

  //assign interrupts to defaul handler
  for (int ii = 0; ii < PLIC_NUM_INTERRUPTS; ii ++){
    g_ext_interrupt_handlers[ii] = no_interrupt_handler;
  }

    // Enable the Machine-External bit in MIE
    set_csr(mie, MIP_MEIP);
}
/*-----------------------------------------------------------*/

void led_init()  {
    GPIO_REG(GPIO_INPUT_EN)    &= ~((0x1<< RED_LED_OFFSET) | (0x1<< GREEN_LED_OFFSET) | (0x1 << BLUE_LED_OFFSET)) ;
    GPIO_REG(GPIO_OUTPUT_EN)   |=  ((0x1<< RED_LED_OFFSET)| (0x1<< GREEN_LED_OFFSET) | (0x1 << BLUE_LED_OFFSET)) ;
    GPIO_REG(GPIO_OUTPUT_VAL)  &= ~((0x1<< RED_LED_OFFSET) | (0x1<< GREEN_LED_OFFSET) | (0x1 << BLUE_LED_OFFSET)) ;

}
/*-----------------------------------------------------------*/

/*
**configures the wake button for irq trigger
**requires that the wake pin is connected to pin2
*/
void wake_irq_init()  {

    //dissable hw io function
    GPIO_REG(GPIO_IOF_EN )    &=  ~(1 << PIN_2_OFFSET);

    //set to input
    GPIO_REG(GPIO_INPUT_EN)   |= (1<<PIN_2_OFFSET);
    GPIO_REG(GPIO_PULLUP_EN)  |= (1<<PIN_2_OFFSET);

    //set to interrupt on falling edge
    GPIO_REG(GPIO_FALL_IE)    |= (1<<PIN_2_OFFSET);

    enable_interrupt(INT_GPIO_BASE+PIN_2_OFFSET, 2, &wake_ISR);
}
/*-----------------------------------------------------------*/


static void prvSetupHardware( void )
{
    interrupts_init();
    led_init();
    wake_irq_init();
}
/*-----------------------------------------------------------*/

#ifdef D_USE_RTOSAL
void vApplicationGetIdleTaskMemory(rtosalStaticTask_t **ppxIdleTaskTCBBuffer, rtosalStack_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
  *ppxIdleTaskTCBBuffer = (rtosalStaticTask_t*)&xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = (rtosalStack_t*)&xIdleStack[0];
  *pulIdleTaskStackSize = D_IDLE_TASK_SIZE;
}

/* If static allocation is supported then the application must provide the
   following callback function - which enables the application to optionally
   provide the memory that will be used by the timer task as the task's stack
   and TCB. */
void vApplicationGetTimerTaskMemory(rtosalStaticTask_t **ppxTimerTaskTCBBuffer, rtosalStack_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
{
  *ppxTimerTaskTCBBuffer = (rtosalStaticTask_t*)&xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = (rtosalStack_t*)&xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
#endif /* #ifdef D_USE_RTOSAL */

