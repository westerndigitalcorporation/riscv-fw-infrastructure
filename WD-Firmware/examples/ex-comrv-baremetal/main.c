#include "common_types.h"
#include "comrv_api.h"

/* non overlay function */
__attribute__((noinline)) void NonOverlayFunc(void)
{
   *((unsigned int*)0x80001000) = *((unsigned int*)0x80001010) + *((unsigned int*)0x800010200);
}

/* overlay function 1 */
void _OVERLAY_ OverlayFunc1(void)
{
   NonOverlayFunc();
}

/* overlay function 0 */
void _OVERLAY_ OverlayFunc0(void)
{
   OverlayFunc1();
}

int main(void)
{
   comrvInit();

   OverlayFunc0();
}
