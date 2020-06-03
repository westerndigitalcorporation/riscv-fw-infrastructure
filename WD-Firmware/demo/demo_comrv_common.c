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
* include files
*/
#include "common_types.h"
#include "psp_macros.h"
#include "comrv_api.h"
#ifdef D_COMRV_RTOS_SUPPORT
#include "rtosal_task_api.h"
#include "rtosal_time_api.h"
#include "rtosal_mutex_api.h"
#endif /* D_COMRV_RTOS_SUPPORT */

/**
* definitions
*/
#define M_DEMO_COMRV_RTOS_FENCE()   M_PSP_INST_FENCE(); \
                                    M_PSP_INST_FENCEI();

extern void* _OVERLAY_STORAGE_START_ADDRESS_;

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
extern u32_t xcrc32(const u08_t *pBuf, s32_t siLen, u32_t uiInit);

/**
* global variables
*/
#ifdef D_COMRV_FW_INSTRUMENTATION
comrvInstrumentationArgs_t g_stInstArgs;
#endif /* D_COMRV_FW_INSTRUMENTATION */

#ifdef D_COMRV_RTOS_SUPPORT
rtosalMutex_t stComrvMutex;
static u32_t uiPrevIntState;
#endif /* D_COMRV_RTOS_SUPPORT */

/**
* functions
*/

/**
* memory copy hook
*
* @param  none
*
* @return none
*/
void comrvMemcpyHook(void* pDest, void* pSrc, u32_t sizeInBytes)
{
   u32_t loopCount = sizeInBytes/(sizeof(u32_t)), i;
   /* copy dwords */
   for (i = 0; i < loopCount ; i++)
   {
      *((u32_t*)pDest + i) = *((u32_t*)pSrc + i);
   }
   loopCount = sizeInBytes - (loopCount*(sizeof(u32_t)));
   /* copy bytes */
   for (i = (i-1)*(sizeof(u32_t)) ; i < loopCount ; i++)
   {
      *((u08_t*)pDest + i) = *((u08_t*)pSrc + i);
   }
}

/**
* load overlay group hook
*
* @param pLoadArgs - refer to comrvLoadArgs_t for exact args
*
* @return loaded address or NULL if unable to load
*/
void* comrvLoadOvlayGroupHook(comrvLoadArgs_t* pLoadArgs)
{
   comrvMemcpyHook(pLoadArgs->pDest, (u08_t*)&_OVERLAY_STORAGE_START_ADDRESS_ + pLoadArgs->uiGroupOffset, pLoadArgs->uiSizeInBytes);
   /* it is upto the end user of comrv to synchronize the instruction and data stream after
      overlay data has been written to destination memory */
   M_DEMO_COMRV_RTOS_FENCE();
   return pLoadArgs->pDest;
}

/**
* notification hook
*
* @param  pInstArgs - pointer to instrumentation arguments
*
* @return none
*/
#ifdef D_COMRV_FW_INSTRUMENTATION
void comrvInstrumentationHook(const comrvInstrumentationArgs_t* pInstArgs)
{
   g_stInstArgs = *pInstArgs;
}
#endif /* D_COMRV_FW_INSTRUMENTATION */

/**
* error hook
*
* @param  pErrorArgs - pointer to error arguments
*
* @return none
*/
void comrvErrorHook(const comrvErrorArgs_t* pErrorArgs)
{
   comrvStatus_t stComrvStatus;
   comrvGetStatus(&stComrvStatus);
   /* we can't continue so loop forever */
   M_PSP_EBREAK();
}

/**
* crc calculation hook
*
* @param pAddress         - memory address to calculate
*        memSizeInBytes   - number of bytes to calculate
*        uiExpectedResult - expected crc result
*
* @return calculated CRC
*/
#ifdef D_COMRV_ENABLE_CRC_SUPPORT
u32_t comrvCrcCalcHook(const void* pAddress, u16_t usMemSizeInBytes, u32_t uiExpectedResult)
{
   volatile u32_t uiCrc;
   uiCrc = xcrc32(pAddress, usMemSizeInBytes, 0xffffffff);
   return !(uiExpectedResult == uiCrc);
}
#endif /* D_COMRV_ENABLE_CRC_SUPPORT */

/**
* Invalidate data cache hook
*
* @param pAddress         - memory address to invalidate
*        uiNumSizeInBytes - number of bytes to invalidate
*
* @return none
*/
void comrvInvalidateDataCacheHook(const void* pAddress, u32_t uiNumSizeInBytes)
{
   (void)pAddress;
   (void)uiNumSizeInBytes;
}

#ifdef D_COMRV_RTOS_SUPPORT
/**
* enter critical section
*
* @param None
*
* @return 0 - success, non-zero - failure
*/
u32_t comrvEnterCriticalSectionHook(void)
{
   if (rtosalGetSchedulerState() != D_RTOSAL_SCHEDULER_NOT_STARTED)
   {
      if (rtosalMutexWait(&stComrvMutex, D_RTOSAL_WAIT_FOREVER) != D_RTOSAL_SUCCESS)
      {
         return 1;
      }
   }
   else
   {
      pspInterruptsDisable(&uiPrevIntState);
   }

   return 0;
}

/**
* exit critical section
*
* @param None
*
* @return 0 - success, non-zero - failure
*/
u32_t comrvExitCriticalSectionHook(void)
{
   if (rtosalGetSchedulerState() != D_RTOSAL_SCHEDULER_NOT_STARTED)
   {
      if (rtosalMutexRelease(&stComrvMutex) != D_RTOSAL_SUCCESS)
      {
         return 1;
      }
   }
   else
   {
      pspInterruptsRestore(uiPrevIntState);
   }

   return 0;
}

/**
 * demoRtosalcalculateTimerPeriod - Calculates Timer period
 *
 */
void demoRtosalcalculateTimerPeriod(void)
{
   u32_t uiTimerPeriod = 0;

    #if (0 == D_CLOCK_RATE) || (0 == D_TICK_TIME_MS)
        #error "Core frequency values definitions are missing"
    #endif

   uiTimerPeriod = (D_CLOCK_RATE * D_TICK_TIME_MS / D_PSP_MSEC);
   /* Store calculated timerPeriod for future use */
   rtosalTimerSetPeriod(uiTimerPeriod);
}
#endif /* D_COMRV_RTOS_SUPPORT */

/******************** start temporary build issue workaround ****************/
void _kill(void)
{
}
void _sbrk(void)
{
}
void _getpid(void)
{
}
/******************** end temporary build issue workaround ****************/

