/** This file is in the public domain.
 *
 * This demonstrates direct control of a Sharp Microelectronics Memory
 * LCD, as implemented in the 430BOOST-SHARP96 booster pack.  The
 * behavior is to display a pattern then enter a loop.  In active mode
 * within the loop, the display is shifted up one line at 10Hz.  By
 * pressing a button the mode becomes inactive, during which VCOM is
 * toggled at 1Hz to keep the display from building up DC bias.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <string.h>
#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/serial.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/led.h>
#include <bsp430/utility/sharplcd.h>

#define DISPLAY_HEIGHT BSP430_PLATFORM_SHARPLCD_ROWS
#define DISPLAY_WIDTH_BYTES BSP430_PLATFORM_SHARPLCD_BYTES_PER_LINE

/* Line-addressed display buffer */
uint8_t display_buffer[DISPLAY_HEIGHT * DISPLAY_WIDTH_BYTES];

static void
scroll_buffer_up (int nlines)
{
  int end_line = (DISPLAY_HEIGHT - nlines);
  memmove(display_buffer, display_buffer + (nlines * DISPLAY_WIDTH_BYTES), end_line * DISPLAY_WIDTH_BYTES);
  memset(display_buffer + end_line * DISPLAY_WIDTH_BYTES, 0, nlines);
}

static void
fill_buffer_last ()
{
  static unsigned int offset;
  unsigned char * dp = display_buffer + (DISPLAY_HEIGHT - 1) * DISPLAY_WIDTH_BYTES;
  unsigned int val = ++offset;
  int ci;
  for (ci = 0; ci <= DISPLAY_WIDTH_BYTES; ++ci) {
    *dp++ = val++;
  }
}

typedef struct sButtonState {
  sBSP430halISRIndexedChainNode button_cb; /* Callback structure */
  const unsigned char bit;      /* Bit for button */
  volatile unsigned int in_mask; /* Bit set if button is pressed */
  volatile unsigned int active;
} sButtonState;

static int
button_isr_ni (const struct sBSP430halISRIndexedChainNode * cb,
               void * context,
               int idx)
{
  hBSP430halPORT hal = (hBSP430halPORT)context;
  volatile sBSP430hplPORTIE * hpl = BSP430_PORT_HAL_GET_HPL_PORTIE(hal);
  sButtonState * sp = (sButtonState *)cb;
  int rv = 0;

  /* Toggle state only on falling edge */
  sp->in_mask = (hpl->ies & sp->bit) ^ sp->bit;
  if (sp->in_mask) {
    sp->active = ! sp->active;
    rv = BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
  }
  hpl->ies ^= sp->bit;

  return rv;
}

static sButtonState button_state = {
  .button_cb = { .callback_ni = button_isr_ni },
  .bit = BSP430_PLATFORM_BUTTON0_PORT_BIT
};


void main ()
{
  hBSP430halPORT b0hal = hBSP430portLookup(BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE);
  unsigned long wake_utt;
  unsigned long scroll_interval_utt;
  unsigned long stable_interval_utt;
  sBSP430sharplcd device;
  hBSP430sharplcd dev;
  int li;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();
  BSP430_CORE_ENABLE_INTERRUPT();

  cprintf("\nsharp96 " __DATE__ " " __TIME__ "\n");
  dev = hBSP430sharplcdInitializePlatformDevice(&device);

  cprintf("Sharp Memory LCD size %u rows x %u columns with %u bytes per line\n",
          dev->lines, dev->columns, dev->line_size);
  cprintf("SPI on %s at %p, bus rate %lu Hz\n",
          xBSP430serialName(BSP430_PLATFORM_SHARPLCD_SPI_PERIPH_HANDLE) ?: "UNKNOWN",
          device.spi, (unsigned long)BSP430_PLATFORM_SHARPLCD_SPI_BUS_HZ);
#if BSP430_PLATFORM_PERIPHERAL_HELP
  cprintf("SPI Pins: %s\n", xBSP430platformPeripheralHelp(BSP430_PLATFORM_SHARPLCD_SPI_PERIPH_HANDLE, BSP430_PERIPHCFG_SERIAL_SPI3));
#endif /* BSP430_PLATFORM_PERIPHERAL_HELP */
  cprintf("CS active high on %s.%u\n",
          xBSP430portName(BSP430_PLATFORM_SHARPLCD_CS_PORT_PERIPH_HANDLE),
          iBSP430portBitPosition(BSP430_PLATFORM_SHARPLCD_CS_PORT_BIT));
  cprintf("LCD_EN on %s.%u\n",
          xBSP430portName(BSP430_PLATFORM_SHARPLCD_LCD_EN_PORT_PERIPH_HANDLE),
          iBSP430portBitPosition(BSP430_PLATFORM_SHARPLCD_LCD_EN_PORT_BIT));
  cprintf("PWR_EN on %s.%u\n",
          xBSP430portName(BSP430_PLATFORM_SHARPLCD_PWR_EN_PORT_PERIPH_HANDLE),
          iBSP430portBitPosition(BSP430_PLATFORM_SHARPLCD_PWR_EN_PORT_BIT));

  if (! dev) {
    cprintf("ERROR: Missing resource\n");
    return;
  }

  if (b0hal) {
    BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
    volatile sBSP430hplPORTIE * b0hpl;
    int b0pin = iBSP430portBitPosition(BSP430_PLATFORM_BUTTON0_PORT_BIT);

    cprintf("Scroll/pause button at %s.%u\n",
            xBSP430portName(BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE),
            b0pin);
    b0hpl = BSP430_PORT_HAL_GET_HPL_PORTIE(b0hal);
    BSP430_CORE_DISABLE_INTERRUPT();
    do {
      button_state.button_cb.next_ni = b0hal->pin_cbchain_ni[b0pin];
      b0hal->pin_cbchain_ni[b0pin] = &button_state.button_cb;
      b0hpl->sel &= ~BSP430_PLATFORM_BUTTON0_PORT_BIT;
      b0hpl->dir &= ~BSP430_PLATFORM_BUTTON0_PORT_BIT;
#if (BSP430_PORT_SUPPORTS_REN - 0)
      BSP430_PORT_HAL_SET_REN(b0hal, BSP430_PLATFORM_BUTTON0_PORT_BIT, BSP430_PORT_REN_PULL_UP);
#endif /* BSP430_PORT_SUPPORTS_REN */
      if (b0hpl->in & BSP430_PLATFORM_BUTTON0_PORT_BIT) {
        button_state.in_mask = BSP430_PLATFORM_BUTTON0_PORT_BIT;
        b0hpl->ies |= BSP430_PLATFORM_BUTTON0_PORT_BIT;
      } else {
        button_state.in_mask = 0;
        b0hpl->ies &= ~BSP430_PLATFORM_BUTTON0_PORT_BIT;
      }
      button_state.active = 1;
      b0hpl->ifg &= ~BSP430_PLATFORM_BUTTON0_PORT_BIT;
      b0hpl->ie |= BSP430_PLATFORM_BUTTON0_PORT_BIT;
    } while (0);
    BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  }
  {
    BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
    BSP430_CORE_DISABLE_INTERRUPT();
    do {
      iBSP430sharplcdSetEnabled_ni(dev, 1);
    } while (0);
    BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  }

  cprintf("Initializing display buffer\n");
  for (li = 0; li < DISPLAY_HEIGHT; ++li) {
    scroll_buffer_up(1);
    fill_buffer_last();
  }
  cprintf("Loading initial display\n");
  iBSP430sharplcdUpdateDisplayLines_rh(dev, 1, -1, display_buffer);

  scroll_interval_utt = BSP430_UPTIME_MS_TO_UTT(100);
  stable_interval_utt = BSP430_UPTIME_MS_TO_UTT(BSP430_SHARPLCD_REFRESH_INTERVAL_MS);
  wake_utt = ulBSP430uptime();
  while (1) {
    long rem;
    if (button_state.active) {
      vBSP430ledSet(BSP430_LED_RED, 0);
      vBSP430ledSet(BSP430_LED_GREEN, -1);
      scroll_buffer_up(1);
      fill_buffer_last();
      iBSP430sharplcdUpdateDisplayLines_rh(dev, 1, -1, display_buffer);
      wake_utt += scroll_interval_utt;
    } else {
      vBSP430ledSet(BSP430_LED_GREEN, 0);
      vBSP430ledSet(BSP430_LED_RED, -1);
      iBSP430sharplcdRefreshDisplay_rh(dev);
      wake_utt += stable_interval_utt;
    }
    rem = lBSP430uptimeSleepUntil_ni(wake_utt, LPM3_bits);
    if (0 < rem) {
      cprintf("woke early, button active %u\n", button_state.active);
    }
   }
  iBSP430sharplcdClearDisplay_rh(dev);
  iBSP430sharplcdSetEnabled_ni(dev, 0);
}
