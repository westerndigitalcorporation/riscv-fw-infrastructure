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
* @author Ofer Shinaar
* @date   24.02.2020
* @brief  memory map for EH1
*/

#ifndef __BSP_MEM_MAP_H__
#define __BSP_MEM_MAP_H__

//[OS]: this is currently obsolete
//Memory map SweRV EH1 (Arup)
//[OS] #define EXT_SRAM_BASE_ADDRES 	0x00000000
//[OS] #define ICCM_BASE_ADDRESS		0xEE000000
//[OS] #define DCCM_BASE_ADDRESS		0xF0080000
//[OS] #define UART_BASE_ADDRESS		0x44A00000

//Memory map SweRV EH1 (SweRVWolf - Olof)
#define D_UART_BASE_ADDRESS     0x80002000
#define D_LED_BASE_ADDRESS      0x80001010
#define D_VERSION_REV           0x80001000
#define D_VERSION_MINOR         0x80001001
#define D_VERSION_MAJOR         0x80001002
#define D_VERSION_DIRTY         0x80001003
#define D_VERSION_SHA           0x80001004


#endif //__BSP_MEM_MAP_H__

