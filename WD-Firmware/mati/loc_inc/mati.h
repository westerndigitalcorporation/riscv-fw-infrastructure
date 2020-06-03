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
/**  Project Name :                    **/
/**  File Name    :                                            **/ 
/**  File Creator :                                         **/ 
/**  Creation Date:                                            **/ 
/**                                                                       **/ 
/**  File purpose :  This header file defines which MQX test to perform.  **/
/**                  Steps required in order to run MQX related tests:    **/ 
/**                                                                       **/
/**                     of the test                                       **/
/***************************************************************************/

/***************************************************************************/
/**         MULTIPLE INCLUSION                                            **/
/***************************************************************************/
#ifndef __MATI_H
#define __MATI_H


/***************************************************************************/
/**         INCLUDES                                                      **/
/***************************************************************************/
///#if (PROD_MATI_MQX == 1) || (PROD_MATI_AOM == 1)
///#include "rom_tydf.h"
#include "mati_types.h"
#include "mati_tests.h"
///#endif

#if (PROD_MATI_MQX == 1)
  #include "rtos_api_pointers.h"
#endif

#if (PROD_MATI_AOM == 1)
  #include "comrv_api.h"
#endif



/***************************************************************************/
/**         DEFINITIONS                                                   **/
/***************************************************************************/
#define D_VALID_32_BYTE 0xDEADBEEF

#if (PROD_MATI_MQX==1)

  #define D_MATI_DEFAULT_TASK_STACK_SIZE  1024

  // leftovers when run mati from rom.
  // if we need to run it again from rom, we need to update the values
  #if defined(ROM)

    #define D_MATI_MQX_BASE_KERNEL_BYTES    4096
    #define D_MATI_INTERRUPT_STACK_SIZE     1024

    #define D_MATI_TASK_HEADER              260
    #define D_MATI_MQX_PER_THREAD_BYTES     244
    #define D_MATI_MQX_AOM_PER_THREAD_BYTES 16
    #define D_MATI_MQX_CORE_EXTRA           5000

    #define D_MQX_SYSROM_SIZE   ( ( (D_MATI_MQX_BASE_KERNEL_BYTES /*4096*/ + D_MATI_INTERRUPT_STACK_SIZE /*1024*/ + (D_MATI_DEFAULT_TASK_STACK_SIZE + D_MATI_TASK_HEADER) * D_NUMBER_OF_TASKS + /*8428*/( (D_MATI_MQX_PER_THREAD_BYTES + D_MATI_MQX_AOM_PER_THREAD_BYTES) * D_NUMBER_OF_TASKS ) /*1820*/ ) + D_MATI_MQX_CORE_EXTRA /*3400*/) & 0xFFFFFFFCUL)

    #pragma Data("MQX_Heap", DATA)
      U_08 G_MQX_KERNEL_SIZE[D_MQX_SYSROM_SIZE];
    #pragma Data();

    #define SYSRAM_START        &G_MQX_KERNEL_SIZE[0]
    #define SYSRAM_END          &G_MQX_KERNEL_SIZE[D_MQX_SYSROM_SIZE - 1]

  #endif
#endif

//#define D_FRAMEWORK_CB_ADDR     (S_FRAMEWORK_CONTROL_BLOCK_PTR)((void*)(&_MRAMM_PAST_END_) - sizeof(S_FRAMEWORK_CONTROL_BLOCK));
//#define D_FRAMEWORK_CB_ADDR     (RESET_AREA_RGN_ORIGIN + sizeof(S_FA_RESET_AREA) + sizeof(S_BTST_CB))
#define D_FRAMEWORK_CB_ADDR     (MRAMM_EXTENDED_ORIGIN + MRAMM_EXTENDED_LENGTH - D_MATI_FRAMEWORK_CONTROL_BLOCK_SIZE)
#define M_GET_MATI_CB()         (void*)(S_FRAMEWORK_CONTROL_BLOCK_PTR)D_FRAMEWORK_CB_ADDR;

/***************************************************************************/
/**    MACROS                                                             **/
/***************************************************************************/

/***************************************************************************/
/**         ENUM                                                          **/
/***************************************************************************/

/***************************************************************************/
/**         TYPEDEFS                                                      **/
/***************************************************************************/


/***************************************************************************/
/**    EXPORTED GLOBALS                                                   **/
/***************************************************************************/
///#if (PROD_MATI_AOM==1) || (PROD_MATI_MQX==1)
extern S_FRAMEWORK_CONTROL_BLOCK_PTR G_pFrameWorkCB;
extern S_TEST_DEFINITIONS G_testDefinitions;
///#endif


/***************************************************************************/
/**         FUNCTIONS PROTOTYPES                                          **/
/***************************************************************************/

#if (PROD_MATI_MQX==1)
  // test tasks & init
  E_MATI_Result f_mati_init_test_task(S_TEST_DEFINITIONS test_definitions);
  void        f_mati_test_task_1(U_32 param);
  void        f_mati_test_task_2(U_32 param);
  void        f_mati_test_task_3(U_32 param);
  void        f_mati_test_task_4(U_32 param);
  void        f_mati_test_task_5(U_32 param);
  void        f_mati_test_task_6(U_32 param);
  void        f_mati_test_task_7(U_32 param);

  E_MATI_Result f_mati_start_tasks();
  U_32        f_mati_number_of_free_threads();
  U_32        f_mati_get_random_thread();
  E_MATI_Result f_mati_release_thread_from_pool(U_32 thread);
  U_32        f_mati_get_index_of_bit_in_byte(U_32 number);
  void        f_mati_asignTasksStruct();
#endif

/***************************************************************************/
/**    EXTERNAL PROTOTYPES                                                **/
/***************************************************************************/


/***************************************************************************/
/**         END OF FILE                                                   **/
/***************************************************************************/
#endif /* EOF */

