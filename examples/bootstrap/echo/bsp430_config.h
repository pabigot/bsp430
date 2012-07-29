#include <bsp430/platform/bsp430_config.h>

/* We need to access HAL internals to implement the callbacks */
#define configBSP430_UART_EXPOSE_STATE 1
