/** This file is in the public domain.
 *
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>

volatile unsigned int events_ni;

static int
rx_cb (void)
{
  ++events_ni;
  return BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
}

void main ()
{
  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();
  vBSP430consoleSetRxCallback_ni(rx_cb);

  BSP430_CORE_ENABLE_INTERRUPT();
  cprintf("rxcb " __DATE__ " " __TIME__ "\n"
          "Sleep until keys pressed.\n"
          "Additional events may occur while displaying previous events.\n");
  while (1) {
    char timestamp[BSP430_UPTIME_AS_TEXT_LENGTH];
    int c;
    unsigned int events;
    
    BSP430_CORE_DISABLE_INTERRUPT();
    events = events_ni;
    events_ni = 0;
    if (! events) {
      BSP430_CORE_LPM_ENTER_NI(LPM0_bits | GIE);
      continue;
    }
    BSP430_CORE_ENABLE_INTERRUPT();
    cprintf("%s: Up with %u rx callbacks:", xBSP430uptimeAsText(ulBSP430uptime_ni(), timestamp), events);
    while (0 <= ((c = cgetchar()))) {
      cprintf(" %02x", c);
    }
    cputchar('\n');
  }
}
