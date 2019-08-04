#include "common_types.h"
#include "comrv_api.h"

volatile u32_t globalCounter = 0;

/* non overlay function */
void NonOverlayFunc(void)
{
}

/* overlay function 1 */
OVERLAY_SECTION void _OVERLAY_ OverlayFunc1(void)
{
  globalCounter++;
  INVOKE_OVERLAY_ENGINE(NonOverlayFunc, 0);
  globalCounter--;
}

/* overlay function 0 */
OVERLAY_SECTION void _OVERLAY_ OverlayFunc0(void)
{
  globalCounter++;
  INVOKE_OVERLAY_ENGINE(OverlayFunc1, 1);
  globalCounter--;
}

int main(void)
{
  comrvInit();

  do
  {
    globalCounter++;
	INVOKE_OVERLAY_ENGINE(OverlayFunc0, 1);
    globalCounter--;
  } while(globalCounter == 0);
}
