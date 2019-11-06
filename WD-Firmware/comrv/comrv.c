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
/**
* macros
*/
#define M_COMRV_READ_TOKEN_REG(x)        asm volatile ("mv %0, t5" : "=r" (x)  : );
#define M_COMRV_SET_ENTRY_ADDR(address)  asm volatile ("la t6, "#address : : : );
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

#define M_COMRV_GET_OVL_GROUP_SIZE(groupID)  (pOverlayOffsetTable[groupID+1] - pOverlayOffsetTable[groupID])
#define M_COMRV_GET_GROUP_OFFSET(token)      ((pOverlayOffsetTable[token.fields.overlayGroupID]) << 9)
#define M_COMRV_GET_OFFSET(token)            ((token.fields.offset) * D_COMRV_OFFSET_SCALE_VALUE)

/**
* types
*/
typedef struct comrvStackFrame
{
  /* holds return address (caller is non overlay function) or return
     offset (caller is overlay function) */
  u32_t callerReturnAddress;
  /* holds callee address (callee is non overlay function) or callee
     token (callee is an overlay function) */
  u32_t calleeToken;
  /* holds the offset in bytes to the previous stack frame */
  s16_t offsetPrevFrame;
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
  /* if the calleetoken is a multi group token, this field holds the
     actual loaded token entry in the multi group table */
  u16_t calleeMultiGroupTableEntry;
#endif /* d_comrv_multi_group_support */
} comrvStackFrame_t;

/* overlay token fields */
typedef struct comrvToken
{
  /* overlay token indication 0: address; 1: overlay token */
  u32_t overlayAddressToken:1;
  /* overlay group ID the function resides in */
  u32_t overlayGroupID:16;
  /* data/function offset within the overlay group – 4 bytes granularity */
  u32_t offset:10;
  /* 2 reserved bits */
  u32_t reserved:2;
  /* specify the heap ID this overlay group belongs to */
  u32_t heapID:2;
  /* multi group indication */
  u32_t multiGroup:1;
} comrvTokenFields_t;

/* overlay token */
typedef union comrvOverlayToken
{
  u32_t              value;
  comrvTokenFields_t fields;
} comrvOverlayToken_t;

/* overlay token */
typedef struct comrvPropertiesFields
{
  u08_t locked:1;
  u08_t data:1;
  u08_t size:3;
  u08_t reserved:3;
} comrvPropertiesFields_t;

typedef union comrvEntryProperties
{
  u08_t                   value;
  comrvPropertiesFields_t fields;
} comrvEntryProperties_t;

/* overlay token entry */
typedef struct comrvOverlayHeapEntry
{
#ifdef D_COMRV_EVICTION_LRU
  u08_t prevIndex;
  u08_t nextIndex;
#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */
  comrvEntryProperties_t properties;
  comrvOverlayToken_t    token;
  void*                  pAddress;
} comrvHeapEntry_t;

/* com-rv control block */
typedef struct comrvCB
{
#ifdef D_COMRV_EVICTION_LRU
  u08_t            lruIndex;
  u08_t            mruIndex;
#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */
  u08_t            numOfOverlayEntries;
  comrvHeapEntry_t overlayHeap[];
} comrvCB_t;

/**
* local prototypes
*/
static void  comrvUpdateHeapEntryAccess      (u08_t entryIndex);
static u08_t comrvGetEvictionCandidates      (u08_t requestedEvictionSize, u08_t* pEvictCandidatesList);
static void* comrvSearchForLoadedOverlayGroup(comrvOverlayToken_t token);

/**
* external prototypes
*/
extern void  comrv_entry               (void);
extern void  comrvMemcpyHook           (void* pDest, void* pSrc, u32_t sizeInBytes);
extern u32_t comrvCrcCalcHook          (void* pAddress, u16_t memprySize);
extern void  comrvNotificationHook     (u32_t notificationNum, u32_t token);
extern void* comrvLoadOvlayGroupHook   (u32_t groupOffset, void* pDest, u32_t sizeInBytes);

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
   u08_t              i;
   void*              pBaseAddress = &__OVERLAY_SEC_START__;
   comrvStackFrame_t* pStackEndAddr = (comrvStackFrame_t*)&__OVERLAY_STACK_END__ - 1;

   /* determine the location of comrv internal control block */
   pComrvCB = (comrvCB_t*)pStackStartAddr;// - sizeof(comrvCB_t));
   /* determine the number of heap entries */
   pComrvCB->numOfOverlayEntries = D_COMRV_NUM_OF_OVERLAY_ENTRIES;
   /* determine the location of comrv overlay heap structure */
   //pComrvCB->overlayHeap = (comrvHeapEntry_t*)((u08_t*)pComrvCB - (pComrvCB->numOfOverlayEntries*sizeof(comrvHeapEntry_t)));
#ifdef D_COMRV_EVICTION_LRU
   /* initialize all heap entries */
   for (i = 0 ; i < pComrvCB->numOfOverlayEntries ; i++)
   {
      /* initially each entry points to the previous and next neighbor cells */
      pComrvCB->overlayHeap[i].prevIndex        = i-1;
      pComrvCB->overlayHeap[i].nextIndex        = i+1;
      pComrvCB->overlayHeap[i].token.value      = D_COMRV_ENTRY_TOKEN_INIT_VALUE;
      pComrvCB->overlayHeap[i].properties.value = D_COMRV_ENTRY_PROPERTIES_INIT_VALUE;
      pComrvCB->overlayHeap[i].pAddress         = pBaseAddress;
      pBaseAddress = ((u08_t*)pBaseAddress + D_COMRV_OVL_GROUP_SIZE_MIN);
   }
   /* mark the last entry in the LRU list */
   pComrvCB->overlayHeap[i-1].nextIndex = D_COMRV_LRU_LAST_ITEM;
   /* set the index of the list LRU and MRU */
   pComrvCB->lruIndex = 0;
   pComrvCB->mruIndex = i-1;
#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */
   pStackStartAddr = (comrvStackFrame_t*)&pComrvCB->overlayHeap[pComrvCB->numOfOverlayEntries];
   /* Init comrv stack - mark the last stack frame */
   pStackStartAddr->offsetPrevFrame = D_COMRV_END_OF_STACK;
   /* initialize all stack entries */
   for (pStackStartAddr++ ; pStackStartAddr < pStackEndAddr ; pStackStartAddr++)
   {
      pStackStartAddr->offsetPrevFrame = (s16_t)-sizeof(comrvStackFrame_t);
      pStackStartAddr->calleeToken = 0;
   }

#ifndef D_COMRV_USE_OS
   /* set the address of COMRV stack and initialize it */
   pStackStartAddr--;
   M_COMRV_SET_STACK_ADDR(pStackStartAddr);
   pStackStartAddr->offsetPrevFrame = D_COMRV_END_OF_STACK;
#endif /* D_COMRV_USE_OS */

   /* set the address of COMRV stack pool */
   pStackStartAddr--;
   M_COMRV_SET_POOL_ADDR(pStackStartAddr);

   /* set the address of COMRV entry point */
   M_COMRV_SET_ENTRY_ADDR(comrv_entry);
}

/**
* Search if current overlay token is already loaded
*
* @param pComrvStackFrame - address of current comrv stack frame
*
* @return void* - address of the overlay function/data
*/
void* comrvGetAddressFromToken(comrvStackFrame_t* pComrvStackFrame)
{
   comrvOverlayToken_t token;
   comrvHeapEntry_t*   pEntry;
   u08_t               isInvoke;
   void*               pAddress;
   u16_t               overlayGroupSize, offset;
   u08_t               numOfEvictionCandidates, index, sizeOfEvictionCandidates;
   u08_t               entryIndex, evictCandidateList[D_COMRV_CANDIDATE_LIST_SIZE];
#ifdef D_COMRV_CRC
   u32_t               crc;
#endif /* D_COMRV_CRC */
#ifdef D_COMRV_FW_PROFILING
   u32_t               profilingIndication;
#endif /* D_COMRV_FW_PROFILING */
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
   u32_t               temp;
   u16_t               selectedMultiGroupEntry;
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */

   /* get the invoke indication */
   isInvoke = (u32_t)pComrvStackFrame & D_COMRV_PROFILING_INVOKE_VAL;
   /* are we calling a new overlay function */
   if (isInvoke == D_COMRV_PROFILING_INVOKE_VAL)
   {
      /* clear the invoke indication from pComrvStackFrame address */
      pComrvStackFrame = (comrvStackFrame_t*)((u32_t)pComrvStackFrame & (~D_COMRV_PROFILING_INVOKE_VAL));
#ifdef D_COMRV_FW_PROFILING
      profilingIndication = D_COMRV_NO_LOAD_AND_INVOKE_IND;
#endif /* D_COMRV_FW_PROFILING */
   }
   /* we are returning to an overlay function or accessing overlay data */
   else
   {
#ifdef D_COMRV_FW_PROFILING
      profilingIndication = D_COMRV_NO_LOAD_AND_RETURN_IND;
#endif /* D_COMRV_FW_PROFILING */
   }

   /* read the requested token value */
   M_COMRV_READ_TOKEN_REG(token.value);

#ifdef D_COMRV_MULTI_GROUP_SUPPORT
   /* if the requested token isn't a multi-group token */
   if (token.fields.multiGroup == 0)
   {
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */
      /* search for token */
      pAddress = comrvSearchForLoadedOverlayGroup(token);
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
   }
   /* search for a multi-group overlay token */
   else
   {
      pAddress = NULL;
      /* first entryIndex to search from in the multi group table is determined by the overlayGroupID
         field of the requested token */
      entryIndex = token.fields.overlayGroupID;
      do
      {
         /* search for the token */
         pAddress = comrvSearchForLoadedOverlayGroup(pOverlayMultiGroupTokensTable[entryIndex]);
      /* continue the search as long as the group wasn't found and we have additional tokens */
      } while ((pAddress == NULL) && (pOverlayMultiGroupTokensTable[entryIndex].value != 0));

      /* save the selected multi group entry */
      selectedMultiGroupEntry = entryIndex-1;
   }
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */

   /* get the group size */
   overlayGroupSize = M_COMRV_GET_OVL_GROUP_SIZE(token.fields.overlayGroupID);

   /* if the data/function is not loaded we need to evict and load it */
   if (pAddress == NULL)
   {
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
      /* if the requested token is a multi-group token */
      if (token.fields.multiGroup != 0)
      {
         /* for now we take the first token in the list of tokens */
         token = pOverlayMultiGroupTokensTable[token.fields.overlayGroupID];
         /* save the selected multi group entry */
         selectedMultiGroupEntry = token.fields.overlayGroupID;
      }
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */

      /* disable ints */
      // TODO: disable ints

      /* get eviction candidates according to the requested pOverlayGroupSize */
      numOfEvictionCandidates = comrvGetEvictionCandidates(overlayGroupSize, evictCandidateList);

      entryIndex = 0;
      /* we need to handle heap fragmentation since we got more
         than 1 eviction candidate */
      if (numOfEvictionCandidates > 1)
      {
         /* no handling to the last eviction candidate */
         numOfEvictionCandidates--;
         while (entryIndex < numOfEvictionCandidates)
         {
            /* get the candidate entry */
            pEntry = &pComrvCB->overlayHeap[evictCandidateList[entryIndex]];
            /* calc the source address */
            pAddress = ((u32_t*)pEntry->pAddress + M_COMRV_GET_OVL_GROUP_SIZE(pEntry->token.fields.overlayGroupID));
            /* perform code copy */
            comrvMemcpyHook(pEntry->pAddress, pAddress,
                  pComrvCB->overlayHeap[evictCandidateList[entryIndex]].pAddress - pAddress);

            /* after code copy we need to align the entries structures */
            for(index = evictCandidateList[entryIndex] ; index < evictCandidateList[entryIndex+1] ; index++)
            {
               pEntry = &pComrvCB->overlayHeap[index + pComrvCB->overlayHeap[index].properties.fields.size];
#ifdef D_COMRV_OVL_DATA_SUPPORT
               /* an overlay data is present when handling defragmentation */
               if (pEntry->properties.data)
               {
                  /* enable ints */
                  // TODO: enable ints
                  comrvNotificationHook(D_COMRV_OVL_DATA_DEFRAG_ERR, token.value);
               }
#endif /* D_COMRV_OVL_DATA_SUPPORT */
               pComrvCB->overlayHeap[index].properties = pEntry->properties;
               pComrvCB->overlayHeap[index].token.value = pEntry->token.value;
#ifdef D_COMRV_EVICTION_LRU
               pComrvCB->overlayHeap[pEntry->prevIndex].nextIndex = index;
               pComrvCB->overlayHeap[pEntry->nextIndex].prevIndex = index;
#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */
            }
            entryIndex++;
         }
         numOfEvictionCandidates++;
      }
      /* this means the end user locked all entries */
      else if (numOfEvictionCandidates == 0)
      {
         /* enable ints */
         // TODO: enable ints
         comrvNotificationHook(D_COMRV_NO_AVAILABLE_ENTRY_ERR, token.value);
      }
      index = evictCandidateList[entryIndex];
      /* update the entry access */
      comrvUpdateHeapEntryAccess(index);
      /* update the heap entry with the new token */
      pComrvCB->overlayHeap[index].token.value = token.value;
      /* update the heap entry properties with the group size */
      pComrvCB->overlayHeap[index].properties.fields.size = overlayGroupSize;
      /* if evicted size is larger than requested size we need to update the remaining tail*/
      sizeOfEvictionCandidates = evictCandidateList[numOfEvictionCandidates] - overlayGroupSize;
      if (sizeOfEvictionCandidates != 0)
      {
         entryIndex += overlayGroupSize;
         pComrvCB->overlayHeap[entryIndex].properties.fields.size = sizeOfEvictionCandidates;
#ifdef D_COMRV_EVICTION_LRU
         pComrvCB->overlayHeap[pComrvCB->overlayHeap[entryIndex].prevIndex].nextIndex = pComrvCB->overlayHeap[entryIndex].nextIndex;
         pComrvCB->overlayHeap[entryIndex].nextIndex         = index;
         pComrvCB->overlayHeap[entryIndex].prevIndex         = D_COMRV_LRU_FIRST_ITEM;
         /* mark the group ID so that it won't pop in the next search */
         pComrvCB->overlayHeap[entryIndex].token.value       = D_COMRV_ENTRY_TOKEN_INIT_VALUE;
         pComrvCB->overlayHeap[entryIndex].properties.value &= D_COMRV_ENTRY_PROPERTIES_RESET_MASK;
         pComrvCB->lruIndex = index;
#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */
      }
      /* TODO:Q: should I temporary mark it before load and unmark it after load, so the memory
         won't move in case of context switch during the load */
      /* enable ints */
      // TODO
      /* the group size in bytes */
      overlayGroupSize <<= 9;
      /* now we can load the overlay group */
      pAddress = comrvLoadOvlayGroupHook(M_COMRV_GET_GROUP_OFFSET(pComrvCB->overlayHeap[index].token),
            pComrvCB->overlayHeap[index].pAddress, overlayGroupSize);
      /* if group wasn't loaded */
      if (pAddress == NULL)
      {
         comrvNotificationHook(D_COMRV_LOAD_ERR, token.value);
      }

#ifdef D_COMRV_CRC
      /* calculate crc */
      crc = comrvCrcCalcHook(pAddress, overlayGroupSize-sizeof(u32_t));
      if (crc != *((u08_t*)pAddress + (overlayGroupSize-sizeof(u32_t))))
      {
         comrvNotificationHook(D_COMRV_CRC_CHECK_ERR, token.value);
      }
#endif /* D_COMRV_CRC */

#ifdef D_COMRV_FW_PROFILING
      /* update for FW profiling loaded the function */
      profilingIndication |= D_COMRV_PROFILING_LOAD_VAL;
#endif /* D_COMRV_FW_PROFILING */
   }
   else
   {
      overlayGroupSize <<= 9;
   }

   /* get actual function/data offset */
   offset = M_COMRV_GET_OFFSET(token);

   /* are we returning to an overlay function */
   if (isInvoke != D_COMRV_PROFILING_INVOKE_VAL)
   {
      /* calculate the function return offset; at this point pComrvStackFrame->calleeToken
         will hold the return address */
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
      /*  */
      if ()
      {
         temp = M_COMRV_GET_OFFSET(pOverlayMultiGroupTokensTable[pComrvStackFrame->calleeMultiGroupTableEntry]);
         overlayGroupSize = TODO: get the group size
         offset += ((u32_t)(pComrvStackFrame->calleeToken) - temp) & (--overlayGroupSize);
      }
      else
      {
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */
         offset += ((u32_t)(pComrvStackFrame->calleeToken) - offset) & (--overlayGroupSize);
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
      pComrvStackFrame->calleeMultiGroupTableEntry = selectedMultiGroupEntry;
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */
   }

#ifdef D_COMRV_FW_PROFILING
   comrvNotificationHook(profilingIndication , token.value);
#endif /* D_COMRV_FW_PROFILING */

   /* group is now loaded to memory so we can return the address of the data/function */
   return (void*)((u08_t*)pAddress + offset);
}

/**
* Get comrv heap eviction candidates according to a given size
*
* @param requestedEvictionSize - size requested for eviction; expressed in
*                                units of D_COMRV_OVL_GROUP_SIZE_MIN
*        pEvictCandidatesList  - output eviction candidate list of comrv heap indexes
* @return number of eviction candidates in the output list
*/
u08_t comrvGetEvictionCandidates(u08_t requestedEvictionSize, u08_t* pEvictCandidatesList)
{
   u08_t accumulatedSize = 0, index = 0;
   u08_t entryIndex, numberOfCandidates = 0;
   u32_t evictCandidateMap[D_COMRV_EVICT_CANDIDATE_MAP_SIZE], candidates;

   /* first lets clear the candidates list */
   memset(evictCandidateMap, 0, sizeof(u32_t)*D_COMRV_EVICT_CANDIDATE_MAP_SIZE);

#ifdef D_COMRV_EVICTION_LRU
   /* get the first lru entry */
   entryIndex = pComrvCB->lruIndex;
   /* loop LRU list until the eviction candidates accumulated size
      reaches the requested eviction size */
   do
   {
      /* verify the entry isn't locked */
      if (pComrvCB->overlayHeap[entryIndex].properties.fields.locked != D_COMRV_ENTRY_LOCKED)
      {
         /* count the number of candidates */
         numberOfCandidates++;
         /* accumulate size */
         accumulatedSize += pComrvCB->overlayHeap[entryIndex].properties.fields.size;
         /* set the eviction candidate in the eviction map */
         evictCandidateMap[entryIndex/D_COMRV_DWORD_IN_BITS] |= (1 << entryIndex);
      }
      /* move to the next LRU candidate */
      entryIndex = pComrvCB->overlayHeap[entryIndex].nextIndex;
   /* loop as long as we didn't get to the requested eviction size or we reached end of the list
      (means that all entries are locked) */
   } while (accumulatedSize < requestedEvictionSize && entryIndex != D_COMRV_LRU_LAST_ITEM);

#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */

   /* now we have eviction candidates bitmap of heap entries - lets create
      an output sorted list of these entries */
   for (entryIndex = 0 ; index != numberOfCandidates && entryIndex < D_COMRV_EVICT_CANDIDATE_MAP_SIZE ; entryIndex++)
   {
      /* get the candidates */
      candidates = evictCandidateMap[entryIndex];
      /* convert each candidate to an actual value in pEvictCandidatesList */
      while (candidates)
      {
         /* get the lsb that is set */
         pEvictCandidatesList[index] = candidates & (-candidates);
         /* subtract the lsb that is set */
         candidates -= pEvictCandidatesList[index]--;
         pEvictCandidatesList[index++] += entryIndex*D_COMRV_DWORD_IN_BITS;
      }
   }

   /* set the total size of eviction candidates in the last entry */
   pEvictCandidatesList[numberOfCandidates] = accumulatedSize;

   return numberOfCandidates;
}

/**
* search if a specific token is already loaded to the heap
*
* @param token - the token to search for
* @return if the token is loaded the return value is set to the loaded address
*         otherwise NULL
*/
static void* comrvSearchForLoadedOverlayGroup(comrvOverlayToken_t token)
{
   u08_t entryIndex;

   /* loop all entries */
   for (entryIndex = 0 ; entryIndex < pComrvCB->numOfOverlayEntries ; entryIndex++)
   {
      /* if token already loaded */
      if (pComrvCB->overlayHeap[entryIndex].token.fields.overlayGroupID == token.fields.overlayGroupID)
      {
         /* update that the entry was accessed */
         comrvUpdateHeapEntryAccess(entryIndex);
         /* return the actual function location within the loaded overlay group */
         return pComrvCB->overlayHeap[entryIndex].pAddress;
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
static void comrvUpdateHeapEntryAccess(u08_t entryIndex)
{
#ifdef D_COMRV_EVICTION_LRU
   /* there is no need to update if entryIndex is already MRU */
   if (entryIndex !=  pComrvCB->mruIndex)
   {
      /* entryIndex is not the lruIndex */
      if (entryIndex !=  pComrvCB->lruIndex)
      {
         /* update previous item's 'next index' */
         pComrvCB->overlayHeap[pComrvCB->overlayHeap[entryIndex].prevIndex].nextIndex = pComrvCB->overlayHeap[entryIndex].nextIndex;
      }
      else
      {
         /* update the global lru index */
         pComrvCB->lruIndex = pComrvCB->overlayHeap[entryIndex].nextIndex;
         /* update the lru item with the previous item index */
         pComrvCB->overlayHeap[pComrvCB->lruIndex].prevIndex = D_COMRV_LRU_FIRST_ITEM;
      }
      /* update entryIndex previous index */
      pComrvCB->overlayHeap[entryIndex].prevIndex = pComrvCB->mruIndex;
      /* update entryIndex next index - last item (MRU)*/
      pComrvCB->overlayHeap[entryIndex].nextIndex = D_COMRV_LRU_LAST_ITEM;
      /* update the old mru's next index */
      pComrvCB->overlayHeap[pComrvCB->mruIndex].nextIndex = entryIndex;
      /* update the new MRU */
      pComrvCB->mruIndex = entryIndex;
   }
#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */
}
