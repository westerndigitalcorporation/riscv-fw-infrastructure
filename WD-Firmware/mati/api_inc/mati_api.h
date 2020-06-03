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
#ifndef __MATI_API_H
#define __MATI_API_H


/***************************************************************************/
/**         INCLUDES                                                      **/
/***************************************************************************/

/***************************************************************************/
/**         DEFINITIONS                                                   **/
/***************************************************************************/
#if (PROD_MATI_AOM==1) && defined(PROD_BTST_ENABLED)

  #define D_MATI_INCR_HIT_COUNTER_USER_MOVE_OVERLAY       G_mati_aom_funcs_hit_counter.uiUserMoveOverlayVect++;
  #define D_MATI_INCR_HIT_COUNTER_FILE_SYSTEM_READ        G_mati_aom_funcs_hit_counter.uiFileSystemRead++;
  #define D_MATI_INCR_HIT_COUNTER_FIND_SPACE_IN_RAM       G_mati_aom_funcs_hit_counter.uiFindSpaceInRam++;
  #define D_MATI_INCR_HIT_COUNTER_LOOKUP_OVERLAY_TABLE    G_mati_aom_funcs_hit_counter.uiLookupOverlayTable++;
  #define D_MATI_INCR_HIT_COUNTER_AOM_OVERLAY_LOADER      G_mati_aom_funcs_hit_counter.uiAomOverlayLoader++;
  #define M_MATI_CALL_AOM_FUNCTION()                      f_mati_aom_gmt_thread_function();
    // if we in the thread safe thread and in the first overlayed function call, then raise the SWI.
  #define M_MATI_RAISE_SWI_FOR_THREAD_SAFE_TEST_AND_YIELD() f_mati_raise_swi_for_thread_safe_test_and_yield();
  #define M_MATI_SYSTEM_TEST()
  #define D_MATI_FRAMEWORK_CONTROL_BLOCK_SIZE             (0x100)  //sizeof(S_FRAMEWORK_CONTROL_BLOCK);

#elif (PROD_MATI_MQX==1) && defined(PROD_BTST_ENABLED)

  #define D_MATI_INCR_HIT_COUNTER_USER_MOVE_OVERLAY
  #define D_MATI_INCR_HIT_COUNTER_FILE_SYSTEM_READ
  #define D_MATI_INCR_HIT_COUNTER_FIND_SPACE_IN_RAM
  #define D_MATI_INCR_HIT_COUNTER_LOOKUP_OVERLAY_TABLE
  #define D_MATI_INCR_HIT_COUNTER_AOM_OVERLAY_LOADER
  #define M_MATI_RAISE_SWI_FOR_THREAD_SAFE_TEST_AND_YIELD()
  #define M_MATI_CALL_AOM_FUNCTION()
  #define M_MATI_SYSTEM_TEST()
  #define D_MATI_FRAMEWORK_CONTROL_BLOCK_SIZE              (0x100)  //sizeof(S_FRAMEWORK_CONTROL_BLOCK);

#else //PROD_BTST_ENABLED is enable + PROD_MATI_MQX = 1/0  + PROD_MATI_AOM = 1/0

  #define D_MATI_INCR_HIT_COUNTER_USER_MOVE_OVERLAY
  #define D_MATI_INCR_HIT_COUNTER_FILE_SYSTEM_READ
  #define D_MATI_INCR_HIT_COUNTER_FIND_SPACE_IN_RAM
  #define D_MATI_INCR_HIT_COUNTER_LOOKUP_OVERLAY_TABLE
  #define D_MATI_INCR_HIT_COUNTER_AOM_OVERLAY_LOADER
  #define M_MATI_RAISE_SWI_FOR_THREAD_SAFE_TEST_AND_YIELD()
  #define M_MATI_CALL_AOM_FUNCTION()
  #define D_MATI_FRAMEWORK_CONTROL_BLOCK_SIZE               (0)

  #if PROD_MATI_MQX==1
    #define M_MATI_SYSTEM_TEST()                          f_mati_system_test();
  #else
    #define M_MATI_SYSTEM_TEST()
  #endif

#endif

typedef enum
{
  E_MATI_OK = 0,
  E_MATI_ERROR
}E_MATI_Result;


/***************************************************************************/
/**         MACROS                                                        **/


/***************************************************************************/
/**    MACROS                                                             **/
/***************************************************************************/

/***************************************************************************/
/**         TYPEDEFS                                                      **/
/***************************************************************************/
// struct that will hold AOM functions hit counters
typedef struct
{
  u32_t uiAomOverlayLoader;    // AOM_overlay_loader
  u32_t uiLookupOverlayTable;  // lookup_overlay_table
  u32_t uiFindSpaceInRam;      // find_space_in_ram
  u32_t uiFileSystemRead;      // f_fileSystemRead in AOM_mqx_user_read_backing_store_vect
  u32_t uiUserMoveOverlayVect; // AOM_mqx_user_move_overlay_vect
  u32_t uiErrorNum;            // AOM_mqx_user_system_error_vect error number
//  u32_t uiAomReadFile;        // aom_ReadFile
} S_MATI_AOM_FUNCS_HIT_COUNTER, *S_AOM_FUNCS_HIT_COUNTER_PTR ;

/***************************************************************************/
/**         EXPORTED GLOBALS                                              **/
/***************************************************************************/
#if (PROD_MATI_AOM==1)
  extern S_MATI_AOM_FUNCS_HIT_COUNTER G_mati_aom_funcs_hit_counter;
  extern void f_mati_raise_swi_for_thread_safe_test_and_yield(void);
  extern void f_mati_aom_gmt_thread_function(void);
#endif

#if (PROD_MATI_MQX==1)
  extern void f_mati_system_test(void);
#endif

/***************************************************************************/
/**    EXPORTED GLOBALS                                                   **/
/***************************************************************************/

/***************************************************************************/
/**         ENUM                                                          **/
/***************************************************************************/

/***************************************************************************/
/**         FUNCTIONS PROTOTYPES                                          **/
/***************************************************************************/
E_MATI_Result f_mati_main();

/***************************************************************************/
/**         END OF FILE                                                   **/
/***************************************************************************/
#endif /* MATI_API_C */

