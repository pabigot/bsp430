/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output.  Also support an output buffer so we can
 * compare ISR-based transmission with DMA transmission. */
#define configBSP430_CONSOLE 1
#ifndef BSP430_CONSOLE_TX_BUFFER_SIZE
#define BSP430_CONSOLE_TX_BUFFER_SIZE 80
#endif /* BSP430_CONSOLE_TX_BUFFER_SIZE */

/* Monitor uptime and provide generic timer with delay capability. */
#define configBSP430_UPTIME 1
#define configBSP430_UPTIME_DELAY 1

/* Use the DMA HAL.  We'll also use the implicitly-included ISR
 * capability */
#define configBSP430_HAL_DMA 1

/* Use the secondary timer for high-resolution timing without overflow
 * support. */
#define configBSP430_TIMER_CCACLK 1

#if (BSP430_PLATFORM_TRXEB - 0)
#define CONSOLE_DMATSEL 21
#define CONSOLE_DMADA &UCA1TXBUF
#elif (BSP430_PLATFORM_EXP430FG4618 - 0)
#define CONSOLE_DMATSEL 4
#define CONSOLE_DMADA &UCA0TXBUF
#endif

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
