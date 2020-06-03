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

#include "mati.h"
#include "mati_utilities.h"
#include "psp_defines.h"
#include "rtosal_task_api.h"

/***************************************************************************/
/**    DEFINITIONS                                                        **/
/***************************************************************************/


#define D_THREAD_1 0x0
#define D_THREAD_2 0x1
#define D_THREAD_3 0x2
#define D_THREAD_4 0x3
#define D_THREAD_5 0x4
#define D_THREAD_6 0x5
#define D_THREAD_7 0x6


/***************************************************************************/
/**    MACROS                                                             **/
/***************************************************************************/
#define M_MERGE_STRINGS(a,b,c)  a b c

/***************************************************************************/
/**    TYPES                                                              **/
/***************************************************************************/

/***************************************************************************/
/**    LOCAL PROTOTYPES                                                   **/
/***************************************************************************/

/***************************************************************************/
/**    EXTERNAL PROTOTYPES                                                **/
/***************************************************************************/
extern u32_t G_uiTasksId[D_NUMBER_OF_TASKS];

/***************************************************************************/
/**    GLOBAL VARIABLES                                                   **/
/***************************************************************************/
u32_t  G_threads_bitmap;

/***************************************************************************/
/**    START IMPLEMENTATION                                               **/
/***************************************************************************/

BOOL f_mati_is_thread_free(u32_t uiThread)
{
  BOOL res = D_PSP_FALSE;
  if ((G_threads_bitmap & (1 << uiThread)) == 0)
  {
    res = D_PSP_TRUE;
  }
  return res;
}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : generic_task_function                            **/
/**   Description      : MATI main threads function                       **/
/**   Input parameters : Task number                                      **/
/**   Output parameters: None                                             **/
/**   Return value     : None                                             **/
/**   Remarks          : This function takes the parameter (thread number)**/
/**                      and check if the configuration holds a function  **/
/**                      to run. if it contain a function pointer, then   **/
/**                      it runs the function and clear the pointer.      **/
/**                                                                       **/
/***************************************************************************/
void generic_task_function( u32_t uiTaskNumber)
{
// need to verify that MATI is still running before killing the task
   while (E_MATI_OK == f_mati_frameworkIsRunning(G_pFrameWorkCB))
   {
      if ( G_pFrameWorkCB->sTestTasksFunctionArr[uiTaskNumber] != NULL )
      {
         /* run the test function in case that we configure one */
         G_pFrameWorkCB->sTestTasksFunctionArr[uiTaskNumber]( &(G_pFrameWorkCB->sTestTasksFunctionParamArr[uiTaskNumber]) );

         /* reset the function pointer in the function array after finish the job */
         G_pFrameWorkCB->sTestTasksFunctionArr[uiTaskNumber] = NULL;
      }
      else
      {
         rtosalTaskSleep/*f_rtos_timeDelayMilliSeconds*/(D_TIMEOUT_MICRO);
      }
   }//while
}

void f_mati_test_task_1(u32_t param)
{
  generic_task_function(param);
}

void f_mati_test_task_2(u32_t param)
{
  generic_task_function(param);
}

void f_mati_test_task_3(u32_t param)
{
  generic_task_function(param);
}

void f_mati_test_task_4(u32_t param)
{
  generic_task_function(param);
}

void f_mati_test_task_5(u32_t param)
{
  generic_task_function(param);
}

void f_mati_test_task_6(u32_t param)
{
  generic_task_function(param);
}

void f_mati_test_task_7(u32_t param)
{
  generic_task_function(param);
}



/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_get_last_thread                           **/
/**   Description      : get the last free thread from the thread pull    **/
/**   Input parameters : None                                             **/
/**   Output parameters: None                                             **/
/**   Return value     : thread index                                     **/
/**   Remarks          : None                                             **/
/**                                                                       **/
/***************************************************************************/
u32_t f_mati_get_last_thread()
{
  u32_t res = 0;
  u08_t threads_bitmap;
  for  ( threads_bitmap = 0 ; threads_bitmap < D_NUMBER_OF_THREADS && res == 0 ; threads_bitmap++)
  {
    if ( f_mati_is_thread_free(threads_bitmap) )
    {
      res = threads_bitmap;
    }
  }
  return res;
}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_number_of_free_threads                         **/
/**   Description      : get random thread from the thread pull           **/
/**   Input parameters : None                                             **/
/**   Output parameters: None                                             **/
/**   Return value     : thread index                                     **/
/**   Remarks          :                                                  **/
/***************************************************************************/
u32_t  f_mati_number_of_free_threads()
{
  u08_t free_threads_counter = D_NUMBER_OF_THREADS ;
  for  (u08_t threads_bitmap = G_threads_bitmap; threads_bitmap > 0 ; threads_bitmap = threads_bitmap >> 1)
  {
    if ( threads_bitmap & 1 )
      free_threads_counter--;
  }
  return free_threads_counter;
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_get_random_thread                         **/
/**   Description      : get random thread from the thread pull and       **/
/**                      turn on the thread bit in the bit map            **/
/**   Input parameters : None                                             **/
/**   Output parameters: None                                             **/
/**   Return value     : thread index                                     **/
/**   Remarks          : in case of an error, return 0x80000000           **/
/***************************************************************************/
u32_t  f_mati_get_random_thread()
{
  u32_t rand;
  u32_t freeThreds = f_mati_number_of_free_threads();

  if ( freeThreds == 0)
  {
    rand = 31;
  }
  else if (freeThreds == 1)
  {
    rand = f_mati_get_last_thread();
  }
  else
  {
    do
    {
      rand = f_mati_getRandom() % (D_NUMBER_OF_THREADS) ;
    } while ( !f_mati_is_thread_free(rand));
  }

  G_threads_bitmap |=  (1 << rand);               // turn on the thread bit in the threads bitmap
  return rand;
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_get_index_of_bit_in_byte                       **/
/**   Description      : return the index of the first bit that is not 0  **/
/**   Input parameters : number - the number that we indexing             **/
/**   Output parameters: None                                             **/
/**   Return value     : the index of tje bit that is turn on             **/
/**   Remarks          :                                                  **/
/***************************************************************************/
u32_t  f_mati_get_index_of_bit_in_byte(u32_t number)
{
  u32_t index = 0;

  if (number > 0 && number < 256)
  {
    while ( !(number & 0x1))
    {
      index++;
      number >>= 1;
    }
  }

  return index;
}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_get_random_thread                              **/
/**   Description      : release thread to the thread pull and turn off   **/
/**                      the thread bit in the bit map                    **/
/**   Input parameters : the thread number                                **/
/**   Output parameters: None                                             **/
/**   Return value     : RTOS_OK in case of success, else RTOS_ERROR      **/
/**   Remarks          :                                                  **/
/***************************************************************************/
E_MATI_Result  f_mati_release_thread_from_pool(u32_t thread_bitmask_to_release)
{
  G_threads_bitmap &= ~ thread_bitmask_to_release;
  ///f_rtos_taskYield();
  rtosalTaskYield();
  return E_MATI_OK;
}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_get_threads_bitmap                             **/
/**   Description      : gets the occupied threads bitmap                 **/
/**   Input parameters : None                                             **/
/**   Output parameters: None                                             **/
/**   Return value     : threads bitmask                                  **/
/**   Remarks          :                                                  **/
/***************************************************************************/
u32_t f_get_threads_bitmap(){
  return G_threads_bitmap;
}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : start_tasks                                      **/
/**   Description      : define the tests tasks and initiate them         **/
/**   Input parameters : None                                             **/
/**   Output parameters: None                                             **/
/**   Return value     : RTOS_OK in case of success, else RTOS_ERROR      **/
/**   Remarks          :                                                  **/
/***************************************************************************/

E_MATI_Result f_mati_start_tasks(  )
{
#if 0 /* ronen */
   u32_t i = 0;

   G_uiTasksId[0] = f_rtos_taskCreateBlocked(D_PROCESSOR_MAIN, D_TEST_TASK_1_TEMPLATE_INDEX, D_TEST_TASK_1_ID);
   G_uiTasksId[1] = f_rtos_taskCreateBlocked(D_PROCESSOR_MAIN, D_TEST_TASK_2_TEMPLATE_INDEX, D_TEST_TASK_2_ID);
   G_uiTasksId[2] = f_rtos_taskCreateBlocked(D_PROCESSOR_MAIN, D_TEST_TASK_3_TEMPLATE_INDEX, D_TEST_TASK_3_ID);
   G_uiTasksId[3] = f_rtos_taskCreateBlocked(D_PROCESSOR_MAIN, D_TEST_TASK_4_TEMPLATE_INDEX, D_TEST_TASK_4_ID);
   G_uiTasksId[4] = f_rtos_taskCreateBlocked(D_PROCESSOR_MAIN, D_TEST_TASK_5_TEMPLATE_INDEX, D_TEST_TASK_5_ID);
   G_uiTasksId[5] = f_rtos_taskCreateBlocked(D_PROCESSOR_MAIN, D_TEST_TASK_6_TEMPLATE_INDEX, D_TEST_TASK_6_ID);
   G_uiTasksId[6] = f_rtos_taskCreateBlocked(D_PROCESSOR_MAIN, D_TEST_TASK_7_TEMPLATE_INDEX, D_TEST_TASK_7_ID);

   while ( i < D_NUMBER_OF_THREADS )
   {
      if (G_uiTasksId[i] == RTOS_MQX_NULL_TASK_ID)
      {
      f_mati_setErrorBit(G_pFrameWorkCB, E_TEST_ERROR_INIT);
      return E_MATI_ERROR;
      }
      G_pFrameWorkCB->sTestTasksFunctionArr[i] = NULL;  // roeeH - verify we are starting the tests Tasks functions from a NULL pointers
      i++;
   }

   if (f_rtos_taskMakeReady("testTask1") == RTOS_ERROR ||
      f_rtos_taskMakeReady("testTask2") == RTOS_ERROR ||
      f_rtos_taskMakeReady("testTask3") == RTOS_ERROR ||
      f_rtos_taskMakeReady("testTask4") == RTOS_ERROR ||
      f_rtos_taskMakeReady("testTask5") == RTOS_ERROR ||
      f_rtos_taskMakeReady("testTask6") == RTOS_ERROR ||
      f_rtos_taskMakeReady("testTask7") == RTOS_ERROR)
   {
      f_mati_setErrorBit(G_pFrameWorkCB, E_TEST_ERROR_INIT);
      return E_MATI_ERROR;
   }
#endif /* 0 ronen */

   return E_MATI_OK;
}

E_MATI_Result f_resetCB_TasksFunctions()
{
  u08_t i = 0;

  while ( i < D_NUMBER_OF_THREADS )
  {
    G_pFrameWorkCB->sTestTasksFunctionArr[i] = NULL;  // roeeH - verify we are starting the tests Tasks functions from a NULL pointers
    i++;
  }
  return E_MATI_OK;
}

// inits
E_MATI_Result f_mati_init_test_task(S_TEST_DEFINITIONS test_definitions)
{
   u08_t i = D_NUMBER_OF_THREADS;

   /* Do this first - static status flag indicates we're running */
   // TODO ronen fix me //f_rtos_setRunflag();

   if (test_definitions.eClearTestData == E_TEST_CONFIG_CLEAR_DATA_ON)
   {
      G_pFrameWorkCB->uiTestSharedMemory = 0;
      G_pFrameWorkCB->uiTestSyncPoint = 0;
   }

   G_testDefinitions.eIsBlocking  = E_TEST_CONFIG_BLOCKING_ON;
   G_testDefinitions.eIsUnitTest  = E_TEST_CONFIG_TEST_TYPE_UNIT;
   f_mati_clearErrorBits(G_pFrameWorkCB);

   f_rtos_memZero((u08_t*)&G_pFrameWorkCB->sTestTasksFunctionParamArr, D_NUMBER_OF_THREADS * sizeof(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM));

   // dequeue the tasks from the timeout queueu
   while (i--)
   {
      ;//_time_dequeue(G_Tasks_id[i]);
   }
   return E_MATI_OK;
}

