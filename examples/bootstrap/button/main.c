/** This file is in the public domain.
 *
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/led.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/periph/port.h>

#if ! (BSP430_PLATFORM_BUTTON0 - 0)
#error No button available on this platform
#endif /* BSP430_PLATFORM_BUTTON0 */

#define LPM_BITS (LPM0_bits | GIE)

typedef struct sButtonState {
  sBSP430halISRCallbackIndexed button_cb;
  unsigned char bit;
  volatile int in_mask;
  volatile int count;
} sButtonState;

static int
button_isr (const struct sBSP430halISRCallbackIndexed * cb,
            void * context,
            int idx)
{
  hBSP430halPORT hal = (hBSP430halPORT)context;
  sButtonState * sp = (sButtonState *)cb;
  ++sp->count;
  sp->in_mask = BSP430_PORT_HAL_HPL_IN(hal) & sp->bit;
  BSP430_PORT_HAL_GET_HPL_PORTIE(hal)->ies ^= sp->bit;
  vBSP430ledSet(0, -1);
  return LPM_BITS;
}

static sButtonState button_state = {
  .button_cb = { .callback = button_isr },
  .bit = BSP430_PLATFORM_BUTTON0_PORT_BIT
};

void main ()
{
  hBSP430halPORT b0hal = hBSP430portLookup(BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE);
  volatile sBSP430hplPORTIE * b0hpl;
  int b0pin = iBSP430portBitPosition(BSP430_PLATFORM_BUTTON0_PORT_BIT);
  int i;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  for (i = 0; i < 5; ++i) {
    cprintf("Up\n");
    __delay_cycles(10000);
  }
  cprintf("There's supposed to be a button at %s.%u\n",
          xBSP430portName(BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE),
          b0pin);
  if (! b0hal) {
    cprintf("Whoops, guess it's not really there\n");
    return;
  }
  b0hpl = BSP430_PORT_HAL_GET_HPL_PORTIE(b0hal);
  button_state.button_cb.next = b0hal->pin_callback[b0pin];
  b0hal->pin_callback[b0pin] = &button_state.button_cb;
  b0hpl->sel &= ~BSP430_PLATFORM_BUTTON0_PORT_BIT;
  b0hpl->dir &= ~BSP430_PLATFORM_BUTTON0_PORT_BIT;
#if BSP430_PORT_SUPPORTS_REN - 0
  BSP430_PORT_HAL_HPL_REN(b0hal) |= BSP430_PLATFORM_BUTTON0_PORT_BIT;
  BSP430_PORT_HAL_HPL_OUT(b0hal) |= BSP430_PLATFORM_BUTTON0_PORT_BIT;
#endif /* BSP430_PORT_SUPPORTS_REN */
  if (b0hpl->in & BSP430_PLATFORM_BUTTON0_PORT_BIT) {
    b0hpl->ies |= BSP430_PLATFORM_BUTTON0_PORT_BIT;
  } else {
    b0hpl->ies &= ~BSP430_PLATFORM_BUTTON0_PORT_BIT;
  }
  b0hpl->ifg &= ~BSP430_PLATFORM_BUTTON0_PORT_BIT;
  b0hpl->ie |= BSP430_PLATFORM_BUTTON0_PORT_BIT;
  cprintf("Button is configured.  Try pressing it.  No debouncing is done.\n");

  vBSP430ledSet(0, 1);

  while (1) {
    static const char * state_str[] = { "released", "pressed" };

    cprintf("Count %u, in mask 0x%02x: %s\n", button_state.count, button_state.in_mask, state_str[!button_state.in_mask]);
    __bis_status_register(LPM_BITS);
  }
}
