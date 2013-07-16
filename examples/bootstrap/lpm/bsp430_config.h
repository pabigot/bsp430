/* No console, no uptime, no crystal.  Will not invoke platform
 * initialize, so no need to configure those.  The only thing needed
 * is to specify the target LPM mode. */

/** Low power mode to enter.  Valid values are 0 through 4, plus -1 to
 * indicate that the application should spin with the CPU enabled. */
#ifndef APP_LPM
#define APP_LPM 4
#endif /* APP_LPM */

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
