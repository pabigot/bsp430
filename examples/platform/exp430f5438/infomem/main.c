/** This file is in the public domain.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/periph/flash.h>
#include <bsp430/periph/timer.h>
#include <inttypes.h>
#include <sys/crtld.h>

#define CYCLE_COUNTER() (BSP430_HPL_TA1->r)

void dumpRegion (const char * header,
                 const unsigned char * addr,
                 size_t len)
{
  size_t i;

  cprintf("%s:", header);
  for (i = 0; i < len; ++i) {
    if (0 == (i % 16)) {
      cprintf("\n%p", addr+i);
    }
    if (0 == (i % 8)) {
      cputchar(' ');
    }
    cprintf(" %02x", addr[i]);
  }
  cputchar('\n');
}

char dummy[128];

void main ()
{
  unsigned int ct[5];
  int i;
  unsigned int * ctp;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  ctp = ct;
  BSP430_HPL_TA1->ctl = TASSEL_2 | MC_2 | TACLR;
  *ctp++ = CYCLE_COUNTER();
  cprintf("Cycle timer %lu Hz\n", ulBSP430timerFrequency_Hz_ni(BSP430_PERIPH_TA1));
  *ctp++ = CYCLE_COUNTER();
  cprintf("Last printf took %u..%u = %u ticks\n", ct[0], ct[1], ct[1]-ct[0]);

  cprintf("\nInformation memory segments are %u bytes long\n", __info_segment_size);
  cprintf("FCTL: %04x ---- %04x %04x\n", FCTL1, FCTL3, FCTL4);

  for (i = 0; i < __info_segment_size; ++i) {
    dummy[i] = 0x80 + i;
  }

  ctp = ct;
  *ctp++ = CYCLE_COUNTER();
  iBSP430flashEraseSegment_ni(__infob);
  *ctp++ = CYCLE_COUNTER();
  cprintf("Erase segment took %u cycles\n", ct[1] - ct[0]);

  ctp = ct;
  *ctp++ = CYCLE_COUNTER();
  iBSP430flashWriteData_ni(__infob, dummy, __info_segment_size);
  *ctp++ = CYCLE_COUNTER();
  cprintf("Write segment took %u cycles\n", ct[1] - ct[0]);

  dumpRegion("INFOA", __infoa, __info_segment_size);
  dumpRegion("INFOB", __infob, __info_segment_size);

  iBSP430flashEraseSegment_ni(__infob);

}
