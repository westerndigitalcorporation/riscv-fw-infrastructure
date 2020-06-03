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

/***************************************************************************/
/**         MULTIPLE INCLUSION                                            **/
/***************************************************************************/
#ifndef __MATI_ERRORS_H
#define __MATI_ERRORS_H


/***************************************************************************/
/**         INCLUDES                                                      **/
/***************************************************************************/
#include "mati_api.h"

/***************************************************************************/
/**         DEFINITIONS                                                   **/
/***************************************************************************/

/***************************************************************************/
/**    MACROS                                                             **/
/***************************************************************************/

/***************************************************************************/
/**         TYPEDEFS                                                      **/
/***************************************************************************/

/***************************************************************************/
/**    EXPORTED GLOBALS                                                   **/
/***************************************************************************/

/***************************************************************************/
/**         FUNCTIONS PROTOTYPES                                          **/
/***************************************************************************/

/***************************************************************************/
/*            ERRORS                                                       */
/***************************************************************************/
typedef enum 
{
  E_TEST_ERROR_NO_ERROR                             = 0x0,
  E_TEST_ERROR_INIT                                 = (0x1 << 0),
  E_TEST_ERROR_GENERIC                              = (0x1 << 1),
  E_TEST_ERROR_NEGATIVE_FAILED                      = (0x1 << 2),
  E_TEST_ERROR_TIMEOUT                              = (0x1 << 3),
                                                    
  //ERROR_LWEVENT                                   
  E_TEST_ERROR_LWEVENT_MISSMATCH_MASK               = (0x1 << 4),
  E_TEST_ERROR_LWEVENT_LOCK_WITHOUT_TIMEOUT         = (0x1 << 5),
  E_TEST_ERROR_LWEVENT_RELEASE_WITHOUT_TIMEOUT      = (0x1 << 6),
  E_TEST_ERROR_LWEVENT_LOCK_WITH_TIMEOUT            = (0x1 << 7),
  E_TEST_ERROR_LWEVENT_RELEASE_WITH_TIMEOUT         = (0x1 << 8),
  E_TEST_ERROR_LWEVENT_RELASE_WITHALL               = (0x1 << 9),
  E_TEST_ERROR_LWEVENT_CLEAR_ERROR                  = (0x1 << 10),

  /* TEST_GROUP_QUEUE */
  E_TEST_ERROR_QUEUE_ENQUEUE_FAIED                  = (0x1 << 4),
  E_TEST_ERROR_QUEUE_DEQUEUE_FAILED                 = (0x1 << 5),
  E_TEST_ERROR_QUEUE_ENQUEUE_TO_FULL_FAIED          = (0x1 << 6),
  E_TEST_ERROR_QUEUE_DEQUEUE_FROM_EMPTY_FAILED      = (0x1 << 7),
  E_TEST_ERROR_QUEUE_INSERT_FAILED                  = (0x1 << 8),
  E_TEST_ERROR_QUEUE_INSERT_TO_FULL_FAILED          = (0x1 << 9),
  E_TEST_ERROR_QUEUE_UNLINK_FAIL                    = (0x1 << 10),
  E_TEST_ERROR_QUEUE_UNLINK_FROM_EMPTY_FAILED       = (0x1 << 11),
  E_TEST_ERROR_QUEUE_IS_EMPTY_FAILED                = (0x1 << 12),
  E_TEST_ERROR_QUEUE_SIZE_FAILED                    = (0x1 << 13),
                                                    
  /* TEST_GROUP_MUTEX */                            
  E_TEST_ERROR_MUTEX_LOCK_FAILED                    = (0x1 << 4),
  E_TEST_ERROR_MUTEX_RELEASE_FAILED                 = (0x1 << 5),
  E_TEST_ERROR_MUTEX_WAIT_COUNT_FAILED              = (0x1 << 6),
  E_TEST_ERROR_MUTEX_SCHED_INHERIT_FAILED           = (0x1 << 7),
  E_TEST_ERROR_MUTEX_SCHED_PROTECTION_FAILED        = (0x1 << 8),
  E_TEST_ERROR_MUTEX_QUEUE_WAITING_FAILED           = (0x1 << 9),
  E_TEST_ERROR_MUTEX_PRIORITY_QUEUE_WAITING_FAILED  = (0x1 << 10),

  /* TEST_GROUP_FAST_MESSAGE_QUEUE */
  E_TEST_ERROR_FMQUEUE_GET_FREE_FAILED              = (0x1 << 4),
  E_TEST_ERROR_FMQUEUE_PUT_FREE_FAILED              = (0x1 << 5),
  E_TEST_ERROR_FMQUEUE_SEND_FAILED                  = (0x1 << 6),
  E_TEST_ERROR_FMQUEUE_RECEIVE_FAILED               = (0x1 << 7),
  E_TEST_ERROR_FMQUEUE_IS_EMPTY_FAILED              = (0x1 << 8),
  E_TEST_ERROR_FMQUEUE_SIZE_FAILED                  = (0x1 << 9),
  E_TEST_ERROR_FMQUEUE_RECEIVE_WRONG_VALUE_FAILED   = (0x1 << 10),

  /* TEST_GROUP_LIGHT_WEIGHT_SEMAPHORE */
  E_TEST_ERROR_LWSEM_WAIT_FAILED                    = (0x1 << 4),
  E_TEST_ERROR_LWSEM_POST_FAILED                    = (0x1 << 5),
  E_TEST_ERROR_LWSEM_TIMEOUT_FAILED                 = (0x1 << 6),

  /* TEST_GROUP_LIGHT_WEIGHT_MEMORY */
  E_TEST_ERROR_LWMEM_ALOC_FAILED                    = (0x1 << 4),
  E_TEST_ERROR_LWMEM_FREE_FAILED                    = (0x1 << 5),
  E_TEST_ERROR_LWMEM_SECURE_FREE_FAILED             = (0x1 << 6),

  /* TEST_GROUP_AFFINITY */
  E_TEST_ERROR_AFFINITY_ACQUIRE_FAILED              = (0x1 << 4),
  E_TEST_ERROR_AFFINITY_RELEASE_FAILED              = (0x1 << 5),
  E_TEST_ERROR_AFFINITY_REACQUIRE_FAILED            = (0x1 << 6),
  E_TEST_ERROR_AFFINITY_BANK_MISMATCH               = (0x1 << 7),

  /* TEST_GROUP_ISR */
  E_TEST_ERROR_UISR_INIT_FAILED                     = (0x1 << 4),
  E_TEST_ERROR_KISR_INIT_FAILED                     = (0x1 << 5),
  E_TEST_ERROR_ISR_FAILED                           = (0x1 << 6),

  /* TEST_GROUP_AOM */
  E_TEST_ERROR_AOM_SANITY_FAILED                    = (0x1 << 4),
  E_TEST_ERROR_AOM_UNIFORM_GROUPS_FAILED            = (0x1 << 5),
  E_TEST_ERROR_AOM_NON_UNIFORM_GROUPS_FAILED        = (0x1 << 6),
  E_TEST_ERROR_AOM_MULTI_GROUPS_FAILED              = (0x1 << 7),
  E_TEST_ERROR_AOM_LOCK_UNLOCK_FAILED               = (0x1 << 8),
  E_ERROR_AOM_DEFRAG_FAILED                         = (0x1 << 9),
  E_TEST_ERROR_AOM_OVL_CRC_ERROR                    = (0x1 << 10),
  E_ERROR_AOM_CRITICAL_SECTION_FAILED               = (0x1 << 11),
  E_TEST_ERROR_AOM_THREAD_SAFE_READ_BACK_STORE      = (0x1 << 12),
  E_TEST_ERROR_AOM_FAILED                           = (0x1 << 13),

  E_TEST_ERROR_AFFINITY_REACQUIRE_EDGE = 0xdeadbeef // enforcing struct alignment: ensure enum size is U_32
}E_TEST_ERROR;

#endif /* EOF */

