#include "common_types.h"
#include "comrv_api.h"


#define OVL_OverlayFunc0 _OVERLAY_
#define OVL_OverlayFunc1 _OVERLAY_
#define OVL_OverlayFunc2 _OVERLAY_
#define OVL_benchmark  //_OVERLAY_
#define OVL_jpegdct  //_OVERLAY_

//extern int OVL_benchmark benchmark(void);

volatile u32_t globalCount = 0;
volatile u32_t gOverlayFunc0 = 0;
volatile u32_t gOverlayFunc1 = 0;
volatile u32_t gOverlayFunc2 = 0;

/* overlay function 2 */
void OVL_OverlayFunc2 OverlayFunc2(void)
{
   gOverlayFunc2+=3;
}

/* non overlay function */
__attribute__((noinline)) void NonOverlayFunc(void)
{
   globalCount+=1;
   OverlayFunc2();
   globalCount+=2;
}

/* overlay function 1 */
void OVL_OverlayFunc1 OverlayFunc1(void)
{
   gOverlayFunc1+=3;
   NonOverlayFunc();
   gOverlayFunc1+=4;
}

/* overlay function 0 */
void OVL_OverlayFunc0 OverlayFunc0(void)
{
   gOverlayFunc0+=1;
   OverlayFunc1();
   gOverlayFunc0+=2;
}

// TODO: remove this once I add our psp interface
#if 0
   extern void psp_vect_table();
   #define CSRW(reg, val) asm volatile ("csrw " #reg ", %0" :: "r"(val));
#else
   #define CSRW(reg, val);
#endif

int main(int argc, char **argv)
{
   comrvInitArgs_t stComrvInitArgs;

   /* set the mtvec - used only in debug */
   // TODO: change to our psp
   CSRW(mtvec, &psp_vect_table);

   comrvInit(&stComrvInitArgs);

   globalCount+=1;
   OverlayFunc0();
   //benchmark();
   globalCount+=2;
   return 0;
}
