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
#include "rtosal_macro.h"
#include "rtosal.h"
#include "psp_api.h"
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

/**
* global variables
*/


/**
* Task creation function
*
* @param  pRtosalTaskCb               - pointer to the task control block to be created
* @param  pTaskName                 - pointer to the task name
* @param  uiPriority                - task priority
* @param  fptrRtosalTaskEntryPoint    - task function handler
* @param  uiTaskEntryPointParameter - task function handler input parameter
* @param  uiStackSize               - task stack size in bytes
* @param  pStackBuffer              - pointer to the stack buffer
* @param  uiTimeSliceTicks          - Number of ticks a task is allowed to run before
*                                   other ready tasks of the same priority are given
*                                   cpu time; can be one of the following values:
*                                   D_RTOSAL_NO_TIME_SLICE or 1 - 0xFFFFFFFF
* @param  uiAutoStart               - if D_RTOSAL_AUTO_START is used, task starts running;
*                                   if D_RTOSAL_DONT_START is used, the task is suspended
*                                   and rtosalTaskResume() must be invoked to run the task.
* @param  uiPreemptThuiReshold        - define the priority value which a value below
*                                   and including will not cause preemption of the
*                                   created task. For example, suppose a thread of
*                                   priority 20 only interacts with a group of
*                                   threads that have priorities between 15 and 20.
*                                   During its critical sections, the thread of
*                                   priority 20 can set its preemption-thuiReshold
*                                   to 15, thereby preventing preemption from all
*                                   of the threads that it interacts with. This
*                                   still permits really important threads (priorities
*                                   between 0 and 14) to preempt this thread during
*                                   its critical section processing, which uiResults
*                                   in much more uiResponsive processing
*
* @return u32_t                    - D_RTOSAL_SUCCESS
*                                  - D_RTOSAL_TASK_ERROR Invalid task control pointer
*                                  - D_RTOSAL_PTR_ERROR Invalid starting adduiRess of the
*                                    entry point or the stack area is invalid, usually NULL
*                                  - D_RTOSAL_SIZE_ERROR Size of stack area is invalid.
*                                  - D_RTOSAL_PRIORITY_ERROR Invalid task priority
*                                  - D_RTOSAL_THuiResH_ERROR Invalid preemptionthuiReshold specified.
*                                    This value must be a valid priority less than or equal
*                                    to the initial priority of the task
*                                  - D_RTOSAL_START_ERROR Invalid auto-start selection
*                                  - D_RTOSAL_CALLER_ERROR Invalid caller of this service
*/
u32_t rtosalTaskCreate(rtosalTask_t* pRtosalTaskCb, const s08_t* pTaskName, rtosalPriority_t uiPriority,
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
   uiRes = tx_thread_create((TX_THREAD*)pRtosalTaskCb->cTaskCB, pTaskName,
                      fptrRtosalTaskEntryPoint, uiTaskEntryPointParameter,
                      pStackBuffer, uiStackSize, uiPriority, uiPreemptThuiReshold,
                      uiTimeSliceTicks, uiAutoStart);
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Delete a task
*
* @param  pRtosalTaskCb - pointer to the task control block to be destroyed
*
* @return u32_t      - D_RTOSAL_SUCCESS
*                    - D_RTOSAL_TASK_ERROR Invalid application thread pointer
*                    - D_RTOSAL_DELETE_ERROR Specified thread is not in a
*                      terminated or completed state
*                    - D_RTOSAL_CALLER_ERROR Invalid caller of this service
*/
u32_t rtosalTaskDestroy(rtosalTask_t* pRtosalTaskCb)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalTaskCb, pRtosalTaskCb == NULL, D_RTOSAL_TASK_ERROR);

#ifdef D_USE_FREERTOS
   vTaskDelete(pRtosalTaskCb->taskHandle);
   uiRes = D_RTOSAL_SUCCESS;
#elif D_USE_THREADX
   uiRes = tx_thread_delete((TX_THREAD*)pRtosalTaskCb->cTaskCB);
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}


/**
* Change the priority of a specific task
*
* @param  pRtosalTaskCb  - pointer to the task control block its priority will be modified
* @param  uiNewPriority - new task priority
*
* @return u32_t        - D_RTOSAL_SUCCESS
*                      - D_RTOSAL_TASK_ERROR Invalid application thread pointer
*                      - D_RTOSAL_PRIORITY_ERROR Specified new priority is not valid
*                      - D_RTOSAL_PTR_ERROR Invalid pointer to previous priority
*                        storage location
*                      - D_RTOSAL_CALLER_ERROR Invalid caller of this service
*/
u32_t rtosalTaskPriorityChange(rtosalTask_t* pRtosalTaskCb, u32_t uiNewPriority,
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
   uiRes = tx_thread_priority_change((TX_THREAD*)pRtosalTaskCb->cTaskCB, uiNewPriority, pOldPriority)
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
void rtosalTaskYield(void)
{
#ifdef D_USE_FREERTOS
   taskYIELD();
#elif D_USE_THREADX
   tx_thread_relinquish();
#endif /* #ifdef D_USE_FREERTOS */
}

/**
* uiResume a suspended task
*
* @param  pRtosalTaskCb - pointer to the task control block to be uiResumed
*
* @return u32_t       - D_RTOSAL_SUCCESS
*                     - D_RTOSAL_SUSPEND_LIFTED Previously set delayed suspension was lifted
*                     - D_RTOSAL_TASK_ERROR Invalid application thread pointer
*                     - D_RTOSAL_uiResUME_ERROR Specified task is not suspended
*                     or was previously suspended by a service other than 
*                     rtosalTaskSuspend
*/
u32_t rtosalTaskResume(rtosalTask_t* pRtosalTaskCb)
{
   u32_t uiRes;
#ifdef D_USE_FREERTOS
   /* specify if a context switch is needed as a uiResult calling FreeRTOS ...ISR function */
   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#endif /* #ifdef D_USE_FREERTOS */

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalTaskCb, pRtosalTaskCb == NULL, D_RTOSAL_TASK_ERROR);

#ifdef D_USE_FREERTOS
   /* rtosalTaskResume invoked from an ISR context */
   if (pspIsInterruptContext() == D_INT_CONTEXT)
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
   uiRes = tx_thread_uiResume((TX_THREAD*)pRtosalTaskCb->cTaskCB)
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Suspend the execution of a current task for a specific time
*
* @param  uiTimerTicks - amount of time ticks to delay the task execution
*
* @return u32_t        - D_RTOSAL_SUCCESS
*                      - D_RTOSAL_WAIT_ABORTED Suspension was aborted by 
*                      another thread, timer, or ISR
*                      - D_RTOSAL_CALLER_ERROR Service called from a non-task
*/
u32_t rtosalTaskSleep(u32_t uiTimerTicks)
{
   u32_t uiRes;

#ifdef D_USE_FREERTOS
   vTaskDelay(uiTimerTicks);
   uiRes = D_RTOSAL_SUCCESS;
#elif D_USE_THREADX
   uiRes = tx_thread_sleep(uiTimerTicks);
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Suspend the execution of a specific task
*
* @param  pRtosalTaskCb - pointer to the task control block its execution will be suspended
*
* @return u32_t       - D_RTOSAL_SUCCESS
*                     - D_RTOSAL_TASK_ERROR Invalid application task pointer
*                     - D_RTOSAL_SUSPEND_ERROR Specified task is in a terminated
*                     or completed state
*                     - D_RTOSAL_CALLER_ERROR Invalid caller of this service
*/
u32_t rtosalTaskSuspend(rtosalTask_t* pRtosalTaskCb)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalTaskCb, pRtosalTaskCb == NULL, D_RTOSAL_TASK_ERROR);

#ifdef D_USE_FREERTOS
   vTaskSuspend(pRtosalTaskCb->taskHandle);
   uiRes = D_RTOSAL_SUCCESS;
#elif D_USE_THREADX
   uiRes = tx_thread_suspend((TX_THREAD*)pRtosalTaskCb->cTaskCB);
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Abort a task which is in currently blocked
*
* @param  pRtosalTaskCb - pointer to the task control block to be aborted
*
* @return u32_t      - D_RTOSAL_SUCCESS
*                    - D_RTOSAL_TASK_ERROR Invalid application task pointer
*                    - D_RTOSAL_WAIT_ABORT_ERROR Specified task is not in a
*                      waiting state
*/
u32_t rtosalTaskWaitAbort(rtosalTask_t* pRtosalTaskCb)
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
#elif D_USE_THREADX
   uiRes = tx_thread_wait_abort((TX_THREAD*)pRtosalTaskCb->cTaskCB)
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}
