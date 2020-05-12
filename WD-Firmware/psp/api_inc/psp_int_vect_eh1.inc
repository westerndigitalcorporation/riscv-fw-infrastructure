/*
 SPDX-License-Identifier: Apache-2.0
 Copyright 2019 Western Digital Corporation or its affiliates.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http:www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

/** Non standard CSRs in SweRV first-generation cores   **/


.macro M_PSP_CALL_EXT_INT_HANDLER
  csrwi 0xBCA, 1  /* Trigger capture of the interrupt source ID(handler address), write '1' to meicpct */
  csrr t0, 0xFC8  /* Obtain external interrupt handler address from meihap register */
  lw t0, 0(t0)
  jalr t0         /* Invoke user interrupt handler */
.endm






