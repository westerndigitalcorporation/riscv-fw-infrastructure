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

#ifdef BTST_GROUP_MATI
  #include "pl_btst.h"
#endif
#include "mati.h"
#include "mati_utilities.h"
#include "mati_types.h"
#include "mati_api.h"
#include "psp_api.h"
#include "rtosal_task_api.h"
#include "psp_attributes.h"
///#include "pl_btst.h"
///#include "rom_sfr.h"
///#include "rtos_user_api.h"

#if (PROD_MATI_AOM==1)
  #include "comrv_types.h"
#elif (PROD_MATI_MQX==1)
  #include "rtos_api_pointers.h"
#endif

/***************************************************************************/
/**    DEFINITIONS                                                        **/
/***************************************************************************/

/***************************************************************************/
/**    MACROS                                                             **/
/***************************************************************************/
///#if (PROD_MATI_AOM==1)
///M_PSP_INT_VAR_DECLARE() ; M_PSP_INT_DISABLE();
#define M_MATI_INT_DISABLE(pPrevIntState) pspInterruptsDisable(pPrevIntState)
///M_PSP_INT_RESTORE() ;
#define M_MATI_INT_ENABLE(uiPrevIntState)  pspInterruptsRestore(uiPrevIntState)
///#else
///  #define M_MATI_INT_DISABLE() M_INT_DISABLE()
///  #define M_MATI_INT_ENABLE()  M_INT_ENABLE()
///#endif
/***************************************************************************/
/**    TYPES                                                              **/
/***************************************************************************/

/***************************************************************************/
/**    LOCAL PROTOTYPES                                                   **/
/***************************************************************************/

/***************************************************************************/
/**    EXTERNAL PROTOTYPES                                                **/
/***************************************************************************/
#if (PROD_MATI_AOM==1)
//  extern u32_t f_aom_mem_init (chunks_t* pChunks);
#endif

extern u32_t G_uiTasksId[];

/***************************************************************************/
/**    EXTERNAL GLOBAL PROTOTYPES                                         **/
/***************************************************************************/
#if (PROD_MATI_AOM==1)
  extern S_MATI_AOM_FUNCS_HIT_COUNTER  G_mati_aom_funcs_hit_counter;
#endif

/***************************************************************************/
/**    GLOBAL VARIABLES                                                   **/
/***************************************************************************/
const u32_t uiRandomArr[20] =
{
  231027533,
  554046009,
  376164818,
  342696543,
  898318967,
  703217309,
  916228864,
  337697701,
  654640277,
  835417678,
  265022102,
  145785459,
  345448207,
  8342683,
  307981101,
  796684134,
  736136043,
  669031654,
  519369138,
  613133053
};

/***************************************************************************/
/**    START IMPLEMENTATION                                               **/
/***************************************************************************/

#if (PROD_MATI_MQX==1)

/***************************************************************************/
/**   f_mati_taskGetStackBase                                             **/
/** \brief get the current task's stack base pointer                      **/
/***************************************************************************/
/**  @param None                                                          **/
/**                                                                       **/
/**  @return None                                                         **/
/**                                                                       **/
/***************************************************************************/
void* f_mati_taskGetStackBase(void)
{
  RTOS_TD_STRUCT_PTR  current_td_ptr = f_rtos_taskGetCurrentTaskPtr();
  return current_td_ptr->STACK_BASE;
}


/***************************************************************************/
/**   f_mati_taskGetStackTop                                              **/
/** \brief get the current task's stack top  pointer                      **/
/***************************************************************************/
/**  @param None                                                          **/
/**                                                                       **/
/**  @return None                                                         **/
/**                                                                       **/
/***************************************************************************/
void* f_mati_taskGetStackTop(void)
{
  RTOS_TD_STRUCT_PTR  current_td_ptr = f_rtos_taskGetCurrentTaskPtr();
  return current_td_ptr->STACK_LIMIT;
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_register_test                             **/
/**   Description      : register a function and it parameters            **/
/**                      to random thread from the thread pull.           **/
/**   Input parameters : function pointer                                 **/
/**                      function first parameter                         **/
/**                      function second parameter                        **/
/**                      function third parameter                         **/
/**   Output parameters: none                                             **/
/**   Return value     : the assigned thread bit than is turn on in the   **/
/**                      bitmap                                           **/
/**   Remarks          : It's not acctualy pure randomize, but it is good **/
/**                      enough for now                                   **/
/***************************************************************************/
u08_t f_mati_register_test(pTask pTaskFunctionPointer, E_MATI_TEST_REG_SYNC_POINT uiSyncPoint, u32_t uiParam2, u32_t uiParam3 )
{
  u08_t thread = f_mati_get_random_thread();
  G_pFrameWorkCB->sTestTasksFunctionArr[thread] = pTaskFunctionPointer;
  G_pFrameWorkCB->sTestTasksFunctionParamArr[thread].uiParam1 = uiSyncPoint;
  G_pFrameWorkCB->sTestTasksFunctionParamArr[thread].uiParam2 = uiParam2;
  G_pFrameWorkCB->sTestTasksFunctionParamArr[thread].uiParam3 = uiParam3;
  return thread;
}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_startTestAndWait                          **/
/**   Description      : initiate the test and wait for the tests to end  **/
/**   Input parameters : None                                             **/
/**   Output parameters: None                                             **/
/**   Return value     : None                                             **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
void f_mati_startTestAndWait(u08_t ucUsedThreadsBitmask)
{
  f_mati_setTestStarted(G_pFrameWorkCB);

  if(0 == f_mati_busyWaitSleep((u32_t*)&(G_pFrameWorkCB->eTestProgress), (u32_t)E_CB_TEST_PROGRESS_COMPLETE, D_TIMEOUT_TEST))
  {
    f_mati_setErrorBit(G_pFrameWorkCB, E_TEST_ERROR_TIMEOUT);
  }
  if ( G_pFrameWorkCB->eTestErrorBitmap != E_TEST_ERROR_NO_ERROR )
  {
    SET( D_SET_MERGE_ERROR_CODE(G_pFrameWorkCB->eTestGroup, G_pFrameWorkCB->uiTestCurrent, G_pFrameWorkCB->eTestErrorBitmap ));
  }
  f_mati_release_thread_from_pool(ucUsedThreadsBitmask);
}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_frameworkWaitForCompletion                **/
/**   Description      :                                **/
/**   Input parameters : S_FRAMEWORK_CONTROL_BLOCK_PTR                    **/
/**                      Value that we want to assign                     **/
/**   Output parameters: None                                             **/
/**   Return value     : RTOS_OK in case of success, else RTOS_ERROR      **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
E_MATI_Result /* TODO: fix me D_PSP_ALWAYS_INLINE */ f_mati_frameworkWaitForCompletion( S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{
  E_MATI_Result res = E_MATI_OK;
  if(!f_mati_busyWaitSleep((u32_t*)&(pFrameWorkCB->eIsMatiStarted), (u32_t)E_CB_MATI_RUN_STATUS_NOT_RUNNING, D_TIMEOUT_TEST * E_CB_TEST_GROUP_MAX))
  {
    res = E_MATI_ERROR;
  }
  return res;
}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_busyWaitYield                             **/
/**   Description      : busy wait on condition with yield.               **/
/**   Input parameters : Pointer to WORD variable that we sample          **/
/**                      Value that we compare to the first parameter     **/
/**                      wait timeout in milliseconds                     **/
/**   Output parameters: None                                             **/
/**   Return value     : 0 if return due to timeout, else the             **/
/**                      milliseconds that remain                         **/
/**   Remarks          : the condition is WORD type                       **/
/**                                                                       **/
/***************************************************************************/
u32_t f_mati_busyWaitYield(u32_t* pWaitVariable, u32_t uiWaitValue, u32_t uiTimeOut)
{
  u32_t uiTimeYield = 0;

  while (*pWaitVariable != uiWaitValue && uiTimeOut > 0 )
  {
    uiTimeYield = f_rtos_timeGetTimeMs();

    f_rtos_taskYield();

    uiTimeYield = f_rtos_timeGetTimeMs() - uiTimeYield;

    if ( uiTimeOut > uiTimeYield )
    {
      uiTimeOut -= uiTimeYield;
    }
    else
    {
      uiTimeOut = 0;
    }
  }
  return uiTimeOut;
}



/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_getRandom                                 **/
/**   Description      : return u32_t random number from the MQX kernel    **/
/**   Input parameters : None                                             **/
/**   Output parameters: None                                             **/
/**   Return value     : None                                             **/
/**   Remarks          : in the future replace to SanDisk implementation  **/
/**                                                                       **/
/***************************************************************************/
u32_t f_mati_getRandom()
{
  return (uiRandomArr[f_rtos_timeGetMQXTicksCounter()%20 ]);
}


#endif //(PROD_MATI_MQX==1)


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_busyWaitSleep                             **/
/**   Description      : busy wait on condition with 2 milliseconds       **/
/**                      sleep interval.                                  **/
/**   Input parameters : Pointer to WORD variable that we sample          **/
/**                      Value that we compare to the first parameter     **/
/**                      wait timeout in milliseconds                     **/
/**   Output parameters: None                                             **/
/**   Return value     : 0 if return due to timeout, else the             **/
/**                      milliseconds that remain                         **/
/**   Remarks          : the condition is WORD type                       **/
/**                                                                       **/
/***************************************************************************/
u32_t f_mati_busyWaitSleep(u32_t* pWaitVariable, u32_t uiWaitValue, u32_t uiTimeout)
{

  while (*pWaitVariable != uiWaitValue && uiTimeout > 0 )
  {

    if (uiTimeout > D_TIMEOUT_MICRO )
    {
      ///f_rtos_timeDelayMilliSeconds(D_TIMEOUT_MICRO);
      rtosalTaskSleep(D_TIMEOUT_MICRO);
      uiTimeout -= D_TIMEOUT_MICRO;
    }
    else
    {
      ///f_rtos_timeDelayMilliSeconds(uiTimeout);
      rtosalTaskSleep(uiTimeout);
      uiTimeout = 0;
    }
  }
  return uiTimeout;
}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_validateTestResult                        **/
/**   Description      : check if the test is fail or succeeded           **/
/**   Input parameters : Pointer to WORD shared variable that we assign   **/
/**                      Value that we want to assign                     **/
/**   Output parameters: None                                             **/
/**   Return value     : RTOS_OK in case of success, else RTOS_ERROR      **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
void f_mati_validateTestResult(E_TEST_CONFIG eTestType, S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{
  if ( (eTestType == E_TEST_CONFIG_TEST_POSITIVE_TYPE && pFrameWorkCB->eTestErrorBitmap == E_TEST_ERROR_NO_ERROR) ||
       (eTestType == E_TEST_CONFIG_TEST_NEGATIVE_TYPE && pFrameWorkCB->eTestErrorBitmap != E_TEST_ERROR_NO_ERROR) )
  {
    pFrameWorkCB->eTestErrorBitmap = E_TEST_ERROR_NO_ERROR;
  }
  else
  {
    // if the test failed but the test is negative --> the error bitmap is empty, so assign generic error message
    if (eTestType == E_TEST_CONFIG_TEST_NEGATIVE_TYPE)
    {
      f_mati_setErrorBit(pFrameWorkCB, E_TEST_ERROR_NEGATIVE_FAILED);
    }
  }
  f_mati_setTestCompleted(pFrameWorkCB);
}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_secureSet                                 **/
/**   Description      : secure setter for shared variable                **/
/**   Input parameters : Pointer to WORD shared variable that we assign   **/
/**                      Value that we want to assign                     **/
/**   Output parameters: None                                             **/
/**   Return value     : RTOS_OK in case of success, else RTOS_ERROR      **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
E_MATI_Result f_mati_secureSet(u32_t* pResource, u32_t uiValue)
{
   u32_t uiPrevIntState;
   E_MATI_Result res;

   M_MATI_INT_DISABLE(&uiPrevIntState);
   if (pResource == NULL)
   {
      res = E_MATI_ERROR;
   }
   else
   {
      res = E_MATI_OK;
      *pResource = uiValue;
   }

   M_MATI_INT_ENABLE(uiPrevIntState);

   return res;
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_setrErrorBit                              **/
/**   Description      : secure setter for E_TEST_ERROR                   **/
/**   Input parameters : Pointer to E_TEST_ERROR                          **/
/**                      Value that we want to assign                     **/
/**   Output parameters: None                                             **/
/**   Return value     : E_MATI_OK in case of success, else E_MATI_ERROR  **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
E_MATI_Result f_mati_setErrorBit(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB, E_TEST_ERROR eValue)
{
   u32_t uiPrevIntState;
   E_MATI_Result res;

   if (pFrameWorkCB == NULL)
   {
      res = E_MATI_ERROR;
   }
   else
   {
      res = E_MATI_OK;
   }

   M_MATI_INT_DISABLE(&uiPrevIntState);

   pFrameWorkCB->eTestErrorBitmap |= eValue;

   M_MATI_INT_ENABLE(uiPrevIntState);

   return E_MATI_OK;
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_clearErrorBits                            **/
/**   Description      : secure setter for E_TEST_ERROR                   **/
/**   Input parameters : Pointer to E_TEST_ERROR                          **/
/**                      Value that we want to assign                     **/
/**   Output parameters: None                                             **/
/**   Return value     : RTOS_OK in case of success, else RTOS_ERROR      **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
E_MATI_Result f_mati_clearErrorBits(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{
   u32_t uiPrevIntState;
   M_MATI_INT_DISABLE(&uiPrevIntState);
   pFrameWorkCB->eTestErrorBitmap = E_TEST_ERROR_NO_ERROR;
   M_MATI_INT_ENABLE(uiPrevIntState);
   return E_MATI_OK;
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_getErrorBits                              **/
/**   Description      : secure setter for E_TEST_ERROR                   **/
/**   Input parameters : Pointer to E_TEST_ERROR                          **/
/**                      Value that we want to assign                     **/
/**   Output parameters: None                                             **/
/**   Return value     : RTOS_OK in case of success, else RTOS_ERROR      **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
E_TEST_ERROR /* TODO: fix me D_PSP_ALWAYS_INLINE */ f_mati_getErrorBits(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{
  return pFrameWorkCB->eTestErrorBitmap;
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_frameworkStop                             **/
/**   Description      :                                                  **/
/**   Input parameters : S_FRAMEWORK_CONTROL_BLOCK_PTR                    **/
/**                      Value that we want to assign                     **/
/**   Output parameters: None                                             **/
/**   Return value     : RTOS_OK in case of success, else RTOS_ERROR      **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
E_MATI_Result /* TODO: fix me D_PSP_ALWAYS_INLINE */ f_mati_frameworkStop( S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{
	return f_mati_secureSet((u32_t*)&pFrameWorkCB->eIsMatiStarted, (u32_t)E_CB_MATI_RUN_STATUS_NOT_RUNNING);
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_frameworkStart                            **/
/**   Description      :                                                  **/
/**   Input parameters : S_FRAMEWORK_CONTROL_BLOCK_PTR                    **/
/**                      Value that we want to assign                     **/
/**   Output parameters: None                                             **/
/**   Return value     : RTOS_OK in case of success, else RTOS_ERROR      **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
E_MATI_Result /* TODO: fix me D_PSP_ALWAYS_INLINE */ f_mati_frameworkStart( S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{
  /**
   * Note: This function runs prior to f_rtos_init() thus,
   * not all pointers and resources have been initialized,
   * and calling secure_set() (and in return interrupt disable) will fail.
   **/
  pFrameWorkCB->eIsMatiStarted = (u32_t)E_CB_MATI_RUN_STATUS_RUNNING;
	//return f_mati_secure_set((u32_t*)&frameWorkCB->eIsMatiStarted, (u32_t)E_CB_MATI_RUN_STATUS_RUNNING);
  return E_MATI_OK;
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_frameworkIsRunning                        **/
/**   Description      :                                                  **/
/**   Input parameters : S_FRAMEWORK_CONTROL_BLOCK_PTR                    **/
/**                      Value that we want to assign                     **/
/**   Output parameters: None                                             **/
/**   Return value     : RTOS_OK in case of success, else RTOS_ERROR      **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
E_MATI_Result /* TODO: fix me D_PSP_ALWAYS_INLINE */ f_mati_frameworkIsRunning( S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{
  return (pFrameWorkCB->eIsMatiStarted == E_CB_MATI_RUN_STATUS_RUNNING) ? E_MATI_OK : E_MATI_ERROR;
}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_test_in_progress                          **/
/**   Description      :                                                  **/
/**   Input parameters : S_FRAMEWORK_CONTROL_BLOCK_PTR                    **/
/**                      Value that we want to assign                     **/
/**   Output parameters: None                                             **/
/**   Return value     : RTOS_OK in case of success, else RTOS_ERROR      **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
E_MATI_Result /* TODO: fix me D_PSP_ALWAYS_INLINE */ f_mati_test_in_progress(S_FRAMEWORK_CONTROL_BLOCK_PTR frameWorkCB)
{
  return  (E_CB_TEST_PROGRESS_COMPLETE != frameWorkCB->eTestProgress ? E_MATI_OK: E_MATI_ERROR) ;
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_setTestProgress                           **/
/**   Description      :                                                  **/
/**   Input parameters : S_FRAMEWORK_CONTROL_BLOCK_PTR                    **/
/**                      Value that we want to assign                     **/
/**   Output parameters: None                                             **/
/**   Return value     : RTOS_OK in case of success, else RTOS_ERROR      **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
E_MATI_Result /* TODO: fix me D_PSP_ALWAYS_INLINE */ f_mati_setTestProgress(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB, E_CB_TEST_PROGRESS eProgress)
{
   u32_t uiPrevIntState;
   M_MATI_INT_DISABLE(&uiPrevIntState);
   pFrameWorkCB->eTestProgress = eProgress;
   M_MATI_INT_ENABLE(uiPrevIntState);
   return E_MATI_OK;
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_setTestCompleted                          **/
/**   Description      :                                                  **/
/**   Input parameters : S_FRAMEWORK_CONTROL_BLOCK_PTR                    **/
/**                     E_CB_TEST_PROGRESS Value that we want to assign   **/
/**   Output parameters: None                                             **/
/**   Return value     : RTOS_OK in case of success, else RTOS_ERROR      **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
E_MATI_Result /* TODO: fix me D_PSP_ALWAYS_INLINE */ f_mati_setTestCompleted(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{
  return f_mati_setTestProgress(pFrameWorkCB, E_CB_TEST_PROGRESS_COMPLETE);
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_setTestStarted                            **/
/**   Description      :                                                  **/
/**   Input parameters : S_FRAMEWORK_CONTROL_BLOCK_PTR                    **/
/**                     E_CB_TEST_PROGRESS Value that we want to assign   **/
/**   Output parameters: None                                             **/
/**   Return value     : RTOS_OK in case of success, else RTOS_ERROR      **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
E_MATI_Result /* TODO: fix me D_PSP_ALWAYS_INLINE */ f_mati_setTestStarted(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{
  return f_mati_setTestProgress(pFrameWorkCB, E_CB_TEST_PROGRESS_RUNNING);
}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_resetAomGlobal                            **/
/**   Description      : reset the test aom global struct and call reset  **/
/**   Input parameters : None                                             **/
/**   Output parameters: None                                             **/
/**   Return value     : None                                             **/
/**   Remarks          : None                                             **/
/**                                                                       **/
/***************************************************************************/
#if (PROD_MATI_AOM==1)

void f_mati_resetAomGlobal()
{
  memset((u08_t*)&G_mati_aom_funcs_hit_counter, 0, sizeof(G_mati_aom_funcs_hit_counter) );
  //f_sysSwResetToController(0xFFFF,0xFFFF,0,NULL);
}

#endif //(PROD_MATI_AOM==1)

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_initializeBtstInputParams                 **/
/**   Description      : extract the btst params                          **/
/**   Input parameters : pointer to control block variable                **/
/**   Output parameters: None                                             **/
/**   Return value     : None                                             **/
/**   Remarks          : None                                             **/
/**                                                                       **/
/***************************************************************************/
void f_mati_initializeBtstInputParams(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{
  pFrameWorkCB->sBtstParams.usBtstParam16  = 0x1234; // TODO: was f_plBtst_getTestRandomSeed();
  pFrameWorkCB->sBtstParams.uiBtstParam32  = 0x1234; // TODO: was f_plBtst_getTestRandomSeed_32();
  pFrameWorkCB->sBtstParams.ullBtstParam64 = 0x1234; // TODO: was f_plBtst_getTestRandomSeed_64();
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_getInputParams                            **/
/**   Description      : extract the btst params                          **/
/**   Input parameters : pointer to control block variable                **/
/**   Output parameters: None                                             **/
/**   Return value     : None                                             **/
/**   Remarks          : None                                             **/
/**                                                                       **/
/***************************************************************************/
S_FRAMEWORK_BTSTS_PARAMS_PTR f_mati_getInputParams(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{
  return (&pFrameWorkCB->sBtstParams);
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_resetTimer                                **/
/**   Description      : reset specific timer                             **/
/**   Input parameters : timer index to reset                             **/
/**   Output parameters: None                                             **/
/**   Return value     : 0 in case of success, else 1                     **/
/**   Remarks          : None                                             **/
/**                                                                       **/
/***************************************************************************/
u08_t f_mati_resetTimer(u08_t ucTimerToReset)
{
   u08_t ucRes = 0;
#if 0 /* ronen */
   if (ucTimerToReset != D_TIMER0_ID)
   {
      M_PSP_SET_AUX_REG(0x2,        D_TIMER0_CONTROL);
      M_PSP_SET_AUX_REG(0xFFFFFFFF, D_TIMER0_LIMIT);
      M_PSP_SET_AUX_REG(0,          D_TIMER0_COUNT);
   }
   else if(ucTimerToReset != D_TIMER1_ID)
   {
      M_PSP_SET_AUX_REG(0x2,        D_TIMER1_CONTROL);
      M_PSP_SET_AUX_REG(0xFFFFFFFF, D_TIMER1_LIMIT);
      M_PSP_SET_AUX_REG(0,          D_TIMER1_COUNT);
   }
   else
   {
      ucRes = 1;
   }
#endif /* 0 ronen */
   return ucRes;
}



/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_getTime                                   **/
/**   Description      :                                                  **/
/**   Input parameters : None                                             **/
/**   Output parameters: ulCounter_p - time continer                      **/
/**   Return value     : None                                             **/
/**   Remarks          : none                                             **/
/**                                                                       **/
/***************************************************************************/
void f_mati_getTime(u64_t *ulCounter_p)
{
   /// TODO: f_romHwSfr_getTimestampInUsec_rom(ulCounter_p);

   //return counter.lowBits;
   //return M_CPU_GET_AUX_REG(D_INT_TEST_PSP_AUX_TCOUNT0);
   //return M_CPU_GET_AUX_REG(D_INT_TEST_PSP_AUX_TCOUNT1);
   //return M_FA_SET_TBU_GET_TIMER_FULL_LSB() /*| M_FA_SET_TBU_GET_TIMER_FULL_MSB()*/;
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_advanceULCounter                            **/
/**   Description      : advance global generic counter                   **/
/**   Input parameters : None                                             **/
/**   Output parameters: None                                             **/
/**   Return value     : None                                             **/
/**   Remarks          : none                                             **/
/**                                                                       **/
/***************************************************************************/
void f_mati_advanceULCounter(u64_t* ulCounter)
{
   *ulCounter++;
#if 0 /* ronen */
   if (ulCounter->lowBits == MAX_UINT32)
   {
      ulCounter->lowBits = 0;
      ulCounter->highBits += 1;
   }
   else
   {
      ulCounter->lowBits += 1;
   }
#endif /* 0 ronen */
   if (*ulCounter == 0xFFFFFFFFFFFFFFFF /* TODO */)
   {
      M_PSP_EBREAK();
   }
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_resetCounter                              **/
/**   Description      : reset global generic counter                     **/
/**   Input parameters : None                                             **/
/**   Output parameters: None                                             **/
/**   Return value     : None                                             **/
/**   Remarks          : none                                             **/
/**                                                                       **/
/***************************************************************************/
void f_mati_resetULCounter(u64_t* ulCounter)
{
///   ulCounter->highBits = 0;
///   ulCounter->lowBits = 0;
   *ulCounter = 0;
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : SET                                              **/
/**   Description      : STUB SET function                                **/
/**   Input parameters : None                                             **/
/**   Output parameters: None                                             **/
/**   Return value     : None                                             **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
void SET(u32_t setMsg)
{
  ;
}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_assert                                    **/
/**   Description      : replace fa assert function                       **/
/**   Input parameters : None                                             **/
/**   Output parameters: None                                             **/
/**   Return value     : None                                             **/
/**   Remarks          : increment CB shared memory variable              **/
/**                                                                       **/
/***************************************************************************/
void f_mati_assert(void)
{
  G_pFrameWorkCB->uiTestSharedMemory++;
}

