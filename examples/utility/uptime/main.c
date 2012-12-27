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

#ifndef APP_DELAY_MS
#define APP_DELAY_MS 10100
#endif /* APP_DELAY_MS */

void main ()
{
  unsigned long wake_utt;
  unsigned int iters = 0;
  
  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  {
    long ts;
    long utt;

    cprintf("Uptime timer running at %lu Hz\n", ulBSP430uptimeConversionFrequency_Hz_ni());
    ts = 12345;
    utt = BSP430_UPTIME_MS_TO_UTT(ts);
    cprintf("%ld ms is %ld utt\n", ts, utt);
    ts = BSP430_UPTIME_UTT_TO_MS(utt);
    cprintf("%ld utt is %ld ms\n", utt, ts);
    ts = 12345;
    utt = BSP430_UPTIME_US_TO_UTT(ts);
    cprintf("%ld us is %ld utt\n", ts, utt);
    ts = BSP430_UPTIME_UTT_TO_US(utt);
    cprintf("%ld utt is %ld us\n", utt, ts);
  }

#define APP_DELAY_UTT BSP430_UPTIME_MS_TO_UTT(APP_DELAY_MS)

  wake_utt = ulBSP430uptime_ni() + APP_DELAY_UTT;
  cprintf("Application wakes every %lu ms unless event occurs\n", (unsigned long)APP_DELAY_MS);
  cprintf("First wake at %s\n", xBSP430uptimeAsText_ni(wake_utt));
  while (1) {
    long rem;
    int rc;
    
    rem = lBSP430uptimeSleepUntil_ni(wake_utt, LPM1_bits);
    if (0 >= rem) {
      wake_utt += APP_DELAY_UTT;
    }
    cprintf("%s: wake with %ld remaining,", xBSP430uptimeAsText_ni(ulBSP430uptime_ni()), rem);
    cprintf(" GIE %s,", (__read_status_register() & GIE) ? "ON" : "off");
    rc = cgetchar_ni();
    if (0 <= rc) {
      cprintf(" input '");
      do {
        cputchar_ni(rc);
      } while (0 <= ((rc = cgetchar_ni())));
      cprintf("',");
    }
    cprintf(" next wake %s\n", xBSP430uptimeAsText_ni(wake_utt));
    ++iters;
    if (3 == iters) {
      cprintf("Disabling delay for 3 iters\n");
      iBSP430uptimeDelaySetEnabled_ni(0);
    } else if (6 == iters) {
      cprintf("Re-enabling delay\n");
      iBSP430uptimeDelaySetEnabled_ni(1);
    }
  }
}
