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
* @file   cti_api.h
* @author Ronen Haen
* @date   01.07.2020
* @brief  The file defines cti (comrv testing infrastructute) interface
*/
#ifndef __CTI_API_H
#define __CTI_API_H

/*
 * INCLUDES
 */

/*
 * DEFINITIONS
 */
/* before comrv engine enter critical section */
#define D_CTI_TASK_SYNC_NONE                        (0)
/* before comrv engine enter critical section */
#define D_CTI_TASK_SYNC_BEFORE_ENTER_CRITICAL_SEC   (1)
/* after comrv engine enter critical section */
#define D_CTI_TASK_SYNC_AFTER_ENTER_CRITICAL_SEC    (2)
/* before comrv engine exit critical section */
#define D_CTI_TASK_SYNC_BEFORE_EXIT_CRITICAL_SEC    (4)
/* after comrv engine exit critical section */
#define D_CTI_TASK_SYNC_AFTER_EXIT_CRITICAL_SEC     (8)
/* comrv engine after search and load flow */
#define D_CTI_TASK_SYNC_AFTER_SEARCH_LOAD           (16)

/*
 * MACROS
 */
#ifdef D_CTI_RTOS
#define M_CTI_TRIGGER_SW_INT()  (*((u32_t*)D_SW_INT_ADDRESS) = 1);
#define M_CTI_CLEAR_SW_INT()  (*((u32_t*)D_SW_INT_ADDRESS) = 0);
#define M_CTI_SYNC_POINT(x) \
   if ((x) == ctiGetCurrentSyncPoint()) \
   { \
      M_CTI_TRIGGER_SW_INT(); \
   }
#else
#define M_CTI_SYNC_POINT(x)
#endif /* D_CTI_RTOS */

#define M_CTI_MARK_DEFRAG() g_stCtiOvlFuncsHitCounter.uiDefragCounter++;

/*
 * TYPEDEFS
 */

/* hold functions hit counters */
typedef struct
{
   /* comrv load operation counter */
   u32_t uiComrvLoad;
   /* comrv de-fragmentation operation counter */
   u32_t uiDefragCounter;
   /* error number */
   u32_t uiErrorNum;
} stCtiFuncsHitCounter;

typedef enum
{
  E_CTI_OK = 0,
  E_CTI_ERROR
}E_CTI_RESULT;

/*
 * EXPORTED GLOBALS
 */
extern stCtiFuncsHitCounter g_stCtiOvlFuncsHitCounter;

/*
* ENUM
*/

/*
* FUNCTIONS PROTOTYPES
*/
void ctiMain();
void ctiTaskBTest(void);
u32_t ctiGetCurrentSyncPoint(void);
void ctiSetCurrentSyncPoint(u32_t uiNewTestSyncPointVal);

#endif /* __CTI_API_H */

