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
#include <bsp430/utility/eui64.h>
#include <bsp430/utility/console.h>

static void
testAsText ()
{
  uBSP430eui64 eui64;
  char buffer[BSP430_EUI64_AS_TEXT_LENGTH];
  uint8_t * bp;

  memset(&eui64, 0, sizeof(eui64));
  BSP430_UNITTEST_ASSERT_EQUAL_ASCIIZ(xBSP430eui64AsText(&eui64, buffer), "00-00-00-00-00-00-00-00");
  bp = eui64.bytes;
  *bp++ = 0x01;
  *bp++ = 0x12;
  *bp++ = 0x43;
  *bp++ = 0x84;
  *bp++ = 0xA5;
  *bp++ = 0xC6;
  *bp++ = 0x37;
  *bp++ = 0x8F;
  BSP430_UNITTEST_ASSERT_EQUAL_ASCIIZ(xBSP430eui64AsText(&eui64, buffer), "01-12-43-84-A5-C6-37-8F");
}

static void
testGood ()
{
  uBSP430eui64 eui64;
  char buffer[BSP430_EUI64_AS_TEXT_LENGTH];
  typedef struct sTestCase {
    const char * const text;
    uBSP430eui64 value;
  } sTestCase;
  const sTestCase cases[] = {
    { "01-12-43-84-A5-C6-37-8F", { .bytes = { 0x01, 0x12, 0x43, 0x84, 0xa5, 0xc6, 0x37, 0x8f } } },
    { "1-2-3-4-5-6-7-f", { .bytes = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x0f } } },
    { "-2----6--", { .bytes = { 0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00 } } },
  };
  int ci;
  int rc;

  for (ci = 0; ci < sizeof(cases)/sizeof(*cases); ++ci) {
    const sTestCase * tcp = cases + ci;
    size_t len = strlen(tcp->text);
    memset(&eui64, 0xa5, sizeof(eui64));
    rc = iBSP430eui64Parse(tcp->text, len, &eui64);
    BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, rc);
    BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, memcmp(&eui64, &tcp->value, sizeof(eui64)));
    cputs(xBSP430eui64AsText(&eui64, buffer));
  }
}

static void
testBad ()
{
  uBSP430eui64 eui64;
  char buffer[BSP430_EUI64_AS_TEXT_LENGTH];
  const char * cases[] = {
    "01-12-43-84-A5C6-37-8F",   /* missing separator */
    "1-2-3-4-S-6-7-f",          /* bad char */
    "-2----6-",                 /* too short */
  };
  int ci;
  int rc;

  for (ci = 0; ci < sizeof(cases)/sizeof(*cases); ++ci) {
    const char * cp = cases[ci];
    size_t len = strlen(cp);
    memset(&eui64, 0xa5, sizeof(eui64));
    rc = iBSP430eui64Parse(cp, len, &eui64);
    BSP430_UNITTEST_ASSERT_EQUAL_FMTd(-1, rc);
    cputs(xBSP430eui64AsText(&eui64, buffer));
  }
}

static void
testNull ()
{
  uBSP430eui64 eui64;
  memset(&eui64, 0, sizeof(eui64));
  BSP430_UNITTEST_ASSERT_TRUE(BSP430_EUI64_IS_NULL(&eui64));
  BSP430_UNITTEST_ASSERT_FALSE(BSP430_EUI64_IS_INVALID(&eui64));
  memset(&eui64, 0xff, sizeof(eui64));
  BSP430_UNITTEST_ASSERT_FALSE(BSP430_EUI64_IS_NULL(&eui64));
  BSP430_UNITTEST_ASSERT_TRUE(BSP430_EUI64_IS_INVALID(&eui64));
  memset(&eui64, 0xa5, sizeof(eui64));
  BSP430_UNITTEST_ASSERT_FALSE(BSP430_EUI64_IS_NULL(&eui64));
  BSP430_UNITTEST_ASSERT_FALSE(BSP430_EUI64_IS_INVALID(&eui64));
  BSP430_EUI64_INVALIDATE(&eui64);
  BSP430_UNITTEST_ASSERT_FALSE(BSP430_EUI64_IS_NULL(&eui64));
  BSP430_UNITTEST_ASSERT_TRUE(BSP430_EUI64_IS_INVALID(&eui64));
}

void main ()
{
  vBSP430platformInitialize_ni();
  vBSP430unittestInitialize();

  testAsText();
  testGood();
  testBad();
  testNull();

  vBSP430unittestFinalize();
}
