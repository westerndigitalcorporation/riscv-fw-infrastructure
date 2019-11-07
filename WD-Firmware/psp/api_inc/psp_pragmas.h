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
* @file   psp_pragmas.h
* @author Ofer Shinaar
* @date   11/2019
* @brief  The file defines the psp pragmas
*/
#ifndef __PSP_PRAGMAS_H__
#define __PSP_PRAGMAS_H__

  #if defined (__GNUC__) || defined (__clang__)


  #define DO_PRAGMA(_PRAGMA_NAME_)     _Pragma(#_PRAGMA_NAME_)
  #define TODO(_MY_MSG_)               DO_PRAGMA(message ("TODO - " #_MY_MSG_))
  #define PRE_COMPILED_MSG(_MY_MSG_)   DO_PRAGMA(message (#_MY_MSG_))


#endif // defined __GNUC__ || defined __clang__

#endif /* __PSP_MACRO_H__ */
