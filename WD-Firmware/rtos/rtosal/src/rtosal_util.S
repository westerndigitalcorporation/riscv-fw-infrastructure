/*
    FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution and was contributed
    to the project by Technolution B.V. (www.technolution.nl,
    freertos-riscv@technolution.eu) under the terms of the FreeRTOS
    contributors license.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry''s de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
This implementation supports riscv privileged v1.10
*/
#include "psp_asm_macros.inc"
#include "rtosal_defines.h"
#include "rtosal_asm_macros.h"

.global pxCurrentTCB
.global rtosalHandleEcall

.extern vTaskSwitchContext



/* Implements context-switch as a result of ECALL exception */
rtosalHandleEcall:
    /* Update & save MEPC contents on the Task's stack: */
    /* 1. Use Task's stack (rather than ISR's stack) */
    M_CHANGE_SP_FROM_ISR_TO_APP_STACK  pxCurrentTCB, 0
    /* 2. Read MEPC content as saved on stack (location 0) */
    M_LOAD  a0, D_MEPC_LOC_IN_STK(sp)
    /* 3. Advance the address by 4 in order to return the next instraction right after the ECALL yielding */
    M_ADDI  a0, a0, 4
    /* 4. Write back the number to Task's stack (location 0) */
    M_STORE a0, D_MEPC_LOC_IN_STK(sp)
#ifdef D_USE_FREERTOS
//NatiR - remove the ifdef. in addition - contextSwitchFunc --> set as a macro
    M_CHANGE_SP_FROM_APP_TO_ISR_STACK
    jal contextSwitchFunc
#else
   #error Add appropriate RTOS definitions
#endif
    M_RESTORE_CONTEXT pxCurrentTCB, 0    /* This macro restors sp to the one used by current Task, and restores MEPC and MSTATUS CSRs*/
    M_CLEAR_INT_CONTEXT
    M_POP_REGFILE
    mret

