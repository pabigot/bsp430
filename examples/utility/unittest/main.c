/** This file is in the public domain.
 *
 * Test with:
 *
 * -DAPP_ONLY_PASSING
 * -DconfigBSP430_UNITTEST_FAILFAST
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/unittest.h>

#ifndef APP_ONLY_PASSING
#define APP_ONLY_PASSING 0
#endif /* APP_ONLY_PASSING */

void main ()
{
  int rv;

  (void)rv;
  vBSP430platformInitialize_ni();
  vBSP430unittestInitialize();
#if ! APP_ONLY_PASSING
  BSP430_UNITTEST_FAIL("unconditional");
#endif /* APP_ONLY_PASSING */
  BSP430_UNITTEST_ASSERT_TRUE(1 == 1);
  BSP430_UNITTEST_ASSERT_TRUE(1 != 2);
  BSP430_UNITTEST_ASSERT_FALSE(1 != 1);
  BSP430_UNITTEST_ASSERT_FALSE(1 == 2);
#if ! APP_ONLY_PASSING
  rv = 42;
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(1, rv);
#endif /* APP_ONLY_PASSING */
  vBSP430unittestFinalize();
}
