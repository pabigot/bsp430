/** This file is in the public domain.
 *
 * Basic demonstration of the BSP430 DMA interface.  Shows memcpy and
 * memset about 2-3x faster than code.
 *
 * UART DMA transmission is about the same speed as polling, and about
 * 100% faster than ISR at 9600 (10% faster at 115200); the data rates
 * don't allow for much better improvement in throughput, though load
 * on the system should be significantly less with DMA.
 *
 * @homepage http://github.com/pabigot/bsp430
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/periph/dma.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph/timer.h>
#include <string.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 2048
#endif /* BUFFER_SIZE */

#ifndef CAPTURE_LENGTH
#define CAPTURE_LENGTH 32
#endif /* CAPTURE_LENGTH */

uint8_t buffer[BUFFER_SIZE];
uint8_t src8 = 1;
uint16_t src16 = 1;
const char message[] =
  "0000 Text should be transmitted over the console\r\n"
  "0050 Text should be transmitted over the console\r\n"
#if 9600 < BSP430_CONSOLE_BAUD_RATE
  "0100 Text should be transmitted over the console\r\n"
  "0150 Text should be transmitted over the console\r\n"
#if 115200 < BSP430_CONSOLE_BAUD_RATE
  "0200 Text should be transmitted over the console\r\n"
  "0250 Text should be transmitted over the console\r\n"
  "0300 Text should be transmitted over the console\r\n"
  "0350 Text should be transmitted over the console\r\n"
  "0400 Text should be transmitted over the console\r\n"
  "0450 Text should be transmitted over the console\r\n"
  "0500 Text should be transmitted over the console\r\n"
  "0550 Text should be transmitted over the console\r\n"
  "0600 Text should be transmitted over the console\r\n"
  "0650 Text should be transmitted over the console\r\n"
  "0700 Text should be transmitted over the console\r\n"
  "0750 Text should be transmitted over the console\r\n"
  "0800 Text should be transmitted over the console\r\n"
  "0850 Text should be transmitted over the console\r\n"
  "0900 Text should be transmitted over the console\r\n"
  "0950 Text should be transmitted over the console\r\n"
#endif /* 115200 */
#endif /* 9600 */
  ;

unsigned int captures[CAPTURE_LENGTH];

typedef enum eChannelStages {
  CS_idle,
  CS_needEnable,
  CS_completed,
  CS_countdown,
} eChannelStages;

struct sChannel {
  sBSP430halISRIndexedChainNode cb;
  volatile int stage;
  volatile int counter;
  volatile unsigned long t0;
  volatile unsigned long t1;
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

  if (CS_needEnable == statep->stage) {
    statep->t0 = ulBSP430uptime_ni();
    statep->stage = CS_completed;
    chp->ctl |= DMAEN;
  } else if (CS_countdown == statep->stage) {
    statep->counter -= 1;
    if (0 >= statep->counter) {
      chp->ctl &= ~(DMAEN | DMAIE);
      statep->stage = CS_idle;
      rv = BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
    }
  } else {
    statep->t1 = ulBSP430uptime_ni();
    chp->ctl &= ~(DMAEN | DMAIE);
    statep->stage = CS_idle;
    rv = BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
  }
  return rv;
}

static struct sChannel channel0 = {
  .cb = { .callback_ni = dma_isr_ni }
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
  unsigned long lt0;
  unsigned long lt1;
  unsigned long lc;

  /* Disable ISR-driven output for most of this application. */
  (void)iBSP430consoleTransmitUseInterrupts_ni(0);

  cprintf("\n\ndma " __DATE__ " " __TIME__ "\n");

  /* Configure a timer off SMCLK to measure the performance of
   * different actions. */
  if (NULL == hrt) {
    cprintf("High-resolution timer not available\n");
    return;
  }
#if 0
  /* Initializing for safe counter is only necessary when using
   * uiBSP430timerAsyncCounterRead_ni() or
   * uiBSP430timerSafeCounterRead_ni() */
  vBSP430timerSafeCounterInitialize_ni(hrt);
#endif
  hrt->ctl = TASSEL_2 | MC_2 | TACLR;
  t0 = uiBSP430timerSyncCounterRead_ni(hrt);
  t1 = uiBSP430timerSyncCounterRead_ni(hrt);
  timing_overhead = t1 - t0;
  cprintf("Timing with SMCLK at %lu Hz, nominal timing overhead %u ticks\n", ulBSP430timerFrequency_Hz_ni(BSP430_TIMER_CCACLK_PERIPH_HANDLE), timing_overhead);

  /* Hook the DMA interrupt in, though we don't use it until the last
   * step. */
  BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRIndexedChainNode,
                                  BSP430_HAL_DMA->ch_cbchain_ni[0],
                                  channel0.cb,
                                  next_ni);

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
  t0 = uiBSP430timerSyncCounterRead_ni(hrt);
  memset(buffer, src8, sizeof(buffer));
  t1 = uiBSP430timerSyncCounterRead_ni(hrt);
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
  t0 = uiBSP430timerSyncCounterRead_ni(hrt);
  chp->ctl |= DMAREQ;
  t1 = uiBSP430timerSyncCounterRead_ni(hrt);
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
  t0 = uiBSP430timerSyncCounterRead_ni(hrt);
  chp->ctl |= DMAREQ;
  t1 = uiBSP430timerSyncCounterRead_ni(hrt);
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

  t0 = uiBSP430timerSyncCounterRead_ni(hrt);
  chp->ctl |= DMAREQ;
  t1 = uiBSP430timerSyncCounterRead_ni(hrt);
  chp->ctl &= ~DMAEN;
  buffer[chp->sz] = 0;
  cprintf("Buffer copy done in %u, result:\n\t%s\n", t1 - t0, buffer);

  cprintf("UART transmission is %u bytes, no less than about %lu ms at %lu baud\n",
          chp->sz, ((BSP430_CONSOLE_BAUD_RATE / 2) + chp->sz * 10000UL) / BSP430_CONSOLE_BAUD_RATE,
          (unsigned long)BSP430_CONSOLE_BAUD_RATE);
  cprintf("Bytes-per-second is about %lu\n", (BSP430_CONSOLE_BAUD_RATE / 10UL));

  cprintf("Writing %u bytes through busy-waiting console interface:\n\t", chp->sz);
  lt0 = ulBSP430uptime_ni();
  cputtext(message);
  lt1 = ulBSP430uptime_ni();
  /* Delay to allow last character to get out the serial port */
  BSP430_UPTIME_DELAY_MS_NI(10, LPM0_bits, 0);
  cprintf("polled console output took %lu = %lu us\n", lt1 - lt0, BSP430_UPTIME_UTT_TO_US(lt1 - lt0));

#if 0 < BSP430_CONSOLE_TX_BUFFER_SIZE
  (void)iBSP430consoleTransmitUseInterrupts_ni(1);
  cprintf("Writing %u bytes through %u-octet interrupt-driven console interface:\n\t", chp->sz, BSP430_CONSOLE_TX_BUFFER_SIZE);
  lt0 = ulBSP430uptime_ni();
  cputtext(message);
  /* Low-overhead flush */
  while (0 != iBSP430consoleWaitForTxSpace_ni(-1)) {
    /* nop */
  }
  lt1 = ulBSP430uptime_ni();
  (void)iBSP430consoleTransmitUseInterrupts_ni(0);
  /* Delay to allow last character to get out the serial port */
  BSP430_UPTIME_DELAY_MS_NI(10, LPM0_bits, 0);
  cprintf("isr console output took %lu = %lu us\n", lt1 - lt0, BSP430_UPTIME_UTT_TO_US(lt1 - lt0));
#endif

  BSP430_HPL_DMA->ctl0 = CONSOLE_DMATSEL * DMA0TSEL0;
  chp->ctl = DMADT_0 | DMASRCINCR_3 | DMADSTBYTE | DMASRCBYTE | DMALEVEL;
  chp->da = (uintptr_t)CONSOLE_DMADA;

  channel0.stage = CS_needEnable;
  channel0.t0 = channel0.t1 = 0;
  cprintf("Triggering DMA-driven TX during LPM0: CTL %04x\n\t", chp->ctl);
  iBSP430consoleFlush();
  lc = 0;
  chp->ctl |= DMAIE | DMAIFG;
  BSP430_UPTIME_DELAY_MS_NI(10000, LPM0_bits, CS_idle == channel0.stage);
  cprintf("We're back, did it happen? CTL %04x t0 %lu t1 %lu\n", chp->ctl, channel0.t0, channel0.t1);
  cprintf("dma console output took %lu = %lu us\n", channel0.t1-channel0.t0, BSP430_UPTIME_UTT_TO_US(channel0.t1-channel0.t0));
  cprintf("Counter incremented to %lu\n", lc);

  BSP430_HPL_DMA->ctl0 = CONSOLE_DMATSEL * DMA0TSEL0;
  chp->ctl = DMADT_0 | DMASRCINCR_3 | DMADSTBYTE | DMASRCBYTE | DMALEVEL;
  channel0.stage = CS_needEnable;
  channel0.t0 = channel0.t1 = 0;
  lc = 0;
  cprintf("Triggering DMA-driven TX while CPU active: CTL %04x\n\t", chp->ctl);
  iBSP430consoleFlush();
  chp->ctl |= DMAIE | DMAIFG;
  BSP430_CORE_ENABLE_INTERRUPT();
  do {
    ++lc;
  } while (CS_idle != channel0.stage);
  BSP430_CORE_DISABLE_INTERRUPT();
  cprintf("Counter incremented to %lu while dma output took %lu = %lu us\n",
          lc, channel0.t1-channel0.t0, BSP430_UPTIME_UTT_TO_US(channel0.t1-channel0.t0));

#if defined(DMA_TIMER_PERIPH_HANDLE)
  do {
    sBSP430halTIMER * const dmat_hal = hBSP430timerLookup(DMA_TIMER_PERIPH_HANDLE);
    volatile sBSP430hplTIMER * dmat_hpl;
    static unsigned int const ccidx = 0;
    unsigned int * cp = captures;
    unsigned int * const cpe = cp + sizeof(captures)/sizeof(*captures);

    if (! dmat_hal) {
      cprintf("No DMA-trigger timer available\n");
      break;
    }

    /* Source from SMCLK counting up to 1 ms. */
    dmat_hpl = dmat_hal->hpl;
    dmat_hpl->ctl = TASSEL_2 | TACLR;
    dmat_hpl->ccr[ccidx] = ulBSP430clockSMCLK_Hz_ni() / 1000;
    dmat_hpl->cctl[ccidx] = 0;

    cprintf("Capture at %u ticks of %lu Hz clock on %s.%u\n",
            dmat_hpl->ccr[ccidx],
            ulBSP430timerFrequency_Hz_ni(BSP430_TIMER_CCACLK_PERIPH_HANDLE),
            xBSP430timerName(DMA_TIMER_PERIPH_HANDLE),
            ccidx);

    memset(captures, 0, sizeof(captures));
    dmat_hpl->ctl |= MC_1 | TACLR;
    while (cp < cpe) {
      while (! (CCIFG & dmat_hpl->cctl[ccidx])) {
        /* nop */
      }
      *cp++ = hBSP430uptimeTimer()->hpl->r;
      dmat_hpl->cctl[ccidx] &= ~CCIFG;
    }
    dmat_hpl->ctl &= ~(MC0 | MC1);
    cprintf("Manual capture %u entries; deltas:\n", cpe - captures);
    cp = captures;
    while (++cp < cpe) {
      cprintf(" %7u", cp[0] - cp[-1]);
    }
    cputchar('\n');

    memset(captures, 0, sizeof(captures));

    /* Repeated word transfer from uptime clock to captures array
     * triggered at 1ms intervals. */
    BSP430_HPL_DMA->ctl0 = ONEMS_DMATSEL * DMA0TSEL0;
    chp->ctl = DMADT_0 | DMADSTINCR_3 | DMASRCINCR_0;
    chp->da = (uintptr_t)captures;
    chp->sa = (uintptr_t)&hBSP430uptimeTimer()->hpl->r;
    chp->sz = (cpe - captures);

    /* Enable interrupt in countdown mode */
    channel0.stage = CS_completed;
    chp->ctl |= DMAEN | DMAIE;

    cprintf("DMA capture %u on timer\n", channel0.counter);

    lt0 = ulBSP430uptime_ni();
    dmat_hpl->cctl[ccidx] &= ~CCIFG;
    dmat_hpl->ctl |= MC_1 | TACLR;
    BSP430_UPTIME_DELAY_MS_NI(5000, LPM0_bits, CS_idle == channel0.stage);
    dmat_hpl->ctl &= ~(MC0 | MC1);
    lt1 = ulBSP430uptime_ni();

    cprintf("DMA capture %u entries counter %d in %lu ms; deltas:\n", channel0.counter, cpe - captures, BSP430_UPTIME_UTT_TO_MS(lt1 - lt0));
    cp = captures;
    while (++cp < cpe) {
      cprintf(" %7u", cp[0] - cp[-1]);
    }
    cputchar('\n');

  } while (0);
#endif /* DMA_TIMER_PERIPH_HANDLE */
}
