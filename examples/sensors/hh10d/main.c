/** This file is in the public domain.
 *
 *
 * @homepage http://github.com/pabigot/bsp430
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
#ifndef APP_HH10D_PORT_PERIPH_HANDLE
#error No HH10D port specified
#endif /* APP_HH10D_PORT_PERIPH_HANDLE */

struct sHH10D {
  sBSP430halISRIndexedChainNode cb;
  volatile sBSP430hplTIMER * freq_timer;
  int uptime_ccidx;
  unsigned int sample_duration_utt;
  unsigned int last_capture;
  unsigned int last_period_count;
};

static void
register_hh10d_ni (struct sHH10D * sp)
{
  /* Hook into the uptime infrastructure and have the HH10D callback
   * invoked once per second, starting as soon as interrupts are
   * enabled. */
  sp->cb.next_ni = xBSP430uptimeTimer()->cc_cbchain_ni[sp->uptime_ccidx];
  xBSP430uptimeTimer()->cc_cbchain_ni[sp->uptime_ccidx] = &sp->cb;
  xBSP430uptimeTimer()->hpl->cctl[sp->uptime_ccidx] = CCIFG | CCIE;
}

static int
hh10d_1Hz_isr_ni (const struct sBSP430halISRIndexedChainNode *cb,
                  void *context,
                  int idx)
{
  volatile struct sHH10D * hh10d = (struct sHH10D *)cb;
  unsigned int capture;
  hBSP430halTIMER timer = (hBSP430halTIMER)context;
  vBSP430ledSet(0, -1);

  /* Record the HH10D counter, schedule the next wakeup, then return
   * waking the MCU and inhibiting further interrupts when active. */
  capture = hh10d->freq_timer->r;
  hh10d->last_period_count = capture - hh10d->last_capture;
  hh10d->last_capture = capture;
  timer->hpl->ccr[idx] += hh10d->sample_duration_utt;
  return LPM4_bits | GIE;
}

static struct sHH10D hh10d = {
  .cb = { .callback = hh10d_1Hz_isr_ni },
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

void main ()
{
  sBSP430halPORT * hh10d_port = hBSP430portLookup(APP_HH10D_PORT_PERIPH_HANDLE);
  hBSP430halSERIAL i2c = hBSP430serialLookup(APP_HH10D_I2C_PERIPH_HANDLE);
  int uptime_Hz;
  int hh10d_offs;
  int hh10d_sens;

  vBSP430platformInitialize_ni();

  (void)iBSP430consoleInitialize();
  cprintf("\nApplication starting\n");
  BSP430_CORE_DELAY_CYCLES(10000);

  if (NULL == hh10d_port) {
    cprintf("\nERROR: No port HAL; did you enable configBSP430_HAL_%s?\n", xBSP430portName(APP_HH10D_PORT_PERIPH_HANDLE) ?: "whatever");
    return;
  }

  /* Initialize the state information used in the HH10D ISR */
  hh10d.freq_timer = xBSP430hplLookupTIMER(APP_HH10D_TIMER_PERIPH_HANDLE);
  hh10d.uptime_ccidx = APP_HH10D_UPTIME_CC_INDEX;
  uptime_Hz = ulBSP430uptimeResolution_Hz_ni();
  hh10d.sample_duration_utt = uptime_Hz;

  cprintf("HH10D I2C on %s at %p, bus rate %lu Hz, address 0x%02x\n",
          xBSP430serialName(APP_HH10D_I2C_PERIPH_HANDLE) ?: "UNKNOWN",
          i2c, ulBSP430clockSMCLK_Hz_ni() / APP_HH10D_I2C_PRESCALER,
          APP_HH10D_I2C_ADDRESS);
#if BSP430_PLATFORM_PERIPHERAL_HELP
  cprintf("HH10D I2C Pins: %s\n", xBSP430platformPeripheralHelp(APP_HH10D_I2C_PERIPH_HANDLE, BSP430_PERIPHCFG_SERIAL_I2C));
#endif /* BSP430_PLATFORM_PERIPHERAL_HELP */
  cprintf("Monitoring HH10D on %s.%u using timer %s\n",
          xBSP430portName(APP_HH10D_PORT_PERIPH_HANDLE) ?: "P?",
          bitToPin(APP_HH10D_PORT_BIT),
          xBSP430timerName(APP_HH10D_TIMER_PERIPH_HANDLE) ?: "T?");
  cprintf("Uptime CC block %s.%u at %u Hz sample duration %u ticks\n",
          xBSP430timerName(BSP430_UPTIME_TIMER_PERIPH_HANDLE),
          APP_HH10D_UPTIME_CC_INDEX, uptime_Hz, hh10d.sample_duration_utt);


  i2c = hBSP430serialOpenI2C(i2c, (0x100 <= UCMST) ? (UCMST >> 8) : UCMST,
                             UCSSEL_2, APP_HH10D_I2C_PRESCALER);
  if (! i2c) {
    cprintf("I2C open failed.\n");
    return;
  }

  (void)iBSP430i2cSetAddresses_ni(i2c, -1, APP_HH10D_I2C_ADDRESS);

  hh10d_sens = 0;
  {
    int rc;
    uint8_t addr = 10;
    uint8_t data[4];
    rc = iBSP430i2cTxData_ni(i2c, &addr, sizeof(addr));
    if (sizeof(addr) == rc) {
      rc = iBSP430i2cRxData_ni(i2c, data, sizeof(data));
      if (sizeof(data) == rc) {
        hh10d_sens = (data[0] << 8) | data[1];
        hh10d_offs = (data[2] << 8) | data[3];
      }
    }
  }
  if (hh10d_sens) {
    cprintf("I2C read offset %u sensitivity %u\n", hh10d_offs, hh10d_sens);
  } else {
    cprintf("I2C read of offset and sensitivity failed\n");
  }

  /* Select input peripheral function mode for CCACLK timer clock
   * signal, which is where the HH10D's frequency signal should be
   * found, and configure the assigned timer to count that input
   * continuously. */
  BSP430_PORT_HAL_HPL_SEL(hh10d_port) |= APP_HH10D_PORT_BIT;
  BSP430_PORT_HAL_HPL_DIR(hh10d_port) &= ~APP_HH10D_PORT_BIT;
  hh10d.freq_timer->ctl = TASSEL_0 | MC_2 | TACLR;

  /* Hook into the uptime infrastructure and have the HH10D callback
   * invoked once per second, starting one second from now */
  register_hh10d_ni(&hh10d);

  /* Go to low power mode with interrupts enabled */
  BSP430_CORE_LPM_ENTER_NI(LPM1_bits | GIE);
  cprintf("Initial %u\n", hh10d.last_capture);

  while (1) {
    BSP430_CORE_LPM_ENTER_NI(LPM1_bits | GIE);
    cprintf("%lu: Sample %u in %u uptime ticks", ulBSP430uptime_ni(), hh10d.last_period_count, hh10d.sample_duration_utt);
    if (0 != hh10d_sens) {
      cprintf(": RH %u%%", (unsigned int)(((hh10d_offs - hh10d.last_period_count) * (unsigned long) hh10d_sens) / 4096));
    }
    cputchar_ni('\n');
  }
}
