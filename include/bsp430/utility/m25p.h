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
 * @brief Support for M25P-compatible serial SPI flash memory devices.
 *
 * Micron/Numonyx serial SPI flash devices such as the M25P16 and
 * M25PE20 are commonly used on MSP430-based wireless sensor
 * platforms.  This module defines the command and status constants
 * for the M25P interface; a structure holding the required SPI, CS#,
 * and RESET# information to interact with the device, and
 * declarations for module functions that encode and transmit
 * commands.
 *
 * The module also supports #configBSP430_PLATFORM_M25P and
 * #BSP430_PLATFORM_M25P to allow applications to request access to a
 * platform-supplied flash memory chip without encoding platform
 * dependencies in the source code.
 *
 * As with other parts of BSP430, this is a low-level interface that
 * does not provide convenience wrappers for functions like erasing a
 * sector or chip, or reading and writing the device contents.  It
 * does provide enough to allow the application to do this, including
 * the ability to initiate a read or write operation and complete it
 * using programmed I/O (iBSP430m25pCompleteTxRx_ni()),
 * interrupt-driven SPI transactions, or DMA, based on the
 * application's needs.
 *
 * @note The commands constants defined in this module (e.g.,
 * #BSP430_M25P_CMD_PW) cover all known M25P implementations.  Not all
 * commands are supported on all devices.
 *
 * @ref ex_utility_m25p shows how to use this interface.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_M25P_H
#define BSP430_UTILITY_M25P_H

#include <bsp430/core.h>
#include <bsp430/serial.h>
#include <bsp430/periph/port.h>

/** Define to request that platform enable its M25P flash.
 *
 * Some platforms have an external serial flash memory suitable for
 * control by the <bsp430/utility/m25p.h> interface.  Defining this
 * macro in your <bsp430_config.h> will register a request for that
 * device. */
/** Define to a true value to enable any platform-provided M25P serial
 * flash memory.
 *
 * @cppflag
 * @affects #BSP430_PLATFORM_M25P
 * @defaulted
 */
#ifndef configBSP430_PLATFORM_M25P
#define configBSP430_PLATFORM_M25P 0
#endif /* configBSP430_PLATFORM_M25P */

/** Indicate that an M25P serial flash device is available on the
 * platform.  This is set by the platform-specific header when
 * #configBSP430_PLATFORM_M25P is true and the platform supports an
 * M25P device.
 *
 * See @ref ex_utility_m25p for how to use the following identifiers
 * to configure and use the platform device:
 *
 * @li #BSP430_PLATFORM_M25P_SPI_PERIPH_HANDLE
 * @li #BSP430_PLATFORM_M25P_CSn_PORT_PERIPH_HANDLE and #BSP430_PLATFORM_M25P_CSn_PORT_BIT
 * @li #BSP430_PLATFORM_M25P_RSTn_PORT_PERIPH_HANDLE and #BSP430_PLATFORM_M25P_RSTn_PORT_BIT
 * @li #BSP430_PLATFORM_M25P_PWR_PORT_PERIPH_HANDLE and #BSP430_PLATFORM_M25P_PWR_PORT_BIT
 * @li Device size: #BSP430_PLATFORM_M25P_SUBSECTOR_SIZE, #BSP430_PLATFORM_M25P_SECTOR_SIZE, #BSP430_PLATFORM_M25P_SECTOR_COUNT
 * @li Device capability: #BSP430_PLATFORM_M25P_SUPPORTS_PE, #BSP430_PLATFORM_M25P_SUPPORTS_PW,
 *
 * @cppflag
 * @dependency #configBSP430_PLATFORM_M25P
 * @defaulted */
#if defined(BSP430_DOXYGEN)
#define BSP430_PLATFORM_M25P include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

#if defined(BSP430_DOXYGEN)

/** Peripheral handle for SPI access to platform-provided M25P serial flash
 * @dependency #BSP430_PLATFORM_M25P */
#define BSP430_PLATFORM_M25P_SPI_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which SPI flash power is placed.
 * @dependency #BSP430_PLATFORM_M25P
 */
#define BSP430_PLATFORM_M25P_PWR_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** Port bit on #BSP430_PLATFORM_M25P_PWR_PORT_PERIPH_HANDLE for SPI flash power
 * @dependency #BSP430_PLATFORM_M25P
 */
#define BSP430_PLATFORM_M25P_PWR_PORT_BIT include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which SPI flash power is placed.
 * @dependency #BSP430_PLATFORM_M25P
 */
#define BSP430_PLATFORM_M25P_RSTn_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** Port bit on #BSP430_PLATFORM_M25P_RSTn_PORT_PERIPH_HANDLE for SPI flash power
 * @dependency #BSP430_PLATFORM_M25P
 */
#define BSP430_PLATFORM_M25P_RSTn_PORT_BIT include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which SPI flash chip-select (inverted) is placed.
 * @dependency #BSP430_PLATFORM_M25P
 */
#define BSP430_PLATFORM_M25P_CSn_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** Port bit on #BSP430_PLATFORM_M25P_CSn_PORT_PERIPH_HANDLE for SPI flash CSn
 * @dependency #BSP430_PLATFORM_M25P
 */
#define BSP430_PLATFORM_M25P_CSn_PORT_BIT include <bsp430/platform.h>

/** Defined to a true value iff the M25P flash on the platform
 * supports the #BSP430_M25P_CMD_PE PAGE ERASE command.  If false or
 * absent, assume the device may be erased only by using
 * #BSP430_M25P_CMD_SE or #BSP430_M25P_CMD_BE. */
#define BSP430_PLATFORM_M25P_SUPPORTS_PE include <bsp430/platform.h>

/** Defined to a true value iff the M25P flash on the platform
 * supports the #BSP430_M25P_CMD_PW PAGE WRITE command.  If false or
 * absent, assume the device may be written only by following a @link
 * BSP430_M25P_CMD_PE page erase@endlink, @link BSP430_M25P_CMD_SSE
 * subsector erase@endlink, @link BSP430_M25P_CMD_SE sector
 * erase@endlink, or @link BSP430_M25P_CMD_BE bulk erase@endlink with
 * #BSP430_M25P_CMD_PP. */
#define BSP430_PLATFORM_M25P_SUPPORTS_PW include <bsp430/platform.h>

/** The number of bytes in each sector of the platform M25P device.
 * @dependency #BSP430_PLATFORM_M25P
 */
#define BSP430_PLATFORM_M25P_SECTOR_SIZE include <bsp430/platform.h>

/** The number of bytes in each subsector of the platform M25P device.
 *
 * Undefined if the device does not support subsector operations.
 *
 * @dependency #BSP430_PLATFORM_M25P
 */
#define BSP430_PLATFORM_M25P_SUBSECTOR_SIZE include <bsp430/platform.h>

/** The number of sectors in the platform M25P device.
 * @dependency #BSP430_PLATFORM_M25P
 */
#define BSP430_PLATFORM_M25P_SECTOR_COUNT include <bsp430/platform.h>

#endif /* BSP430_DOXYGEN */

/** Information required to access an M25P-based serial SPI flash
 * device.  Boards that allow control of power to the device must do
 * so externally from this module. */
typedef struct sBSP430m25p {
  /** The SPI peripheral used to access the device.  A device must be
   * assigned. */
  hBSP430halSERIAL spi;
  /** The port peripheral used to control the device CS# (chip select
   * inverted) signal.  A port must be provided. */
  volatile sBSP430hplPORT * csn_port;
  /** The port peripheral used to control the device RESET# (reset
   * inverted) signal.  If no port is provided, the M25P functions
   * will not manipulate the RESET# signal. */
  volatile sBSP430hplPORT * rstn_port;
  /** The bit identifying the csn_port peripheral port pin that
   * controls the device CS# signal. */
  uint8_t csn_bit;
  /** The bit identifying the rstn_port peripheral port pin that
   * controls the device RESET# signal. */
  uint8_t rstn_bit;
} sBSP430m25p;

/** Handle used to access an M25P-based serial SPI flash device. */
typedef sBSP430m25p * hBSP430m25p;

/** WRITE ENABLE command */
#define BSP430_M25P_CMD_WREN 0x06
/** WRITE DISABLE command */
#define BSP430_M25P_CMD_WRDI 0x04
/** READ IDENTIFICATION command. */
#define BSP430_M25P_CMD_RDID 0x9f
/** READ STATUS REGISTER command */
#define BSP430_M25P_CMD_RDSR 0x05
/** WRITE STATUS REGISTER command. */
#define BSP430_M25P_CMD_WRSR 0x01
/** WRITE to LOCK REGISTER command */
#define BSP430_M25P_CMD_WRLR 0xe5
/** READ LOCK REGISTER command */
#define BSP430_M25P_CMD_RDLR 0xe8
/** READ DATA BYTES command */
#define BSP430_M25P_CMD_READ 0x03
/** READ DATA BYTES at HIGHER SPEED command */
#define BSP430_M25P_CMD_FAST_READ 0x0b
/** PAGE WRITE command */
#define BSP430_M25P_CMD_PW 0x0a
/** PAGE PROGRAM command.  Not available on all devices.  See #BSP430_PLATFORM_M25P_SUPPORTS_PE.  */
#define BSP430_M25P_CMD_PP 0x02
/** PAGE ERASE command.  Not available on all devices.  See #BSP430_PLATFORM_M25P_SUPPORTS_PE. */
#define BSP430_M25P_CMD_PE 0xdb
/** SUBSECTOR ERASE command.  Not available on all devices.
 * #BSP430_PLATFORM_M25P_SUBSECTOR_SIZE may indicate that support is
 * available. */
#define BSP430_M25P_CMD_SSE 0x20
/** SECTOR ERASE command */
#define BSP430_M25P_CMD_SE 0xd8
/** BULK ERASE command */
#define BSP430_M25P_CMD_BE 0xc7
/** DEEP POWER-DOWN command */
#define BSP430_M25P_CMD_DP 0xb9
/** RELEASE from DEEP POWER-DOWN command */
#define BSP430_M25P_CMD_RELDP 0xab
/** READ ELECTRONIC SIGNATURE command.  Overloads #BSP430_M25P_CMD_RELDP on some devices. */
#define BSP430_M25P_CMD_RES 0xab

/** Write-in-progress bit within M25P status register.  Bit is read-only. */
#define BSP430_M25P_SR_WIP 0x01
/** Write-enable-latch bit within M25P status register. */
#define BSP430_M25P_SR_WEL 0x02
/** Block protect bit BP0 within M25P status register.  Not available
 * on all devices. */
#define BSP430_M25P_SR_BP0 0x04
/** Block protect bit BP1 within M25P status register.  Not available
 * on all devices. */
#define BSP430_M25P_SR_BP1 0x08
/** Status-register-write-disable bit within M25P status register.
 * Not available on all devices. */
#define BSP430_M25P_SR_SRWD 0x80

/** Set the RESET# signal to place the device into reset */
#define BSP430_M25P_RESET_SET(_dev) do {                \
    if (NULL != (_dev)->rstn_port) {                    \
      (_dev)->rstn_port->out &= ~(_dev)->rstn_bit;      \
    }                                                   \
  } while (0)

/** Clear the RESET# signal to take the device out of reset */
#define BSP430_M25P_RESET_CLEAR(_dev) do {      \
    if (NULL != (_dev)->rstn_port) {                    \
      (_dev)->rstn_port->out |= (_dev)->rstn_bit;       \
    }                                                   \
  } while (0)

/** Assert the CS# signal in preparation for interacting with the
 * device. */
#define BSP430_M25P_CS_ASSERT(_dev) do {        \
    (_dev)->csn_port->out &= ~(_dev)->csn_bit;  \
  } while (0)

/** De-assert the CS# signal after interacting with the device. */
#define BSP430_M25P_CS_DEASSERT(_dev) do {      \
    (_dev)->csn_port->out |= (_dev)->csn_bit;   \
  } while (0)


/** Initialize an M25P device.
 *
 * This function initializes the SPI bus and peripheral ports to
 * interact with the device.  The SPI bus is configured as specified
 * by the parameters.  The CS# and RESET# ports are configured to
 * output mode, with CS# de-asserted (not selected) and RESET#
 * asserted (held in reset mode).
 *
 * To comply with device power-up requirements, power should not be
 * supplied to the device until this function has completed.  Further
 * interaction with the device should be delayed for a device-specific
 * period following application of power.
 *
 * @param dev the M25P device handle
 *
 * @param ctl0_byte as with hBSP430serialOpenSPI()
 *
 * @param ctl1_byte as with hBSP430serialOpenSPI()
 *
 * @param prescaler as with hBSP430serialOpenSPI()
 *
 * @return @p dev if the initialization was successful, or a null
 * handle if it failed. */
hBSP430m25p hBSP430m25pInitialize (hBSP430m25p dev,
                                   unsigned char ctl0_byte,
                                   unsigned char ctl1_byte,
                                   unsigned int prescaler);

/** Read the M25P device status register assuming interrupts are
 * disabled.
 *
 * The status register can be polled to detect completion of
 * long-running operations such as #BSP430_M25P_CMD_BE.  Generally you
 * will need to wait until #BSP430_M25P_SR_WIP is clear before
 * executing a new command.
 *
 * @param dev the M25P device handle
 *
 * @return the non-negative value of the status register, or -1 if an
 * error occurred. */
int iBSP430m25pStatus_ni (hBSP430m25p dev);

/** Read the M25P device status register regardless of interrupt state.
 *
 * This wraps iBSP430m25pStatus_ni() with code that saves the
 * interrupt state, reads the status register, then restores the
 * interrupt state prior to returning the result. */
static BSP430_CORE_INLINE
int iBSP430m25pStatus (hBSP430m25p dev)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  int rv;

  BSP430_CORE_DISABLE_INTERRUPT();
  rv = iBSP430m25pStatus_ni(dev);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/** Strobe an M25P device command.
 *
 * The CS# signal is asserted, the @p cmd transmitted, and CS# is
 * de-asserted.  This interface is used for commands like
 * #BSP430_M25P_CMD_WREN and #BSP430_M25P_CMD_BE that neither provide
 * nor receive data and do not use an address.
 *
 * Note that for many of these commands #BSP430_M25P_SR_WIP will
 * remain set for a device/command-specific period until the requested
 * operation has completed.
 *
 * @param dev the M25P device handle
 *
 * @param cmd the command to be executed, e.g. #BSP430_M25P_CMD_RDSR
 *
 * @return 0 on success, or a negative value to indicate an error. */
int iBSP430m25pStrobeCommand_ni (hBSP430m25p dev,
                                 uint8_t cmd);

/** Strobe an M25P address-based command.
 *
 * This interface is appropriate for commands like #BSP430_M25P_CMD_PE
 * which affect an address but require no data be written or read.  It
 * is functionally equivalent to
 * iBSP430m25pInitiateAddressCommand_ni() followed immediately by
 * #BSP430_M25P_CS_DEASSERT().
 *
 * Note that for many of these commands #BSP430_M25P_SR_WIP will
 * remain set for a device/command-specific period until the requested
 * operation has completed.
 *
 * @param dev the M25P device handle
 *
 * @param cmd the command to be executed, e.g. #BSP430_M25P_CMD_PE
 *
 * @param addr the address at which the command applies.
 *
 * @return 0 if successful, -1 on an error. */
int iBSP430m25pStrobeAddressCommand_ni (hBSP430m25p dev,
                                        uint8_t cmd,
                                        unsigned long addr);

/** Initiate an M25P device command.
 *
 * The CS# signal is asserted and the @p cmd transmitted.  CS# is
 * de-asserted if an error occurs; otherwise CS# is left asserted and
 * the call returns allowing the caller to write additional data to
 * the device or to receive data from the device through programmed
 * I/O, interrupt-driven I/O, or DMA.
 *
 * To use programmed I/O to complete a read or write operation, see
 * iBSP430m25pCompleteTxRx_ni().
 *
 * @note CS# is left asserted after a successful call to this
 * function.
 *
 * @param dev the M25P device handle
 *
 * @param cmd the command to be executed, e.g. #BSP430_M25P_CMD_RDSR
 *
 * @return 0 if successful, -1 on an error.  If successful CS# is left
 * asserted and the caller should invoke #BSP430_M25P_CS_DEASSERT()
 * after writing and reading command-specific data. */
int iBSP430m25pInitiateCommand_ni (hBSP430m25p dev,
                                   uint8_t cmd);

/** Initiate an M25P address-based command.
 *
 * The CS# signal is asserted, the @p cmd transmitted, and the address
 * transmitted.  CS# is de-asserted if an error occurs; otherwise CS#
 * is left asserted and the call returns allowing the caller to write
 * additional data to the device or to receive data from the device
 * through programmed I/O, interrupt-driven I/O, or DMA.
 *
 * To use programmed I/O to complete a read or write operation, see
 * iBSP430m25pCompleteTxRx_ni().
 *
 * @note CS# is left asserted after a successful call to this
 * function.
 *
 * @param dev the M25P device handle
 *
 * @param cmd the command to be executed, e.g. #BSP430_M25P_CMD_RDSR
 *
 * @param addr the address at which the command applies.
 *
 * @return 0 if successful, -1 on an error.  If successful CS# is left
 * asserted and the caller should invoke #BSP430_M25P_CS_DEASSERT()
 * after writing and reading command-specific data. */
int iBSP430m25pInitiateAddressCommand_ni (hBSP430m25p dev,
                                          uint8_t cmd,
                                          unsigned long addr);

/** Complete an initiated command using programmed I/O.
 *
 * This simply wraps iBSP430spiTxRx_ni() to transmit data to the
 * device and/or receive data from the device, ensuring that
 * #BSP430_M25P_CS_DEASSERT() is invoked to complete the transaction
 * initiated by iBSP430m25pInitiateCommand_ni() or
 * iBSP430m25pInitiateAddressCommand_ni().
 *
 * @param dev the M25P device handle
 *
 * @param tx_data as with iBSP430spiTxRx_ni()
 *
 * @param tx_len as with iBSP430spiTxRx_ni()
 *
 * @param rx_len as with iBSP430spiTxRx_ni()
 *
 * @param rx_data as with iBSP430spiTxRx_ni()
 *
 * @return as with iBSP430spiTxRx_ni() */
int iBSP430m25pCompleteTxRx_ni (hBSP430m25p dev,
                                const uint8_t * tx_data,
                                size_t tx_len,
                                size_t rx_len,
                                uint8_t * rx_data);

#endif /* BSP430_UTILITY_M25P_H */
