/* Copyright (c) 2012, Peter A. Bigot <bigotp@acm.org>
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

#include <bsp430/utility/onewire.h>
#include <bsp430/clock.h>

/** Define protocol state times in microseconds. */
enum {
  /** Minimum time to hold bus low to ensure reset */
  OWT_RSTL_us = 480,

  /** Time to wait for presence detection after reset to quiesce */
  OWT_RSTH_us = 480,

  /** Delay before presence pulse is known to be valid (15us..60us) */
  OWT_PDHIGH_us = 60,

  /** Minimum time to hold bus low when writing a zero */
  OWT_LOW0_us = 60,

  /** Minimum time to hold bus low when writing a one */
  OWT_LOW1_us = 1,

  /** Recovery delay between write/read transaction cycles */
  OWT_REC_us = 1,

  /** Time to hold bus low when initiating a read slot */
  OWT_INT_us = 1,

  /** Point at which read value should be sampled */
  OWT_RDV_us = 15 - OWT_INT_us,

  /** Minimum duration of a read or write slot */
  OWT_SLOT_us = 60,
};

/** Convert microseconds to cycles.
 *
 * Assumes clock is running at nominal speed. */
#define US_TO_CYCLES(_us) (((_us) * BSP430_CLOCK_NOMINAL_MCLK_HZ) / 1000000)

int
iBSP430onewireReset (const xBSP430onewireBus * bus)
{
  int present;
  
  /* Hold bus low for OWT_RESET_us */
  bus->port->out &= ~bus->bit;
  bus->port->dir |= bus->bit;
  __delay_cycles(US_TO_CYCLES(OWT_RSTL_us));

  /* Release bus and switch to input until presence pulse should be
   * visible. */
  bus->port->dir &= ~bus->bit;
#if defined(__MSP430_HAS_PORT1_R__)
  /* If PORT1 has resistor capability, all ports do.  In case the
   * device isn't wired correctly, pull up the bus so the device
   * detection is correct.
   *
   * Since the protocol requires that the reset command be issued
   * before any other command, we're going to assume any resistor
   * enable bit remains set for the subsequent commands.  We do,
   * however, reconfigure the PxOUT register to pull-up if the last
   * setting would cause a pulldown configuration, as it would
   * here. */
  bus->port->ren |= bus->bit;
  bus->port->out |= bus->bit;
#endif /* __MSP430_HAS_PORT1_R__ */
  __delay_cycles(US_TO_CYCLES(OWT_PDHIGH_us));

  /* Record presence if bus is low (DS182x is holding it there) */
  present = !(bus->port->in & bus->bit);

  /* Wait for reset cycle to complete */
  __delay_cycles(US_TO_CYCLES(OWT_RSTH_us - OWT_PDHIGH_us));

  return present;
}

void
vBSP430onewireShutdown (const xBSP430onewireBus * bus)
{
  bus->port->out &= ~bus->bit;
  bus->port->dir &= ~bus->bit;
}

void
vBSP430onewireWriteByte (const xBSP430onewireBus * bus,
                         int byte)
{
  int bp;

  for (bp = 0; bp < 8; ++bp) {
    bus->port->out &= ~bus->bit;
    bus->port->dir |= bus->bit;
    if (byte & 0x01) {
      __delay_cycles(US_TO_CYCLES(OWT_LOW1_us));
      bus->port->dir &= ~bus->bit;
#if defined(__MSP430_HAS_PORT1_R__)
      /* Leave REN set from reset command, configure for pullup */
      bus->port->out |= bus->bit;
#endif /* __MSP430_HAS_PORT1_R__ */
      __delay_cycles(US_TO_CYCLES(OWT_SLOT_us - OWT_LOW1_us + OWT_REC_us));
    } else {
      __delay_cycles(US_TO_CYCLES(OWT_LOW0_us));
      bus->port->dir &= ~bus->bit;
#if defined(__MSP430_HAS_PORT1_R__)
      /* Leave REN set from reset command, configure for pullup */
      bus->port->out |= bus->bit;
#endif /* __MSP430_HAS_PORT1_R__ */
      __delay_cycles(US_TO_CYCLES(OWT_SLOT_us - OWT_LOW0_us + OWT_REC_us));
    }
    byte >>= 1;
  }
}

int
iBSP430onewireReadBit (const xBSP430onewireBus * bus)
{
  int rv;
  
  bus->port->out &= ~bus->bit;
  bus->port->dir |= bus->bit;
  __delay_cycles(US_TO_CYCLES(OWT_INT_us));
  bus->port->dir &= ~bus->bit;
  __delay_cycles(US_TO_CYCLES(OWT_RDV_us));
  rv = !!(bus->port->in & bus->bit);
  __delay_cycles(US_TO_CYCLES(OWT_SLOT_us - OWT_RDV_us - OWT_INT_us + OWT_REC_us));
  return rv;
}

int
iBSP430onewireReadByte (const xBSP430onewireBus * bus)
{
  int byte = 0;
  int bit = 1;

  do {
    if (iBSP430onewireReadBit(bus)) {
      byte |= bit;
    }
    bit <<= 1;
  } while (0x100 != bit);
  return byte;
}

int
iBSP430onewireComputeCRC (const unsigned char * romp,
        int len)
{
  static const unsigned char OW_CRC_POLY = 0x8c;
  unsigned char crc = 0;

  while (0 < len--) {
    int bi;

    crc ^= *romp++;
    for (bi = 0; bi < 8; ++bi) {
      if (crc & 1) {
	crc = (crc >> 1) ^ OW_CRC_POLY;
      } else {
	crc >>= 1;
      }
    }
  }
  return crc;
}