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
#include <bsp430/periph.h>


const sBSP430halISRVoidChainNode * volatile root;
sBSP430halISRVoidChainNode n1;
sBSP430halISRVoidChainNode n2;
sBSP430halISRVoidChainNode n3;

void main ()
{
  vBSP430platformInitialize_ni();
  vBSP430unittestInitialize();

  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, root);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, n1.next_ni);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, n2.next_ni);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, n3.next_ni);

  BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRVoidChainNode, root, n1, next_ni);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&n1, root);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, n1.next_ni);

  BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRVoidChainNode, root, n2, next_ni);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&n2, root);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&n1, n2.next_ni);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, n1.next_ni);

  /* Unlinking an unlinked node has no effect on the chain */
  BSP430_HAL_ISR_CALLBACK_UNLINK_NI(sBSP430halISRVoidChainNode, root, n3, next_ni);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&n2, root);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&n1, n2.next_ni);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, n1.next_ni);

  /* Unlinking an internal node doesn't affect the root */
  BSP430_HAL_ISR_CALLBACK_UNLINK_NI(sBSP430halISRVoidChainNode, root, n1, next_ni);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&n2, root);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, n2.next_ni);
  /* Unlinking cleans up */
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, n1.next_ni);

  /* Unlinking the last node clears the root */
  BSP430_HAL_ISR_CALLBACK_UNLINK_NI(sBSP430halISRVoidChainNode, root, n2, next_ni);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, root);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, n2.next_ni);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, n1.next_ni);

  vBSP430unittestFinalize();
}
