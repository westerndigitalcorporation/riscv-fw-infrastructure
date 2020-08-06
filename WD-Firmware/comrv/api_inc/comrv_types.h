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
* @file   comrv_types.h
* @author Ronen Haen
* @date   11.06.2019
* @brief  The defines COM-RV specific types
* 
*/
#ifndef  __COMRV_TYPES_H__
#define  __COMRV_TYPES_H__

/**
* include files
*/
#include "common_types.h"
#include "comrv_defines.h"

/**
* types
*/
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
   #ifdef D_COMRV_MIN_NUM_OF_MULTI_GROUPS
      typedef u08_t multigroupEntryIndex_t;
   #else
      typedef u16_t multigroupEntryIndex_t;
   #endif /* D_COMRV_MIN_NUM_OF_MULTI_GROUPS */
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */

typedef enum comrvLockState
{
   D_COMRV_GROUP_STATE_UNLOCK = 0,
   D_COMRV_GROUP_STATE_LOCK   = 1
}comrvLockState_t;

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
  /* alignment value to D_COMRV_OVL_GROUP_SIZE_MAX; granularity is expressed in
     number of D_COMRV_OVL_GROUP_SIZE_MIN. This value is used to help extracting the
     return offset from a return address */
  u08_t ucAlignmentToMaxGroupSize;
#if defined(D_COMRV_MULTI_GROUP_SUPPORT) && defined(D_COMRV_MIN_NUM_OF_MULTI_GROUPS)
  /* if the callee token is a multi group token, this field holds the
     actual loaded token entry in the multi group table */
  multigroupEntryIndex_t tCalleeMultiGroupTableEntry;
#elif defined(D_COMRV_MULTI_GROUP_SUPPORT)
  u08_t ucReserved[3];
  multigroupEntryIndex_t tCalleeMultiGroupTableEntry;
#else
  /* padding */
  u08_t ucReserved;
#endif /* D_COMRV_MULTI_GROUP_SUPPORT && D_COMRV_MIN_NUM_OF_MULTI_GROUPS */
} comrvStackFrame_t;

/* overlay token fields */
//TODO: check without bitfield
typedef struct comrvTokenFields
{
  /* overlay token indication 0: address; 1: overlay token */
  u32_t uiOverlayAddressToken:1;
  /* overlay group ID the function resides in */
  u32_t uiOverlayGroupID:16;
  /* data/function offset within the overlay group – 4 bytes granularity */
  u32_t uiOffset:10;
  /* caller thunk indication */
  u32_t uiThunk:1;
  /* reserved */
  u32_t uiReserved:1;
  /* specify the cache ID this overlay group belongs to */
  u32_t uiCacheID:2;
  /* multi group indication */
  u32_t uiMultiGroup:1;
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
  /* entry is locked by the end user - can't be evicted */
  u08_t ucEvictLock:1;
  /* entry is overlay data */
  u08_t ucData:1;
  /* loaded group size in D_COMRV_OVL_GROUP_SIZE_MIN granularity */
  u08_t ucSizeInMinGroupSizeUnits:4;
  /* entry lock - memory can't be moved until overlay is loaded & CRC'ed
     or entry is overlay data so can't be moved until user released it */
  u08_t ucEntryLock:1;
  u08_t ucReserved:1;
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
typedef struct comrvCacheEntry
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
#ifdef D_COMRV_MULTI_GROUP_SUPPORT
  u16_t             ucMultiGroupOffset;
#endif /* D_COMRV_MULTI_GROUP_SUPPORT */
  /* last index of cache entry */
  u08_t             ucLastCacheEntry;
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
   /* end user specified that multigroup and offset
      tables can be loaded when comrvInit() is called;
      Value of 0 means tables are not loaded in comrvInit()
      and it is the responsibility of the end user to
      directly call comrvLoadTables(); any non zero value
      specifies that tables are loaded in comrvInit() */
   u08_t ucCanLoadComrvTables;
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

/* comrv task stack registers value */
typedef struct comrvTaskStackRegsVal
{
   u32_t uiRegS0;
   u32_t uiRegS1;
   u32_t uiRegA0;
   u32_t uiRegA1;
   u32_t uiRegA2;
   u32_t uiRegA3;
   u32_t uiRegA4;
   u32_t uiRegA5;
   u32_t uiRegA6;
   u32_t uiRegA7;
   u32_t uiRegS2;
   u32_t uiRegS3;
   u32_t uiRegS4;
   u32_t uiRegS5;
   u32_t uiRegS6;
   u32_t uiRegS7;
   u32_t uiRegS8;
   u32_t uiRegS9;
   u32_t uiRegS10;
   u32_t uiRegS11;
   u32_t uiRegRa;
   u32_t uiRegT5;
}comrvTaskStackRegsVal_t;

/* comrv reset type */
typedef enum comrvResetType
{
   /* reset cache control block  - reset all loaded groups and eviction values*/
   E_RESET_TYPE_CACHE = 0,
   /* reset cache control block including the 'offset' and 'multi-group' tables
      if this option means the end user must reload these tables using
      comrvLoadTables */
   E_RESET_TYPE_ALL   = 1
}comrvResetType_t;

#endif /* __COMRV_TYPES_H__ */

