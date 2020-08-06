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
* @file   psp_mutex_eh2.c
* @author Nati Rapaport
* @date   24.06.2020
* @brief  The file supplies the psp mutex relevant APIs
*         Relevant for SweRV-EH2 (multi-harts and atomic commands supported)
*
*/ 

/**
* include files
*/
#include "psp_api.h"
#include "psp_internal_mutex_eh2.h"


/**
* types
*/
/* This structure is defined and used INTERNALLY (i.e. it is not exposed in the api) to calculate the overall size of the mutexs-heap.
 * In the mutexs-heap there are two consecutive parts - in the 1'st one there is a series of mutexs (32bit each).
 * In the 2'nd section there is a series of pspMutexMngmng structures. Each such structure indicates (1) whether the associated mutex is 'occupied'
 * (i.e. created by the application and is in use) or 'unoccupied' (i.e. destroyed by the application or haven't been created at all)
 * and (2) in case it was created, which hart created the mutex.
 * Note that the mutexs heap is not organized as a series of pspMutexCb_t structures. Instead, first there are the N dwords (32bits) for N mutexs (these are
 * the uiMutexState fields) and then there are N pspMutexMngmnt_t structures (8bits) as management information for N mutexs.
 * So, the overall size of the heap is N * sizeof(pspMutexCb_t)
 */
typedef struct pspMutexMngmng
{
   u08_t  ucMutexOccupied :1;  /* D_PSP_MUTEX_OCCUIPED / D_PSP_MUTEX_UNOCCUIPED */
   u08_t  ucMutexCreator  :1;  /* Created by: 0 - Hart0 / 1 - Hart1 .  Only the creator is allowed to destroy */
} pspMutexMngmnt_t;

/* */
typedef struct pspMutexControlBlock
{
   u32_t            uiMutexState;       /* D_PSP_MUTEX_LOCKED / D_PSP_MUTEX_UNLOCKED */
   pspMutexMngmnt_t stMutexMngmnt;
} pspMutexCb_t;

/**
* definitions
*/
#define D_PSP_SIZE_OF_MUTEX_CB  sizeof(pspMutexCb_t)

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
/* Address of the mutex-heap. Allocated by the application and supplied to PSP for initialization and handling */
u32_t g_uiAppMutexsHeapAddress;
/* Number of mutexs in the mutex-heap. This number is set by the application */
u32_t g_uiAppNumberOfMutexs;

/**
* APIs
*/

/**
* @brief - Initialize (zero) mutexs that PSP is using internally for multi-harts safe activities
*
*/
D_PSP_TEXT_SECTION void pspMutexInitPspMutexs(void)
{
  /* Call Internal-Mutexs module to do its initializations */
  pspInternalMutexInit();
}


/* @brief - Verify the input mutex address is valid. i.e. inside the range of the mutex-heap
*
* @parameter - mutex address
*
* @return - TRUE / FALSE
*/
D_PSP_TEXT_SECTION u32_t pspIsMutexAddressValid(u32_t* uiMutexAddress)
{
  u32_t uiValid = D_PSP_FALSE;

  /* uiMutexAddress is valid if it is inside the Mutex Heap but not in the Management part */
  if ((uiMutexAddress >= (u32_t*)g_uiAppMutexsHeapAddress) &&
      (uiMutexAddress < (u32_t*)(g_uiAppMutexsHeapAddress + g_uiAppNumberOfMutexs*D_PSP_SIZE_OF_MUTEX)))
  {
    uiValid = D_PSP_TRUE;
  }

  return (uiValid);
}

/**
* @brief - Initialize (zero) heap of mutexs.
*
* @parameter - Address of the mutexs heap
* @parameter - Number of mutexs in the heap
*
*/
D_PSP_TEXT_SECTION void pspMutexHeapInit(pspMutex_t* pMutexHeapAddress, u32_t uiNumOfmutexs)
{
  /* Assert if address is NULL or number of mutexs is 0 */
  M_PSP_ASSERT((NULL != pMutexHeapAddress) && (0 != uiNumOfmutexs)) ;

  /* Protect the mutex-heap initialization. Make sure it cannot be done simultaneously by multiple harts */
  M_PSP_INTERNAL_MUTEX_LOCK(E_MUTEX_INTERNAL_FOR_MUTEX_HEAP_MNG);

  /* Initialize (== zero) the mutexs heap */
  pspMemsetBytes(pMutexHeapAddress, 0, uiNumOfmutexs * D_PSP_SIZE_OF_MUTEX_CB);

  /* Save the information - address of mutexs heap and number of mutexs */
  g_uiAppMutexsHeapAddress = (u32_t)pMutexHeapAddress;
  g_uiAppNumberOfMutexs    = uiNumOfmutexs;

  /* Remove the protection */
  M_PSP_INTERNAL_MUTEX_UNLOCK(E_MUTEX_INTERNAL_FOR_MUTEX_HEAP_MNG);
}

/**
* @brief - Create a mutex in the mutexs-heap.
*          - Mark it as 'occupied'
*          - Mark the hart that created it
*
* @return - Address of the created mutex. In case of failure return NULL.
*/
D_PSP_TEXT_SECTION pspMutex_t* pspMutexCreate(void)
{
  u08_t             ucMutexIndex;               /* Index used for scanning */
  pspMutexMngmnt_t* pMutexMnmgmnt;              /* Pointer to a mutex management information structure */
  u32_t             uiMutexHeapMngmntAddress;   /* Start address of 'management' section in the mutexs heap */
  u32_t*            pMutexAddress = NULL;       /* Pointer to a mutex that found in the mutexs-heap */
  u32_t             uiHartId = M_PSP_GET_HART_ID();


  /* Protect the creation of a mutex. Make sure the creation cannot be done simultaneously by multiple harts */
  M_PSP_INTERNAL_MUTEX_LOCK(E_MUTEX_INTERNAL_FOR_MUTEX_HEAP_MNG);

  /* Calculate the address of the management section in the mutexs heap - It is after the part of all mutexs in the heap */
  uiMutexHeapMngmntAddress = (g_uiAppMutexsHeapAddress + g_uiAppNumberOfMutexs * D_PSP_SIZE_OF_MUTEX);

  /* Search for an unoccupied mutex in the management section of the mutexs heap */
  for(ucMutexIndex = 0; ucMutexIndex < g_uiAppNumberOfMutexs; ucMutexIndex++)
  {
    /* Get the management information of the indexed mutex */
    pMutexMnmgmnt = (pspMutexMngmnt_t*)(uiMutexHeapMngmntAddress + ucMutexIndex*sizeof(pspMutexMngmnt_t));

    /* If the mutex is unoccupied - then you can occupy it now */
    if (D_PSP_MUTEX_UNOCCUIPED == pMutexMnmgmnt->ucMutexOccupied)
    {
      /* Set the address of the found mutex in the mutexs-heap */
      pMutexAddress = (u32_t*)((g_uiAppMutexsHeapAddress + (ucMutexIndex * D_PSP_SIZE_OF_MUTEX)));
      /* Mark the mutex as 'occupied' */
      pMutexMnmgmnt->ucMutexOccupied = D_PSP_MUTEX_OCCUIPED;
      /* Mark the hart that created the mutex */
      pMutexMnmgmnt->ucMutexCreator = uiHartId;
      /* Make sure to mark the mutex as 'unlocked' */
      *pMutexAddress = D_PSP_MUTEX_UNLOCKED;
      /* mutex is found, break out of the loop*/
      break;
    }
  }

  /* Remove the protection */
  M_PSP_INTERNAL_MUTEX_UNLOCK(E_MUTEX_INTERNAL_FOR_MUTEX_HEAP_MNG);

  /* Return the address of the mutex. NULL == there's no available mutex in the mutexs-heap */
  return (pMutexAddress);
}

/**
* @brief - Destroy (remove the 'occupied' mark) a mutex in the mutexs-heap
*
* @parameter - Address of the mutex to be destroyed
*
* @return    - In case of success - return NULL. In case of failure - return the given mutex address.
*/
D_PSP_TEXT_SECTION pspMutex_t* pspMutexDestroy(pspMutex_t* pMutex)
{
  u08_t             ucMutexIndex;
  u32_t             uiMutexHeapMngmntAddress;   /* Start address of 'management' section in the mutexs heap */
  pspMutexMngmnt_t* pMutexMnmgmnt;              /* Pointer to a mutex management information structure */
  u32_t*            pRetMutexAddr = NULL;       /* Pointer to a mutex. Used for return value */
  u32_t             uiHartId = M_PSP_GET_HART_ID();

  /* Verify mutex address validity*/
  M_PSP_ASSERT(D_PSP_TRUE == pspIsMutexAddressValid(pMutex));

  /* Protect the mutex destroy. Make sure the destroy cannot be done simultaneously by multiple harts */
  M_PSP_INTERNAL_MUTEX_LOCK(E_MUTEX_INTERNAL_FOR_MUTEX_HEAP_MNG);

  /* Calculate the start address of the management section in the mutexs heap - It is after the part of all mutexs in the heap */
  uiMutexHeapMngmntAddress = (g_uiAppMutexsHeapAddress + g_uiAppNumberOfMutexs * D_PSP_SIZE_OF_MUTEX);

  ucMutexIndex = (((u32_t)pMutex - g_uiAppMutexsHeapAddress) / D_PSP_SIZE_OF_MUTEX);

  /* Point to the management information of the mutex to be destroyed */
  pMutexMnmgmnt = (pspMutexMngmnt_t*)(uiMutexHeapMngmntAddress + ucMutexIndex*sizeof(pspMutexMngmnt_t));

  /* Only the hart that created this mutex can also destroy it */
  if (uiHartId == pMutexMnmgmnt->ucMutexCreator)
  {
    pMutexMnmgmnt->ucMutexOccupied = D_PSP_MUTEX_UNOCCUIPED;  /* mark the mutex as 'unoccupied' */
    pMutexMnmgmnt->ucMutexCreator  = 0;                       /* clean the 'creator' field */
    *pMutex = D_PSP_MUTEX_UNLOCKED;                           /* For the sake of good order, set the mutex state to 'free' */
  }
  else
  {
    /* Failure. The hart that request to destroy the mutex is not the one that created it. Return the given mutex address to specify that it is still alive */
    pRetMutexAddr = pMutex;
  }

  /* Remove the protection */
  M_PSP_INTERNAL_MUTEX_UNLOCK(E_MUTEX_INTERNAL_FOR_MUTEX_HEAP_MNG);

  return (pRetMutexAddr);
}

/**
* @brief - Lock a mutex using atomic commands.
*
* @parameter - pointer to the mutex to lock
*
*/
D_PSP_TEXT_SECTION void pspMutexAtomicLock(pspMutex_t* pMutex)
{
  /* Verify mutex address validity*/
  M_PSP_ASSERT(D_PSP_TRUE == pspIsMutexAddressValid(pMutex));

  /* Lock the mutex */
  M_PSP_ATOMIC_ENTER_CRITICAL_SECTION(pMutex);
}

/**
* @brief - Free a mutex using atomic commands.
*
* @parameter - pointer to the mutex to free
*
*/
D_PSP_TEXT_SECTION void pspMutexAtomicUnlock(pspMutex_t* pMutex)
{
  /* Verify mutex address validity*/
  M_PSP_ASSERT(D_PSP_TRUE == pspIsMutexAddressValid(pMutex));

  /* Unlock the mutex */
  M_PSP_ATOMIC_EXIT_CRITICAL_SECTION(pMutex);
}
