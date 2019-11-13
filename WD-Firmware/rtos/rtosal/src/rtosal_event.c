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
* @file   rtosal_event.c
* @author Ronen Haen
* @date   21.01.2019 
* @brief  The file implements the RTOS AL event API
* 
*/

/**
* include files
*/
#include "rtosal_event_api.h"
#include "rtosal_macros.h"
#include "rtosal.h"
#include "psp_api.h"
#ifdef D_USE_FREERTOS
   #include "event_groups.h"
#else
   #error "Add appropriate RTOS definitions"
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
* Create an event group
*
* @param pRtosalEventGroupCb   - pointer to event group control block
* @param pRtosalEventGroupName - string of the group name (for debuging)
*
* @return u32_t            - D_RTOSAL_SUCCESS
*                          - D_RTOSAL_GROUP_ERROR - the group CB is invalid or been used
*                          - D_RTOSAL_CALLER_ERR - the caller can not call this function 
*/
RTOSAL_SECTION u32_t rtosalEventGroupCreate(rtosalEventGroup_t* pRtosalEventGroupCb, s08_t* pRtosalEventGroupName)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalEventGroupCb, pRtosalEventGroupCb == NULL, D_RTOSAL_GROUP_ERROR);

#ifdef D_USE_FREERTOS
   /* unused parameter with FreeRTOS */
   (void)pRtosalEventGroupName;
   /* create the event group */
   pRtosalEventGroupCb->eventGroupHandle = xEventGroupCreateStatic((StaticEventGroup_t*)pRtosalEventGroupCb->cEventGroupCB);

   if (pRtosalEventGroupCb->eventGroupHandle != NULL)
   {
      uiRes = D_RTOSAL_SUCCESS;
   }
   else
   {
      uiRes = D_RTOSAL_GROUP_ERROR;
   }

#elif D_USE_THREADX

   /* create the event group */
   // TODO:
   // uiRes = add call to threadx create flags API

#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Destroy a specific event group
*
* @param pRtosalEventGroupCb - pointer to event group control block to be deleted
*
* @return u32_t           - D_RTOSAL_SUCCESS
*                         - D_RTOSAL_GROUP_ERROR - the group CB is invalid or been used
*                         - D_RTOSAL_CALLER_ERR - the caller can not call this function 
*/
RTOSAL_SECTION u32_t rtosalEventGroupDestroy(rtosalEventGroup_t* pRtosalEventGroupCb)
{
   u32_t uiRes;

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalEventGroupCb, pRtosalEventGroupCb == NULL, D_RTOSAL_GROUP_ERROR);
   
#ifdef D_USE_FREERTOS
   vEventGroupDelete(pRtosalEventGroupCb->eventGroupHandle);
   uiRes = D_RTOSAL_SUCCESS;
#elif D_USE_THREADX
   #error "Add THREADX appropriate definitions"
#else
   #error "Add appropriate RTOS definitions"
#endif

   return uiRes;
}

/**
* Set the event bits of a specific event group
*
* @param pRtosalEventGroupCb  - pointer to event group control block to set the event bits
* @param stSetRtosalEventBits - value of the event bits vector to set
* @param uiSetOption          - one of the values: D_RTOSAL_AND or D_RTOSAL_OR
* @param pRtosalEventBits     - pointer to the value of the event bits vector
*
* @return u32_t          - D_RTOSAL_SUCCESS
*                        - D_RTOSAL_GROUP_ERROR - the group CB is invalid or been used 
*                        - D_RTOSAL_OPTION_ERROR - bad param on uiSetOption
*                        - D_RTOSAL_FAIL - fail to set event
*/
RTOSAL_SECTION u32_t rtosalEventGroupSet(rtosalEventGroup_t* pRtosalEventGroupCb,
                          rtosalEventBits_t stSetRtosalEventBits,
                          u32_t uiSetOption, rtosalEventBits_t* pRtosalEventBits)
{
   u32_t uiRes;
#ifdef D_USE_FREERTOS
   /* specify if a context switch is needed as a uiResult calling FreeRTOS ...ISR function */
   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */
   
   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalEventGroupCb, pRtosalEventGroupCb == NULL, D_RTOSAL_GROUP_ERROR);
   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalEventBits, pRtosalEventBits == NULL, D_RTOSAL_GROUP_ERROR);

#ifdef D_USE_FREERTOS
   /* rtosalEventGroupSet invoked from an ISR context */
   if (pspIsInterruptContext() == D_PSP_INT_CONTEXT)
   {
      uiRes = xEventGroupSetBitsFromISR(pRtosalEventGroupCb->eventGroupHandle,
                                      stSetRtosalEventBits, &xHigherPriorityTaskWoken);
      if (uiRes == pdPASS)
      {
         uiRes = D_RTOSAL_SUCCESS;
      }
      else
      {
         /* The message could not be sent */
         uiRes = D_RTOSAL_FAIL;
      }
      /* due to the set bit/s we got an indicating that a context switch
         should be requested before the interrupt exits */ 
      if (uiRes == D_RTOSAL_SUCCESS && xHigherPriorityTaskWoken == pdTRUE)
      {
         rtosalContextSwitchIndicationSet();
      }
   }
   else
   {   
      *pRtosalEventBits = xEventGroupSetBits(pRtosalEventGroupCb->eventGroupHandle, stSetRtosalEventBits);
      uiRes = D_RTOSAL_SUCCESS;
   }  
#elif D_USE_THREADX
   // TODO:
   // uiRes = add call to ThreadX set flags API
   /* if we need to retrieve the set event bits */
   if (uiRes == D_RTOSAL_SUCCESS)
   {
      // uiRes = add call to ThreadX set flags API
   }
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

   return uiRes;
}

/**
* Retrieve the event bits of a specific event group
*
* @param pRtosalEventGroupCb - pointer to event group control block to retrieve
*                              the bits from
* @param pRtosalEventBits    - pointer to the value of the RTOS event bits vector
* @param uiRetrieveEvents    - event bits vector to retrieve
* @param uiRetrieveOption    - one of the values: D_RTOSAL_AND, D_RTOSAL_AND_CLEAR,
*                              D_RTOSAL_OR or D_RTOSAL_OR_CLEAR; using the 'AND' will wait for all bits;
*                              using the 'OR' will wait for at least one bit to be set;
*                              using the 'CLEAR' will clear the bit when read.
* @param uiWaitTimeoutTicks - wait value if event vector bits are not set;
*                             can be one of the values: D_RTOSAL_NO_WAIT, D_RTOSAL_WAIT_FOREVER
*                             or timer ticks value
*
* @return u32_t          - D_RTOSAL_SUCCESS
*                        - D_RTOSAL_DELETED - The event was already deleted when this api was called 
*                        - D_RTOSAL_NO_EVENTS - time out while waiting for the event 
*                        - D_RTOSAL_WAIT_ABORTED - aborted by different consumer (like other thread)
*                        - D_RTOSAL_GROUP_ERROR - the ptr in the group CB is invalid
*                        - D_RTOSAL_PTR_ERROR - bad pRtosalEventBits
*                        - D_RTOSAL_WAIT_ERROR - illegal use of wait (wait can be used only from thread)
*                        - D_RTOSAL_OPTION_ERROR - bad uiRetrieveOption
*/
RTOSAL_SECTION u32_t rtosalEventGroupGet(rtosalEventGroup_t* pRtosalEventGroupCb, u32_t uiRetrieveEvents,
                          rtosalEventBits_t* pRtosalEventBits, u32_t uiRetrieveOption,
                          u32_t uiWaitTimeoutTicks)
{
   u32_t uiRes;
#ifdef D_USE_FREERTOS
   BaseType_t xClearOnExit;
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */

   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalEventGroupCb, pRtosalEventGroupCb == NULL, D_RTOSAL_GROUP_ERROR);

#ifdef D_USE_FREERTOS
   M_RTOSAL_VALIDATE_FUNC_PARAM(pRtosalEventBits, pRtosalEventBits == NULL, D_RTOSAL_GROUP_ERROR);

   /* invoked from an ISR context */
   if (pspIsInterruptContext() == D_PSP_INT_CONTEXT)
   {
      *pRtosalEventBits = xEventGroupGetBitsFromISR(pRtosalEventGroupCb->eventGroupHandle);
   }
   else
   {
      /* determine if event bits will be cleared in the event group before 
         xEventGroupWaitBits() returns */
      if (uiRetrieveOption == D_RTOSAL_AND_CLEAR || uiRetrieveOption == D_RTOSAL_OR_CLEAR)
      {
	      xClearOnExit = pdTRUE;
      }
      else
      {
	      xClearOnExit = pdFALSE;
      }
      /* determine if to wait for all the event bits */
      if (uiRetrieveOption == D_RTOSAL_AND_CLEAR || uiRetrieveOption == D_RTOSAL_AND)
      {
         uiRetrieveOption = pdTRUE;
      }
      else
      {
         uiRetrieveOption = pdFALSE;
      }
      /* call FreeRTOS interface */
      *pRtosalEventBits =  xEventGroupWaitBits(pRtosalEventGroupCb->eventGroupHandle,
                                             (EventBits_t)uiRetrieveEvents,
                                             xClearOnExit, uiRetrieveOption,
                                             (TickType_t)uiWaitTimeoutTicks);
   }
   uiRes = D_RTOSAL_SUCCESS;
#elif D_USE_THREADX
   #error "Add appropriate THREADX definitions"
#else
   #error "Add appropriate RTOS definitions"
#endif /* #ifdef D_USE_FREERTOS */
   return uiRes;
}
