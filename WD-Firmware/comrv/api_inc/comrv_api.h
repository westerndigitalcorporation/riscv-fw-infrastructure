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
* @file   comrv_api.h
* @author Ronen Haen
* @date   11.06.2019
* @brief  The file defines the COM-RV interfaces
*/
#ifndef __COMRV_TASK_API_H__
#define __COMRV_TASK_API_H__

#ifndef __clang__
#error comrv can compile only with llvm
#endif // #ifndef __clang__

/**
* include files
*/
#include "comrv_config.h"

/**
* definitions
*/
// TODO: ronen - use psp defines
#define D_COMRV_INLINE     __attribute__((inline))
#define D_COMRV_NO_INLINE  __attribute__((noinline))
#define _OVERLAY_          __attribute__((overlaycall)) D_COMRV_NO_INLINE

#define D_COMRV_NUM_OF_CACHE_ENTRIES      (D_COMRV_OVL_CACHE_SIZE_IN_BYTES/D_COMRV_OVL_GROUP_SIZE_MIN)

/* eviction algorithm definition */
#if (D_COMRV_EVICTION_POLICY == 0)
   #define D_COMRV_EVICTION_LRU
#elif (D_COMRV_EVICTION_POLICY == 1)
   #define D_COMRV_EVICTION_LRU
#elif (D_COMRV_EVICTION_POLICY == 2)
   #define D_COMRV_EVICTION_LFU
#elif (D_COMRV_EVICTION_POLICY == 3)
   #define D_COMRV_EVICTION_MIX_LRU_LFU
#endif /* */

/* profile selection */
#if D_COMRV_NUM_OF_CACHE_ENTRIES < 0xFF
   #define D_COMRV_PROFILE 1
#elif D_COMRV_NUM_OF_CACHE_ENTRIES < 0xFFFF
   #define D_COMRV_PROFILE 2
#else
   #define D_COMRV_PROFILE 3
#endif /* D_COMRV_PROFILE==0 */

#if D_COMRV_PROFILE == 1

  #ifdef D_COMRV_EVICTION_LRU
    typedef u08_t lru_t;
    typedef u16_t lruIndexes_t;
  #elif defined(D_COMRV_EVICTION_LFU)
  #elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
  #endif /* D_COMRV_EVICTION_LRU */

#elif D_COMRV_PROFILE == 2

  #ifdef D_COMRV_EVICTION_LRU
    typedef u16_t lru_t;
    typedef u32_t lruIndexes_t;
  #elif defined(D_COMRV_EVICTION_LFU)
  #elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
  #endif /* D_COMRV_EVICTION_LRU */

#elif D_COMRV_PROFILE == 3

  #ifdef D_COMRV_EVICTION_LRU
    typedef u32_t lru_t;
    typedef u64_t lruIndexes_t;
  #elif defined(D_COMRV_EVICTION_LFU)
  #elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
  #endif /* D_COMRV_EVICTION_LRU */

#endif /* D_COMRV_PROFILE_1 */

/* comrv errors */
#define D_COMRV_INVALID_INIT_PARAMS_ERR   0
#define D_COMRV_NOT_ENOUGH_ENTRIES        1
#define D_COMRV_LOAD_ERR                  2
#define D_COMRV_OVL_DATA_DEFRAG_ERR       3
#define D_COMRV_CRC_CHECK_ERR             4

/* comrv defines */
#define D_COMRV_PROFILE_BASE_IND          0xFC
#define D_COMRV_PROFILING_INVOKE_BIT      1
#define D_COMRV_PROFILING_LOAD_BIT        2
/* Load and invoke an overlay function */
#define D_COMRV_LOAD_AND_INVOKE_IND       (D_COMRV_PROFILE_BASE_IND | D_COMRV_PROFILING_LOAD_BIT | D_COMRV_PROFILING_INVOKE_BIT)
/* invoke an overlay function already loaded */
#define D_COMRV_NO_LOAD_AND_INVOKE_IND    (D_COMRV_PROFILE_BASE_IND | D_COMRV_PROFILING_INVOKE_BIT)
/* load and return from an overlay function */
#define D_COMRV_LOAD_AND_RETURN_IND       (D_COMRV_PROFILE_BASE_IND | D_COMRV_PROFILING_LOAD_BIT)
/* return from an overlay function w/o loading */
#define D_COMRV_NO_LOAD_AND_RETURN_IND    (D_COMRV_PROFILE_BASE_IND)

/**
* macros
*/

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
  /* specify the cache ID this overlay group belongs to */
  u32_t cacheID:2;
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
  /* entry is ucData or locked by the end user - can't be evicted */
  u08_t ucLocked:1;
  /* entry is overlay data */
  u08_t ucData:1;
  /* loaded group size in D_COMRV_OVL_GROUP_SIZE_MIN granularity */
  u08_t ucSizeInMinGroupSizeUnits:4;
  u08_t reserved:2;
} comrvPropertiesFields_t;

/* cache entry */
typedef union comrvEntryProperties
{
  u08_t                   ucValue;
  comrvPropertiesFields_t stFields;
} comrvEntryProperties_t;

#ifdef D_COMRV_EVICTION_LRU
/* overlay cache entry */
typedef union comrvEvictionLru
{
   struct comrvEvictionLruFields_t
   {
      /* bidirectional linked list - index of previous LRU item */
      lru_t     typPrevLruIndex;
      /* bidirectional linked list - index of next LRU item */
      lru_t     typNextLruIndex;
   } stFields;
   /* value of both lru fields in a single value */
   lruIndexes_t typValue;
} comrvEvictionLru_u;
#elif defined(D_COMRV_EVICTION_LFU)
#elif defined(D_COMRV_EVICTION_MIX_LRU_LFU)
#endif /* D_COMRV_EVICTION_LRU */

/* overlay cache entry */
typedef struct comrvCacheEntrycomrvCacheEntry
{
#ifdef D_COMRV_EVICTION_LRU
   comrvEvictionLru_u    unLru;
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
  /* the cache entries */
  comrvCacheEntry_t stOverlayCache[D_COMRV_NUM_OF_CACHE_ENTRIES];
} comrvCB_t;

/* status structure */
typedef struct comrvStatus
{
   /* address of comrv stack */
   const comrvStackFrame_t *pComrvStack;
   /* address of comrv control block */
   const comrvCB_t         *pComrvCB;
} comrvStatus_t;

/* comrv init arguments */
typedef struct comrvInitArgs
{
   /* address the memory region which shall be used
      for loading and executing overlay groups */
   void* pCacheMemoeyAddress;
   /* size in bytes of the memory pointed by pCacheMemoeyAddress
      the size must be aligned to D_COMRV_OVL_GROUP_SIZE_MIN */
   u32_t uiCacheSizeInBytes;
}comrvInitArgs_t;

/* comrv load arguments */
typedef struct comrvLoadArgs
{
   /* offset in bytes of the group to be loaded */
   u32_t uiGroupOffset;
   /* size in bytes of the group to be loaded */
   u32_t uiSizeInBytes;
   /* destination address to copy to the loaded group */
   void* pDest;
}comrvLoadArgs_t;

/* comrv error arguments */
typedef struct comrvErrorArgs
{
   /* the token that this error refers to */
   u32_t uiToken;
   /* error number */
   u32_t uiErrorNum;
}comrvErrorArgs_t;

/* comrv instrumentation arguments */
typedef struct comrvInstrumentationArgs
{
   /* the token that this instrumentation refers to */
   u32_t uiToken;
   /* instrumentation number */
   u32_t uiInstNum;
}comrvInstrumentationArgs_t;

/**
* local prototypes
*/

/**
* external prototypes
*/

/**
* global variables
*/

/**
* APIs
*/
void                   comrvInit(comrvInitArgs_t* pInitParams);
void                   comrvGetStatus(comrvStatus_t* pComrvStatus);
D_COMRV_NO_INLINE void comrvInitApplicationStack(void);

#endif /* __COMRV_TASK_API_H__ */
