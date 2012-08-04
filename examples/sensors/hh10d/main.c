/** This file is in the public domain.
 *
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 *
 */

#include <bsp430/platform.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/onewire.h>
#include <bsp430/periph/timer.h>

/* Sanity check that the features we requested are present */
#if ! (BSP430_CONSOLE - 0)
#error Console is not configured correctly
#endif /* BSP430_CONSOLE */
/* Sanity check that the features we requested are present */
#if ! (BSP430_UPTIME - 0)
#error Uptime is not configured correctly
#endif /* BSP430_UPTIME */
/* Sanity check that we have the HH10D interface */
#ifndef APP_HH10D_PORT_HPL
#error No HH10D port specified
#endif /* APP_HH10D_PORT_HPL */

static int uptime_Hz;

volatile unsigned int hh10d_counter;
static int
hh10d_1Hz_isr (const struct sBSP430halISRCallbackIndexed *cb,
               void *context,
               int idx)
{
  hBSP430halTIMER timer = (hBSP430halTIMER)context;
  vBSP430ledSet(0, -1);

  /* Record the HH10D counter, schedule the next wakeup, then return
   * waking the MCU but inhibiting further interrupts. */
  hh10d_counter = APP_HH10D_TIMER_HPL->r;
  timer->hpl->ccr[idx] += uptime_Hz;
  return LPM4_bits | GIE;
}

static sBSP430halISRCallbackIndexed hh10d_isr_cb = {
  .callback = hh10d_1Hz_isr
};

void main ()
{
  int puc_sr = __read_status_register();
  unsigned int last_hh10d_counter;
  volatile sBSP430hplTIMER * uptime_hpl = BSP430_UPTIME_TIMER_HAL_HANDLE->hpl;

  vBSP430platformInitialize_ni();

#if BSP430_PLATFORM_SPIN_FOR_JUMPER - 0
  vBSP430platformSpinForJumper_ni();
#endif /* BSP430_PLATFORM_SPIN_FOR_JUMPER */

  (void)xBSP430consoleInitialize();
  cprintf("\nHere we go...puc %x now %x\n", puc_sr, __read_status_register());

  uptime_Hz = ulBSP430uptimeResolution_Hz_ni();
  cprintf("Uptime now %lu with resolution %u\n",
          ulBSP430uptime_ni(), uptime_Hz);

  /* Select input port mode for CCACLK timer clock signal, which is
   * where the HH10D's frequency signal should be found, and configure
   * the assigned timer to count that input continuously. */
  APP_HH10D_PORT_HPL->sel |= APP_HH10D_PORT_PIN;
  APP_HH10D_TIMER_HPL->ctl = TASSEL_0 | MC_2 | TACLR;

  /* Hook into the uptime infrastructure and have the HH10D callback
   * invoked once per second, starting one second from now */
  hh10d_isr_cb.next = BSP430_UPTIME_TIMER_HAL_HANDLE->cc_callback[APP_HH10D_UPTIME_CC_INDEX];
  BSP430_UPTIME_TIMER_HAL_HANDLE->cc_callback[APP_HH10D_UPTIME_CC_INDEX] = &hh10d_isr_cb;
  uptime_hpl->ccr[APP_HH10D_UPTIME_CC_INDEX] = uptime_hpl->r + uptime_Hz;
  uptime_hpl->cctl[APP_HH10D_UPTIME_CC_INDEX] = CCIE;

  /* Go to low power mode with interrupts enabled */
  __bis_status_register(LPM1_bits | GIE);
  last_hh10d_counter = hh10d_counter;
  cprintf("Initial %u\n", last_hh10d_counter);

  while (1) {
    __bis_status_register(LPM0_bits | GIE);
    cprintf("%u\n", hh10d_counter - last_hh10d_counter);
    last_hh10d_counter = hh10d_counter;
  }
}
