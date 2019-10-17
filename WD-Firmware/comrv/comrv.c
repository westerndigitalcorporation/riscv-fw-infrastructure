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
* @brief  The file implements the COM-RV interfaces
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
#define D_COMRV_NUM_OF_OVERLAY_ENTRIES   (((u08_t*)&__OVERLAY_SEC_END__ - (u08_t*)&__OVERLAY_SEC_START__)/D_COMRV_OVL_GROUP_SIZE_MIN)
#define D_COMRV_END_OF_STACK             0xDEAD
#define D_COMRV_LRU_LAST_ITEM            0xFF
#define D_COMRV_LRU_FIRST_ITEM           0xFF
#define D_COMRV_EVICT_CANDIDATE_MAP_SIZE 4
#define D_COMRV_DWORD_IN_BITS            32
#define D_COMRV_ENTRY_LOCKED             1
/**
* macros
*/
#define M_COMRV_READ_TOKEN_REG(x)       asm volatile ("mv %0, t5" : "=r" (x)  : );
#define M_COMRV_SET_ENTRY_ADDR(address) asm volatile ("la t6, "#address : : : );
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

#define M_COMRV_GET_OVL_GROUP_SIZE(tokenRegister) (D_COMRV_OVL_GROUP_SIZE_MIN * \
                                                   (TBD_GroupArray[tokenRegister.token.overlayGroupID+1] - \
                                                    TBD_GroupArray[tokenRegister.token.overlayGroupID]));
/**
* types
*/
typedef struct comrvStackFrame
{
  /* holds return address (caller is non overlay function) or return offset (caller is overlay function) */
  u32_t callerReturnAddressOffset;
  /* holds callee address (callee is non overlay function) or callee token (callee is overlay function) */
  u32_t calleeToken;
  /* holds the offset in bytes to the previous stack frame */
  s16_t offsetPrevFrame;
  /* size of the loaded overlay group - 1; used to avoid rereading this value from the overlay offset table */
  u16_t overlayGroupSize;
} comrvStackFrame_t;

#if 1
/* overlay token */
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
} comrvToken_t;

#else

typedef struct comrvToken
{
  u32_t addressToken;
} comrvToken_t;
#endif

/* overlay token */
typedef union comrvOverlayTokenRegister
{
  u32_t        value;
  comrvToken_t token;
} comrvOverlayTokenRegister_t;

/* overlay token */
typedef struct comrvEntryProperties
{
  u08_t merged:1;
  u08_t locked:1;
  u08_t size:3;
  u08_t reserved:2;
} comrvEntryProperties_t;

/* overlay token entry */
typedef struct comrvOverlayHeapEntry
{
#ifdef D_COMRV_EVICTION_LRU
  u08_t prevIndex;
  u08_t nextIndex;
#elif defined(D_COMRV_EVICTION_LFU)
#endif /* D_COMRV_EVICTION_LRU */
  comrvEntryProperties_t      properties;
  comrvOverlayTokenRegister_t tokenRegister;
  void*                       actualAddress;
} comrvOverlayHeapEntry_t;

/* com-rv control block */
typedef struct comrvCB
{
#ifdef D_COMRV_EVICTION_LRU
  u08_t                    lruIndex;
  u08_t                    mruIndex;
#elif defined(D_COMRV_EVICTION_LFU)
#endif /* D_COMRV_EVICTION_LRU */
  u08_t                    numOfOverlayEntries;
  comrvOverlayHeapEntry_t* overlayHeap;
} comrvCB_t;

/**
* local prototypes
*/
static void* comrvSearchForLoadedToken(comrvToken_t token);
static void  comrvUpdateHeapEntryAccess(u08_t entryIndex);
static u08_t comrvGetCandidatesForEviction(u16_t requestedEvictionSize, u32_t* pEvictCandidatesMap);

/**
* external prototypes
*/
extern void comrv_entry(void);

/**
* global variables
*/
comrvStackFrame_t* pStackStartAddr = (comrvStackFrame_t*)&__OVERLAY_STACK_START__;
comrvCB_t*         pComrvCB;
u16_t TBD_GroupArray[5];
comrvOverlayTokenRegister_t TBD_MultiGroupArray[5];


/**
* COM-RV initialization function
*
* @param  none
*
* @return none
*/
void comrvInit(void)
{
   u08_t i;
   comrvStackFrame_t* pStackEndAddr = (comrvStackFrame_t*)&__OVERLAY_STACK_END__ - 1;

   /* determine the location of comrv internal control block */
   pComrvCB = (comrvCB_t*)((&__OVERLAY_SEC_END__) - sizeof(comrvCB_t));
   /* determine the number of heap entries */
   pComrvCB->numOfOverlayEntries = D_COMRV_NUM_OF_OVERLAY_ENTRIES;
   /* determine the location of comrv overlay heap structure */
   pComrvCB->overlayHeap = (comrvOverlayHeapEntry_t*)((u08_t*)pComrvCB - (pComrvCB->numOfOverlayEntries*sizeof(comrvOverlayHeapEntry_t)));
#ifdef D_COMRV_EVICTION_LRU
   /* initialize all heap entries */
   for (i = 0 ; i < pComrvCB->numOfOverlayEntries ; i++)
   {
      /* initially each entry points to the previous and next neighbor cells */
      pComrvCB->overlayHeap[i].prevIndex = i-1;
      pComrvCB->overlayHeap[i].nextIndex = i+1;
   }
   /* mark the last entry in the LRU list */
   pComrvCB->overlayHeap[i-1].nextIndex = D_COMRV_LRU_LAST_ITEM;
   /* set the index of the list LRU and MRU */
   pComrvCB->lruIndex = 0;
   pComrvCB->mruIndex = i-1;
#elif defined(D_COMRV_EVICTION_LFU)
#endif /* D_COMRV_EVICTION_LRU */
   /* Init comrv stack - mark the last stack frame */
   pStackStartAddr->offsetPrevFrame = D_COMRV_END_OF_STACK;
   /* initialize all stack entries */
   for (pStackStartAddr++ ; pStackStartAddr < pStackEndAddr ; pStackStartAddr++)
   {
      pStackStartAddr->offsetPrevFrame = (s16_t)-sizeof(comrvStackFrame_t);
      pStackStartAddr->calleeToken = 0;
   }

#ifndef __OS__
   /* set the address of COMRV stack and initialize it */
   pStackStartAddr--;
   M_COMRV_SET_STACK_ADDR(pStackStartAddr);
   pStackStartAddr->offsetPrevFrame = D_COMRV_END_OF_STACK;
#endif /* __OS__ */

   /* set the address of COMRV stack pool */
   pStackStartAddr--;
   M_COMRV_SET_POOL_ADDR(pStackStartAddr);

   /* set the address of COMRV entry point */
   M_COMRV_SET_ENTRY_ADDR(comrv_entry);
}

/**
* Search if current overlay token is already loaded
*
* @param  none
*
* @return void* - address of the overlay function/data or NULL if not loaded
*/
void* comrvGetAddressFromToken(u16_t* pOverlayGroupSize)
{
   u08_t                       entry, numOfEvictionCandidates;
   u16_t                       groupSize;
   u32_t                       evictCandidateMap[D_COMRV_EVICT_CANDIDATE_MAP_SIZE];
   void*                       pAddress;
   comrvOverlayTokenRegister_t tokenRegister;

   /* read the requested token value */
   M_COMRV_READ_TOKEN_REG(tokenRegister.value);

#ifdef D_COMRV_MULTI_GROUP_SUPPORT
   /* if the requested token isn't a multi-group token */
   if (tokenRegister.token.multiGroup == 0)
   {
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */
      /* search for token */
      pAddress = comrvSearchForLoadedToken(tokenRegister.token);
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
   }
   /* search for a multi-group overlay token */
   else
   {
      pAddress = NULL;
      /* first entry to search from in the multi group table is determined by the overlayGroupID
         field of the requested token */
      entry = tokenRegister.token.overlayGroupID;
      do
      {
         /* search for the token */
         pAddress = comrvSearchForLoadedToken(TBD_MultiGroupArray[entry].token);
      /* continue the search as long as the group wasn't found and we have additional tokens */
      } while ((pAddress == NULL) && (TBD_MultiGroupArray[++entry].value != 0));
   }
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */

   /* if the data/function is already loaded we can return its address */
   if (pAddress != NULL)
   {
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
      return (void*)((u32_t*)pAddress + TBD_MultiGroupArray[--entry].token.offset);
#else
      return (void*)((u32_t*)pAddress + tokenRegister.token.offset);
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */
   }

#ifdef D_COMRV_MULTI_GROUP_SUPPORT
   /* if the requested token is a multi-group token */
   if (tokenRegister.token.multiGroup != 0)
   {
      /* for now we take the first token in the list of tokens */
      tokenRegister = TBD_MultiGroupArray[tokenRegister.token.overlayGroupID];
   }
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */

   /* get the group size */
   groupSize = M_COMRV_GET_OVL_GROUP_SIZE(tokenRegister);

   /* get eviction candidates according to the requested groupSize */
   numOfEvictionCandidates = comrvGetCandidatesForEviction(groupSize, evictCandidateMap);

   /* if no heap entry available */
   if (numOfEvictionCandidates == 0)
   {
      /* this means the end used locked all entries */
      comrvErrorInddicationHook(D_COMRV_ERR_NO_AVAILABLE_ENTRY);
   }

   *pOverlayGroupSize = 512-1;
   pComrvCB->overlayHeap[0].tokenRegister.value = tokenRegister.value;
   pComrvCB->overlayHeap[0].actualAddress = (void*)(tokenRegister.value ^ 1);
   return pComrvCB->overlayHeap[0].actualAddress;
}

u08_t comrvGetCandidatesForEviction(u16_t requestedEvictionSize, u32_t* pEvictCandidatesMap)
{
   u08_t entryIndex, numberOfCandidates = 0;
   u16_t accumulatedSize = 0;

   /* first lets clear the candidates list */
   memset(pEvictCandidatesMap, 0, sizeof(u32_t)*D_COMRV_EVICT_CANDIDATE_MAP_SIZE);

#ifdef D_COMRV_EVICTION_LRU
   /* get the first lru entry */
   entryIndex = pComrvCB->lruIndex;
   /* loop LRU list until the eviction candidates accumulated size
      reaches the requested eviction size */
   do
   {
      /* verify the entry isn't locked */
      if (pComrvCB->overlayHeap[entryIndex].properties.locked != D_COMRV_ENTRY_LOCKED)
      {
         /* count the number of candidates */
         numberOfCandidates++;
         /* accumulate size */
         accumulatedSize += M_COMRV_GET_OVL_GROUP_SIZE(pComrvCB->overlayHeap[entryIndex].tokenRegister);
         /* set the eviction candidate in the eviction map */
         pEvictCandidatesMap[entryIndex/D_COMRV_DWORD_IN_BITS] |= (entryIndex & (D_COMRV_DWORD_IN_BITS-1));
      }
      /* move to the next LRU candidate */
      entryIndex = pComrvCB->overlayHeap[entryIndex].nextIndex;
   /* loop as long as we didn't get to the requested eviction size or we reached end of the list
      (means that all entries are locked) */
   } while (accumulatedSize < requestedEvictionSize && entryIndex != D_COMRV_LRU_LAST_ITEM);

#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */
   return numberOfCandidates;
}

static void* comrvSearchForLoadedToken(comrvToken_t token)
{
   u08_t entryIndex;

   /* loop all entries */
   for (entryIndex = 0 ; entryIndex < pComrvCB->numOfOverlayEntries ; entryIndex++)
   {
      /* if token already loaded */
      if (pComrvCB->overlayHeap[entryIndex].tokenRegister.token.overlayGroupID == token.overlayGroupID)
      {
         /* update that the entry was accessed */
         comrvUpdateHeapEntryAccess(entryIndex);
         /* return the actual function location (function offset expressed in 4 bytes granularity) */
         return (void*)((u08_t*)pComrvCB->overlayHeap[entryIndex].actualAddress + (token.offset << 2));
      }
   }
   /* overlay group not loaded */
   return NULL;
}

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
#else

#endif /* D_COMRV_EVICTION_LRU */
}

void comrvErrorInddicationHook(u32_t errorNum)
{
}

void comrvMemcpyHook(void* pDest, void* pSrc, u32_t sizeInBytes)
{
   memcpy(pDest, pSrc, sizeInBytes);
}

void comrvLoadHook(u32_t groupOffset, void* pDest, u32_t sizeInBytes)
{
}

void comrvNotificationHook(void)
{
}
