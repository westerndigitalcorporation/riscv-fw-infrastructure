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
/**                                                                       **/ 
/**         Copyright 2007 SanDisk Corporation | All rights reserved      **/ 
/**                                                                       **/ 
/**  Project Name :                                                       **/
/**  File Name    :                                                       **/
/**  File Creator :                                                       **/
/**  Creation Date:                                                       **/
/**                                                                       **/ 
/**  File purpose :  This header file defines which MQX test to perform.  **/
/**                  Steps required in order to run MQX related tests:    **/ 
/**                  1. ifdef out MQX_init_struct definition in sys_rtos.c**/ 
/**                  2. In f_sys_initArc1, ifdef out calls to the         **/ 
/**                     following functions:                              **/
/**                      - f_sys_aomInit                                  **/ 
/**                      - f_sys_initFwConfig                             **/ 
/**                  3. Merge 3 sections from test_main.c into pl_main.c  **/ 
/**                     - Sections are marked with:                       **/ 
/**                       //Merge Section - Start                         **/ 
/**                       //Merge Section - End                           **/ 
/**                  4. in BE ip.config, add the following include path   **/
/**                     under RAM link:                                   **/
/**                     - ut/MQX/include                                  **/
/**                     - ut/MQX/Source                                   **/
/**                  5. Each test expected result is defined in the C file**/
/**                     of the test                                       **/
/***************************************************************************/

/***************************************************************************/
/**         MULTIPLE INCLUSION                                            **/
/***************************************************************************/
#ifndef __MATI_TYPES_H
#define __MATI_TYPES_H

/***************************************************************************/
/**         INCLUDES                                                      **/
/***************************************************************************/
#include "common_types.h"
#include "mati_errors.h"

/***************************************************************************/
/**         ENUM                                                          **/
/***************************************************************************/

/***************************************************************************/
/**         DEFINITIONS                                                   **/
/***************************************************************************/
#define D_PTR_                *

#define D_NUMBER_OF_THREADS   7                                      // default number of threads
#define D_TIMEOUT_WAIT        20                                     // ms -  default timeout in milliseconds
#define D_TIMEOUT_DELTA       5                                      // ms -  default timeout overhead
#define D_TIMEOUT_TEST        800                                    // ms -  default timeout of entire test
#define D_TIMEOUT_EXTEND      D_TIMEOUT_WAIT*2 + D_TIMEOUT_DELTA * 2 // ms -  default timeout for extended test
#define D_TIMEOUT_SHORT       D_TIMEOUT_WAIT / 2                     // ms -  default timeout for short test
#define D_TIMEOUT_MICRO       2                                      // ms -  default timeout for thread sleep


#define D_MATI_SRC_ROM  0x0
#define D_MATI_SRC_RAM  0x1

#define D_PROCESSOR_MAIN    0
#define D_PROCESSOR_SECOND  1

// Tasks index in the MQX template struct
#define D_MAIN_MUX_TASK_TEMPLATE_INDEX  10
#define D_TEST_TASK_1_TEMPLATE_INDEX    11
#define D_TEST_TASK_2_TEMPLATE_INDEX    12
#define D_TEST_TASK_3_TEMPLATE_INDEX    13
#define D_TEST_TASK_4_TEMPLATE_INDEX    14
#define D_TEST_TASK_5_TEMPLATE_INDEX    15
#define D_TEST_TASK_6_TEMPLATE_INDEX    16
#define D_TEST_TASK_7_TEMPLATE_INDEX    17
#define D_TEST_DUMMY_1                  18
#define D_TEST_DUMMY_2                  19
#define D_TEST_DUMMY_3                  20

// TASKS IDs
#define D_TEST_TASK_1_ID    0
#define D_TEST_TASK_2_ID    1
#define D_TEST_TASK_3_ID    2
#define D_TEST_TASK_4_ID    3
#define D_TEST_TASK_5_ID    4
#define D_TEST_TASK_6_ID    5
#define D_TEST_TASK_7_ID    6
#define D_NUMBER_OF_TASKS   7

#define D_TASK_HIGHEST_PRIORITY  6
#define D_TASK_HIGHER_PRIORITY   7
#define D_TASK_WORKER_PRIORITY   10
#define D_TASK_MAIN_PRIORITY     11
#define D_TASK_DEFULT_PRIORITY   12
#define D_TASK_LOWER_PRIORITY    13
#define D_TASK_LOWEST_PRIORITY   14

/***************************/
/*   BUSY_LOOP_CONDITION   */
/***************************/
#define D_CB_WAIT_ON_SYNC_POINT   1
#define D_CB_WAIT_ON_SHARED_MEM   2
#define D_CB_WAIT_ON_START_TEST   3


// mutex
#define D_MUTEX_BLOCKING            0X1
#define D_MUTEX_NON_BLOCKING        0X2

#define D_NONE                   0

#define M_MATI_5_NOPS \
   asm volatile ("nop");  \
   asm volatile ("nop");  \
   asm volatile ("nop");  \
   asm volatile ("nop");  \
   asm volatile ("nop");  \

#define M_MATI_10_NOPS \
   M_MATI_5_NOPS \
   M_MATI_5_NOPS \

#define M_MATI_50_NOPS \
   M_MATI_10_NOPS \
   M_MATI_10_NOPS \
   M_MATI_10_NOPS \
   M_MATI_10_NOPS \
   M_MATI_10_NOPS \

#define M_MATI_100_NOPS \
   M_MATI_50_NOPS \
   M_MATI_50_NOPS \

/*********************/
/*   CONTROL BLOCK   */
/*********************/
typedef enum
{
 E_CB_MATI_RUN_STATUS_RUNNING     = 1,
 E_CB_MATI_RUN_STATUS_NOT_RUNNING = 0,

 E_CB_MATI_RUN_STATUS_EDGE = 0xdeadbeef // enforcing struct alignment: ensure enum size is u32_t
}E_CB_MATI_RUN_STATUS;


typedef enum
{
  E_CB_TEST_PROGRESS_MORE_TESTS     = 0,
  E_CB_TEST_PROGRESS_NOT_IMPLEMEMNTED = E_CB_TEST_PROGRESS_MORE_TESTS,

  E_CB_TEST_PROGRESS_RUNNING,

  E_CB_TEST_PROGRESS_NONE,
  E_CB_TEST_PROGRESS_NO_MORE_TESTS 	= E_CB_TEST_PROGRESS_NONE,
  E_CB_TEST_PROGRESS_COMPLETE = E_CB_TEST_PROGRESS_NO_MORE_TESTS,
  E_CB_TEST_PROGRESS_EDGE = 0xdeadbeef // enforcing struct alignment: ensure enum size is u32_t
}E_CB_TEST_PROGRESS;

// tests groups
typedef enum _E_CB_TEST_GROUP
{
  E_CB_TEST_GROUP_START       =  0x0,
  E_CB_TEST_GROUP_LWEVENTS    =  E_CB_TEST_GROUP_START,
  E_CB_TEST_GROUP_QUEUE       =  0X1,
  E_CB_TEST_GROUP_MUTEX       =  0X2,
  E_CB_TEST_GROUP_FMQUEUE     =  0X3,
  E_CB_TEST_GROUP_LWSEMAPHORE =  0X4,
  E_CB_TEST_GROUP_LWMEMRY     =  0X5,
  E_CB_TEST_GROUP_ISR         =  0X6,
  E_CB_TEST_GROUP_AOM         =  0X7,
  E_CB_TEST_GROUP_AFFINITY    =  0x8,
  E_CB_TEST_GROUP_HW_STACK    =  0x9,
  E_CB_TEST_GROUP_NONE,
  E_CB_TEST_GROUP_MAX         = E_CB_TEST_GROUP_NONE,
  E_CB_TEST_GROUP_EDGE = 0xdeadbeef // enforcing struct alignment: ensure enum size is u32_t
}E_CB_TEST_GROUP;

// LWevent test
typedef enum
{
  E_CB_TEST_LWEVENT_FIRST_TEST    = 0x0,
  E_CB_TEST_LWEVENT_WAIT_PRIVATE	= E_CB_TEST_LWEVENT_FIRST_TEST,
  E_CB_TEST_LWEVENT_WAIT_GLOBAL 	= 0X1,
  E_CB_TEST_LWEVENT_WAIT_OVERLAP	= 0X2,
  E_CB_TEST_LWEVENT_WAIT_TIMEOUT	= 0X3,
  E_CB_TEST_LWEVENT_AND_OR_COND 	= 0X4,
  E_CB_TEST_LWEVENT_SAMPLE_MASK 	= 0X5,
  E_CB_TEST_LWEVENT_NONE,
  E_CB_TEST_LWEVENT_MAX = E_CB_TEST_LWEVENT_NONE
}E_CB_TEST_LWEVENT;

// Queue test
typedef enum
{
  E_CB_TEST_QUEUE_ENEQUEU_DEQUEUE = 0X0,
  E_CB_TEST_QUEUE_INSERT_UNLINK	  = 0X1,
  E_CB_TEST_QUEUE_IS_EMPTY        =	0X2,
  E_CB_TEST_QUEUE_SIZE            = 0X3,
  E_CB_TEST_QUEUE_NONE,
  E_CB_TEST_QUEUE_MAX = E_CB_TEST_QUEUE_NONE
}E_CB_TEST_QUEUE;

// Mutex test
typedef enum
{
  E_CB_TEST_MUTEX_FIRST_TEST                    = 0x0,
  E_CB_TEST_MUTEX_LOCK_RELEASE                  = E_CB_TEST_LWEVENT_FIRST_TEST,
  E_CB_TEST_MUTEX_NON_BLOCK_LOCK                = 0X1,
  E_CB_TEST_MUTEX_WAIT_COUNT                    = 0X2,
  E_CB_TEST_MUTEX_SCHEDUAL_PRIORITY_INHERITANCE = 0X3,
  E_CB_TEST_MUTEX_SCHEDUAL_PRIORITY_PROTECTION 	= 0X4,
  E_CB_TEST_MUTEX_WAIT_PROTOCOL_QUEUEU          = 0X5,
  E_CB_TEST_MUTEX_WAIT_PROTOCOL_PRIORITY_QUEUEU = 0X6,
  E_CB_TEST_MUTEX_NONE,
  E_CB_TEST_MUTEX_MAX = E_CB_TEST_MUTEX_NONE
}E_CB_TEST_MUTEX;

// FMQueue
typedef enum
{
  E_CB_TEST_FMQUEUE_SEND_RECIEVE               = 0X0,
  E_CB_TEST_FMQUEUE_SEND_QUEUE_ENDS            = 0x1,
  E_CB_TEST_FMQUEUE_RECIEVE_QUEUE_ENDS         = 0X2,
  E_CB_TEST_FMQUEUE_SIGNAL                     = 0X3,
  E_CB_TEST_FMQUEUE_IS_EMPTY_AND_SIZE          = 0X4,
  E_CB_TEST_FMQUEUE_NONE,
  E_CB_TEST_FMQUEUE_MAX = E_CB_TEST_FMQUEUE_NONE
}E_CB_TEST_FMQUEUE;

// LWSem
typedef enum
{
  E_CB_TEST_LWSEM_WAIT_POST_WITHOUT_TIMEOUT_SEMAPHORE_EQUAL_TO_THREADS  =  0X0,
  E_CB_TEST_LWSEM_WAIT_POST_WITHOUT_TIMEOUT_SEMAPHORE_LESS_THAN_THREADS =  0X1,
  E_CB_TEST_LWSEM_WAIT_POST_WITHOUT_TIMEOUT_SEMAPHORE_MORE_THAN_THREADS =  0X2,
  E_CB_TEST_LWSEM_WAIT_POST_WITH_TIMEOUT                                =  0x3,
  E_CB_TEST_LWSEM_POOL                                                  =  0X4,
  E_CB_TEST_LWSEM_NONE,
  E_CB_TEST_LWSEM_MAX = E_CB_TEST_LWSEM_NONE
}E_CB_TEST_LWSEM;

// LWMem
typedef enum
{
  E_CB_TEST_LWMEM_ALLOCATION_AND_FREE                                  = 0X0,
  E_CB_TEST_LWMEM_ALLOCATION_AND_SECURE_FREE                           = 0X1,
  E_CB_TEST_LWMEM_NONE,
  E_CB_TEST_LWMEM_MAX = E_CB_TEST_LWMEM_NONE
}E_CB_TEST_LWMEM;


// ISR
typedef enum
{
  E_CB_TEST_ISR_GROUP_USERISR_TEST     = 0X0,
  E_CB_TEST_ISR_GROUP_KERNELISR_TEST   = 0X1,
  E_CB_TEST_ISR_GROUP_BLOCKINGISR_TEST = 0x2,
  E_CB_TEST_ISR_NONE,
  E_CB_TEST_ISR_MAX = E_CB_TEST_ISR_NONE
}E_CB_TEST_ISR;

// AOM
typedef enum
{
  E_CB_TEST_AOM_GROUP_SANITY                   = 0x0,
  E_CB_TEST_AOM_GROUP_WITH_SAME_SIZE           = 0x1,
  E_CB_TEST_AOM_GROUP_WITH_DIFFERENT_SIZE      = 0x2,
  E_CB_TEST_AOM_GROUP_MULTIGROUP               = 0x3,
  E_CB_TEST_AOM_GROUP_LOCK_UNLOCK              = 0x4,
  E_CB_TEST_AOM_GROUP_DEFRAG_OVERLAY_MEMORY    = 0x5,
  E_CB_TEST_AOM_GROUP_AOMA                     = 0x6,
  E_CB_TEST_AOM_THREAD_SAFE                    = 0x7,
  E_CB_TEST_AOM_OVL_CRC_CHECK                  = 0x8,
  E_CB_TEST_AOM_CRITICAL_SECTION               = 0x9,
  E_CB_TEST_AOM_NONE,
  E_CB_TEST_AOM_MAX = E_CB_TEST_AOM_NONE
}E_CB_TEST_AOM;

// Affinity
typedef enum
{
  E_CB_TEST_AFFINITY_GROUP_MAIN_TEST                 = 0X0,
  E_CB_TEST_AFFINITY_GROUP_BANK_RACE_TEST            = 0x1,
  E_CB_TEST_AFFINITY_GROUP_CONTEX_SWITCH_TIMING_TEST = 0x2,
  E_CB_TEST_AFFINITY_NONE,
  E_CB_TEST_AFFINITY_MAX = E_CB_TEST_AFFINITY_NONE
}E_CB_TEST_AFFINITY;

// HW Stack
typedef enum
{
  E_CB_TEST_HW_STACK_GROUP_MAIN_TEST  = 0x0,
  E_CB_TEST_HW_STACK_NONE,
  E_CB_TEST_HW_STACK_MAX = E_CB_TEST_HW_STACK_NONE
}E_CB_TEST_HW_STACK;

/******************************/
/*   TEST CONFIGURATIONS      */
/******************************/
typedef enum
{
  E_TEST_CONFIG_TEST_TYPE_UNIT         = 0,
  E_TEST_CONFIG_TEST_TYPE_PERFORMANCE  = 1,
}E_TEST_CONFIG_UNIT;

typedef enum
{
//  E_TEST_CONFIG_TEST_TYPE_POWER        = 2,
//  E_TEST_CONFIG_TEST_TYPE_STRESS       = 3,
  E_TEST_CONFIG_TEST_NEGATIVE_TYPE     = 0,
  E_TEST_CONFIG_TEST_POSITIVE_TYPE     = 1,
}E_TEST_CONFIG;

typedef enum
{
  E_TEST_CONFIG_CLEAR_DATA_ON          = 1,
  E_TEST_CONFIG_CLEAR_DATA_OFF         = 0,
}E_TEST_CONFIG_CLEAR_DATA;

typedef enum
{
  E_TEST_CONFIG_NON_BLOCKING           = 0,
  E_TEST_CONFIG_BLOCKING_ON            = 1,
}E_TEST_CONFIG_BLOCKING;

typedef enum
{
  E_TEST_CONFIG_BTST_ON                = 0,
  E_TEST_CONFIG_BTST_OFF               = 1,
}E_TEST_CONFIG_BTST;

typedef enum
{
  E_MATI_FW_STACK_SIZE_1024                = 0b0,
  E_MATI_FW_STACK_SIZE_1536                = 0b01,
  E_MATI_FW_STACK_SIZE_2048                = 0b11,
  E_MATI_FW_STACK_SIZE_2560                = 0b100,
  E_MATI_FW_STACK_SIZE_3072                = 0b101,
  E_MATI_FW_STACK_SIZE_3584                = 0b110,
  E_MATI_FW_STACK_SIZE_4096                = 0b111,
}E_MATI_FW_STACK_SIZE;

typedef enum
{
  E_MATI_SYNC_POINT_0                  = 0,
  E_MATI_SYNC_POINT_1                  = 1,
  E_MATI_SYNC_POINT_2                  = 2,
  E_MATI_SYNC_POINT_3                  = 3,
  E_MATI_SYNC_POINT_4                  = 4,
  E_MATI_SYNC_POINT_5                  = 5,
  E_MATI_SYNC_POINT_6                  = 6,
  E_MATI_SYNC_POINT_7                  = 7,
  E_MATI_SYNC_POINT_8                  = 8,
  E_MATI_SYNC_POINT_9                  = 9,
  E_MATI_SYNC_POINT_10                 = 10,
}E_MATI_TEST_REG_SYNC_POINT;

typedef enum
{
  E_MATI_NUMBER_OF_WORKERS_THREADS_0                  = 0,
  E_MATI_NUMBER_OF_WORKERS_THREADS_1                  = 1,
  E_MATI_NUMBER_OF_WORKERS_THREADS_2                  = 2,
  E_MATI_NUMBER_OF_WORKERS_THREADS_3                  = 3,
  E_MATI_NUMBER_OF_WORKERS_THREADS_4                  = 4,
  E_MATI_NUMBER_OF_WORKERS_THREADS_5                  = 5,
  E_MATI_NUMBER_OF_WORKERS_THREADS_6                  = 6,
  E_MATI_NUMBER_OF_WORKERS_THREADS_7                  = 7,
  E_MATI_NUMBER_OF_WORKERS_THREADS_8                  = 8,
  E_MATI_NUMBER_OF_WORKERS_THREADS_9                  = 9,
  E_MATI_NUMBER_OF_WORKERS_THREADS_10                 = 10,
}E_MATI_TEST_NUM_OF_WORKERS_THREADS;

typedef enum
{
  E_MATI_NUMBER_THREADS_0                  = 0,
  E_MATI_NUMBER_THREADS_1                  = 1,
  E_MATI_NUMBER_THREADS_2                  = 2,
  E_MATI_NUMBER_THREADS_3                  = 3,
  E_MATI_NUMBER_THREADS_4                  = 4,
  E_MATI_NUMBER_THREADS_5                  = 5,
  E_MATI_NUMBER_THREADS_6                  = 6,
  E_MATI_NUMBER_THREADS_7                  = 7,
  E_MATI_NUMBER_THREADS_8                  = 8,
  E_MATI_NUMBER_THREADS_9                  = 9,
  E_MATI_NUMBER_THREADS_10                 = 10,
}E_MATI_TEST_NUM_THREADS;

/***************************************************************************/
/**         TYPEDEFS                                                      **/
/***************************************************************************/
#if 0
typedef u08_t U_08;
typedef s08_t S_08;
typedef u16_t u16_t;
typedef s16_t S_16;
typedef u32_t u32_t;
typedef s32_t S_32;
typedef u64_t u64_t;
typedef s64_t S_U64;
#endif
typedef u08_t BOOL;

// struct that holds the thread function parameters
typedef struct
{
  u32_t  uiParam1;
  u32_t  uiParam2;
  u32_t  uiParam3;
}S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM, *S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR;

typedef E_TEST_ERROR (*pTask)(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR params);

/************
 * struct that contain the parameters as given by the FBCC
 *
 *
 *      MatiFW --> usBtstParam16 param:
 *
 *
 *  15       12     11      10       9       8       7     6       5      4       3          0
 *  +---------+-------+----------------  -----+-------+-----+-------+------+-------+-----------+
 *  |         | STACK | AFFINITY| ISR | LWMEM | LWSEM | FMQ | MUTEX | LWEV | Queue | StackSize |
 *  +---------+-------+---------------+-------+-------+-----+-------+------+-------+-----------+
 *             StackSize:
 *      +-----------------+---------+
 *      |STACK_SIZE_1024  | 0b0     |
 *      |STACK_SIZE_1536  | 0b01,   |
 *      |STACK_SIZE_2048  | 0b11,   |
 *      |STACK_SIZE_2560  | 0b100,  |
 *      |STACK_SIZE_3072  | 0b101,  |
 *      |STACK_SIZE_3584  | 0b110,  |
 *      |STACK_SIZE_4096  | 0b111,  |
 *      +-----------------+---------+
 *
 ************/
#pragma pack(push)

typedef struct
{
  union{
    ///This is the base struct definition
    struct
    {
      u16_t usBtstParam16;     // btst unsigned short parameter as given by the FBCC
      u32_t uiBtstParam32;     // btst unsigned int parameter as given by the FBCC
      u64_t ullBtstParam64;    // btst unsigned long long parameter as given by the FBCC
    };

    /// Test definition gets u16_t to be used as params.
    struct
    {
      struct
      {
        u16_t usNumStackSize                     : 3; ///[2:0] see enum E_MATI_FW_STACK_SIZE
        u16_t usQueueTestGroupEnabled            : 1; ///[3]
        u16_t usLWEventTestGroupEnabled          : 1; ///[4]
        u16_t usMutexTestGroupEnabled            : 1; ///[5]
        u16_t usFMQueueTestGroupEnabled          : 1; ///[6]
        u16_t usLWSemaphoreTestGroupEnabled      : 1; ///[7]
        u16_t usLWMemTestGroupEnabled            : 1; ///[8]
        u16_t usISRTestGroupEnabled              : 1; ///[9]
        u16_t usAffinityTestGroupEnabled         : 1; ///[10]
        u16_t usHardwareStackTestGroupEnabled    : 1; ///[11]
        u16_t usDummy                            : 4; /// [15:12] FFU
      }MatiFrameworkParams;

      /// Per test we have u32_t + u64_t to serve them.
      union
      {
        struct
        {
          u32_t uiDummy32;
          u64_t ullDummy64;
        }TEST_HW_STACK_GROUP_MAIN_TEST;
      };
    };
  };

} S_FRAMEWORK_BTSTS_PARAMS, *S_FRAMEWORK_BTSTS_PARAMS_PTR;
#pragma pack(pop)

// struct that will hold control block
typedef struct
{
  E_CB_TEST_GROUP                          eTestGroup;         // holds the test group bitmap
  u32_t                                     uiTestCurrent;      // holds the current test bitmap
  E_TEST_ERROR                             eTestErrorBitmap;   // holds the error bitmap
  u32_t                                     uiTestSyncPoint;    // flag - sync point for the test threads
  u32_t                                     uiTestSharedMemory; // generic parameter
  E_CB_MATI_RUN_STATUS                     eIsMatiStarted;     // flag - if we need to run
  E_CB_TEST_PROGRESS                       eTestProgress;      // flag - used per test suite, to understand when test tasks finish.
  u32_t                                     uiIsValidData;        /* contain a pattern to validate data
                                                                  * This is a global struct that is enstored in a DDR location
                                                                  * that is not tempered with during warm resets
                                                                  * this variable is used to know whether
                                                                  * or not the variable has already been initialized.*/
  S_FRAMEWORK_BTSTS_PARAMS                 sBtstParams;         // struct that contain the btst parameters
  S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM sTestTasksFunctionParamArr[D_NUMBER_OF_THREADS]; // will hold the function parameters
  pTask                                    sTestTasksFunctionArr[D_NUMBER_OF_THREADS];  // holds an array of function pointers
} S_FRAMEWORK_CONTROL_BLOCK, *S_FRAMEWORK_CONTROL_BLOCK_PTR;

typedef  E_CB_TEST_PROGRESS (*mati_tests_progress_function_ptr)(S_FRAMEWORK_CONTROL_BLOCK_PTR frameWorkCB_p);
typedef  E_TEST_ERROR (*mati_test_function_ptr)(S_FRAMEWORK_CONTROL_BLOCK_PTR frameWorkCB_p);


// struct that will hold test definitions
typedef struct
{
  E_TEST_CONFIG_UNIT       eIsUnitTest;    // 0 - unit test, 1 - performance test
  E_TEST_CONFIG_CLEAR_DATA eClearTestData; // 0 - clear global data, 1 - don't clean global data
  E_TEST_CONFIG_BLOCKING   eIsBlocking;    // 0 - non blocking tests, 1 - blocking test
  E_TEST_CONFIG_BTST       eIsBtst;        // 0 - will be called using BTST, 1 - will be called directly
  E_TEST_CONFIG            eTestConfig;    // 0 - negative test, 1 - positive test
} S_TEST_DEFINITIONS, *S_TEST_DEFINITIONS_PTR ;


/***************************************************************************/
/**         STRUCTS                                                       **/
/***************************************************************************/

/***************************************************************************/
/**         FUNCTIONS PROTOTYPES                                          **/
/***************************************************************************/

/***************************************************************************/
/**         END OF FILE                                                   **/
/***************************************************************************/

#endif /* EOF */

