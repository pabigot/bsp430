/** This file is in the public domain.
 *
 * A fairly uninteresting example for sanity-checking DAC.  It's
 * amazingly accurate (about 0.5% off); too bad it's present on only
 * one experimenter board.
 *
 * DAC12_0 output on P6.6 (H8.7) of FG4618.
 * 
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>

void main ()
{
  const unsigned int divisor = 4095;
  const unsigned int ref_mV = 2500;
  unsigned int out_dac = 0;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  /* Turn off console interrupt-driven transmission until
   * configuration complete. */
  iBSP430consoleTransmitUseInterrupts_ni(0);

  cprintf("\n\ndac " __DATE__ " " __TIME__ "\n");

#if ! (BSP430_PLATFORM_EXP430FG4618 - 0)
  cprintf("Not a supported platform.\n");
#else /* BSP430_PLATFORM_EXP430FG4618 */

  cprintf("Put your meter on P6.6 found at H8.7 on near the DE9 shell.\n");

  /* Enable REFON at 2.5V */
  ADC12CTL0 = REFON | REF2_5V;

  /* Delay 17ms to allow REF to stabilize */
  {
    unsigned long wake_utt = ulBSP430uptime_ni() + BSP430_UPTIME_MS_TO_UTT(17);
    while (0 < lBSP430uptimeSleepUntil_ni(wake_utt, LPM0_bits)) {
      /* nop */
    }
  }

  DAC12_0CTL = DAC12IR + DAC12AMP_5 + DAC12ENC; // Int ref gain 1

  cprintf("calibrating\n");
  out_dac = 0;
  DAC12_0CTL |= DAC12CALON;
  do {
    BSP430_UPTIME_DELAY_MS_NI(10, LPM0_bits, 0);
    ++out_dac;
  } while (DAC12_0CTL & DAC12CALON);
  cprintf("done calibrating %u iters\n", out_dac);
  out_dac = 0;

  /* Rest of app is normally in LPM1 and console output is done in
   * background. */
  iBSP430consoleTransmitUseInterrupts_ni(1);
  while (1) {
    unsigned long now_utt = ulBSP430uptime_ni();
    unsigned int out_mV = ((divisor / 2) + out_dac * (unsigned long)ref_mV) / divisor;
    
    DAC12_0DAT = out_dac;
    cprintf("%s: out %u/%u = %u mV\n", xBSP430uptimeAsText_ni(now_utt),
            out_dac, divisor, out_mV);

    /* NB: MSP430FG4618 will hang here if LPM disables ACLK. */
    BSP430_UPTIME_DELAY_MS_NI(5000, LPM3_bits, 0);
    out_dac = (out_dac + 0x100) & divisor;
  }
#endif /* BSP430_PLATFORM_EXP430FG4618 */
}
