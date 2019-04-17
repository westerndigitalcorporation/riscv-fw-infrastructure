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
* @file   common_types.h
* @author Ronen Haen
* @date   21.01.2019
* @brief  The file defines the fw types
*/
#ifndef  __FW_TYPES_H__
#define  __FW_TYPES_H__

/**
* include files
*/

/**
* types
*/
typedef signed char        s08_t;
typedef signed short       s16_t;
typedef signed int         s32_t;
typedef signed long long   s64_t;
typedef unsigned char      u08_t;
typedef unsigned short     u16_t;
typedef unsigned int       u32_t;
typedef unsigned long long u64_t;

/* for general use */
typedef void (*fptrFunction)(void);

#endif /* __FW_TYPES_H__ */
