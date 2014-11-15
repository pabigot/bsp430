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

#ifndef BSP430_UTILITY_SHARPLCD_H
#define BSP430_UTILITY_SHARPLCD_H

/** @file
 *
 * @brief Interface for <a href="http://www.sharpmemorylcd.com">Sharp Memory LCD</a> displays.
 *
 * This display may be used natively, or in conjunction with
 * #configBSP430_UTILITY_U8GLIB.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/core.h>
#include <bsp430/serial.h>
#include <bsp430/periph/port.h>

/** Define to a true value to request that platform enable its <a
 * href="http://www.sharpmemorylcd.com">Sharp Microelectronics Memory
 * LCD display</a>.
 *
 * @cppflag
 * @affects #BSP430_PLATFORM_SHARPLCD
 * @defaulted
 */
#ifndef configBSP430_PLATFORM_SHARPLCD
#define configBSP430_PLATFORM_SHARPLCD 0
#endif /* configBSP430_PLATFORM_SHARPLCD */

/** Indicate that a Sharp Microelectronics LCD display is available on
 * the platform.  This is set by the platform-specific header when
 * #configBSP430_PLATFORM_BOOSTERPACK_SHARP96 is true and the platform
 * has resource mappings for the device, or on platforms that natively
 * contain a Sharp Memory LCD display.
 *
 * @li #BSP430_PLATFORM_SHARPLCD_SPI_PERIPH_HANDLE
 * @li #BSP430_PLATFORM_SHARPLCD_CS_PORT_PERIPH_HANDLE and #BSP430_PLATFORM_SHARPLCD_CS_PORT_BIT
 * @li #BSP430_PLATFORM_SHARPLCD_PWR_EN_PORT_PERIPH_HANDLE and #BSP430_PLATFORM_SHARPLCD_PWR_EN_PORT_BIT
 * @li #BSP430_PLATFORM_SHARPLCD_LCD_EN_PORT_PERIPH_HANDLE and #BSP430_PLATFORM_SHARPLCD_LCD_EN_PORT_BIT
 * @li Device size: #BSP430_PLATFORM_SHARPLCD_ROWS and #BSP430_PLATFORM_SHARPLCD_COLUMNS
 *
 * @cppflag
 * @dependency #configBSP430_PLATFORM_SHARPLCD, #configBSP430_PLATFORM_BOOSTERPACK_SHARP96
 * @defaulted */
#if defined(BSP430_DOXYGEN)
#define BSP430_PLATFORM_SHARPLCD include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** Define to a true value to request that platform enable an
 * interface to the <a
 * href="http://www.ti.com/tool/430boost-sharp96">430BOOST-SHARP96</a>
 * booster pack.
 *
 * @cppflag
 * @affects #BSP430_PLATFORM_SHARPLCD, #BSP430_PLATFORM_BOOSTERPACK_SHARP96
 * @defaulted
 */
#ifndef configBSP430_PLATFORM_BOOSTERPACK_SHARP96
#define configBSP430_PLATFORM_BOOSTERPACK_SHARP96 0
#endif /* configBSP430_PLATFORM_BOOSTERPACK_SHARP96 */

/** Indicate that a Sharp Microelectronics LCD display is available on
 * the platform through a <a
 * href="http://www.ti.com/tool/430boost-sharp96">430BOOST-SHARP96</a>
 * booster pack.  The macro is undefined or false if the boosterpack
 * is not requested or its required resources have not been defined
 * for the platform.
 *
 * @cppflag
 * @dependency #configBSP430_PLATFORM_BOOSTERPACK_SHARP96
 * @defaulted */
#if defined(BSP430_DOXYGEN)
#define BSP430_PLATFORM_BOOSTERPACK_SHARP96 include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

#if defined(BSP430_DOXYGEN)
/** The height of the display in pixels.
 * @platformdep */
#define BSP430_PLATFORM_SHARPLCD_ROWS include <bsp430/platform.h>
#endif /* BSP430_PLATFORM_SHARPLCD_ROWS */
#if defined(BSP430_DOXYGEN)
/** The width of the display in pixels
 * @platformdep */
#define BSP430_PLATFORM_SHARPLCD_COLUMNS include <bsp430/platform.h>
#endif /* BSP430_PLATFORM_SHARPLCD_COLUMNS */
/** The width of the display in bytes */
#define BSP430_PLATFORM_SHARPLCD_BYTES_PER_LINE ((7 + BSP430_PLATFORM_SHARPLCD_COLUMNS) / 8)

#if defined(BSP430_DOXYGEN) || !defined(BSP430_SHARPLCD_REFRESH_INTERVAL_MS)
/** The interval at which the VCOM signal must be toggled, using
 * iBSP430sharplcdRefreshDisplay_rh(), in order to avoid DC bias
 * build-up.  1 Hz is good; maximum rate is 30 Hz, some devices allow
 * 0.5 Hz.
 *
 * @defaulted */
#define BSP430_SHARPLCD_REFRESH_INTERVAL_MS 1000
#endif /* BSP430_SHARPLCD_REFRESH_INTERVAL_MS */

/** The desired SPI bus speed for the device.
 *
 * For LS013B4DN04 the maximum value is 1 MHz.  For other models it
 * may be up to 2 MHz. */
#if defined(BSP430_DOXYGEN) || !defined(BSP430_PLATFORM_SHARPLCD_SPI_BUS_HZ)
#define BSP430_PLATFORM_SHARPLCD_SPI_BUS_HZ 1000000UL
#endif /* BSP430_SHARPLCD_SPI_BUS_HZ */

/** Parameter to pass to hBSP430serialOpenSPI() as the @c ctl0_byte. */
#define BSP430_SHARPLCD_CTL0_INITIALIZER BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCCKPH | UCMSB | UCMST)

#if defined(BSP430_DOXYGEN)
/** BSP430 peripheral handle for the port that controls the display chip select line.
 * @note This signal is active-high unlike most SPI-controlled devices.
 * @platformdep */
#define BSP430_PLATFORM_SHARPLCD_CS_PORT_PERIPH_HANDLE include <bsp430/platform.h>
/** Port bit on #BSP430_PLATFORM_SHARPLCD_CS_PORT_PERIPH_HANDLE that
 * controls the display chip select.
 *
 * @note This signal is active-high.
 * @platformdep  */
#define BSP430_PLATFORM_SHARPLCD_CS_PORT_BIT include <bsp430/platform.h>
/** BSP430 peripheral handle for the port that controls the display
 * DISP on/off line.
 * @platformdep  */
#define BSP430_PLATFORM_SHARPLCD_LCD_EN_PORT_PERIPH_HANDLE include <bsp430/platform.h>
/** Port bit on #BSP430_PLATFORM_SHARPLCD_LCD_EN_PORT_PERIPH_HANDLE
 * that turns the display on or off.  The signal is active-high.
 * @platformdep  */
#define BSP430_PLATFORM_SHARPLCD_LCD_EN_PORT_BIT include <bsp430/platform.h>
/** BSP430 peripheral handle for the port that provides display power.
 * @note As-delivered the display is powered through
 * #BSP430_PLATFORM_SHARPLCD_LCD_EN_PORT_BIT; this GPIO is only
 * necessary when using alternative power configurations.
 * @platformdep  */
#define BSP430_PLATFORM_SHARPLCD_PWR_EN_PORT_PERIPH_HANDLE include <bsp430/platform.h>
/** Port bit on #BSP430_PLATFORM_SHARPLCD_PWR_EN_PORT_PERIPH_HANDLE
 * that provides display power.  The signal is active-high.
 * @platformdep  */
#define BSP430_PLATFORM_SHARPLCD_PWR_EN_PORT_BIT include <bsp430/platform.h>
/** BSP430 peripheral handle for the SPI interface to the device.
 * @platformdep
 */
#define BSP430_PLATFORM_SHARPLCD_SPI_PERIPH_HANDLE include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** A structure holding resource references and state required to
 * manipulate a Sharp Memory LCD. */
typedef struct sBSP430sharplcd {
  volatile sBSP430hplPORT * cs; /**< Port that controls chip-select */
  volatile sBSP430hplPORT * lcd_en; /**< Port that controls LCD enable */
  volatile sBSP430hplPORT * pwr_en; /**< Port that controls power enable */
  hBSP430halSERIAL spi;             /**< SPI device handle */
  unsigned int lines;               /**< Display height in pixels */
  unsigned int columns;             /**< Display width in pixels */
  unsigned int line_size;           /**< Number of bytes in each line */
  uint8_t cs_bit;                   /**< Bit in #cs to control chip select */
  uint8_t lcd_en_bit;               /**< Bit in #lcd_en to control LCD enable  */
  uint8_t pwr_en_bit;               /**< Bit in #pwr_en to control power enable */
  uint8_t vcom_state_;              /**< *INTERNAL* Last configured VCOM state */
} sBSP430sharplcd;

/** Handle for a Sharp Memory LCD device */
typedef sBSP430sharplcd * hBSP430sharplcd;

/** The command bit pattern set for dynamic mode.
 *
 * This is what's used to update pixels.  Absence of this bit
 * indicates a static mode command. */
#define BSP430_SHARPLCD_MODE_DYNAMIC 0x80

/** The command bit pattern that controls VCOM.
 *
 * This bit may be combined with
 * #BSP430_SHARPLCD_MODE_DYNAMIC or used in static mode.
 * To eliminate DC bias build-up a command should be sent to the
 * display at a minimum frequency of 1Hz (maximum 30 Hz) while the
 * display is active.  This bit should alternate in those commands
 * using a 50% duty cycle. */
#define BSP430_SHARPLCD_VCOM 0x40

/** The command bit pattern to clear all flags in the display.
 *
 * This may be set in a static mode command to reset the display. */
#define BSP430_SHARPLCD_CLEAR_ALL 0x20

/** Assert the CS signal in preparation for interacting with the
 * device. */
#define BSP430_SHARPLCD_CS_ASSERT(_dev) do {                            \
    (_dev)->cs->out |= (_dev)->cs_bit;                                  \
  } while (0)

/** De-assert the CS signal after interacting with the device */
#define BSP430_SHARPLCD_CS_DEASSERT(_dev) do {                          \
    (_dev)->cs->out &= ~(_dev)->cs_bit;                                 \
  } while (0)

/** Initialize @p dev using the platform device constants (viz.,
 * #BSP430_PLATFORM_SHARPLCD_SPI_PERIPH_HANDLE,
 * #BSP430_PLATFORM_SHARPLCD_CS_PORT_PERIPH_HANDLE,
 * #BSP430_PLATFORM_SHARPLCD_ROWS, etc.)
 *
 * This only initializes the device structure and configures the SPI
 * interface.  It does not configure any required GPIOs nor does it
 * power-up the device.  See iBSP430sharplcdSetEnabled_ni().
 *
 * @param dev pointer to an #sBSP430sharplcd structure that will be
 * used to hold the device state.
 *
 * @return @p dev if successful, a null pointer if a critical resource
 * could not be obtained. */
hBSP430sharplcd hBSP430sharplcdInitializePlatformDevice (hBSP430sharplcd dev);

/** Enable or disable the display.
 *
 * If @p enablep is true, this enables power to the device and
 * executes its startup sequence.  If @p enablep is false this
 * executes the device shutdown sequence and removes power from it.
 *
 * @param dev device to be powered
 *
 * @param enablep nonzero to enable device, zero to disable it
 *
 * @return zero on success, a negative error code on failure. */
int iBSP430sharplcdSetEnabled_ni (hBSP430sharplcd dev, int enablep);

/** Issue the command to quickly clear the display.
 *
 * @param dev device with display to be cleared
 *
 * @return zero on success, a negative error code on failure. */
int iBSP430sharplcdClearDisplay_rh (hBSP430sharplcd dev);

/** Issue the command to refresh the display.
 *
 * This sends a static-mode command that toggles the VCOM signal to
 * the screen, preventing DC bias build-up.  It should be invoked once
 * per second while the display is powered.
 *
 * @param dev device with display to be refreshed
 *
 * @return zero on success, a negative error code on failure */
int iBSP430sharplcdRefreshDisplay_rh (hBSP430sharplcd dev);

/** Replace lines in the display with new content.
 *
 * @param dev device to be updated
 *
 * @param start_line the first line to be updated.  Lines are numbered
 * starting with 1; values less than this result in an error return.
 *
 * @param num_lines the number of lines to be updated.  A negative
 * value is interpreted to mean all lines starting with @p start_line.
 *
 * @param line_data the monochrome pixel values to be used.  Each line
 * requires @c dev->line_size bytes.
 *
 * @return zero on success, a negative error code on failure. */
int iBSP430sharplcdUpdateDisplayLines_rh (hBSP430sharplcd dev,
                                          int start_line,
                                          int num_lines,
                                          const uint8_t * line_data);

#endif /* BSP430_UTILITY_SHARPLCD_H */
