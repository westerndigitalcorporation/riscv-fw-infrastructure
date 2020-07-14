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
* @file   rtosal_task.c
* @author Ronen Haen
* @date   21.01.2019 
* @brief  The file implements the RTOS AL task API
* 
*/

/**
* include files
*/
#include "rtosal_task_api.h"
#include "rtosal_macros.h"
#include "rtosal_util.h"
#include "rtosal_interrupt_api.h"
#include "psp_api.h"
#ifdef D_USE_FREERTOS
   #include "task.h"
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

#ifdef D_COMRV
   #include "comrv_api.h"
#endif /* D_COMRV */

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
extern void rtosal_vect_table(void);
extern void rtosalHandleEcall(void);
extern void rtosalTimerIntHandler(void);

/**
* global variables
*/
/* function pointer Application initialization. The function should be implemented by the application */
rtosalApplicationInit_t  fptrAppInit = NULL;

/* application specific timer-tick handler function. The function should be implemented by the application  */
rtosalTimerTickHandler_t fptrTimerTickHandler ;

/**
* Task creation function
*
* @param  pRtosalTaskCb             - Pointer to the task control block to be created
* @param  pTaskName                 - String of the Task name (for debuging)
* @param  uiPriority                - Task priority E_RTOSAL_PRIO_0 (highest) to E_RTOSAL_PRIO_31 (lowest)
* @param  fptrRtosalTaskEntryPoint  - Task function handler
* @param  uiTaskEntryPointParameter - Task function handler input parameter
* @param  uiStackSize               - Task stack size in bytes
* @param  pStackBuffer              - Pointer to the stack buffer
* @param  uiTimeSliceTicks          - Ticks widnow time for time slice usage. Use D_RTOSAL_NO_TIME_SLICE 
*                                     for no time slicing 
* @param  uiAutoStart               - D_RTOSAL_AUTO_START: start run
*                                     D_RTOSAL_DONT_START: task will be suspend. rtosalTaskResume() 
*                    must be invoked to run the task
* @param  uiPreemptThuiReshold      - Priority threshold, any priority which is higher then uiPreemptThuiReshold
*                                     will cause preemprion to this task. (value must be below or equal to uiPriority value)
*                                     If uiPreemptThuiReshold = uiPriority then this task will never be preempted  
*
* @return u32_t                    - D_RTOSAL_SUCCESS
*                                  - D_RTOSAL_TASK_ERROR - The ptr, cTaskCB, in the pRtosalTaskCb is invalid
*                                  - D_RTOSAL_PTR_ERROR - Invalid fptrRtosalTaskEntryPoint or pStackBuffer
*                                  - D_RTOSAL_SIZE_ERROR - Invalid uiStackSize (must be at lest D_RTOSAL_MIN_STACK_SIZE) 
*                                  - D_RTOSAL_PRIORITY_ERROR - Invalid uiPriority (must be E_RTOSAL_PRIO_0 to E_RTOSAL_PRIO_MAX)
*                                  - D_RTOSAL_THRESH_ERROR - Invalid uiPreemptThuiReshold. uiPreemptThuiReshold <= uiPriority
*                                  - D_RTOSAL_START_ERROR - Invalid uiAutoStart
*                                  - D_RTOSAL_CALLER_ERROR - The caller can not call this function
*/
RTOSAL_SECTION u32_t rtosalTaskCreate(rtosalTask_t* pRtosalTaskCb, const s08_t* pTaskName, rtosalPriority_t uiPriority,
                                      rtosalTaskHandler_t fptrRtosTaskEntryPoint, u32_t uiTaskEntryPointParameter,
                                      u32_t uiStackSize, void * pStackBuffer, u32_t uiTimeSliceTicks,
                                      u32_t uiAutoStart, u32_t uiPreemptThuiReshold)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalTaskCb, pRtosalTaskCb == NULL, D_RTOSAL_TASK_ERROR);
   M_RTOSAL_VALIDATE_FUNC_PARAM(uiPriority, uiPriority > D_MAX_PRIORITY, D_RTOSAL_PRIORITY_ERROR);

#ifdef D_USE_FREERTOS
   M_RTOSAL_VALIDATE_FUNC_PARAM(pStackBuffer, pStackBuffer == NULL, D_RTOSAL_PTR_ERROR);
   pRtosalTaskCb->taskHandle = xTaskCreateStatic(fptrRtosTaskEntryPoint, (const char * const) pTaskName,
                                  uiStackSize, (void*)uiTaskEntryPointParameter,
                                  (UBaseType_t)uiPriority, (StackType_t*)pStackBuffer,
                                  (StaticTask_t*)pRtosalTaskCb->cTaskCB);
   /* we need to suspend the task if required */
   if (pRtosalTaskCb->taskHandle != NULL)
   {
      /* we need to suspend the task if required */
      if (uiAutoStart == D_RTOSAL_DONT_START)
      {
         vTaskSuspend(pRtosalTaskCb->taskHandle);
      }
      uiRes = D_RTOSAL_SUCCESS;
   }
   else
   {
      uiRes = D_RTOSAL_TASK_ERROR;
   }
#elif D_USE_THREADX
   #error "Add THREADX appropriate definitions"
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Delete a task
*
* @param  pRtosalTaskCb - pointer to the task control block to be destroyed
*
* @return u32_t      - D_RTOSAL_SUCCESS
*                    - D_RTOSAL_TASK_ERROR   - The ptr, cTaskCB, in the pRtosalTaskCb is invalid
*                    - D_RTOSAL_DELETE_ERROR - The caller is a task that try to delete itself or the 
*                                              the task is not idle
*                    - D_RTOSAL_CALLER_ERROR - The caller can not call this function
*/
RTOSAL_SECTION u32_t rtosalTaskDestroy(rtosalTask_t* pRtosalTaskCb)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalTaskCb, pRtosalTaskCb == NULL, D_RTOSAL_TASK_ERROR);

#ifdef D_USE_FREERTOS
   vTaskDelete(pRtosalTaskCb->taskHandle);
   uiRes = D_RTOSAL_SUCCESS;
#elif D_USE_THREADX
   #error "Add THREADX appropriate definitions"
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}


/**
* Change the priority of a specific task
*
* @param  pRtosalTaskCb - Pointer to the task control block its priority will be modified
* @param  uiNewPriority - New task priority
* @param  pOldPriority  - Output value via pointer to the old priority
* @return u32_t        - D_RTOSAL_SUCCESS
*                      - D_RTOSAL_TASK_ERROR     - The ptr, cTaskCB, in the pRtosalTaskCb is invalid
*                      - D_RTOSAL_PRIORITY_ERROR - Invalid uiPriority (must be max E_RTOSAL_PRIO_0 to E_RTOSAL_PRIO_MAX)
*                      - D_RTOSAL_PTR_ERROR      - Invalid pOldPriority
*                      - D_RTOSAL_CALLER_ERROR   - The caller can not call this function
*/
RTOSAL_SECTION u32_t rtosalTaskPriorityChange(rtosalTask_t* pRtosalTaskCb, u32_t uiNewPriority,
                                              u32_t *pOldPriority)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalTaskCb, pRtosalTaskCb == NULL, D_RTOSAL_TASK_ERROR);
   M_RTOSAL_VALIDATE_FUNC_PARAM(uiNewPriority, uiNewPriority > D_MAX_PRIORITY, D_RTOSAL_PRIORITY_ERROR);

#ifdef D_USE_FREERTOS
   M_RTOSAL_VALIDATE_FUNC_PARAM(pOldPriority, pOldPriority == NULL, D_RTOSAL_PTR_ERROR);
   /* first get the current priority */
   *pOldPriority = uxTaskPriorityGet(pRtosalTaskCb->taskHandle);
   /* set the new priority */
   vTaskPrioritySet(pRtosalTaskCb->taskHandle, uiNewPriority);
   uiRes = D_RTOSAL_SUCCESS;
#elif D_USE_THREADX
   #error "Add THREADX appropriate definitions"
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Voluntarily yield CPU time to another task
*
* @param  None
*
* @return None
*/
RTOSAL_SECTION void rtosalTaskYield(void)
{
#ifdef D_USE_FREERTOS
   taskYIELD();
#elif D_USE_THREADX
   tx_thread_relinquish();
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */
}

/**
* Resume a suspended task
*
* @param  pRtosalTaskCb - pointer to the task control block to be uiResumed
*
* @return u32_t       - D_RTOSAL_SUCCESS
*                     - D_RTOSAL_SUSPEND_REMOVED - Task suspend by time was removed 
*                     - D_RTOSAL_TASK_ERROR      - The ptr, cTaskCB, in the pRtosalTaskCb is invalid
*                     - D_RTOSAL_RESUME_ERROR    - The task is not suspended 
*/
RTOSAL_SECTION u32_t rtosalTaskResume(rtosalTask_t* pRtosalTaskCb)
{
   u32_t uiRes;
#ifdef D_USE_FREERTOS
   /* specify if a context switch is needed as a uiResult calling FreeRTOS ...ISR function */
   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalTaskCb, pRtosalTaskCb == NULL, D_RTOSAL_TASK_ERROR);

#ifdef D_USE_FREERTOS
   /* rtosalTaskResume invoked from an ISR context */
   if (rtosalIsInterruptContext() == D_RTOSAL_INT_CONTEXT)
   {
      /* if we uiResume from an ISR */
      uiRes = xTaskResumeFromISR(pRtosalTaskCb->taskHandle);
      if (uiRes == pdTRUE)
      {
         /* we need to switch context before ISR exit */
         xHigherPriorityTaskWoken = pdTRUE;
         uiRes = D_RTOSAL_SUCCESS;
      }
      else
      {
         uiRes = D_RTOSAL_NO_INSTANCE;
      }
   }
   else
   {
      vTaskResume(pRtosalTaskCb->taskHandle);
      uiRes = D_RTOSAL_SUCCESS;
   }
   /* due to the uiResume we got an indicating that a context
   switch should be requested before the interrupt exits */ 
   if (uiRes == D_RTOSAL_SUCCESS && xHigherPriorityTaskWoken == pdTRUE)
   {
      rtosalContextSwitchIndicationSet();
   }
#elif D_USE_THREADX
   #error "Add THREADX appropriate definitions"
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Suspend the execution of a current task for a specific time
*
* @param  uiTimerTicks - amount of time ticks to delay the task execution
*
* @return u32_t        - D_RTOSAL_SUCCESS
*                      - D_RTOSAL_WAIT_ABORTED - aborted by different consumer (like other thread)
*                      - D_RTOSAL_CALLER_ERROR - The caller can not call this function
*/
RTOSAL_SECTION u32_t rtosalTaskSleep(u32_t uiTimerTicks)
{
   u32_t uiRes;

#ifdef D_USE_FREERTOS
   vTaskDelay(uiTimerTicks);
   uiRes = D_RTOSAL_SUCCESS;
#elif D_USE_THREADX
   #error "Add THREADX appropriate definitions"
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Suspend the execution of a specific task
*
* @param  pRtosalTaskCb - pointer to the task control block its execution will be suspended
*
* @return u32_t       - D_RTOSAL_SUCCESS
*                     - D_RTOSAL_TASK_ERROR - The ptr, cTaskCB, in the pRtosalTaskCb is invalid
*                     - D_RTOSAL_SUSPEND_ERROR - The task is "done state" like terminated
*                     - D_RTOSAL_CALLER_ERROR - The caller can not call this function
*/
RTOSAL_SECTION u32_t rtosalTaskSuspend(rtosalTask_t* pRtosalTaskCb)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalTaskCb, pRtosalTaskCb == NULL, D_RTOSAL_TASK_ERROR);

#ifdef D_USE_FREERTOS
   vTaskSuspend(pRtosalTaskCb->taskHandle);
   uiRes = D_RTOSAL_SUCCESS;
#elif D_USE_THREADX
   #error "Add THREADX appropriate definitions"
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Abort a task which is in currently blocked
*
* @param  pRtosalTaskCb - pointer to the task control block to be aborted
*
* @return u32_t      - D_RTOSAL_SUCCESS
*                    - D_RTOSAL_TASK_ERROR - The ptr, cTaskCB, in the pRtosalTaskCb is invalid
*                    - D_RTOSAL_WAIT_ABORT_ERROR - The task is not in a block/wait state
*/
RTOSAL_SECTION u32_t rtosalTaskWaitAbort(rtosalTask_t* pRtosalTaskCb)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalTaskCb, pRtosalTaskCb == NULL, D_RTOSAL_TASK_ERROR);

#ifdef D_USE_FREERTOS
   /* check if the task is in blocking state */
   uiRes = xTaskAbortDelay(pRtosalTaskCb->taskHandle);
   if (uiRes == pdPASS)
   {
      uiRes = D_RTOSAL_SUCCESS;
   }
   else 
   {
      /* task isn't in blocking state */
      uiRes = D_RTOSAL_WAIT_ABORT_ERROR;
   }
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

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
  /* Register interrupt vector */
  pspInterruptsSetVectorTableAddress(&rtosal_vect_table);

#ifdef D_USE_FREERTOS
  /* Initialize the timer-tick handler function pointer to NULL */
  fptrTimerTickHandler = NULL;

  /* register E_CALL exception handler */
  pspRegisterExceptionHandler(rtosalHandleEcall, E_EXC_ENVIRONMENT_CALL_FROM_MMODE);

  /* register timer interrupt handler */
  pspRegisterInterruptHandler(rtosalTimerIntHandler, E_MACHINE_TIMER_CAUSE);

  fptrInit(NULL);
  vTaskStartScheduler();
#elif D_USE_THREADX
   #error "Add THREADX appropriate definitions"
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

/**
* Registration for TimerTick handler function
*/
void rtosalRegisterTimerTickHandler(rtosalTimerTickHandler_t fptrHandler)
{
  fptrTimerTickHandler = fptrHandler;
}

/**
* @brief retrieve scheduler state
*
* @return   D_RTOSAL_SCHEDULER_NOT_STARTED
*           D_RTOSAL_SCHEDULER_RUNNING
*           D_RTOSAL_SCHEDULER_SUSPENDED
*           D_RTOSAL_SCHEDULER_UNKNOWN_STATE
*/
u32_t rtosalGetSchedulerState(void)
{
   u32_t uiRet;
#ifdef D_USE_FREERTOS
   uiRet = xTaskGetSchedulerState();
   switch (uiRet)
   {
      case taskSCHEDULER_NOT_STARTED:
         uiRet = D_RTOSAL_SCHEDULER_NOT_STARTED;
         break;
      case taskSCHEDULER_RUNNING:
         uiRet = D_RTOSAL_SCHEDULER_RUNNING;
         break;
      case taskSCHEDULER_SUSPENDED:
         uiRet = D_RTOSAL_SCHEDULER_SUSPENDED;
         break;
      default:
         uiRet = D_RTOSAL_SCHEDULER_UNKNOWN_STATE;
         break;
   }
#elif D_USE_THREADX
   #error "Add THREADX appropriate definitions"
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */
   return uiRet;
}

/**
* @brief This interface is called from the rtos porting layer to
*        update the task stack if needed before context switch is performed.
*        If comrv is enabled, it is called to save context in case switching is done
*        while executing in an overlay function
*
* @param pxTopOfStack - pointer to current task stack
*
*/
void rtosalUpdateStackPriorContextSwitch(volatile rtosalStack_t** pxTopOfStack)
{
#ifdef D_COMRV
#ifdef D_USE_FREERTOS
   u32_t*                pUiNewSP = NULL;
   comrvTaskStackRegsVal_t* pComrvTaskStackRegsVal;

   /* save comrv context in case context switch is done while in an overlay function */
   pUiNewSP = comrvSaveContextSwitch(&((*pxTopOfStack)[D_RTOSAL_MEPC_INDEX_ON_TASK_STACK]),
                                     &((*pxTopOfStack)[D_RTOSAL_T3_INDEX_ON_TASK_STACK]),
                                     &pComrvTaskStackRegsVal);
   /* check if we need to change task sp address */
   if (pUiNewSP != NULL)
   {
      /* adjust sp to a new context switch frame */
      pUiNewSP -= (D_PSP_REG32_BYTE_WIDTH * D_RTOSAL_NUM_OF_STACK_ENTRIES);
      /* save a0-a7, s0-s11 on to the task stack */
      pUiNewSP[D_RTOSAL_S0_INDEX_ON_TASK_STACK]  = pComrvTaskStackRegsVal->uiRegS0;
      pUiNewSP[D_RTOSAL_S1_INDEX_ON_TASK_STACK]  = pComrvTaskStackRegsVal->uiRegS1;
      pUiNewSP[D_RTOSAL_A0_INDEX_ON_TASK_STACK]  = pComrvTaskStackRegsVal->uiRegA0;
      pUiNewSP[D_RTOSAL_A1_INDEX_ON_TASK_STACK]  = pComrvTaskStackRegsVal->uiRegA1;
      pUiNewSP[D_RTOSAL_A2_INDEX_ON_TASK_STACK]  = pComrvTaskStackRegsVal->uiRegA2;
      pUiNewSP[D_RTOSAL_A3_INDEX_ON_TASK_STACK]  = pComrvTaskStackRegsVal->uiRegA3;
      pUiNewSP[D_RTOSAL_A4_INDEX_ON_TASK_STACK]  = pComrvTaskStackRegsVal->uiRegA4;
      pUiNewSP[D_RTOSAL_A5_INDEX_ON_TASK_STACK]  = pComrvTaskStackRegsVal->uiRegA5;
      pUiNewSP[D_RTOSAL_A6_INDEX_ON_TASK_STACK]  = pComrvTaskStackRegsVal->uiRegA6;
      pUiNewSP[D_RTOSAL_A7_INDEX_ON_TASK_STACK]  = pComrvTaskStackRegsVal->uiRegA7;
      pUiNewSP[D_RTOSAL_S2_INDEX_ON_TASK_STACK]  = pComrvTaskStackRegsVal->uiRegS2;
      pUiNewSP[D_RTOSAL_S3_INDEX_ON_TASK_STACK]  = pComrvTaskStackRegsVal->uiRegS3;
      pUiNewSP[D_RTOSAL_S4_INDEX_ON_TASK_STACK]  = pComrvTaskStackRegsVal->uiRegS4;
      pUiNewSP[D_RTOSAL_S5_INDEX_ON_TASK_STACK]  = pComrvTaskStackRegsVal->uiRegS5;
      pUiNewSP[D_RTOSAL_S6_INDEX_ON_TASK_STACK]  = pComrvTaskStackRegsVal->uiRegS6;
      pUiNewSP[D_RTOSAL_S7_INDEX_ON_TASK_STACK]  = pComrvTaskStackRegsVal->uiRegS7;
      pUiNewSP[D_RTOSAL_S8_INDEX_ON_TASK_STACK]  = pComrvTaskStackRegsVal->uiRegS8;
      pUiNewSP[D_RTOSAL_S9_INDEX_ON_TASK_STACK]  = pComrvTaskStackRegsVal->uiRegS9;
      pUiNewSP[D_RTOSAL_S10_INDEX_ON_TASK_STACK] = pComrvTaskStackRegsVal->uiRegS10;
      pUiNewSP[D_RTOSAL_S11_INDEX_ON_TASK_STACK] = pComrvTaskStackRegsVal->uiRegS11;
      pUiNewSP[D_RTOSAL_RA_INDEX_ON_TASK_STACK] = pComrvTaskStackRegsVal->uiRegRa;
      pUiNewSP[D_RTOSAL_T5_INDEX_ON_TASK_STACK] = pComrvTaskStackRegsVal->uiRegT5;
      /* save new task sp */
      *pxTopOfStack = pUiNewSP;
   }
#elif D_USE_THREADX
   #error "Add THREADX appropriate definitions"
#else
   #error "Add appropriate RTOS definitions"
#endif /* D_USE_FREERTOS */
#endif /* D_COMRV */
}
