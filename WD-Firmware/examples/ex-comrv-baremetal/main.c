#include "common_types.h"
#include "comrv_api.h"

/* non overlay function */
void NonOverlayFunc(void)
{
	//*((unsigned int*)0x80001000) = *((unsigned int*)0x80001010) + *((unsigned int*)0x800010200);
}

/* overlay function 1 */
OVERLAY_SECTION_1 void _OVERLAY_ OverlayFunc1(void)
{
  INVOKE_OVERLAY_ENGINE(NonOverlayFunc, 0);
}

/* overlay function 0 */
OVERLAY_SECTION_0 void _OVERLAY_ OverlayFunc0(void)
{
  INVOKE_OVERLAY_ENGINE(OverlayFunc1, 1);
}

int main(void)
{
  comrvInit();

  while (1)
  {
	INVOKE_OVERLAY_ENGINE(OverlayFunc0, 1);
  }
}
