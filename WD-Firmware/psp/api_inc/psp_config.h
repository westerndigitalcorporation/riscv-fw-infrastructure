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
* @file   psp_config.h
* @author Ronen Haen
* @date   20.05.2019
* @brief  The file defines the psp configuration
*/
#ifndef  __PSP_CONFIG_H__
#define  __PSP_CONFIG_H__

/**
* include files
*/

/**
* definitions
*/

/* TODO: change D_PSP_ERROR_CHECK to be determined by the build system */
#define D_PSP_ERROR_CHECK            0
#define D_PSP_NUM_OF_INTS_EXCEPTIONS E_EXC_LAST_CAUSE

/* Number of HW threads in the core */
#ifndef D_NUMBER_OF_HARTS
    #define D_PSP_NUM_OF_HARTS    1
#else
    #define D_PSP_NUM_OF_HARTS    D_NUMBER_OF_HARTS
#endif


#endif /* __PSP_CONFIG_H__ */
