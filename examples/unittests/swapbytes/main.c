/** This file is in the public domain.
 *
 * @homepage http://github.com/pabigot/bsp430
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/unittest.h>

/* Use separate compilation to prevent compile-time evaluation of the
 * expression. */
uint16_t bswap16i16o16 (uint16_t in);
uint16_t bswap16i32o32 (uint32_t in);
uint32_t bswap32i32o32 (uint32_t in);

void main ()
{
  vBSP430platformInitialize_ni();
  vBSP430unittestInitialize();

  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(0x1234, BSP430_CORE_SWAP_16(0x3412));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(0x1234, bswap16i16o16(0x3412));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(0x1234, bswap16i32o32(0x3412));

  BSP430_UNITTEST_ASSERT_EQUAL_FMTlx(0x7856, BSP430_CORE_SWAP_16(0x12345678L));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlx(0x7856L, bswap16i32o32(0x12345678L));

  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(0xFEDC, BSP430_CORE_SWAP_16(0xDCFE));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(0xFEDC, bswap16i16o16(0xDCFE));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(0xFEDC, bswap16i32o32(0xDCFE));

  BSP430_UNITTEST_ASSERT_EQUAL_FMTlx(0x98BA, BSP430_CORE_SWAP_16(0xFEDCBA98));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlx(0x98BAL, bswap16i32o32(0xFEDCBA98L));

  vBSP430unittestFinalize();
}
