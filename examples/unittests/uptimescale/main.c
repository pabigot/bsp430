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

void main ()
{
  const char * units;
  const unsigned long long one_s = 32768;
  unsigned int sv;

  vBSP430platformInitialize_ni();
  vBSP430unittestInitialize();

  /* Disable the timer and set its frequency to a known value.  We
   * don't need/want it running for the tests. */
  vBSP430uptimeSuspend_ni();
  ulBSP430uptimeSetConversionFrequency_ni(one_s);

  BSP430_CORE_ENABLE_INTERRUPT();

  sv = uiBSP430uptimeScaleForDisplay(1, &units);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(30, sv);
  BSP430_UNITTEST_ASSERT_EQUAL_ASCIIZ("us", units);

  sv = uiBSP430uptimeScaleForDisplay(32, &units);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(976, sv);
  BSP430_UNITTEST_ASSERT_EQUAL_ASCIIZ("us", units);

  sv = uiBSP430uptimeScaleForDisplay(33, &units);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(1007, sv);
  BSP430_UNITTEST_ASSERT_EQUAL_ASCIIZ("us", units);

  sv = uiBSP430uptimeScaleForDisplay(100, &units);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(3051, sv);
  BSP430_UNITTEST_ASSERT_EQUAL_ASCIIZ("us", units);

  sv = uiBSP430uptimeScaleForDisplay(1000, &units);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(30, sv);
  BSP430_UNITTEST_ASSERT_EQUAL_ASCIIZ("ms", units);

  sv = uiBSP430uptimeScaleForDisplay(3276, &units);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(99, sv);
  BSP430_UNITTEST_ASSERT_EQUAL_ASCIIZ("ms", units);

  sv = uiBSP430uptimeScaleForDisplay(32768, &units);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(1000, sv);
  BSP430_UNITTEST_ASSERT_EQUAL_ASCIIZ("ms", units);

  sv = uiBSP430uptimeScaleForDisplay(3*32768ULL, &units);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(3000, sv);
  BSP430_UNITTEST_ASSERT_EQUAL_ASCIIZ("ms", units);

  vBSP430unittestFinalize();
}
