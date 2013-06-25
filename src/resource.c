/* Copyright 2013, Peter A. Bigot
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the software nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** @file
 *
 * @brief Definitions for shared HAL serial support.
 *
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/resource.h>
#include <bsp430/periph.h>

int
iBSP430resourceClaim_ni (hBSP430resource resource,
                         void * self,
                         eBSP430resourceWait wait_type,
                         hBSP430resourceWaiter waiter)
{
  volatile hBSP430resourceWaiter * wp = &resource->waiter;

  /* Claim succeeds if nobody holds the resource or if the requester
   * already holds the resource. */
  if ((0 == resource->count)
      || (self == resource->holder)) {
    if (0 == resource->count) {
      /* First-time success requires bookkeeping.  Record the holder
       * of the resource. */
      resource->holder = (NULL == self) ? resource : self;
      if (NULL != waiter) {
        /* Remove the waiter from the list (it should only be in there
         * once) */
        while (NULL != *wp) {
          if (waiter == *wp) {
            *wp = (*wp)->next;
            continue;
          }
          wp = &(*wp)->next;
        }
      }
    }
    resource->count += 1;
    return 0;
  }

  /* Register the waiter, if there is one to be registered and it's
   * not already registered. */
  if ((eBSP430resourceWait_NONE != wait_type) && (NULL != waiter)) {
    /* Make sure waiter not already in the queue */
    while (NULL != *wp) {
      if (waiter == *wp) {
        waiter = NULL;
        break;
      }
      wp = &(*wp)->next;
    }
    if (NULL != waiter) {
      if (eBSP430resourceWait_LIFO == wait_type) {
        waiter->next = resource->waiter;
        resource->waiter = waiter;
      } else if (eBSP430resourceWait_FIFO == wait_type) {
        *wp = waiter;
        waiter->next = NULL;
      }
    }
  }

  return -1;
}

int
iBSP430resourceRelease_ni (hBSP430resource resource,
                           void * self)
{
  int rv;

  /* If the resource is not held by whoever is attempting to release
   * it, error or deadlock for diagnostic purposes. */
  while (((NULL != self) && (resource->holder != self))
         || ((NULL == self) && (resource->holder != resource))) {
#if (BSP430_CORE_NDEBUG - 0)
    return -1;
#endif /* BSP430_CORE_NDEBUG */
  }
  if (0 < resource->count) {
    resource->count -= 1;
  }
  rv = 0;
  if (0 == resource->count) {
    resource->holder = NULL;
    /* Notify whoever's next in the queue, if anybody.  Note that the
     * callback is entitled to try to claim the resource, so the
     * waiter list must not be accessed after this. */
    if (resource->waiter) {
      rv = resource->waiter->callback_ni(resource, resource->waiter);
    }
  }
  return rv;
}

int
iBSP430resourceCancelWait_ni (hBSP430resource resource,
                              hBSP430resourceWaiter waiter)
{
  volatile hBSP430resourceWaiter * wp = &resource->waiter;
  int num_removals = 0;

  /* Remove the waiter from the list.  It should only appear there
   * once, but we'll check the whole thing. */
  while (NULL != *wp) {
    if (waiter == *wp) {
      *wp = (*wp)->next;
      ++num_removals;
      continue;
    }
    wp = &(*wp)->next;
  }
  return (0 == num_removals) ? -1 : 0;
}

int
iBSP430resourceSetFlagOnRelease (hBSP430resource resource,
                                 hBSP430resourceWaiter waiter)
{
  const sBSP430resourceReleaseFlag * fi = (const sBSP430resourceReleaseFlag *)(waiter->context);
  *(fi->flagp) |= fi->flagv;
  return BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
}
