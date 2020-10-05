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
* @file   cti_tests.h
* @author Ronen Haen
* @date   01.07.2020
* @brief  The file defines cti (comrv testing infrastructute) test overlay functions
*/
#ifndef __CTI_TESTS_H
#define __CTI_TESTS_H

/*
* INCLUDES
*/
#include "comrv_defines.h"

/*
* MACROS
*/

/*
* DEFINITIONS
*/

/*
* TYPEDEFS
*/

/*
* EXPORTED GLOBALS
*/

/*
* FUNCTIONS PROTOTYPES
*/
typedef int (*comrvti_test_ovl)(S_FW_CB_PARAM_PTR);
extern const ctiTestFunctionPtr g_pLookupTableCtiTestOvl[E_CB_TEST_OVL_MAX];
void ctiSwiIsr();

/*
* OVL functions prototypes
*/
void _OVERLAY_ ctiTestFuncOverlay101Vect(void);
void _OVERLAY_ ctiTestFuncOverlay102Vect(void);
void _OVERLAY_ ctiTestFuncOverlay103Vect(void);
void _OVERLAY_ ctiTestFuncOverlay104Vect(void);
void _OVERLAY_ ctiTestFuncOverlay105Vect(void);
void _OVERLAY_ ctiTestFuncOverlay106Vect(void);
void _OVERLAY_ ctiTestFuncOverlay107Vect(void);
void _OVERLAY_ ctiTestFuncOverlay109Vect(void);
void _OVERLAY_ ctiTestFuncOverlay110Vect(void);
void _OVERLAY_ ctiTestFuncOverlay111Vect(void);
void _OVERLAY_ ctiTestFuncOverlay112Vect(void);
#ifdef D_COMRV_RTOS_SUPPORT
void _OVERLAY_ ctiTestFuncOverlay108Vect(void);
void _OVERLAY_ ctiTestFuncOverlay113Vect(u32_t var1, u32_t var2, u32_t var3);
#endif /* D_COMRV_RTOS_SUPPORT */


/*
* OVL DUMMY functions prototypes and define
*/
void  _OVERLAY_ ctiTestFuncOverlay120Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay121Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay122Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay123Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay124Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay125Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay126Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay127Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay128Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay129Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay130Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay131Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay132Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay133Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay134Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay135Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay136Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay137Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay138Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay139Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay140Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay141Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay142Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay143Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay144Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay145Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay146Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay147Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay148Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay149Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay150Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay151Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay152Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay153Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay154Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay155Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay156Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay157Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay158Vect(void);
void  _OVERLAY_ ctiTestFuncOverlay159Vect(void);

#endif /* __CTI_TESTS_H */
