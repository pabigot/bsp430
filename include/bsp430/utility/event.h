/* Copyright 2014, Peter A. Bigot
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
 * @brief Support for recording timestamped and fact-of events
 *
 * Many event-driven programs need ways to communicate information
 * from interrupt handlers to the main processing loop.  This module
 * provides facilities for two types of information:
 *
 * @li Timestamped tagged events held in a circular buffer
 * @li Flagged events held in a bit vector
 *
 * The BSP430 infrastructure cannot know how what events are used by
 * an application.  Nor can an application know what events might be
 * used by libraries or modules it links to.  This means the
 * identifying values for distinct events cannot be defined at
 * compile-time or conflicts might be produced.  Instead, this module
 * provides a facility to assign event identifiers during application
 * and component initialization.
 *
 * An event "tag" is an ordinal created by ucBSP430eventTagAllocate()
 * and used to identify the specific event held in an internal
 * circular buffer of capacity #BSP430_EVENT_RECORD_NUM_SUPPORTED.
 * The number of permitted tags is a compile-time constant
 * #BSP430_EVENT_TAG_NUM_SUPPORTED.  The tag
 * #ucBSP430eventTag_LostEventRecord is pre-allocated and is used by
 * iBSP430eventTagGetRecords() to indicate the loss of older events due
 * to internal buffer overrun.
 *
 * An event "flag" is a bit assigned by uiBSP430eventFlagAllocate()
 * and is used to identify occurrence or presence of an edge or level
 * event.  The number of permitted flags is the number of bits in an
 * unsigned int.  The flag #uiBSP430eventFlag_EventRecord is
 * pre-allocated and is used by xBSP430eventRecordEvent_ni() to
 * indicate that unconsumed events need to be processed by
 * iBSP430eventTagGetRecords().
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_EVENT_H
#define BSP430_UTILITY_EVENT_H

#include <bsp430/core.h>

/** A utility union that allows event records to hold one value of any
 * scalar type not exceeding 32 bits.  The union field that is valid
 * for a particular even tag should be documented at the point the tag
 * is declared. */
typedef union uBSP430eventAnyType {
  char c;
  unsigned char uc;
  int i;
  unsigned int ui;
  long l;
  unsigned long ul;
  uint8_t u8;
  int8_t i8;
  uint16_t u16;
  int16_t i16;
  uint32_t u32;
  int32_t i32;
  size_t sz;
  void * p;
  void (* fp)();
} uBSP430eventAnyType;

/** Information about a particular allocated event tag.
 *
 * @see xBSP430eventTagLookup(). */
typedef struct sBSP430eventTagConfig {
  /** A short identifier that can be displayed in diagnostics */
  const char * id;
  /** A sequence number used to distinguish events with the corresponding tag. */
  volatile unsigned int seqno;
} sBSP430eventTagConfig;

#if defined(BSP430_DOXYGEN) || ! defined(BSP430_EVENT_TAG_NUM_SUPPORTED)
/** The number of event tags that the application will support.  The
 * value must be at least 2, and cannot exceed 255.  The per-tag
 * memory required is the size of #sBSP430eventTagConfig. */
#define BSP430_EVENT_TAG_NUM_SUPPORTED 8
#endif /* BSP430_EVENT_TAG_NUM_SUPPORTED */

#if defined(BSP430_DOXYGEN) || ! defined(BSP430_EVENT_RECORD_NUM_SUPPORTED)
/** The capacity of the tagged event record buffer.  The value must be
 * at least 2 and cannot exceed 255.  Using a power-of-2 may produce
 * slightly faster code.  The per-record memory required is the size
 * of #sBSP430eventTagRecord. */
#define BSP430_EVENT_RECORD_NUM_SUPPORTED 16
#endif /* BSP430_EVENT_RECORD_NUM_SUPPORTED */

/** LostEventRecord is generated when extracting pending events if the
 * ring buffer overflowed.  The number of events that were lost is
 * available in the sBSP430eventRecord::u::sz field.  The
 * sBSP430eventRecord::flags field is not used. */
static const unsigned char ucBSP430eventTag_LostEventRecord = 0;

/** EventRecord is flagged whenever there is at least one record in
 * the tagged event buffer. */
static const unsigned int uiBSP430eventFlag_EventRecord = 0x01;

/** A tagged event record */
typedef struct sBSP430eventTagRecord {
  /** Tag-specific additional data.  The union field used by the tag
   * should be identified by the tag documentation. */
  uBSP430eventAnyType u;

  /** The ulBSP430uptime() value at the time the event was
   * recorded. */
  unsigned long timestamp_utt;

  /** The sequence number of the event within its tag. */
  unsigned int seqno;

  /** The tag identifier from ucBSP430eventTagAllocate().  Use
   * hBSP430eventTagLookup() to obtain diagnostic information about
   * the tag. */
  unsigned char tag;

  /** Tag-specific flags.  The semantics of bits in this field should
   * be described in the tag documentation. */
  unsigned char flags;
} sBSP430eventTagRecord;

/** @cond DOXYGEN_EXCLUDE */
/* Not public API: the number of tags currently allocated */
extern unsigned char nBSP430eventTagConfig_;
/* Not public API: the configuration data for all tags */
extern sBSP430eventTagConfig xBSP430eventTagConfig_[];
/* Not public API: the event flag state */
extern volatile unsigned int uiBSP430eventFlags_v_;
/** @endcond */

/** Obtain information such as diagnostic text associated with the tag
 * when it was allocated.
 *
 * @param tag a tag as returned by ucBSP430eventTagAllocate().
 *
 * @return the tag configuration data, or a null pointer if @p tag is
 * not a valid allocated tag. */
static BSP430_CORE_INLINE
const sBSP430eventTagConfig *
hBSP430eventTagLookup (unsigned char tag)
{
  if (tag < nBSP430eventTagConfig_) {
    return xBSP430eventTagConfig_ + tag;
  }
  return NULL;
}

/** Allocate a new tag.
 *
 * Since BSP430 cannot know what events are of interest to an
 * application, and some events may be defined internal to libraries
 * the application uses, tags cannot be predefined as constants.  This
 * function should be invoked by initialization code for any
 * capability that uses tags, producing a value that is constant for
 * the remainder of the execution.
 *
 * @warning If allocation would exceed #BSP430_EVENT_TAG_NUM_SUPPORTED
 * tags this function will spin in place, allowing the developer to
 * detect what would otherwise be a memory access error.  Since tag
 * allocation should never be data-dependent, allocation violations
 * will be immediately visible during testing.
 *
 * @param id text used by generic code to identify the tag
 *
 * @return a unique tag */
unsigned char ucBSP430eventTagAllocate (const char * id);

/** Allocate a new flag.
 *
 * As with ucBSP430eventTagAllocate(), the flags that are required by
 * an application or its libraries cannot be compile-time constants.
 *
 * @warning If no more event flags are available this function will
 * spin in place, allowing the developer to detect what would
 * otherwise be a memory access error.  Since flag allocation should
 * never be data-dependent, allocation violations will be immediately
 * visible during testing.
 *
 * @return a unique unsigned int value with a single bit set. */
unsigned int uiBSP430eventFlagAllocate ();

/** Test whether there are unhandled event flags.
 *
 * @return 0 if there are no unhandled flags, nonzero if there are
 * unhandled flags. */
static BSP430_CORE_INLINE
int iBSP430eventFlagsEmpty_ni ()
{
  return !uiBSP430eventFlags_v_;
}

/** Interrupt-safe wrapper around iBSP430eventFlagsEmpty_ni() */
static BSP430_CORE_INLINE
int iBSP430eventFlagsEmpty ()
{
  int rv;
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    rv = iBSP430eventFlagsEmpty_ni();
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/** Transfer responsibility for unhandled flags to the application.
 *
 * This returns the current set of unhandled flags, and clears them
 * from the pending set.  Any flags that the application cannot handle
 * immediately may be returned to the infrastructure using
 * vBSP430eventFlagsSet_ni(). */
static BSP430_CORE_INLINE
unsigned int uiBSP430eventFlagsGet_ni ()
{
  unsigned int rv = uiBSP430eventFlags_v_;
  uiBSP430eventFlags_v_ = 0;
  return rv;
}

/** Interrupt-safe wrapper around uiBSP430eventFlagsGet_ni() */
static BSP430_CORE_INLINE
unsigned int uiBSP430eventFlagsGet ()
{
  unsigned int rv;
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    rv = uiBSP430eventFlagsGet_ni();
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/** Record the need to handle one or more flags
 *
 * Any flags set in @p flags will be added to the currently pending
 * set of flags.
 *
 * @param flags flags that must be handled.
 */
static BSP430_CORE_INLINE
void vBSP430eventFlagsSet_ni (unsigned int flags)
{
  uiBSP430eventFlags_v_ |= flags;
}

/** Interrupt-safe wrapper around vBSP430eventFlagsSet_ni() */
static BSP430_CORE_INLINE
void vBSP430eventFlagsSet (unsigned int flags)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    vBSP430eventFlagsSet_ni(flags);
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
}

/** Record an event along with its metadata.
 *
 * @param tag the tag identifying the type of event
 *
 * @param flags any tag-specific flags that provide information about
 * the event
 *
 * @param up pointer to any additional tag-specific metadata.  If
 * null, the sBSP430eventTagRecord::u field is cleared.
 *
 * @return a pointer to the completed event record, providing the
 * event timestamp. */
const volatile sBSP430eventTagRecord *
xBSP430eventRecordEvent_ni (unsigned char tag,
                            unsigned char flags,
                            const uBSP430eventAnyType * up);

/** Interrupt-safe wrapper around xBSP430eventRecordEvent_ni(). */
static BSP430_CORE_INLINE
void vBSP430eventRecordEvent (unsigned char tag,
                              unsigned char flags,
                              const uBSP430eventAnyType * up)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    xBSP430eventRecordEvent_ni(tag, flags, up);
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
}

/** Transfer tagged events from the infrastructure to the application.
 *
 * This function should be invoked whenever
 * #uiBSP430eventFlag_EventRecord is set.
 *
 * @param evts space into which tagged event records may be copied
 *
 * @param len the maximum number of tagged event records that may be
 * copied into @p evts
 *
 * @return the actual number of tagged event records that were copied
 * into @p evts */
int iBSP430eventTagGetRecords (sBSP430eventTagRecord * evts,
                               int len);

#endif /* BSP430_UTILITY_EVENT_H */
