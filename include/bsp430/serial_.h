/* Copyright (c) 2012, Peter A. Bigot
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
 * @brief Structures for abstracted serial interface.
 *
 * The MSP430 line has a variety of peripherals that support serial
 * communications, including the origin 2xx/4xx family USCI, 5xx USCI,
 * and 5xx eUSCI.  These are sufficiently similar in function that it
 * is useful to have a hardware abstraction layer that delegates to
 * the peripherals available on the current platform.
 *
 * To break circular dependencies between the shared <bsp430/serial.h>
 * file and the individual peripherals that are supported by it, this
 * header contains the structures that are required by the individual
 * peripherals to declare their own interfaces.  Those headers then
 * include this one, and <bsp430/serial.h> includes the relevant
 * individual peripheral headers.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_SERIAL__H
#define BSP430_SERIAL__H

#include <bsp430/core.h>
#include <bsp430/periph.h>

/** Field value for variant stored in
 * sBSP430halSERIAL.hal_state.cflags when HPL reference is to an
 * #sBSP430hplUSCI. */
#define BSP430_SERIAL_HAL_HPL_VARIANT_USCI 1

/** Field value for variant stored in
 * sBSP430halSERIAL.hal_state.cflags when HPL reference is to an
 * #sBSP430hplUSCI5. */
#define BSP430_SERIAL_HAL_HPL_VARIANT_USCI5 2

/** Field value for variant stored in
 * sBSP430halSERIAL.hal_state.cflags when HPL reference is to an
 * #sBSP430hplEUSCIA. */
#define BSP430_SERIAL_HAL_HPL_VARIANT_EUSCIA 3

/** Field value for variant stored in
 * sBSP430halSERIAL.hal_state.cflags when HPL reference is to an
 * #sBSP430hplEUSCIA. */
#define BSP430_SERIAL_HAL_HPL_VARIANT_EUSCIB 4

/* !BSP430! instance=usci,usci5,euscia,euscib */
/* !BSP430! periph=serial insert=hal_variant_hpl_macro */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_variant_hpl_macro] */
/** True iff the HPL pointer of the serial HAL is a USCI variant. */
#define BSP430_SERIAL_HAL_HPL_VARIANT_IS_USCI(_hal) (BSP430_SERIAL_HAL_HPL_VARIANT_USCI == BSP430_PERIPH_HAL_STATE_CFLAGS_VARIANT(_hal))

/** Get the serial HPL pointer if the HAL is a USCI variant.
 *
 * Value is a null pointer if the HAL references a different HPL type. */
#define BSP430_SERIAL_HAL_GET_HPL_USCI(_hal) ((BSP430_SERIAL_HAL_HPL_VARIANT_IS_USCI(_hal)) ? (_hal)->hpl.usci : (void *)0)

/** Get the serial HPL auxiliary pointer if the HAL is a USCI variant.
 *
 * Value is a null pointer if the HAL references a different HPL type. */
#define BSP430_SERIAL_HAL_GET_HPLAUX_USCI(_hal) ((BSP430_SERIAL_HAL_HPL_VARIANT_IS_USCI(_hal)) ? (_hal)->hpl_aux.usci : (void *)0)

/** True iff the HPL pointer of the serial HAL is a USCI5 variant. */
#define BSP430_SERIAL_HAL_HPL_VARIANT_IS_USCI5(_hal) (BSP430_SERIAL_HAL_HPL_VARIANT_USCI5 == BSP430_PERIPH_HAL_STATE_CFLAGS_VARIANT(_hal))

/** Get the serial HPL pointer if the HAL is a USCI5 variant.
 *
 * Value is a null pointer if the HAL references a different HPL type. */
#define BSP430_SERIAL_HAL_GET_HPL_USCI5(_hal) ((BSP430_SERIAL_HAL_HPL_VARIANT_IS_USCI5(_hal)) ? (_hal)->hpl.usci5 : (void *)0)

/** Get the serial HPL auxiliary pointer if the HAL is a USCI5 variant.
 *
 * Value is a null pointer if the HAL references a different HPL type. */
#define BSP430_SERIAL_HAL_GET_HPLAUX_USCI5(_hal) ((BSP430_SERIAL_HAL_HPL_VARIANT_IS_USCI5(_hal)) ? (_hal)->hpl_aux.usci5 : (void *)0)

/** True iff the HPL pointer of the serial HAL is a EUSCIA variant. */
#define BSP430_SERIAL_HAL_HPL_VARIANT_IS_EUSCIA(_hal) (BSP430_SERIAL_HAL_HPL_VARIANT_EUSCIA == BSP430_PERIPH_HAL_STATE_CFLAGS_VARIANT(_hal))

/** Get the serial HPL pointer if the HAL is a EUSCIA variant.
 *
 * Value is a null pointer if the HAL references a different HPL type. */
#define BSP430_SERIAL_HAL_GET_HPL_EUSCIA(_hal) ((BSP430_SERIAL_HAL_HPL_VARIANT_IS_EUSCIA(_hal)) ? (_hal)->hpl.euscia : (void *)0)

/** Get the serial HPL auxiliary pointer if the HAL is a EUSCIA variant.
 *
 * Value is a null pointer if the HAL references a different HPL type. */
#define BSP430_SERIAL_HAL_GET_HPLAUX_EUSCIA(_hal) ((BSP430_SERIAL_HAL_HPL_VARIANT_IS_EUSCIA(_hal)) ? (_hal)->hpl_aux.euscia : (void *)0)

/** True iff the HPL pointer of the serial HAL is a EUSCIB variant. */
#define BSP430_SERIAL_HAL_HPL_VARIANT_IS_EUSCIB(_hal) (BSP430_SERIAL_HAL_HPL_VARIANT_EUSCIB == BSP430_PERIPH_HAL_STATE_CFLAGS_VARIANT(_hal))

/** Get the serial HPL pointer if the HAL is a EUSCIB variant.
 *
 * Value is a null pointer if the HAL references a different HPL type. */
#define BSP430_SERIAL_HAL_GET_HPL_EUSCIB(_hal) ((BSP430_SERIAL_HAL_HPL_VARIANT_IS_EUSCIB(_hal)) ? (_hal)->hpl.euscib : (void *)0)

/** Get the serial HPL auxiliary pointer if the HAL is a EUSCIB variant.
 *
 * Value is a null pointer if the HAL references a different HPL type. */
#define BSP430_SERIAL_HAL_GET_HPLAUX_EUSCIB(_hal) ((BSP430_SERIAL_HAL_HPL_VARIANT_IS_EUSCIB(_hal)) ? (_hal)->hpl_aux.euscib : (void *)0)

/* END AUTOMATICALLY GENERATED CODE [hal_variant_hpl_macro] */
/* !BSP430! end=hal_variant_hpl_macro */

/** Specify byte written when reading a SPI byte.
 * 
 * The SPI protocal requires that a byte be written for every byte
 * received; the reception occurs during the write.  In many cases,
 * the value of the byte written is completely irrelevant.  For the
 * purposes of debugging a SPI bus trace, it's nice to have an ordinal
 * value written, so this is the default.
 *
 * However, some peripherals require a specific value; in particular
 * an MMC-based SD card apparently requires 0xFF.  Allow the default
 * to be overridden by the application.
 *
 * @param i_ the ordinal of the received byte
 *
 * @defaulted
 */
#ifndef BSP430_SERIAL_SPI_READ_TX_BYTE
#define BSP430_SERIAL_SPI_READ_TX_BYTE(i_) (i_)
#endif /* BSP430_SERIAL_SPI_READ_TX_BYTE */

/* Forward declarations */
struct sBSP430hplUSCI;
struct sBSP430usciHPLAux;
struct sBSP430hplUSCI5;
struct sBSP430hplEUSCIA;
struct sBSP430hplEUSCIB;
struct sBSP430serialDispatch;

/** Structure holding hardware abstraction layer state for serial
 * devices. */
typedef struct sBSP430halSERIAL {
  /** Common header used to extract the correct HPL pointer type from
   * the hpl union. */
  sBSP430hplHALStatePrefix hal_state;

  /** Allow the HAL state to be independent of the underlying HPL layout.
   *
   * Use #BSP430_PERIPH_HAL_STATE_CFLAGS_VARIANT(), or the selector
   * macros like #BSP430_SERIAL_HAL_HPL_VARIANT_IS_USCI() and
   * #BSP430_SERIAL_HAL_GET_HPL_USCI(), to introspect a HAL
   * instance. */
  union {
    /** Access to the HPL pointer ignoring its underlying type */
    volatile void * any;

    /** Access to the HPL pointer as a 2xx/4xx USCI peripheral */
    volatile struct sBSP430hplUSCI * usci;

    /** Access to the HPL pointer as a 5xx/6xx USCI peripheral */
    volatile struct sBSP430hplUSCI5 * usci5;

    /** Access to the HPL pointer as a 5xx/6xx eUSCI type A peripheral */
    volatile struct sBSP430hplEUSCIA * euscia;

    /** Access to the HPL pointer as a 5xx/6xx eUSCI type B peripheral */
    volatile struct sBSP430hplEUSCIB * euscib;
  } const hpl;

  /** Support for additional data specific to the HPL that is not part
   * of the standard HPL structure.
   *
   * This is necessary for 2xx/4xx USCI, where the interrupt enable
   * and flag registers are outside the #sBSP430hplUSCI structure.
   * Future HPL implementations (e.g., USART) might need a similar
   * facility.
   *
   * Use #BSP430_SERIAL_HAL_GET_HPLAUX_USCI() to access the
   * corresponding structure, which is declared within the
   * peripheral-specific header. */
  union {
    /** Access to the HPL auxiliary pointer ignoring its underlying
     * type */
    void * any;
    /** Access to the HPL auxiliary pointer for the 2xx/4xx USCI
     * peripheral */
    struct sBSP430usciHPLAux * usci;
  } const hpl_aux;

  /** Location in which an incoming character is stored when an
   * #rx_cbchain_ni is non-null. */
  uint8_t rx_byte;

  /** Location in which an outgoing character is stored when a
   * #tx_cbchain_ni is non-null. */
  uint8_t tx_byte;

  /** The callback chain to invoke when a byte is received.
   *
   * A non-null value enables interrupt-driven reception, and data
   * will be provided to the callbacks on receiption.  The received
   * character will be stored in #rx_byte */
  const struct sBSP430halISRVoidChainNode * rx_cbchain_ni;

  /** The callback chain to invoke when space is available in the
   * transmission buffer
   *
   * A non-null value enables interrupt-driven transmission, and the
   * chain will be invoked as necessary.  The infrastructure must be
   * made aware of data to be transmitted, so it can enable the
   * interrupt that will request the data through the callback.  This
   * is done using vBSP430serialWakeupTransmit_ni().  If the callback
   * has data to transmit, it should store it in #tx_byte and include
   * #BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN in its return value.  It
   * should also include #BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT if
   * it is known that there will not be data available after the
   * transmission.  If the callback has no data to transmit, it should
   * return zero. */
  const struct sBSP430halISRVoidChainNode * tx_cbchain_ni;

  /** Total number of received octets */
  unsigned long num_rx;

  /** Total number of transmitted octets */
  unsigned long num_tx;

#if BSP430_SERIAL - 0
  /** @cond DOXYGEN_EXCLUDE */
  const struct sBSP430serialDispatch * const dispatch;
  /** @endcond */
#endif /* BSP430_SERIAL */
} sBSP430halSERIAL;

/** Handle for a serial HAL instance */
typedef struct sBSP430halSERIAL * hBSP430halSERIAL;

/** @cond DOXYGEN_EXCLUDE */
struct sBSP430serialDispatch {
#if configBSP430_SERIAL_ENABLE_UART - 0
  hBSP430halSERIAL (* openUART) (hBSP430halSERIAL hal,
                                 unsigned char ctl0_byte,
                                 unsigned char ctl1_byte,
                                 unsigned long baud);
  int (* uartRxByte_ni) (hBSP430halSERIAL hal);
  int (* uartTxByte_ni) (hBSP430halSERIAL hal,
                         uint8_t c);
  int (* uartTxData_ni) (hBSP430halSERIAL hal,
                         const uint8_t * data,
                         size_t len);
  int (* uartTxASCIIZ_ni) (hBSP430halSERIAL hal, const char * str);
#endif /* configBSP430_SERIAL_ENABLE_UART */
#if configBSP430_SERIAL_ENABLE_SPI - 0
  hBSP430halSERIAL (* openSPI) (hBSP430halSERIAL hal,
                                unsigned char ctl0_byte,
                                unsigned char ctl1_byte,
                                unsigned int prescaler);
  int (* spiTxRx_ni) (hBSP430halSERIAL hal, const uint8_t * tx_data, size_t tx_len, size_t rx_len, uint8_t * rx_data);
#endif /* configBSP430_SERIAL_ENABLE_SPI */
#if configBSP430_SERIAL_ENABLE_I2C - 0
  hBSP430halSERIAL (* openI2C) (hBSP430halSERIAL hal,
                                unsigned char ctl0_byte,
                                unsigned char ctl1_byte,
                                unsigned int prescaler);
  int (* i2cSetAddresses_ni) (hBSP430halSERIAL hal, int own_address, int slave_address);
  int (* i2cRxData_ni) (hBSP430halSERIAL hal, uint8_t * rx_data, size_t rx_len);
  int (* i2cTxData_ni) (hBSP430halSERIAL hal, const uint8_t * tx_data, size_t tx_len);
#endif /* configBSP430_SERIAL_ENABLE_I2C */
  int (* setHold_ni) (hBSP430halSERIAL hal, int holdp);
  int (* close) (hBSP430halSERIAL hal);
  void (* wakeupTransmit_ni) (hBSP430halSERIAL hal);
  void (* flush_ni) (hBSP430halSERIAL hal);
};
/** @endcond */

#endif /* BSP430_SERIAL__H */
