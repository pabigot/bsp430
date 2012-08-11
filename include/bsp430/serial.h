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

/** @file
 *
 * @brief Abstracted serial interface.
 *
 * The MSP430 line has a variety of peripherals that support serial
 * communications, including the origin 2xx/4xx family USCI, 5xx USCI,
 * and 5xx eUSCI.  These are sufficiently similar in function that it
 * is useful to have a hardware abstraction layer that delegates to
 * the peripherals available on the current platform.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_SERIAL_H
#define BSP430_SERIAL_H

#include <bsp430/common.h>
#include <bsp430/periph.h>

/** Field value for variant stored in
 * #sBSP430halSERIAL.hal_state.cflags when HPL reference is to an
 * #sBSP430hplUSCI. */
#define BSP430_SERIAL_HAL_HPL_VARIANT_USCI 1

/** Field value for variant stored in
 * #sBSP430halSERIAL.hal_state.cflags when HPL reference is to an
 * #sBSP430hplUSCI5. */
#define BSP430_SERIAL_HAL_HPL_VARIANT_USCI5 2

/** Field value for variant stored in
 * #sBSP430halSERIAL.hal_state.cflags when HPL reference is to an
 * #sBSP430hplEUSCIA. */
#define BSP430_SERIAL_HAL_HPL_VARIANT_EUSCIA 3

/** Field value for variant stored in
 * #sBSP430halSERIAL.hal_state.cflags when HPL reference is to an
 * #sBSP430hplEUSCIA. */
#define BSP430_SERIAL_HAL_HPL_VARIANT_EUSCIB 4

/* NOTE TO MAINTAINER: You will be tempted to refactor the boolean
 * configBSP430_SERIAL_USE_FOO flag to a valued
 * configBSP430_SERIAL_USE_VARIANT==BSP430_SERIAL_HAL_HPL_VARIANT_FOO
 * flag.  Don't.  There are MCUs (such as msp430fg4618) that provide
 * serial capabilities through multiple modules (USART+USCI) where SPI
 * might come from one and UART from another. */

/** @def configBSP430_SERIAL_USE_USCI
 *
 * Define to true value to allow the generic serial dispatches to
 * recognize #sBSP430hplUSCI as an underlying serial implementation.
 * This defaults to true iff the MCU supports the USCI device as
 * implemented on 2xx/4xx MCUs.
 *
 * @cppflag
 * @defaulted  */
#ifndef configBSP430_SERIAL_USE_USCI
#define configBSP430_SERIAL_USE_USCI defined(__MSP430_HAS_USCI__)
#endif /* configBSP430_SERIAL_USE_USCI */

/** @def configBSP430_SERIAL_USE_USCI5
 *
 * Define to true value to allow the generic serial dispatches to
 * recognize #sBSP430hplUSCI5 as an underlying serial implementation.
 * This defaults to true iff the MCU supports the USCI device as
 * implemented on 5xx MCUs.
 *
 * @cppflag
 * @defaulted  */
#ifndef configBSP430_SERIAL_USE_USCI5
#define configBSP430_SERIAL_USE_USCI5 defined(__MSP430_HAS_USCI_A0__)
#endif /* configBSP430_SERIAL_USE_USCI5 */

/** @def configBSP430_SERIAL_USE_EUSCI
 *
 * Define to true value to allow the generic serial dispatches to
 * recognize #sBSP430hplEUSCIA and #sBSP430hplEUSCIB as underlying
 * serial implementation.  This defaults to true iff the MCU supports
 * the eUSCI device as implemented on 5xx MCUs.
 *
 * @cppflag
 * @defaulted  */
#ifndef configBSP430_SERIAL_USE_EUSCI
#define configBSP430_SERIAL_USE_EUSCI (defined(__MSP430_HAS_EUSCI_A0__) || defined(__MSP430_HAS_EUSCI_B0__))
#endif /* configBSP430_SERIAL_USE_EUSCI */

/** @def configBSP430_SERIAL_ENABLE_UART
 *
 * Define to a true value to allow the general serial layer to
 * recognize and dispatch UART-related functions.  If left false,
 * UART-related functions will not be added to the serial abstraction
 * dispatch table.
 *
 * @note This flag does not control the availabilty of UART-related
 * functions within the peripheral implementation, only access to them
 * through the serial abstraction.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_SERIAL_ENABLE_UART
#define configBSP430_SERIAL_ENABLE_UART (configBSP430_CONSOLE - 0)
#endif /* configBSP430_SERIAL_ENABLE_UART */

/** @def configBSP430_SERIAL_ENABLE_SPI
 *
 * Define to a true value to allow the general serial layer to
 * recognize and dispatch SPI-related functions.  If left false,
 * SPI-related functions will not be added to the serial abstraction
 * dispatch table.
 *
 * @note This flag does not control the availabilty of SPI-related
 * functions within the peripheral implementation, only access to them
 * through the serial abstraction.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_SERIAL_ENABLE_SPI
#define configBSP430_SERIAL_ENABLE_SPI 0
#endif /* configBSP430_SERIAL_ENABLE_SPI */

/** @def configBSP430_SERIAL_ENABLE_I2C
 *
 * Define to a true value to allow the general serial layer to
 * recognize and dispatch I2C-related functions.  If left false,
 * I2C-related functions will not be added to the serial abstraction
 * dispatch table.
 *
 * @note This flag does not control the availabilty of I2C-related
 * functions within the peripheral implementation, only access to them
 * through the serial abstraction.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_SERIAL_ENABLE_I2C
#define configBSP430_SERIAL_ENABLE_I2C 0
#endif /* configBSP430_SERIAL_ENABLE_I2C */

/** @def BSP430_SERIAL
 *
 * Defined by the infrastructure to a true expression in the case
 * where at least one protocol is to be supported by the serial
 * abstraction.  If it evaluates to false, the serial abstraction will
 * be absent from the compiled infrastructure code.
 *
 * @cppflag */
#define BSP430_SERIAL                           \
  ((configBSP430_SERIAL_ENABLE_UART - 0)        \
   || (configBSP430_SERIAL_ENABLE_SPI - 0)      \
   || (configBSP430_SERIAL_ENABLE_I2C - 0))

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
   * corresponding structure, which is defined internally to the
   * implementation. */
  union {
    /** Access to the HPL auxiliary pointer ignoring its underlying
     * type */
    void * any;
    /** Access to the HPL auxiliary pointer for the 2xx/4xx USCI
     * peripheral */
    struct sBSP430usciHPLAux * usci;
  } const hpl_aux;

  /** Location in which an incoming character is stored when an
   * #rx_callback is defined. */
  uint8_t rx_byte;

  /** Location in which an outgoing character is stored when a
   * #tx_callback is defined. */
  uint8_t tx_byte;

  /** The callback chain to invoke when a byte is received.
   *
   * A non-null value enables interrupt-driven reception, and data
   * will be provided to the callbacks on receiption.  The received
   * character will be stored in #rx_byte */
  const struct sBSP430halISRCallbackVoid * rx_callback;

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
  const struct sBSP430halISRCallbackVoid * tx_callback;

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

#if defined(BSP430_DOXYGEN) || (BSP430_SERIAL - 0)

#if defined(BSP430_DOXYGEN) || (configBSP430_SERIAL_ENABLE_UART - 0)

/** Request and configure a serial device in UART mode.
 *
 * @warning When the underlying implementation is an EUSCI device (as
 * on FR5xx chips), the header defines used to construct the value @a
 * ctl0_byte are specified for a 16-bit access.  The ctl0 byte is the
 * upper byte of the ctlw0 word that comprises @a ctl0 and @a ctl1 on
 * these MCUs, so on those devices you must use the @c _H suffix to
 * select the high-byte version of these constants or divide your
 * configured value by 256 to place it in the low byte of the
 * argument.
 *
 * @param hal the handle for the HAL interface for the serial device
 * to be configured.  These are found from the peripheral handle using
 * hBSP430serialLookup().
 *
 * @param ctl0_byte The configuration to be written to the device's
 * ctl0 byte.  For UART mode, potential values specified in the
 * <msp430.h> header include #UCPEN, #UCPAR, #UCMSB, #UC7BIT, #UCSPB
 * (but <b>see the warning above</b>).  The UCMODE and UCSYNC elements
 * of the byte are ignored.  In most cases for UART mode a value of 0
 * producing 8N1 serial communications is appropriate.
 *
 * @param ctl1_byte The configuration to be written to the device's
 * ctl1 byte.  For UART mode, potential values specified in the
 * <msp430.h> header include #UCRXEIE, #UCBRKIE, and #UCDORM.  The
 * UCSSEL and UCSWRST elements of the byte are cleared as the function
 * configures those.  In most cases a value of 0 is appropriate.
 *
 * @param baud The desired baud rate.  This will be configured
 * automatically.  ACLK will be used as the baud rate clock if ACLK is
 * both at least 20 kHz and at least three times faster than the
 * requested baud rate; otherwise SMCLK will be used.  The function
 * invokes ulBSP430clockSMCLK_Hz() and usBSP430clockACLK_Hz() as
 * necessary to determine the actual speed of the baud rate clock.
 *
 * @return @a A peripheral-specific HAL handle if the allocation and
 * configuration is successful, and a null handle if something went
 * wrong.
 *
 * @delegated This function exists only as an inline delegate to a
 * peripheral-specific implementation.
 *
 * @dependency #configBSP430_SERIAL_ENABLE_UART */
static __inline__
hBSP430halSERIAL hBSP430serialOpenUART (hBSP430halSERIAL hal,
                                        unsigned char ctl0_byte,
                                        unsigned char ctl1_byte,
                                        unsigned long baud)
{
  return hal->dispatch->openUART(hal, ctl0_byte, ctl1_byte, baud);
}

/** Receive a byte from a UART-configured device.
 *
 * This routine should only be invoked when @link
 * sBSP430halSERIAL.rx_callback @a hal->rx_callback @endlink is null.
 * If a callback is present, it is expected to be used to accept data
 * on reception.
 *
 * @param hal the serial device over which the data should be
 * transmitted
 *
 * @return the character received if any is ready to return, or -1 if
 * the device has no data available
 *
 * @delegated This function exists only as an inline delegate to a
 * peripheral-specific implementation. */
static __inline__
int iBSP430uartRxByte_ni (hBSP430halSERIAL hal)
{
  return hal->dispatch->uartRxByte_ni(hal);
}

/** Transmit a byte over a UART-configured device.
 *
 * This routine should only be invoked when @link
 * sBSP430halSERIAL.tx_callback @a hal->tx_callback @endlink is null.
 * If a callback is present, it is expected to be used to provide data
 * for transmission.
 *
 * @param hal the serial device over which the data should be
 * transmitted
 *
 * @param c a data byte to be transmitted
 *
 * @return the input character @a c if transmitted, or -1 if an error
 * occurred
 *
 * @delegated This function exists only as an inline delegate to a
 * peripheral-specific implementation. */
static __inline__
int iBSP430uartTxByte_ni (hBSP430halSERIAL hal, uint8_t c)
{
  return hal->dispatch->uartTxByte_ni(hal, c);
}

/** Transmit a block of data over a UART-configured device.
 *
 * This routine should only be invoked when @link
 * sBSP430halSERIAL.tx_callback @a hal->tx_callback @endlink is null.
 * If a callback is present, it is expected to be used to provide data
 * for transmission.
 *
 * @param hal the serial device over which the data should be
 * transmitted
 *
 * @param data pointer to the start of a sequence of data to transmit
 *
 * @param len the number of octets to be transmitted
 *
 * @return the number of octets successfully transmitted
 *
 * @delegated This function exists only as an inline delegate to a
 * peripheral-specific implementation. */
static __inline__
int iBSP430uartTxData_ni (hBSP430halSERIAL hal,
                          const uint8_t * data,
                          size_t len)
{
  return hal->dispatch->uartTxData_ni(hal, data, len);
}

/** Transmit a sequence of characters over a UART-configured device.
 *
 * This routine should only be invoked when @link
 * sBSP430halSERIAL.tx_callback @a hal->tx_callback @endlink is null.
 * If a callback is present, it is expected to be used to provide data
 * for transmission.
 *
 * @param hal the serial device over which the data should be
 * transmitted
 *
 * @param str a NUL-terminated sequence of character data to be
 * transmitted.  The NUL serves only as a termination marker, and is
 * not transmitted.
 *
 * @return the number of bytes transmitted, or -1 if an error
 * occurs
 *
 * @delegated This function exists only as an inline delegate to a
 * peripheral-specific implementation. */
static __inline__
int iBSP430uartTxASCIIZ_ni (hBSP430halSERIAL hal, const char * str)
{
  return hal->dispatch->uartTxASCIIZ_ni(hal, str);
}
#endif /* configBSP430_SERIAL_ENABLE_UART */

#if defined(BSP430_DOXYGEN) || (configBSP430_SERIAL_ENABLE_SPI - 0)

/** Request and configure a serial device in SPI mode.
 *
 * @warning When the underlying implementation is an EUSCI device (as
 * on FR5xx chips), the header defines used to construct the value @a
 * ctl0_byte are specified for a 16-bit access.  The ctl0 byte is the
 * upper byte of the ctlw0 word that comprises @a ctl0 and @a ctl1 on
 * these MCUs, so on those devices you must use the @c _H suffix to
 * select the high-byte version of these constants or divide your
 * configured value by 256 to place it in the low byte of the
 * argument.
 *
 * @param hal the handle for the HAL interface for the serial device
 * to be configured.  These are found from the peripheral handle using
 * hBSP430serialLookup().
 *
 * @param ctl0_byte The configuration to be written to the device's
 * ctl0 byte.  For SPI mode, potential values specified in the
 * <msp430.h> header include #UCCKPH, #UCCKPL, #UCMSB, #UC7BIT,
 * #UCMST, and #UCMODE_0 or #UCMODE_1 or #UCMODE_2 (but <b>see the
 * warning above</b>).  The UCSYNC field is cleared before being
 * written.  Selection of #UCMODE_3 (I2C mode) will result in this
 * function returning an error.
 *
 * @param ctl1_byte The configuration to be written to the device's
 * ctl1 byte.  For SPI mode, potential values specified in the
 * <msp430.h> header are only configuration of the clock: #UCSSEL_0,
 * #UCSSEL_1, #UCSSEL_2, #UCSSEL_3.  The #UCSWRST field is controlled
 * by the function.
 *
 * @param prescaler The value by which the clock selected in @a
 * ctl1_byte is divided to produce the SPI clock.  A value of zero
 * will result in this function returning an error.
 *
 * @return @a A peripheral-specific HAL handle if the allocation and
 * configuration is successful, and a null handle if something went
 * wrong (e.g., absence of the requested @a periph, an invalid mode in
 * @a ctl0_byte or a zero-valued @a prescaler).
 *
 * @delegated This function exists only as an inline delegate to a
 * peripheral-specific implementation. */
static __inline__
hBSP430halSERIAL hBSP430serialOpenSPI (hBSP430halSERIAL hal,
                                       unsigned char ctl0_byte,
                                       unsigned char ctl1_byte,
                                       unsigned int prescaler)
{
  return hal->dispatch->openSPI(hal, ctl0_byte, ctl1_byte, prescaler);
}

/** Transmit and receive using a SPI-configured device
 *
 * This routine transmits @a tx_len octets from @a tx_data, storing
 * the octets received in response into @a rx_data.  It then transmits
 * @a rx_len dummy bytes, appending the resulting response into @a
 * rx_data.
 *
 * This routine should only be invoked when @link
 * sBSP430halSERIAL.tx_callback @a hal->tx_callback @endlink and @link
 * sBSP430halSERIAL.rx_callback @a hal->rx_callback @endlink are null.
 * If callbacks are present, they are expected to be used to provide
 * data for transmission and to process received data.
 *
 * @param hal the serial device over which the data is transmitted and
 * received
 *
 * @param tx_data the data to be transmitted (generally, a command).
 * The pointer may be null only if @a tx_len is zero.
 *
 * @param tx_len the number of bytes to transmit as the command.  The
 * value may be zero if this call is reading additional data resulting
 * from a previous command.
 *
 * @param rx_len the number of additional bytes expected in response,
 * exclusive of the synchronous responses to bytes transmitted from @a
 * tx_data.
 *
 * @param rx_data where to store the responses received during the
 * transmit and receive phases.  The space available must be at least
 * @a tx_len + @a rx_len.
 *
 * @return the total number of bytes stored in @a rx_data, or -1 if an
 * error occcured.
 */
static __inline__
int iBSP430spiTxRx_ni (hBSP430halSERIAL hal,
                       const uint8_t * tx_data,
                       size_t tx_len,
                       size_t rx_len,
                       uint8_t * rx_data)
{
  return hal->dispatch->spiTxRx_ni(hal, tx_data, tx_len, rx_len, rx_data);
}

#endif /* configBSP430_SERIAL_ENABLE_SPI */

#if defined(BSP430_DOXYGEN) || (configBSP430_SERIAL_ENABLE_I2C - 0)

/** Request and configure a serial device in I2C mode.
 *
 * @warning When the underlying implementation is an EUSCI device (as
 * on FR5xx chips), the header defines used to construct the value @a
 * ctl0_byte are specified for a 16-bit access.  The ctl0 byte is the
 * upper byte of the ctlw0 word that comprises @a ctl0 and @a ctl1 on
 * these MCUs, so on those devices you must use the @c _H suffix to
 * select the high-byte version of these constants or divide your
 * configured value by 256 to place it in the low byte of the
 * argument.
 *
 * @param hal the handle for the HAL interface for the serial device
 * to be configured.  These are found from the peripheral handle using
 * hBSP430serialLookup().
 *
 * @param ctl0_byte The configuration to be written to the device's
 * ctl0 byte.  For I2C mode, potential values specified in the
 * <msp430.h> header include #UCA10, #UCSLA10, #UCMMM, and #UCMST (but
 * <b>see the warning above</b>).  The UCSYNC field is cleared and the
 * UCMODE field is configured for I2C before being written.
 *
 * @param ctl1_byte The configuration to be written to the device's
 * ctl1 byte.  For I2C mode, potential values specified in the
 * <msp430.h> header are configuration of the clock (#UCSSEL_0,
 * #UCSSEL_1, #UCSSEL_2, or #UCSSEL_3) and perhaps #UCTR.  The
 * #UCSWRST field is controlled by the function.
 *
 * @param prescaler The value by which the clock selected in @a
 * ctl1_byte is divided to produce the I2C clock.  A value of zero
 * will result in this function returning an error.
 *
 * @return @a A peripheral-specific HAL handle if the allocation and
 * configuration is successful, and a null handle if something went
 * wrong (e.g., absence of the requested @a periph or a zero-valued @a
 * prescaler).
 *
 * @delegated This function exists only as an inline delegate to a
 * peripheral-specific implementation. */
static __inline__
hBSP430halSERIAL hBSP430serialOpenI2C (hBSP430halSERIAL hal,
                                       unsigned char ctl0_byte,
                                       unsigned char ctl1_byte,
                                       unsigned int prescaler)
{
  return hal->dispatch->openI2C(hal, ctl0_byte, ctl1_byte, prescaler);
}

/** Configure I2C addresses
 *
 * This routine sets the own-address and slave-address registers of an
 * I2C peripheral.  The device should have been opened as an I2C
 * device prior to invoking this function.
 *
 * @param hal the serial device to be configured
 *
 * @param own_address the value to use as this device's address.  A
 * negative value leaves the current configured own address unchanged.
 *
 * @param slave_address the value to use as the slave address.  A
 * negative value leaves the current configured slave address
 * unchanged.
 *
 * @return 0 if successfully set, -1 if an error occurs. */
static __inline__
int iBSP430i2cSetAddresses_ni (hBSP430halSERIAL hal,
                               int own_address,
                               int slave_address)
{
  return hal->dispatch->i2cSetAddresses_ni(hal, own_address, slave_address);
}

/** Transmit using an I2C-configured device
 *
 * This routine transmits @a tx_len octets from @a tx_data.  It will
 * return an error if the device is configured with a transmit
 * callback.
 *
 * This routine should only be invoked when @link
 * sBSP430halSERIAL.tx_callback @a hal->tx_callback @endlink is null.
 * If a callback is present, it is expected to be used to provide data
 * for transmission.  Note that such a callback must handle I2C start
 * and stop conditions, which are peripheral-specific.
 *
 * @param hal the serial device over which the data is transmitted and
 * received
 *
 * @param tx_data the data to be transmitted
 *
 * @param tx_len the number of bytes to transmit.  A transaction
 * writing more than 255 bytes may be rejected.
 *
 * @return the total number of bytes transmitted, or -1 if an error
 * occcured.
 */
static __inline__
int iBSP430i2cTxData_ni (hBSP430halSERIAL hal,
                         const uint8_t * tx_data,
                         size_t tx_len)
{
  return hal->dispatch->i2cTxData_ni(hal, tx_data, tx_len);
}

/** Receive using an I2C-configured device
 *
 * This routine receives @a rx_len octets into @a rx_data, storing the
 * octets received in response into @a rx_data.  It will return an
 * error if the device is configured with a receive callback.
 *
 * This routine should only be invoked when @link
 * sBSP430halSERIAL.tx_callback @a hal->tx_callback @endlink is null.
 * If a callback is present, it is expected to be used to provide data
 * for transmission.  Note that such a callback must handle I2C start
 * and stop conditions, which are peripheral-specific.
 *
 * @param hal the serial device from which the data is received
 *
 * @param rx_data where to store the data.  The space available must
 * be at least @a rx_len octets.
 *
 * @param rx_len the number of bytes expected in response.  A
 * transaction reading more than 255 bytes may be rejected.
 *
 * @return the total number of bytes stored in @a rx_data, or -1 if an
 * error occcured.
 */
static __inline__
int iBSP430i2cRxData_ni (hBSP430halSERIAL hal,
                         uint8_t * rx_data,
                         size_t rx_len)
{
  return hal->dispatch->i2cRxData_ni(hal, rx_data, rx_len);
}
#endif /* configBSP430_SERIAL_ENABLE_I2C */

/** Place a serial device in hold mode
 *
 * When placed in hold mode, the #UCSWRST bit (or peripheral-specific
 * analog) is set, clearing all errors and stopping all activity but
 * retaining all configuration information.  In addition, the function
 * reconfigures the associated port pins to their digital I/O
 * function.  When the hold is released, the port pins are
 * reconfigured to their peripheral function, the #UCSWRST bit is
 * cleared, and any interrupts for which callbacks are registered are
 * re-enabled.  (Note that enabling the interrupts at the peripheral
 * level does not change the #GIE state, which should be cleared while
 * this function is executing.)
 *
 * @note Placing a serial peripheral into hold mode prior to entering
 * a low power mode will often reduce current consumption.
 *
 * @param hal a serial HAL handle to a peripheral that has been
 * opened
 *
 * @param holdp a nonzero value if the peripheral is to be placed into
 * hold mode, and a zero value to release it from hold mode
 *
 * @return 0 if the transition was successful, -1 if an error
 * occurred.  Potential errors include inability to configure the
 * peripheral pins.  On error, the peripheral is left in reset mode.
 */
static __inline__
int iBSP430serialSetHold_ni (hBSP430halSERIAL hal,
                             int holdp)
{
  return hal->dispatch->setHold_ni(hal, holdp);
}

/** Release a serial device.
 *
 * This places the device into reset mode and resets the peripheral
 * pins to port function.  It does not release or disassociate any
 * callbacks.
 *
 * @param hal a serial HAL handle previously obtained through
 * hBSP430serialOpenUART(), hBSP430serialOpenSPI(), or
 * hBSP430serialOpenI2C().
 *
 * @return 0 if the close occurred without error.
 *
 * @delegated This function exists only as an inline delegate to a
 * peripheral-specific implementation. */
static __inline__
int iBSP430serialClose (hBSP430halSERIAL hal)
{
  return hal->dispatch->close(hal);
}

/** Wake up the interrupt-driven transmission if necessary.
 *
 * Normally the transmission infrastructure transmits data as soon as
 * space is available in the transmission buffer.  The infrastructure
 * is disabled when the sBSP430halSERIAL.tx_callback indicates that no
 * more data is available.  When this has happened, it must be told
 * that more data has been added and the infrastructure re-enabled.
 *
 * For efficiency, this should only be called if it is believed that
 * data is ready to be presented in a transmission callback, but that
 * the transmission infrastructure may be idle.
 *
 * @param hal a serial HAL handle
 *
 * @delegated This function exists only as an inline delegate to a
 * peripheral-specific implementation. */
static __inline__
void vBSP430serialWakeupTransmit_ni (hBSP430halSERIAL hal)
{
  return hal->dispatch->wakeupTransmit_ni(hal);
}

/** Spin until any in-progress transmission or reception is complete.
 *
 * This is used to ensure the device is idle prior to reconfiguring it.
 *
 * @param hal a serial HAL handle
 *
 * @delegated This function exists only as an inline delegate to a
 * peripheral-specific implementation. */
static __inline__
void vBSP430serialFlush_ni (hBSP430halSERIAL hal)
{
  return hal->dispatch->flush_ni(hal);
}

#endif /* BSP430_SERIAL - 0 */

#if configBSP430_SERIAL_USE_USCI - 0
#include <bsp430/periph/usci.h>
#endif /* configBSP430_SERIAL_USE_USCI */
#if configBSP430_SERIAL_USE_USCI5 - 0
#include <bsp430/periph/usci5.h>
#endif /* configBSP430_SERIAL_USE_USCI5 */
#if configBSP430_SERIAL_USE_EUSCI - 0
#include <bsp430/periph/eusci.h>
#endif /* configBSP430_SERIAL_USE_EUSCI */

/** Get the HAL handle for a specific serial peripheral.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_USCI_A0.
 *
 * @return the HAL handle for the peripheral.  A null pointer is
 * returned if the handle does not correspond to a serial peripheral
 * for which the HAL interface has been enabled.
 */
hBSP430halSERIAL hBSP430serialLookup (tBSP430periphHandle periph);

/** Get a human-readable identifier for the serial peripheral
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_USCI_A0.
 *
 * @return The short name of the port, e.g. "USCI_A0".  If the peripheral
 * is not recognized as a serial device, a null pointer is returned. */
const char * xBSP430serialName (tBSP430periphHandle periph);


#endif /* BSP430_SERIAL_H */
