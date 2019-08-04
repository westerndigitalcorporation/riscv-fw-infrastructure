#include "common_types.h"
#include "comrv_api.h"

OVERLAY_SECTION void _OVERLAY_ OverlayFunc1(void);

/* non overlay function */
void NonOverlayFunc(void)
{
}

/* overlay function 1 */
OVERLAY_SECTION void _OVERLAY_ OverlayFunc1(void)
{
  INVOKE_OVERLAY_ENGINE(NonOverlayFunc, 0);
}

/* overlay function 0 */
OVERLAY_SECTION void _OVERLAY_ OverlayFunc0(void)
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
