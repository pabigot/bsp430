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

#ifndef BSP430_PLATFORM_TRXEB_PLATFORM_H
#define BSP430_PLATFORM_TRXEB_PLATFORM_H

/** @file
 *
 * @brief Platform-specific include for <a href="http://www.ti.com/tool/cc1125dk">MSP-TRXEB</a>
 *
 * The TrxEB is a transceiver evaluation board for TI RF modules.  It
 * is not easy to locate, but can be purchased with certain RF
 * evaluation models, and is documented in <a
 * href="http://www.ti.com/litv/pdf/swru294a">SWRU294</a>.
 *
 * The following platform-specific features are supported:
 * <ul>
 *
 * <li> #vBSP430platformSpinForJumper_ni is not implemented on this
 * platform.
 *
 * <li> #BSP430_PERIPH_EXPOSED_CLOCKS Clocks are made available at
 * dedicated labelled test points above the LCD.
 *
 * </ul>
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

/** Unconditionally define this, so as to produce errors if there is a
 * conflict in definition. */
#define BSP430_PLATFORM_TRXEB 1

/** @cond DOXYGEN_EXCLUDE */

/* Available button definitions */
/* Button0 is LEFT */
#define BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_PLATFORM_BUTTON0_PORT_BIT BIT1
#define BSP430_PLATFORM_BUTTON_LEFT_PORT_PERIPH_HANDLE BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE
#define BSP430_PLATFORM_BUTTON_LEFT_PORT_BIT BSP430_PLATFORM_BUTTON0_PORT_BIT
/* Button1 is RIGHT */
#define BSP430_PLATFORM_BUTTON1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_PLATFORM_BUTTON1_PORT_BIT BIT2
#define BSP430_PLATFORM_BUTTON_RIGHT_PORT_PERIPH_HANDLE BSP430_PLATFORM_BUTTON1_PORT_PERIPH_HANDLE
#define BSP430_PLATFORM_BUTTON_RIGHT_PORT_BIT BSP430_PLATFORM_BUTTON1_PORT_BIT
/* Button2 is SELECT */
#define BSP430_PLATFORM_BUTTON2_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_PLATFORM_BUTTON2_PORT_BIT BIT3
#define BSP430_PLATFORM_BUTTON_SELECT_PORT_PERIPH_HANDLE BSP430_PLATFORM_BUTTON2_PORT_PERIPH_HANDLE
#define BSP430_PLATFORM_BUTTON_SELECT_PORT_BIT BSP430_PLATFORM_BUTTON2_PORT_BIT
/* Button3 is UP */
#define BSP430_PLATFORM_BUTTON3_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_PLATFORM_BUTTON3_PORT_BIT BIT4
#define BSP430_PLATFORM_BUTTON_UP_PORT_PERIPH_HANDLE BSP430_PLATFORM_BUTTON3_PORT_PERIPH_HANDLE
#define BSP430_PLATFORM_BUTTON_UP_PORT_BIT BSP430_PLATFORM_BUTTON3_PORT_BIT
/* Button4 is DOWN */
#define BSP430_PLATFORM_BUTTON4_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_PLATFORM_BUTTON4_PORT_BIT BIT5
#define BSP430_PLATFORM_BUTTON_DOWN_PORT_PERIPH_HANDLE BSP430_PLATFORM_BUTTON4_PORT_PERIPH_HANDLE
#define BSP430_PLATFORM_BUTTON_DOWN_PORT_BIT BSP430_PLATFORM_BUTTON4_PORT_BIT

/* I think LED1 is yellow.  It may be green. */
#define BSP430_LED_RED 0
#define BSP430_LED_YELLOW 1
#define BSP430_LED_GREEN 2
#define BSP430_LED_RED1 3

/* How to use ACLK as a capture/compare input source */
/* Settings for TB0: T0B6 ccis=1 ; clk P4.7 ; cc0 P4.0 ; cc1 P4.1 */
#ifndef BSP430_TIMER_CCACLK_ACLK_CCIDX
/* NB: Check against BSP430_TIMER_CCACLK_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_ACLK_CCIDX 6
#endif /* BSP430_TIMER_CCACLK_ACLK_CCIDX */
#ifndef BSP430_TIMER_CCACLK_ACLK_CCIS
/* NB: Check against BSP430_TIMER_CCACLK_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_ACLK_CCIS CCIS_1
#endif /* BSP430_TIMER_CCACLK_ACLK_CCIS */
#ifndef BSP430_TIMER_CCACLK_CLK_PORT_BIT
/* NB: Check against BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_CLK_PORT_BIT BIT7
#endif /* BSP430_TIMER_CCACLK_CLK_PORT_BIT */
#ifndef BSP430_TIMER_CCACLK_CC0_PORT_BIT
/* NB: Check against BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_CC0_PORT_BIT BIT0
#endif /* BSP430_TIMER_CCACLK_CC0_PORT_BIT */
#ifndef BSP430_TIMER_CCACLK_CC1_PORT_BIT
/* NB: Check against BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_CC1_PORT_BIT BIT1
#endif /* BSP430_TIMER_CCACLK_CC1_PORT_BIT */

/* !BSP430! insert=rfem_platform platform=trxeb mcu=msp430f5438a */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [rfem_platform] */
#if (configBSP430_RFEM - 0)
#define BSP430_RFEM 1
#endif /* configBSP430_RFEM */
#if (BSP430_RFEM - 0)
#define BSP430_RFEM_SERIAL_PERIPH_HANDLE BSP430_PERIPH_USCI5_B0
#define BSP430_RFEM_RF1P3_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_RFEM_RF1P3_PORT_BIT BIT4
#define BSP430_RFEM_RF1P3_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA0
#define BSP430_RFEM_RF1P3_TIMER_CCIDX 3
#define BSP430_RFEM_RF1P3_TIMER_CCIS CCIS_0
#define BSP430_RFEM_RF1P4_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_RFEM_RF1P4_PORT_BIT BIT1
#define BSP430_RFEM_RF1P4_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA0
#define BSP430_RFEM_RF1P4_TIMER_CCIDX 0
#define BSP430_RFEM_RF1P4_TIMER_CCIS CCIS_0
#define BSP430_RFEM_RF1P5_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT8
#define BSP430_RFEM_RF1P5_PORT_BIT BIT2
#define BSP430_RFEM_RF1P5_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA0
#define BSP430_RFEM_RF1P5_TIMER_CCIDX 2
#define BSP430_RFEM_RF1P5_TIMER_CCIS CCIS_1
#define BSP430_RFEM_RF1P6_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_RFEM_RF1P6_PORT_BIT BIT5
#define BSP430_RFEM_RF1P6_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA0
#define BSP430_RFEM_RF1P6_TIMER_CCIDX 4
#define BSP430_RFEM_RF1P6_TIMER_CCIS CCIS_0
#define BSP430_RFEM_RF1P7_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3
#define BSP430_RFEM_RF1P7_PORT_BIT BIT4
#define BSP430_RFEM_RF1P9_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3
#define BSP430_RFEM_RF1P9_PORT_BIT BIT5
#define BSP430_RFEM_RF1P10_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_RFEM_RF1P10_PORT_BIT BIT7
#define BSP430_RFEM_RF1P11_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT8
#define BSP430_RFEM_RF1P11_PORT_BIT BIT3
#define BSP430_RFEM_RF1P11_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA0
#define BSP430_RFEM_RF1P11_TIMER_CCIDX 3
#define BSP430_RFEM_RF1P11_TIMER_CCIS CCIS_1
#define BSP430_RFEM_RF1P12_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_RFEM_RF1P12_PORT_BIT BIT3
#define BSP430_RFEM_RF1P12_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA0
#define BSP430_RFEM_RF1P12_TIMER_CCIDX 2
#define BSP430_RFEM_RF1P12_TIMER_CCIS CCIS_0
#define BSP430_RFEM_RF1P14_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3
#define BSP430_RFEM_RF1P14_PORT_BIT BIT0
#define BSP430_RFEM_RF1P15_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT8
#define BSP430_RFEM_RF1P15_PORT_BIT BIT4
#define BSP430_RFEM_RF1P15_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA0
#define BSP430_RFEM_RF1P15_TIMER_CCIDX 4
#define BSP430_RFEM_RF1P15_TIMER_CCIS CCIS_1
#define BSP430_RFEM_RF1P16_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3
#define BSP430_RFEM_RF1P16_PORT_BIT BIT3
#define BSP430_RFEM_RF1P17_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT8
#define BSP430_RFEM_RF1P17_PORT_BIT BIT5
#define BSP430_RFEM_RF1P17_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA1
#define BSP430_RFEM_RF1P17_TIMER_CCIDX 0
#define BSP430_RFEM_RF1P17_TIMER_CCIS CCIS_1
#define BSP430_RFEM_RF1P18_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3
#define BSP430_RFEM_RF1P18_PORT_BIT BIT1
#define BSP430_RFEM_RF1P20_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3
#define BSP430_RFEM_RF1P20_PORT_BIT BIT2
#define BSP430_RFEM_RF2P15_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT8
#define BSP430_RFEM_RF2P15_PORT_BIT BIT0
#define BSP430_RFEM_RF2P15_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA0
#define BSP430_RFEM_RF2P15_TIMER_CCIDX 0
#define BSP430_RFEM_RF2P15_TIMER_CCIS CCIS_1
#define BSP430_RFEM_RF2P17_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT8
#define BSP430_RFEM_RF2P17_PORT_BIT BIT1
#define BSP430_RFEM_RF2P17_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA0
#define BSP430_RFEM_RF2P17_TIMER_CCIDX 1
#define BSP430_RFEM_RF2P17_TIMER_CCIS CCIS_1
#define BSP430_RFEM_RF2P18_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_RFEM_RF2P18_PORT_BIT BIT2
#define BSP430_RFEM_RF2P18_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA0
#define BSP430_RFEM_RF2P18_TIMER_CCIDX 1
#define BSP430_RFEM_RF2P18_TIMER_CCIS CCIS_0
#define BSP430_RFEM_RF2P19_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_RFEM_RF2P19_PORT_BIT BIT0
#endif /* BSP430_RFEM */
/* END AUTOMATICALLY GENERATED CODE [rfem_platform] */
/* !BSP430! end=rfem_platform */

/* Enable U8GLIB if requested */
#define BSP430_UTILITY_U8GLIB (configBSP430_UTILITY_U8GLIB - 0)

/** @endcond */

#ifndef configBSP430_PLATFORM_TRXEB_ALS
/** Enable HPL support for on-board ambient light sensor
 *
 * The TrxEB has an SFH 5711 ambient light sensor from Osram on board,
 * positioned to the right and below the LCD.  It receives power
 * through P6.1, and provides an analog logarithmic measure of ambient
 * light over P6.2 which is A2 for the on-board ADC.  Defining this
 * constant to a true value enables the configuration of port 6 as
 * well as provides constants for applications to use the sensor. */
#define configBSP430_PLATFORM_TRXEB_ALS 0
#endif /* configBSP430_PLATFORM_TRXEB_ALS */

#if defined(BSP430_DOXYGEN) || (configBSP430_PLATFORM_TRXEB_ALS - 0)
/** BSP430 port HPL reference on which ambient light sensor is placed.
 * @dependency #configBSP430_PLATFORM_TRXEB_ALS
 */
#define BSP430_PLATFORM_TRXEB_ALS_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT6

/** Port bit on #BSP430_PLATFORM_TRXEB_ALS_PORT_PERIPH_HANDLE for ALS power
 * @dependency #configBSP430_PLATFORM_TRXEB_ALS
 */
#define BSP430_PLATFORM_TRXEB_ALS_PWR_PORT_BIT BIT1

/** Port bit on #BSP430_PLATFORM_TRXEB_ALS_PORT_PERIPH_HANDLE for ALS output
 * @dependency #configBSP430_PLATFORM_TRXEB_ALS
 */
#define BSP430_PLATFORM_TRXEB_ALS_OUT_PORT_BIT BIT2

/** ADC12 channel for ALS output
 * @dependency #configBSP430_PLATFORM_TRXEB_ALS
 */
#define BSP430_PLATFORM_TRXEB_ALS_OUT_INCH ADC12INCH_2
#endif /* configBSP430_PLATFORM_TRXEB_ALS */

#ifndef configBSP430_PLATFORM_TRXEB_ACCEL
/** Enable HPL support for on-board accelerometer
 *
 * The TrxEB has a CMA3000-D01 digital accelerometer from VTI on
 * board.  It receives power through P6.0, and supplies an interrupt
 * on P2.0.  Interface to it uses IO_SPI0.  Defining this constant to
 * a true value enables the configuration of port 6 as well as
 * provides constants for applications to use the sensor. */
#define configBSP430_PLATFORM_TRXEB_ACCEL 0
#endif /* configBSP430_PLATFORM_TRXEB_ACCEL */

#if defined(BSP430_DOXYGEN) || (configBSP430_PLATFORM_TRXEB_ACCEL - 0)

/** BSP430 peripheral handle for port on which accelerometer power is placed.
 * @dependency #configBSP430_PLATFORM_TRXEB_ACCEL
 */
#define BSP430_PLATFORM_TRXEB_ACCEL_PWR_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT6

/** Port bit on #BSP430_PLATFORM_TRXEB_ACCEL_PWR_PORT_PERIPH_HANDLE for accelerometer power
 * @dependency #configBSP430_PLATFORM_TRXEB_ACCEL
 */
#define BSP430_PLATFORM_TRXEB_ACCEL_PWR_PORT_BIT BIT0

/** BSP430 peripheral handle for port on which accelerometer chip-select (inverted) is placed.
 * @dependency #configBSP430_PLATFORM_TRXEB_ACCEL
 */
#define BSP430_PLATFORM_TRXEB_ACCEL_CSn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT8

/** Port bit on #BSP430_PLATFORM_TRXEB_ACCEL_CSn_PORT_PERIPH_HANDLE for accelerometer CSn
 * @dependency #configBSP430_PLATFORM_TRXEB_ACCEL
 */
#define BSP430_PLATFORM_TRXEB_ACCEL_CSn_PORT_BIT BIT7

#ifndef configBSP430_PLATFORM_TRXEB_ACCEL_IRQ
/** Enable HAL support for on-board accelerometer interrupt.
 *
 * @dependency #configBSP430_PLATFORM_TRXEB_ACCEL
 * @affects #BSP430_PLATFORM_TRXEB_ACCEL_IRQ_PORT_PERIPH_HANDLE
 */
#define configBSP430_PLATFORM_TRXEB_ACCEL_IRQ 0
#endif /* configBSP430_PLATFORM_TRXEB_ACCEL_IRQ */

#if defined(BSP430_DOXYGEN) || (configBSP430_PLATFORM_TRXEB_ACCEL_IRQ - 0)
/** BSP430 peripheral handle for port on which accelerometer power is placed.
 * @dependency #configBSP430_PLATFORM_TRXEB_ACCEL_IRQ
 */
#define BSP430_PLATFORM_TRXEB_ACCEL_IRQ_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2

/** Port bit on #BSP430_PLATFORM_TRXEB_ACCEL_IRQ_PORT_PERIPH_HANDLE for accelerometer IRQ
 * @dependency #configBSP430_PLATFORM_TRXEB_ACCEL_IRQ
 */
#define BSP430_PLATFORM_TRXEB_ACCEL_IRQ_PORT_BIT BIT0

#endif /* configBSP430_PLATFORM_TRXEB_ACCEL_IRQ */

#endif /* configBSP430_PLATFORM_TRXEB_ACCEL */

#ifndef configBSP430_PLATFORM_M25P
/** Enable HPL support for on-board SPI flash
 *
 * The TrxEB has a M25PE20 2 Mib (256 kiB) serial flash from Micron on
 * board.  It receives power through P7.6, and has RESETn on P7.2.
 * Interface to it uses IO_SPI0.  Defining this constant to a true
 * value enables the configuration of port 7 as well as provides
 * constants for applications to use the flash. */
#define configBSP430_PLATFORM_M25P 0
#endif /* configBSP430_PLATFORM_M25P */

#if (configBSP430_PLATFORM_M25P - 0)

/** Peripheral handle for SPI access to serial flash */
#define BSP430_PLATFORM_M25P_SPI_PERIPH_HANDLE BSP430_PLATFORM_TRXEB_IO_SPI0_PERIPH_HANDLE

/** BSP430 peripheral handle for port on which SPI flash power is placed.
 * @dependency #configBSP430_PLATFORM_M25P
 */
#define BSP430_PLATFORM_M25P_PWR_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT7

/** Port bit on #BSP430_PLATFORM_M25P_PWR_PORT_PERIPH_HANDLE for SPI flash power
 * @dependency #configBSP430_PLATFORM_M25P
 */
#define BSP430_PLATFORM_M25P_PWR_PORT_BIT BIT6

/** BSP430 peripheral handle for port on which SPI flash power is placed.
 * @dependency #configBSP430_PLATFORM_M25P
 */
#define BSP430_PLATFORM_M25P_RSTn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT7

/** Port bit on #BSP430_PLATFORM_M25P_RSTn_PORT_PERIPH_HANDLE for SPI flash power
 * @dependency #configBSP430_PLATFORM_M25P
 */
#define BSP430_PLATFORM_M25P_RSTn_PORT_BIT BIT2

/** BSP430 peripheral handle for port on which SPI flash chip-select (inverted) is placed.
 * @dependency #configBSP430_PLATFORM_M25P
 */
#define BSP430_PLATFORM_M25P_CSn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT8

/** Port bit on #BSP430_PLATFORM_M25P_CSn_PORT_PERIPH_HANDLE for SPI flash CSn
 * @dependency #configBSP430_PLATFORM_M25P
 */
#define BSP430_PLATFORM_M25P_CSn_PORT_BIT BIT6

/* M25PE20 supports #BSP430_M25P_CMD_PW */
#define BSP430_PLATFORM_M25P_SUPPORTS_PW 1

/* M25PE20 supports #BSP430_M25P_CMD_PE */
#define BSP430_PLATFORM_M25P_SUPPORTS_PE 1

/* M25PE20 supports #BSP430_M25P_CMD_SSE */
#define BSP430_PLATFORM_M25P_SUPPORTS_SSE 1

/* M25PE20 has four 64 kiB sectors */
#define BSP430_PLATFORM_M25P_SECTOR_SIZE 0x10000

/* M25PE20 has sixty-four 4 kiB subsectors */
#define BSP430_PLATFORM_M25P_SUBSECTOR_SIZE 0x1000

/* M25PE20 has four 64 kiB sectors */
#define BSP430_PLATFORM_M25P_SECTOR_COUNT 4

#endif /* configBSP430_PLATFORM_M25P */

#ifndef configBSP430_PLATFORM_TRXEB_LCD

/** Enable HPL support for on-board LCD
 *
 * The TrxEB has a DOGM128E-6 monochromatic 128x64 pixel LCD on board.
 * It receives power and does control through pins on ports 7 and 9.
 *
 * There is a backlight infrastructure on P3.6, but the additional
 * circuitry for the module is not included on the board.
 *
 * The LCD is controlled using IO_SPI0.
 *
 * Defining this constant to a true value enables the configuration of
 * the HPL for ports 7 and 9 and the SPI interface.
 *
 * @see #configBSP430_UTILITY_U8GLIB
 * @defaulted
 * @cppflag */
#define configBSP430_PLATFORM_TRXEB_LCD 0
#endif /* configBSP430_PLATFORM_TRXEB_LCD */

#if defined(BSP430_DOXYGEN) || (configBSP430_PLATFORM_TRXEB_LCD - 0)

/** Peripheral handle for SPI access to TrxEB LCD */
#define BSP430_PLATFORM_TRXEB_LCD_SPI_PERIPH_HANDLE BSP430_PLATFORM_TRXEB_IO_SPI0_PERIPH_HANDLE

/** BSP430 peripheral handle for port to which LCD power is connected.
 * @dependency #configBSP430_PLATFORM_TRXEB_LCD
 */
#define BSP430_PLATFORM_TRXEB_LCD_PWR_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT7

/** Port bit on #BSP430_PLATFORM_TRXEB_LCD_PWR_PORT_PERIPH_HANDLE for LCD power
 * @dependency #configBSP430_PLATFORM_TRXEB_LCD
 */
#define BSP430_PLATFORM_TRXEB_LCD_PWR_PORT_BIT BIT7

/** BSP430 peripheral handle for port to which LCD reset (inverted) is connected.
 * @dependency #configBSP430_PLATFORM_TRXEB_LCD
 */
#define BSP430_PLATFORM_TRXEB_LCD_RSTn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT7

/** Port bit on #BSP430_PLATFORM_TRXEB_LCD_RSTn_PORT_PERIPH_HANDLE for LCD RSTn
 * @dependency #configBSP430_PLATFORM_TRXEB_LCD
 */
#define BSP430_PLATFORM_TRXEB_LCD_RSTn_PORT_BIT BIT3

/** BSP430 peripheral handle for port to which LCD chip-select (inverted) is connected.
 * @dependency #configBSP430_PLATFORM_TRXEB_LCD
 */
#define BSP430_PLATFORM_TRXEB_LCD_CSn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT9

/** Port bit on #BSP430_PLATFORM_TRXEB_LCD_CSn_PORT_PERIPH_HANDLE for LCD CSn
 * @dependency #configBSP430_PLATFORM_TRXEB_LCD
 */
#define BSP430_PLATFORM_TRXEB_LCD_CSn_PORT_BIT BIT6

/** BSP430 peripheral handle for port to which LCD A0 (CMD=0, DATA=1) is connected.
 * @dependency #configBSP430_PLATFORM_TRXEB_LCD
 */
#define BSP430_PLATFORM_TRXEB_LCD_A0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT9

/** Port bit on #BSP430_PLATFORM_TRXEB_LCD_A0_PORT_PERIPH_HANDLE for LCD A0
 * @dependency #configBSP430_PLATFORM_TRXEB_LCD
 */
#define BSP430_PLATFORM_TRXEB_LCD_A0_PORT_BIT BIT7

/** Width, in pixel columns, of the DOGM128E-6 display */
#define BSP430_PLATFORM_TRXEB_LCD_COLUMNS 128

/** Height, in pages, of the DOGM128E-6 display */
#define BSP430_PLATFORM_TRXEB_LCD_PAGES 8

/** Height, in pixel rows, of each page of the DOGM128E-6 display */
#define BSP430_PLATFORM_TRXEB_LCD_ROWS_PER_PAGE 8

#endif /* configBSP430_PLATFORM_TRXEB_LCD */

#ifndef configBSP430_PLATFORM_TRXEB_IO_SPI0
/** Enable use of IO_SPI0 peripheral.
 *
 * This peripheral services the SPI flash and LCD, and is default
 * enabled when #configBSP430_PLATFORM_M25P or
 * #configBSP430_PLATFORM_TRXEB_LCD is enabled.
 *
 * @defaulted
 * @cppflag
 */
#define configBSP430_PLATFORM_TRXEB_IO_SPI0 ((configBSP430_PLATFORM_M25P - 0) || (configBSP430_PLATFORM_TRXEB_LCD - 0))
#endif /* configBSP430_PLATFORM_TRXEB_IO_SPI0 */

#ifndef configBSP430_PLATFORM_TRXEB_IO_SPI1
/** Enable use of IO_SPI1 peripheral.
 *
 * This peripheral services the accelerometer, and is default enabled
 * when #configBSP430_PLATFORM_TRXEB_ACCEL is enabled.
 *
 * @defaulted
 * @cppflag
 */
#define configBSP430_PLATFORM_TRXEB_IO_SPI1 (configBSP430_PLATFORM_TRXEB_ACCEL - 0)
#endif /* configBSP430_PLATFORM_TRXEB_IO_SPI1 */

#if (configBSP430_PLATFORM_TRXEB_ACCEL - 0)
/** Peripheral handle for SPI access to accelerometer */
#define BSP430_PLATFORM_TRXEB_ACCEL_SPI_PERIPH_HANDLE BSP430_PLATFORM_TRXEB_IO_SPI1_PERIPH_HANDLE
#endif /* configBSP430_PLATFORM_TRXEB_ACCEL */

#if defined(BSP430_DOXYGEN) || (configBSP430_PLATFORM_TRXEB_IO_SPI0 - 0)
/** BSP430 port peripheral handle for IO_SPI0.
 *
 * IO_SPI0 on USCI_B2 services the LCD and the serial flash.
 * @dependency #configBSP430_PLATFORM_TRXEB_IO_SPI0 */
#define BSP430_PLATFORM_TRXEB_IO_SPI0_PERIPH_HANDLE BSP430_PERIPH_USCI5_B2
#endif /* configBSP430_PLATFORM_TRXEB_IO_SPI0 */

#if defined(BSP430_DOXYGEN) || (configBSP430_PLATFORM_TRXEB_IO_SPI1 - 0)
/** BSP430 port peripheral handle for IO_SPI1.
 *
 * IO_SPI1 on USCI_A2 services the accelerometer.
 * @dependency #configBSP430_PLATFORM_TRXEB_IO_SPI1 */
#define BSP430_PLATFORM_TRXEB_IO_SPI1_PERIPH_HANDLE BSP430_PERIPH_USCI5_A2
#endif /* configBSP430_PLATFORM_TRXEB_IO_SPI1 */

/* Include generic file, in case this is being included directly */
#include <bsp430/platform.h>

#endif /* BSP430_PLATFORM_TRXEB_H */
