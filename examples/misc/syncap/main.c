/** This file is in the public domain.
 *
 * MSP430 timers when executing in compare mode with SCS set will
 * record the value of the CCI at the time of the capture in the SCCI
 * bit.
 *
 * The documention does not say that it will do this when executing in
 * capture mode.  Since the timer supports capture on both edges, it
 * would be nice to know for sure which event caused the interrupt.
 *
 * The answer: On MSP430F5438A SCCI does not capture the value of CCI
 * that evoked an edge detection interrupt.
 *
 * Further, because the FLIH clears CCIFG in the process of
 * identifying the interrupt source it's possible that the CCR value
 * has been overwritten by the time the handler gets invoked.  See
 * some comments in capture_isr_ni() regarding this.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/periph/timer.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/console.h>

typedef struct sCapture {
  sBSP430halISRIndexedChainNode cb;
  int last_cci;
  unsigned int count;
  unsigned int errors;
  unsigned long event_tt;
} sCapture;

static int
capture_isr_ni (const struct sBSP430halISRIndexedChainNode *cb,
                void *context,
                int idx)
{
  struct sCapture * cdp = (struct sCapture *)cb;
  hBSP430halTIMER timer = (hBSP430halTIMER)context;
  unsigned int ccr;
  unsigned int cctl;

  ccr = timer->hpl->ccr[idx];
  cctl = timer->hpl->cctl[idx];
  timer->hpl->cctl[idx] &= ~COV;
  cdp->event_tt = (ulBSP430timerOverflowAdjusted_ni(timer, ccr) << 16) + ccr;
  cdp->count += 1;
  cdp->last_cci = !!(cctl & CCI);

  /* If CCIFG is set at least one capture occurred between the time
   * the FLIH processed the interrupt and the time we got here.  We
   * don't know which event the CCR came from.
   *
   * If COV is set at least one additional event occurred before the
   * FLIH processed the interrupt, or at least two additional events
   * occurred since then.
   *
   * Really, this doesn't work at all well if the signal needs to be
   * debounced. */
  cdp->errors += !! (cctl & (COV | CCIFG));
  return 0;
}

static sCapture captured = {
  .cb = { .callback_ni = capture_isr_ni }
};

static int bitToPin (int v)
{
  int pin = 0;
  unsigned int bit = 1;
  while (bit && !(v & bit)) {
    ++pin;
    bit <<= 1;
  }
  return bit ? pin : -1;
}

#define PORT_CCIDX 1

void main ()
{
  sBSP430halPORT * port_hal;
  hBSP430halTIMER hal;
  volatile sBSP430hplTIMER * hpl;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();
  cputchar('\n');
  BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 1000);

  cprintf("\n\nsynccap " __DATE__ " " __TIME__ "\n");
  cprintf("External input on %s.%u corresponds to %s.%u%c\n",
          xBSP430portName(BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE),
          bitToPin(BSP430_TIMER_CCACLK_CC1_PORT_BIT),
          xBSP430timerName(BSP430_TIMER_CCACLK_PERIPH_HANDLE),
          PORT_CCIDX,
          'A' + (BSP430_TIMER_CCACLK_CC1_CCIS / CCIS0));
  port_hal = hBSP430portLookup(BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE);
  if (NULL == port_hal) {
    cprintf("Port HAL not available\n");
    return;
  }
  hal = hBSP430timerLookup(BSP430_TIMER_CCACLK_PERIPH_HANDLE);
  if (NULL == hal) {
    cprintf("Can't find timer\n");
    return;
  }

  BSP430_PORT_HAL_HPL_DIR(port_hal) &= ~BSP430_TIMER_CCACLK_CC1_PORT_BIT;
  BSP430_PORT_HAL_HPL_SEL(port_hal) |= BSP430_TIMER_CCACLK_CC1_PORT_BIT;
  hpl = hal->hpl;
  hpl->cctl[1] = CM_3 | BSP430_TIMER_CCACLK_CC1_CCIS | SCS | CAP | CCIE;
  captured.last_cci = !!(hpl->cctl[1] & CCI);
  BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRIndexedChainNode,
                                  hal->cc_cbchain_ni[PORT_CCIDX],
                                  captured.cb,
                                  next_ni);
  hpl->ctl = TASSEL_2 | MC_2 | TACLR | TAIE;

  /* Need to enable interrupts so timer overflow events are properly
   * acknowledged */
  while (1) {
    unsigned int ccr;
    unsigned int cctl;
    unsigned int count;
    unsigned int errors;
    int last_cci;
    unsigned long event_tt;

    BSP430_CORE_DISABLE_INTERRUPT();
    do {
      event_tt = captured.event_tt;
      captured.event_tt = 0;
      count = captured.count;
      errors = captured.errors;
      captured.count = 0;
      captured.errors = 0;
      last_cci = captured.last_cci;
      cctl = hpl->cctl[PORT_CCIDX];
      ccr = hpl->ccr[PORT_CCIDX];
      hpl->cctl[PORT_CCIDX] &= ~COV;
    } while (0);
    BSP430_CORE_ENABLE_INTERRUPT();

    cprintf("Up %lu ACLK ticks, timer %lu event %lx ccr %x cctl %04x\n"
            "\tCCI %d SCCI %d COV %d ; recorded CCI %d ; count %u errors %d\n",
            ulBSP430uptime(),
            ulBSP430timerCounter(hal, NULL),
            event_tt,
            ccr,
            cctl,
            !!(cctl & CCI), !!(cctl & SCCI), !!(cctl & COV), last_cci, count, errors);
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ);
  }
}
