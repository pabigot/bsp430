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

#include <bsp430/platform.h>
#include <bsp430/utility/event.h>
#include <bsp430/utility/uptime.h>
#include <string.h>

#if (BSP430_EVENT_RECORD_NUM_SUPPORTED > 255)
#error Too many event tags
#endif /* BSP430_EVENT_RECORD_NUM_SUPPORTED */
#if (BSP430_EVENT_RECORD_NUM_SUPPORTED > 255)
#error Too many event records
#endif /* BSP430_EVENT_RECORD_NUM_SUPPORTED */

unsigned char nBSP430eventTagConfig_;
sBSP430eventTagConfig xBSP430eventTagConfig_[BSP430_EVENT_TAG_NUM_SUPPORTED];

static volatile sBSP430eventTagRecord xBSP430eventRecord[BSP430_EVENT_RECORD_NUM_SUPPORTED];
static volatile size_t event_lost_count;
static volatile uint8_t event_head;
static volatile uint8_t event_tail;
volatile unsigned int uiBSP430eventFlags_v_;

unsigned int
uiBSP430eventFlagAllocate ()
{
  static unsigned int event_flag_last;
  unsigned int rv;

  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    unsigned long fv = event_flag_last;
    if (! fv) {
      fv = uiBSP430eventFlag_EventRecord;
    }
    fv <<= 1;
    rv = fv;
    while (! rv) {
    }
    event_flag_last = rv;
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

unsigned char
ucBSP430eventTagAllocate (const char * id)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  unsigned char rc = 0;
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    sBSP430eventTagConfig * cp = xBSP430eventTagConfig_ + nBSP430eventTagConfig_;
    if (0 == nBSP430eventTagConfig_) {
      cp->id = "LostEventRecord";
      ++cp;
      ++nBSP430eventTagConfig_;
    }
    /* Spin here if creating the configuration would overflow the
     * config array */
    while (sizeof(xBSP430eventTagConfig_)/sizeof(*xBSP430eventTagConfig_) <= nBSP430eventTagConfig_) {
    }
    cp->id = id;
    cp->seqno = 0;
    rc = (cp - xBSP430eventTagConfig_);
    ++nBSP430eventTagConfig_;
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rc;
}

#define EVENT_COUNT() ((event_head > event_tail) ? (event_head - event_tail) : (event_tail - event_head))
#define EVENT_EMPTY() (event_head == event_tail)
#define EVENT_INCREMENT_PTR(_p) do {                    \
    if (++(_p) == BSP430_EVENT_RECORD_NUM_SUPPORTED) {  \
      _p = 0;                                           \
    }                                                   \
  } while (0)

const volatile sBSP430eventTagRecord *
xBSP430eventRecordEvent_ni (unsigned char tag,
                            unsigned char flags,
                            const uBSP430eventAnyType * up)
{
  volatile sBSP430eventTagRecord * ep = xBSP430eventRecord + event_head;
  ep->tag = tag;
  ep->flags = flags;
  if (up) {
    ep->u = *up;
  } else {
    memset((unsigned char *)&ep->u, 0, sizeof(ep->u));
  }
  ep->timestamp_utt = ulBSP430uptime_ni();
  if (tag < nBSP430eventTagConfig_) {
    ep->seqno = xBSP430eventTagConfig_[tag].seqno++;
  }
  vBSP430eventFlagsSet_ni(uiBSP430eventFlag_EventRecord);
  EVENT_INCREMENT_PTR(event_head);
  if (event_head == event_tail) {
    EVENT_INCREMENT_PTR(event_tail);
    ++event_lost_count;
  }
  return ep;
}

int
iBSP430eventTagGetRecords (sBSP430eventTagRecord * evts,
                           int len)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  int nevt = 0;

  if (0 == len) {
    return len;
  }
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    if (0 != event_lost_count) {
      memset(evts, 0, sizeof(*evts));
      evts->tag = ucBSP430eventTag_LostEventRecord;
      evts->u.sz = event_lost_count;
      event_lost_count = 0;
      ++nevt;
    }
    while ((nevt < len) && !EVENT_EMPTY()) {
      evts[nevt++] = xBSP430eventRecord[event_tail];
      EVENT_INCREMENT_PTR(event_tail);
    }
    if (!EVENT_EMPTY()) {
      vBSP430eventFlagsSet_ni(uiBSP430eventFlag_EventRecord);
    }
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return nevt;
}
