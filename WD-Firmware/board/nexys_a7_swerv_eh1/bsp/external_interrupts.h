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
* @file   external_interrupts.h
* @author Nati Rapaport
* @date   29.03.2020
* @brief  External_interrupts creation in Nexys_A7 SweRVolf FPGA 
*/
#ifndef  __BSP_EXTERNAL_INTERRUPTS_H__
#define  __BSP_EXTERNAL_INTERRUPTS_H__

/**
* include files
*/

/**
* definitions
*/
/* In SwreVolf we have the capability to create 2 different external interrupts */
#define D_BSP_IRQ_3           3
#define D_BSP_IRQ_4           4
#define D_BSP_FIRST_IRQ_NUM   D_BSP_IRQ_3
#define D_BSP_LAST_IRQ_NUM    D_BSP_IRQ_4



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
* macros
*/


/**
* global variables
*/

/**
* APIs
*/

/**
* Zero the register that used for IRQs generation
*
* @param - uiExtInterruptPolarity - Active High / Low
*/
void bspClearGenerationRegister(u32_t uiExtInterruptPolarity);

/**
* For IRQ3 or IRQ4 on the SweRVolf FPGA board, set polarity, type(i.e.edge/level) and triggering the interrupt
*
* @param - uiExtInterruptNumber - D_BSP_IRQ_3 or D_BSP_IRQ_4
* @param - uiExtInterruptPolarity - Active High / Low
* @param - uiExtInterruptType - Edge (pulse of 1 clock cycle) / Level (change level)
*/
void bspGenerateExtInterrupt(u32_t uiExtInterruptNumber, u32_t uiExtInterruptPolarity, u32_t uiExtInterruptType);


/**
* Clear generation of external interrupt(s)
*
* @param - uiExtInterruptNumber - D_BSP_IRQ_3 or D_BSP_IRQ_4
*
*/
void bspClearExtInterrupt(u32_t uiExtInterruptNumber);



#endif /* __BSP_EXTERNAL_INTERRUPTS_H__ */
