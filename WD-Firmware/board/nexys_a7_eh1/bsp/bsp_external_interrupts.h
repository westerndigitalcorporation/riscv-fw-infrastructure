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
* @file   bsp_external_interrupts.h
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

/* Specified RAM address for generation of external interrupts (SwerVolf special implementation) */
#if (0 != D_EXT_INTS_GENERATION_REG_ADDRESS)
    #define D_BSP_EXT_INTS_GENERATION_REGISTER    D_EXT_INTS_GENERATION_REG_ADDRESS
#else
    #error "External interrupts generation address is not defined"
#endif

/* In SwreVolf we have the capability to create 2 different external interrupts */
#define D_BSP_IRQ_3           3
#define D_BSP_IRQ_4           4
#define D_BSP_FIRST_IRQ_NUM   D_BSP_IRQ_3
#define D_BSP_LAST_IRQ_NUM    D_BSP_IRQ_4

/*
Register 0x8000100B (offset B in sys_con memory, at SweRVolf FPGA) is used for generating external interrupts by demo FW
-------------------------------------------
| Bits | Name               |    Description   |
| ---- | ------------------ | ------------------
|    7 | irq4_triger        | --> 1 = Trigger IRQ line 4
|    6 | irq4_type          | --> Interrupt type: 0 = Level. IRQ4 is asserted until sw_irq4 is cleared, 1 = Edge. Writing to sw_irq4 only asserts IRQ4 for one clock cycle
|    5 | irq4_polarity      | --> IRQ4 polarity: 0 = Active high, 1 = active low
|    4 | rout_timer_to_irq4 | --> When set to '1', Timer interrupt is routed to irq4
|    3 | irq3_triger        | --> 1 = Trigger IRQ line 3
|    2 | irq3_type          | --> Interrupt type: 0 = Level. IRQ3 is asserted until sw_irq3 is cleared, 1 = Edge. Writing to sw_irq3 only asserts IRQ3 for one clock cycle
|    1 | irq3_polarity      | --> IRQ3 polarity: 0 = Active high, 1 = active low
|    0 | rout_timer_to_irq3 | --> When set to '1', Timer interrupt is routed to irq3
-------------------------------------------*/
#define D_BSP_ROUT_TIMER_TO_IRQ3  0
#define D_BSP_IRQ3_POLARITY_BIT   1
#define D_BSP_IRQ3_TYPE_BIT       2
#define D_BSP_IRQ3_ACTIVATE_BIT   3
#define D_BSP_ROUT_TIMER_TO_IRQ4  4
#define D_BSP_IRQ4_POLARITY_BIT   5
#define D_BSP_IRQ4_TYPE_BIT       6
#define D_BSP_IRQ4_ACTIVATE_BIT   7


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
* Initialize the register that used for triggering IRQs generation
*
* @param uiExtInterruptPolarity - indicates the polarity mode. Based on that, the triggering register should be initialized.
*/
void bspInitializeGenerationRegister(u32_t uiExtInterruptPolarity);

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
