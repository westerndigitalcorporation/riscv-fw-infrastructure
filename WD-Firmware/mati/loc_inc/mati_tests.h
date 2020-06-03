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
#ifndef __MATI_TESTS_H
#define __MATI_TESTS_H


/***************************************************************************/
/**         INCLUDES                                                      **/
/***************************************************************************/
#include "mati.h"
#include "mati_types.h"
#include "comrv_defines.h"

/***************************************************************************/
/**    MACROS                                                             **/
/***************************************************************************/
#define M_INCREMENT(x) (x + 1)
#define M_DECREMENT(x) (x - 1)


/***************************************************************************/
/**         DEFINITIONS                                                   **/
/***************************************************************************/
#define D_OVERLAY_GROUP_FUNCTION1 1
#define D_OVERLAY_GROUP_FUNCTION2 3
#define D_OVERLAY_GROUP_FUNCTION3 2

// FMQueue
#define D_QUEUE_NUMBER_OF_MESSAGES 0xA


#define D_USER_ISR     0
#define D_KERNEL_ISR   1
#define D_BLOCKING_ISR 2

/***************************************************************************/
/**         TYPEDEFS                                                      **/
/***************************************************************************/
typedef enum
{
  E_MATI_TEST_HW_STACK_PRIMARY = 0,
  E_MATI_TEST_HW_STACK_THREAD_1,
  E_MATI_TEST_HW_STACK_THREAD_2,
  E_MATI_TEST_HW_STACK_THREAD_3,
  E_MATI_TEST_HW_STACK_THREAD_NONE,
  E_MATI_TEST_HW_STACK_THREAD_MAX = E_MATI_TEST_HW_STACK_THREAD_NONE
}E_MATI_TEST_HW_STACK;

/***************************************************************************/
/**    EXPORTED GLOBALS                                                   **/
/***************************************************************************/

/***************************************************************************/
/**         FUNCTIONS PROTOTYPES                                          **/
/***************************************************************************/
u08_t f_mati_register_test(pTask task_function_pointer, E_MATI_TEST_REG_SYNC_POINT uiSyncPoint, u32_t param2, u32_t param3 );
#ifdef PROD_BTST_ENABLED
  E_BTST_TEST_RESULT f_btstMati_testMQX(void);
  E_BTST_TEST_RESULT f_btstMati_testAOM(void);
#endif

//************************ tests groups function pointers  ************************//
E_CB_TEST_PROGRESS f_mati_handleTestGroupLwevents(S_FRAMEWORK_CONTROL_BLOCK_PTR frameWorkCB_p);
E_CB_TEST_PROGRESS f_mati_handleTestGroupQueue(S_FRAMEWORK_CONTROL_BLOCK_PTR frameWorkCB_p);
E_CB_TEST_PROGRESS f_mati_handleTestGroupMutex(S_FRAMEWORK_CONTROL_BLOCK_PTR frameWorkCB_p);
E_CB_TEST_PROGRESS f_mati_handleTestGroupFMQueue(S_FRAMEWORK_CONTROL_BLOCK_PTR frameWorkCB_p);
E_CB_TEST_PROGRESS f_mati_handleTestGroupLWSemaphore(S_FRAMEWORK_CONTROL_BLOCK_PTR frameWorkCB_p);
E_CB_TEST_PROGRESS f_mati_handleTestGroupLWMemry(S_FRAMEWORK_CONTROL_BLOCK_PTR frameWorkCB_p);
E_CB_TEST_PROGRESS f_mati_handleTestGroupISR(S_FRAMEWORK_CONTROL_BLOCK_PTR frameWorkCB_p);
E_CB_TEST_PROGRESS f_mati_handleTestGroupAffinity(S_FRAMEWORK_CONTROL_BLOCK_PTR frameWorkCB_p) ;
E_CB_TEST_PROGRESS f_mati_handleTestGroupHWStack(S_FRAMEWORK_CONTROL_BLOCK_PTR frameWorkCB_p);
E_CB_TEST_PROGRESS f_mati_handleTestGroupNone(S_FRAMEWORK_CONTROL_BLOCK_PTR frameWorkCB_p) ;


//************************ tests registration functions prototypes ************************//
// LWEvent

u08_t f_mati_register_test_lwevent_wait_private_not_overlapping_bitmask();
u08_t f_mati_register_test_lwevent_wait_global_not_overlapping_bitmask();
u08_t f_mati_register_test_lwevent_wait_private_overlapping_bitmask();
u08_t f_mati_register_test_lwevent_wait_private_with_timeout_bitmask();
u08_t f_mati_register_test_lwevent_waitAll_private();



// Queue
u08_t f_mati_register_test_queue_enqueue_dequeue();
u08_t f_mati_register_test_queue_insert_unlink();
u08_t f_mati_register_test_queue_is_empty();
u08_t f_mati_register_test_queue_size();

// Mutex
u08_t f_mati_register_test_mutex_lock_and_release();
u08_t f_mati_register_test_mutex_non_blocking_lock();
u08_t f_mati_register_test_mutex_wait_count();
u08_t f_mati_register_test_mutex_schedual_priority_inharitance();
u08_t f_mati_register_test_mutex_schedual_priority_protection();
u08_t f_mati_register_test_mutex_wait_protocol_queue();
u08_t f_mati_register_test_mutex_wait_protocol_priority_queue();

// FMQueue
u08_t f_mati_register_test_fmqueue_send_recieve();
u08_t f_mati_register_test_fmqueue_send_queue_ends();
u08_t f_mati_register_test_fmqueue_recieve_queue_ends();
u08_t f_mati_register_test_fmqueue_signal();
u08_t f_mati_register_test_fmqueue_is_empty_and_size();

// LWSem
u08_t f_mati_register_test_lwsem_wait_post_without_timeout_semaphore_size_equal_to_threads();
u08_t f_mati_register_test_lwsem_wait_post_without_timeout_semaphore_size_more_than_threads();
u08_t f_mati_register_test_lwsem_wait_post_without_timeout_semaphore_size_less_than_threads();
u08_t f_mati_register_test_lwsem_wait_post_with_timeout();
u08_t f_mati_register_test_lwsem_poll();

// LWMem
u08_t f_mati_register_test_lwmemory_alloc_and_secure_free();
u08_t f_mati_register_test_lwmemory_alloc_and_free();

// ISR
u08_t f_mati_register_test_isr_user();
u08_t f_mati_register_test_isr_kernel();
u08_t f_mati_register_test_blocking_isr(void);
void test(void);

// AOM
#if (PROD_MATI_AOM==1)
  void f_mati_aom_run_test_from_second_thread();
#endif

// Affinity
u08_t f_mati_register_test_affinity_main();
u08_t f_mati_register_test_affinity_bank_race();
u08_t f_mati_register_test_affinity_contex_switch_timing();

// hw stack
u08_t f_mati_register_test_hwstack_main();


//************************ tests functions prototypes ************************//

// LWEvent
E_TEST_ERROR f_mati_test_lwevent_init(void);
E_TEST_ERROR f_mati_test_lwevent_wait_manager_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_lwevent_wait_test_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_lwevent_wait_with_timeout_manager_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params) ;
E_TEST_ERROR f_mati_test_lwevent_wait_with_timeout_test_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params) ;

// Queue
E_TEST_ERROR f_mati_test_queue_enqueue_dequeue(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_queue_insert_unlink(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_queue_is_empty(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_queue_size(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);

// Mutex
E_TEST_ERROR f_mati_test_mutex_manager_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_mutex_lock_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_mutex_wait_count_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_mutex_protocol_manager_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_mutex_protocol_lock_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);

// FMQueue
E_TEST_ERROR f_mati_test_fmqueue_manager_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_fmqueue_worker_producer_test_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_fmqueue_worker_consumer_test_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
//void mati_test_fmqueue_worker_producer_with_signal_test_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_fmqueue_worker_consumer_with_signal_test_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_fmqueue_worker_test_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);

// LWSem
E_TEST_ERROR f_mati_test_lwsem_manager_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_lwsem_worker_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_lwsem_worker_with_timeout_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);

// LWMEM
E_TEST_ERROR f_mati_test_lwmem_manager(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_lwmem_worker(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);

// ISR
E_TEST_ERROR f_mati_test_isr_manager_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_isr_worker_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);


// AOM
typedef int (*mati_test_aom)(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR);
extern const mati_test_function_ptr G_lookup_table_mati_test_aom[E_CB_TEST_AOM_MAX];
void f_mati_test_eviction(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
void f_mati_aom_run_test_from_second_thread();
void f_mati_thread_safe_test_wrapper();
void f_mati_raise_swi_for_thread_safe_test_and_yield();

// Affinity
E_TEST_ERROR f_mati_test_affinity_worker_bank_race_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_affinity_main_worker_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_affinity_main_manager_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_affinity_worker_context_switch_timing_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);

// HW Stack
E_TEST_ERROR f_mati_test_hw_stack_manager_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);
E_TEST_ERROR f_mati_test_hw_stack_worker_thread(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);



/// ----------------------------------------------------------------
/// AOM functions defs
/// ----------------------------------------------------------------
#define OVL_f_mati_test_func_overlay101_vect _OVERLAY_
#define OVL_f_mati_test_func_overlay102_vect _OVERLAY_
#define OVL_f_mati_test_func_overlay103_vect _OVERLAY_
#define OVL_f_mati_test_func_overlay104_vect _OVERLAY_
#define OVL_f_mati_test_func_overlay105_vect _OVERLAY_
#define OVL_f_mati_test_func_overlay106_vect _OVERLAY_
#define OVL_f_mati_test_func_overlay107_vect _OVERLAY_
#define OVL_f_mati_test_func_overlay108_vect _OVERLAY_
#define OVL_f_mati_test_func_overlay109_vect _OVERLAY_
#define OVL_f_mati_test_func_overlay110_vect _OVERLAY_
#define OVL_f_mati_test_func_overlay111_vect _OVERLAY_
#define OVL_f_mati_test_func_overlay112_vect _OVERLAY_

/// ----------------------------------------------------------------
/// AOM functions prototypes
/// ----------------------------------------------------------------
void OVL_f_mati_test_func_overlay101_vect f_mati_test_func_overlay101_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void OVL_f_mati_test_func_overlay102_vect f_mati_test_func_overlay102_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void OVL_f_mati_test_func_overlay103_vect f_mati_test_func_overlay103_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void OVL_f_mati_test_func_overlay104_vect f_mati_test_func_overlay104_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void OVL_f_mati_test_func_overlay105_vect f_mati_test_func_overlay105_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void OVL_f_mati_test_func_overlay106_vect f_mati_test_func_overlay106_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void OVL_f_mati_test_func_overlay107_vect f_mati_test_func_overlay107_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void OVL_f_mati_test_func_overlay108_vect f_mati_test_func_overlay108_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void OVL_f_mati_test_func_overlay109_vect f_mati_test_func_overlay109_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void OVL_f_mati_test_func_overlay110_vect f_mati_test_func_overlay110_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void OVL_f_mati_test_func_overlay111_vect f_mati_test_func_overlay111_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void OVL_f_mati_test_func_overlay112_vect f_mati_test_func_overlay112_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);



/// ----------------------------------------------------------------
/// AOM DUMMY functions prototypes and define
/// ----------------------------------------------------------------
void  _OVERLAY_ f_mati_test_func_overlay120_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay121_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay122_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay123_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay124_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay125_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay126_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay127_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay128_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay129_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay130_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay131_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay132_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay133_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay134_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay135_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay136_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay137_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay138_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay139_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay140_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay141_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay142_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay143_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay144_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay145_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay146_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay147_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay148_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay149_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay150_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay151_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay152_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay153_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay154_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay155_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay156_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay157_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay158_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);
void  _OVERLAY_ f_mati_test_func_overlay159_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR frameWorkCB_p);

#endif /* EOF */
