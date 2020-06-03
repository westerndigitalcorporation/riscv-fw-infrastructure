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
/**************************************************************************
***************************************************************************
***
*** File:
***
*** Comments: 
***    
***
*** Expected Output:
***
*** 
***************************************************************************
***************************************************************************/


/***************************************************************************/
/**    INCLUDE FILES                                                      **/
/***************************************************************************/
#if defined(PROD_MATI_MQX) || defined(PROD_MATI_AOM)
  #if ((PROD_MATI_MQX==1) && (PROD_MATI_AOM==1))
    #error "MATI product should contain MATI_MQX=1 OR MATI_AOM=1 definition, not both"
  #elif ((PROD_MATI_MQX==0) && (PROD_MATI_AOM==0))
    #error "MATI product should contain MATI_MQX=1 OR MATI_AOM=1 definition, at least one of them"
  #endif
#endif

#include "mati.h"
#include "mati_utilities.h"
#include "mati_tests.h"
#include "mati_api.h"
#include "psp_macros.h"
#include "psp_attributes.h"
#include "psp_intrinsics.h"

#if (PROD_MATI_MQX==1)
  #if defined(RAM)
    #include "pl_rtos.h"
    #include "string.h"
    #include "rtos_user_api.h"
  #endif
  #include "rtos_api_pointers.h"
#endif

/***************************************************************************/
/**    DEFINITIONS                                                        **/
/***************************************************************************/

/***************************************************************************/
/**    MACROS                                                             **/
/***************************************************************************/

/***************************************************************************/
/**    TYPES                                                              **/
/***************************************************************************/

/***************************************************************************/
/**    LOCAL PROTOTYPES                                                   **/
/***************************************************************************/
#if (PROD_MATI_AOM==1)
  void f_mati_mainAomMuxTask(u32_t param);
#elif (PROD_MATI_MQX==1)
  void f_mati_mainMqxMuxTask(u32_t uiPparam);
#endif

/***************************************************************************/
/**    EXTERNAL PROTOTYPES                                                **/
/***************************************************************************/
//extern LINKER_SYM _MRAMM_PAST_END_; //_MRAMM_PAST_END_
extern void (*f_rtos_taskMQXExit) (u32_t uiError) ;

/***************************************************************************/
/**    GLOBAL VARIABLES                                                   **/
/***************************************************************************/
S_TEST_DEFINITIONS G_testDefinitions =
{
  E_TEST_CONFIG_TEST_TYPE_UNIT,
  E_TEST_CONFIG_CLEAR_DATA_ON,
  E_TEST_CONFIG_BLOCKING_ON,
  E_TEST_CONFIG_BTST_OFF,
  E_TEST_CONFIG_TEST_POSITIVE_TYPE
};

/*---------------------------------------------------------*/
#if (PROD_MATI_MQX==1)
  const mati_tests_progress_function_ptr G_lookupTable_mati_test_group_handle[E_CB_TEST_GROUP_MAX] =
  {
    f_mati_handleTestGroupLwevents,      /* E_CB_TEST_GROUP_LWEVENTS */
    f_mati_handleTestGroupQueue,         /* E_CB_TEST_GROUP_QUEUE */
    f_mati_handleTestGroupMutex,         /* E_CB_TEST_GROUP_MUTEX */
    f_mati_handleTestGroupFMQueue,       /* E_CB_TEST_GROUP_FMQUEUE */
    f_mati_handleTestGroupLWSemaphore,   /* E_CB_TEST_GROUP_LWSEMAPHORE */
    f_mati_handleTestGroupLWMemry,       /* E_CB_TEST_GROUP_LWMEMRY */
    f_mati_handleTestGroupISR,           /* E_CB_TEST_GROUP_ISR */
    f_mati_handleTestGroupNone,          /* E_CB_TEST_GROUP_AOM - **SKIP**  */
    f_mati_handleTestGroupAffinity,      /* E_CB_TEST_GROUP_AFFINITY */
    f_mati_handleTestGroupHWStack        /* E_CB_TEST_GROUP_HW_STACK */
  };
#endif

/*---------------------------------------------------------*/
S_FRAMEWORK_CONTROL_BLOCK G_FrameWorkCB;
S_FRAMEWORK_CONTROL_BLOCK_PTR G_pFrameWorkCB = &G_FrameWorkCB;

#if (PROD_MATI_MQX==1)

  u32_t G_uiTasksId[D_NUMBER_OF_TASKS] = {0};

  /** MQX TASKS Initialization Data **/
  RTOS_TASK_TEMPLATE_STRUCT  MATI_MQX_test_template_list_rom[] =
  {
    // Task Index,		                Function,				   stack,                          Priority,               Name,          Attributes,				   Param,  Time Slice
    { D_MAIN_MUX_TASK_TEMPLATE_INDEX, f_mati_mainMqxMuxTask, D_MATI_DEFAULT_TASK_STACK_SIZE, D_TASK_DEFULT_PRIORITY, "main",      MQX_AUTO_START_TASK, 0,      0 },
    { D_TEST_TASK_1_TEMPLATE_INDEX,   f_mati_test_task_1,    D_MATI_DEFAULT_TASK_STACK_SIZE, D_TASK_DEFULT_PRIORITY, "testTask1", 0,                   0,      0 },
    { D_TEST_TASK_2_TEMPLATE_INDEX,   f_mati_test_task_2,    D_MATI_DEFAULT_TASK_STACK_SIZE, D_TASK_DEFULT_PRIORITY, "testTask2", 0,                   0,      0 },
    { D_TEST_TASK_3_TEMPLATE_INDEX,   f_mati_test_task_3,    D_MATI_DEFAULT_TASK_STACK_SIZE, D_TASK_DEFULT_PRIORITY, "testTask3", 0,                   0,      0 },
    { D_TEST_TASK_4_TEMPLATE_INDEX,   f_mati_test_task_4,    D_MATI_DEFAULT_TASK_STACK_SIZE, D_TASK_DEFULT_PRIORITY, "testTask4", 0,                   0,      0 },
    { D_TEST_TASK_5_TEMPLATE_INDEX,   f_mati_test_task_5,    D_MATI_DEFAULT_TASK_STACK_SIZE, D_TASK_DEFULT_PRIORITY, "testTask5", 0,                   0,      0 },
    { D_TEST_TASK_6_TEMPLATE_INDEX,   f_mati_test_task_6,    D_MATI_DEFAULT_TASK_STACK_SIZE, D_TASK_DEFULT_PRIORITY, "testTask6", 0,                   0,      0 },
    { D_TEST_TASK_7_TEMPLATE_INDEX,   f_mati_test_task_7,    D_MATI_DEFAULT_TASK_STACK_SIZE, D_TASK_DEFULT_PRIORITY, "testTask7", 0,                   0,      0 },
    { D_TEST_DUMMY_1,                 NULL,                  0,                              D_TASK_LOWER_PRIORITY,  "dummy 1",   0,                   0,      0 },
    { D_TEST_DUMMY_2,                 NULL,                  0,                              D_TASK_LOWEST_PRIORITY, "dummy 2",   0,                   0,      0 },
    { D_TEST_DUMMY_3,                 NULL,                  0,                              D_TASK_WORKER_PRIORITY, "dummy 2",   0,                   0,      0 },
    { 0,                              0,                     0,                              0,                      0,           0,                   0,      0 }
  };

  #if defined(ROM)
    /** MQX Initialization Data **/
    RTOS_MQX_INITIALIZATION_STRUCT MATI_MQX_test_init_struct_rom =
    {
      1,                                            // PROCESSOR_NUMBER
      SYSRAM_START,                                 // START_OF_KERNEL_MEMORY
      SYSRAM_END,                                   // END_OF_KERNEL_MEMORY
      D_MATI_INTERRUPT_STACK_SIZE,                  // INTERRUPT_STACK_SIZE
      (rtosPointer)MATI_MQX_test_template_list_rom, // TASK_TEMPLATE_LIST
      2,                                            // MQX_HARDWARE_INTERRUPT_LEVEL_MAX
      0,                                            // MAX_BUFFER_POOLS
      0,                                            // MAX_QUEUE_NUMBER
      NULL,                                         // IO_CHANNEL
      0                                             // IO_OPEN_MODE
    };
  #endif

    RTOS_TASK_TEMPLATE_STRUCT_PTR  g_TaskTemplateListPtrSave = NULL;       /* Save location for the address of the task template list for this processor.
                                                                            * To be restored upon Mati completion, to return control to pl_main.c
                                                                            * and reset.
                                                                            */
#endif //if (__MQX_AOM__ == 1)

/***************************************************************************/
/**    START IMPLEMENTATION                                               **/
/***************************************************************************/

#if (PROD_MATI_MQX==1) && defined(RAM)
  /***************************************************************************/
  /**                                                                       **/
  /**   Function Name    : f_mati_updateTaskStructFromParmas                **/
  /**   Description      : update MATI_MQX_test_template_list_rom           **/
  /**                      with input params from user                      **/
  /**   Input parameters : S_FRAMEWORK_CONTROL_BLOCK_PTR                    **/
  /**   Output parameters: None                                             **/
  /**   Return value     : None                                             **/
  /**   Remarks          :                                                  **/
  /***************************************************************************/
  void f_mati_updateTaskStructFromParmas(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
  {

    u32_t uiNumEntries = sizeof(MATI_MQX_test_template_list_rom)/ sizeof(RTOS_TASK_TEMPLATE_STRUCT);
    u32_t uiStackSize = 0;
    u32_t i;
    S_FRAMEWORK_BTSTS_PARAMS_PTR pBtstParam = f_mati_getInputParams(pFrameWorkCB);
    switch (pBtstParam->MatiFrameworkParams.usNumStackSize)
    {
      case E_MATI_FW_STACK_SIZE_1024:
        uiStackSize = 1024; break;
      case E_MATI_FW_STACK_SIZE_1536:
        uiStackSize = 1536; break;
      case E_MATI_FW_STACK_SIZE_2048:
        uiStackSize = 2048; break;
      case E_MATI_FW_STACK_SIZE_2560:
        uiStackSize = 2560; break;
      case E_MATI_FW_STACK_SIZE_3072:
        uiStackSize = 3072; break;
      case E_MATI_FW_STACK_SIZE_3584:
        uiStackSize = 3584; break;
      case E_MATI_FW_STACK_SIZE_4096:
        uiStackSize = 4096; break;
    }// Switch

    for ( i = 0; i < uiNumEntries ; i++ )
    {
      if(NULL != MATI_MQX_test_template_list_rom[i].TASK_ADDRESS)
      {
        MATI_MQX_test_template_list_rom[i].TASK_STACKSIZE = uiStackSize;
      }
    }//for every task
  }

  /***************************************************************************/
  /**                                                                       **/
  /**   Function Name    : f_mati_get_enabled_test_group                    **/
  /**   Description      : returns the next enabled test group to           **/
  /**                      run                                              **/
  /**   Input parameters : S_FRAMEWORK_CONTROL_BLOCK_PTR                    **/
  /**   Output parameters: None                                             **/
  /**   Return value     : E_CB_TEST_GROUP                                  **/
  /**   Remarks          :                                                  **/
  /***************************************************************************/
  E_CB_TEST_GROUP f_mati_get_enabled_test_group(S_FRAMEWORK_CONTROL_BLOCK_PTR frameWorkCB_p)
  {

    E_CB_TEST_GROUP retVal = E_CB_TEST_GROUP_MAX;
    S_FRAMEWORK_BTSTS_PARAMS_PTR s_BtstParam_p = f_mati_getInputParams(frameWorkCB_p);
    if(s_BtstParam_p->MatiFrameworkParams.usQueueTestGroupEnabled)
    {
      s_BtstParam_p->MatiFrameworkParams.usQueueTestGroupEnabled = 0;
      retVal = E_CB_TEST_GROUP_QUEUE;
    }
    else if(s_BtstParam_p->MatiFrameworkParams.usLWEventTestGroupEnabled)
    {
      s_BtstParam_p->MatiFrameworkParams.usLWEventTestGroupEnabled = 0;
      retVal = E_CB_TEST_GROUP_LWEVENTS;
    }
    else if(s_BtstParam_p->MatiFrameworkParams.usMutexTestGroupEnabled)
    {
      s_BtstParam_p->MatiFrameworkParams.usMutexTestGroupEnabled = 0;
      retVal = E_CB_TEST_GROUP_MUTEX;
    }
    else if(s_BtstParam_p->MatiFrameworkParams.usFMQueueTestGroupEnabled)
    {
      s_BtstParam_p->MatiFrameworkParams.usFMQueueTestGroupEnabled = 0;
      retVal = E_CB_TEST_GROUP_FMQUEUE;
    }
    else if(s_BtstParam_p->MatiFrameworkParams.usLWSemaphoreTestGroupEnabled)
    {
      s_BtstParam_p->MatiFrameworkParams.usLWSemaphoreTestGroupEnabled = 0;
      retVal = E_CB_TEST_GROUP_LWSEMAPHORE;
    }
    else if(s_BtstParam_p->MatiFrameworkParams.usLWMemTestGroupEnabled)
    {
      s_BtstParam_p->MatiFrameworkParams.usLWMemTestGroupEnabled = 0;
      retVal = E_CB_TEST_GROUP_LWMEMRY;
    }
    else if(s_BtstParam_p->MatiFrameworkParams.usISRTestGroupEnabled)
    {
      s_BtstParam_p->MatiFrameworkParams.usISRTestGroupEnabled = 0;
      retVal = E_CB_TEST_GROUP_ISR;
    }
    else if(s_BtstParam_p->MatiFrameworkParams.usAffinityTestGroupEnabled)
    {
      s_BtstParam_p->MatiFrameworkParams.usAffinityTestGroupEnabled = 0;
      retVal = E_CB_TEST_GROUP_AFFINITY;
    }
    else if(s_BtstParam_p->MatiFrameworkParams.usHardwareStackTestGroupEnabled)
    {
      s_BtstParam_p->MatiFrameworkParams.usHardwareStackTestGroupEnabled = 0;
      retVal = E_CB_TEST_GROUP_HW_STACK;
    }
    return (retVal);
  }

  /***************************************************************************/
  /**                                                                       **/
  /**   Function Name    : f_mati_asign_tasks_struct                        **/
  /**   Description      : replace MQX global structure pointer to MATI     **/
  /**   Input parameters : None                                             **/
  /**   Output parameters: None                                             **/
  /**   Return value     : None                                             **/
  /**   Remarks          :                                                  **/
  /***************************************************************************/
  void f_mati_asignTasksStruct()
  {
    g_TaskTemplateListPtrSave = G_MQX_init_struct.TASK_TEMPLATE_LIST;
    G_MQX_init_struct.TASK_TEMPLATE_LIST = MATI_MQX_test_template_list_rom;
  }
  /***************************************************************************/
  /**                                                                       **/
  /**   Function Name    : f_mati_restore_tasks_struct                      **/
  /**   Description      : restore MQX global structure pointer to MQX      **/
  /**                      default.                                          **/
  /**   Input parameters : None                                             **/
  /**   Output parameters: None                                             **/
  /**   Return value     : None                                             **/
  /**   Remarks          :                                                  **/
  /***************************************************************************/

  void f_mati_restoreTasksStruct()
  {
    G_MQX_init_struct.TASK_TEMPLATE_LIST = g_TaskTemplateListPtrSave;
  }
#endif
/*
u32_t D_PSP_INLINE f_mati_isGFrameWorkCBValid()
{
  return (G_pFrameWorkCB->uiIsValidData == D_VALID_32_BYTE);
}
*/
void D_PSP_INLINE f_mati_initializeGFrameWorkCB()
{
#if (PROD_MATI_MQX==1)
  f_rtos_memZero((void*)G_pFrameWorkCB, sizeof(S_FRAMEWORK_CONTROL_BLOCK));
#elif (PROD_MATI_AOM==1)
  memset((void*)(G_pFrameWorkCB), (u32_t)0, (u32_t)(sizeof(S_FRAMEWORK_CONTROL_BLOCK)));
#endif
  G_pFrameWorkCB->uiIsValidData = D_VALID_32_BYTE;  // MARKER FLAG - MARKS initialization, flag is preserved between resets.
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_main                                      **/
/**   Description      : MQX entry point                                  **/
/**   Input parameters : who called us (ROM / RAM / MODEL)                **/
/**   Output parameters: None                                             **/
/**   Return value     : None                                             **/
/**   Remarks          : If we are in ROM we will test AOM and MQX with   **/
/**                      self initiation                                  **/
/**                      If we are in RAM then we only assign the MQX     **/
/**                      struct and use the RAM MQX init                  **/
/***************************************************************************/
E_MATI_Result f_mati_main()
{
   //f_mati_test_func_overlay112_vect(0);

   //G_pFrameWorkCB = M_GET_MATI_CB();
   G_testDefinitions.eClearTestData = E_TEST_CONFIG_CLEAR_DATA_ON;
   G_testDefinitions.eIsBtst = E_TEST_CONFIG_BTST_ON;

   /* if this is the first time we are using this global, it would need to be initialized */
   f_mati_initializeGFrameWorkCB();

   #if (PROD_MATI_AOM==1)
      f_mati_mainAomMuxTask(0);
   #else
      f_mati_initializeBtstInputParams(G_pFrameWorkCB);
      f_mati_frameworkStart(G_pFrameWorkCB);
      f_mati_updateTaskStructFromParmas(G_pFrameWorkCB);
      f_mati_asignTasksStruct();
      /* Initialize MQX and start scheduling threads */
      f_rtos_init(&G_MQX_init_struct);// XXX NEVER RETURNS XXX
      f_mati_frameworkWaitForCompletion(G_pFrameWorkCB);
      f_mati_restoreTasksStruct();
   #endif

   return E_MATI_OK;
}

#if (PROD_MATI_MQX==1)
  E_CB_TEST_PROGRESS f_mati_handleTestGroupNone(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
  {
    return E_CB_TEST_PROGRESS_NO_MORE_TESTS;
  }
#endif

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_mainMqxMuxTask                            **/
/**   Description      : MATI main tests functions multiplexer            **/
/**   Input parameters : Test group preconditions index from              **/
/**                      precondition array                               **/
/**   Output parameters: None                                             **/
/**   Return value     : None                                             **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
#if (PROD_MATI_MQX==1)
void f_mati_mainMqxMuxTask(u32_t uiPparam)
{
  f_rtos_icacheInvalidate(NULL);
  _sync();
  //_brk();

  if ( E_MATI_OK == f_mati_start_tasks() )
  {
    G_pFrameWorkCB->eTestGroup = f_mati_get_enabled_test_group( G_pFrameWorkCB);
    for (G_pFrameWorkCB->uiTestCurrent = 0 ; G_pFrameWorkCB->eTestGroup < E_CB_TEST_GROUP_MAX ; G_pFrameWorkCB->uiTestCurrent++)
    {
      // init some generic parameters and exit if fails
      if (f_mati_init_test_task(G_testDefinitions) == RTOS_ERROR)
      {
        //SET( D_SET_MERGE_ERROR_CODE(G_frameWorkCB->eTestGroup, G_frameWorkCB->uiTestCurrent, f_mati_get_error_bits(G_frameWorkCB) ) );/// XXX TODO XXX TODO revise.
        break;
      }

      //M_BTST_ASSERT( ( E_CB_TEST_GROUP_MAX > G_pFrameWorkCB->eTestGroup ) , f_mati_getErrorBits(G_pFrameWorkCB));
      /*       Run The Test            */
      E_CB_TEST_PROGRESS progress = G_lookupTable_mati_test_group_handle[G_pFrameWorkCB->eTestGroup](G_pFrameWorkCB);

      if(E_CB_TEST_PROGRESS_NO_MORE_TESTS == progress)
      {
        G_pFrameWorkCB->uiTestCurrent = -1;//progressed back to 0x0 in for statement
        G_pFrameWorkCB->eTestGroup = f_mati_get_enabled_test_group( G_pFrameWorkCB);
      }

      if(E_TEST_ERROR_NO_ERROR != f_mati_getErrorBits(G_pFrameWorkCB) && G_testDefinitions.eIsBlocking == E_TEST_CONFIG_BLOCKING_ON)
      {/** Note:
        *  After each test validate_test_results() is run and checks for error bits
        *  with respect to negative / positive testing considerations
        */
        //SET( D_SET_MERGE_ERROR_CODE(G_frameWorkCB->eTestGroup, G_frameWorkCB->uiTestCurrent, f_mati_get_error_bits(G_frameWorkCB) ) );/// XXX TODO XXX TODO revise.
        break;
      }
    } // for

    f_mati_frameworkStop(G_pFrameWorkCB);
    //_brk();
    f_rtos_taskMQXExit(0);
  }
	return;
} // main_mux_task

#endif

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_mainAomMuxTask                            **/
/**   Description      : MATI main AOM tests functions multiplexer        **/
/**   Input parameters :                                                  **/
/**   Output parameters: None                                             **/
/**   Return value     : None                                             **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
void f_mati_mainAomMuxTask(u32_t uiParam)
{
  E_CB_TEST_AOM testId = G_pFrameWorkCB->uiTestCurrent;
  if (E_CB_TEST_AOM_MAX > testId )
  {
    f_mati_resetAomGlobal();
    G_lookup_table_mati_test_aom[testId](G_pFrameWorkCB);
    // ronen M_PSP_ASSERT(E_TEST_ERROR_NO_ERROR == f_mati_getErrorBits(G_pFrameWorkCB), f_mati_getErrorBits(G_pFrameWorkCB));
    G_pFrameWorkCB->uiTestCurrent++;
  }
  else
  {
    // we need to reset the global variable in the last iteration
    f_mati_initializeGFrameWorkCB();
  }
}

