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

/************************************************************
 * the overlay area is 4KB
 * the overlay table occupied 2KB
 * thus we have 2KB for our groups
 ***********************************************************/
#include "mati_types.h"
#include "mati_errors.h"
#include "mati.h"
#include "mati_utilities.h"
#include "mati_tests.h"
#include "mati_api.h"
#include "comrv_api.h"
#include "comrv_defines.h"
#include "comrv_types.h"
///#include "rtos_api_pointers.h"
///#include "pl_btst.h"
///#include "pl_rtos.h"
///#include "fa_fname.h"
#include "psp_api.h"
///#include "stdlib.h" //_crc32
///#include "Global.h"
///#include "sys_cpu.h"

///-------------------------------------------------------------------------
///    MACROS
///-------------------------------------------------------------------------

/// omt(__x) Ported from aom.c:
//#define omt(__x) (&overlay_mapping_table[__x])

/* Create a new task id from processor number and task number */
//#define M_MATI_BUILD_TASKID(procnum, tasknum) (u32_t)(((u32_t)(procnum) << 16) | ((u32_t)(tasknum) & 0xFFFF))

#define M_MATI_COMRV_DUMMY_FUNCTION(x) \
   void _OVERLAY_ f_mati_test_func_overlay ## x ## _vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR pFrameWorkCB) \
   { \
      M_MATI_10_NOPS \
      M_MATI_10_NOPS \
      M_MATI_10_NOPS \
   };  \

///-------------------------------------------------------------------------
///    DEFINITIONS
///-------------------------------------------------------------------------
#define D_MATI_GMT_THREAD_NEW_PRIORITY    (7)
#define D_MATI_BPM_THREAD_NEW_PRIORITY    (6)
#define D_MATI_EXCPT_SWI_VECTOR           (18)
#define D_PROCESSOR_NUMBER                (1)

// generate aom dummy function to fill the ovelay area
#define D_MATI_COMTV_FUNCTIONS_GENERATOR \
  M_MATI_COMRV_DUMMY_FUNCTION(120) \
  M_MATI_COMRV_DUMMY_FUNCTION(121) \
  M_MATI_COMRV_DUMMY_FUNCTION(122) \
  M_MATI_COMRV_DUMMY_FUNCTION(123) \
  M_MATI_COMRV_DUMMY_FUNCTION(124) \
  M_MATI_COMRV_DUMMY_FUNCTION(125) \
  M_MATI_COMRV_DUMMY_FUNCTION(126) \
  M_MATI_COMRV_DUMMY_FUNCTION(127) \
  M_MATI_COMRV_DUMMY_FUNCTION(128) \
  M_MATI_COMRV_DUMMY_FUNCTION(129) \
  M_MATI_COMRV_DUMMY_FUNCTION(130) \
  M_MATI_COMRV_DUMMY_FUNCTION(131) \
  M_MATI_COMRV_DUMMY_FUNCTION(132) \
  M_MATI_COMRV_DUMMY_FUNCTION(133) \
  M_MATI_COMRV_DUMMY_FUNCTION(134) \
  M_MATI_COMRV_DUMMY_FUNCTION(135) \
  M_MATI_COMRV_DUMMY_FUNCTION(136) \
  M_MATI_COMRV_DUMMY_FUNCTION(137) \
  M_MATI_COMRV_DUMMY_FUNCTION(138) \
  M_MATI_COMRV_DUMMY_FUNCTION(139) \
  M_MATI_COMRV_DUMMY_FUNCTION(140) \
  M_MATI_COMRV_DUMMY_FUNCTION(141) \
  M_MATI_COMRV_DUMMY_FUNCTION(142) \
  M_MATI_COMRV_DUMMY_FUNCTION(143) \
  M_MATI_COMRV_DUMMY_FUNCTION(144) \
  M_MATI_COMRV_DUMMY_FUNCTION(145) \
  M_MATI_COMRV_DUMMY_FUNCTION(146) \
  M_MATI_COMRV_DUMMY_FUNCTION(147) \
  M_MATI_COMRV_DUMMY_FUNCTION(148) \
  M_MATI_COMRV_DUMMY_FUNCTION(149) \
  M_MATI_COMRV_DUMMY_FUNCTION(150) \
  M_MATI_COMRV_DUMMY_FUNCTION(151) \
  M_MATI_COMRV_DUMMY_FUNCTION(152) \
  M_MATI_COMRV_DUMMY_FUNCTION(153) \
  M_MATI_COMRV_DUMMY_FUNCTION(154) \
  M_MATI_COMRV_DUMMY_FUNCTION(155) \
  M_MATI_COMRV_DUMMY_FUNCTION(156) \
  M_MATI_COMRV_DUMMY_FUNCTION(157) \
  M_MATI_COMRV_DUMMY_FUNCTION(158) \
  M_MATI_COMRV_DUMMY_FUNCTION(159)



///----------------------------------------------------------
/// call the dummy functions to fill the resident area
/// run at GROUP 109-129 - GROUP size 0x200
///----------------------------------------------------------
#define D_MATI_AOM_FUNCTIONS_CALL \
  f_mati_test_func_overlay120_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay121_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay122_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay123_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay124_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay125_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay126_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay127_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay128_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay129_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay130_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay131_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay132_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay133_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay134_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay135_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay136_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay137_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay138_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay139_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay140_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay141_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay142_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay143_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay144_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay145_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay146_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay147_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay148_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay149_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay150_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay151_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay152_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay153_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay154_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay155_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay156_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay157_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay158_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);\
  f_mati_test_func_overlay159_vect(&G_frameWorkCB_p->sTestTasksFunctionParamArr[0]);


///-------------------------------------------------------------------------
///    TYPES
///-------------------------------------------------------------------------

///-------------------------------------------------------------------------
///    LOCAL PROTOTYPES
///-------------------------------------------------------------------------
E_TEST_ERROR f_mati_aom_test_sanity(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
E_TEST_ERROR f_mati_aom_test_group_with_same_size(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
E_TEST_ERROR F_mati_aom_test_group_with_different_size(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
E_TEST_ERROR f_mati_aom_test_multigroup(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
E_TEST_ERROR f_mati_aom_test_lock_unlock(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
E_TEST_ERROR f_mati_aom_test_defrag_overlayed_functions(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
E_TEST_ERROR f_mati_aom_test_grouping_aoma(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
E_TEST_ERROR f_mati_aom_test_thread_safe(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
E_TEST_ERROR f_mati_aom_test_CRC_check(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
E_TEST_ERROR f_mati_aom_test_critical_section(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB);
s32_t         f_mati_aom_user_system_error_vect(u32_t errornum);
void         f_mati_aom_gmt_thread_function(void);
void         f_mati_raise_swi_for_thread_safe_test_and_yield(void);
void         f_mati_aom_sw_isr_change_gmt_prio(void* dd);

///-------------------------------------------------------------------------
///    EXTERNAL PROTOTYPES
///-------------------------------------------------------------------------
/* ronen
extern _Far u08_t _fAOM_sdata, _eAOM_sdata;
extern omt_entry_t overlay_mapping_table[];
*/

#if AOM_DBG_CRITICAL ==  1
  extern void AOM_set_critical(ulong_t tid);
  extern void AOM_clear_critical(ulong_t tid);
#endif

///-------------------------------------------------------------------------
///    GLOBAL VARIABLES
///-------------------------------------------------------------------------
u32_t GlobalVar;
u32_t tempArr[256] = {0};
S_MATI_AOM_FUNCS_HIT_COUNTER G_mati_aom_funcs_hit_counter;
/* ronen
RTOS_LWEVENT_STRUCT G_matiLWEvent;
RTOS_LWEVENT_STRUCT_PTR G_matiLWEvent_p;
*/

//typedef int (*mati_test_aom)(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR);
const mati_test_function_ptr G_lookup_table_mati_test_aom[E_CB_TEST_AOM_MAX] =
{
  f_mati_aom_test_sanity,                     /* E_CB_TEST_AOM_GROUP_SANITY */
  f_mati_aom_test_group_with_same_size,       /* E_CB_TEST_AOM_GROUP_WITH_SAME_SIZE */
  F_mati_aom_test_group_with_different_size,  /* E_CB_TEST_AOM_GROUP_WITH_DIFFERENT_SIZE */
  f_mati_aom_test_multigroup,                 /* E_CB_TEST_AOM_GROUP_MULTIGROUP */
  f_mati_aom_test_lock_unlock,                /* E_CB_TEST_AOM_GROUP_LOCK_UNLOCK */
  f_mati_aom_test_defrag_overlayed_functions, /* E_CB_TEST_AOM_GROUP_DEFRAG_OVERLAY_MEMORY */
  f_mati_aom_test_grouping_aoma,              /* E_CB_TEST_AOM_GROUP_AOMA */
  f_mati_aom_test_thread_safe,                /* E_CB_TEST_AOM_THREAD_SAFE */
  f_mati_aom_test_CRC_check,                  /* E_CB_TEST_AOM_OVL_CRC_CHECK */
  f_mati_aom_test_critical_section,           /* E_CB_TEST_AOM_CRITICAL_SECTION */
};

///-------------------------------------------------------------------------
///    START IMPLEMENTATION
///-------------------------------------------------------------------------

/**************************************************************************
 the following groups contains the following functions:

group 101:
  f_mati_test_func_overlay101_vect,
  f_mati_test_func_overlay105_vect

group 102
  --f_mati_test_func_overlay105_vect,
  f_mati_test_func_overlay102_vect,
  f_mati_test_func_overlay104_vect,
  f_mati_test_func_overlay107_vect

group 103:
  f_mati_test_func_overlay103_vect

group 104:
  f_mati_test_func_overlay106_vect

group 105:
  f_mati_test_func_overlay108_vect

group 106:
  f_mati_test_func_overlay109_vect

group 107:
  f_mati_test_func_overlay110_vect

group 108:
  f_mati_test_func_overlay111_vect

group 109:
  f_mati_test_func_overlay112_vect

**************************************************************************/

/* define dummy functions to fill the overlay */
D_MATI_COMTV_FUNCTIONS_GENERATOR

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_aom_user_system_error_vect                **/
/**                                                                       **/
/**   Description      : new implementation to the error function pointer **/
/**                      that is necessary to some of the tests           **/
/**                                                                       **/
/**   Input parameters : errornum - error number                          **/
/**                                                                       **/
/**   Output parameters: None                                             **/
/**                                                                       **/
/**   Return value     : errornum - error number                          **/
/**                                                                       **/
/**   Remarks          : none                                             **/
/**                                                                       **/
/***************************************************************************/
s32_t f_mati_aom_user_system_error_vect(u32_t errornum)
{
#if 0 /* ronen */
  if (errornum ==  et_USER_ERROR)
  {
    G_mati_aom_funcs_hit_counter.uiErrorNum = errornum; /* OS/DK: add event for AOM critical section instead of fatal and break; */
#if AOM_DBG_CRITICAL == 1
    AOM_clear_critical(f_rtos_taskGetId(NULL));
#endif
  }
#endif /* 0 ronen */
  return errornum;
}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_aom_test_sanity                           **/
/**                                                                       **/
/**   Description      : 5.2.1 - sanity test                              **/
/**                                                                       **/
/**   Input parameters : structure that holds 3 parameters                **/
/**                      param1:                                          **/
/**                      param2:                                          **/
/**                      param3:                                          **/
/**                                                                       **/
/**   Output parameters: None                                             **/
/**                                                                       **/
/**   Return value     : error bit map                                    **/
/**                                                                       **/
/**   Remarks          : verify aom groups are loaded when needed         **/
/**                                                                       **/
/***************************************************************************/
E_TEST_ERROR f_mati_aom_test_sanity(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{

  // call FUNCTION in GROUP 101 - GROUP size 0x200
  f_mati_test_func_overlay101_vect(pFrameWorkCB->sTestTasksFunctionParamArr);


  // we expect 2 hits in file_syetem_read; one for the lookup table and one for the overlay function in GROUP 101
  if (G_mati_aom_funcs_hit_counter.uiFileSystemRead != 2)
  {
    f_mati_setErrorBit(pFrameWorkCB, E_TEST_ERROR_AOM_FAILED);
  }
  else
  {
    // call FUNCTION in GROUP 103 - GROUP size is 0x200
    f_mati_test_func_overlay103_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

    // we expect 1 hits in file_syetem_read; none for the lookup table, one for the overlay function in GROUP 103
    if (G_mati_aom_funcs_hit_counter.uiFileSystemRead != 3)
    {
      f_mati_setErrorBit(pFrameWorkCB, E_TEST_ERROR_AOM_SANITY_FAILED);
    }
    else
    {
      // call FUNCTION in GROUP 102 - GROUP size 0x200
      f_mati_test_func_overlay102_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

      // call function from GROUP 102 that calls another function from GROUP 102 - GROUP size 0x200
      f_mati_test_func_overlay104_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

      // we expect 0 hits in file_syetem_read because the function supposed to be resident
      if (G_mati_aom_funcs_hit_counter.uiFileSystemRead != 4)
      {
        f_mati_setErrorBit(pFrameWorkCB, E_TEST_ERROR_AOM_SANITY_FAILED);
      }
    }
  }
  return f_mati_getErrorBits(pFrameWorkCB);
}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_aom_test_group_with_same_size             **/
/**                                                                       **/
/**   Description      : 5.2.2.1 - Groups with the same size              **/
/**                                                                       **/
/**   Input parameters : structure that holds 3 parameters                **/
/**                      param1:                                          **/
/**                      param2:                                          **/
/**                      param3:                                          **/
/**                                                                       **/
/**   Output parameters: None                                             **/
/**                                                                       **/
/**   Return value     : error bit map                                    **/
/**                                                                       **/
/**   Remarks          : verify aom groups are loaded when needed while   **/
/**                      the groups with the same size                    **/
/**                                                                       **/
/***************************************************************************/
E_TEST_ERROR f_mati_aom_test_group_with_same_size(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{
  // call FUNCTION in GROUP 101 - GROUP size 0x200
  f_mati_test_func_overlay101_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

  // we expect 2 hits in file_syetem_read; one for the lookup table and one for the overlay function in GROUP 101
  if (G_mati_aom_funcs_hit_counter.uiFileSystemRead != 2)
  {
    f_mati_setErrorBit(pFrameWorkCB, E_TEST_ERROR_AOM_FAILED);
  }
  else
  {
    // call FUNCTION in GROUP 101 and 102 - GROUP size 0x200
    f_mati_test_func_overlay105_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

    // we expect 0 hits in file_syetem_read because the function was loaded in GROUP 101
    if (G_mati_aom_funcs_hit_counter.uiFileSystemRead != 2)
    {
      f_mati_setErrorBit(pFrameWorkCB, E_TEST_ERROR_AOM_UNIFORM_GROUPS_FAILED);
    }
    else
    {
      // call FUNCTION in GROUP 103 - GROUP size is 0x200
      f_mati_test_func_overlay103_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

      // we expect 1 hits in file_syetem_read; for the overlay function in GROUP 103
      if (G_mati_aom_funcs_hit_counter.uiFileSystemRead != 3)
      {
        f_mati_setErrorBit(pFrameWorkCB, E_TEST_ERROR_AOM_UNIFORM_GROUPS_FAILED);
      }
    }
  }
  return f_mati_getErrorBits(pFrameWorkCB);
}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : F_mati_aom_test_group_with_different_size        **/
/**                                                                       **/
/**   Description      : 5.2.2.2 - Groups with the same size              **/
/**                                                                       **/
/**   Input parameters : structure that holds 3 parameters                **/
/**                      param1:                                          **/
/**                      param2:                                          **/
/**                      param3:                                          **/
/**                                                                       **/
/**   Output parameters: None                                             **/
/**                                                                       **/
/**   Return value     : error bit map                                    **/
/**                                                                       **/
/**   Remarks          : verify aom groups are loaded when needed while   **/
/**                      the groups with different size                   **/
/**                                                                       **/
/***************************************************************************/
E_TEST_ERROR F_mati_aom_test_group_with_different_size(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{
  // call FUNCTION in GROUP 101 - GROUP size 0x200
  f_mati_test_func_overlay101_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

  // we expect 2 hits in file_syetem_read; one for the lookup table and one for the overlay function in GROUP 101
  if (G_mati_aom_funcs_hit_counter.uiFileSystemRead != 2)
  {
    f_mati_setErrorBit(pFrameWorkCB, E_TEST_ERROR_AOM_FAILED);
  }
  else
  {
    // call FUNCTION in GROUP 104 (GROUP size 0xA00), the GROUP is bigger than GROUP 101
    f_mati_test_func_overlay106_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

    // we expect 1 hits in file_syetem_read; for the overlay function in GROUP 104
    if (G_mati_aom_funcs_hit_counter.uiFileSystemRead != 3)
    {
      f_mati_setErrorBit(pFrameWorkCB, E_TEST_ERROR_AOM_NON_UNIFORM_GROUPS_FAILED);
    }
  }
  return f_mati_getErrorBits(pFrameWorkCB);
}



/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_aom_test_multigroup                       **/
/**                                                                       **/
/**   Description      : 5.2.2.3 - multiple Groups                        **/
/**                                                                       **/
/**   Input parameters : structure that holds 3 parameters                **/
/**                      param1:                                          **/
/**                      param2:                                          **/
/**                      param3:                                          **/
/**                                                                       **/
/**   Output parameters: None                                             **/
/**                                                                       **/
/**   Return value     : error bit map                                    **/
/**                                                                       **/
/**   Remarks          : verify aom groups are loaded when needed while   **/
/**                      they are located in multiple groups              **/
/**                                                                       **/
/***************************************************************************/
E_TEST_ERROR f_mati_aom_test_multigroup(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{
#if (MULTIGROUP_TOKEN_SUPPORT==1)

  //_brk();
  void* overlayFunction_p;

  // call FUNCTION in GROUP 101 - GROUP size 0x200
  f_mati_test_func_overlay101_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

  // we expect 2 hits in file_syetem_read; one for the lookup table and one for the overlay function in GROUP 101
  if (G_mati_aom_funcs_hit_counter.uiFileSystemRead != 2)
  {
    f_mati_setErrorBit(pFrameWorkCB, E_TEST_ERROR_AOM_FAILED);
  }
  else
  {
    // save the first multigroup function instance address
    overlayFunction_p = &f_mati_test_func_overlay105_vect;

    // fill the resident area with dummy functions and try to evict the first GROUP
    #undef M_MATI_COMRV_DUMMY_FUNCTION
    #define M_MATI_COMRV_DUMMY_FUNCTION(x)  f_mati_test_func_overlay ## x ## _vect(&pFrameWorkCB->sTestTasksFunctionParamArr[0]);
    D_MATI_COMTV_FUNCTIONS_GENERATOR

    // call FUNCTION in GROUP 102 - GROUP size 0x200
    f_mati_test_func_overlay102_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

    // we expect no hit in file_syetem_read because the function is already resident
    if (G_mati_aom_funcs_hit_counter.uiFileSystemRead != 3)
    {
      f_mati_setErrorBit(pFrameWorkCB, E_TEST_ERROR_AOM_MULTI_GROUPS_FAILED);
    }
    else
    {
      // compare the addresses of the multigroup function
      if (overlayFunction_p == &f_mati_test_func_overlay105_vect)
      {
        f_mati_setErrorBit(pFrameWorkCB, E_TEST_ERROR_AOM_MULTI_GROUPS_FAILED);
      }
    }
  }
#endif

  return f_mati_getErrorBits(pFrameWorkCB);
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_aom_test_lock_unlock                      **/
/**                                                                       **/
/**   Description      : 5.2.2.4 - Lock / Unlock Groups                   **/
/**                                                                       **/
/**   Input parameters : structure that holds 3 parameters                **/
/**                      param1:                                          **/
/**                      param2:                                          **/
/**                      param3:                                          **/
/**                                                                       **/
/**   Output parameters: None                                             **/
/**                                                                       **/
/**   Return value     : error bit map                                    **/
/**                                                                       **/
/**   Remarks          : verify aom groups are locked and released when   **/
/**                      needed. need to verify we configure the aom      **/
/**                                                                       **/
/***************************************************************************/
E_TEST_ERROR f_mati_aom_test_lock_unlock(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{
  u08_t ucCounter = 10;

  #undef M_MATI_COMRV_DUMMY_FUNCTION
  #define M_MATI_COMRV_DUMMY_FUNCTION(x)  f_mati_test_func_overlay ## x ## _vect(&pFrameWorkCB->sTestTasksFunctionParamArr[0]);
  D_MATI_COMTV_FUNCTIONS_GENERATOR

  f_mati_test_func_overlay101_vect(pFrameWorkCB->sTestTasksFunctionParamArr); // call FUNCTION in GROUP 101 - GROUP size is 0x200
  f_mati_test_func_overlay102_vect(pFrameWorkCB->sTestTasksFunctionParamArr); // call FUNCTION in GROUP 102 - GROUP size is 0x200
  f_mati_test_func_overlay103_vect(pFrameWorkCB->sTestTasksFunctionParamArr); // call FUNCTION in GROUP 103 - GROUP size is 0x200
  f_mati_test_func_overlay104_vect(pFrameWorkCB->sTestTasksFunctionParamArr); // call FUNCTION in GROUP 102 - GROUP size is 0x200
  f_mati_test_func_overlay105_vect(pFrameWorkCB->sTestTasksFunctionParamArr); // call FUNCTION in GROUP 101 - GROUP size is 0x200
  f_mati_test_func_overlay107_vect(pFrameWorkCB->sTestTasksFunctionParamArr); // call FUNCTION in GROUP 102 - GROUP size is 0x200

  while (ucCounter--)
  {
    /// run at GROUP 107 - GROUP at size 0xc00
    f_mati_test_func_overlay111_vect(pFrameWorkCB->sTestTasksFunctionParamArr);
  }

  //_brk();

  comrvLockUnlockOverlayGroupByFunction((void*)f_mati_test_func_overlay111_vect, D_COMRV_GROUP_STATE_LOCK);

  /// run at GROUP 109 - GROUP at size 0x800
  f_mati_test_func_overlay112_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

/*
  // call FUNCTION in GROUP 101 - GROUP size 0x200
  f_mati_test_func_overlay101_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

  // we expect 2 hits in file_syetem_read; one for the lookup table and one for the overlay function in GROUP 101
  if (G_mati_aom_funcs_hit_counter.uiFileSystemRead != 2)
  {
    f_mati_setErrorBit(pFrameWorkCB, E_TEST_ERROR_AOM_FAILED);
  }
  else
  {
    // lock GROUP 101
    f_aom_lockGroup((void*)f_mati_test_func_overlay101_vect);

    // fill the resident area with dummy functions and try to evict the first GROUP
    #undef M_MATI_COMRV_DUMMY_FUNCTION
    #define M_MATI_COMRV_DUMMY_FUNCTION(x)  f_mati_test_func_overlay ## x ## _vect(&pFrameWorkCB->sTestTasksFunctionParamArr[0]);
    D_MATI_OVL_FUNCTIONS_GENERATOR

    // call FUNCTION in GROUP 101
    f_mati_test_func_overlay101_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

    // we expect 0 hits in file_syetem_read; the GROUP supposed to be locked
    if (G_mati_aom_funcs_hit_counter.uiFileSystemRead != 0)
    {
      f_mati_setErrorBit(pFrameWorkCB, E_TEST_ERROR_AOM_NON_UNIFORM_GROUPS_FAILED);
    }
    else
    {
      // unlock GROUP 101
      f_aom_unLockGroup((void*)f_mati_test_func_overlay101_vect);

      // fill the resident area with dummy functions and try to evict the first GROUP
      D_MATI_OVL_FUNCTIONS_GENERATOR

      // call FUNCTION in GROUP 101
      f_mati_test_func_overlay101_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

      // we expect 1 hits in file_syetem_read; the GROUP supposed to be evicted
      if (G_mati_aom_funcs_hit_counter.uiFileSystemRead != 3)
      {
        f_mati_setErrorBit(pFrameWorkCB, E_TEST_ERROR_AOM_NON_UNIFORM_GROUPS_FAILED);
      }
    }
  }
*/
  return f_mati_getErrorBits(pFrameWorkCB);

}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_aom_test_defrag_overlayed_functions       **/
/**                                                                       **/
/**   Description      : 5.2.2.5 - Defrag Overlay Area                    **/
/**                                                                       **/
/**   Input parameters : structure that holds 3 parameters                **/
/**                      param1:                                          **/
/**                      param2:                                          **/
/**                      param3:                                          **/
/**                                                                       **/
/**   Output parameters: None                                             **/
/**                                                                       **/
/**   Return value     : error bit map                                    **/
/**                                                                       **/
/**   Remarks          : verify aom core using defrag                     **/
/**                                                                       **/
/***************************************************************************/
E_TEST_ERROR f_mati_aom_test_defrag_overlayed_functions(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{
  u08_t usLoopCounter = 25;

  // call FUNCTION in GROUP 104 (GROUP size 0xA00)
  f_mati_test_func_overlay106_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

  // lock the GROUP to avoid eviction before filling the overlay area
  comrvLockUnlockOverlayGroupByFunction((void*)f_mati_test_func_overlay106_vect, D_COMRV_GROUP_STATE_LOCK);

  // fill the resident area with dummy functions and try to evict the first GROUP
  #undef M_MATI_COMRV_DUMMY_FUNCTION
  #define M_MATI_COMRV_DUMMY_FUNCTION(x)  f_mati_test_func_overlay ## x ## _vect(&pFrameWorkCB->sTestTasksFunctionParamArr[0]);
  D_MATI_COMTV_FUNCTIONS_GENERATOR

  // unlock the GROUP to avoid evict and create hole in the overlay area
  comrvLockUnlockOverlayGroupByFunction((void*)f_mati_test_func_overlay106_vect, D_COMRV_GROUP_STATE_UNLOCK);


  // call the function several times to raise its eviction ratio
  while (usLoopCounter--)
  {
    // call FUNCTION in GROUP 101 - size of 0x200
    f_mati_test_func_overlay101_vect(pFrameWorkCB->sTestTasksFunctionParamArr);
  }

  // call FUNCTION in GROUP 105 - size of 0x400
  f_mati_test_func_overlay108_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

  usLoopCounter = 25;
  // call the function several times to raise its eviction ratio
  while (usLoopCounter--)
  {
    // call FUNCTION in GROUP 102 - size of 0x200
    f_mati_test_func_overlay102_vect(pFrameWorkCB->sTestTasksFunctionParamArr);
  }

  G_mati_aom_funcs_hit_counter.uiUserMoveOverlayVect = 0;

  // run at GROUP 107 - GROUP at size 0x600
  f_mati_test_func_overlay110_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

  // we expect more than 1 hits in User_Move_Overlay;
  if (G_mati_aom_funcs_hit_counter.uiUserMoveOverlayVect < 1)
  {
    f_mati_setErrorBit(pFrameWorkCB, E_ERROR_AOM_DEFRAG_FAILED);
  }

  // run at GROUP 107 - GROUP at size 0x600
  f_mati_test_func_overlay110_vect(pFrameWorkCB->sTestTasksFunctionParamArr);
  return f_mati_getErrorBits(pFrameWorkCB);
}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_aom_test_grouping_aoma                    **/
/**                                                                       **/
/**   Description      : 5.2.2.6 - AOMA                                   **/
/**                                                                       **/
/**   Input parameters : structure that holds 3 parameters                **/
/**                      param1:                                          **/
/**                      param2:                                          **/
/**                      param3:                                          **/
/**                                                                       **/
/**   Output parameters: None                                             **/
/**                                                                       **/
/**   Return value     : error bit map                                    **/
/**                                                                       **/
/**   Remarks          : verify aoma                                      **/
/**                                                                       **/
/***************************************************************************/
E_TEST_ERROR f_mati_aom_test_grouping_aoma(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{

  u32_t loopCount = 15;

  while( loopCount--)
  {
    f_mati_test_func_overlay120_vect(pFrameWorkCB->sTestTasksFunctionParamArr);
    f_mati_test_func_overlay121_vect(pFrameWorkCB->sTestTasksFunctionParamArr);
  }

  loopCount = 75;

  while (loopCount--)
  {
    f_mati_test_func_overlay123_vect(pFrameWorkCB->sTestTasksFunctionParamArr);
    f_mati_test_func_overlay124_vect(pFrameWorkCB->sTestTasksFunctionParamArr);
  }

  loopCount = 125;

  while (loopCount-- )
  {
    f_mati_test_func_overlay125_vect(pFrameWorkCB->sTestTasksFunctionParamArr);
    f_mati_test_func_overlay126_vect(pFrameWorkCB->sTestTasksFunctionParamArr);
  }

  return f_mati_getErrorBits(pFrameWorkCB);
}


/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_aom_test_critical_section                 **/
/**                                                                       **/
/**   Description      : 5.2.2.7 - critical section                       **/
/**                                                                       **/
/**   Input parameters : structure that holds 3 parameters                **/
/**                      param1:                                          **/
/**                      param2:                                          **/
/**                      param3:                                          **/
/**                                                                       **/
/**   Output parameters: None                                             **/
/**                                                                       **/
/**   Return value     : error bit map                                    **/
/**                                                                       **/
/**   Remarks          : verify critical section from multiple            **/
/**                      threads (GMT)                                    **/
/**                                                                       **/
/***************************************************************************/
E_TEST_ERROR f_mati_aom_test_critical_section(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{

#if AOM_DBG_CRITICAL == 1

  //_brk();

  pRTOS_User_ISR_t GOriginalUserISR_p;
  U_16 usCriticalSection = 0 ;

  // Assign a system error function to capture  when we fail on critical section
  f_aom_user_system_error = f_mati_aom_user_system_error_vect;

  // call FUNCTION in GROUP 101 - GROUP size 0x200
  // call the first aom function to verify sanity
  f_mati_test_func_overlay101_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

  // if the critical section allocation succeed then we will try to trigger the error
  if (G_mati_aom_funcs_hit_counter.uiErrorNum == 0)
  {
    f_aom_registerCriticalSectionMonitor(&usCriticalSection);

    // call FUNCTION in GROUP 102 - GROUP size 0x200
    // supposed to trigger an error.
    // the error function will disable the critical section bit to avoid infinity loop
    f_mati_test_func_overlay102_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

    if (G_mati_aom_funcs_hit_counter.uiErrorNum != 0)
    {
      // reset the error counter
      G_mati_aom_funcs_hit_counter.uiErrorNum = 0;

      // turn the critical bit on after exiting the error function using synopsys API
      AOM_set_critical(f_rtos_taskGetId(NULL));

      // replace the SWI function with the new one to trigger the GMT priority change
      GOriginalUserISR_p = M_INT_INSTALL_ISR(D_MATI_EXCPT_SWI_VECTOR, f_mati_aom_sw_isr_change_gmt_prio, (void*)0);

      // enable SWI
      M_PSP_INT_ENABLE_CORE_INT_NUM(D_MATI_EXCPT_SWI_VECTOR);

      if (G_mati_aom_funcs_hit_counter.uiErrorNum != 0)
      {
        f_mati_setErrorBit(pFrameWorkCB, E_ERROR_AOM_CRITICAL_SECTION_FAILED);
      }
    }
    else
    {
      f_mati_setErrorBit(pFrameWorkCB, E_ERROR_AOM_CRITICAL_SECTION_FAILED);
    }

    f_aom_unregisterCriticalSectionMonitor(&usCriticalSection);

  }
#endif
  return f_mati_getErrorBits(pFrameWorkCB);
}





/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_aom_gmt_thread_function                   **/
/**   Description      : call overlay function from GMT thread (while the **/
/**                      read backing store supposed to be locked by INP  **/
/**   Input parameters : Control Block                                    **/
/**   Output parameters: None                                             **/
/**   Return value     : MATI_OK if succeed, MATI_ERROR otherwise.        **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
void f_mati_aom_gmt_thread_function(void)
{
#if 0 /* ronen */
  S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM params;
  G_matiLWEvent_p = &G_matiLWEvent;
  G_pFrameWorkCB = M_GET_MATI_CB();
  u32_t uiPriority;

  if ( (G_pFrameWorkCB->uiTestCurrent == E_CB_TEST_AOM_THREAD_SAFE) || (G_pFrameWorkCB->uiTestCurrent == E_CB_TEST_AOM_CRITICAL_SECTION) )
  {

    // lock the GMT thread on LWEvent before waitForMount function
    f_rtos_lightWeightEventInit(G_matiLWEvent_p, 1);
    f_rtos_lightWeightEventWait(G_matiLWEvent_p, 1, 0x1, 0, 0);

    // if MATI is in thread safe test and we can trigger the call for overlay function
    if (G_pFrameWorkCB->uiTestCurrent == E_CB_TEST_AOM_THREAD_SAFE)
    {
      // call function from GROUP 102 that calls another function from GROUP 102 - GROUP size 0x200
      f_mati_test_func_overlay104_vect(&params);

      // increment the sync point to sign that we return from the tested function
      f_mati_secureSet( &G_pFrameWorkCB->uiTestSyncPoint,  M_INCREMENT(G_pFrameWorkCB->uiTestSyncPoint));

      // wait for the next critical section test
      f_rtos_lightWeightEventWait(G_matiLWEvent_p, 1, 0x1, 0, 0);

      // call FUNCTION in GROUP 104 (GROUP size 0xA00)
      f_mati_test_func_overlay106_vect(&params);

      // increment the sync point to finish the test
      f_mati_secureSet( &G_pFrameWorkCB->uiTestSyncPoint,  M_INCREMENT(G_pFrameWorkCB->uiTestSyncPoint));

    }
    else if (G_pFrameWorkCB->uiTestCurrent == E_CB_TEST_AOM_CRITICAL_SECTION)
    {
      // call function from GROUP 102 that calls another function from GROUP 102 - GROUP size 0x200
      f_mati_test_func_overlay104_vect(&params);
    }

    // reassign the GMT and BPM thread priority
    f_rtos_taskSetPriority(M_MATI_BUILD_TASKID( D_PROCESSOR_NUMBER, D_GMT_THREAD_TEMPLATE_INDEX), D_GMT_THREAD_PRIORITY, &uiPriority, 0);
    f_rtos_taskSetPriority(M_MATI_BUILD_TASKID( D_PROCESSOR_NUMBER, D_BPM_THREAD_TEMPLATE_INDEX), D_MATI_BPM_THREAD_NEW_PRIORITY, &uiPriority, 0);
  }
#endif /* 0 ronen */
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_raise_swi_for_thread_safe_test_and_yield  **/
/**   Description      : test the aom thread safe core                    **/
/**   Input parameters : Control Block                                    **/
/**   Output parameters: None                                             **/
/**   Return value     : MATI_OK if succeed, MATI_ERROR otherwise.        **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
void f_mati_raise_swi_for_thread_safe_test_and_yield(void)
{
#if 0 /* ronen */
  if (G_pFrameWorkCB->uiTestCurrent == E_CB_TEST_AOM_THREAD_SAFE)
  {
    // we use the first entry to the read backing store to read the offset table
    if (G_pFrameWorkCB->uiTestSyncPoint == 1)
    {
      f_mati_secureSet( &G_pFrameWorkCB->uiTestSyncPoint,  M_INCREMENT(G_pFrameWorkCB->uiTestSyncPoint));
    }

    // we use the second entry to the read backing store to read the second function GROUP
    else if (G_pFrameWorkCB->uiTestSyncPoint == 2 && G_mati_aom_funcs_hit_counter.uiFileSystemRead == 2 && G_mati_aom_funcs_hit_counter.uiAomOverlayLoader==1)
    {
      f_psp_intTriggerSwi(D_MATI_EXCPT_SWI_VECTOR, TRUE);
    }
    // we use the fourth entry to the read backing store to read the fourth function GROUP
    else if (G_pFrameWorkCB->uiTestSyncPoint == 4 && G_mati_aom_funcs_hit_counter.uiFileSystemRead == 3 && G_mati_aom_funcs_hit_counter.uiAomOverlayLoader==3)
    {
      f_psp_intTriggerSwi(D_MATI_EXCPT_SWI_VECTOR, TRUE);
    }
  }
#endif /* 0 ronen */
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_aom_sw_isr_change_gmt_prio                **/
/**   Description      : the isr function that initiate from GMT thread   **/
/**                      by raising the thread priority                   **/
/**   Input parameters : None                                             **/
/**   Output parameters: None                                             **/
/**   Return value     : None                                             **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
void /*D_EXCEPTION*/ f_mati_aom_sw_isr_change_gmt_prio(void* dd)
{
#if 0 /* ronen */
  u32_t uiOldTaskPrio = 0;

  // we write 0 to the HINT register to clear the interrupts
  f_psp_intTriggerSwi(D_MATI_EXCPT_SWI_VECTOR, FALSE);
  //_sr( 0, 0x201);

  f_rtos_taskSetPriority(M_MATI_BUILD_TASKID( D_PROCESSOR_NUMBER, D_GMT_THREAD_TEMPLATE_INDEX), D_MATI_GMT_THREAD_NEW_PRIORITY, &uiOldTaskPrio, 0);
  f_rtos_taskSetPriority(M_MATI_BUILD_TASKID( D_PROCESSOR_NUMBER, D_BPM_THREAD_TEMPLATE_INDEX), D_MATI_BPM_THREAD_NEW_PRIORITY, &uiOldTaskPrio, 0);

  // to trigger the light weight event that the GMT thread is waiting on
  f_rtos_lightWeightEventSet( G_matiLWEvent_p, 1, 0x1);
#endif /* ronen */
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_aom_test_thread_safe                      **/
/**   Description      : test the aom thread safe core                    **/
/**   Input parameters : Control Block                                    **/
/**   Output parameters: None                                             **/
/**   Return value     : MATI_OK if succeed, MATI_ERROR otherwise.        **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
E_TEST_ERROR f_mati_aom_test_thread_safe(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{
#if 0 /* rnen */
  pRTOS_User_ISR_t GOriginalUserISR_p;

  // replace the SWI function with the new one //pRTOS_Kernel_ISR_t
  GOriginalUserISR_p = M_INT_INSTALL_ISR(D_MATI_EXCPT_SWI_VECTOR, f_mati_aom_sw_isr_change_gmt_prio, (void*)0);

  M_PSP_INT_ENABLE_CORE_INT_NUM(D_MATI_EXCPT_SWI_VECTOR);

  //set the sync point to trigger the GMT thread
  f_mati_secureSet( &pFrameWorkCB->uiTestSyncPoint,  M_INCREMENT(pFrameWorkCB->uiTestSyncPoint));

  // call FUNCTION in GROUP 101 - GROUP size 0x200
  // to initiate the first thread safe test (read_backing_store)
  f_mati_test_func_overlay101_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

  // wait for the first part (non _task_stop_preemption() part test) to end
  if ( f_mati_busyWaitSleep( &pFrameWorkCB->uiTestSyncPoint, 3, D_TIMEOUT_WAIT) > 0 )
  {
    f_mati_secureSet( &pFrameWorkCB->uiTestSyncPoint,  M_INCREMENT(pFrameWorkCB->uiTestSyncPoint));

    // call FUNCTION in GROUP 103 - GROUP size is 0x200
    // to initiate the second thread safe test (AOM_overlay_loader)

    f_mati_test_func_overlay103_vect(pFrameWorkCB->sTestTasksFunctionParamArr);

    // wait for the first part (non _task_stop_preemption() part test) to end
    if ( f_mati_busyWaitSleep( &pFrameWorkCB->uiTestSyncPoint, 4, D_TIMEOUT_WAIT) == 0 )
    {
      f_mati_setErrorBit( pFrameWorkCB,  E_TEST_ERROR_AOM_THREAD_SAFE_READ_BACK_STORE);
    }
  }
  else
  {
    f_mati_setErrorBit(pFrameWorkCB, E_TEST_ERROR_AOM_THREAD_SAFE_READ_BACK_STORE);
  }

  // assign the original SWI ISR
  M_INT_INSTALL_ISR(D_MATI_EXCPT_SWI_VECTOR, GOriginalUserISR_p, (void*)0);

  G_pFrameWorkCB->uiTestSyncPoint = 0;
#endif /* ronen */
  return f_mati_getErrorBits(pFrameWorkCB);
}
















/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_verify_overlay_crc                        **/
/**   Description      : Calculate and compare loaded ovl CRC             **/
/**   Input parameters : omt_entry_t  overlay descriptor                  **/
/**   Output parameters: None                                             **/
/**   Return value     : MATI_OK if CRC match MATI_ERROR otherwise.       **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
E_MATI_Result f_mati_verify_overlay_crc(/*omt_entry_t*/void* omtEntry_p)
{
#if 0 /* ronen */
   u32_t crc;
   u32_t precomputed;
   // Calling for CRC calculation on a loaded page.
   // last 4 bytes of the page contain a compile time calculated crc32.
   // compare the calculated value with the one in the overlay.
   crc = _crc32(-1, (const char *)omtEntry_p->pc_start, omtEntry_p->pc_size - 4);
   precomputed = *(u32_t*)(((u32_t)omtEntry_p->pc_start) + ((u32_t)omtEntry_p->pc_size) - 4);

   return (crc == precomputed ? E_MATI_OK : E_MATI_ERROR);
#else
   return 0;
#endif /*0 ronen */
}

/***************************************************************************/
/**                                                                       **/
/**   Function Name    : f_mati_verify_overlay_crc                        **/
/**   Description      : Calculate and compare loaded ovl CRC             **/
/**   Input parameters : omt_entry_t  overlay descriptor                  **/
/**   Output parameters: None                                             **/
/**   Return value     : MATI_OK if CRC match MATI_ERROR otherwise.       **/
/**   Remarks          :                                                  **/
/**                                                                       **/
/***************************************************************************/
E_TEST_ERROR f_mati_aom_test_CRC_check(S_FRAMEWORK_CONTROL_BLOCK_PTR pFrameWorkCB)
{
#if 0 /* ronen */
   u32_t i;

   omt_entry_t *omtEntry_p;
   /// Calling overlay functions, So that we would have loaded overlays.
   f_mati_test_func_overlay101_vect(pFrameWorkCB->sTestTasksFunctionParamArr); // call FUNCTION in GROUP 101 - GROUP size is 0x200
   f_mati_test_func_overlay102_vect(pFrameWorkCB->sTestTasksFunctionParamArr); // call FUNCTION in GROUP 102 - GROUP size is 0x200
   f_mati_test_func_overlay103_vect(pFrameWorkCB->sTestTasksFunctionParamArr); // call FUNCTION in GROUP 103 - GROUP size is 0x200

   /// Go over overlay table entries and if they are loaded, check their CRC.
   for( i = 0 ; i <= OVERLAY_TABLE_ENTRIES ; i++  )
   {
      omtEntry_p = omt(i);
      if( LOADED_ENTRY_FLAG & omtEntry_p->flags )
      {
         if(E_MATI_OK != f_mati_verify_overlay_crc(omtEntry_p))
         {
            f_mati_setErrorBit(pFrameWorkCB, E_TEST_ERROR_AOM_OVL_CRC_ERROR);
         }
      }
   }// for OVERLAY_TABLE_ENTRIES
#endif /* ronen */
   return f_mati_getErrorBits(pFrameWorkCB);
}
/********************************************************************************/

///-------------------------------------------------------------------------
/// AOM overlay  functions
///-------------------------------------------------------------------------

///----------------------------------------------------------
/// run at GROUP 101 - GROUP size 0x200
///----------------------------------------------------------
void OVL_f_mati_test_func_overlay101_vect f_mati_test_func_overlay101_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR pFrameWorkCB)
{
   u32_t* newVar = &GlobalVar;
   u32_t loopCount = 13;

   *newVar = 0;
   while( loopCount--)
   {
      *newVar +=1;
   }
}

///----------------------------------------------------------
/// run at GROUP 102 and call function at GROUP 103 - GROUP size 0x200
///----------------------------------------------------------
void OVL_f_mati_test_func_overlay102_vect f_mati_test_func_overlay102_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR pFrameWorkCB)
{
   u32_t* newVar = &GlobalVar;
   u32_t loopCount = 100;

   *newVar = 0;
   while( loopCount--)
   {
      *newVar +=1;
   }

   // call FUNCTION in GROUP 103 - GROUP size is 0x200
   f_mati_test_func_overlay103_vect(pFrameWorkCB);
}

///----------------------------------------------------------
/// run at GROUP 103 - GROUP size 0x200
///----------------------------------------------------------
void OVL_f_mati_test_func_overlay103_vect f_mati_test_func_overlay103_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR pFrameWorkCB)
{
   u32_t* newVar = &GlobalVar;
   u32_t loopCount = 50;

   *newVar = 0;
   while( loopCount--)
   {
      *newVar +=1;
   }
}

///----------------------------------------------------------
/// run at GROUP 102 and call function at GROUP 102 - GROUP size 0x200
///----------------------------------------------------------
void OVL_f_mati_test_func_overlay104_vect f_mati_test_func_overlay104_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR pFrameWorkCB)
{
   u32_t* newVar = &GlobalVar;
   u32_t loopCount = 10;

   *newVar = 0;
   while( loopCount--)
   {
      *newVar +=1;
   }

   // call FUNCTION in GROUP 102 - GROUP size is 0x200
   f_mati_test_func_overlay107_vect(pFrameWorkCB);
}

///----------------------------------------------------------
/// run at GROUP 100 and at GROUP 101 && 102 in case of multigroup - GROUP size 0x200
///----------------------------------------------------------
void OVL_f_mati_test_func_overlay105_vect f_mati_test_func_overlay105_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR pFrameWorkCB)
{
   u32_t* newVar = &GlobalVar;
   u32_t loopCount = 25;

   *newVar = 0;

   while( loopCount--)
   {
      *newVar +=1;
   }
}

///----------------------------------------------------------
/// run at GROUP 103 - GROUP size 0xA00
///----------------------------------------------------------
void OVL_f_mati_test_func_overlay106_vect f_mati_test_func_overlay106_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR pFrameWorkCB)
{
   u32_t* newVar = &GlobalVar;
   u32_t loopCount = 256;
   u32_t* new_arr = tempArr;
   *newVar = 0;

   M_MATI_100_NOPS;
   M_MATI_100_NOPS;
   M_MATI_100_NOPS;
   M_MATI_50_NOPS;
   M_MATI_10_NOPS;
   M_MATI_10_NOPS;
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");
   asm volatile ("nop");

   while( loopCount--)
   {
      *newVar +=1;
      new_arr[loopCount] = (*newVar)++ ;

      if (new_arr[loopCount] % 7 == 0)
      {
         new_arr[loopCount]++;
      }
   }

   M_MATI_100_NOPS;
   M_MATI_50_NOPS;
   M_MATI_10_NOPS;
   M_MATI_10_NOPS;
   M_MATI_10_NOPS;
   M_MATI_10_NOPS;
}

///----------------------------------------------------------
/// run at GROUP 102 - GROUP at size 0x200
///----------------------------------------------------------
void OVL_f_mati_test_func_overlay107_vect f_mati_test_func_overlay107_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR pFrameWorkCB)
{
   u32_t* newVar = &GlobalVar;
   u32_t loopCount = 75;

   *newVar = 0;

   while( loopCount--)
   {
      *newVar +=1;
   }
}

///----------------------------------------------------------
/// run at GROUP 105 - GROUP at size 0x400
///----------------------------------------------------------
void  OVL_f_mati_test_func_overlay108_vect  f_mati_test_func_overlay108_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR pFrameWorkCB)
{
   u32_t* newVar = &GlobalVar;
   u32_t loopCount = 13;

   *newVar = 0;
   M_MATI_100_NOPS;
   M_MATI_50_NOPS;
   M_MATI_10_NOPS;
   M_MATI_10_NOPS;
   M_MATI_10_NOPS;
   M_MATI_10_NOPS;

   while( loopCount--)
   {
      *newVar +=1;
   }
}

///----------------------------------------------------------
/// run at GROUP 106 - GROUP at size 0x400
///----------------------------------------------------------
void OVL_f_mati_test_func_overlay109_vect f_mati_test_func_overlay109_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR pFrameWorkCB)
{
   u32_t* newVar = &GlobalVar;
   u32_t loopCount = 13;

   *newVar = 0;
   M_MATI_100_NOPS;

   while( loopCount--)
   {
      *newVar +=1;
   }
}

///----------------------------------------------------------
/// run at GROUP 107 - GROUP at size 0x600
///----------------------------------------------------------
void OVL_f_mati_test_func_overlay110_vect f_mati_test_func_overlay110_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR pFrameWorkCB)
{
   u32_t* newVar = &GlobalVar;
   u32_t loopCount = 13;

   *newVar = 0;

   while( loopCount--)
   {
      *newVar +=1;
   }

   M_MATI_100_NOPS;
   M_MATI_100_NOPS;
   M_MATI_50_NOPS;
   M_MATI_10_NOPS;
   M_MATI_10_NOPS;
}

///----------------------------------------------------------
/// run at GROUP 107 - GROUP at size 0xc00
///----------------------------------------------------------
void OVL_f_mati_test_func_overlay111_vect f_mati_test_func_overlay111_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR pFrameWorkCB)
{
   u32_t* newVar = &GlobalVar;
   u32_t loopCount = 13;

  *newVar = 0;

   while( loopCount--)
   {
      *newVar +=1;
   }

   M_MATI_100_NOPS;
   M_MATI_100_NOPS;
   M_MATI_100_NOPS;
   M_MATI_100_NOPS;
   M_MATI_100_NOPS;
   M_MATI_100_NOPS;
   M_MATI_50_NOPS;
   M_MATI_50_NOPS;
   M_MATI_10_NOPS;
   M_MATI_10_NOPS;
   M_MATI_10_NOPS;
   M_MATI_10_NOPS;
}

///----------------------------------------------------------
/// run at GROUP 109 - GROUP at size 0x800
///----------------------------------------------------------
void OVL_f_mati_test_func_overlay112_vect f_mati_test_func_overlay112_vect(S_FRAMEWORK_CONTROL_BLOCK_FUNCTION_PARAM_PTR pFrameWorkCB)
{
   u32_t* newVar = &GlobalVar;
   u32_t loopCount = 13;

  *newVar = 0;

   while( loopCount--)
   {
      *newVar +=1;
   }

   M_MATI_100_NOPS;
   M_MATI_100_NOPS;
   M_MATI_100_NOPS;
   M_MATI_100_NOPS;
   M_MATI_10_NOPS;
   M_MATI_10_NOPS;
   M_MATI_10_NOPS;
   M_MATI_10_NOPS;
}
