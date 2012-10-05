/** This file is in the public domain.
 *
 * Inspired by http://e2e.ti.com/support/microcontrollers/msp43016-bit_ultra-low_power_mcus/f/166/p/217914/768650.aspx
 * 
 * @homepage http://github.com/pabigot/bsp430
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph/sys.h>
#include <bsp430/periph/flash.h>
#include <string.h>

#define VECTOR_LENGTH_BYTES 128
#define VECTOR_OFFSET ((void*)0xFF80)

unsigned char ivect[VECTOR_LENGTH_BYTES];
unsigned char erased_ivect[VECTOR_LENGTH_BYTES];

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
      cputchar_ni(' ');
    }
    cprintf(" %02x", addr[i]);
  }
  cputchar_ni('\n');
}

void main ()
{
#if BSP430_MODULE_SYS - 0
  unsigned long reset_causes = 0;
  unsigned int reset_flags = 0;
#endif /* BSP430_MODULE_SYS */

#if BSP430_MODULE_SYS - 0
  {
    unsigned int sysrstiv;

    /* Record all the reset causes */
    while (0 != ((sysrstiv = uiBSP430sysSYSRSTGenerator_ni(&reset_flags)))) {
      reset_causes |= 1UL << (sysrstiv / 2);
    }
  }
#endif /* BSP430_MODULE_SYS */

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

#if BSP430_MODULE_SYS - 0
  cprintf("System reset bitmask %lx; causes:\n", reset_causes);
  {
    int bit = 0;
    while (bit < (8 * sizeof(reset_causes))) {
      if (reset_causes & (1UL << bit)) {
        cprintf("\t%s\n", xBSP430sysSYSRSTIVDescription(2*bit));
      }
      ++bit;
    }
  }

  cputtext_ni("System reset included:");
  if (reset_flags) {
    if (reset_flags & BSP430_SYS_FLAG_SYSRST_BOR) {
      cputtext_ni(" BOR");
    }
    if (reset_flags & BSP430_SYS_FLAG_SYSRST_LPM5WU) {
      cputtext_ni(" LPM5WU");
    }
    if (reset_flags & BSP430_SYS_FLAG_SYSRST_POR) {
      cputtext_ni(" POR");
    }
    if (reset_flags & BSP430_SYS_FLAG_SYSRST_PUC) {
      cputtext_ni(" PUC");
    }
  } else {
    cputtext_ni(" no data");
  }
  cputchar_ni('\n');
#endif /* BSP430_MODULE_SYS */

  memcpy(ivect, VECTOR_OFFSET, VECTOR_LENGTH_BYTES);
  dumpRegion("Cached vectors", ivect, VECTOR_LENGTH_BYTES);
  dumpRegion("Vectors", VECTOR_OFFSET, VECTOR_LENGTH_BYTES);
  (void)iBSP430flashEraseSegment_ni((void*)0xFE00);
  dumpRegion("Vectors with 0xFE00", VECTOR_OFFSET, VECTOR_LENGTH_BYTES);
  (void)iBSP430flashEraseSegment_ni(VECTOR_OFFSET);
  vBSP430ledSet(0, 1);
  dumpRegion("Vectors as erased", VECTOR_OFFSET, VECTOR_LENGTH_BYTES);
  memcpy(erased_ivect, VECTOR_OFFSET, VECTOR_LENGTH_BYTES);
  (void)iBSP430flashWriteData_ni(VECTOR_OFFSET, ivect, VECTOR_LENGTH_BYTES);
  vBSP430ledSet(1, 1);

  dumpRegion("Erased Vectors", erased_ivect, VECTOR_LENGTH_BYTES);
  dumpRegion("Restored Vectors", VECTOR_OFFSET, VECTOR_LENGTH_BYTES);
}
