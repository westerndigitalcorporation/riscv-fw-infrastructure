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
* @file   psp_mutex_eh2.h
* @author Nati Rapaport
* @date   24.06.2020
* @brief  The file defines the psp mutex and relevant APIs
*         Relevant for SweRV-EH2 (multi-harts and atomic commands supported)
*         
*/
#ifndef __PSP_MUTEX_EH2_H__
#define __PSP_MUTEX_EH2_H__

/**
* include files
*/

/**
* definitions
*/
#define D_PSP_MUTEX_OCCUIPED    1 /* 'occupied' means- the mutex is used by the application (can 'lock' and 'free' it) */
#define D_PSP_MUTEX_UNOCCUIPED  0 /* 'unoccupied' means- the mutex is not used by the application (cannot 'lock' or 'free' it) */
#define D_PSP_MUTEX_LOCKED      1
#define D_PSP_MUTEX_UNLOCKED    0

/**
* types
*/
typedef struct pspMutexControlBlock
{
   u32_t  uiMutexState;        /* D_PSP_MUTEX_LOCKED / D_PSP_MUTEX_UNLOCKED */
   u32_t  uiMutexOccupied :1;  /* D_PSP_MUTEX_OCCUIPED / D_PSP_MUTEX_UNOCCUIPED */
   u32_t  uiMutexCreator  :1;  /* Created by: 0 - Hart0 / 1 - Hart1 .  Only the creator is allowed to destroy */
} pspMutexCb_t;

/**
* local prototypes
*/

/**
* external prototypes
*/

/**
* macros
*/

/**
* global variables
*/

/**
* APIs
*/

/**
* @brief - Initialize (zero) mutexs that PSP is using internally for multi-harts safe activities
*
*/
void pspMutexInitPspMutexs(void);

/**
* @brief - Initialize (zero) heap of mutexs.
* 
* @parameter - Address of the mutexs heap
* @parameter - Number of mutexs in the heap
*
*/
void pspMutexHeapInit(pspMutexCb_t* pMutexHeapAddress, u32_t uiNumOfMutexs);

/**
* @brief - Create a mutex in the mutexs-heap.
*          - Mark it as 'occupied'
*          - Mark the hart that created it
*
* @return - Address of the created mutex. In case of failure return NULL.
*/
pspMutexCb_t* pspMutexCreate(void);

/**
* @brief - Destroy (remove the 'occupied' mark) a mutex in the mutexs-heap
*
* @parameter - Address of the mutex to be destroyed
*
* @return    - Address of the destroyed mutex. In case of failure return NULL.
*/
pspMutexCb_t* pspMutexDestroy(pspMutexCb_t* pMutex);

/**
* @brief - Lock a mutex using atomic commands. 
* 
* @parameter - pointer to the mutex to lock
* 
*/
void pspMutexAtomicLock(pspMutexCb_t* pMutex);

/**
* @brief - Free a mutex using atomic commands. 
* 
* @parameter - pointer to the mutex to free
* 
*/
void pspMutexAtomicUnlock(pspMutexCb_t* pMutex);


#endif /* __PSP_MUTEX_EH2_H__ */
