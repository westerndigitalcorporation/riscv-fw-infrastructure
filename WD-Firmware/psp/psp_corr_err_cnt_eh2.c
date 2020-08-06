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
* @file   psp_corr_err_cnt_eh2.c
* @author Nati Rapaport
* @date   05.07.2020
* @brief  The file contains interface for correctable-error counters in eh2
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
#define D_PSP_CORR_ERR_MAX_THRESHOLD 26

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
* @brief Set Thershold for a counter of specific correctable-error
*
* @param -  eCorrectableerrorCounter - one of E_ICACHE_CORR_ERR_COUNTER or E_ICCM_CORR_ERR_COUNTER or E_DCCM_CORR_ERR_COUNTER
* @param -  uiThreshold - When set,  an interrupt is raised when 2**uiThreshold errors occur
*
*           ***Note*** 26 is the largest acceptable value. If a larger value is set, it is treated as 26.
*
*           ***Note*** Pay attention to register your ISR for E_MACHINE_CORRECTABLE_ERROR_CAUSE interrupt
*
*/
D_PSP_TEXT_SECTION void pspCorErrCntSetThreshold(ePspCorrectableErrorCounters_t eCounter, u32_t uiThreshold)
{
  u32_t uiCsrValueToSet = 0 ;

  /* Maximum number to set as threshold is 26 */
  M_PSP_ASSERT(D_PSP_CORR_ERR_MAX_THRESHOLD >= uiThreshold);

  /* As these CSRs are common to all harts, make sure that they will not be accessed simultaneously by more than single hart */
  M_PSP_INTERNAL_MUTEX_LOCK(E_MUTEX_INTERNAL_FOR_CORR_ERR_COUNTERS);

  uiCsrValueToSet = (uiThreshold << D_PSP_CORR_ERR_THRESH_SHIFT);

  /* Set the threshold in the relevant CSR */
  switch (eCounter)
  {
    case E_ICACHE_CORR_ERR_COUNTER:
      M_PSP_WRITE_CSR(D_PSP_MICECT_NUM, uiCsrValueToSet);
      break;
    case E_ICCM_CORR_ERR_COUNTER:
      M_PSP_SET_CSR(D_PSP_MICCMECT_NUM, uiCsrValueToSet);
      break;
    case E_DCCM_CORR_ERR_COUNTER:
      M_PSP_SET_CSR(D_PSP_MDCCMECT_NUM, uiCsrValueToSet);
      break;
    default:
      break;
  }

  /* Remove the multi-harts access protection */
  M_PSP_INTERNAL_MUTEX_UNLOCK(E_MUTEX_INTERNAL_FOR_CORR_ERR_COUNTERS);
}
