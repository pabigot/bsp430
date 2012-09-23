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
#include <bsp430/utility/console.h>
#include <bsp430/utility/cli.h>
#include <string.h>

static void
testNextToken (void)
{
  const char * command = "  one two ";
  size_t last_remaining;
  size_t remaining = strlen(command);
  const char * key;
  size_t len;

  last_remaining = remaining;
  key = xBSP430cliNextToken(command, &remaining, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(len, 3);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(key, command+2);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(last_remaining-2, remaining);
  last_remaining = remaining -= len;
  command = key + len;
  key = xBSP430cliNextToken(command, &remaining, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(len, 3);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(key, command+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(last_remaining-1, remaining);
  last_remaining = remaining -= len;
  command = key + len;
  key = xBSP430cliNextToken(command, &remaining, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(len, 0);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(key, command+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(0, remaining);
}

void main (void)
{
  vBSP430platformInitialize_ni();
  vBSP430unittestInitialize();

  testNextToken();

  vBSP430unittestFinalize();
}

