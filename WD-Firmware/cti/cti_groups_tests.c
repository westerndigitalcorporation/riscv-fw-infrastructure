/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2020 Western Digital Corporation or its affiliates.
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
* @file   cti_groups_tests.c
* @author Ronen Haen
* @date   01.07.2020
* @brief  The file implements cti framework
*/

/************************************************************
 * the overlay area is 4KB
 * the overlay table occupied 2KB
 * thus we have 2KB for our groups
 ***********************************************************/
/**
* include files
*/
#include "common_types.h"
#include "comrv_api.h"
#include "cti.h"
#include "psp_api.h"
#ifdef D_COMRV_RTOS_SUPPORT
   #include "rtosal_event_api.h"
#endif /* D_COMRV_RTOS_SUPPORT */

/**
* macros
*/
#define M_CTI_DUMMY_FUNCTION(x) \
   void _OVERLAY_ ctiTestFuncOverlay ## x ## Vect() \
   { \
      M_CTI_10_NOPS \
      M_CTI_10_NOPS \
      M_CTI_10_NOPS \
   };  \

/**
* definitions
*/
#define D_CTI_OVERLAY_TABLE_ENTRIES      (3)
#define D_CTI_NUM_OF_LOOPS               13
#define D_CTI_SUM_OF_ARGS                6

/* generate ovl dummy function to fill the ovelay area */
#define M_CTI_FUNCTIONS_GENERATOR \
  M_CTI_DUMMY_FUNCTION(120) \
  M_CTI_DUMMY_FUNCTION(121) \
  M_CTI_DUMMY_FUNCTION(122) \
  M_CTI_DUMMY_FUNCTION(123) \
  M_CTI_DUMMY_FUNCTION(124) \
  M_CTI_DUMMY_FUNCTION(125) \
  M_CTI_DUMMY_FUNCTION(126) \
  M_CTI_DUMMY_FUNCTION(127) \
  M_CTI_DUMMY_FUNCTION(128) \
  M_CTI_DUMMY_FUNCTION(129) \
  M_CTI_DUMMY_FUNCTION(130) \
  M_CTI_DUMMY_FUNCTION(131) \
  M_CTI_DUMMY_FUNCTION(132) \
  M_CTI_DUMMY_FUNCTION(133) \
  M_CTI_DUMMY_FUNCTION(134) \
  M_CTI_DUMMY_FUNCTION(135) \
  M_CTI_DUMMY_FUNCTION(136) \
  M_CTI_DUMMY_FUNCTION(137) \
  M_CTI_DUMMY_FUNCTION(138) \
  M_CTI_DUMMY_FUNCTION(139) \
  M_CTI_DUMMY_FUNCTION(140) \
  M_CTI_DUMMY_FUNCTION(141) \
  M_CTI_DUMMY_FUNCTION(142) \
  M_CTI_DUMMY_FUNCTION(143) \
  M_CTI_DUMMY_FUNCTION(144) \
  M_CTI_DUMMY_FUNCTION(145) \
  M_CTI_DUMMY_FUNCTION(146) \
  M_CTI_DUMMY_FUNCTION(147) \
  M_CTI_DUMMY_FUNCTION(148) \
  M_CTI_DUMMY_FUNCTION(149) \
  M_CTI_DUMMY_FUNCTION(150) \
  M_CTI_DUMMY_FUNCTION(151) \
  M_CTI_DUMMY_FUNCTION(152) \
  M_CTI_DUMMY_FUNCTION(153) \
  M_CTI_DUMMY_FUNCTION(154) \
  M_CTI_DUMMY_FUNCTION(155) \
  M_CTI_DUMMY_FUNCTION(156) \
  M_CTI_DUMMY_FUNCTION(157) \
  M_CTI_DUMMY_FUNCTION(158) \
  M_CTI_DUMMY_FUNCTION(159)

/**
* types
*/

/**
* local prototypes
*/
void         ctiTaskBTest(void);
E_TEST_ERROR ctiOvlTestSanity(S_FW_CB_PTR pCtiFrameWorkCB);
E_TEST_ERROR ctiOvlTestCrcCheck(S_FW_CB_PTR pCtiFrameWorkCB);
E_TEST_ERROR ctiOvlTestMultigroup(S_FW_CB_PTR pCtiFrameWorkCB);
E_TEST_ERROR ctiOvlTestThreadSafe(S_FW_CB_PTR pCtiFrameWorkCB);
E_TEST_ERROR ctiOvlTestLockUnlock(S_FW_CB_PTR pCtiFrameWorkCB);
E_TEST_ERROR ctiOvlTestGroupingOvlt(S_FW_CB_PTR pCtiFrameWorkCB);
E_TEST_ERROR ctiOvlTestCriticalSection(S_FW_CB_PTR pCtiFrameWorkCB);
E_TEST_ERROR ctiOvlTestGroupWithSameSize(S_FW_CB_PTR pCtiFrameWorkCB);
E_TEST_ERROR ctiOvlTestGroupWithDifferentSize(S_FW_CB_PTR pCtiFrameWorkCB);
E_TEST_ERROR ctiOvlTestDefragOverlayedFunctions(S_FW_CB_PTR pCtiFrameWorkCB);

/**
* external prototypes
*/
extern void demoComrvSetErrorHandler(void* fptrAddress);

/**
* global variables
*/
u32_t g_uiVar;
u32_t g_uiTempArr[256] = {0};
stCtiFuncsHitCounter g_stCtiOvlFuncsHitCounter;
void* G_pProgramCounterAddress;
#ifdef D_COMRV_RTOS_SUPPORT
   extern rtosalEventGroup_t stRtosalEventGroupCb;
#endif /* D_COMRV_RTOS_SUPPORT */

const ctiTestFunctionPtr g_pLookupTableCtiTestOvl[E_CB_TEST_OVL_MAX] =
{
  ctiOvlTestSanity,                    /* E_CB_TEST_OVL_GROUP_SANITY */
  ctiOvlTestGroupWithSameSize,         /* E_CB_TEST_OVL_GROUP_WITH_SAME_SIZE */
  ctiOvlTestGroupWithDifferentSize,    /* E_CB_TEST_OVL_GROUP_WITH_DIFFERENT_SIZE */
  ctiOvlTestMultigroup,                /* E_CB_TEST_OVL_GROUP_MULTIGROUP */
  ctiOvlTestLockUnlock,                /* E_CB_TEST_OVL_GROUP_LOCK_UNLOCK */
  ctiOvlTestDefragOverlayedFunctions,  /* E_CB_TEST_OVL_GROUP_DEFRAG_OVERLAY_MEMORY */
  ctiOvlTestGroupingOvlt,              /* E_CB_TEST_OVL_GROUP_OVLA */
  ctiOvlTestCrcCheck,                  /* E_CB_TEST_OVL_OVL_CRC_CHECK */
  ctiOvlTestCriticalSection,           /* E_CB_TEST_OVL_CRITICAL_SECTION */
  ctiOvlTestThreadSafe,                /* E_CB_TEST_OVL_THREAD_SAFE */
};

/*
* the following groups contains the following functions:
*
*group 101:
*  ctiTestFuncOverlay101Vect,
*  ctiTestFuncOverlay105Vect
*
*group 102
*  ctiTestFuncOverlay105Vect,
*  ctiTestFuncOverlay102Vect,
*  ctiTestFuncOverlay104Vect,
*  ctiTestFuncOverlay107Vect
*
*group 103:
*  ctiTestFuncOverlay103Vect
*
*group 104:
*  ctiTestFuncOverlay106Vect
*
*group 105:
*  ctiTestFuncOverlay108Vect
*
*group 106:
*  ctiTestFuncOverlay109Vect not used in code
*
*group 107:
*  ctiTestFuncOverlay110Vect
*
*group 108:
*  ctiTestFuncOverlay111Vect
*
*group 109:
*  ctiTestFuncOverlay112Vect
*/

/* define dummy functions to fill the overlay */
M_CTI_FUNCTIONS_GENERATOR

/**
* @brief error function pointer used by some tests
*
* @param  errornum - error number
*
* @return errornum - error number
*/
u32_t ctiOvlUserSystemErrorVect(const comrvErrorArgs_t* pErrorArgs)
{
   g_stCtiOvlFuncsHitCounter.uiErrorNum = pErrorArgs->uiErrorNum;
   return pErrorArgs->uiErrorNum;
}

/**
* @brief sanity test: verify ovl groups are loaded when needed
*
* @param  structure that holds 3 parameters
*
* @return error bit map
*/
E_TEST_ERROR ctiOvlTestSanity(S_FW_CB_PTR pCtiFrameWorkCB)
{
   /* call FUNCTION in GROUP 101 - GROUP size 0x200 */
   ctiTestFuncOverlay101Vect();

   /* we expect 1 hits in file_syetem_read for the overlay function in GROUP 101 */
   if (g_stCtiOvlFuncsHitCounter.uiComrvLoad != 1)
   {
      ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_FAILED);
   }
   else
   {
      /* call FUNCTION in GROUP 103 - GROUP size is 0x200 */
      ctiTestFuncOverlay103Vect();

      /* we expect 1 hits in file_syetem_read; none for the lookup table, one for the overlay function in GROUP 103 */
      if (g_stCtiOvlFuncsHitCounter.uiComrvLoad != 2)
      {
         ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_SANITY_FAILED);
      }
      else
      {
         /* call FUNCTION in GROUP 102 - GROUP size 0x200 */
         ctiTestFuncOverlay102Vect();

         /* call function from GROUP 102 that calls another function from GROUP 102 - GROUP size 0x200 */
         ctiTestFuncOverlay104Vect();

         /* we expect 0 hits in file_syetem_read because the function supposed to be resident */
         if (g_stCtiOvlFuncsHitCounter.uiComrvLoad != 3)
         {
            ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_SANITY_FAILED);
         }
      }
   }

   return ctiGetErrorBits(pCtiFrameWorkCB);
}

/**
* @brief Groups with the same size: verify ovl groups are loaded when
*        needed while the groups with the same size
*
* @param  structure that holds 3 parameters
*
* @return error bit map
*/
E_TEST_ERROR ctiOvlTestGroupWithSameSize(S_FW_CB_PTR pCtiFrameWorkCB)
{
   /* call FUNCTION in GROUP 101 - GROUP size 0x200 */
   ctiTestFuncOverlay101Vect();

   /* we expect 1 hit in file_syetem_read; the overlay function in GROUP 101 */
   if (g_stCtiOvlFuncsHitCounter.uiComrvLoad != 1)
   {
      ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_FAILED);
   }
   else
   {
      /* call FUNCTION in GROUP 101 and 102 - GROUP size 0x200 */
      ctiTestFuncOverlay105Vect();

      /* we expect 0 hits in file_syetem_read because the function was loaded in GROUP 101 */
      if (g_stCtiOvlFuncsHitCounter.uiComrvLoad != 1)
      {
         ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_UNIFORM_GROUPS_FAILED);
      }
      else
      {
         /* call FUNCTION in GROUP 103 - GROUP size is 0x200 */
         ctiTestFuncOverlay103Vect();

         /* we expect 1 hits in file_syetem_read; for the overlay function in GROUP 103 */
         if (g_stCtiOvlFuncsHitCounter.uiComrvLoad != 2)
         {
            ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_UNIFORM_GROUPS_FAILED);
         }
      }
   }

   return ctiGetErrorBits(pCtiFrameWorkCB);
}

/**
* @brief Groups with the same size: verify ovl groups are loaded when
*           needed while the groups with the same size
*
* @param  structure that holds 3 parameters
*
* @return error bit map
*/
E_TEST_ERROR ctiOvlTestGroupWithDifferentSize(S_FW_CB_PTR pCtiFrameWorkCB)
{
   /* call FUNCTION in GROUP 101 - GROUP size 0x200 */
   ctiTestFuncOverlay101Vect();

   /* we expect 1 hits in file_syetem_read; one for the lookup table and one for the overlay 
      function in GROUP 101 */
   if (g_stCtiOvlFuncsHitCounter.uiComrvLoad != 1)
   {
      ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_FAILED);
   }
   else
   {
      /* call FUNCTION in GROUP 104 (GROUP size 0xA00), the GROUP is bigger than GROUP 101 */
      ctiTestFuncOverlay106Vect();

      /* we expect 1 hits in file_syetem_read; for the overlay function in GROUP 104 */
      if (g_stCtiOvlFuncsHitCounter.uiComrvLoad != 2)
      {
         ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_NON_UNIFORM_GROUPS_FAILED);
      }
   }

   return ctiGetErrorBits(pCtiFrameWorkCB);
}

/**
* @brief multiple Groups: verify ovl groups are loaded when
*        needed while the groups with the same size
*
* @param  structure that holds 3 parameters
*
* @return error bit map
*/
E_TEST_ERROR ctiOvlTestMultigroup(S_FW_CB_PTR pCtiFrameWorkCB)
{
#ifdef D_COMRV_ENABLE_MULTI_GROUP_SUPPORT

   void* pOverlayFunctionPC;

   /* call FUNCTION in GROUP 101 - GROUP size 0x200 */
   ctiTestFuncOverlay101Vect();

   /* call MG function group 101 */
   ctiTestFuncOverlay105Vect();

   /* we expect 1 hit in file_syetem_read; for the overlay function in GROUP 101 */
   if (g_stCtiOvlFuncsHitCounter.uiComrvLoad != 1)
   {
      ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_FAILED);
      return ctiGetErrorBits(pCtiFrameWorkCB);
   }

   /* save the first multigroup function instance address */
   pOverlayFunctionPC = G_pProgramCounterAddress;

   /* fill the cache area with by calling overlay functions - will evict the first GROUP */
   #undef M_CTI_DUMMY_FUNCTION
   #define M_CTI_DUMMY_FUNCTION(x)  ctiTestFuncOverlay ## x ## Vect();
   M_CTI_FUNCTIONS_GENERATOR

   /* call FUNCTION in GROUP 102 - GROUP size 0x200 */
   ctiTestFuncOverlay102Vect();

   if (g_stCtiOvlFuncsHitCounter.uiComrvLoad != 43)
   {
      ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_MULTI_GROUPS_FAILED);
      return ctiGetErrorBits(pCtiFrameWorkCB);
   }

   /* call MG function group 105 */
   ctiTestFuncOverlay105Vect();

   /* we expect no hit in file_syetem_read because the function is already resident */
   if (g_stCtiOvlFuncsHitCounter.uiComrvLoad != 43)
   {
      ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_MULTI_GROUPS_FAILED);
      return ctiGetErrorBits(pCtiFrameWorkCB);
   }

   /* compare the addresses of the MG function */
   if (pOverlayFunctionPC == G_pProgramCounterAddress)
   {
     ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_MULTI_GROUPS_FAILED);
   }

#endif /* D_COMRV_ENABLE_MULTI_GROUP_SUPPORT */

   return ctiGetErrorBits(pCtiFrameWorkCB);
}

/**
* @brief Lock / Unlock Groups: verify ovl groups are loaded when
*        needed while the groups with the same size
*
* @param  structure that holds 3 parameters
*
* @return error bit map
*/
E_TEST_ERROR ctiOvlTestLockUnlock(S_FW_CB_PTR pCtiFrameWorkCB)
{
   u08_t ucCounter = 10;

   #undef M_CTI_DUMMY_FUNCTION
   #define M_CTI_DUMMY_FUNCTION(x)  ctiTestFuncOverlay ## x ## Vect();
   M_CTI_FUNCTIONS_GENERATOR

   /* call FUNCTION in GROUP 101 - GROUP size is 0x200 */
   ctiTestFuncOverlay101Vect();
   /* call FUNCTION in GROUP 102 - GROUP size is 0x200 */
   ctiTestFuncOverlay102Vect();
   /* call FUNCTION in GROUP 103 - GROUP size is 0x200 */
   ctiTestFuncOverlay103Vect();
   /* call FUNCTION in GROUP 102 - GROUP size is 0x200 */
   ctiTestFuncOverlay104Vect();
   /* call FUNCTION in GROUP 101 - GROUP size is 0x200 */
   ctiTestFuncOverlay105Vect();
   /* call FUNCTION in GROUP 102 - GROUP size is 0x200 */
   ctiTestFuncOverlay107Vect();

   while (ucCounter--)
   {
      /* run at GROUP 107 - GROUP at size 0xc00 */
      ctiTestFuncOverlay111Vect();
   }

   comrvLockUnlockOverlayGroupByFunction((void*)ctiTestFuncOverlay111Vect, D_COMRV_GROUP_STATE_LOCK);

   /* run at GROUP 109 - GROUP at size 0x800 */
   ctiTestFuncOverlay112Vect();

   /* call FUNCTION in GROUP 101 - GROUP size 0x200 */
   ctiTestFuncOverlay101Vect();

   if (g_stCtiOvlFuncsHitCounter.uiComrvLoad != 45)
   {
      ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_FAILED);
   }
   else
   {
      /* lock GROUP 101 */
      comrvLockUnlockOverlayGroupByFunction((void*)ctiTestFuncOverlay101Vect, D_COMRV_GROUP_STATE_LOCK);

      /* fill the resident area with dummy functions and try to evict the first GROUP */
      #undef M_CTI_DUMMY_FUNCTION
      #define M_CTI_DUMMY_FUNCTION(x)  ctiTestFuncOverlay ## x ## Vect();
      M_CTI_FUNCTIONS_GENERATOR

      /* call FUNCTION in GROUP 101 */
      ctiTestFuncOverlay101Vect();

      /* we expect 0 hits in file_syetem_read; the GROUP supposed to be locked */
      if (g_stCtiOvlFuncsHitCounter.uiComrvLoad != 85)
      {
         ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_LOCK_UNLOCK_FAILED);
      }
      else
      {
         /* unlock GROUP 101 */
         comrvLockUnlockOverlayGroupByFunction((void*)ctiTestFuncOverlay101Vect, D_COMRV_GROUP_STATE_UNLOCK);

         /* fill the resident area with dummy functions and try to evict the first GROUP */
         M_CTI_FUNCTIONS_GENERATOR

         /* call FUNCTION in GROUP 101 */
         ctiTestFuncOverlay101Vect();

         /* we expect 1 hits in file_syetem_read; the GROUP supposed to be evicted */
         if (g_stCtiOvlFuncsHitCounter.uiComrvLoad != 126)
         {
            ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_LOCK_UNLOCK_FAILED);
         }
      }
   }

   return ctiGetErrorBits(pCtiFrameWorkCB);
}

/**
* @brief Defrag Overlay Area: verify ovl core using defrag
*
* @param  structure that holds 3 parameters
*
* @return error bit map
*/
E_TEST_ERROR ctiOvlTestDefragOverlayedFunctions(S_FW_CB_PTR pCtiFrameWorkCB)
{
   /* fill cache */
   ctiTestFuncOverlay120Vect();
   ctiTestFuncOverlay121Vect();
   ctiTestFuncOverlay122Vect();
   ctiTestFuncOverlay123Vect();
   ctiTestFuncOverlay124Vect();
   ctiTestFuncOverlay125Vect();
   ctiTestFuncOverlay126Vect();
   ctiTestFuncOverlay127Vect();
   ctiTestFuncOverlay128Vect();
   ctiTestFuncOverlay129Vect();
   ctiTestFuncOverlay130Vect();
   ctiTestFuncOverlay131Vect();
   ctiTestFuncOverlay132Vect();
   ctiTestFuncOverlay133Vect();
   ctiTestFuncOverlay134Vect();
   ctiTestFuncOverlay135Vect();
   ctiTestFuncOverlay136Vect();
   ctiTestFuncOverlay137Vect();
   ctiTestFuncOverlay138Vect();
   ctiTestFuncOverlay139Vect();
   ctiTestFuncOverlay140Vect();
   ctiTestFuncOverlay141Vect();
   ctiTestFuncOverlay142Vect();
   ctiTestFuncOverlay143Vect();
   ctiTestFuncOverlay144Vect();
   ctiTestFuncOverlay145Vect();
   ctiTestFuncOverlay146Vect();
   ctiTestFuncOverlay147Vect();
   ctiTestFuncOverlay148Vect();
   ctiTestFuncOverlay149Vect();
   ctiTestFuncOverlay150Vect();
   ctiTestFuncOverlay151Vect();

   /* make funcs 125, 135 and 147 to be the next eviction
      candidates by not calling them */
   ctiTestFuncOverlay120Vect();
   ctiTestFuncOverlay121Vect();
   ctiTestFuncOverlay122Vect();
   ctiTestFuncOverlay123Vect();
   ctiTestFuncOverlay124Vect();
   ctiTestFuncOverlay126Vect();
   ctiTestFuncOverlay127Vect();
   ctiTestFuncOverlay128Vect();
   ctiTestFuncOverlay129Vect();
   ctiTestFuncOverlay130Vect();
   ctiTestFuncOverlay131Vect();
   ctiTestFuncOverlay132Vect();
   ctiTestFuncOverlay133Vect();
   ctiTestFuncOverlay134Vect();
   ctiTestFuncOverlay136Vect();
   ctiTestFuncOverlay137Vect();
   ctiTestFuncOverlay138Vect();
   ctiTestFuncOverlay139Vect();
   ctiTestFuncOverlay140Vect();
   ctiTestFuncOverlay141Vect();
   ctiTestFuncOverlay142Vect();
   ctiTestFuncOverlay143Vect();
   ctiTestFuncOverlay144Vect();
   ctiTestFuncOverlay145Vect();
   ctiTestFuncOverlay146Vect();
   ctiTestFuncOverlay148Vect();
   ctiTestFuncOverlay149Vect();
   ctiTestFuncOverlay150Vect();
   ctiTestFuncOverlay151Vect();

   /* evict lock functions 126, 136 and 148 */
   comrvLockUnlockOverlayGroupByFunction(ctiTestFuncOverlay126Vect, D_COMRV_GROUP_STATE_LOCK);
   comrvLockUnlockOverlayGroupByFunction(ctiTestFuncOverlay136Vect, D_COMRV_GROUP_STATE_LOCK);
   comrvLockUnlockOverlayGroupByFunction(ctiTestFuncOverlay148Vect, D_COMRV_GROUP_STATE_LOCK);

   /* clear load counter */
   g_stCtiOvlFuncsHitCounter.uiComrvLoad = 0;

   /* reset counter */
   g_stCtiOvlFuncsHitCounter.uiDefragCounter = 0;

   /* lets call a 0x600 bytes overlay - we expect that
      funcs 125, 138 and 147 be evicted */
   ctiTestFuncOverlay110Vect();

   /* we expect more than 1 hit in defrag and 1 read in read */
   if (g_stCtiOvlFuncsHitCounter.uiDefragCounter < 1 ||
       g_stCtiOvlFuncsHitCounter.uiComrvLoad != 1)
   {
      ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_DEFRAG_FAILED);
      return ctiGetErrorBits(pCtiFrameWorkCB);
   }

   /* make sure other functions were not evicted */
   ctiTestFuncOverlay120Vect();
   ctiTestFuncOverlay121Vect();
   ctiTestFuncOverlay122Vect();
   ctiTestFuncOverlay123Vect();
   ctiTestFuncOverlay124Vect();
   ctiTestFuncOverlay126Vect();
   ctiTestFuncOverlay127Vect();
   ctiTestFuncOverlay128Vect();
   ctiTestFuncOverlay129Vect();
   ctiTestFuncOverlay130Vect();
   ctiTestFuncOverlay131Vect();
   ctiTestFuncOverlay132Vect();
   ctiTestFuncOverlay133Vect();
   ctiTestFuncOverlay134Vect();
   ctiTestFuncOverlay136Vect();
   ctiTestFuncOverlay137Vect();
   ctiTestFuncOverlay138Vect();
   ctiTestFuncOverlay139Vect();
   ctiTestFuncOverlay140Vect();
   ctiTestFuncOverlay141Vect();
   ctiTestFuncOverlay142Vect();
   ctiTestFuncOverlay143Vect();
   ctiTestFuncOverlay144Vect();
   ctiTestFuncOverlay145Vect();
   ctiTestFuncOverlay146Vect();
   ctiTestFuncOverlay148Vect();
   ctiTestFuncOverlay149Vect();
   ctiTestFuncOverlay150Vect();
   ctiTestFuncOverlay151Vect();
   if (g_stCtiOvlFuncsHitCounter.uiComrvLoad == 1)
   {
      /* make sure 125, 135 and 147 are evicted */
      ctiTestFuncOverlay125Vect();
      ctiTestFuncOverlay135Vect();
      ctiTestFuncOverlay147Vect();
      if (g_stCtiOvlFuncsHitCounter.uiComrvLoad == 4)
      {
         /* make sure locked functions were not evicted */
         ctiTestFuncOverlay126Vect();
         ctiTestFuncOverlay136Vect();
         ctiTestFuncOverlay148Vect();
         if (g_stCtiOvlFuncsHitCounter.uiComrvLoad != 4)
         {
            ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_DEFRAG_FAILED);
         }
         else
         {
            /* make sure locked overlays become lru */
            ctiTestFuncOverlay120Vect();
            ctiTestFuncOverlay121Vect();
            ctiTestFuncOverlay122Vect();
            ctiTestFuncOverlay123Vect();
            ctiTestFuncOverlay124Vect();
            ctiTestFuncOverlay125Vect();
            ctiTestFuncOverlay127Vect();
            ctiTestFuncOverlay128Vect();
            ctiTestFuncOverlay129Vect();
            ctiTestFuncOverlay130Vect();
            ctiTestFuncOverlay131Vect();
            ctiTestFuncOverlay132Vect();
            ctiTestFuncOverlay133Vect();
            ctiTestFuncOverlay134Vect();
            ctiTestFuncOverlay135Vect();
            ctiTestFuncOverlay137Vect();
            ctiTestFuncOverlay138Vect();
            ctiTestFuncOverlay139Vect();
            ctiTestFuncOverlay140Vect();
            ctiTestFuncOverlay141Vect();
            ctiTestFuncOverlay142Vect();
            ctiTestFuncOverlay143Vect();
            ctiTestFuncOverlay144Vect();
            ctiTestFuncOverlay145Vect();
            ctiTestFuncOverlay146Vect();
            ctiTestFuncOverlay147Vect();
            ctiTestFuncOverlay149Vect();
            ctiTestFuncOverlay150Vect();
            ctiTestFuncOverlay151Vect();
            /* calling the following functions will cause the
               eviction lru items to be spread such that the actual
               lru won't be used (locked) */
            ctiTestFuncOverlay121Vect();
            ctiTestFuncOverlay122Vect();
            ctiTestFuncOverlay123Vect();
            ctiTestFuncOverlay124Vect();
            /* reset counter */
            g_stCtiOvlFuncsHitCounter.uiDefragCounter = 0;
            /* no we want to load an overlay size of 0x600 and see that
               defrag works while keeping the locked lru as the real lru */
            ctiTestFuncOverlay110Vect();
            /* we expect more than 1 hit in defrag */
            if (g_stCtiOvlFuncsHitCounter.uiDefragCounter < 1)
            {
               ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_DEFRAG_FAILED);
            }
         }
      }
      else
      {
         ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_DEFRAG_FAILED);
      }
   }
   else
   {
      ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_DEFRAG_FAILED);
   }

   return ctiGetErrorBits(pCtiFrameWorkCB);
}

/**
* @brief grouping tool
*
* @param  structure that holds 3 parameters
*
* @return error bit map
*/
E_TEST_ERROR ctiOvlTestGroupingOvlt(S_FW_CB_PTR pCtiFrameWorkCB)
{
   u32_t uiLoopCount = 25;

   while( uiLoopCount--)
   {
      ctiTestFuncOverlay120Vect();
      ctiTestFuncOverlay121Vect();
   }

   uiLoopCount = 75;

   while (uiLoopCount--)
   {
      ctiTestFuncOverlay123Vect();
      ctiTestFuncOverlay124Vect();
   }

   uiLoopCount = 125;

   while (uiLoopCount-- )
   {
      ctiTestFuncOverlay125Vect();
      ctiTestFuncOverlay126Vect();
   }

   return ctiGetErrorBits(pCtiFrameWorkCB);
}

/**
* @brief critical section (FW POV): verify critical section (comrv
*                  disable/enable); this is used to verify
*                  that overlay functions are not been called
*
* @param  structure that holds 3 parameters
*
* @return error bit map
*/
E_TEST_ERROR ctiOvlTestCriticalSection(S_FW_CB_PTR pCtiFrameWorkCB)
{

   /* Assign a system error function to capture  when we fail on critical section */
   demoComrvSetErrorHandler(ctiOvlUserSystemErrorVect);

   /* call FUNCTION in GROUP 101 - GROUP size 0x200
      call the first ovl function to verify sanity */
   ctiTestFuncOverlay101Vect();

   /* comrv is working fine so we can test the disable/enable */
   if (g_stCtiOvlFuncsHitCounter.uiErrorNum == 0)
   {
      /* disable comrv */
      comrvDisable();

      /* call FUNCTION in GROUP 102 - GROUP size 0x200
         supposed to trigger an error.
         the error function will disable the critical section bit to avoid
         infinity loop */
      ctiTestFuncOverlay102Vect();

      if (g_stCtiOvlFuncsHitCounter.uiErrorNum == 0)
      {
         ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_CRITICAL_SECTION_FAILED);
      }

      /* enable comrv */
      comrvEnable();
   }

   /* clear the function pointer for the remaining tests */
   demoComrvSetErrorHandler(NULL);

   return ctiGetErrorBits(pCtiFrameWorkCB);
}

/**
* @brief Calculate and compare loaded overlay CRC
*
* @param  None
*
* @return E_TEST_ERROR
*/
E_TEST_ERROR ctiOvlTestCrcCheck(S_FW_CB_PTR pCtiFrameWorkCB)
{
   /* Assign a system error function to capture when we fail on on CRC */
   demoComrvSetErrorHandler(ctiOvlUserSystemErrorVect);

   /* call FUNCTION in GROUP 101 - GROUP size is 0x200 */
   ctiTestFuncOverlay101Vect();

   /* call FUNCTION in GROUP 102 - GROUP size is 0x200 */
   ctiTestFuncOverlay102Vect();

   /* call FUNCTION in GROUP 103 - GROUP size is 0x200 */
   ctiTestFuncOverlay103Vect();

   /* clear the function pointer for the remaining tests */
   demoComrvSetErrorHandler(NULL);

   /* check we didn't get a CRC error */
   if (g_stCtiOvlFuncsHitCounter.uiErrorNum != 0)
   {
      ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_CRC_FAILED);
   }

   return ctiGetErrorBits(pCtiFrameWorkCB);
}

/**
* @brief Test the ovl thread safe core
*
* @param  None
*
* @return E_TEST_ERROR
*/
E_TEST_ERROR ctiOvlTestThreadSafe(S_FW_CB_PTR pCtiFrameWorkCB)
{
#ifdef D_COMRV_RTOS_SUPPORT
   /* register software interrupt handler */
   pspMachineInterruptsRegisterIsr(ctiSwiIsr, E_MACHINE_SOFTWARE_CAUSE);

   /* enable sw int */
   pspMachineInterruptsEnableIntNumber(D_PSP_INTERRUPTS_MACHINE_SW);

   /* update sync point */
   pCtiFrameWorkCB->uiTestSyncPoint = D_CTI_TASK_SYNC_BEFORE_ENTER_CRITICAL_SEC;

   /* call FUNCTION in GROUP 101 - GROUP size 0x200
      to initiate the first thread safe test (read_backing_store) */
   ctiTestFuncOverlay101Vect();

   /* update sync point */
   pCtiFrameWorkCB->uiTestSyncPoint = D_CTI_TASK_SYNC_AFTER_ENTER_CRITICAL_SEC;

   /* call FUNCTION in GROUP 103 - GROUP size is 0x200
      to initiate the second thread safe test  */
   ctiTestFuncOverlay103Vect();

   /* update sync point */
   pCtiFrameWorkCB->uiTestSyncPoint = D_CTI_TASK_SYNC_BEFORE_EXIT_CRITICAL_SEC;

   /* call FUNCTION in GROUP 104 - GROUP size is 0x200
      to initiate the second thread safe test  */
   ctiTestFuncOverlay104Vect();

   /* update sync point */
   pCtiFrameWorkCB->uiTestSyncPoint = D_CTI_TASK_SYNC_AFTER_EXIT_CRITICAL_SEC;

   /* call FUNCTION in GROUP 106 - GROUP size is 0x200
      to initiate the second thread safe test  */
   ctiTestFuncOverlay106Vect();

   /* update sync point */
   pCtiFrameWorkCB->uiTestSyncPoint = D_CTI_TASK_SYNC_AFTER_SEARCH_LOAD;

   /* call FUNCTION in GROUP 113 - GROUP size is 0x200
      to initiate the second thread safe test  */
   ctiTestFuncOverlay113Vect(1, 2, 3);

   /* we expect 1 load - the high priority task completed running and cleared
     the load counter */
   if (g_stCtiOvlFuncsHitCounter.uiComrvLoad != 1)
   {
      ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_THREAD_SAFE_READ_BACK_STORE);
   }
#endif /* D_COMRV_RTOS_SUPPORT */

   return ctiGetErrorBits(pCtiFrameWorkCB);
}

#ifdef D_COMRV_RTOS_SUPPORT
/**
* @brief task b test function; test 2 tasks calling
*        overlay functions (high priority task)
*
* @param  None
*
* @return None
*/
void ctiTaskBTest(void)
{
   S_FW_CB_PTR pCtiFrameWorkCB = g_pCtiFrameWorkCB;
   rtosalEventBits_t stRtosalEventBits;

   /* sync point 1 */
   rtosalEventGroupGet(&stRtosalEventGroupCb, D_CTI_TASK_SYNC_BEFORE_ENTER_CRITICAL_SEC,
         &stRtosalEventBits, D_RTOSAL_OR_CLEAR, D_RTOSAL_WAIT_FOREVER);

   /* call function from GROUP 108 (GROUP size 0x400) */
   ctiTestFuncOverlay108Vect();

   /* we expect 1 load */
   if (g_stCtiOvlFuncsHitCounter.uiComrvLoad == 1)
   {
      /* sync point 2 */
      rtosalEventGroupGet(&stRtosalEventGroupCb, D_CTI_TASK_SYNC_AFTER_ENTER_CRITICAL_SEC,
            &stRtosalEventBits, D_RTOSAL_OR_CLEAR, D_RTOSAL_WAIT_FOREVER);

      /* call overlay func */
      ctiTestFuncOverlay110Vect();

      /* we expect total 3 loads */
      if (g_stCtiOvlFuncsHitCounter.uiComrvLoad == 3)
      {
         /* sync point 3 */
         rtosalEventGroupGet(&stRtosalEventGroupCb, D_CTI_TASK_SYNC_BEFORE_EXIT_CRITICAL_SEC,
               &stRtosalEventBits, D_RTOSAL_OR_CLEAR, D_RTOSAL_WAIT_FOREVER);

         /* call overlay func */
         ctiTestFuncOverlay111Vect();

         /* we expect total of 5 loads */
         if (g_stCtiOvlFuncsHitCounter.uiComrvLoad == 5)
         {
            /* sync point 4 */
            rtosalEventGroupGet(&stRtosalEventGroupCb, D_CTI_TASK_SYNC_AFTER_EXIT_CRITICAL_SEC,
                  &stRtosalEventBits, D_RTOSAL_OR_CLEAR, D_RTOSAL_WAIT_FOREVER);

            /* call overlay func */
            ctiTestFuncOverlay112Vect();

            /* we expect total of 7 loads */
            if (g_stCtiOvlFuncsHitCounter.uiComrvLoad == 7)
            {
               /* sync point 5 */
               rtosalEventGroupGet(&stRtosalEventGroupCb, D_CTI_TASK_SYNC_AFTER_SEARCH_LOAD,
                     &stRtosalEventBits, D_RTOSAL_OR_CLEAR, D_RTOSAL_WAIT_FOREVER);

               /* fill the resident area with dummy functions and try to evict the first GROUP */
               #undef M_CTI_DUMMY_FUNCTION
               #define M_CTI_DUMMY_FUNCTION(x)  ctiTestFuncOverlay ## x ## Vect();
               M_CTI_FUNCTIONS_GENERATOR

               /* clear load counter */
               g_stCtiOvlFuncsHitCounter.uiComrvLoad = 0;
            }
            else
            {
               ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_THREAD_SAFE_READ_BACK_STORE);
            }
         }
         else
         {
            ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_THREAD_SAFE_READ_BACK_STORE);
         }
      }
      else
      {
         ctiSetErrorBit(pCtiFrameWorkCB, E_TEST_ERROR_OVL_THREAD_SAFE_READ_BACK_STORE);
      }
   }
}

/**
* @brief this function is called when a swi is raised
*        (currently only applies to whisper)
*
* @param  None
*
* @return None
*/
void ctiSwiIsr(void)
{
   u32_t uiSyncPintVal = ctiGetCurrentSyncPoint();
   rtosalEventBits_t stRtosalEventBits;

   /* clear swi indication so we are not interrupted again */
   M_CTI_CLEAR_SW_INT();

   /* clear sync point so that comrv engine won't hit it again  */
   ctiSetCurrentSyncPoint(D_CTI_TASK_SYNC_NONE); \

   /* send an event - next sync point */
   rtosalEventGroupSet(&stRtosalEventGroupCb, uiSyncPintVal,
                       D_RTOSAL_OR, &stRtosalEventBits);
}
#endif /* D_COMRV_RTOS_SUPPORT */

/**
* @brief run at GROUP 101 - GROUP size 0x200
*
* @param  pCtiFrameWorkCB
*
* @return None
*/
void _OVERLAY_ ctiTestFuncOverlay101Vect(void)
{
   u32_t* pVar = &g_uiVar;
   u32_t uiLoopCount = D_CTI_NUM_OF_LOOPS;

   *pVar = 0;
   while( uiLoopCount--)
   {
      *pVar +=1;
   }
}

/**
* @brief run at GROUP 102 and call function at GROUP 103 - GROUP size 0x200
*
* @param  pCtiFrameWorkCB
*
* @return None
*/
void _OVERLAY_ ctiTestFuncOverlay102Vect(void)
{
   u32_t* pVar = &g_uiVar;
   u32_t uiLoopCount = 100;

   *pVar = 0;
   while( uiLoopCount--)
   {
      *pVar +=1;
   }

   /* call FUNCTION in GROUP 103 - GROUP size is 0x200 */
   ctiTestFuncOverlay103Vect();
}

/**
* @brief run at GROUP 103 - GROUP size 0x200
*
* @param  pCtiFrameWorkCB
*
* @return None
*/
void _OVERLAY_ ctiTestFuncOverlay103Vect(void)
{
   u32_t* pVar = &g_uiVar;
   u32_t uiLoopCount = 50;

   *pVar = 0;
   while( uiLoopCount--)
   {
      *pVar +=1;
   }
}

/**
* @brief run at GROUP 102 and call function at GROUP 102 - GROUP size 0x200
*
* @param  pCtiFrameWorkCB
*
* @return None
*/
void _OVERLAY_ ctiTestFuncOverlay104Vect(void)
{
   u32_t* pVar = &g_uiVar;
   u32_t uiLoopCount = 10;

   *pVar = 0;
   while( uiLoopCount--)
   {
      *pVar +=1;
   }

   /* call FUNCTION in GROUP 102 - GROUP size is 0x200 */
   ctiTestFuncOverlay107Vect();
}

/**
* @brief run at GROUP 100 and at GROUP 101 && 102 in case of multigroup
* GROUP size 0x200
*
* @param  pCtiFrameWorkCB
*
* @return None
*/
void _OVERLAY_ ctiTestFuncOverlay105Vect(void)
{
   u32_t* pVar = &g_uiVar;
   u32_t uiLoopCount = 25;

   *pVar = 0;

   while( uiLoopCount--)
   {
      *pVar +=1;
   }

   asm volatile ("auipc %0, 0" : "=r" (G_pProgramCounterAddress)  : );
}

/**
* @brief run at GROUP 103 - GROUP size 0xA00
*
* @param  pCtiFrameWorkCB
*
* @return None
*/
void _OVERLAY_ ctiTestFuncOverlay106Vect(void)
{
   u32_t* pVar = &g_uiVar;
   u32_t uiLoopCount = 256;
   u32_t* pArr = g_uiTempArr;
   *pVar = 0;

   M_CTI_512B_NOPS;

   while( uiLoopCount--)
   {
      *pVar +=1;
      pArr[uiLoopCount] = (*pVar)++ ;

      if (pArr[uiLoopCount] % 7 == 0)
      {
         pArr[uiLoopCount]++;
      }
   }

   M_CTI_512B_NOPS;
   M_CTI_512B_NOPS;
   M_CTI_512B_NOPS;
   M_CTI_100_NOPS;
}

/**
* @brief run at GROUP 102 - GROUP at size 0x200
*
* @param  None
*
* @return None
*/
void _OVERLAY_ ctiTestFuncOverlay107Vect(void)
{
   u32_t* pVar = &g_uiVar;
   u32_t uiLoopCount = 75;

   *pVar = 0;

   while( uiLoopCount--)
   {
      *pVar +=1;
   }
}

#ifdef D_COMRV_RTOS_SUPPORT
/**
* @brief run at GROUP 105 - GROUP at size 0x400
*
* @param  pCtiFrameWorkCB
*
* @return None
*/
void  _OVERLAY_  ctiTestFuncOverlay108Vect(void)
{
   u32_t* pVar = &g_uiVar;
   u32_t uiLoopCount = D_CTI_NUM_OF_LOOPS;

   *pVar = 0;

   M_CTI_512B_NOPS;
   M_CTI_100_NOPS;

   while( uiLoopCount--)
   {
      *pVar +=1;
   }
}
#endif /* D_COMRV_RTOS_SUPPORT */

/**
* @brief run at GROUP 106 - GROUP at size 0x400
*
* @param  pCtiFrameWorkCB
*
* @return None
*/
void _OVERLAY_ ctiTestFuncOverlay109Vect(void)
{
   u32_t* pVar = &g_uiVar;
   u32_t uiLoopCount = D_CTI_NUM_OF_LOOPS;

   *pVar = 0;
   M_CTI_512B_NOPS;
   M_CTI_100_NOPS;

   while( uiLoopCount--)
   {
      *pVar +=1;
   }
}

/**
* @brief run at GROUP 107 - GROUP at size 0x600
*
* @param  pCtiFrameWorkCB
*
* @return None
*/
void _OVERLAY_ ctiTestFuncOverlay110Vect(void)
{
   u32_t* pVar = &g_uiVar;
   u32_t uiLoopCount = D_CTI_NUM_OF_LOOPS;

   *pVar = 0;

   while( uiLoopCount--)
   {
      *pVar +=1;
   }

   M_CTI_512B_NOPS;
   M_CTI_512B_NOPS;
   M_CTI_100_NOPS;
}

/**
* @brief run at GROUP 107 - GROUP at size 0xc00
*
* @param  pCtiFrameWorkCB
*
* @return None
*/
void _OVERLAY_ ctiTestFuncOverlay111Vect(void)
{
   u32_t* pVar = &g_uiVar;
   u32_t uiLoopCount = D_CTI_NUM_OF_LOOPS;

  *pVar = 0;

   while( uiLoopCount--)
   {
      *pVar +=1;
   }

   M_CTI_512B_NOPS;
   M_CTI_512B_NOPS;
   M_CTI_512B_NOPS;
   M_CTI_512B_NOPS;
   M_CTI_512B_NOPS;
   M_CTI_100_NOPS;
}

/**
* @brief run at GROUP 109 - GROUP at size 0x800
*
* @param  pCtiFrameWorkCB
*
* @return None
*/
void _OVERLAY_ ctiTestFuncOverlay112Vect(void)
{
   u32_t* pVar = &g_uiVar;
   u32_t uiLoopCount = D_CTI_NUM_OF_LOOPS;

  *pVar = 0;

   while( uiLoopCount--)
   {
      *pVar +=1;
   }

   M_CTI_512B_NOPS;
   M_CTI_512B_NOPS;
   M_CTI_512B_NOPS;
   M_CTI_100_NOPS;
}

#ifdef D_COMRV_RTOS_SUPPORT
/**
* @brief run at GROUP 110 - GROUP at size 0x800
*
* @param  uiVar1, uiVar2, uiVar3 - integers
*
* @return None
*/
void _OVERLAY_ ctiTestFuncOverlay113Vect(u32_t uiVar1, u32_t uiVar2, u32_t uiVar3)
{
   /* verify we got all correct args */
   if (uiVar1 + uiVar2 + uiVar3 != D_CTI_SUM_OF_ARGS)
   {
      M_PSP_EBREAK();
   }
   M_CTI_100_NOPS;
}
#endif /* D_COMRV_RTOS_SUPPORT */
