/** This file is in the public domain.
 *
 *
 * @homepage http://github.com/pabigot/bsp430
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

typedef struct sButtonState {
  sBSP430halISRIndexedChainNode button_cb; /* Callback structure */
  const unsigned char bit;      /* Bit for button */
  volatile int in_mask;         /* Bit set if button is pressed */
  volatile int count;           /* Number of interrupts occured */
} sButtonState;

static int
button_isr_ni (const struct sBSP430halISRIndexedChainNode * cb,
               void * context,
               int idx)
{
  hBSP430halPORT hal = (hBSP430halPORT)context;
  volatile sBSP430hplPORTIE * hpl = BSP430_PORT_HAL_GET_HPL_PORTIE(hal);
  sButtonState * sp = (sButtonState *)cb;

  ++sp->count;

  /* Record whether the button is currently pressed based on the edge
   * type we captured (1 means transition to 0 = released, 0 means
   * transition to 1 = pressed).  Configure to detect a state change
   * opposite of the one we just captured, regardless of what that
   * state might be.  This algorithm coupled with interrupts being
   * disabled outside of LPM helps ensure we interleave
   * pressed/released notifications even in the presence of
   * bounces. */
  sp->in_mask = (hpl->ies & sp->bit) ^ sp->bit;
  hpl->ies ^= sp->bit;
  vBSP430ledSet(0, -1);

  /* Wakeup.  Whether this clears #GIE depends on
   * #configBSP430_CORE_LPM_EXIT_CLEAR_GIE */
  return BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
}

static sButtonState button_state = {
  .button_cb = { .callback = button_isr_ni },
  .bit = BSP430_PLATFORM_BUTTON0_PORT_BIT
};

void main ()
{
  hBSP430halPORT b0hal = hBSP430portLookup(BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE);
  volatile sBSP430hplPORTIE * b0hpl;
  int b0pin = iBSP430portBitPosition(BSP430_PLATFORM_BUTTON0_PORT_BIT);

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cprintf("\nbutton " __DATE__ " " __TIME__ "\n");
  
  cprintf("There's supposed to be a button at %s.%u\n",
          xBSP430portName(BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE),
          b0pin);
  if (! b0hal) {
    cprintf("Whoops, guess it's not really there\n");
    return;
  }
  b0hpl = BSP430_PORT_HAL_GET_HPL_PORTIE(b0hal);
  button_state.button_cb.next_ni = b0hal->pin_cbchain_ni[b0pin];
  b0hal->pin_cbchain_ni[b0pin] = &button_state.button_cb;
  b0hpl->sel &= ~BSP430_PLATFORM_BUTTON0_PORT_BIT;
  b0hpl->dir &= ~BSP430_PLATFORM_BUTTON0_PORT_BIT;
#if BSP430_PORT_SUPPORTS_REN - 0
  BSP430_PORT_HAL_HPL_REN(b0hal) |= BSP430_PLATFORM_BUTTON0_PORT_BIT;
  BSP430_PORT_HAL_HPL_OUT(b0hal) |= BSP430_PLATFORM_BUTTON0_PORT_BIT;
#endif /* BSP430_PORT_SUPPORTS_REN */
  if (b0hpl->in & BSP430_PLATFORM_BUTTON0_PORT_BIT) {
    button_state.in_mask = BSP430_PLATFORM_BUTTON0_PORT_BIT;
    b0hpl->ies |= BSP430_PLATFORM_BUTTON0_PORT_BIT;
  } else {
    button_state.in_mask = 0;
    b0hpl->ies &= ~BSP430_PLATFORM_BUTTON0_PORT_BIT;
  }
  b0hpl->ifg &= ~BSP430_PLATFORM_BUTTON0_PORT_BIT;
  b0hpl->ie |= BSP430_PLATFORM_BUTTON0_PORT_BIT;
  cprintf("Button is configured.  Try pressing it.  No debouncing is done.\n");
#if ! (configBSP430_CORE_LPM_EXIT_CLEAR_GIE - 0)
  cprintf("WARNING: Interrupts remain enabled after wakeup\n");
#endif /* configBSP430_CORE_LPM_EXIT_CLEAR_GIE */

  vBSP430ledSet(0, 1);

  while (1) {
    static const char * state_str[] = { "released", "pressed" };

    cprintf("Count %u, in mask 0x%02x: %s\n", button_state.count, button_state.in_mask, state_str[!button_state.in_mask]);

    /* Note that we've never turned interrupts on, but
     * BSP430_CORE_LPM_ENTER_NI() does so internally so the ISR can be
     * executed.  Whether they are cleared before returning to this
     * loop depends on #configBSP430_CORE_LPM_EXIT_CLEAR_GIE. */
    BSP430_CORE_LPM_ENTER_NI(LPM0_bits);
#if ! (configBSP430_CORE_LPM_EXIT_CLEAR_GIE - 0)
    /* Infrastructure didn't clear this, so we should */
    BSP430_CORE_DISABLE_INTERRUPT();
#endif /* configBSP430_CORE_LPM_EXIT_CLEAR_GIE */
  }
}
