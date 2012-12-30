/** This file is in the public domain.
 *
 * Basic demonstration of the BSP430 DMA interface.  Shows memcpy and
 * memset about 2-3x faster than code, and UART transmission about 10x
 * faster than polling.
 * 
 * @homepage http://github.com/pabigot/bsp430
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/periph/dma.h>
#include <bsp430/utility/led.h>
#include <string.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 2048
#endif /* BUFFER_SIZE */

uint8_t buffer[BUFFER_SIZE];
uint8_t src8 = 1;
uint16_t src16 = 1;
const char message[] = "This is text that should be transmitted over the console\r\n";

struct sChannel {
  sBSP430halISRIndexedChainNode cb;
  int stage;
  unsigned int t0;
  unsigned int t1;
};

static int
dma_isr_ni (const struct sBSP430halISRIndexedChainNode * cb,
            void * context,
            int idx)
{
  struct sChannel * statep = (struct sChannel *)cb;
  hBSP430halDMA dma = (hBSP430halDMA)context;
  volatile sBSP430hplDMAchannel * chp = dma->hpl->ch + idx;
  int rv = 0;

  if (0 == statep->stage) {
    statep->t0 = uiBSP430timerSafeCounterRead_ni(xBSP430hplLookupTIMER(BSP430_TIMER_CCACLK_PERIPH_HANDLE));
    chp->ctl |= DMAEN;
  } else {
    statep->t1 = uiBSP430timerSafeCounterRead_ni(xBSP430hplLookupTIMER(BSP430_TIMER_CCACLK_PERIPH_HANDLE));
    chp->ctl &= ~DMAEN;
    rv = BSP430_HAL_ISR_CALLBACK_EXIT_LPM | BSP430_HAL_ISR_CALLBACK_EXIT_CLEAR_GIE;
  }
  ++statep->stage;
  return rv;
}

static struct sChannel channel0 = {
  .cb = { .callback = dma_isr_ni },
};

void main ()
{
  volatile sBSP430hplTIMER * const hrt = xBSP430hplLookupTIMER(BSP430_TIMER_CCACLK_PERIPH_HANDLE);
  volatile sBSP430hplDMAchannel * chp = BSP430_HPL_DMA->ch + 0;
  unsigned int t0;
  unsigned int t1;
  unsigned int timing_overhead;
  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cprintf("\n\ndma " __DATE__ " " __TIME__ "\n");

  /* Configure a timer off SMCLK to measure the performance of
   * different actions. */
  if (NULL == hrt) {
    cprintf("High-resolution timer not available\n");
    return;
  }
  vBSP430timerSafeCounterInitialize_ni(BSP430_TIMER_CCACLK_PERIPH_HANDLE);
  hrt->ctl = TASSEL_2 | MC_2 | TACLR;
  t0 = uiBSP430timerSafeCounterRead_ni(hrt);
  t1 = uiBSP430timerSafeCounterRead_ni(hrt);
  timing_overhead = t1 - t0;
  cprintf("Timing with SMCLK at %lu Hz, nominal timing overhead %u ticks\n", ulBSP430timerFrequency_Hz_ni(BSP430_TIMER_CCACLK_PERIPH_HANDLE), timing_overhead);

  /* Hook the DMA interrupt in, though we don't use it until the last
   * step. */
  channel0.cb.next_ni = BSP430_HAL_DMA->ch_cbchain_ni[0];
  BSP430_HAL_DMA->ch_cbchain_ni[0] = &channel0.cb;

#if defined(DMARMWDIS)
  /* Good practice where this is available, avoids corrupted data */
  BSP430_HPL_DMA->ctl4 = DMARMWDIS;
#endif /* DMARMWDIS */

  /* Dump some basic state */
  chp->ctl = 0;
  chp->da = 0;
  chp->sa = 0;
  chp->sz = 0;
  cprintf("Clear DMA0: CTL %04x ; SA %04x ; DA %04x ; SZ %04x\n",
          chp->ctl, (unsigned int)chp->sa, (unsigned int)chp->da, chp->sz);
  cprintf("Buffer %u bytes at %p, src8 at %p, src16 at %p\n",
          sizeof(buffer), buffer, &src8, &src16);
  cprintf("Initial %u .. %u\n", buffer[0], buffer[sizeof(buffer)-1]);

  /* Time zeroing an array using memset */
  t0 = uiBSP430timerSafeCounterRead_ni(hrt);
  memset(buffer, src8, sizeof(buffer));
  t1 = uiBSP430timerSafeCounterRead_ni(hrt);
  cprintf("memset %u bytes took %u: %u .. %u\n", sizeof(buffer),
          t1 - t0, buffer[0], buffer[sizeof(buffer)-1]);

  chp->ctl = DMADT_1 | DMADSTINCR_3 | DMADSTBYTE | DMASRCBYTE;
  chp->da = (uintptr_t)buffer;
  chp->sa = (uintptr_t)&src8;
  chp->sz = sizeof(buffer);
  chp->ctl |= DMAEN;
  cprintf("DMA0: CTL %04x ; SA %04x ; DA %04x ; SZ %04x\n",
          chp->ctl, (unsigned int)chp->sa, (unsigned int)chp->da, chp->sz);
  ++src8;
  t0 = uiBSP430timerSafeCounterRead_ni(hrt);
  chp->ctl |= DMAREQ;
  t1 = uiBSP430timerSafeCounterRead_ni(hrt);
  chp->ctl &= ~DMAEN;
  cprintf("DMA init %u bytes from %u took %u: %u .. %u\n", sizeof(buffer), src8,
          t1 - t0, buffer[0], buffer[sizeof(buffer)-1]);

  ++src8;
  src16 = (src8 << 8) | src8;
  chp->ctl = DMADT_1 | DMADSTINCR_3;
  chp->sa = (uintptr_t)&src16;
  chp->sz = sizeof(buffer)/sizeof(src16);
  chp->ctl |= DMAEN;

  cprintf("DMA0: CTL %04x ; SA %04x ; DA %04x ; SZ %04x\n",
          chp->ctl, (unsigned int)chp->sa, (unsigned int)chp->da, chp->sz);
  t0 = uiBSP430timerSafeCounterRead_ni(hrt);
  chp->ctl |= DMAREQ;
  t1 = uiBSP430timerSafeCounterRead_ni(hrt);
  chp->ctl &= ~DMAEN;

  cprintf("DMA init %u words from %04x took %u: %u .. %u\n", sizeof(buffer), src16,
          t1 - t0, buffer[0], buffer[sizeof(buffer)-1]);

  chp->ctl = DMADT_1 | DMASRCINCR_3 | DMADSTINCR_3 | DMADSTBYTE | DMASRCBYTE;
  chp->sa = (uintptr_t)message;
  memset(buffer, 0, sizeof(buffer));
  chp->sz = strlen(message);
  chp->ctl |= DMAEN;

  cprintf("DMA0: CTL %04x ; SA %04x ; DA %04x ; SZ %04x\n",
          chp->ctl, (unsigned int)chp->sa, (unsigned int)chp->da, chp->sz);

  t0 = uiBSP430timerSafeCounterRead_ni(hrt);
  chp->ctl |= DMAREQ;
  t1 = uiBSP430timerSafeCounterRead_ni(hrt);
  chp->ctl &= ~DMAEN;
  buffer[chp->sz] = 0;
  cprintf("Buffer copy done in %u, result:\n\t%s\n", t1 - t0, buffer);

  cprintf("Writing %u bytes through console interface:\n\t", chp->sz);
  iBSP430consoleFlush();
  t0 = uiBSP430timerSafeCounterRead_ni(hrt);
  cputtext_ni(message);
  iBSP430consoleFlush();
  t1 = uiBSP430timerSafeCounterRead_ni(hrt);
  cprintf("console output took %u\n", t1 - t0);

  BSP430_HPL_DMA->ctl0 = CONSOLE_DMATSEL * DMA0TSEL0;
  chp->ctl = DMADT_0 | DMASRCINCR_3 | DMADSTBYTE | DMASRCBYTE | DMALEVEL;
  chp->da = (uintptr_t)CONSOLE_DMADA;

  channel0.stage = 0;
  channel0.t0 = channel0.t1 = 0;
  cprintf("Triggering interrupt-driven TX: CTL %04x\n\t", chp->ctl);
  iBSP430consoleFlush();
  chp->ctl |= DMAIE | DMAIFG;
  BSP430_UPTIME_DELAY_MS_NI(10000, LPM0_bits, 1 < channel0.stage);
  cprintf("We're back, did it happen? CTL %04x t0 %u t1 %u delta %u\n", chp->ctl, channel0.t0, channel0.t1, channel0.t1-channel0.t0);
}
