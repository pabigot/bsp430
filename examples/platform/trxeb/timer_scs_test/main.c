/** This file is in the public domain.
 *
 * This code was used to determine the behavior of the #SCS bit in 5xx
 * Timer_B implementations when the timer clock is asynchronous to
 * MCLK.  The take-away is that SCS means a capture will occur on the
 * next falling edge of the timer clock, even if the input signal has
 * returned to its normal state beforehahd.  Where the timer clock is
 * asynchronous to and slower than MCLK this means toggling CCIS0 when
 * CCIS1 is set will not necessarily load the capture register with
 * the current counter value, as is suggested in the user's guide.
 *
 * See http://e2e.ti.com/support/microcontrollers/msp43016-bit_ultra-low_power_mcus/f/166/t/236172.aspx
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/unittest.h>
#include <bsp430/utility/console.h>
#include <bsp430/periph/timer.h>
#include <bsp430/periph/port.h>

#define HALF_TICK_OUTCLK() do {                                         \
    BSP430_PORT_HAL_HPL_OUT(outclk_port_hal) ^= APP_OUTCLK_PORT_BIT;    \
    outclk += !! (BSP430_PORT_HAL_HPL_OUT(outclk_port_hal) & APP_OUTCLK_PORT_BIT); \
  } while (0)

#define HALF_TRIGGER() do {                                             \
    BSP430_PORT_HAL_HPL_OUT(trigger_port_hal) ^= APP_TRIGGER_PORT_BIT;  \
  } while (0)

#define INTERNAL_HALF_TRIGGER() do {            \
    timer_hpl->cctl[APP_CCIDX] ^= CCIS0;                  \
  } while (0)

void main ()
{
  volatile sBSP430hplTIMER * const timer_hpl = xBSP430hplLookupTIMER(BSP430_TIMER_CCACLK_PERIPH_HANDLE);
  unsigned int outclk;
  unsigned int last_outclk;
  unsigned int cctl;
  hBSP430halPORT const inclk_port_hal = hBSP430portLookup(BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE);
  hBSP430halPORT const cc_port_hal = hBSP430portLookup(APP_CC_PORT_PERIPH_HANDLE);
  hBSP430halPORT const outclk_port_hal = hBSP430portLookup(APP_OUTCLK_PORT_PERIPH_HANDLE);
  hBSP430halPORT const trigger_port_hal = hBSP430portLookup(APP_TRIGGER_PORT_PERIPH_HANDLE);
  vBSP430platformInitialize_ni();
  vBSP430unittestInitialize();

  cprintf("timer_scs_test: " __DATE__ " " __TIME__ "\n");
  if (NULL == timer_hpl) {
    cprintf("ERROR: Unable to get timer HPL access\n");
    return;
  }
  if (NULL == inclk_port_hal) {
    cprintf("ERROR: Unable to get INCLK port access\n");
    return;
  }
  if (NULL == cc_port_hal) {
    cprintf("ERROR: Unable to get CC0 port access\n");
    return;
  }
  if (NULL == outclk_port_hal) {
    cprintf("ERROR: Unable to get OUTCLK port access\n");
    return;
  }
  if (NULL == trigger_port_hal) {
    cprintf("ERROR: Unable to get TRIGGER port access\n");
    return;
  }
  cprintf("Please connect OUTCLK at %s.%u to %s INCLK at %s.%u\n",
          xBSP430portName(APP_OUTCLK_PORT_PERIPH_HANDLE),
          iBSP430portBitPosition(APP_OUTCLK_PORT_BIT),
          xBSP430timerName(BSP430_TIMER_CCACLK_PERIPH_HANDLE),
          xBSP430portName(BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE),
          iBSP430portBitPosition(BSP430_TIMER_CCACLK_CLK_PORT_BIT));
  cprintf("Please connect TRIGGER at %s.%u to %s CCI0%c at %s.%u\n",
          xBSP430portName(APP_TRIGGER_PORT_PERIPH_HANDLE),
          iBSP430portBitPosition(APP_TRIGGER_PORT_BIT),
          xBSP430timerName(BSP430_TIMER_CCACLK_PERIPH_HANDLE),
          'A' + (APP_CC_CCIS / CCIS0),
          xBSP430portName(APP_CC_PORT_PERIPH_HANDLE),
          iBSP430portBitPosition(APP_CC_PORT_BIT));

  /* OUTCLK: output low */
  outclk = 0;
  BSP430_PORT_HAL_HPL_OUT(outclk_port_hal) &= ~APP_OUTCLK_PORT_BIT;
  BSP430_PORT_HAL_HPL_DIR(outclk_port_hal) |= APP_OUTCLK_PORT_BIT;
  BSP430_PORT_HAL_HPL_SEL(outclk_port_hal) &= ~APP_OUTCLK_PORT_BIT;

  /* TRIGGER: output low */
  BSP430_PORT_HAL_HPL_OUT(trigger_port_hal) &= ~APP_TRIGGER_PORT_BIT;
  BSP430_PORT_HAL_HPL_DIR(trigger_port_hal) |= APP_TRIGGER_PORT_BIT;
  BSP430_PORT_HAL_HPL_SEL(trigger_port_hal) &= ~APP_TRIGGER_PORT_BIT;

  /* INCLK: input peripheral function */
  BSP430_PORT_HAL_HPL_SEL(inclk_port_hal) &= ~BSP430_TIMER_CCACLK_CLK_PORT_BIT;
  BSP430_PORT_HAL_HPL_SEL(inclk_port_hal) |= BSP430_TIMER_CCACLK_CLK_PORT_BIT;

  /* CC0: input peripheral function */
  BSP430_PORT_HAL_HPL_DIR(cc_port_hal) &= ~APP_CC_PORT_BIT;
  BSP430_PORT_HAL_HPL_SEL(cc_port_hal) |= APP_CC_PORT_BIT;
  cprintf("P4: DIR %04x SEL %04x\n", P4DIR, P4SEL);

#define CLEAR_CAPTURE() do {                    \
    timer_hpl->cctl[APP_CCIDX] &= ~CCIFG;                 \
    timer_hpl->ccr[APP_CCIDX] = 0;                        \
  } while (0)

#define ASSERT_CLOCK_HIGH() do {                                        \
    BSP430_UNITTEST_ASSERT_TRUE(BSP430_PORT_HAL_HPL_OUT(outclk_port_hal) & APP_OUTCLK_PORT_BIT); \
  } while (0)

#define ASSERT_CLOCK_LOW() do {                                         \
    BSP430_UNITTEST_ASSERT_FALSE(BSP430_PORT_HAL_HPL_OUT(outclk_port_hal) & APP_OUTCLK_PORT_BIT); \
  } while (0)

#define ASSERT_CCI_HIGH() do {                              \
    BSP430_UNITTEST_ASSERT_TRUE(CCI & timer_hpl->cctl[APP_CCIDX]);        \
  } while (0)

#define ASSERT_CCI_LOW() do {                                \
    BSP430_UNITTEST_ASSERT_FALSE(CCI & timer_hpl->cctl[APP_CCIDX]);        \
  } while (0)

#define ASSERT_INTERNAL_TRIGGER_HIGH() do {                             \
    BSP430_UNITTEST_ASSERT_TRUE(CCIS_3 == (CCIS_3 & timer_hpl->cctl[APP_CCIDX])); \
  } while (0)

#define ASSERT_INTERNAL_TRIGGER_LOW() do {                              \
    BSP430_UNITTEST_ASSERT_TRUE(CCIS_2 == (CCIS_3 & timer_hpl->cctl[APP_CCIDX])); \
  } while (0)

#define ASSERT_NO_CAPTURE() do {                                \
    BSP430_UNITTEST_ASSERT_FALSE(CCIFG & timer_hpl->cctl[APP_CCIDX]);     \
    BSP430_UNITTEST_ASSERT_EQUAL_FMTx(0, timer_hpl->ccr[APP_CCIDX]);      \
  } while (0)

#define ASSERT_YES_CAPTURE() do {                                       \
    BSP430_UNITTEST_ASSERT_TRUE(CCIFG & timer_hpl->cctl[APP_CCIDX]);              \
    BSP430_UNITTEST_ASSERT_EQUAL_FMTx(timer_hpl->r, timer_hpl->ccr[APP_CCIDX]);   \
  } while (0)

  /* Capture asynchronously on falling edge from TRIGGER. */
  timer_hpl->ccr[APP_CCIDX] = 0;
  timer_hpl->cctl[APP_CCIDX] = CM_2 | APP_CC_CCIS | SCCI | CAP;
  timer_hpl->cctl[APP_CCIDX] = CM_3 | APP_CC_CCIS | SCCI | CAP;
  cprintf("%u: Timer: R %04x CTL %04x CCR %04x CCTL %04x\n", __LINE__, timer_hpl->r, timer_hpl->ctl, timer_hpl->ccr[APP_CCIDX], timer_hpl->cctl[APP_CCIDX]);

  /* Count upwards. */
  timer_hpl->ctl = TASSEL_0 | MC_2 | TACLR;

  /* Validate that we can control the clock input. */
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, timer_hpl->r);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, 0);
  ASSERT_CLOCK_LOW();
  HALF_TICK_OUTCLK();
  ASSERT_CLOCK_HIGH();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, 1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, timer_hpl->r);
  HALF_TICK_OUTCLK();
  ASSERT_CLOCK_LOW();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, timer_hpl->r);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, 1);
  HALF_TICK_OUTCLK();
  ASSERT_CLOCK_HIGH();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, timer_hpl->r);
  HALF_TICK_OUTCLK();
  ASSERT_CLOCK_LOW();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, timer_hpl->r);

  /* Verify that we can control the trigger, and that asynchronous
   * captures work. */
  ASSERT_NO_CAPTURE();
  ASSERT_CCI_LOW();
  HALF_TRIGGER();
  if (CM0 & timer_hpl->cctl[APP_CCIDX]) {
    ASSERT_YES_CAPTURE();
    if (! (CM1 & timer_hpl->cctl[APP_CCIDX])) {
      CLEAR_CAPTURE();
    }
  } else {
    ASSERT_NO_CAPTURE();
  }
  ASSERT_CCI_HIGH();
  HALF_TRIGGER();
  ASSERT_YES_CAPTURE();
  ASSERT_CCI_LOW();
  CLEAR_CAPTURE();

  /* Advance the clock.  Make sure no capture occurred. */
  last_outclk = outclk;
  HALF_TICK_OUTCLK();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, last_outclk+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, timer_hpl->r);
  ASSERT_NO_CAPTURE();
  last_outclk = outclk;
  HALF_TICK_OUTCLK();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(last_outclk, timer_hpl->r);
  ASSERT_NO_CAPTURE();

  /* Now turn on synchronous capture */
  timer_hpl->cctl[APP_CCIDX] |= SCS;

  /* Advance the clock.  Make sure no capture occurred */
  cctl = timer_hpl->cctl[APP_CCIDX];
  last_outclk = outclk;
  ASSERT_CLOCK_LOW();
  HALF_TICK_OUTCLK();
  ASSERT_CLOCK_HIGH();
  ASSERT_NO_CAPTURE();
  HALF_TICK_OUTCLK();
  ASSERT_CLOCK_LOW();
  ASSERT_NO_CAPTURE();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(last_outclk+1, timer_hpl->r);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(cctl, timer_hpl->cctl[APP_CCIDX]);

  /* Do this again to make really sure no capture occurred. */
  HALF_TICK_OUTCLK();
  ASSERT_NO_CAPTURE();
  HALF_TICK_OUTCLK();
  ASSERT_NO_CAPTURE();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(last_outclk+2, timer_hpl->r);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(cctl, timer_hpl->cctl[APP_CCIDX]);

  /*
  F1) CCI rises then falls while CLK remains low.  CCIFG remains low.  When CLK
  rises the counter is incremented but CCIFG remains low.  CCIFG is set only
  when CLK falls.
  */

  /* CLK is low. */
  ASSERT_CLOCK_LOW();

  /* Check that external triggers with sync async do not trigger when
   * clock does not advance. */
  ASSERT_NO_CAPTURE();
  ASSERT_CCI_LOW();
  HALF_TRIGGER();
  ASSERT_NO_CAPTURE();
  ASSERT_CCI_HIGH();
  HALF_TRIGGER();
  ASSERT_NO_CAPTURE();
  ASSERT_CCI_LOW();

  /* Advance the clock.  No action on rising edge. */
  last_outclk = outclk;
  HALF_TICK_OUTCLK();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, last_outclk+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, timer_hpl->r);
  ASSERT_NO_CAPTURE();
  ASSERT_CCI_LOW();
  ASSERT_CLOCK_HIGH();

  /* Falling edge does not increment counter but does cause capture */
  last_outclk = outclk;
  HALF_TICK_OUTCLK();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, last_outclk);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, timer_hpl->r);
  ASSERT_YES_CAPTURE();
  ASSERT_CCI_LOW();
  CLEAR_CAPTURE();

  /*
  F2) CCI rises then falls while CLK remains high.  CCIFG remains low.  When
  CLK falls CCIFG is set.
  */

  /* Advance the clock.  No action on rising edge. */
  last_outclk = outclk;
  HALF_TICK_OUTCLK();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, last_outclk+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, timer_hpl->r);
  ASSERT_NO_CAPTURE();
  ASSERT_CCI_LOW();
  ASSERT_CLOCK_HIGH();

  /* Check that external triggers with sync async do not trigger when
   * clock does not advance. */
  ASSERT_NO_CAPTURE();
  ASSERT_CCI_LOW();
  HALF_TRIGGER();
  ASSERT_NO_CAPTURE();
  ASSERT_CCI_HIGH();
  HALF_TRIGGER();
  ASSERT_NO_CAPTURE();
  ASSERT_CCI_LOW();

  /* Falling edge does not increment counter but does cause capture */
  last_outclk = outclk;
  HALF_TICK_OUTCLK();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, last_outclk);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, timer_hpl->r);
  ASSERT_YES_CAPTURE();
  ASSERT_CCI_LOW();
  CLEAR_CAPTURE();

  /* Now test it with internal toggles.  First, asynchronous: */
  timer_hpl->cctl[APP_CCIDX] = CM_3 | CCIS_2 | SCCI | CAP;
  ASSERT_NO_CAPTURE();
  ASSERT_INTERNAL_TRIGGER_LOW();
  ASSERT_CCI_LOW();
  HALF_TRIGGER();
  ASSERT_CCI_LOW();
  ASSERT_NO_CAPTURE();
  INTERNAL_HALF_TRIGGER();
  ASSERT_CCI_HIGH();
  ASSERT_INTERNAL_TRIGGER_HIGH();
  ASSERT_YES_CAPTURE();
  CLEAR_CAPTURE();
  HALF_TRIGGER();
  ASSERT_CCI_HIGH();
  ASSERT_NO_CAPTURE();
  INTERNAL_HALF_TRIGGER();
  ASSERT_CCI_LOW();
  ASSERT_INTERNAL_TRIGGER_LOW();
  ASSERT_YES_CAPTURE();
  ASSERT_CCI_LOW();
  CLEAR_CAPTURE();

  /* Now synchronous */
  timer_hpl->cctl[APP_CCIDX] |= SCS;

  ASSERT_NO_CAPTURE();
  ASSERT_INTERNAL_TRIGGER_LOW();
  INTERNAL_HALF_TRIGGER();
  ASSERT_INTERNAL_TRIGGER_HIGH();
  ASSERT_NO_CAPTURE();
  INTERNAL_HALF_TRIGGER();
  ASSERT_INTERNAL_TRIGGER_LOW();
  ASSERT_NO_CAPTURE();

  /* Advance the clock.  No action on rising edge. */
  last_outclk = outclk;
  HALF_TICK_OUTCLK();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, last_outclk+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, timer_hpl->r);
  ASSERT_NO_CAPTURE();
  ASSERT_CCI_LOW();
  ASSERT_CLOCK_HIGH();

  /* Falling edge does not increment counter but does cause capture */
  last_outclk = outclk;
  HALF_TICK_OUTCLK();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, last_outclk);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(outclk, timer_hpl->r);
  ASSERT_YES_CAPTURE();
  ASSERT_CCI_LOW();
  CLEAR_CAPTURE();

  cprintf("%u: Timer: R %04x CTL %04x CCR %04x CCTL %04x\n", __LINE__, timer_hpl->r, timer_hpl->ctl, timer_hpl->ccr[APP_CCIDX], timer_hpl->cctl[APP_CCIDX]);
  vBSP430unittestFinalize();
}
