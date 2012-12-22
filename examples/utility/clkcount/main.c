/** This file is in the public domain.
 *
 * This example assumes a 1PPS signal is provided on CC0 on the CCACLK
 * infrastructure.  It uses this to measure the actual frequency of
 * ACLK over several windows.  It can be used to assess VLOCLK and
 * REFOCLK accuracy, as well as validate the capacitance setting for
 * LFXT.
 *
 * Options:
 *
 * APP_USE_SMCLK : Define to true to use SMCLK in the capture timer.
 * Default false uses ACLK.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/periph/timer.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/led.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <stdlib.h>
#include <string.h>

#ifndef HISTORY_EXP
#define HISTORY_EXP 4
#endif /* HISTORY_EXP */

#define MAX_SAMPLES (1 << (HISTORY_EXP))
#define SAMPLES_MASK (MAX_SAMPLES - 1)

unsigned long sums[HISTORY_EXP];
unsigned long samples[MAX_SAMPLES];

struct sCaptures {
  sBSP430halISRIndexedChainNode cb;
  unsigned long last_count;
  volatile unsigned long count;
  unsigned int nsamples;
};

static int
capture_isr_ni (const struct sBSP430halISRIndexedChainNode * cb,
                void * context,
                int idx)
{
  struct sCaptures * cp = (struct sCaptures *)cb;
  hBSP430halTIMER timer = (hBSP430halTIMER)context;
  unsigned long now_tt = ulBSP430timerCounter_ni(timer, NULL);

  cp->count = now_tt + ((int)timer->hpl->ccr[idx] - (unsigned int)now_tt);
  cp->nsamples += 1;
  return BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
}

static struct sCaptures captures = {
  .cb = { .callback = capture_isr_ni },
};

/* Simulate floating-point output */
#define EMIT_SCALED(_s,_n) do {                         \
    static const int MULTIPLIER = 100;                  \
    unsigned long whole = (MULTIPLIER * (_s)) / (_n);   \
    unsigned int frac = whole % MULTIPLIER;             \
    whole /= MULTIPLIER;                                \
    cprintf(" %7lu.%02u", whole, frac);                 \
  } while (0)

void main ()
{
  hBSP430halTIMER timer;
  hBSP430halPORT h_cc0;
  unsigned long nominal_Hz;
  unsigned int tassel;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cprintf("\nclkcounter " __DATE__ " " __TIME__ "\n");
  cprintf("BSP430_CLOCK_LFXT1_IS_FAULTED_NI(): %u\n", !!BSP430_CLOCK_LFXT1_IS_FAULTED_NI());
#if APP_USE_SMCLK - 0
  nominal_Hz = ulBSP430clockSMCLK_Hz_ni();
  cprintf("Expected SMCLK clock freq: %lu Hz\n", nominal_Hz);
  tassel = TASSEL_2;
#else
  nominal_Hz = ulBSP430clockACLK_Hz_ni();
  cprintf("Expected ACLK clock freq: %lu Hz\n", nominal_Hz);
  tassel = TASSEL_1;
#endif

  timer = hBSP430timerLookup(BSP430_TIMER_CCACLK_PERIPH_HANDLE);
  if (! timer) {
    cprintf("ERROR: CCACLK HAL not available\n");
    return;
  }
  h_cc0 = hBSP430portLookup(BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE);
  if (! h_cc0) {
    cprintf("ERROR: CC0 port HAL not available\n");
    return;
  }

  cprintf("Timing uses %s\n", xBSP430timerName(BSP430_TIMER_CCACLK_PERIPH_HANDLE) ?: "T??");
  cprintf("CC0 uses %s.%u\n",
          xBSP430portName(BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE) ?: "P?",
          iBSP430portBitPosition(BSP430_TIMER_CCACLK_CC0_PORT_BIT));
  if (0 == iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_EXPOSED_CLOCKS, 0, 1)) {
    cprintf("Clock signals: %s\n", xBSP430platformPeripheralHelp(BSP430_PERIPH_EXPOSED_CLOCKS, 0));
  }

  /* Set CC0 port for input as CC0 trigger, with weak pull-down */
  BSP430_PORT_HAL_HPL_DIR(h_cc0) &= ~BSP430_TIMER_CCACLK_CC0_PORT_BIT;
#if BSP430_PORT_SUPPORTS_REN
  BSP430_PORT_HAL_HPL_OUT(h_cc0) &= ~BSP430_TIMER_CCACLK_CC0_PORT_BIT;
  BSP430_PORT_HAL_HPL_REN(h_cc0) |= BSP430_TIMER_CCACLK_CC0_PORT_BIT;
#endif /* BSP430_PORT_SUPPORTS_REN */
  BSP430_PORT_HAL_HPL_SEL(h_cc0) |= BSP430_TIMER_CCACLK_CC0_PORT_BIT;
  P4SEL |= BSP430_TIMER_CCACLK_CC0_PORT_BIT;

  /* Chain in interrupt handler */
  captures.cb.next_ni = timer->cc_cbchain_ni[0];
  timer->cc_cbchain_ni[0] = &captures.cb;

  /* Capture synchronously on rising edge and interrupt */
  timer->hpl->cctl[0] = BSP430_TIMER_CCACLK_CC0_CCIS | CM_1 | SCS | CAP | CCIE;

  /* Configure timer to run continuously off selected clock */
  timer->hpl->ctl = 0;
  vBSP430timerResetCounter_ni(timer);
  timer->hpl->ctl = tassel | MC_2 | TACLR | TAIE;

  BSP430_CORE_LPM_ENTER_NI(LPM0_bits | GIE);
  memset(sums, 0, sizeof(sums));
  memset(samples, 0, sizeof(samples));
  captures.last_count = captures.count;
  captures.nsamples = 0;
  while (1) {
    unsigned int nsamples;
    unsigned long delta;
    long err;
    unsigned long ppm;
    int i;
    int si;
    
    BSP430_CORE_LPM_ENTER_NI(LPM0_bits | GIE);
    delta = captures.count - captures.last_count;
    captures.last_count = captures.count;
    nsamples = captures.nsamples;
    BSP430_CORE_ENABLE_INTERRUPT();
    err = delta - nominal_Hz;
    if (0 > err) {
      err = -err;
    }
    /* Use calculation that accounts for potential overflow based on
     * nominal frequency */
    if (100000UL > nominal_Hz) {
      ppm = (err * 10000 / (nominal_Hz / 100));
    } else {
      ppm = (1000 * err) / (nominal_Hz / 1000);
    }
    cprintf("%4u %-7lu (err %lu ppm) : ", nsamples, delta, ppm);
    si = (nsamples - 1) & SAMPLES_MASK;
    for (i = 0; i < HISTORY_EXP; ++i) {
      int sps = (1 << (i+1));
      sums[i] += delta;
      if (nsamples >= sps) {
        unsigned int di = (MAX_SAMPLES + si - sps) & SAMPLES_MASK;
        sums[i] -= samples[di];
        EMIT_SCALED(sums[i], sps);
      }
    }
    samples[si] = delta;
    cputchar('\n');
  }
}
