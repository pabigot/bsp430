/* Copyright 2013-2014, Peter A. Bigot
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
 * @brief Support for resource management in BSP430
 *
 * BSP430 allows a large number of peripheral resources to be shared
 * among subsystems with loose coupling.  For example, different
 * source modules can share the interrupt capabilities of a digital
 * port vector without requiring that the interrupt handler be
 * modified.  In this case the resource is a pin on the port, and that
 * resource itself is not normally shared among different subsystems.
 *
 * Experience has indicated that in some cases other peripherals, such
 * as a SPI or I2C bus, may need to perform asynchronous activities
 * using a resource without explicit coordination between systems.
 *
 * In the original conception of BSP430 this was achieved by making
 * all uses of the resource non-interruptible and run-to-completion.
 * This has proved to be inadequate, especially in cases where the
 * resource may be used for an extended period during which realtime
 * needs require that interrupts be processed promptly.
 *
 * The #sBSP430resource structure and the API defined in this file
 * support sharing these resources with a mutual exclusion capability
 * similar to semaphores.  Atomicity is ensured by disabling
 * interrupts during all manipulation of resource structures.  While
 * the resource infrastructure itself will not block, it does provide
 * a mechanism for subsystems to register a callback so they are
 * notified of changes in the resource allocation state.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2013-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_RESOURCE_H
#define BSP430_RESOURCE_H

#include <bsp430/core.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Forward declaration */
struct sBSP430resourceWaiter;

/** Structure holding mutual exclusion data associated with some
 * system resource.
 *
 * Each resource structure should be initialized to all zeros prior to
 * any possible reference to it. */
typedef struct sBSP430resource {
  /** The identifier associated with whatever subsystem is holding
   * this resource.  This is the @p self parameter in a successful
   * iBSP430resourceClaim_ni() call. */
  void * volatile holder;

  /** The number of times the current holder has claimed the resource
   * minus the number of times it has released the resource.  A
   * non-zero value indicates that the resource is held. */
  unsigned int volatile count;

  /** Pointer to a sequence of records identifying subsystems that
   * wish to be notified when the resource is released.  These are
   * maintained in priority order, influenced by
   * #eBSP430resourceWait. */
  struct sBSP430resourceWaiter * volatile waiter;
} sBSP430resource;

/** A handle for a specific system resource */
typedef struct sBSP430resource * hBSP430resource;

/** The callback to provide a subsystem notification of a change in a
 * resource's state.
 *
 * This callback is invoked by iBSP430resourceRelease_ni() when a
 * resource is released, or if the wait list changes so that a new
 * head may be able to claim the resource (possibly recursively).  The
 * API specifically does not guarantee that the resource is in fact
 * available when the callback is invoked.
 *
 * The callback is explicitly permitted to invoke
 * iBSP430resourceClaim_ni() in an attempt to claim the resource.
 * Alternatively, it may set a flag and make the attempt to claim or
 * decision to release at a later time, or invoke
 * iBSP430resourceCancelWait_ni() if it no longer needs to be on the
 * wait queue.
 *
 * @param resource the resource that has just been released
 *
 * @param waiter the waiter record provided by the subsystem that
 * wishes to be notified on resource availability.
 *
 * @return An integral value consistent with @ref callback_retval that
 * allows the notified subsystem to affect subsequent execution if the
 * resource was released during an ISR top half. */
typedef int (* iBSP430resourceWaitCallback_ni) (hBSP430resource resource,
                                                struct sBSP430resourceWaiter * waiter);

/** Structure registering a subsystem that needs to be informed when a
 * resource is released. */
typedef struct sBSP430resourceWaiter {
  /** The function called by iBSP430resourceRelease_ni() */
  iBSP430resourceWaitCallback_ni callback_ni;

  /** Any additional information of value to the @a callback_ni
   * function.  Examples might be a pointer to
   * #sBSP430resourceReleaseFlag if @a callback_ni is
   * iBSP430resourceSetFlagOnRelease(), or a pointer to the subsystem
   * requesting the resource if @a callback_ni will attempt to invoke
   * iBSP430resourceClaim_ni(). */
  const void * context;

  /** The next waiting subsystem in decreasing priority order. */
  struct sBSP430resourceWaiter * volatile next;
} sBSP430resourceWaiter;

/** Instructions for how a subsystem may prioritize itself on a list
 * of #sBSP430resourceWaiter instances.  Used in iBSP430resourceClaim_ni(). */
typedef enum eBSP430resourceWait {
  /** Indicate that iBSP430resourceClaim_ni() does not register a
   * waiter if the resource is already in use */
  eBSP430resourceWait_NONE,

  /** Indicate that iBSP430resourceClaim_ni() should add this waiter
   * to the end of the queue if the resource is already in use.
   *
   * If the waiter is already in the queue its position is not
   * changed. */
  eBSP430resourceWait_FIFO,

  /** Indicate that iBSP430resourceClaim_ni() should add this waiter
   * to the beginning of the queue if the resource is already in use.
   *
   * If the waiter is already in the queue its position is not
   * changed. */
  eBSP430resourceWait_LIFO,
} eBSP430resourceWait;

/** A handle for a structure holding information on a subsystem awaiting a resource */
typedef struct sBSP430resourceWaiter * hBSP430resourceWaiter;

/** Attempt to claim the right to use a resource.
 *
 * This is a non-blocking call.  The caller is granted the right to
 * use the resource if the resource is not in use, or if the @link
 * sBSP430resource::holder holder@endlink of the resource is @p self.
 * In either case, sBSP430resource::count is incremented, and 0 is
 * returned.
 *
 * If the resource is in use by another holder, the attempt to claim
 * fails and -1 is returned.
 *
 * If the resource claim succeeds, any appearance of @p waiter is
 * removed from the resource sBSP430resource::waiter list prior to
 * return.  If the resource claim fails and @p waiter is not a null
 * pointer, @p waiter is added into the sBSP430resource::waiter waiter
 * list in the order specified by @p wait_type if absent from that
 * list, and left in its original position if already present in the
 * list.
 *
 * @note BSP430 does not aspire to be an RTOS, and the weak
 * prioritization supported by @p wait_type is not affected by
 * repeated failed resource claim attempts.  If necessary the waiter
 * can be reprioritized by @link iBSP430resourceCancelWait_ni()
 * removing it@endlink and making another attempt to claim the
 * resource.
 *
 * @param resource a pointer to the structure associated with a
 * resource that may be shared among subsystems.
 *
 * @param self an identifier unique to the requesting subsystem.  If
 * this is a null pointer, attempts to claim the resource will succeed
 * only if it is not in use; in that situation, in a successful claim
 * the holder is set to be the resource itself.  In many cases, @p
 * waiter is an appropriate unique value to use for @p self when a
 * recursive mutex is required, and @c NULL when a non-recursive mutex
 * is required.
 *
 * @param wait_type How @p waiter should be prioritized relative to
 * any other waiters that may already be in the queue
 *
 * @param waiter An optional structure that can be used to notify the
 * calling subsystem when the resource may be available.  A null
 * pointer is used when the caller will retry as needed without a
 * notification.
 *
 * @return 0 if the resource was successfully claimed.  A negative
 * value if it could not be claimed.
 */
int iBSP430resourceClaim_ni (hBSP430resource resource,
                             void * self,
                             eBSP430resourceWait wait_type,
                             hBSP430resourceWaiter waiter);

/** Release a resource held by a subsystem.
 *
 * Note that the return value does not indicate whether the resource
 * is still held by this subsystem.  If the resource was released, the
 * first waiter on the resource's waiter list (if any) will be
 * notified.
 *
 * @param resource a pointer to the structure associated with a
 * resource that may be shared among subsystems.
 *
 * @param self as in iBSP430resourceClaim_ni().  If @p self does not
 * match the @link sBSP430resource::holder holder@endlink of the
 * resource the application will spin in place (if #BSP430_CORE_NDEBUG
 * is zero) or return a negative error code (if #BSP430_CORE_NDEBUG is
 * nonzero).
 *
 * @return A negative value if an error occurs; 0 if the release was
 * successful and there are no subsystems queued to be notified on
 * release; otherwise the result of invoking the
 * sBSP430resourceWaiter::callback_ni function of the highest-priority
 * waiter.
 */
int iBSP430resourceRelease_ni (hBSP430resource resource,
                               void * self);

/** Remove @p waiter from the queue for @p resource.
 *
 * This function is used when a subsystem that has requested a
 * resource determines that the resource is no longer needed.
 *
 * If @p waiter was at the head of the resource's wait queue the
 * callback of the new head of the queue (if any) will be recursively
 * invoked and its return value used as the return value of this
 * function.  This ensures that invocation of this function from
 * within an iBSP430resourceWaitCallback_ni() will not cause the
 * notification of availability to be lost, and that recursive
 * resource allocation is satisfied as soon as possible.
 *
 * @note There is no validation (and no penalty) if @p waiter is not
 * on the @p resource wait list.
 *
 * @param resource a pointer to the structure associated with a shared
 * resource
 *
 * @param waiter the wait handle used for notification when the
 * resource becomes available.
 *
 * @return The return value from the callback to the new head if that
 * was invoked, or zero.
 */
int iBSP430resourceCancelWait_ni (hBSP430resource resource,
                                  hBSP430resourceWaiter waiter);

/** A record identifying a flag and the bits in it that should be set
 * when a resource is released.
 *
 * A pointer to an instance of this type should be stored in the
 * sBSP430resourceWaiter::context field of a waiter that uses
 * iBSP430resourceSetFlagOnRelease() as its @a
 * sBSP430resourceWaiter::callback_ni. */
typedef struct sBSP430resourceReleaseFlag {
  /** Pointer to an integer holding one or more flags */
  volatile unsigned int * flagp;

  /** A mask identifying one or more bits that are set in @a *flagp by
   * iBSP430resourceSetFlagOnRelease(). */
  unsigned int flagv;
} sBSP430resourceReleaseFlag;

/** Function conforming to #iBSP430resourceWaitCallback_ni
 *
 * In many cases the release of a resource can be communicated to a
 * waiting subsystem by setting a flag in the subsystem's event set
 * and waking the system to process pending events.
 *
 * @param resource the resource that is being released
 *
 * @param waiter the waiter record provided by the subsystem in an
 * unsuccessful call to iBSP430resourceClaim_ni().
 *
 * @return #BSP430_HAL_ISR_CALLBACK_EXIT_LPM */
int iBSP430resourceSetFlagOnRelease (hBSP430resource resource,
                                     hBSP430resourceWaiter waiter);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSP430_RESOURCE_H */
