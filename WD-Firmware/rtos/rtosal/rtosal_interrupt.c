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
* @file   rtosal_event.c
* @author Ronen Haen
* @date   21.01.2019 
* @brief  The file implements the RTOS AL interrupt API
* 
*/

/**
* include files
*/
#include "rtosal_interrupt_api.h"
#include "rtosal.h"
#include "rtosal_interrupt_cfg.h"
#include "rtosal_macro.h"
#ifdef D_USE_FREERTOS
   #include "task.h"
#endif /* #ifdef D_USE_FREERTOS */

/**
* definitions
*/

/**
* macros
*/

/**
* types
*/
typedef struct rtosalInterrupt
{

} rtosalInterrupt_t;

/**
* local prototypes
*/

/**
* external prototypes
*/
extern unsigned long rtosalDefaultExceptionIntHandler(unsigned long mcause, unsigned long sp, unsigned long ecallArg);
extern void rtosalDefaultMSoftIntHandler(void);
extern void rtosalDefaultMTimerIntHandler(void);
extern void rtosalDefaultMExternIntHandler(void);
extern void rtosalDefaultEmptyIntHandler(void);

/**
* global variables
*/
rtosalInterrupt_t exceptions_ints[D_NUM_OF_INTS_EXCEPTIONS];
rtosalInterrupt_t u_soft_ints[D_NUM_OF_INTS_U_SOFT];
rtosalInterrupt_t s_soft_ints[D_NUM_OF_INTS_S_SOFT];
rtosalInterrupt_t m_soft_ints[D_NUM_OF_INTS_M_SOFT];
rtosalInterrupt_t u_timer_ints[D_NUM_OF_INTS_U_TIMER];
rtosalInterrupt_t s_timer_ints[D_NUM_OF_INTS_S_TIMER];
rtosalInterrupt_t m_timer_ints[D_NUM_OF_INTS_M_TIMER];
rtosalInterrupt_t u_external_ints[D_NUM_OF_INTS_U_EXTERN];
rtosalInterrupt_t s_external_ints[D_NUM_OF_INTS_S_EXTERN];
rtosalInterrupt_t m_external_ints[D_NUM_OF_INTS_M_EXTERN];
rtosalInterrupt_t reserved_ints[D_NUM_OF_INTS_RESERVED];

#if 0//D_VECT_TABLE == 0
rtosalGroupInterruptHandle_t  interruptMainVector[] = {
               { rtosalDefaultExceptionIntHandler },
               { rtosalDefaultEmptyIntHandler },
               { rtosalDefaultEmptyIntHandler },
               { rtosalDefaultMSoftIntHandler },
               { rtosalDefaultEmptyIntHandler },
               { rtosalDefaultEmptyIntHandler },
               { rtosalDefaultEmptyIntHandler },
               { rtosalDefaultMTimerIntHandler },
               { rtosalDefaultEmptyIntHandler },
               { rtosalDefaultEmptyIntHandler },
               { rtosalDefaultEmptyIntHandler },
               { rtosalDefaultMExternIntHandler },
               { rtosalDefaultEmptyIntHandler } };
#endif /* #if D_VECT_TABLE == 0 */

rtosalExceptionHandler_t fptrIntExceptionIntHandler = rtosalDefaultExceptionIntHandler;
rtosalInterruptHandler_t fptrIntSSoftIntHandler = rtosalDefaultEmptyIntHandler;
rtosalInterruptHandler_t fptrIntRsrvdSoftIntHandler = rtosalDefaultEmptyIntHandler;
rtosalInterruptHandler_t fptrIntMSoftIntHandler = rtosalDefaultMSoftIntHandler;
rtosalInterruptHandler_t fptrIntUTimerIntHandler = rtosalDefaultEmptyIntHandler;
rtosalInterruptHandler_t fptrIntSTimerIntHandler = rtosalDefaultEmptyIntHandler;
rtosalInterruptHandler_t fptrIntRsrvdTimerIntHandler = rtosalDefaultEmptyIntHandler;
rtosalInterruptHandler_t fptrIntMTimerIntHandler = rtosalDefaultMTimerIntHandler;
rtosalInterruptHandler_t fptrIntUExternIntHandler = rtosalDefaultEmptyIntHandler;
rtosalInterruptHandler_t fptrIntSExternIntHandler = rtosalDefaultEmptyIntHandler;
rtosalInterruptHandler_t fptrIntRsrvdExternIntHandler = rtosalDefaultEmptyIntHandler;
rtosalInterruptHandler_t fptrIntMExternIntHandler = rtosalDefaultMExternIntHandler;
rtosalInterruptHandler_t fptrIntUSoftIntHandler = rtosalDefaultEmptyIntHandler;

/**
* This function is invoked by the system timer interrupt
*
* @param  none
*
* @return none
*/
void rtosalTick(void)
{
#ifdef D_USE_FREERTOS
   if (xTaskIncrementTick() == pdTRUE)
   {
      vTaskSwitchContext();
   }
#elif D_USE_THREADX
   // call threadx
#endif /* #ifdef D_USE_FREERTOS */
}

/**
* The function installs an interrupt service routine per risc-v cuase
*
* @param fptrRtosalInterruptHandler – function pointer to the interrupt service routine
* @param eSourceInt                 – interrupt source
*
* @return u32_t                   - D_RTOSAL_SUCCESS
*                                 - D_RTOSAL_PTR_ERROR - provided invalid function pointer
*                                 - D_RTOSAL_FAIL - unsupported value in eSourceInt
*/
u32_t rtosalInstallIsr(rtosalInterruptHandler_t fptrRtosalInterruptHandler,
		               rtosalInterruptSource_t eSourceInt)
{
   M_RTOSAL_VALIDATE_FUNC_PARAM(fptrRtosalInterruptHandler, fptrRtosalInterruptHandler == NULL,
		                        D_RTOSAL_PTR_ERROR);
   M_RTOSAL_VALIDATE_FUNC_PARAM(&eSourceInt, eSourceInt >= E_LAST_SOURCE_INT, D_RTOSAL_FAIL);

   switch (eSourceInt)
   {
      case E_USER_SOFTWARE_SOURCE_INT:
    	  fptrIntUSoftIntHandler = fptrRtosalInterruptHandler;
    	  break;
      case E_SUPERVISOR_SOFTWARE_SOURCE_INT:
    	  fptrIntSSoftIntHandler = fptrRtosalInterruptHandler;
          break;
      case E_RESERVED_SOFTWARE_SOURCE_INT:
    	  fptrIntRsrvdSoftIntHandler = fptrRtosalInterruptHandler;
    	  break;
      case E_MACHINE_SOFTWARE_SOURCE_INT:
    	  fptrIntMSoftIntHandler = fptrRtosalInterruptHandler;
          break;
      case E_USER_TIMER_SOURCE_INT:
    	  fptrIntUTimerIntHandler = fptrRtosalInterruptHandler;
    	  break;
      case E_SUPERVISOR_TIMER_SOURCE_INT:
    	  fptrIntSTimerIntHandler = fptrRtosalInterruptHandler;
    	  break;
      case E_RESERVED_TIMER_SOURCE_INT:
    	  fptrIntRsrvdTimerIntHandler = fptrRtosalInterruptHandler;
    	  break;
      case E_MACHINE_TIMER_SOURCE_INT:
    	  fptrIntMTimerIntHandler = fptrRtosalInterruptHandler;
    	  break;
      case E_USER_EXTERNAL_SOURCE_INT:
    	  fptrIntUExternIntHandler = fptrRtosalInterruptHandler;
          break;
      case E_SUPERVISOR_EXTERNAL_SOURCE_INT:
    	  fptrIntSExternIntHandler = fptrRtosalInterruptHandler;
          break;
      case E_RESERVED_EXTERNAL_SOURCE_INT:
    	  fptrIntRsrvdExternIntHandler = fptrRtosalInterruptHandler;
    	  break;
      case E_MACHINE_EXTERNAL_SOURCE_INT:
    	  fptrIntMExternIntHandler = fptrRtosalInterruptHandler;
    	  break;
      default:
    	  return D_RTOSAL_FAIL;
   }

   return D_RTOSAL_SUCCESS;
}

/**
* The function installs an exception handler
*
* @param fptrRtosalExceptionHandler – function pointer to the exception handler
*
* @return u32_t                   - D_RTOSAL_SUCCESS
*                                 - D_RTOSAL_PTR_ERROR - provided invalid function pointer
*/
u32_t rtosalInstallExceptionIsr(rtosalExceptionHandler_t fptrRtosalExceptionHandler)
{
   M_RTOSAL_VALIDATE_FUNC_PARAM(fptrRtosalExceptionHandler, fptrRtosalExceptionHandler == NULL,
								D_RTOSAL_PTR_ERROR);

   fptrIntExceptionIntHandler = fptrRtosalExceptionHandler;

   return D_RTOSAL_SUCCESS;
}
