/** This file is in the public domain.
 *
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/unittest.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/console.h>

void
testP2SEL (void)
{
  int sel;
#if (BSP430_PORT_SUPPORTS_SEL2 - 0)
  const int MAX_SEL = 3;
#else
  const int MAX_SEL = 1;
#endif
  unsigned int pxsel = PORT_SEL0 & ~(PORT_BIT);
#ifdef PORT_SEL1
  unsigned int pxsel2 = PORT_SEL1 & ~(PORT_BIT);
#define SEL1_VALUE (!!(PORT_SEL1 & PORT_BIT))
#define SEL1_SHIFT (SEL1_VALUE << 1)
#else
#define SEL1_VALUE -1
#define SEL1_SHIFT 0
#endif

  for (sel = 0; sel <= MAX_SEL; ++sel) {
    unsigned int sv;
    cprintf("%d: in SEL1 %d SEL0 %d\n", sel, SEL1_VALUE, !!(PORT_SEL0 & PORT_BIT));
    BSP430_PORT_HAL_SET_SEL(PORT_HAL, PORT_BIT, sel);
    sv = SEL1_SHIFT | (!!(PORT_SEL0 & PORT_BIT));
    cprintf("%d: out SEL1 %d SEL0 %d\n", sel, SEL1_VALUE, !!(PORT_SEL0 & PORT_BIT));
    BSP430_UNITTEST_ASSERT_EQUAL_FMTd(sv, sel);
    BSP430_UNITTEST_ASSERT_EQUAL_FMTx(PORT_SEL0 & ~(PORT_BIT), pxsel);
#ifdef PORT_SEL1
    BSP430_UNITTEST_ASSERT_EQUAL_FMTx(PORT_SEL1 & ~(PORT_BIT), pxsel2);
#endif /* PORT_SEL1 */

    BSP430_PORT_HAL_SET_SEL(PORT_HAL, PORT_BIT, 0);
    sv = SEL1_SHIFT | (!!(PORT_SEL0 & PORT_BIT));
    BSP430_UNITTEST_ASSERT_EQUAL_FMTd(sv, 0);
    BSP430_PORT_HPL_SET_SEL(PORT_HPL, PORT_BIT, sel);
    sv = SEL1_SHIFT | (!!(PORT_SEL0 & PORT_BIT));
#if (BSP430_PORT_SUPPORTS_SEL2_2XX - 0)
    BSP430_UNITTEST_ASSERT_EQUAL_FMTd(sv, sel & 0x01);
#else
    BSP430_UNITTEST_ASSERT_EQUAL_FMTd(sv, sel);
#endif
  }
}

void main ()
{
  vBSP430platformInitialize_ni();
  vBSP430unittestInitialize();

  testP2SEL();

  vBSP430unittestFinalize();
}
