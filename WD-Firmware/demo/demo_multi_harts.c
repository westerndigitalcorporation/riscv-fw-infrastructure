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
* @file   demo_multi_harts.c
* @author Nati Rapaport
* @date   03.06.2020
* @brief  Demo application for Multi_Harts (HW thread) functionality on EH2
*/

/**
* include files
*/
#include "psp_api.h"
#include "demo_platform_al.h"
#include "demo_utils.h"
#include "bsp_mem_map.h"

/**
* definitions
*/
#define D_DEMO_NUMBER_OF_HARTS                   2
#define D_DEMO_TIMER_PERIOD_MSEC_HART0           100
#define D_DEMO_TIMER_PERIOD_MSEC_HART1           75
#define D_DEMO_NUMBER_OF_TIMER_INTERRUPTS_HART0  20
#define D_DEMO_NUMBER_OF_TIMER_INTERRUPTS_HART1  30
#define D_DEMO_ARRAY_SIZE                        25
#define D_DEMO_NUM_ITERATIONS                    1000
#define D_DEMO_CONST_MULT                        10
#define D_DEMO_NUMBER_OF_MUTEXES_IN_HEAP         1

/* locations of pointers to DCCM area, used for various atomic operations, in a global array */
#define D_DEMO_SYNC_POINT_0_LOC               0
#define D_DEMO_SYNC_POINT_1_LOC               1
#define D_DEMO_SYNC_POINT_2_LOC               2
#define D_DEMO_SYNC_POINT_3_LOC               3
#define D_DEMO_COMMON_ACCUMULATED_NUMBER_LOC  4
#define D_DEMO_MUTEX_LOC                      5
#define D_DEMO_NUMBER_OF_COMMON_POINTS        D_DEMO_MUTEX_LOC+1  /* 6 common points */

/**
* macros
*/

/**
* types
*/

/**
* local prototypes
*/

/**
* external prototypes
*/

/**
* global variables
*/
/* Array that accessed by both harts in this demo */
u32_t g_uiCommonArray[D_DEMO_ARRAY_SIZE];
/* Global data for hart 0 manipulations */
u32_t g_uiVecA0[D_DEMO_ARRAY_SIZE];
u32_t g_uiVecB0[D_DEMO_ARRAY_SIZE];
/* Global data for hart 1 manipulations */
u32_t g_uiVecA1[D_DEMO_ARRAY_SIZE];
u32_t g_uiVecB1[D_DEMO_ARRAY_SIZE];

/* Counting how many Timer interrupts occurred on each hart */
volatile u32_t g_uiNumberOfTimerInterruptsHart0;
volatile u32_t g_uiNumberOfTimerInterruptsHart1;

/* Array of pointers to DCCM area for atomic operations usage in this demo */
u32_t* g_uiAtomicOperationVars[D_DEMO_NUMBER_OF_COMMON_POINTS];

/* Global pointer to a mutex, created and used in this demo */
pspMutex_t* g_pMutex;

/**
* APIs
*/

/**
 * @brief - Locate and initialize the global parameters in this demo that handled by atomic operations
 *          - locate them in DCCM (After area that assigned for PSP usage)
 *          - set them an initialized value (0)
 *
 * @parameter - start address in DCCM where demo can use for its own atomic operations
 */
void demoInitializeAtomicOperationsItems(u32_t uiStartAddressForAtomicOpsItems)
{
  /* Set a series of pointers in g_uiAtomicOperationVars. Each pointer points to an address in DCCM for atomic operations usage in this demo */
  g_uiAtomicOperationVars[D_DEMO_SYNC_POINT_0_LOC]              = (u32_t*)uiStartAddressForAtomicOpsItems;
  g_uiAtomicOperationVars[D_DEMO_SYNC_POINT_1_LOC]              = (u32_t*)(uiStartAddressForAtomicOpsItems+sizeof(u32_t));
  g_uiAtomicOperationVars[D_DEMO_SYNC_POINT_2_LOC]              = (u32_t*)(uiStartAddressForAtomicOpsItems+(sizeof(u32_t)*2));
  g_uiAtomicOperationVars[D_DEMO_SYNC_POINT_3_LOC]              = (u32_t*)(uiStartAddressForAtomicOpsItems+(sizeof(u32_t)*3));
  g_uiAtomicOperationVars[D_DEMO_COMMON_ACCUMULATED_NUMBER_LOC] = (u32_t*)(uiStartAddressForAtomicOpsItems+(sizeof(u32_t)*4));
  g_uiAtomicOperationVars[D_DEMO_MUTEX_LOC]                     = (u32_t*)(uiStartAddressForAtomicOpsItems+(sizeof(u32_t)*5));

  /* Initialize (zero) the common points in the DCCM area */
  pspMemsetBytes((void*)uiStartAddressForAtomicOpsItems, 0, D_DEMO_NUMBER_OF_COMMON_POINTS*sizeof(u32_t));

  /* Initialize the heap of the mutexs */
  pspMutexHeapInit(g_uiAtomicOperationVars[D_DEMO_MUTEX_LOC] ,D_DEMO_NUMBER_OF_MUTEXES_IN_HEAP);
}

/**
 * @brief - set initialized variables in data structures
 *
 * @input parameter - pointer to 1'st unsigned integers array
 * @input parameter - pointer to 2'nd unsigned integers array
 * @input parameter - array size
 */
void demoInitializeDataStructures(u32_t *pArray1, u32_t *pArray2, u32_t uiArraySize)
{
  u32_t uiIterator = 0;

  for (; uiIterator < uiArraySize; uiIterator++)
    {
      *pArray1 = uiIterator;
      pArray1++;
      *pArray2 = uiIterator*uiIterator;
      pArray2++;
    }
}

/**
 * @brief - Use the 2 given array values to calculate a total number
 *
 * @input parameter - pointer to 1'st unsigned integers array
 * @input parameter - pointer to 2'nd unsigned integers array
 * @input parameter - array size
 */
u32_t demoMultiplyArrays(u32_t *pArray1, u32_t *pArray2, u32_t uiArraySize)
{
  u32_t uiIterator = 0, uiSum = 0;

  for (; uiIterator < uiArraySize; uiIterator++)
  {
    uiSum += *pArray1 * *pArray2;
    pArray1++;
    pArray2++;
  }

  return uiSum;
}

/**
 * @brief - Each hart spins here on a global synchronization point, to make sure both harts
 *          start the next activity, following this function, simultaneously
 */
void demoSpinOnSyncPoint(volatile u32_t* pSyncPoint)
{
  volatile u32_t uiCommonParameter, uiCompareAndSetResult;

  /* Each hart spins here until the synchronization point value reach the expected value */
  do
  {
    uiCommonParameter = *pSyncPoint;
    /* Each hart increments the synchronization point number once */
    uiCompareAndSetResult = M_PSP_ATOMIC_COMPARE_AND_SET(pSyncPoint, uiCommonParameter, uiCommonParameter+1);
  } while (uiCompareAndSetResult != 0);
  /* Each hart spins here until synchronization point number = 2 ==> which means both harts arrived here */
  while(D_DEMO_NUMBER_OF_HARTS > *pSyncPoint){};
}

/**
 * @brief - Timer ISR run on Hart0
 */
void demoTimerIsrHart0()
{
  /* Disable Machine-Timer interrupt */
  pspMachineInterruptsDisableIntNumber(D_PSP_INTERRUPTS_MACHINE_TIMER);

  g_uiNumberOfTimerInterruptsHart0++;
  demoOutputMsg("HART0 Timer ISR # %d: \n",g_uiNumberOfTimerInterruptsHart0);

  if(D_DEMO_NUMBER_OF_TIMER_INTERRUPTS_HART0 > g_uiNumberOfTimerInterruptsHart0)
  {
    /* Setup Core's timer for another turn */
    pspMachineTimerCounterSetupAndRun(D_PSP_MACHINE_TIMER, M_DEMO_MSEC_TO_CYCLES(D_DEMO_TIMER_PERIOD_MSEC_HART0));

    /* Enable timer interrupt */
    pspMachineInterruptsEnableIntNumber(D_PSP_INTERRUPTS_MACHINE_TIMER);
  }
}

/**
 * @brief - Timer ISR run on Hart1
 */
void demoTimerIsrHart1()
{
  /* Disable Machine-Timer interrupt */
  pspMachineInterruptsDisableIntNumber(D_PSP_INTERRUPTS_MACHINE_TIMER);

  g_uiNumberOfTimerInterruptsHart1++;
  demoOutputMsg("HART1 Timer ISR # %d: \n",g_uiNumberOfTimerInterruptsHart1);

  if(D_DEMO_NUMBER_OF_TIMER_INTERRUPTS_HART1 > g_uiNumberOfTimerInterruptsHart1)
  {
    /* Setup Core's timer for another turn */
    pspMachineTimerCounterSetupAndRun(D_PSP_MACHINE_TIMER, M_DEMO_MSEC_TO_CYCLES(D_DEMO_TIMER_PERIOD_MSEC_HART1));

    /* Enable timer interrupt */
    pspMachineInterruptsEnableIntNumber(D_PSP_INTERRUPTS_MACHINE_TIMER);
  }
}


/**
 * @brief - Demo application # 1
 *          Data structures manipulation per Hart
 *          Each Hart does the same calculations on its own memory area
 */
void demoMultiHartsAccessSeperateMemory(void)
{
  u32_t uiHartId = M_PSP_MACHINE_GET_HART_ID();
  u32_t uiRes0 = 0, uiRes1 = 0, uiCalculationResult = 0;

  if (E_HART0 == uiHartId)
  {
    pspMachineInterruptsSetVecTableAddress(&M_PSP_VECT_TABLE_HART0);

    demoInitializeDataStructures(g_uiVecA0, g_uiVecB0, D_DEMO_ARRAY_SIZE);
    uiRes0 = demoMultiplyArrays(g_uiVecA0, g_uiVecB0, D_DEMO_ARRAY_SIZE);
  }
  else /* Current Hart is 1 */
  {
    pspMachineInterruptsSetVecTableAddress(&M_PSP_VECT_TABLE_HART1);

    demoInitializeDataStructures(g_uiVecA1, g_uiVecB1, D_DEMO_ARRAY_SIZE);
    uiRes1 = demoMultiplyArrays(g_uiVecA1, g_uiVecB1, D_DEMO_ARRAY_SIZE);
  }
  uiCalculationResult = uiRes0 + uiRes1;
  demoOutputMsg("Current HART number is: %d \n",uiHartId);
  demoOutputMsg("Calculated sum of Hart0 = %d \n",uiRes0);
  demoOutputMsg("Calculated sum of Hart1 = %d \n",uiRes1);
  demoOutputMsg("Total sum on both HARTs = %d \n",uiCalculationResult);

}

/**
 * @brief - Demo application # 2
 *          Data structures manipulation per Hart
 *          The 2 harts increment a global counter in a shared memory area,
 *          using CAS (Compare-and-Set) function with lr/sc atomic commands
 */
void demoMultiHartsCasInSharedMemory()
{
  u32_t volatile uiCommonParameter;
  u32_t volatile uiCompareAndSetResult;
  u32_t uiIterator;

  /* Synchronization point between 2 harts, to make sure both harts start this part simultaneously */
  demoSpinOnSyncPoint(g_uiAtomicOperationVars[D_DEMO_SYNC_POINT_0_LOC]);

  /* Each hart iterates and accumulate a common global number */
  for (uiIterator = 0; uiIterator < D_DEMO_NUM_ITERATIONS; uiIterator++)
  {
    do
    {
      uiCommonParameter = *(g_uiAtomicOperationVars[D_DEMO_COMMON_ACCUMULATED_NUMBER_LOC]);
      uiCompareAndSetResult = M_PSP_ATOMIC_COMPARE_AND_SET(g_uiAtomicOperationVars[D_DEMO_COMMON_ACCUMULATED_NUMBER_LOC], uiCommonParameter, uiCommonParameter+1);
    } while (uiCompareAndSetResult != 0);
  }

  /* Synchronization point between 2 harts, Check the results after both harts finished their iterations */
  demoSpinOnSyncPoint(g_uiAtomicOperationVars[D_DEMO_SYNC_POINT_1_LOC]);

  /* Upon completion of Part2 - check if succeeded */
  /* The global accumulated number should be exactly twice than the loop iterator, as both harts ran the same
   * loop and accumulated the global accumulated counter */
  if((uiIterator*2) != *(g_uiAtomicOperationVars[D_DEMO_COMMON_ACCUMULATED_NUMBER_LOC]))
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }
}

/**
 * @brief - Demo application # 3
 *          Data structures manipulation per Hart
 *          The 2 harts access critical-section shared area, using AMO (Atomic Memory Operation) swap
 *          command to implement mutex lock/free. Each hart puts in the critical-section its own data
 *          then verify they didn't overlapped each other
 */
void demoMultiHartsCriticalSectionAmoMutex()
{
  u32_t       uiHartId = M_PSP_MACHINE_GET_HART_ID();
  u32_t       uiIterator = 0;
  pspMutex_t* pMutexDestroyed; /* Mutex pointer. Used here to verify correct destroy */

  if(E_HART0 == uiHartId)
  {
    /* Initialize (zero) 3 global arrays, used in this demo function. Do it by hart0. No need to do it twice by two harts */
    pspMemsetBytes(g_uiCommonArray, 0, D_DEMO_ARRAY_SIZE);
    pspMemsetBytes(g_uiVecA0, 0, D_DEMO_ARRAY_SIZE);
    pspMemsetBytes(g_uiVecA1, 0, D_DEMO_ARRAY_SIZE);
    /* Request to create a mutex, used in this demo function. Do it only once , so let hart0 to do it */
    g_pMutex = pspMutexCreate();
  }

  /* Synchronization point between 2 harts. Make sure both harts starts the demo-function only after initializations are done and the mutex is created */
  demoSpinOnSyncPoint(g_uiAtomicOperationVars[D_DEMO_SYNC_POINT_2_LOC]);

  /* Spin-lock on critical section entrance */
  pspMutexAtomicLock(g_pMutex);

  /* Set values (hart# specific) in the shared memory array */
  for(uiIterator=0; uiIterator < D_DEMO_ARRAY_SIZE; uiIterator++)
  {
    g_uiCommonArray[uiIterator] = uiIterator + (D_DEMO_CONST_MULT * uiHartId);
  }

  /* In order to verify correct behavior, each hart copy the values from the shared memory to a hart-specific area */
  for(uiIterator=0; uiIterator < D_DEMO_ARRAY_SIZE; uiIterator++)
  {
    if (E_HART0 == uiHartId)
    {
      g_uiVecA0[uiIterator] = g_uiCommonArray[uiIterator];
    }
    else /* hart1 */
    {
      g_uiVecA1[uiIterator] = g_uiCommonArray[uiIterator];
    }
  }

  /* Unlock the critical section */
  pspMutexAtomicUnlock(g_pMutex);

  /* Synchronization point between 2 harts, to make sure that the results check is done after both harts completed the previous part */
  demoSpinOnSyncPoint(g_uiAtomicOperationVars[D_DEMO_SYNC_POINT_3_LOC]);

  /* Verify correct values. No need to do it twice, so let hart0 to do it */
  if (E_HART0 == uiHartId)
  {
    for(uiIterator=0; uiIterator < D_DEMO_ARRAY_SIZE; uiIterator++)
    {
      if ((g_uiVecA0[uiIterator] != uiIterator) ||                     /* Check expected values of Hart0 */
          (g_uiVecA1[uiIterator] != (uiIterator + D_DEMO_CONST_MULT))) /* Check expected values of Hart1 */
      {
          /* Values are not as expected - critical section lock action was failed */
          M_DEMO_ERR_PRINT();
          M_PSP_EBREAK();
      }
    } /* for loop */

    /* No need for the mutex anymore - can be destroyed */
    pMutexDestroyed = pspMutexDestroy(g_pMutex);
    /* NULL returned value indicates that mutex-destroy has been succeeded */
    if (pMutexDestroyed != NULL)
    {
      /* Mutex destroy has been failed */
      M_DEMO_ERR_PRINT();
      M_PSP_EBREAK();
    }
  } /* End of expected values verification */
}

/**
 * @brief - Demo application # 4
 *          Each hart register its own timer ISR and enable Timer interrupt.
 *          Then each hart verify that its own ISR raised as expected.
 *
 */
void demoMultiHartsTimerInterrupts()
{
  u32_t uiHartId = M_PSP_MACHINE_GET_HART_ID();

  /* Disable Timer interrupt */
  pspMachineInterruptsDisableIntNumber(D_PSP_INTERRUPTS_MACHINE_TIMER);

  switch (uiHartId)
  {
    case E_HART0:
      g_uiNumberOfTimerInterruptsHart0 = 0;
      /* Register timer ISR for Hart0 */
      pspMachineInterruptsRegisterIsr(demoTimerIsrHart0, E_MACHINE_TIMER_CAUSE);
      break;
    case E_HART1:
      g_uiNumberOfTimerInterruptsHart1 = 0;
      /* Register timer ISR for Hart1 */
      pspMachineInterruptsRegisterIsr(demoTimerIsrHart1, E_MACHINE_TIMER_CAUSE);
      break;
    default:
      break;
  }

  /* Initialize the timer and let it run */
  pspMachineTimerCounterSetupAndRun(D_PSP_MACHINE_TIMER, M_DEMO_MSEC_TO_CYCLES(D_DEMO_TIMER_PERIOD_MSEC_HART0));
  /* Enable Timer interrupt */
  pspMachineInterruptsEnableIntNumber(D_PSP_INTERRUPTS_MACHINE_TIMER);
  /* Enable all interrupts in mstatus CSR */
  pspMachineInterruptsEnable();

  while(1)
  {
    /* Loop until both HARTs complete their Timer interrupts */
    if (E_HART0 == uiHartId)
    {
      if (D_DEMO_NUMBER_OF_TIMER_INTERRUPTS_HART0 == g_uiNumberOfTimerInterruptsHart0)
      {
        //M_DEMO_END_PRINT();
        break;
      }
    }
    else
    {
      if (D_DEMO_NUMBER_OF_TIMER_INTERRUPTS_HART1 == g_uiNumberOfTimerInterruptsHart1)
      {
        //M_DEMO_END_PRINT();
        break;
      }
    } /* uiHartId = 0/1 */
  } /* while(1) */
}

/**
 * @brief - demoStart - startup point of the demo application. called from main function.
 *
 */
void demoStart(void)
{
  u32_t uiAddressForAtomics;
  static u32_t uiSync = 0;

  M_DEMO_START_PRINT();

  /* Globals initializations could be done once */
  if (0 == uiSync)
  {
    uiSync = 1;

    /* Initialize PSP mutexs */
    pspMutexInitPspMutexs();

    /* Get the start address that the demo can use for its own atomic operations */
    uiAddressForAtomics = pspAtomicsGetAddressForAtomicOperations();

    /* Initialize the parameters for atomic operations in the demo */
    demoInitializeAtomicOperationsItems(uiAddressForAtomics);

    /* Setup vector table for Hart0 */
    pspMachineInterruptsSetVecTableAddress(&M_PSP_VECT_TABLE_HART0);
    /* Setup vector table for Hart1 */
    pspMachineInterruptsSetVecTableAddress(&M_PSP_VECT_TABLE_HART1);
  }

  /* Demo#1 - Each hart access its own memory */
  demoOutputMsg("---- Part 1 : Each hart access its own area ----\n");
  demoMultiHartsAccessSeperateMemory();

   /* Demo#2 - Both harts access shared memory, using CAS lr\sc commands */
   demoOutputMsg("---- Part 2 : 2 harts accumulate global counter ----\n");
   demoMultiHartsCasInSharedMemory();

  /* Demo#3 - Both harts access shared memory using mutex. Lock is done with AMO swap command */
  demoOutputMsg("---- Part 3 : 2 harts access critical section using mutex lock ----\n");
  demoMultiHartsCriticalSectionAmoMutex();

  /* Demo#4 - Timer ISRs run on each hart separately*/
  demoOutputMsg("---- Part 4 : Timer interrupts over 2 harts ----\n");
  demoMultiHartsTimerInterrupts();

  M_DEMO_END_PRINT();
}


