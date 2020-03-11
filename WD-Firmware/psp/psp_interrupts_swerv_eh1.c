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
* @file   psp_interrupts_swerv_eh1.c
* @author Ronen Haen
* @date   20.05.2019
* @brief  The file supplies interrupt services such as  - registration, default handlers and indication, external interrupt services, etc.
*         The file is specific to SweRV EH1 specifications
* 
*/

/**
* include files
*/
#include "psp_api.h"

/**
* definitions
*/

/* The stack used by interrupt service routines */
#if (0 == D_ISR_STACK_SIZE)
    #error "ISR Stack size is not defined"
#else
	static /*D_PSP_DATA_SECTION*/ D_PSP_16_ALIGNED pspStack_t udISRStack[ D_ISR_STACK_SIZE ] ;
	const pspStack_t xISRStackTop = ( pspStack_t ) &( udISRStack[ ( D_ISR_STACK_SIZE ) - 1 ] );
#endif

/* Number of external interrupt sources in the PIC */
#if (0 == D_PIC_NUM_OF_EXT_INTERRUPTS)
    #error "Definition of number of External interrupts in PIC is missing"
#else
    #define PSP_PIC_NUM_OF_EXT_INTERRUPTS D_PIC_NUM_OF_EXT_INTERRUPTS
#endif

/* Number of external interrupt sources in the PIC */
#ifndef D_EXT_INTERRUPT_FIRST_SOURCE_USED /* Note we expect 1'st ext-int source to be 0 */
    #error "Definition of 1'st External interrupt source is missing"
#else
    #define PSP_EXT_INTERRUPT_FIRST_SOURCE_USED    D_EXT_INTERRUPT_FIRST_SOURCE_USED
#endif
#if (0 == D_EXT_INTERRUPT_LAST_SOURCE_USED)
    #error "Definition of last External interrupt source is missing"
#else
    #define PSP_EXT_INTERRUPT_LAST_SOURCE_USED     D_EXT_INTERRUPT_LAST_SOURCE_USED
#endif

/**
* macros
*/

/**
* types
*/

/**
* local prototypes
*/

/* Default ISRs */
void pspDefaultExceptionIntHandler_isr(void);
void pspDefaultEmptyIntHandler_isr(void);

/* External-interrupt related functions */
D_PSP_TEXT_SECTION void pspExternalInterruptDisableNumber(u32_t uiIntNum);
D_PSP_TEXT_SECTION void pspExternalInterruptEnableNumber(u32_t uiIntNum);
D_PSP_TEXT_SECTION void pspExternalInterruptSetPriority(u32_t uiIntNum, u32_t uiPriority);
D_PSP_TEXT_SECTION void pspExternalInterruptSetThreshold(u32_t uiThreshold);
D_PSP_TEXT_SECTION pspInterruptHandler_t pspExternalInterruptRegisterISR(u32_t uiVectorNumber, pspInterruptHandler_t pIsr, void* pParameter);

// NatiR - continue with these. Check what they are
//void pspExtIntSetInterruptMode(u32_t uiInterruptNum, u32_t uiInterruptMode);
//void PSP_intStartup_vect (void);


/**
* external prototypes
*/

/**
* global variables
*/
void (*fptrPspExternalInterruptDisableNumber)(u32_t uiIntNum)                 = pspExternalInterruptDisableNumber;
void (*fptrPspExternalInterruptEnableNumber)(u32_t uiIntNum)                  = pspExternalInterruptEnableNumber;
void (*fptrPspExternalInterruptSetPriority)(u32_t uiIntNum, u32_t uiPriority) = pspExternalInterruptSetPriority;
void (*fptrPspExternalInterruptSetThreshold)(u32_t uiThreshold)               = pspExternalInterruptSetThreshold;
pspInterruptHandler_t (*fptrPspExternalInterruptRegisterISR)(u32_t uiVectorNumber, pspInterruptHandler_t pIsr, void* pParameter) = pspExternalInterruptRegisterISR;

/* Exception handlers */
D_PSP_DATA_SECTION pspInterruptHandler_t  g_fptrExceptions_ints[D_PSP_NUM_OF_INTS_EXCEPTIONS] = {
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr,
                       pspDefaultEmptyIntHandler_isr };

/* Exceptions handler pointer */
//D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntExceptionIntHandler   = pspDefaultExceptionIntHandler_isr;
// [NatiR] Temporarily not in D_PSP_DATA_SECTION. need to further investigate jump to g_fptrIntExceptionIntHandler from ecall
pspInterruptHandler_t g_fptrIntExceptionIntHandler   = pspDefaultExceptionIntHandler_isr;

/* Interrupts handler pointers */
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntSSoftIntHandler       = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntRsrvdSoftIntHandler   = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntMSoftIntHandler       = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntUTimerIntHandler      = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntSTimerIntHandler      = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntRsrvdTimerIntHandler  = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntMTimerIntHandler      = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntUExternIntHandler     = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntSExternIntHandler     = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntRsrvdExternIntHandler = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntMExternIntHandler     = pspDefaultEmptyIntHandler_isr;
D_PSP_DATA_SECTION pspInterruptHandler_t g_fptrIntUSoftIntHandler       = pspDefaultEmptyIntHandler_isr;


/* External interrupt handlers */
D_PSP_DATA_SECTION pspInterruptHandler_t G_Ext_Interrupt_Handlers[PSP_PIC_NUM_OF_EXT_INTERRUPTS];


/**
* The function installs an interrupt service routine per risc-v cause
*
* @param fptrInterruptHandler     – function pointer to the interrupt service routine
* @param uiInterruptCause           – interrupt source
*
* @return u32_t                   - previously registered ISR
*/
D_PSP_TEXT_SECTION pspInterruptHandler_t pspRegisterInterruptHandler(pspInterruptHandler_t fptrInterruptHandler, u32_t uiInterruptCause)
{
   pspInterruptHandler_t fptrFunc;

   M_PSP_ASSERT(fptrInterruptHandler == NULL && uiInterruptCause >= E_LAST_COMMON_CAUSE);

   switch (uiInterruptCause)
   {
      case E_USER_SOFTWARE_CAUSE:
    	  fptrFunc = g_fptrIntUSoftIntHandler;
    	  g_fptrIntUSoftIntHandler = fptrInterruptHandler;
    	  break;
      case E_SUPERVISOR_SOFTWARE_CAUSE:
    	  fptrFunc = g_fptrIntSSoftIntHandler;
    	  g_fptrIntSSoftIntHandler = fptrInterruptHandler;
          break;
      case E_RESERVED_SOFTWARE_CAUSE:
    	  fptrFunc = g_fptrIntRsrvdSoftIntHandler;
    	  g_fptrIntRsrvdSoftIntHandler = fptrInterruptHandler;
    	  break;
      case E_MACHINE_SOFTWARE_CAUSE:
    	  fptrFunc = g_fptrIntMSoftIntHandler;
    	  g_fptrIntMSoftIntHandler = fptrInterruptHandler;
          break;
      case E_USER_TIMER_CAUSE:
    	  fptrFunc = g_fptrIntUTimerIntHandler;
    	  g_fptrIntUTimerIntHandler = fptrInterruptHandler;
    	  break;
      case E_SUPERVISOR_TIMER_CAUSE:
    	  fptrFunc = g_fptrIntSTimerIntHandler;
    	  g_fptrIntSTimerIntHandler = fptrInterruptHandler;
    	  break;
      case E_RESERVED_TIMER_CAUSE:
    	  fptrFunc = g_fptrIntRsrvdTimerIntHandler;
    	  g_fptrIntRsrvdTimerIntHandler = fptrInterruptHandler;
    	  break;
      case E_MACHINE_TIMER_CAUSE:
    	  fptrFunc = g_fptrIntMTimerIntHandler;
    	  g_fptrIntMTimerIntHandler = fptrInterruptHandler;
    	  break;
      case E_USER_EXTERNAL_CAUSE:
    	  fptrFunc = g_fptrIntUExternIntHandler;
    	  g_fptrIntUExternIntHandler = fptrInterruptHandler;
          break;
      case E_SUPERVISOR_EXTERNAL_CAUSE:
    	  fptrFunc = g_fptrIntSExternIntHandler;
    	  g_fptrIntSExternIntHandler = fptrInterruptHandler;
          break;
      case E_RESERVED_EXTERNAL_CAUSE:
    	  fptrFunc = g_fptrIntRsrvdExternIntHandler;
    	  g_fptrIntRsrvdExternIntHandler = fptrInterruptHandler;
    	  break;
      case E_MACHINE_EXTERNAL_CAUSE:
    	  fptrFunc = g_fptrIntMExternIntHandler;
    	  g_fptrIntMExternIntHandler = fptrInterruptHandler;
    	  break;
      default:
    	  fptrFunc = NULL;
    	  break;
   }

   return fptrFunc;
}

/**
* The function installs an exception handler per exception cause
*
* @param fptrInterruptHandler     – function pointer to the exception handler
* @param uiExceptionCause           – exception cause
*
* @return u32_t                   - previously registered ISR
*/
D_PSP_TEXT_SECTION pspInterruptHandler_t pspRegisterExceptionHandler(pspInterruptHandler_t fptrInterruptHandler, u32_t uiExceptionCause)
{
   pspInterruptHandler_t fptrFunc;

   M_PSP_ASSERT(fptrInterruptHandler == NULL && uiExceptionCause >= E_EXC_LAST_COMMON);

   fptrFunc = g_fptrExceptions_ints[uiExceptionCause];

   g_fptrExceptions_ints[uiExceptionCause] = fptrInterruptHandler;

   return fptrFunc;
}

/**
* default exception interrupt handler
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspDefaultExceptionIntHandler_isr(void)
{
   /* get the exception cause */
   u32_t uiCause = M_PSP_READ_CSR(D_PSP_MCAUSE_NUM);

   /* is it a valid cause */
   M_PSP_ASSERT(uiCause < D_PSP_NUM_OF_INTS_EXCEPTIONS);

   /* call the specific exception handler */
   g_fptrExceptions_ints[uiCause]();
}


/**
* default empty interrupt handler
*
* @param none
*
* @return none
*/
D_PSP_TEXT_SECTION void pspDefaultEmptyIntHandler_isr(void)
{
}


/**
* This function disables a specified external interrupt in the PIC
*
* @param intNum = the number of the external interrupt to disable
* @return None
*/
D_PSP_TEXT_SECTION void pspExternalInterruptDisableNumber(u32_t uiIntNum)
{
	/* Clear Int-Enable bit in meie register, corresponds to given source (interrupt-number) */
	M_PSP_WRITE_REGISTER_32((D_PSP_PIC_MEIE_ADDR + (uiIntNum << 2)), 0);
}

/*
* This function enables a specified external interrupt in the PIC
*
* @param intNum = the number of the external interrupt to enable
* @return None
*/
D_PSP_TEXT_SECTION void pspExternalInterruptEnableNumber(u32_t uiIntNum)
{
	/* Set Int-Enable bit in meie register, corresponds to given source (interrupt-number) */
	M_PSP_WRITE_REGISTER_32((D_PSP_PIC_MEIE_ADDR + (uiIntNum << 2)), D_PSP_MEIE_INT_EN_MASK);
}

/*
*  This function sets the priority of a specified external interrupt
*
*  @param intNum = the number of the external interrupt to disable
*  @param priority = priority to be set
* @return None
*/
D_PSP_TEXT_SECTION void pspExternalInterruptSetPriority(u32_t uiIntNum, u32_t uiPriority)
{
	/* Set priority in meipl register, corresponds to given source (interrupt-number) */
	M_PSP_WRITE_REGISTER_32((D_PSP_MEIPL_ADDR + (uiIntNum << 2)), uiPriority);
}

/*
* This function sets the priority threshold of the external interrupts in the PIC
*
* @param threshold = priority threshold to be programmed to PIC
* @return None
*/
D_PSP_TEXT_SECTION void pspExternalInterruptSetThreshold(u32_t uiThreshold)
{
	/* Set in meipt CSR, the priority-threshold */
	M_PSP_WRITE_CSR(D_PSP_MEIPT_NUM, uiThreshold);
}

/*
* This function registers external interrupt handler
*
* @param uiVectorNumber = the number of the external interrupt to register
*        pIsr = the ISR to register
*        pParameter = NOT IN USED for baremetal implementation
* @return pOldIsr = pointer to the previously registered ISR
*/
D_PSP_TEXT_SECTION pspInterruptHandler_t pspExternalInterruptRegisterISR(u32_t uiVectorNumber, pspInterruptHandler_t pIsr, void* pParameter)
{

   void (*pOldIsr)(void) = 0;

  /* check if uiVectorNumber is external interrupts, else do assert */
  if(PSP_EXT_INTERRUPT_FIRST_SOURCE_USED > uiVectorNumber  || PSP_EXT_INTERRUPT_LAST_SOURCE_USED < uiVectorNumber)
  {
    pOldIsr = NULL;
  }
  else
  {
    /* register the interrupt */
	pOldIsr = G_Ext_Interrupt_Handlers[uiVectorNumber];
    G_Ext_Interrupt_Handlers[uiVectorNumber] = pIsr;

    /* dsync make sure changes go to memory */
    //M_PSP_DSYNC(); /* Nati - check here - should I use fence */
  }

  return(pOldIsr);
}

