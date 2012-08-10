/** This file is in the public domain.
 *
 * LaunchPad board revisions prior to 1.5: Remove the TXD, RXD, and
 * TEST jumpers connecting the emulation side of the board from the
 * EXP430G2 side of the board (you can leave or remove RST), and
 * measure across Vcc.  Even with this, you will not get LPM4 current
 * usage below 76 uA when powering from the emulation USB port.  If
 * you completely disconnect the emulation side and power from
 * battery, you can get down to 65 uA on this board.
 *
 * Rev 1.5: 125 nA when powered by battery.  The TXD/RXD and TEST
 * jumpers may be present in this mode, but RST and VCC should be
 * removed.  97 nA when powered by USB.
 *
 * EXP430G2r1.5: Active=434; L0=85; L1=85; L2=28; L3=5.4; L4=?
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/lpm.h>
#include <bsp430/utility/led.h>

static const unsigned int lpm_bits[] = {
  LPM0_bits,
  LPM1_bits,
  LPM2_bits,
  LPM3_bits,
  LPM4_bits
};

void main ()
{
  WDTCTL = WDTPW | WDTHOLD;

#if BSP430_LED - 0
  vBSP430ledInitialize_ni();
  {
    int i;

    vBSP430ledSet(0, 1);
    for (i = 0; i < 20; ++i) {
      vBSP430ledSet(0, -1);
      vBSP430ledSet(1, -1);
      __delay_cycles(BSP430_CLOCK_PUC_MCLK_HZ / 10);
    }
    /* The subsequent vBSP430lpmConfigurePortsForLPM_ni() call should
     * turn off the LED that's still lit. */
  }
#endif

  vBSP430lpmConfigurePortsForLPM_ni();

  __delay_cycles(BSP430_CLOCK_PUC_MCLK_HZ);
  while (1) {
#if (0 <= APP_LPM) && (APP_LPM <= 4)
    __bis_status_register(GIE | lpm_bits[APP_LPM]);
#endif
  }
}
