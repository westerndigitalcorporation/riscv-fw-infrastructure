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

.macro M_PSP_CALL_INT_HANDLER_HART0 fptIntHandler
    /* load the address of provided fptIntHandler */
    la        a0, \fptIntHandler
    /* load the actual handler address */
    M_PSP_LOAD    a0, 0x0(a0)
    /* invoke the interrupt handler */
    jalr      a0
.endm

.macro M_PSP_CALL_INT_HANDLER_HART1 fptIntHandler
    /* load the address of provided fptIntHandler */
    la        a0, \fptIntHandler
    /* load the actual handler address - Since it is of Hart1, then the address of the interrupt handler is
     * located 4 bytes after the address of this interrupt handler of Hart0  */
    M_PSP_LOAD    a0, 0x4(a0)
    /* invoke the interrupt handler */
    jalr      a0
.endm

