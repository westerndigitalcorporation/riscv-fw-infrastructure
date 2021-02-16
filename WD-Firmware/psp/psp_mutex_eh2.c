/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2020-2021 Western Digital Corporation or its affiliates.
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

/**
* definitions
*/
#define D_PSP_SIZE_OF_MUTEX_CB  sizeof(pspMutexCb_t)
/* Size of mutex CB is 64bits (== 8 bytes). In order to check that an address is aligned to 8 bytes, check that low 3 bits are 0 */
#define D_PSP_ADDRESS_ALIGN_TO_SIZEOF_MUTEX_CB   0x7
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


/* @brief - Verify the input mutex address is valid
*
* @parameter - mutex address
*
* @return - TRUE / FALSE
*/
D_PSP_TEXT_SECTION u32_t pspIsMutexAddressValid(u32_t* uiMutexAddress)
{
  u32_t uiValid = D_PSP_FALSE;

  /* uiMutexAddress is valid if it is inside the mutex Heap and is aligned to mutex size (8bytes) */
  if ((uiMutexAddress >= (u32_t*)g_uiAppMutexsHeapAddress) &&
      (uiMutexAddress < (u32_t*)(g_uiAppMutexsHeapAddress + g_uiAppNumberOfMutexs*D_PSP_SIZE_OF_MUTEX_CB)) &&
      (0 == (D_PSP_ADDRESS_ALIGN_TO_SIZEOF_MUTEX_CB & (u32_t)uiMutexAddress)))
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
D_PSP_TEXT_SECTION void pspMutexHeapInit(pspMutexCb_t* pMutexHeapAddress, u32_t uiNumOfmutexs)
{
  /* Assert if address is NULL or number of mutexs is 0 */
  M_PSP_ASSERT((NULL != pMutexHeapAddress) && (0 != uiNumOfmutexs)) ;

  /* Protect the mutex-heap initialization. Make sure it cannot be done simultaneously by multiple harts */
  pspInternalMutexLock(E_MUTEX_INTERNAL_FOR_MUTEX_HEAP_MNG);

  /* Initialize (== zero) the mutexs heap */
  pspMemsetBytes(pMutexHeapAddress, 0, uiNumOfmutexs * D_PSP_SIZE_OF_MUTEX_CB);

  /* Save the information - address of mutexs heap and number of mutexs */
  g_uiAppMutexsHeapAddress = (u32_t)pMutexHeapAddress;
  g_uiAppNumberOfMutexs    = uiNumOfmutexs;

  /* Remove the protection */
  pspInternalMutexUnlock(E_MUTEX_INTERNAL_FOR_MUTEX_HEAP_MNG);
}

/**
* @brief - Create a mutex in the mutexs-heap.
*          - Search for an available mutex in the heap
*          - Mark it as 'occupied'
*          - Mark the hart that created it
*
* @return - Address of the created mutex. In case of failure return NULL.
*/
D_PSP_TEXT_SECTION pspMutexCb_t* pspMutexCreate(void)
{
  u32_t             uiMutexIndex;                                     /* Index used for scanning */
  pspMutexCb_t*     pRetMutex = NULL;                                 /* Returned pointer to the created mutex */
  pspMutexCb_t*     pMutex = (pspMutexCb_t*)g_uiAppMutexsHeapAddress; /* Pointer to a mutex control block */
  u32_t             uiHartId = M_PSP_MACHINE_GET_HART_ID();

  /* Protect the creation of a mutex. Make sure the creation cannot be done simultaneously by multiple harts */
  pspInternalMutexLock(E_MUTEX_INTERNAL_FOR_MUTEX_HEAP_MNG);

  /* Search for an unoccupied mutex in the mutexs heap */
  for(uiMutexIndex = 0; uiMutexIndex < g_uiAppNumberOfMutexs; uiMutexIndex++)
  {
    if(D_PSP_MUTEX_UNOCCUIPED == pMutex->uiMutexOccupied)
    {
      /* Set the address of the found mutex, to be returned */
      pRetMutex = pMutex;
      /* Mark the mutex as 'occupied' */
      pMutex->uiMutexOccupied = D_PSP_MUTEX_OCCUIPED;
      /* Mark the hart that created the mutex */
      pMutex->uiMutexCreator = uiHartId;
      /* Make sure to mark the mutex as 'unlocked' */
      pMutex->uiMutexState = D_PSP_MUTEX_UNLOCKED;
      /* mutex is found, break out of the loop*/
      break;
    }
  }

  /* Remove the protection */
  pspInternalMutexUnlock(E_MUTEX_INTERNAL_FOR_MUTEX_HEAP_MNG);

  /* Return the address of the mutex. NULL == there's no available mutex in the mutexs-heap */
  return (pRetMutex);
}

/**
* @brief - Destroy (remove the 'occupied' mark) a mutex in the mutexs-heap
*
* @parameter - Address of the mutex to be destroyed
*
* @return    - In case of success - return NULL. In case of failure - return the given mutex address.
*/
D_PSP_TEXT_SECTION pspMutexCb_t* pspMutexDestroy(pspMutexCb_t* pMutex)
{
  pspMutexCb_t* pRetMutexAddr;  /* Pointer to a mutex. Used for return value */
  u32_t         uiHartId = M_PSP_MACHINE_GET_HART_ID();

  /* Verify mutex address validity*/
  M_PSP_ASSERT(D_PSP_TRUE == pspIsMutexAddressValid(pMutex));

  /* Protect the mutex destroy. Make sure the destroy cannot be done simultaneously by multiple harts */
  pspInternalMutexLock(E_MUTEX_INTERNAL_FOR_MUTEX_HEAP_MNG);

  /* Only the hart that created this mutex can also destroy it */
  if (uiHartId == pMutex->uiMutexCreator)
  {
    pMutex->uiMutexOccupied = D_PSP_MUTEX_UNOCCUIPED;  /* mark the mutex as 'unoccupied' */
    pMutex->uiMutexCreator  = 0;                       /* clean the 'creator' field */
    pMutex->uiMutexState    = D_PSP_MUTEX_UNLOCKED;    /* set the mutex state to 'free' */
    pRetMutexAddr           = NULL;                    /* returned NULL indicates success to destroy the mutex */
  }
  else
  {
    /* Failure. The hart that request to destroy the mutex is not the one that created it. Return the given mutex address to specify that it is still alive */
    pRetMutexAddr = pMutex;
  }

  /* Remove the protection */
  pspInternalMutexUnlock(E_MUTEX_INTERNAL_FOR_MUTEX_HEAP_MNG);

  return (pRetMutexAddr);
}

/**
* @brief - Lock a mutex using atomic commands.
*
* @parameter - pointer to the mutex to lock
*
*/
D_PSP_TEXT_SECTION void pspMutexAtomicLock(pspMutexCb_t* pMutex)
{
  /* Verify mutex address validity*/
  M_PSP_ASSERT(D_PSP_TRUE == pspIsMutexAddressValid(pMutex));

  /* Lock the mutex */
  M_PSP_ATOMIC_ENTER_CRITICAL_SECTION(&(pMutex->uiMutexState));
}

/**
* @brief - Free a mutex using atomic commands.
*
* @parameter - pointer to the mutex to free
*
*/
D_PSP_TEXT_SECTION void pspMutexAtomicUnlock(pspMutexCb_t* pMutex)
{
  /* Verify mutex address validity*/
  M_PSP_ASSERT(D_PSP_TRUE == pspIsMutexAddressValid(pMutex));

  /* Unlock the mutex */
  M_PSP_ATOMIC_EXIT_CRITICAL_SECTION(&(pMutex->uiMutexState));
}
