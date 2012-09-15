/** This file is in the public domain.
 *
 * Iterate through available LED colors to visually verify the constants
 * are correct.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/led.h>
#include <bsp430/utility/console.h>

typedef struct sColor {
  char * name;
  int idx;
} sColor;

const sColor colors[] = {
  {
    "red",
#ifdef BSP430_LED_RED
    BSP430_LED_RED
#else /* BSP430_LED_RED */
    -1
#endif /* BSP430_LED_RED */
  },
  {
    "green",
#ifdef BSP430_LED_GREEN
    BSP430_LED_GREEN
#else /* BSP430_LED_GREEN */
    -1
#endif /* BSP430_LED_GREEN */
  },
  {
    "white",
#ifdef BSP430_LED_WHITE
    BSP430_LED_WHITE
#else /* BSP430_LED_WHITE */
    -1
#endif /* BSP430_LED_WHITE */
  },
  {
    "orange",
#ifdef BSP430_LED_ORANGE
    BSP430_LED_ORANGE
#else /* BSP430_LED_ORANGE */
    -1
#endif /* BSP430_LED_ORANGE */
  },
  {
    "yellow",
#ifdef BSP430_LED_YELLOW
    BSP430_LED_YELLOW
#else /* BSP430_LED_YELLOW */
    -1
#endif /* BSP430_LED_YELLOW */
  },
  {
    "blue",
#ifdef BSP430_LED_BLUE
    BSP430_LED_BLUE
#else /* BSP430_LED_BLUE */
    -1
#endif /* BSP430_LED_BLUE */
  },
};
const int ncolors = sizeof(colors) / sizeof(*colors);

void main ()
{
  const sColor * cp = colors;
  const sColor * const ecp = colors + sizeof(colors) / sizeof(*colors);

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();
  while (1) {
    if (0 > cp->idx) {
      cprintf("%s is not available\n", cp->name);
    } else {
      vBSP430ledSet(cp->idx, 1);
      cprintf("%s lit\n", cp->name);
      BSP430_CORE_DELAY_CYCLES(5 * BSP430_CLOCK_NOMINAL_MCLK_HZ);
      vBSP430ledSet(cp->idx, 0);
    }
    if (++cp == ecp) {
      cp = colors;
    }
  }
}
