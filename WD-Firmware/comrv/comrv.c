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
* @file   comrv.c
* @author Ronen Haen
* @date   21.06.2019
* @brief  The file implements the COM-RV engine
* 
*/

/**
* include files
*/
#include "comrv.h"
#include "comrv_api.h"

/**
* definitions
*/
#define D_COMRV_END_OF_STACK                          0xDEAD
#define D_COMRV_MRU_ITEM                              0xFF
#define D_COMRV_LRU_ITEM                              0xFF
#define D_COMRV_MAX_GROUP_NUM                         0xFFFF
#define D_COMRV_DWORD_IN_BITS                         32
#define D_COMRV_ENTRY_LOCKED                          1
#define D_COMRV_CANDIDATE_LIST_SIZE                   (1+(D_COMRV_OVL_GROUP_SIZE_MAX/D_COMRV_OVL_GROUP_SIZE_MIN))
#define D_COMRV_ENTRY_TOKEN_INIT_VALUE                0x0001FFFE
#define D_COMRV_ENTRY_PROPERTIES_INIT_VALUE           0x04
#define D_COMRV_ENTRY_PROPERTIES_RESET_MASK           0xC3
#define D_COMRV_OFFSET_SCALE_VALUE                    4
#define D_COMRV_INVOKE_CALLEE_BIT_0                   1
#define D_COMRV_RET_CALLER_BIT_0                      0
#define D_COMRV_NUM_BITS_DWORD                        32
#if (D_COMRV_NUM_OF_CACHE_ENTRIES % D_COMRV_NUM_BITS_DWORD)
  #define D_COMRV_EVICT_CANDIDATE_MAP_SIZE            ((D_COMRV_NUM_OF_CACHE_ENTRIES/sizeof(u32_t)) + 1)
#else
  #define D_COMRV_EVICT_CANDIDATE_MAP_SIZE            (D_COMRV_NUM_OF_CACHE_ENTRIES/sizeof(u32_t))
#endif
#define D_COMRV_PROPERTIES_SIZE_FLD_SHIFT_AMNT        2
#define D_COMRV_GRP_SIZE_IN_BYTES_SHIFT_AMNT          9
#define D_COMRV_GROUP_NOT_FOUND                       0xFFFF
#define D_COMRV_LAST_MULTI_GROUP_ENTRY                0
#define D_COMRV_TABLES_CACHE_ENTRY_INDEX              (D_COMRV_NUM_OF_CACHE_ENTRIES-1)
#define D_COMRV_LAST_CACHE_ENTRY_INDEX                (D_COMRV_TABLES_CACHE_ENTRY_INDEX)
#define D_COMRV_TABLES_OFFSET                         0
/**
* macros
*/
/* read token register (t5) */
#define M_COMRV_READ_TOKEN_REG(x)        asm volatile ("mv %0, t5" : "=r" (x)  : );
/* write stack pool register (t4) */
#define M_COMRV_WRITE_POOL_REG(x)        asm volatile ("mv t4, %0" : : "r" (x) );
/* read stack pool register (t4) */
#define M_COMRV_READ_POOL_REG(x)         asm volatile ("mv %0, t4" : "=r" (x)  : );
/* read comrv stack register (t3) */
#define M_COMRV_READ_STACK_REG(x)        asm volatile ("mv %0, t3" : "=r" (x)  : );
/* write comrv stack register (t3) */
#define M_COMRV_WRITE_STACK_REG(x)       asm volatile ("mv t3, %0" : : "r" (x) );
/* set comrv entry engine address */
#define M_COMRV_SET_ENTRY_ADDR(address)  asm volatile ("la t6, "#address : : : );
/* set the comrv stack pool and comrv stack registers */
#if __riscv_xlen == 64
 #define M_COMRV_SET_STACK_ADDR(address) asm volatile ("la t3, "#address : : : ); \
                                         asm volatile ("ld t3, 0x0(t3)"  : : : );
#elif __riscv_xlen == 32
 #define M_COMRV_SET_STACK_ADDR(address) asm volatile ("la t3, "#address : : : ); \
                                         asm volatile ("lw t3, 0x0(t3)"  : : : );
#endif
/* overlay group size in D_COMRV_OVL_GROUP_SIZE_MIN granularity */
#define M_COMRV_GET_OVL_GROUP_SIZE(unToken)          (pOverlayOffsetTable[unToken.stFields.uiOverlayGroupID+1] - \
                                                      pOverlayOffsetTable[unToken.stFields.uiOverlayGroupID])
/* overlay group size in bytes */
#define M_COMRV_GET_OVL_GROUP_SIZE_IN_BYTES(unToken) (M_COMRV_GET_OVL_GROUP_SIZE(unToken) << 9)
/* token offset in bytes */
#define M_COMRV_GET_TOKEN_OFFSET_IN_BYTES(unToken)   ((unToken.stFields.uiOffset) * D_COMRV_OFFSET_SCALE_VALUE)
/* overlay group offset in bytes */
#define M_COMRV_GET_GROUP_OFFSET_IN_BYTES(unToken)   ((pOverlayOffsetTable[unToken.stFields.uiOverlayGroupID]) << 9)
/* convert a given entry size in to an entry properties value */
#define M_COMRV_CONVERT_TO_ENTRY_SIZE_FROM_VAL(val)  (D_COMRV_PROPERTIES_SIZE_FLD_SHIFT_AMNT << (val))
/* */
#define M_COMRV_GROUP_SIZE_TO_BYTES(groupSize)       ((groupSize) << D_COMRV_GRP_SIZE_IN_BYTES_SHIFT_AMNT)
/* macro for verifying overlay group CRC */
#ifdef D_COMRV_CRC
#define M_COMRV_VERIFY_CRC(pAddressToCalc, usMemSizeInBytes, uiExpectedResult)   \
      if (_BUILTIN_EXPECT(comrvCrcCalcHook(pAddressToCalc, usMemSizeInBytes, uiExpectedResult),0))  \
      {                                                                          \
         stErrArgs.uiErrorNum = D_COMRV_CRC_CHECK_ERR;                           \
         stErrArgs.uiToken    = unToken.uiValue;                                 \
         comrvErrorHook(&stErrArgs);                                             \
      }
#else
#define M_COMRV_VERIFY_CRC(pAddressToCalc, usMemSizeInBytes, uiExpectedResult)
#endif /* D_COMRV_CRC */

#ifdef D_COMRV_DEBUG
#define M_COMRV_ASSERT(conditionMet) if (conditionMet) \
                                     { while(1);}
#else
#define M_COMRV_ASSERT(conditionMet)
#endif /* D_COMRV_DEBUG */

/* calculate the cache address for a given cache entry */
#define M_COMRV_CALC_CACHE_ADDR_IN_BYTES_FROM_ENTRY(ucEntryIndex) ((u08_t*)pComrvCacheBaseAddress + (ucEntryIndex<<9))

#define M_COMRV_CACHE_SIZE_IN_BYTES()   ((u08_t*)&__OVERLAY_CACHE_END__ - (u08_t*)&__OVERLAY_CACHE_START__)
/* this macro is only for code readability (the symbol '__OVERLAY_CACHE_START__'
   is defined in the linker script and defines the start address of comrv cache) */
#define pComrvCacheBaseAddress          (&__OVERLAY_CACHE_START__)
/* address of offset table (last comrv cache entry) */
#define pOverlayOffsetTable             ((u16_t*)((u08_t*)&__OVERLAY_CACHE_END__ - D_COMRV_OVL_GROUP_SIZE_MIN))
/* address of multi group table */
#define pOverlayMultiGroupTokensTable   ((comrvOverlayToken_t*)(pOverlayOffsetTable + g_stComrvCB.ucMultiGroupOffset)) //((comrvOverlayToken_t*)&overlayMultiGroupTokensTable)

/**
* types
*/

/**
* local prototypes
*/
void*        comrvMemset                     (void* pMemory, s32_t siVal, u32_t uiSizeInDwords);
static u08_t comrvGetEvictionCandidates      (u08_t ucRequestedEvictionSize, u08_t* pEvictCandidatesList);
static void  comrvUpdateCacheEvectionParams  (u08_t ucEntryIndex);
static u16_t comrvSearchForLoadedOverlayGroup(comrvOverlayToken_t unToken);

/**
* external prototypes
*/
/* main comrv entry function - all overlay functions are invoked
   through this function (the address of comrvEntry() is set in reg t6) */
extern void  comrvEntry               (void);

/* user hook functions - user application must implement the following functions */
extern void  comrvErrorHook           (const comrvErrorArgs_t* pErrorArgs);
extern void  comrvMemcpyHook          (void* pDest, void* pSrc, u32_t uiSizeInBytes);
extern void* comrvLoadOvlayGroupHook  (comrvLoadArgs_t* pLoadArgs);
#ifdef D_COMRV_CRC
extern u32_t comrvCrcCalcHook         (const void* pAddress, u16_t usMemSizeInBytes, u32_t uiExpectedResult);
#endif /* D_COMRV_CRC */
#ifdef D_COMRV_FW_INSTRUMENTATION
extern void  comrvInstrumentationHook (const comrvInstrumentationArgs_t* pInstArgs);
#endif /* D_COMRV_FW_INSTRUMENTATION */

/**
* global variables
*/
/* global comrv control block */
static comrvCB_t         g_stComrvCB;
/* global comrv stack pool */
static comrvStackFrame_t g_stComrvStackPool[D_COMRV_CALL_STACK_DEPTH];

/* linker symbols defining the start and end of the overlay cache */
extern void *__OVERLAY_CACHE_START__, *__OVERLAY_CACHE_END__;

/**
* COM-RV initialization function
*
* @param  pInitParams - initialization parameters (currently no args)
*
* @return none
*/
void comrvInit(comrvInitArgs_t* pInitArgs)
{
   comrvCacheEntry_t *pCacheEntry;
   u08_t              ucIndex;
   comrvStackFrame_t* pStackPool   = g_stComrvStackPool;
#ifdef D_COMRV_VERIFY_INIT_ARGS
   comrvErrorArgs_t   stErrArgs;
   u32_t              uiCacheSizeInBytes;
#endif /* D_COMRV_VERIFY_INIT_ARGS */

#ifdef D_COMRV_VERIFY_INIT_ARGS
   uiCacheSizeInBytes = M_COMRV_CACHE_SIZE_IN_BYTES();
   /* verify cache configuration - size and alignment to D_COMRV_OVL_GROUP_SIZE_MIN */
   if (_BUILTIN_EXPECT(uiCacheSizeInBytes != D_COMRV_OVL_CACHE_SIZE_IN_BYTES ||
       uiCacheSizeInBytes % D_COMRV_OVL_GROUP_SIZE_MIN, 0))
   {
      stErrArgs.uiErrorNum = D_COMRV_INVALID_INIT_PARAMS_ERR;
      stErrArgs.uiToken    = D_COMRV_INVALID_TOKEN;
      comrvErrorHook(&stErrArgs);
   }
#endif /* D_COMRV_VERIFY_INIT_ARGS */

#ifdef D_COMRV_EVICTION_LRU
   /* initialize all cache entries (exclude last cache entry which is
      reserved for comrv tables) */
   for (ucIndex = 0 ; ucIndex < D_COMRV_LAST_CACHE_ENTRY_INDEX ; ucIndex++)
   {
      pCacheEntry = &g_stComrvCB.stOverlayCache[ucIndex];
      /* initially each entry points to the previous and next neighbor cells */
      pCacheEntry->unLru.stFields.typPrevLruIndex = ucIndex-1;
      pCacheEntry->unLru.stFields.typNextLruIndex = ucIndex+1;
      pCacheEntry->unToken.uiValue                = D_COMRV_ENTRY_TOKEN_INIT_VALUE;
      pCacheEntry->unProperties.ucValue           = D_COMRV_ENTRY_PROPERTIES_INIT_VALUE;
   }
   /* mark the last entry in the LRU list */
   g_stComrvCB.stOverlayCache[ucIndex-1].unLru.stFields.typNextLruIndex = D_COMRV_MRU_ITEM;
   /* set the index of the list LRU and MRU */
   g_stComrvCB.ucLruIndex = 0;
   g_stComrvCB.ucMruIndex = ucIndex-1;

#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */

   /* initialize all stack entries */
   for (ucIndex = 0 ; ucIndex < D_COMRV_CALL_STACK_DEPTH ; ucIndex++, pStackPool++)
   {
      pStackPool->ssOffsetPrevFrame = (s16_t)-sizeof(comrvStackFrame_t);
   }

   /* point to the last frame */
   pStackPool--;

   /* set the address of COMRV stack pool register t4 */
   M_COMRV_WRITE_POOL_REG(pStackPool);

   /* set the address of COMRV entry point in register t6 */
   M_COMRV_SET_ENTRY_ADDR(comrvEntry);

#ifndef D_COMRV_USE_OS
   /* in baremetal applications, stack register is initialized here;
      this must be done after the stack pool register was
      initialized (M_COMRV_WRITE_POOL_REG) */
   comrvInitApplicationStack();
#endif /* D_COMRV_USE_OS */
   /* check if end user enables loading offset and multi group tables */
   if (pInitArgs->ucCanLoadComrvTables != 0)
   {
      comrvLoadTables();
   }
}

/**
* This function is performing the following:
* 1.  load requested token (from t5 register)
* 2.  search for the requested token in the loaded cache
* 2.a for multi group search for each multi group token
* 3.  if group not loaded, load it
* 4.  return the address of the function (call or return address) or dada
*
* @param none
*
* @return void* - address of the overlay function/data
*/
void* comrvGetAddressFromToken(void)
{
   comrvCacheEntry_t   *pEntry;
   comrvOverlayToken_t  unToken;
   comrvErrorArgs_t     stErrArgs;
   comrvLoadArgs_t      stLoadArgs;
   u08_t                ucIsInvoke;
   comrvStackFrame_t   *pComrvStackFrame;
   void                *pAddress, *pNextEvictCandidateCacheAddress, *pDestinationAddress;
   u16_t                usOverlayGroupSize, usOffset, usSearchResultIndex;
   u08_t                ucNumOfEvictionCandidates, ucIndex, ucSizeOfEvictionCandidates;
   u08_t                ucEntryIndex, ucEvictCandidateList[D_COMRV_CANDIDATE_LIST_SIZE];
#ifdef D_COMRV_FW_INSTRUMENTATION
   comrvInstrumentationArgs_t stInstArgs;
   u32_t                uiProfilingIndication;
#endif /* D_COMRV_FW_INSTRUMENTATION */
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
   u32_t                uiTemp;
   u16_t                usSelectedMultiGroupEntry;
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */

   /* read the requested token value (t5) */
   M_COMRV_READ_TOKEN_REG(unToken.uiValue);

   /* read comrv stack register (t3) */
   M_COMRV_READ_STACK_REG(pComrvStackFrame)

   /* get the invoke callee indication */
   ucIsInvoke = (u32_t)pComrvStackFrame & D_COMRV_INVOKE_CALLEE_BIT_0;
   /* are we calling a new overlay function */
   if (ucIsInvoke == D_COMRV_INVOKE_CALLEE_BIT_0)
   {
      /* clear the invoke indication from pComrvStackFrame address */
      pComrvStackFrame = (comrvStackFrame_t*)((u32_t)pComrvStackFrame & (~D_COMRV_INVOKE_CALLEE_BIT_0));
      /* write back the stack register after bit 0 was cleared*/
      M_COMRV_WRITE_STACK_REG(pComrvStackFrame);

#ifdef D_COMRV_FW_INSTRUMENTATION
      uiProfilingIndication = D_COMRV_NO_LOAD_AND_INVOKE_IND;
#endif /* D_COMRV_FW_INSTRUMENTATION */
   }
   /* we are returning to an overlay function or accessing overlay data */
   else
   {
#ifdef D_COMRV_FW_INSTRUMENTATION
      uiProfilingIndication = D_COMRV_NO_LOAD_AND_RETURN_IND;
#endif /* D_COMRV_FW_INSTRUMENTATION */
   }
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
   /* if the requested token isn't a multi-group token */
   if (unToken.stFields.uiMultiGroup == 0)
   {
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */
      /* search for token */
      usSearchResultIndex = comrvSearchForLoadedOverlayGroup(unToken);
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
   }
   /* search for a multi-group overlay token */
   else
   {
      /* first ucEntryIndex to search from in the multi group table is determined by the overlayGroupID
         field of the requested token */
      ucEntryIndex = unToken.stFields.uiOverlayGroupID;
      do
      {
         /* search for the token */
         usSearchResultIndex = comrvSearchForLoadedOverlayGroup(pOverlayMultiGroupTokensTable[ucEntryIndex++]);
      /* continue the search as long as the group wasn't found and we have additional tokens */
      } while ((usSearchResultIndex == D_COMRV_GROUP_NOT_FOUND) && (pOverlayMultiGroupTokensTable[ucEntryIndex].uiValue != D_COMRV_LAST_MULTI_GROUP_ENTRY));

      /* save the selected multi group entry */
      usSelectedMultiGroupEntry = ucEntryIndex-1;
   }
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */

#ifndef D_COMRV_MULTI_GROUP_SUPPORT
   /* get the group size */
   usOverlayGroupSize = M_COMRV_GET_OVL_GROUP_SIZE(unToken);
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */

   /* if the data/function is not loaded we need to evict and load it */
   if (usSearchResultIndex == D_COMRV_GROUP_NOT_FOUND)
   {
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
      /* if the requested token is a multi-group token */
      if (unToken.stFields.uiMultiGroup)
      {
         /* for now we take the first token in the list of tokens */
         // TODO: need to have a more sophisticated way to select the multi-group */
         unToken = pOverlayMultiGroupTokensTable[unToken.stFields.uiOverlayGroupID];
         /* save the selected multi group entry; usSelectedMultiGroupEntry is used to
            update comrv stack frame with the loaded multi group table entry.
            It is used to calculate the actual return offset in case we
            are returning to a multi group token */
         usSelectedMultiGroupEntry = unToken.stFields.uiOverlayGroupID;
      }
      /* get the group size */
      usOverlayGroupSize = M_COMRV_GET_OVL_GROUP_SIZE(unToken);
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */

      /* we need to make sure that from this point
         we won't have new overlay requests */
      M_COMRV_ENTER_CRITICAL_SECTION();

      /* get eviction candidates according to the requested pOverlayGroupSize */
      ucNumOfEvictionCandidates = comrvGetEvictionCandidates(usOverlayGroupSize, ucEvictCandidateList);

      ucEntryIndex = 0;

      /* if you have no/some candidates */
      if (_BUILTIN_EXPECT(ucNumOfEvictionCandidates == 0, 0))
      {
         M_COMRV_EXIT_CRITICAL_SECTION();
         stErrArgs.uiErrorNum = D_COMRV_NOT_ENOUGH_ENTRIES;
         stErrArgs.uiToken    = unToken.uiValue;
         comrvErrorHook(&stErrArgs);
      }
      /* we need to handle cache fragmentation since we got more than 1 eviction candidate */
      else
      {
         /* no handling to the last eviction candidate */
         ucNumOfEvictionCandidates--;
         /* loop all eviction candidates entries */
         while (ucEntryIndex < ucNumOfEvictionCandidates)
         {
            /* get the candidate entry index */
            ucIndex = ucEvictCandidateList[ucEntryIndex];
            /* get the candidate cache entry address */
            pDestinationAddress = M_COMRV_CALC_CACHE_ADDR_IN_BYTES_FROM_ENTRY(ucIndex);
            /* calc the source address - we point here to the cache area
               from which we want to copy the overlay group:
               cache-entry-address + current-entry-group-size = address of the neighbour cache entry  */
            pAddress = pDestinationAddress + M_COMRV_GET_OVL_GROUP_SIZE_IN_BYTES(g_stComrvCB.stOverlayCache[ucIndex].unToken);
            /* get the cache address of the next evict candidate - it is used to calculate
               the amount of memory to copy */
            pNextEvictCandidateCacheAddress = M_COMRV_CALC_CACHE_ADDR_IN_BYTES_FROM_ENTRY(ucEvictCandidateList[ucEntryIndex+1]);
            /* perform code copy - from neighbour cache entry (pAddress) to current evict cache entry */
            comrvMemcpyHook(pDestinationAddress, pAddress, pNextEvictCandidateCacheAddress - pAddress);

            /* after code copy we need to align the entries structures */
            for ( ; ucIndex < ucEvictCandidateList[ucEntryIndex+1] ; ucIndex++)
            {
               /* pEntry will point to the CB entry we want to copy from */
               pEntry = &g_stComrvCB.stOverlayCache[ucIndex + g_stComrvCB.stOverlayCache[ucIndex].unProperties.stFields.ucSizeInMinGroupSizeUnits];
#ifdef D_COMRV_OVL_DATA_SUPPORT
               /* an overlay data is present when handling de-fragmentation */
               if (_BUILTIN_EXPECT(pEntry->unProperties.stFields.ucData, 0))
               {
                  M_COMRV_EXIT_CRITICAL_SECTION();
                  stErrArgs.uiErrorNum = D_COMRV_OVL_DATA_DEFRAG_ERR;
                  stErrArgs.uiToken    = unToken.uiValue;
                  comrvErrorHook(&stErrArgs);
               }
#endif /* D_COMRV_OVL_DATA_SUPPORT */
               /* now we copy the cache entry properties and token */
               g_stComrvCB.stOverlayCache[ucIndex].unProperties = pEntry->unProperties;
               g_stComrvCB.stOverlayCache[ucIndex].unToken.uiValue = pEntry->unToken.uiValue;
#ifdef D_COMRV_EVICTION_LRU
               /* we also need to align the LRU list */
               g_stComrvCB.stOverlayCache[pEntry->unLru.stFields.typPrevLruIndex].unLru.typValue = pEntry->unLru.typValue;
#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */
            } /* end of loop -> for (ucIndex = ... */
            /* now we move to handle the next eviction candidate */
            ucEntryIndex++;
         } /* end of loop -> while (ucEntryIndex < ... */
         /* increment the number of candidates so ucNumOfEvictionCandidates
            points now to the eviction entry that contains the total eviction size */
         ucNumOfEvictionCandidates++;
      } /* if (ucNumOfEvictionCandidates == 0) */
      /* at this point we will have only one entry left (w/ or w/o fragmentation) */
      ucIndex = ucEvictCandidateList[ucEntryIndex];
      /* update the entry access */
      comrvUpdateCacheEvectionParams(ucIndex);
      /* update the cache entry with the new token */
      g_stComrvCB.stOverlayCache[ucIndex].unToken.uiValue = unToken.uiValue;
      /* update the cache entry properties with the group size */
      g_stComrvCB.stOverlayCache[ucIndex].unProperties.stFields.ucSizeInMinGroupSizeUnits = usOverlayGroupSize;
      /* if evicted size is larger than requested size we need to update the CB remaining space */
      ucSizeOfEvictionCandidates = ucEvictCandidateList[ucNumOfEvictionCandidates] - usOverlayGroupSize;
      /* check if the evicted size was bigger than the requested size */
      if (ucSizeOfEvictionCandidates != 0)
      {
         /* point to the CB cache entry to be updated */
         pEntry = &g_stComrvCB.stOverlayCache[ucEntryIndex + usOverlayGroupSize];
         /* mark the group ID so that it won't pop in the next search */
         pEntry->unToken.uiValue      = D_COMRV_ENTRY_TOKEN_INIT_VALUE;
         /* update the cache entry new size - this will also clear remaining properties */
         pEntry->unProperties.ucValue = M_COMRV_CONVERT_TO_ENTRY_SIZE_FROM_VAL(ucSizeOfEvictionCandidates);
#ifdef D_COMRV_EVICTION_LRU
         /* update the cache entry 'next lru' field of the previous lru */
         g_stComrvCB.stOverlayCache[pEntry->unLru.stFields.typPrevLruIndex].unLru.stFields.typNextLruIndex =
               pEntry->unLru.stFields.typNextLruIndex;
         /* update the cache entry 'next lru' field */
         pEntry->unLru.stFields.typNextLruIndex = ucIndex;
         /* update the cache entry 'previous lru' field - now it is the first lru as
            it is now considered 'evicted/empty' */
         pEntry->unLru.stFields.typPrevLruIndex = D_COMRV_LRU_ITEM;
         /* update the global lru index */
         g_stComrvCB.ucLruIndex = ucIndex;
#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */
      } /* if (ucSizeOfEvictionCandidates != 0) */
      // TODO: protect the soon loaded ram
      /* it is safe now to get new requests */
      M_COMRV_EXIT_CRITICAL_SECTION();
      /* the group size in bytes */
      usOverlayGroupSize = M_COMRV_GROUP_SIZE_TO_BYTES(usOverlayGroupSize);
      /* now we can load the overlay group */
      stLoadArgs.uiSizeInBytes = usOverlayGroupSize;
      stLoadArgs.pDest         = M_COMRV_CALC_CACHE_ADDR_IN_BYTES_FROM_ENTRY(ucIndex);
      stLoadArgs.uiGroupOffset = M_COMRV_GET_GROUP_OFFSET_IN_BYTES(g_stComrvCB.stOverlayCache[ucIndex].unToken);
      pAddress = comrvLoadOvlayGroupHook(&stLoadArgs);
      /* if group wasn't loaded */
      if (_BUILTIN_EXPECT(pAddress == 0,0))
      {
         stErrArgs.uiErrorNum = D_COMRV_LOAD_ERR;
         stErrArgs.uiToken    = unToken.uiValue;
         comrvErrorHook(&stErrArgs);
      }

      M_COMRV_VERIFY_CRC(pAddress, usOverlayGroupSize-sizeof(u32_t),
                        *((u08_t*)pAddress + (usOverlayGroupSize-sizeof(u32_t))));

#ifdef D_COMRV_FW_INSTRUMENTATION
      /* update for FW profiling loaded the function */
      uiProfilingIndication |= D_COMRV_PROFILING_LOAD_BIT;
#endif /* D_COMRV_FW_INSTRUMENTATION */
   } /* overlay group is already loaded */
   else
   {
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
      /* get the already loaded overlay token */
      if (unToken.stFields.uiMultiGroup)
      {
         unToken = pOverlayMultiGroupTokensTable[usSelectedMultiGroupEntry];
      }
      /* get the group size */
     usOverlayGroupSize = M_COMRV_GET_OVL_GROUP_SIZE(unToken);
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */
     /* get the loaded address */
     pAddress           = M_COMRV_CALC_CACHE_ADDR_IN_BYTES_FROM_ENTRY(usSearchResultIndex);
     /* the group size in bytes */
	  usOverlayGroupSize = M_COMRV_GROUP_SIZE_TO_BYTES(usOverlayGroupSize);
   } /* if (usSearchResultIndex == D_COMRV_GROUP_NOT_FOUND) */

   /* get actual function/data offset */
   usOffset = M_COMRV_GET_TOKEN_OFFSET_IN_BYTES(unToken);

   /* are we returning to an overlay function */
   if (ucIsInvoke == D_COMRV_RET_CALLER_BIT_0)
   {
      /* calculate the function return offset; at this point
         pComrvStackFrame->uiCalleeToken holds the return address */
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
      /* In multi group token we need to get the offset from previously loaded token */
      if (pComrvStackFrame->usCalleeMultiGroupTableEntry)
      {
         /* we now are at the point of loading a multi-group token so we need to take the
            previous token for which the return address refers to */
         unToken = pOverlayMultiGroupTokensTable[pComrvStackFrame->usCalleeMultiGroupTableEntry];
         /* get the offset */
         uiTemp = M_COMRV_GET_TOKEN_OFFSET_IN_BYTES(unToken);
         /* get the token group size */
         usOverlayGroupSize = M_COMRV_GET_OVL_GROUP_SIZE_IN_BYTES(unToken);
         /* calculate the actual return offset */
         usOffset += ((u32_t)(pComrvStackFrame->uiCalleeToken) - uiTemp) & (--usOverlayGroupSize);
         /* clear the save multi token so it won't be reused */
         pComrvStackFrame->usCalleeMultiGroupTableEntry = 0;
      }
      else
      {
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */
         /* calculate the actual return offset */
         usOffset += ((u32_t)(pComrvStackFrame->uiCalleeToken) - usOffset) & (--usOverlayGroupSize);
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
      }
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */
   }
   /* we are calling a new overlay function or accessing overlay data */
   else
   {
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
     /* Update comrv stack frame with the loaded multi group table entry.
        It is used to calculate the actual return offset in case we
        are returning to a multi group token */
      pComrvStackFrame->usCalleeMultiGroupTableEntry = usSelectedMultiGroupEntry;
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */
   }

#ifdef D_COMRV_FW_INSTRUMENTATION
   stInstArgs.uiInstNum  = uiProfilingIndication;
   stInstArgs.uiToken    = unToken.uiValue;
   comrvInstrumentationHook(&stInstArgs);
#endif /* D_COMRV_FW_INSTRUMENTATION */

   /* group is now loaded to memory so we can return the address of the data/function */
   return ((u08_t*)pAddress + usOffset);
}

/**
* Get comrv cache eviction candidates according to a given size
*
* @param requestedEvictionSize - size requested for eviction; expressed in
*                                units of D_COMRV_OVL_GROUP_SIZE_MIN
*        pEvictCandidatesList  - output eviction candidate list of comrv cache indexes
* @return number of eviction candidates in the output list
*/
u08_t comrvGetEvictionCandidates(u08_t ucRequestedEvictionSize, u08_t* pEvictCandidatesList)
{
   comrvCacheEntry_t  *pCacheEntry;
   u32_t               uiCandidates;
   u08_t               ucAccumulatedSize = 0, ucIndex = 0;
   u08_t               ucEntryIndex, ucNumberOfCandidates = 0;
   u32_t               uiEvictCandidateMap[D_COMRV_EVICT_CANDIDATE_MAP_SIZE];

   /* first lets clear the uiCandidates list */
   comrvMemset(uiEvictCandidateMap, 0, sizeof(u32_t)*D_COMRV_EVICT_CANDIDATE_MAP_SIZE);

#ifdef D_COMRV_EVICTION_LRU
   /* get the first lru entry */
   ucEntryIndex = g_stComrvCB.ucLruIndex;
   /* loop LRU list until the eviction uiCandidates accumulated size
      reaches the requested eviction size */
   do
   {
      /* point to the cache entry CB */
      pCacheEntry = &g_stComrvCB.stOverlayCache[ucEntryIndex];
      /* verify the entry isn't locked */
      if (pCacheEntry->unProperties.stFields.ucLocked != D_COMRV_ENTRY_LOCKED)
      {
         /* count the number of uiCandidates */
         ucNumberOfCandidates++;
         /* accumulate size */
         ucAccumulatedSize += pCacheEntry->unProperties.stFields.ucSizeInMinGroupSizeUnits;
         /* set the eviction candidate in the eviction map */
         uiEvictCandidateMap[ucEntryIndex/D_COMRV_DWORD_IN_BITS] |= (1 << ucEntryIndex);
      }
      /* move to the next LRU candidate */
      ucEntryIndex = pCacheEntry->unLru.stFields.typNextLruIndex;
   /* loop as long as we didn't get to the requested eviction size or we reached end of the list
      (means that all entries are locked) */
   } while (ucAccumulatedSize < ucRequestedEvictionSize && ucEntryIndex != D_COMRV_MRU_ITEM);

#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */

   /* make sure we don't have more than the maximum available entries
      to accommodate one group */
   M_COMRV_ASSERT(ucNumberOfCandidates > (D_COMRV_OVL_GROUP_SIZE_MAX/D_COMRV_OVL_GROUP_SIZE_MIN));

   /* now we have eviction uiCandidates bitmap of cache entries - lets create
      an output sorted list of these entries */
   for (ucEntryIndex = 0 ; ucIndex != ucNumberOfCandidates && ucEntryIndex < D_COMRV_EVICT_CANDIDATE_MAP_SIZE ; ucEntryIndex++)
   {
      /* get the candidates */
      uiCandidates = uiEvictCandidateMap[ucEntryIndex];
      /* convert each candidate to an actual value in pEvictCandidatesList */
      while (uiCandidates)
      {
         /* get the lsb that is set */
         pEvictCandidatesList[ucIndex] = uiCandidates & (-uiCandidates);
         /* subtract the lsb that is set */
         uiCandidates -= pEvictCandidatesList[ucIndex];
         /* decrement by 1 to get the actual zero based value */
         pEvictCandidatesList[ucIndex]--;
         /* add the location of the bit - pEvictCandidatesList[ucIndex] will hold the group number */
         pEvictCandidatesList[ucIndex] += ucEntryIndex*D_COMRV_DWORD_IN_BITS;
         /* move to the next entry in pEvictCandidatesList */
         ucIndex++;
      }
   }

   /* at this point we may have 0, some or all needed memory
      if we don't have all memory we mark as if we have no
      memory but will return the available size if applicable */
   if (ucAccumulatedSize < ucRequestedEvictionSize)
   {
      ucNumberOfCandidates = 0;
   }

   /* set the total size of eviction candidates in the last entry */
   pEvictCandidatesList[ucNumberOfCandidates] = ucAccumulatedSize;

   return ucNumberOfCandidates;
}

/**
* search if a specific token is already loaded to the cache
*
* @param token - the token to search for
* @return if the token is loaded the return value is set to the loaded address
*         otherwise NULL
*/
static u16_t comrvSearchForLoadedOverlayGroup(comrvOverlayToken_t unToken)
{
   u08_t              ucEntryIndex;
   comrvCacheEntry_t *pCacheEntry;

   /* loop all entries excluding the last entry which holds the comrv tables */
   for (ucEntryIndex = 0 ; ucEntryIndex < D_COMRV_LAST_CACHE_ENTRY_INDEX ; ucEntryIndex+=pCacheEntry->unProperties.stFields.ucSizeInMinGroupSizeUnits)
   {
      pCacheEntry = &g_stComrvCB.stOverlayCache[ucEntryIndex];
      /* if token already loaded */
      if (pCacheEntry->unToken.stFields.uiOverlayGroupID == unToken.stFields.uiOverlayGroupID)
      {
         /* return the actual function location within the loaded overlay group */
         return ucEntryIndex;
      }
   }

   /* overlay group not loaded */
   return D_COMRV_GROUP_NOT_FOUND;
}

/**
* Update a given comrv cache entry was accessed
*
* @param entryIndex - the comrv cache entry being accessed
*
* @return none
*/
static void comrvUpdateCacheEvectionParams(u08_t ucEntryIndex)
{
   comrvCacheEntry_t *pCacheEntry;

#ifdef D_COMRV_EVICTION_LRU

   /* there is no need to update if ucEntryIndex is already MRU */
   if (ucEntryIndex !=  g_stComrvCB.ucMruIndex)
   {
      pCacheEntry = &g_stComrvCB.stOverlayCache[ucEntryIndex];
      /* ucEntryIndex is not the ucLruIndex */
      if (ucEntryIndex !=  g_stComrvCB.ucLruIndex)
      {
         /* update previous item's 'next index' */
         g_stComrvCB.stOverlayCache[pCacheEntry->unLru.stFields.typPrevLruIndex].unLru.stFields.typNextLruIndex =
               pCacheEntry->unLru.stFields.typNextLruIndex;
      }
      else
      {
         /* update the global lru index */
         g_stComrvCB.ucLruIndex = pCacheEntry->unLru.stFields.typNextLruIndex;
         /* update the lru item with the previous item index */
         g_stComrvCB.stOverlayCache[g_stComrvCB.ucLruIndex].unLru.stFields.typPrevLruIndex = D_COMRV_LRU_ITEM;
      }
      /* update ucEntryIndex previous index */
      pCacheEntry->unLru.stFields.typPrevLruIndex = g_stComrvCB.ucMruIndex;
      /* update ucEntryIndex next index - last item (MRU)*/
      pCacheEntry->unLru.stFields.typNextLruIndex = D_COMRV_MRU_ITEM;
      /* update the old mru's next index */
      g_stComrvCB.stOverlayCache[g_stComrvCB.ucMruIndex].unLru.stFields.typNextLruIndex = ucEntryIndex;
      /* update the new MRU */
      g_stComrvCB.ucMruIndex = ucEntryIndex;
   }

#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */
}

/**
* get comrv status
*
* @param pComrvStatus - function output
*
* @return none
*/
void comrvGetStatus(comrvStatus_t* pComrvStatus)
{
   pComrvStatus->pComrvCB    = &g_stComrvCB;
   pComrvStatus->pComrvStack = g_stComrvStackPool;
}

/**
* initialize comrv stack - needs to be invoke by each task (if rtos exist)
* when before initializing task stack.
* in bare-metal apps, this function is called by comrv initialization function
* and the user application doesn't need to do that.
*f
* @param none
*
* @return none
*/
D_COMRV_NO_INLINE void comrvInitApplicationStack(void)
{
   comrvStackFrame_t *pStackPool, *pStackFrame;

   /* disable ints */
   // TODO: disable ints
   // TODO: with rtosal, implement condition check if scheduler
   //       is running or not, if running use RTOS sync api

   /* read stack pool register (t4) */
   M_COMRV_READ_POOL_REG(pStackPool);
   /* save the address of the next available stack frame */
   pStackFrame = pStackPool;
   /* update the next stack pool address */
   pStackPool = (comrvStackFrame_t*)((u08_t*)pStackPool + pStackPool->ssOffsetPrevFrame);
   /* write the new stack pool address */
   M_COMRV_WRITE_POOL_REG(pStackPool);
   /* set the address of COMRV stack in t3 */
   M_COMRV_WRITE_STACK_REG(pStackFrame);
   /* mark the last stack frame */
   pStackFrame->ssOffsetPrevFrame = D_COMRV_END_OF_STACK;
   /* clear token field - bit 0 must be 0 to indicate we came
      from non overlay function */
   pStackFrame->uiCalleeToken = 0;

   /* enable ints */
   // TODO: enable ints
   // TODO: with rtosal, implement condition check if scheduler
   //       is running or not, if running use RTOS sync api
}

/**
* comrv version of memset - set dwords only
*
* @param pMemory - address of the memory to be initialized
*        siVal   - pattern to initialize
*        uiSizeInDwords - number of dword to initialize
*
* @return address of the initialized memory
*/
void* comrvMemset(void* pMemory, s32_t siVal, u32_t uiSizeInDwords)
{
   u32_t uiIndex;

   /* memory initialization loop */
   for (uiIndex = 0 ; uiIndex < uiSizeInDwords ; uiIndex++)
   {
      *((u32_t*)pMemory + uiIndex) = siVal;
   }

   return pMemory;
}

/**
* load offset and multigroup tables
*
* @param None
*
* @return None
*/
void comrvLoadTables(void)
{
   void            *pAddress;
   comrvLoadArgs_t  stLoadArgs;
   comrvErrorArgs_t stErrArgs;
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
   u16_t           *pOffsetTableEntry;
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */
   /* at this point comrv cache is empty so we take the
      last entry and use it to store the multigroup and
      offset tables */
   /* tables are located at offset 0 (first group) */
   stLoadArgs.uiGroupOffset = D_COMRV_TABLES_OFFSET;
   /* set the load group size */
   stLoadArgs.uiSizeInBytes = D_COMRV_OVL_GROUP_SIZE_MIN;
   /* load address is the last comrv cache entry */
   stLoadArgs.pDest         = pOverlayOffsetTable;
   /* load the tables */
   pAddress = comrvLoadOvlayGroupHook(&stLoadArgs);
   /* if group wasn't loaded */
   if (_BUILTIN_EXPECT(pAddress == 0,0))
   {
      stErrArgs.uiErrorNum = D_COMRV_TBL_LOAD_ERR;
      stErrArgs.uiToken    = D_COMRV_TABLES_TOKEN;
      comrvErrorHook(&stErrArgs);
   }
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
   /* multi group table location - we start from the offset table
      and search for the first 0 - signals the start of multi group table */
   pOffsetTableEntry = pOverlayOffsetTable;
   /* loop as long as we have non zero offsets */
   while (*(++pOffsetTableEntry) != 0);
   /* next entry will be the first multi group token */
   pOffsetTableEntry++;
   /* calculate the offset to the multi group table */
   g_stComrvCB.ucMultiGroupOffset = (u08_t)(pOffsetTableEntry - pOverlayOffsetTable);
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */

   /* set cache entry token and properties */
   g_stComrvCB.stOverlayCache[D_COMRV_LAST_CACHE_ENTRY_INDEX].unToken.uiValue = D_COMRV_TABLES_TOKEN;
   g_stComrvCB.stOverlayCache[D_COMRV_LAST_CACHE_ENTRY_INDEX].unProperties.stFields.ucLocked = D_COMRV_ENTRY_LOCKED;
   /* we set the size 0 so that debugger will not continue scanning the cache entries */
   g_stComrvCB.stOverlayCache[D_COMRV_LAST_CACHE_ENTRY_INDEX].unProperties.stFields.ucSizeInMinGroupSizeUnits = 0;
}
