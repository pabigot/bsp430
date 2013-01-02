/* Copyright 2012-2013, Peter A. Bigot
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

/** Define protocol state times in microseconds.
 *
 * @note Since all these times are far less than any sane watchdog
 * interval, and the timing can be important, BSP430_CORE_DELAY_CYCLES
 * is not used in this module. */
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

int
iBSP430onewireReset_ni (const sBSP430onewireBus * bus)
{
  int present;

  /* Hold bus low for OWT_RESET_us */
  BSP430_PORT_HAL_HPL_OUT(bus->port) &= ~bus->bit;
  BSP430_PORT_HAL_HPL_DIR(bus->port) |= bus->bit;
  __delay_cycles(BSP430_CLOCK_US_TO_NOMINAL_MCLK(OWT_RSTL_us));

  /* Release bus and switch to input until presence pulse should be
   * visible. */
  BSP430_PORT_HAL_HPL_DIR(bus->port) &= ~bus->bit;
#if BSP430_PORT_SUPPORTS_REN - 0
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
  if (bus->use_ren) {
    BSP430_PORT_HAL_HPL_REN(bus->port) |= bus->bit;
    BSP430_PORT_HAL_HPL_OUT(bus->port) |= bus->bit;
  }
#endif /* BSP430_PORT_SUPPORTS_REN */
  __delay_cycles(BSP430_CLOCK_US_TO_NOMINAL_MCLK(OWT_PDHIGH_us));

  /* Record presence if bus is low (DS182x is holding it there) */
  present = !(BSP430_PORT_HAL_HPL_IN(bus->port) & bus->bit);

  /* Wait for reset cycle to complete */
  __delay_cycles(BSP430_CLOCK_US_TO_NOMINAL_MCLK(OWT_RSTH_us - OWT_PDHIGH_us));

  return present;
}

void
vBSP430onewireShutdown_ni (const sBSP430onewireBus * bus)
{
  BSP430_PORT_HAL_HPL_OUT(bus->port) &= ~bus->bit;
  BSP430_PORT_HAL_HPL_DIR(bus->port) &= ~bus->bit;
}

void
vBSP430onewireWriteByte_ni (const sBSP430onewireBus * bus,
                            int byte)
{
  int bp;

  for (bp = 0; bp < 8; ++bp) {
    BSP430_PORT_HAL_HPL_OUT(bus->port) &= ~bus->bit;
    BSP430_PORT_HAL_HPL_DIR(bus->port) |= bus->bit;
    if (byte & 0x01) {
      __delay_cycles(BSP430_CLOCK_US_TO_NOMINAL_MCLK(OWT_LOW1_us));
      BSP430_PORT_HAL_HPL_DIR(bus->port) &= ~bus->bit;
#if BSP430_PORT_SUPPORTS_REN - 0
      if (bus->use_ren) {
        /* Leave REN set from reset command, configure for pullup */
        BSP430_PORT_HAL_HPL_OUT(bus->port) |= bus->bit;
      }
#endif /* BSP430_PORT_SUPPORTS_REN */
      __delay_cycles(BSP430_CLOCK_US_TO_NOMINAL_MCLK(OWT_SLOT_us - OWT_LOW1_us + OWT_REC_us));
    } else {
      __delay_cycles(BSP430_CLOCK_US_TO_NOMINAL_MCLK(OWT_LOW0_us));
      BSP430_PORT_HAL_HPL_DIR(bus->port) &= ~bus->bit;
#if BSP430_PORT_SUPPORTS_REN - 0
      if (bus->use_ren) {
        /* Leave REN set from reset command, configure for pullup */
        BSP430_PORT_HAL_HPL_OUT(bus->port) |= bus->bit;
      }
#endif /* BSP430_PORT_SUPPORTS_REN */
      __delay_cycles(BSP430_CLOCK_US_TO_NOMINAL_MCLK(OWT_SLOT_us - OWT_LOW0_us + OWT_REC_us));
    }
    byte >>= 1;
  }
}

int
iBSP430onewireReadBit_ni (const sBSP430onewireBus * bus)
{
  int rv;

  BSP430_PORT_HAL_HPL_OUT(bus->port) &= ~bus->bit;
  BSP430_PORT_HAL_HPL_DIR(bus->port) |= bus->bit;
  __delay_cycles(BSP430_CLOCK_US_TO_NOMINAL_MCLK(OWT_INT_us));
  BSP430_PORT_HAL_HPL_DIR(bus->port) &= ~bus->bit;
  __delay_cycles(BSP430_CLOCK_US_TO_NOMINAL_MCLK(OWT_RDV_us));
  rv = !!(BSP430_PORT_HAL_HPL_IN(bus->port) & bus->bit);
  __delay_cycles(BSP430_CLOCK_US_TO_NOMINAL_MCLK(OWT_SLOT_us - OWT_RDV_us - OWT_INT_us + OWT_REC_us));
  return rv;
}

int
iBSP430onewireReadByte_ni (const sBSP430onewireBus * bus)
{
  int byte = 0;
  int bit = 1;

  do {
    if (iBSP430onewireReadBit_ni(bus)) {
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

int
iBSP430onewireReadSerialNumber (const sBSP430onewireBus * bus,
                                sBSP430onewireSerialNumber * snp)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rv = -1;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    int i;
    uint8_t rom[8];

    if (! iBSP430onewireReset_ni(bus)) {
      break;
    }
    vBSP430onewireWriteByte_ni(bus, BSP430_ONEWIRE_CMD_READ_ROM);
    for (i = 0; i < sizeof(rom); ++i) {
      rom[i] = iBSP430onewireReadByte_ni(bus);
    }
    if (0 == iBSP430onewireComputeCRC(rom, sizeof(rom))) {
      for (i = 0; i < sizeof(snp->id); ++i) {
        snp->id[i] = rom[sizeof(rom) - 2 - i];
      }
      rv = 0;
    }
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

int
iBSP430onewireRequestTemperature_ni (const sBSP430onewireBus * bus)
{
  if (! iBSP430onewireReset_ni(bus)) {
    return -1;
  }
  vBSP430onewireWriteByte_ni(bus, BSP430_ONEWIRE_CMD_SKIP_ROM);
  vBSP430onewireWriteByte_ni(bus, BSP430_ONEWIRE_CMD_CONVERT_T);
  return 0;
}

int
iBSP430onewireReadTemperature_ni (const sBSP430onewireBus * bus,
                                  int * temp_xCel)
{
  int t;

  if (! iBSP430onewireReset_ni(bus)) {
    return -1;
  }
  vBSP430onewireWriteByte_ni(bus, BSP430_ONEWIRE_CMD_SKIP_ROM);
  vBSP430onewireWriteByte_ni(bus, BSP430_ONEWIRE_CMD_READ_SCRATCHPAD);
  t = iBSP430onewireReadByte_ni(bus);
  t |= (iBSP430onewireReadByte_ni(bus) << 8);
  *temp_xCel = t;
  return 0;
}
