/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Interrupts should always be disabled on wakeup, to allow safe
 * processing of any recorded events. */
#define configBSP430_CORE_LPM_EXIT_CLEAR_GIE 1

/* Support console buffered input and output at a higher-than-normal
 * data rate. */
#define configBSP430_CONSOLE 1
#define BSP430_CONSOLE_TX_BUFFER_SIZE 100
#define BSP430_CONSOLE_RX_BUFFER_SIZE 16
#define BSP430_CONSOLE_BAUD_RATE 115200

/* Add support to tell the user where to connect things */
#define configBSP430_PLATFORM_PERIPHERAL_HELP 1

/* Monitor uptime and provide generic timer, with delay support */
#define configBSP430_UPTIME 1
#define BSP430_UPTIME_DELAY_CCIDX 1

/* Need a CC register, preferably on the uptime timer, to use for 1PPS
 * capture.  Remember that CC0 is for RTOS switching, CC1 is the
 * default for BSP430_UPTIME_DELAY_CCIDX, and CC2 is the default for
 * BSP430_TIMER_SAFE_COUNTER_READ_CCIDX.  If your TA0 only has three
 * capture/compare registers, you may want to override
 * BSP430_UPTIME_TIMER_PERIPH_CPPID to select a more capable timer. */
#if ((BSP430_PLATFORM_TRXEB - 0)                \
     || (BSP430_PLATFORM_EXP430F5438 - 0))
/* TA0.3 == P1.4 CCI1A */
#define APP_PPS_CCIDX 3
#define APP_PPS_CCIS CCIS_0
#define APP_PPS_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define APP_PPS_PORT_BIT BIT4
#define configBSP430_HAL_PORT1 1
#endif

/* Platform-specific UART for NMEA data */
#if (BSP430_PLATFORM_TRXEB - 0)
#define configBSP430_HAL_USCI5_A0 1
#define APP_NMEA_UART_PERIPH_HANDLE BSP430_PERIPH_USCI5_A0
#endif /* Platform */

/* Site-specific baud rate for NMEA data */
#ifndef APP_NMEA_BAUD_RATE
#define APP_NMEA_BAUD_RATE 9600
#endif /* APP_NMEA_BAUD_RATE */

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
