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
#include "psp_defines.h"
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
*									  must be invoked to run the task
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
   // TODO:
   //uiRes = add a call to ThreadX task create API
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
   // TODO:
   //we should terminate thread before deleting it on ThreadX
   //uiRes = add a call to ThreadX task delete API
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
   // TODO:
   //uiRes = add a call to ThreadX task priority change API
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
#endif /* #ifdef D_USE_FREERTOS */

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalTaskCb, pRtosalTaskCb == NULL, D_RTOSAL_TASK_ERROR);

#ifdef D_USE_FREERTOS
   /* rtosalTaskResume invoked from an ISR context */
   if (pspIsInterruptContext() == D_PSP_INT_CONTEXT)
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
   // TODO:
   //uiRes = add a call to ThreadX task resume API
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
   // TODO:
   //uiRes = add a call to ThreadX task sleep API
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
   // TODO:
   //uiRes = add a call to ThreadX task suspend API
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
#elif D_USE_THREADX
   // TODO:
   //uiRes = add a call to ThreadX task abort API
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}
