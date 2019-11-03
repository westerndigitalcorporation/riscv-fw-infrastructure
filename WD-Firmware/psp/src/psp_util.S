/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * The FreeRTOS kernel's RISC-V port is split between the the code that is
 * common across all currently supported RISC-V chips (implementations of the
 * RISC-V ISA), and code which tailors the port to a specific RISC-V chip:
 *
 * + The code that is common to all RISC-V chips is implemented in
 *   FreeRTOS\Source\portable\GCC\RISC-V-RV32\portASM.S.  There is only one
 *   portASM.S file because the same file is used no matter which RISC-V chip is
 *   in use.
 *
 * + The code that tailors the kernel's RISC-V port to a specific RISC-V
 *   chip is implemented in freertos_risc_v_chip_specific_extensions.h.  There
 *   is one freertos_risc_v_chip_specific_extensions.h that can be used with any
 *   RISC-V chip that both includes a standard CLINT and does not add to the
 *   base set of RISC-V registers.  There are additional
 *   freertos_risc_v_chip_specific_extensions.h files for RISC-V implementations
 *   that do not include a standard CLINT or do add to the base set of RISC-V
 *   registers.
 *
 * CARE MUST BE TAKEN TO INCLDUE THE CORRECT
 * freertos_risc_v_chip_specific_extensions.h HEADER FILE FOR THE CHIP
 * IN USE.  To include the correct freertos_risc_v_chip_specific_extensions.h
 * header file ensure the path to the correct header file is in the assembler's
 * include path.
 *
 * This freertos_risc_v_chip_specific_extensions.h is for use on RISC-V chips
 * that include a standard CLINT and do not add to the base set of RISC-V
 * registers.
 *
 */

#include "psp_specific_definitions.h"
#include "psp_specific_macros.h"
#include "psp_asm_macros.h"
#include "psp_specific_rtos.h"
#include "rtosal_asm_macros.h"

.global pspHandleEcall
.global pspInitializeStack
.extern vTaskSwitchContext

.global pxCurrentTCB

.align 8
.func
pspInitializeStack:

	csrr t0, mstatus					/* Obtain current mstatus value. */
    M_ADDI t1, x0, 0x188                /* Generate the value 0x1880, which are -   */
	slli t1, t1, 4                      /* the MPIE and MPP bits to set in mstatus. */
	or t0, t0, t1						/* Set MPIE and MPP bits in mstatus value. */

    M_ADDI a0, a0, -REGBYTES
    M_STORE t0, 0(a0)                         /* mstatus onto the stack. */
    M_ADDI a0, a0, -(22 * REGBYTES)           /* Space for registers x11-x31. */
    M_STORE a2, 0(a0)                         /* Task parameters (pvParameters parameter) goes into register X10/a0 on the stack. */
    M_ADDI a0, a0, -(6 * REGBYTES)            /* Space for registers x5-x9. */
    M_STORE x0, 0(a0)                         /* Return address onto the stack, could be pspTASK_RETURN_ADDRESS */
    M_ADDI t0, x0, D_pspAdditionalContextSize /* The number of chip specific additional registers. */
chip_specific_stack_frame:                    /* First add any chip specific registers to the stack frame being created. */
    beq t0, x0, 1f                            /* No more chip specific registers to save. */
    M_ADDI a0, a0, -REGBYTES                  /* Make space for chip specific register. */
    M_STORE x0, 0(a0)                         /* Give the chip specific register an initial value of zero. */
    M_ADDI t0, t0, -1                         /* Decrement the count of chip specific registers remaining. */
    j chip_specific_stack_frame               /* Until no more chip specific registers. */
1:
    M_ADDI a0, a0, -REGBYTES
    M_STORE a1, 0(a0)                         /* mret value (pxCode parameter) onto the stack. */
    ret
.endfunc

/* Implements context-switch as a result of ECALL exception */
pspHandleEcall:
    /* Update & save MEPC contents on the Task's stack: */
    /* 1. Use Task's stack (rather than ISR's stack) */
    M_REVERT_SP_FROM_ISR_TO_APP_STACK  pxCurrentTCB, 0
    /* 2. Read MEPC content as saved on stack (location 0) */
    M_LOAD  a0, D_MEPC_LOC_IN_STK(sp)
    /* 3. Advance the address by 4 in order to return the next instraction right after the ECALL yielding */
    M_ADDI  a0, a0, 4
    /* 4. Write back the number to Task's stack (location 0) */
    M_STORE a0, D_MEPC_LOC_IN_STK(sp)
#ifdef D_USE_FREERTOS
    M_SET_SP_FROM_APP_TO_ISR_STACK
    jal vTaskSwitchContext
#else
   #error Add appropriate RTOS definitions
#endif
    M_RESTORE_CONTEXT pxCurrentTCB, 0    /* This macro restors sp to the one used by current Task, and restores MEPC and MSTATUS CSRs*/
    M_CLEAR_INT_CONTEXT
    M_POP_REGFILE
    m_popAdditionalRegs
    mret


