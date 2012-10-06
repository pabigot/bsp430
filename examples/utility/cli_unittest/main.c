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

void
testNextToken (void)
{
  const char * command = "  one two ";
  const char * mcommand = command;
  size_t last_remaining;
  size_t remaining = strlen(command);
  const char * key;
  size_t len;

  last_remaining = remaining;
  key = xBSP430cliNextToken(&mcommand, &remaining, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(len, 3);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(key, command+2);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(last_remaining-5, remaining);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(mcommand, key + len);
  last_remaining = remaining;
  command = mcommand;
  key = xBSP430cliNextToken(&mcommand, &remaining, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(len, 3);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(key, command+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(last_remaining-4, remaining);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(mcommand, key + len);
  last_remaining = remaining;
  command = mcommand;
  key = xBSP430cliNextToken(&mcommand, &remaining, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(len, 0);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(key, command+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(0, remaining);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(mcommand, key + len);
}

void
testNextQToken (void)
{
  const char * command;
  const char * mcommand;
  size_t command_len;
  size_t len;
  const char * tp;

#define SET_INPUT(str_) do {                    \
    mcommand = command = str_;                  \
    command_len = strlen(str_);                 \
  } while (0)                                   \
 
  SET_INPUT("'one two'");
  tp = xBSP430cliNextQToken(&mcommand, &command_len, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(tp, command+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(7,len);

  SET_INPUT("'one two");
  tp = xBSP430cliNextQToken(&mcommand, &command_len, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(tp, command);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(4, len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(mcommand, command+4);
  tp = xBSP430cliNextQToken(&mcommand, &command_len, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(tp, command+5);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(3, len);

  SET_INPUT("''");
  tp = xBSP430cliNextQToken(&mcommand, &command_len, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(tp, command+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(0,len);

  SET_INPUT("\"\"");
  tp = xBSP430cliNextQToken(&mcommand, &command_len, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(tp, command+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(0,len);

  SET_INPUT("'one'x two");
  tp = xBSP430cliNextQToken(&mcommand, &command_len, &len);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(tp, command);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(6, len);

#undef SET_INPUT
}

void
testConsoleBufferExtend (void)
{
  const char * p;
  int rv;
  
  vBSP430cliConsoleBufferClear_ni();
  p = xBSP430cliConsoleBuffer_ni();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(0, strlen(p));
  rv = iBSP430cliConsoleBufferExtend_ni("one", 1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rv, 1);
  p = xBSP430cliConsoleBuffer_ni();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(1, strlen(p));
  BSP430_UNITTEST_ASSERT_EQUAL_ASCIIZ("o", p);
  rv = iBSP430cliConsoleBufferExtend_ni("ne", (size_t)-1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rv, 2);
  p = xBSP430cliConsoleBuffer_ni();
  BSP430_UNITTEST_ASSERT_EQUAL_ASCIIZ("one", p);
}

void main (void)
{
  vBSP430platformInitialize_ni();
  vBSP430unittestInitialize();

  testNextToken();
  testNextQToken();
  testConsoleBufferExtend();

  vBSP430unittestFinalize();
}

