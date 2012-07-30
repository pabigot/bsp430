/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* We need to access HAL internals to implement the callbacks */
#define configBSP430_SERIAL_EXPOSE_STATE 1

#include <bsp430/platform/bsp430_config.h>
