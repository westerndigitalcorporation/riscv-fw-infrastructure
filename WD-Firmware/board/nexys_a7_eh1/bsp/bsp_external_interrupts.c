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
* @file   bsp_external_interrupts.c
* @author Nati Rapaport
* @date   29.03.2020
* @brief  External_interrupts creation in Nexys_A7 SweRVolf FPGA
*/

/**
* include files
*/
#include "psp_api.h"
#include "bsp_external_interrupts.h"

/**
* definitions
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
*
*/
void bspInitializeGenerationRegister(u32_t uiExtInterruptPolarity)
{
    u32_t uiRegisterClear;

    /* For Active-High the initial value of uiRegisterClear is 0 */
    if (D_PSP_EXT_INT_ACTIVE_HIGH == uiExtInterruptPolarity)
    {
        uiRegisterClear = 0;
    }
    else /*(D_PSP_EXT_INT_ACTIVE_LOW == uiExtInterruptPolarity) */
    {
        uiRegisterClear = 0x22; /* 00100010 */
    }

    M_PSP_WRITE_REGISTER_32(D_BSP_EXT_INTS_GENERATION_REGISTER, uiRegisterClear);
}


/**
* For IRQ3 or IRQ4 on the SweRVolf FPGA board, set polarity, type(i.e.edge/level) and triggering the interrupt
*
* @param - uiExtInterruptNumber - D_BSP_IRQ_3 or D_BSP_IRQ_4
* @param - uiExtInterruptPolarity - Active High / Low
* @param - uiExtInterruptType - Edge (pulse of 1 clock cycle) / Level (change level)
*/
void bspGenerateExtInterrupt(u32_t uiExtInterruptNumber, u32_t uiExtInterruptPolarity, u32_t uiExtInterruptType)
{
    u32_t uiExtInterruptBitMap = 0;

    if (D_BSP_IRQ_3 == uiExtInterruptNumber)
    {
        if (D_PSP_EXT_INT_ACTIVE_LOW == uiExtInterruptPolarity)
        {
            uiExtInterruptBitMap |= (1 << D_BSP_IRQ3_POLARITY_BIT); /* bit#1:  1 = Active Low, 0 = Active High */
        }
        if (D_PSP_EXT_INT_EDGE_TRIG_TYPE == uiExtInterruptType)
        {
            uiExtInterruptBitMap |= (1 << D_BSP_IRQ3_TYPE_BIT);  /* bit#2:  1 = Edge, 0 = Level */
        }
        uiExtInterruptBitMap |= (1 << D_BSP_IRQ3_ACTIVATE_BIT);  /* Set the trigger bit */
    }
    else if (D_BSP_IRQ_4 == uiExtInterruptNumber)
    {
        if (D_PSP_EXT_INT_ACTIVE_LOW == uiExtInterruptPolarity)
        {
            uiExtInterruptBitMap |= (1 << D_BSP_IRQ4_POLARITY_BIT); /* bit#5:  1 = Active Low, 0 = Active High */
        }
        if (D_PSP_EXT_INT_EDGE_TRIG_TYPE == uiExtInterruptType)
        {
            uiExtInterruptBitMap |= (1 << D_BSP_IRQ4_TYPE_BIT);  /* bit#6:  1 = Edge, 0 = Level */
        }
        uiExtInterruptBitMap |= (1 << D_BSP_IRQ4_ACTIVATE_BIT);  /* Set the trigger bit */
    }

    M_PSP_WRITE_REGISTER_32(D_BSP_EXT_INTS_GENERATION_REGISTER, uiExtInterruptBitMap);
}

/**
* Clear the trigger that generate external interrupt
*
* @param - uiExtInterruptNumber - D_BSP_IRQ_3 or D_BSP_IRQ_4
*
*/
void bspClearExtInterrupt(u32_t uiExtInterruptNumber)
{
    u32_t uiExtInterruptBitMap = M_PSP_READ_REGISTER_32(D_BSP_EXT_INTS_GENERATION_REGISTER);

    if (D_BSP_IRQ_3 == uiExtInterruptNumber)
    {
        uiExtInterruptBitMap &= ~(1 << D_BSP_IRQ3_ACTIVATE_BIT);
    }
    else if (D_BSP_IRQ_4 == uiExtInterruptNumber)
    {
        uiExtInterruptBitMap &= ~(1 << D_BSP_IRQ4_ACTIVATE_BIT);
    }

    M_PSP_WRITE_REGISTER_32(D_BSP_EXT_INTS_GENERATION_REGISTER, uiExtInterruptBitMap );
}


