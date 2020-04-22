
.global demoNmiPostTest
.global demoNmiPinAssertionHandler

.global uiJumpHereFromNmi  /* Address to jump after NMI occurs in order to be able to return back to 'main' function */
.global uiDelayAtPostTest  /* 1 Billion iterations = 20 seconds (SweRVolf FGPA rate is 5Mhz) */

/* When NMI occures, there is no standard way to return from it to the foreground context.
 * In this function we bypass it in order to be able to roll back to the 'demoStart' and then 'main' functions, so the standard test structure is maintained.
 *
 * This function does the following:
 *  1. Store ra in order to make sure that at the end of the function (after NMI occured, and stack content is no longer reliable) function could safetly return to caller.
 *  2. Get PC value, store it in register and add to it an offset beyond the loop block (see next) and save the result to a global variable (uiJumpHereFromNmi).
 *  3. Loop 1 bilion iterations. Equal to 20 second with 50Mhz FPGA rate.
 *  4. In case NMI has occured in the meantime, the NMI handler function (demoNmiPinAssertionHandler) does jump to the address that stored in uiJumpHereFromNmi -
 *     this address is after the loop block.
 *  5. ra contents is restored
 *  6. Function returns to the caller ('demoStart') - Test passed ok.
 *  7. In case NMI has not occured, the loop exires after 20 seconds and ebraek is done - Tast failed.
 */
demoNmiPostTest:
   mv t1, ra                    /* store ra in order to make sure we could return safely to demoStart function */
   auipc t3, 0                  /* Set current PC in t3 */
   addi t3, t3, 0x40            /* Add offset - until after lable '3' (0x3C bytes) */
   la t4, uiJumpHereFromNmi     /* Store this address in uiJumpHereFromNmi global variable - to that address the NMI handler will jump the PC */ 
   sw t3, 0(t4)

   la t4, uiJumpHereFromNmi     /* 'uiJumpHereFromNmi' is getting the value of the PC + offset after lable '3' (0x28 bytes) */
   lw t4, 0(t4)
   
   /* loop here bilion times - 20 seconds (SweRVolf rate is 50Mhz) */
   sw zero, (t5)
   la t6, uiDelayAtPostTest
   lw t6, 0(t6)
1: 
   addi t5, t5, 1
   bltu t5, t6, 1b
2: 
   ebreak /* Arriving here means test failed, as the NMI should have been occured already */
3: 
   nop /* Add some nop instructions here to make sure the skip is not beyond the 'ret' instruction */
   nop
   nop
   nop
   nop
   mv ra, t1  /* restore ra */
   ret        /* Arriving here means test passed */
   
/* NMI handler */
demoNmiPinAssertionHandler:
   la t3, uiJumpHereFromNmi
   lw t3, 0(t3)
   jalr t3                  /* jump to the address that was stored in 'uiJumpHereFromNmi' global variable */

