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
#include <bsp430/chips/ds3231.h>

void main ()
{
  sDS3231registers regs;
  struct tm tms;
  struct tm tms2;

  vBSP430platformInitialize_ni();
  vBSP430unittestInitialize();

  /* 24-hour encodings */
#define CHECK(e_,d_) do {                                               \
    BSP430_UNITTEST_ASSERT_EQUAL_FMTx(e_, ucDS3231encodeHour(d_, 0));   \
    BSP430_UNITTEST_ASSERT_EQUAL_FMTu(d_, uiDS3231decodeHour(e_));      \
  } while (0)
  CHECK(0x09, 9);
  CHECK(0x10, 10);
  CHECK(0x12, 12);
  CHECK(0x13, 13);
  CHECK(0x19, 19);
  CHECK(0x20, 20);
  CHECK(0x23, 23);
#undef CHECK

  /* 12-hour encodings */
#define CHECK(e_,d_) do {                                               \
    BSP430_UNITTEST_ASSERT_EQUAL_FMTx(e_, ucDS3231encodeHour(d_, 1));   \
    BSP430_UNITTEST_ASSERT_EQUAL_FMTu(d_, uiDS3231decodeHour(e_));      \
  } while (0)
  CHECK(0x49, 9);
  CHECK(0x50, 10);
  CHECK(0x72, 12);
  CHECK(0x61, 13);
  CHECK(0x67, 19);
  CHECK(0x68, 20);
  CHECK(0x71, 23);
#undef CHECK

  memset(&regs, 0, sizeof(regs));
  memset(&tms, 0, sizeof(tms));
  memset(&tms2, 0, sizeof(tms2));
  tms.tm_sec = 55;
  tms.tm_min = 59;
  tms.tm_hour = 11;
  tms.tm_mday = 31;
  tms.tm_mon = 4;
  tms.tm_year = 113;
  tms.tm_wday = 5;
  xDS3231tmToRegisters(&tms, &regs);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(0x55, regs.seconds);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(0x59, regs.minutes);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(0x11, regs.hours);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(tms.tm_wday+1, regs.dow);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(0x31, regs.dom);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(0x85, regs.mon_cen);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(0x13, regs.year);
  xDS3231registersToTm(&regs, &tms2);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(tms.tm_sec, tms2.tm_sec);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(tms.tm_min, tms2.tm_min);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(tms.tm_hour, tms2.tm_hour);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(tms.tm_mday, tms2.tm_mday);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(tms.tm_mon, tms2.tm_mon);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(tms.tm_year, tms2.tm_year);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(tms.tm_wday, tms2.tm_wday);

  vBSP430unittestFinalize();
}
