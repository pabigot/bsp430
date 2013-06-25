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
#include <bsp430/resource.h>

volatile unsigned int flag_v;

static void
testResourceFlag (void)
{
  const sBSP430resourceReleaseFlag flagd = {
    .flagp = &flag_v,
    .flagv = 0xA5
  };
  sBSP430resourceWaiter waiter = {
    .callback_ni = iBSP430resourceSetFlagOnRelease,
    .context = &flagd,
    .next = 0
  };
  sBSP430resource resource = {
    .waiter = &waiter,
  };
  int rc;

  cprintf("# testResourceFlag\n");
  flag_v = 0;
  rc = iBSP430resourceSetFlagOnRelease(&resource, resource.waiter);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(flag_v, flagd.flagv);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, BSP430_HAL_ISR_CALLBACK_EXIT_LPM);
}

static void
testWaiters (void)
{
  const sBSP430resourceReleaseFlag flagds[] = {
    { .flagp = &flag_v, .flagv = 0x0011 },
    { .flagp = &flag_v, .flagv = 0x8020 },
    { .flagp = &flag_v, .flagv = 0x0042 },
    { .flagp = &flag_v, .flagv = 0x4080 },
  };
  sBSP430resourceWaiter waiters[] = {
    { .callback_ni = iBSP430resourceSetFlagOnRelease, .context = flagds+0 },
    { .callback_ni = iBSP430resourceSetFlagOnRelease, .context = flagds+1 },
    { .callback_ni = iBSP430resourceSetFlagOnRelease, .context = flagds+2 },
    { .callback_ni = iBSP430resourceSetFlagOnRelease, .context = flagds+3 },
  };
  sBSP430resource resource;
  int rc;

  cprintf("# testWaiters\n");
  flag_v = 0;
  memset(&resource, 0, sizeof(resource));
  rc = iBSP430resourceClaim_ni(&resource, NULL, eBSP430resourceWait_NONE, NULL);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, 0);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(1, resource.count);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&resource, resource.holder);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, resource.waiter);

  rc = iBSP430resourceClaim_ni(&resource, NULL, eBSP430resourceWait_FIFO, waiters+0);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, -1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(1, resource.count);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&resource, resource.holder);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+0, resource.waiter);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, waiters[0].next);

  rc = iBSP430resourceClaim_ni(&resource, NULL, eBSP430resourceWait_LIFO, waiters+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, -1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(1, resource.count);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&resource, resource.holder);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+1, resource.waiter);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+0, waiters[1].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, waiters[0].next);

  rc = iBSP430resourceClaim_ni(&resource, NULL, eBSP430resourceWait_FIFO, waiters+2);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, -1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(1, resource.count);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&resource, resource.holder);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+1, resource.waiter);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+0, waiters[1].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+2, waiters[0].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, waiters[2].next);

  rc = iBSP430resourceClaim_ni(&resource, NULL, eBSP430resourceWait_LIFO, waiters+3);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, -1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(1, resource.count);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&resource, resource.holder);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+3, resource.waiter);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+1, waiters[3].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+0, waiters[1].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+2, waiters[0].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, waiters[2].next);

  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(0, flag_v);
  rc = iBSP430resourceRelease_ni(&resource, NULL);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, BSP430_HAL_ISR_CALLBACK_EXIT_LPM);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, resource.count);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, resource.holder);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+3, resource.waiter);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(flagds+3, waiters[3].context);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(flagds[3].flagv, flag_v);

  rc = iBSP430resourceClaim_ni(&resource, NULL, eBSP430resourceWait_FIFO, waiters+3);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, 0);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(1, resource.count);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&resource, resource.holder);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+1, resource.waiter);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+0, waiters[1].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+2, waiters[0].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, waiters[2].next);

  rc = iBSP430resourceRelease_ni(&resource, NULL);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, BSP430_HAL_ISR_CALLBACK_EXIT_LPM);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, resource.count);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, resource.holder);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+1, resource.waiter);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(flagds+1, waiters[1].context);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(flagds[3].flagv | flagds[1].flagv, flag_v);

  rc = iBSP430resourceClaim_ni(&resource, NULL, eBSP430resourceWait_FIFO, waiters+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, 0);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(1, resource.count);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&resource, resource.holder);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+0, resource.waiter);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+2, waiters[0].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, waiters[2].next);

  rc = iBSP430resourceRelease_ni(&resource, NULL);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, BSP430_HAL_ISR_CALLBACK_EXIT_LPM);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, resource.count);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, resource.holder);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+0, resource.waiter);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(flagds+0, waiters[0].context);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(flagds[3].flagv | flagds[1].flagv | flagds[0].flagv, flag_v);

  rc = iBSP430resourceClaim_ni(&resource, NULL, eBSP430resourceWait_FIFO, waiters+0);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, 0);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(1, resource.count);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&resource, resource.holder);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+2, resource.waiter);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, waiters[2].next);

  rc = iBSP430resourceRelease_ni(&resource, NULL);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, BSP430_HAL_ISR_CALLBACK_EXIT_LPM);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, resource.count);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, resource.holder);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+2, resource.waiter);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(flagds+2, waiters[2].context);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(flagds[3].flagv | flagds[1].flagv | flagds[0].flagv | flagds[2].flagv, flag_v);

  rc = iBSP430resourceClaim_ni(&resource, NULL, eBSP430resourceWait_FIFO, waiters+2);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, 0);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTu(1, resource.count);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&resource, resource.holder);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, resource.waiter);
}

static void
testMisorderedClaims (void)
{
  const sBSP430resourceReleaseFlag flagds[] = {
    { .flagp = &flag_v, .flagv = 0x0000 },
    { .flagp = &flag_v, .flagv = 0x1000 },
    { .flagp = &flag_v, .flagv = 0x0200 },
    { .flagp = &flag_v, .flagv = 0x0030 },
    { .flagp = &flag_v, .flagv = 0x0004 },
  };
  sBSP430resourceWaiter waiters[] = {
    { .callback_ni = iBSP430resourceSetFlagOnRelease, .context = flagds+0, .next = waiters+3 },
    { .callback_ni = iBSP430resourceSetFlagOnRelease, .context = flagds+1, .next = waiters+2 },
    { .callback_ni = iBSP430resourceSetFlagOnRelease, .context = flagds+2, .next = waiters+1 },
    { .callback_ni = iBSP430resourceSetFlagOnRelease, .context = flagds+3, .next = waiters+0 },
    { .callback_ni = iBSP430resourceSetFlagOnRelease, .context = flagds+4, .next = waiters+4 },
  };
  static const size_t nwaiters = sizeof(waiters)/sizeof(*waiters);
  int i;
  sBSP430resource resource;
  int rc;

  cprintf("# testMisorderedClaims\n");
  flag_v = 0;
  memset(&resource, 0, sizeof(resource));
  for (i = 0; i < nwaiters; ++i) {
    BSP430_UNITTEST_ASSERT_FALSE(NULL == waiters[i].next);
    rc = iBSP430resourceClaim_ni(&resource, waiters+i, eBSP430resourceWait_FIFO, waiters+i);
    BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, (0 == i) ? i : -1);
    BSP430_UNITTEST_ASSERT_EQUAL_FMTu(1, resource.count);
    if (0 == i) {
      BSP430_UNITTEST_ASSERT_FALSE(NULL == waiters[i].next);
      BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, resource.waiter);
    } else {
      if (1 < i) {
        BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+i, waiters[i-1].next);
      }
      BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, waiters[i].next);
    }
  }

  // Release, then claim from 3 leave 1 2 4
  rc = iBSP430resourceRelease_ni(&resource, waiters+0);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, BSP430_HAL_ISR_CALLBACK_EXIT_LPM);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(0x1000, flag_v);

  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+3, waiters[2].next);
  rc = iBSP430resourceClaim_ni(&resource, waiters+3, eBSP430resourceWait_FIFO, waiters+3);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, 0);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+1, resource.waiter);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+4, waiters[2].next);

  // Release, then claim from 4 leave 1 2
  flag_v = 0;
  rc = iBSP430resourceRelease_ni(&resource, waiters+3);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, BSP430_HAL_ISR_CALLBACK_EXIT_LPM);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(0x1000, flag_v);

  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, waiters[4].next);
  rc = iBSP430resourceClaim_ni(&resource, waiters+4, eBSP430resourceWait_FIFO, waiters+4);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, 0);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+1, resource.waiter);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, waiters[2].next);

  // Release, then claim from 2 leave 1
  flag_v = 0;
  rc = iBSP430resourceRelease_ni(&resource, waiters+4);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, BSP430_HAL_ISR_CALLBACK_EXIT_LPM);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(0x1000, flag_v);

  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+2, waiters[1].next);
  rc = iBSP430resourceClaim_ni(&resource, waiters+2, eBSP430resourceWait_FIFO, waiters+2);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, 0);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+1, resource.waiter);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, waiters[1].next);
}

void
testMultiClaim (void)
{
  const sBSP430resourceReleaseFlag flagds[] = {
    { .flagp = &flag_v, .flagv = 0x0000 },
    { .flagp = &flag_v, .flagv = 0x1000 },
    { .flagp = &flag_v, .flagv = 0x0200 },
  };
  sBSP430resourceWaiter waiters[] = {
    { .callback_ni = iBSP430resourceSetFlagOnRelease, .context = flagds+0, .next = waiters+3 },
    { .callback_ni = iBSP430resourceSetFlagOnRelease, .context = flagds+1, .next = waiters+2 },
    { .callback_ni = iBSP430resourceSetFlagOnRelease, .context = flagds+2, .next = waiters+1 },
  };
  static const size_t nwaiters = sizeof(waiters)/sizeof(*waiters);
  int i;
  sBSP430resource resource;
  int rc;

  cprintf("# testMultiClaim\n");
  flag_v = 0;
  memset(&resource, 0, sizeof(resource));
  for (i = 0; i < nwaiters; ++i) {
    BSP430_UNITTEST_ASSERT_FALSE(NULL == waiters[i].next);
    rc = iBSP430resourceClaim_ni(&resource, waiters+i, eBSP430resourceWait_FIFO, waiters+i);
    BSP430_UNITTEST_ASSERT_EQUAL_FMTd(rc, (0 == i) ? i : -1);
    BSP430_UNITTEST_ASSERT_EQUAL_FMTu(1, resource.count);
  }

  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+1, resource.waiter);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+2, waiters[1].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, waiters[2].next);

  for (i = 0; i < 4; ++i) {
    sBSP430resourceWaiter * wp = waiters + 1 + (i & 1);
    rc = iBSP430resourceClaim_ni(&resource, wp, eBSP430resourceWait_FIFO, wp);
    BSP430_UNITTEST_ASSERT_EQUAL_FMTd(-1, rc);
    BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+1, resource.waiter);
    BSP430_UNITTEST_ASSERT_EQUAL_FMTp(waiters+2, waiters[1].next);
    BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, waiters[2].next);
  }
}

int
callback_getval (hBSP430resource resource,
                 hBSP430resourceWaiter waiter)
{
  int * ip = (int *)waiter->context;
  return *ip;
}

void
testCallbackReturnValue (void)
{
  int callback_return;
  sBSP430resourceWaiter waiter = {
    .callback_ni = callback_getval,
    .context = &callback_return
  };
  sBSP430resource resource;
  int rc;

  cprintf("# testCallbackReturnValue\n");
  flag_v = 0;
  memset(&resource, 0, sizeof(resource));
  callback_return = 0x1234;
  rc = callback_getval(&resource, &waiter);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(rc, callback_return);

  rc = iBSP430resourceClaim_ni(&resource, NULL, eBSP430resourceWait_NONE, NULL);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, rc);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, resource.waiter);
  rc = iBSP430resourceClaim_ni(&resource, NULL, eBSP430resourceWait_FIFO, &waiter);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(-1, rc);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&waiter, resource.waiter);
  rc = iBSP430resourceRelease_ni(&resource, NULL);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTx(rc, callback_return);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&waiter, resource.waiter);
}

void main ()
{
  vBSP430platformInitialize_ni();
  vBSP430unittestInitialize();

  testResourceFlag();
  testWaiters();
  testMisorderedClaims();
  testMultiClaim();
  testCallbackReturnValue();

  vBSP430unittestFinalize();
}
