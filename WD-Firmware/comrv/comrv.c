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
#include "common_types.h"
#include "string.h"
#include "comrv.h"
#include "comrv_config.h"
#include "comrv_api.h"

/**
* definitions
*/
#define D_COMRV_NUM_OF_OVERLAY_ENTRIES          (((u08_t*)&__OVERLAY_SEC_END__ - (u08_t*)&__OVERLAY_SEC_START__)/D_COMRV_OVL_GROUP_SIZE_MIN)
#define D_COMRV_END_OF_STACK                    0xDEAD
#define D_COMRV_LRU_LAST_ITEM                   0xFF
#define D_COMRV_LRU_FIRST_ITEM                  0xFF
#define D_COMRV_MAX_GROUP_NUM                   0xFFFF
#define D_COMRV_EVICT_CANDIDATE_MAP_SIZE        4
#define D_COMRV_DWORD_IN_BITS                   32
#define D_COMRV_ENTRY_LOCKED                    1
#define D_COMRV_CANDIDATE_LIST_SIZE             (1+(D_COMRV_OVL_GROUP_SIZE_MAX/D_COMRV_OVL_GROUP_SIZE_MIN))
#define D_COMRV_ENTRY_TOKEN_INIT_VALUE          0x0001FFFE
#define D_COMRV_ENTRY_PROPERTIES_INIT_VALUE     0x04
#define D_COMRV_ENTRY_PROPERTIES_RESET_MASK     0xC3
#define D_COMRV_OFFSET_SCALE_VALUE              4
#define D_COMRV_INVOKE_CALLEE_BIT_0             1
#define D_COMRV_RET_CALLER_BIT_0                0

/* if no profile was set */
#if D_COMRV_PROFILE==0
#define D_COMRV_PROFILE 1
#endif

#if D_COMRV_PROFILE == 1

  #ifdef D_COMRV_EVICTION_LRU
    /* bidirectional linked list - index of previous LRU item */
    typedef u08_t lru_t;
  #elif defined(D_COMRV_EVICTION_LFU)
  #elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
  #endif /* D_COMRV_EVICTION_LRU */

#elif D_COMRV_PROFILE == 2

  #ifdef D_COMRV_EVICTION_LRU
    /* bidirectional linked list - index of previous LRU item */
    typedef u16_t lru_t;
  #elif defined(D_COMRV_EVICTION_LFU)
  #elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
  #endif /* D_COMRV_EVICTION_LRU */

#elif D_COMRV_PROFILE == 3

  #ifdef D_COMRV_EVICTION_LRU
    /* bidirectional linked list - index of previous LRU item */
    typedef u32_t lru_t;
  #elif defined(D_COMRV_EVICTION_LFU)
  #elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
  #endif /* D_COMRV_EVICTION_LRU */

#endif /* D_COMRV_PROFILE_1 */
/**
* macros
*/
/* read token register (t5) */
#define M_COMRV_READ_TOKEN_REG(x)         asm volatile ("mv %0, t5" : "=r" (x)  : );
/* read stack pool register (t4) */
#define M_COMRV_WRITE_POOL_REG(x)         asm volatile ("mv t4, %0" : : "r" (x) );
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
 #define M_COMRV_SET_POOL_ADDR(address)  asm volatile ("la t4, "#address : : : ); \
                                         asm volatile ("ld t4, 0x0(t4)"  : : : );
 #define M_COMRV_SET_STACK_ADDR(address) asm volatile ("la t3, "#address : : : ); \
                                         asm volatile ("ld t3, 0x0(t3)"  : : : );
#elif __riscv_xlen == 32
 #define M_COMRV_SET_POOL_ADDR(address)  asm volatile ("la t4, "#address : : : ); \
                                         asm volatile ("lw t4, 0x0(t4)"  : : : );
 #define M_COMRV_SET_STACK_ADDR(address) asm volatile ("la t3, "#address : : : ); \
                                         asm volatile ("lw t3, 0x0(t3)"  : : : );
#endif

/* overlay group size in D_COMRV_OVL_GROUP_SIZE_MIN granularity */
#define M_COMRV_GET_OVL_GROUP_SIZE(unToken)          (pOverlayOffsetTable[unToken.stFields.overlayGroupID+1] - \
                                                      pOverlayOffsetTable[unToken.stFields.overlayGroupID])
/* overlay group size in bytes */
#define M_COMRV_GET_OVL_GROUP_SIZE_IN_BYTES(unToken) ((pOverlayOffsetTable[unToken.stFields.overlayGroupID+1] - \
                                                       pOverlayOffsetTable[unToken.stFields.overlayGroupID]) << 9)
/* token offset in bytes */
#define M_COMRV_GET_TOKEN_OFFSET_IN_BYTES(unToken)   ((unToken.stFields.offset) * D_COMRV_OFFSET_SCALE_VALUE)
/* overlay group offset in bytes */
#define M_COMRV_GET_GROUP_OFFSET_IN_BYTES(unToken)   ((pOverlayOffsetTable[unToken.stFields.overlayGroupID]) << 9)

/**
* types
*/

/* comrv stack frame */
typedef struct comrvStackFrame
{
  /* holds return address (caller is non overlay function) or return
     offset (caller is overlay function) */
  u32_t uiCallerReturnAddress;
  /* holds callee address (callee is non overlay function) or callee
     token (callee is an overlay function) */
  u32_t uiCalleeToken;
  /* holds the offset in bytes to the previous stack frame */
  s16_t ssOffsetPrevFrame;
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
  /* if the calleetoken is a multi group token, this field holds the
     actual loaded token entry in the multi group table */
  u16_t usCalleeMultiGroupTableEntry;
#endif /* d_comrv_multi_group_support */
} comrvStackFrame_t;

/* overlay token fields */
typedef struct comrvTokenFields
{
  /* overlay token indication 0: address; 1: overlay token */
  u32_t overlayAddressToken:1;
  /* overlay group ID the function resides in */
  u32_t overlayGroupID:16;
  /* data/function offset within the overlay group – 4 bytes granularity */
  u32_t offset:10;
  /* caller thunk indication */
  u32_t thunk:1;
  /* reserved */
  u32_t reserved:1;
  /* specify the heap ID this overlay group belongs to */
  u32_t heapID:2;
  /* multi group indication */
  u32_t multiGroup:1;
} comrvTokenFields_t;

/* overlay token */
typedef union comrvOverlayToken
{
  u32_t              uiValue;
  comrvTokenFields_t stFields;
} comrvOverlayToken_t;

/* cache entry properties */
typedef struct comrvPropertiesFields
{
  /* entry is ucData - can't be evicted */
  u08_t ucLocked:1;
  /* entry is overlay data */
  u08_t ucData:1;
  /* loaded group size in D_COMRV_OVL_GROUP_SIZE_MIN granularity */
  u08_t sizeInMinGroupSizeUnits:4;
  u08_t reserved:2;
} comrvPropertiesFields_t;

/* cache entry */
typedef union comrvEntryProperties
{
  u08_t                   ucValue;
  comrvPropertiesFields_t stFields;
} comrvEntryProperties_t;

/* overlay cache entry */
typedef struct comrvOverlayHeapEntry
{
#ifdef D_COMRV_EVICTION_LRU
  /* bidirectional linked list - index of previous LRU item */
  lru_t                  ucPrevIndex;
  /* bidirectional linked list - index of next LRU item */
  lru_t                  ucNextIndex;
#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */
  /* entry properties */
  comrvEntryProperties_t unProperties;
  /* loaded token */
  comrvOverlayToken_t    unToken;
  /* fixed RAM address that this cache entry refers to */
  void*                  pFixedEntryAddress;
} comrvCacheEntry_t;

/* comrv control block */
typedef struct comrvCB
{
#ifdef D_COMRV_EVICTION_LRU
  /* holds the cache entry index of the LRU item */
  lru_t             ucLruIndex;
  /* holds the cache entry index of the MRU item */
  lru_t             ucMruIndex;
#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */
  /* number of cache entries */
  u08_t             ucNumOfOverlayEntries;
  /* the cache entries */
  comrvCacheEntry_t stOverlayCache[];
} comrvCB_t;

/**
* local prototypes
*/
static void  comrvUpdateHeapEntryAccess      (u08_t ucEntryIndex);
static u08_t comrvGetEvictionCandidates      (u08_t ucRequestedEvictionSize, u08_t* pEvictCandidatesList);
static void* comrvSearchForLoadedOverlayGroup(comrvOverlayToken_t unToken);

/**
* external prototypes
*/
/* main comrv entry function - all overlay functions are invoked
   through this function (the address of comrvEntry() is set in reg t6) */
extern void  comrvEntry              (void);

/* user hook functions - user application must implement the following 4 functions */
extern void  comrvMemcpyHook         (void* pDest, void* pSrc, u32_t uiSizeInBytes);
extern u32_t comrvCrcCalcHook        (void* pAddress, u16_t usMemSizeInBytes);
extern void  comrvNotificationHook   (u32_t uiNotificationNum, u32_t uiToken);
extern void* comrvLoadOvlayGroupHook (u32_t uiGroupOffset, void* pDest, u32_t uiSizeInBytes);

/**
* global variables
*/
comrvStackFrame_t* pStackStartAddr = (comrvStackFrame_t*)&__OVERLAY_STACK_START__;
comrvCB_t*         pComrvCB;
u16_t *pOverlayOffsetTable = (u16_t*)&overlayOffsetTable;
comrvOverlayToken_t *pOverlayMultiGroupTokensTable = (comrvOverlayToken_t*)&overlayMultiGroupTokensTable;

/**
* COM-RV initialization function
*
* @param  none
*
* @return none
*/
void comrvInit(void)
{
   u08_t              ucIndex;
   void*              pBaseAddress = &__OVERLAY_SEC_START__;
   comrvStackFrame_t* pStackEndAddr = (comrvStackFrame_t*)&__OVERLAY_STACK_END__ - 1;

   /* determine the location of comrv internal control block */
   pComrvCB = (comrvCB_t*)pStackStartAddr;// - sizeof(comrvCB_t));
   /* determine the number of heap entries */
   pComrvCB->ucNumOfOverlayEntries = D_COMRV_NUM_OF_OVERLAY_ENTRIES;
   /* determine the location of comrv overlay heap structure */
   //pComrvCB->stOverlayCache = (comrvCacheEntry_t*)((u08_t*)pComrvCB - (pComrvCB->ucNumOfOverlayEntries*sizeof(comrvCacheEntry_t)));
#ifdef D_COMRV_EVICTION_LRU
   /* initialize all heap entries */
   for (ucIndex = 0 ; ucIndex < pComrvCB->ucNumOfOverlayEntries ; ucIndex++)
   {
      /* initially each entry points to the previous and next neighbor cells */
      pComrvCB->stOverlayCache[ucIndex].ucPrevIndex          = ucIndex-1;
      pComrvCB->stOverlayCache[ucIndex].ucNextIndex          = ucIndex+1;
      pComrvCB->stOverlayCache[ucIndex].unToken.uiValue      = D_COMRV_ENTRY_TOKEN_INIT_VALUE;
      pComrvCB->stOverlayCache[ucIndex].unProperties.ucValue = D_COMRV_ENTRY_PROPERTIES_INIT_VALUE;
      pComrvCB->stOverlayCache[ucIndex].pFixedEntryAddress             = pBaseAddress;
      pBaseAddress = ((u08_t*)pBaseAddress + D_COMRV_OVL_GROUP_SIZE_MIN);
   }
   /* mark the last entry in the LRU list */
   pComrvCB->stOverlayCache[ucIndex-1].ucNextIndex = D_COMRV_LRU_LAST_ITEM;
   /* set the index of the list LRU and MRU */
   pComrvCB->ucLruIndex = 0;
   pComrvCB->ucMruIndex = ucIndex-1;
#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */
   pStackStartAddr = (comrvStackFrame_t*)&pComrvCB->stOverlayCache[pComrvCB->ucNumOfOverlayEntries];
   /* initialize all stack entries */
   for (pStackStartAddr++ ; pStackStartAddr < pStackEndAddr ; pStackStartAddr++)
   {
      pStackStartAddr->ssOffsetPrevFrame = (s16_t)-sizeof(comrvStackFrame_t);
   }

   /* set the address of COMRV stack pool */
   pStackStartAddr--;
   M_COMRV_SET_POOL_ADDR(pStackStartAddr);

   /* set the address of COMRV entry point */
   M_COMRV_SET_ENTRY_ADDR(comrvEntry);

#ifndef D_COMRV_USE_OS
   /* in os-less applications, stack register is initialized here;
      this must be done after the stack pool register was
      initialized (M_COMRV_SET_POOL_ADDR) */
   comrvInitApplicationStack();
#endif /* D_COMRV_USE_OS */
}

/**
* This function is performing the following:
* 1.  load requested token (from t5 register)
* 2.  search for the requested token in the loaded cache
* 2.a for multi group search for each multi group token
* 3.  if group not loaded, load it
* 4.  return the address of the function (call or return address)
*
* @param none
*
* @return void* - address of the overlay function/data
*/
void* comrvGetAddressFromToken(void)
{
   comrvOverlayToken_t unToken;
   comrvCacheEntry_t*   pEntry;
   u08_t               ucIsInvoke;
   void*               pAddress;
   comrvStackFrame_t*  pComrvStackFrame;
   u16_t               usOverlayGroupSize, usOffset;
   u08_t               ucNumOfEvictionCandidates, ucIndex, ucSizeOfEvictionCandidates;
   u08_t               ucEntryIndex, ucEvictCandidateList[D_COMRV_CANDIDATE_LIST_SIZE];
#ifdef D_COMRV_CRC
   u32_t               uiCrc;
#endif /* D_COMRV_CRC */
#ifdef D_COMRV_FW_INSTRUMENTATION
   u32_t               uiProfilingIndication;
#endif /* D_COMRV_FW_INSTRUMENTATION */
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
   u32_t               uiTemp;
   u16_t               usSelectedMultiGroupEntry;
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
      pComrvStackFrame = (comrvStackFrame_t*)((u32_t)pComrvStackFrame & (~D_COMRV_PROFILING_INVOKE_VAL));
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
   if (unToken.stFields.multiGroup == 0)
   {
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */
      /* search for token */
      pAddress = comrvSearchForLoadedOverlayGroup(unToken);
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
   }
   /* search for a multi-group overlay token */
   else
   {
      /* first ucEntryIndex to search from in the multi group table is determined by the overlayGroupID
         field of the requested token */
      ucEntryIndex = unToken.stFields.overlayGroupID;
      do
      {
         /* search for the token */
         pAddress = comrvSearchForLoadedOverlayGroup(pOverlayMultiGroupTokensTable[ucEntryIndex++]);
      /* continue the search as long as the group wasn't found and we have additional tokens */
      } while ((pAddress == NULL) && (pOverlayMultiGroupTokensTable[ucEntryIndex].uiValue != 0));

      /* save the selected multi group entry */
      usSelectedMultiGroupEntry = ucEntryIndex-1;
   }
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */

   /* get the group size */
   usOverlayGroupSize = M_COMRV_GET_OVL_GROUP_SIZE(unToken);

   /* if the data/function is not loaded we need to evict and load it */
   if (pAddress == NULL)
   {
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
      /* if the requested token is a multi-group token */
      if (unToken.stFields.multiGroup != 0)
      {
         /* for now we take the first token in the list of tokens */
         unToken = pOverlayMultiGroupTokensTable[unToken.stFields.overlayGroupID];
         /* save the selected multi group entry */
         usSelectedMultiGroupEntry = unToken.stFields.overlayGroupID;
      }
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */

      /* disable ints */
      // TODO: disable ints

      /* get eviction candidates according to the requested pOverlayGroupSize */
      ucNumOfEvictionCandidates = comrvGetEvictionCandidates(usOverlayGroupSize, ucEvictCandidateList);

      ucEntryIndex = 0;
      /* we need to handle heap fragmentation since we got more
         than 1 eviction candidate */
      if (ucNumOfEvictionCandidates > 1)
      {
         /* no handling to the last eviction candidate */
         ucNumOfEvictionCandidates--;
         while (ucEntryIndex < ucNumOfEvictionCandidates)
         {
            /* get the candidate entry */
            pEntry = &pComrvCB->stOverlayCache[ucEvictCandidateList[ucEntryIndex]];
            /* calc the source address */
            pAddress = ((u08_t*)pEntry->pFixedEntryAddress + M_COMRV_GET_OVL_GROUP_SIZE_IN_BYTES(pEntry->unToken));
            /* perform code copy */
            comrvMemcpyHook(pEntry->pFixedEntryAddress, pAddress,
                  pComrvCB->stOverlayCache[ucEvictCandidateList[ucEntryIndex]].pFixedEntryAddress - pAddress);

            /* after code copy we need to align the entries structures */
            for(ucIndex = ucEvictCandidateList[ucEntryIndex] ; ucIndex < ucEvictCandidateList[ucEntryIndex+1] ; ucIndex++)
            {
               pEntry = &pComrvCB->stOverlayCache[ucIndex + pComrvCB->stOverlayCache[ucIndex].unProperties.stFields.sizeInMinGroupSizeUnits];
#ifdef D_COMRV_OVL_DATA_SUPPORT
               /* an overlay data is present when handling defragmentation */
               if (pEntry->unProperties.ucData)
               {
                  /* enable ints */
                  // TODO: enable ints
                  comrvNotificationHook(D_COMRV_OVL_DATA_DEFRAG_ERR, unToken.ucValue);
               }
#endif /* D_COMRV_OVL_DATA_SUPPORT */
               pComrvCB->stOverlayCache[ucIndex].unProperties = pEntry->unProperties;
               pComrvCB->stOverlayCache[ucIndex].unToken.uiValue = pEntry->unToken.uiValue;
#ifdef D_COMRV_EVICTION_LRU
               pComrvCB->stOverlayCache[pEntry->ucPrevIndex].ucNextIndex = ucIndex;
               pComrvCB->stOverlayCache[pEntry->ucNextIndex].ucPrevIndex = ucIndex;
#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */
            }
            ucEntryIndex++;
         }
         ucNumOfEvictionCandidates++;
      }
      /* this means the end user locked all entries */
      else if (ucNumOfEvictionCandidates == 0)
      {
         /* enable ints */
         // TODO: enable ints
         comrvNotificationHook(D_COMRV_NO_AVAILABLE_ENTRY_ERR, unToken.uiValue);
      }
      ucIndex = ucEvictCandidateList[ucEntryIndex];
      /* update the entry access */
      comrvUpdateHeapEntryAccess(ucIndex);
      /* update the heap entry with the new token */
      pComrvCB->stOverlayCache[ucIndex].unToken.uiValue = unToken.uiValue;
      /* update the heap entry properties with the group size */
      pComrvCB->stOverlayCache[ucIndex].unProperties.stFields.sizeInMinGroupSizeUnits = usOverlayGroupSize;
      /* if evicted size is larger than requested size we need to update the remaining tail*/
      ucSizeOfEvictionCandidates = ucEvictCandidateList[ucNumOfEvictionCandidates] - usOverlayGroupSize;
      if (ucSizeOfEvictionCandidates != 0)
      {
         ucEntryIndex += usOverlayGroupSize;
         pComrvCB->stOverlayCache[ucEntryIndex].unProperties.stFields.sizeInMinGroupSizeUnits = ucSizeOfEvictionCandidates;
#ifdef D_COMRV_EVICTION_LRU
         pComrvCB->stOverlayCache[pComrvCB->stOverlayCache[ucEntryIndex].ucPrevIndex].ucNextIndex = pComrvCB->stOverlayCache[ucEntryIndex].ucNextIndex;
         pComrvCB->stOverlayCache[ucEntryIndex].ucNextIndex           = ucIndex;
         pComrvCB->stOverlayCache[ucEntryIndex].ucPrevIndex           = D_COMRV_LRU_FIRST_ITEM;
         /* mark the group ID so that it won't pop in the next search */
         pComrvCB->stOverlayCache[ucEntryIndex].unToken.uiValue       = D_COMRV_ENTRY_TOKEN_INIT_VALUE;
         pComrvCB->stOverlayCache[ucEntryIndex].unProperties.ucValue &= D_COMRV_ENTRY_PROPERTIES_RESET_MASK;
         pComrvCB->ucLruIndex = ucIndex;
#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */
      }
      /* TODO:Q: should I temporary mark it before load and unmark it after load, so the memory
         won't move in case of context switch during the load */
      /* enable ints */
      // TODO
      /* the group size in bytes */
      usOverlayGroupSize <<= 9;
      /* now we can load the overlay group */
      pAddress = comrvLoadOvlayGroupHook(M_COMRV_GET_GROUP_OFFSET_IN_BYTES(pComrvCB->stOverlayCache[ucIndex].unToken),
            pComrvCB->stOverlayCache[ucIndex].pFixedEntryAddress, usOverlayGroupSize);
      /* if group wasn't loaded */
      if (pAddress == NULL)
      {
         comrvNotificationHook(D_COMRV_LOAD_ERR, unToken.uiValue);
      }

#ifdef D_COMRV_CRC
      /* calculate crc */
      uiCrc = comrvCrcCalcHook(pAddress, usOverlayGroupSize-sizeof(u32_t));
      if (uiCrc != *((u08_t*)pAddress + (usOverlayGroupSize-sizeof(u32_t))))
      {
         comrvNotificationHook(D_COMRV_CRC_CHECK_ERR, unToken.ucValue);
      }
#endif /* D_COMRV_CRC */

#ifdef D_COMRV_FW_INSTRUMENTATION
      /* update for FW profiling loaded the function */
      uiProfilingIndication |= D_COMRV_PROFILING_LOAD_VAL;
#endif /* D_COMRV_FW_INSTRUMENTATION */
   }
   else
   {
      usOverlayGroupSize <<= 9;
   }

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
         /* we now are at the point of loading a multi-group toke so we need to take the
            previous token for which the return address refers to */
         unToken = pOverlayMultiGroupTokensTable[pComrvStackFrame->usCalleeMultiGroupTableEntry];
         /* get the offset */
         uiTemp = M_COMRV_GET_TOKEN_OFFSET_IN_BYTES(unToken);
         /* get the token group size */
         usOverlayGroupSize = M_COMRV_GET_OVL_GROUP_SIZE_IN_BYTES(unToken);
         /* calculate the actual return offset */
         usOffset += ((u32_t)(pComrvStackFrame->uiCalleeToken) - uiTemp) & (--usOverlayGroupSize);
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
   comrvNotificationHook(uiProfilingIndication , unToken.ucValue);
#endif /* D_COMRV_FW_INSTRUMENTATION */

   /* group is now loaded to memory so we can return the address of the data/function */
   return (void*)((u08_t*)pAddress + usOffset);
}

/**
* Get comrv heap eviction candidates according to a given size
*
* @param requestedEvictionSize - size requested for eviction; expressed in
*                                units of D_COMRV_OVL_GROUP_SIZE_MIN
*        pEvictCandidatesList  - output eviction candidate list of comrv heap indexes
* @return number of eviction candidates in the output list
*/
u08_t comrvGetEvictionCandidates(u08_t ucRequestedEvictionSize, u08_t* pEvictCandidatesList)
{
   u08_t ucAccumulatedSize = 0, ucIndex = 0;
   u08_t ucEntryIndex, ucNumberOfCandidates = 0;
   u32_t uiEvictCandidateMap[D_COMRV_EVICT_CANDIDATE_MAP_SIZE], uiCandidates;

   /* first lets clear the uiCandidates list */
   memset(uiEvictCandidateMap, 0, sizeof(u32_t)*D_COMRV_EVICT_CANDIDATE_MAP_SIZE);

#ifdef D_COMRV_EVICTION_LRU
   /* get the first lru entry */
   ucEntryIndex = pComrvCB->ucLruIndex;
   /* loop LRU list until the eviction uiCandidates accumulated size
      reaches the requested eviction size */
   do
   {
      /* verify the entry isn't locked */
      if (pComrvCB->stOverlayCache[ucEntryIndex].unProperties.stFields.ucLocked != D_COMRV_ENTRY_LOCKED)
      {
         /* count the number of uiCandidates */
         ucNumberOfCandidates++;
         /* accumulate size */
         ucAccumulatedSize += pComrvCB->stOverlayCache[ucEntryIndex].unProperties.stFields.sizeInMinGroupSizeUnits;
         /* set the eviction candidate in the eviction map */
         uiEvictCandidateMap[ucEntryIndex/D_COMRV_DWORD_IN_BITS] |= (1 << ucEntryIndex);
      }
      /* move to the next LRU candidate */
      ucEntryIndex = pComrvCB->stOverlayCache[ucEntryIndex].ucNextIndex;
   /* loop as long as we didn't get to the requested eviction size or we reached end of the list
      (means that all entries are locked) */
   } while (ucAccumulatedSize < ucRequestedEvictionSize && ucEntryIndex != D_COMRV_LRU_LAST_ITEM);

#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */

   /* now we have eviction uiCandidates bitmap of heap entries - lets create
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
         uiCandidates -= pEvictCandidatesList[ucIndex]--;
         pEvictCandidatesList[ucIndex++] += ucEntryIndex*D_COMRV_DWORD_IN_BITS;
      }
   }

   /* set the total size of eviction candidates in the last entry */
   pEvictCandidatesList[ucNumberOfCandidates] = ucAccumulatedSize;

   return ucNumberOfCandidates;
}

/**
* search if a specific token is already loaded to the heap
*
* @param token - the token to search for
* @return if the token is loaded the return value is set to the loaded address
*         otherwise NULL
*/
static void* comrvSearchForLoadedOverlayGroup(comrvOverlayToken_t unToken)
{
   u08_t ucEntryIndex;

   /* loop all entries */
   for (ucEntryIndex = 0 ; ucEntryIndex < pComrvCB->ucNumOfOverlayEntries ; ucEntryIndex++)
   {
      /* if token already loaded */
      if (pComrvCB->stOverlayCache[ucEntryIndex].unToken.stFields.overlayGroupID == unToken.stFields.overlayGroupID)
      {
         /* update that the entry was accessed */
         comrvUpdateHeapEntryAccess(ucEntryIndex);
         /* return the actual function location within the loaded overlay group */
         return pComrvCB->stOverlayCache[ucEntryIndex].pFixedEntryAddress;
      }
   }
   /* overlay group not loaded */
   return NULL;
}

/**
* Update a given comrv heap entry was accessed
*
* @param entryIndex - the comrv heap entry being accessed
*
* @return none
*/
static void comrvUpdateHeapEntryAccess(u08_t ucEntryIndex)
{
#ifdef D_COMRV_EVICTION_LRU
   /* there is no need to update if ucEntryIndex is already MRU */
   if (ucEntryIndex !=  pComrvCB->ucMruIndex)
   {
      /* ucEntryIndex is not the ucLruIndex */
      if (ucEntryIndex !=  pComrvCB->ucLruIndex)
      {
         /* update previous item's 'next index' */
         pComrvCB->stOverlayCache[pComrvCB->stOverlayCache[ucEntryIndex].ucPrevIndex].ucNextIndex = pComrvCB->stOverlayCache[ucEntryIndex].ucNextIndex;
      }
      else
      {
         /* update the global lru index */
         pComrvCB->ucLruIndex = pComrvCB->stOverlayCache[ucEntryIndex].ucNextIndex;
         /* update the lru item with the previous item index */
         pComrvCB->stOverlayCache[pComrvCB->ucLruIndex].ucPrevIndex = D_COMRV_LRU_FIRST_ITEM;
      }
      /* update ucEntryIndex previous index */
      pComrvCB->stOverlayCache[ucEntryIndex].ucPrevIndex = pComrvCB->ucMruIndex;
      /* update ucEntryIndex next index - last item (MRU)*/
      pComrvCB->stOverlayCache[ucEntryIndex].ucNextIndex = D_COMRV_LRU_LAST_ITEM;
      /* update the old mru's next index */
      pComrvCB->stOverlayCache[pComrvCB->ucMruIndex].ucNextIndex = ucEntryIndex;
      /* update the new MRU */
      pComrvCB->ucMruIndex = ucEntryIndex;
   }
#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */
}

/**
* get comrv status
*
* @param none
*
* @return none
*/
void comrvGetStatus(void)
{
   // TODO: check comrv stack
}

/**
* initialize comrv stack - needs to be invoke by each task (if rtos exist)
* when before initializing task stack.
* in os-less apps, this function is called by comrv initialization function
* and the user application doesn't need to do that.
*
* @param none
*
* @return none
*/
// TODO: ronen - use define for no inline
__attribute__((noinline)) void comrvInitApplicationStack(void)
{
   volatile comrvStackFrame_t *pStackPool, *pStackFrame;

   /* disable ints */
   // TODO: disable ints

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
}
