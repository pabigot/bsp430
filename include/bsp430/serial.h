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
#define BSP430_SERIAL_HAL_HPL_VARIANT_USCI5 1

/** Field value for variant stored in
 * #sBSP430halSERIAL.hal_state.cflags when HPL reference is to an
 * #sBSP430hplEUSCIA. */
#define BSP430_SERIAL_HAL_HPL_VARIANT_EUSCIA 1

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

/** @def configBSP430_SERIAL_USE_EUSCIA
 *
 * Define to true value to allow the generic serial dispatches to
 * recognize #sBSP430hplEUSCIA as an underlying serial implementation.
 * This defaults to true iff the MCU supports the eUSCI device as
 * implemented on 5xx MCUs.
 *
 * @cppflag
 * @defaulted  */
#ifndef configBSP430_SERIAL_USE_EUSCIA
#define configBSP430_SERIAL_USE_EUSCIA defined(__MSP430_HAS_EUSCI_A0__)
#endif /* configBSP430_SERIAL_USE_EUSCIA */

/** @def configBSP430_SERIAL_USE_EUSCIB
 *
 * Define to true value to allow the generic serial dispatches to
 * recognize #sBSP430hplEUSCIB as an underlying serial implementation.
 * This defaults to true iff the MCU supports the eUSCI device as
 * implemented on 5xx MCUs.
 *
 * @cppflag
 * @defaulted  */
#ifndef configBSP430_SERIAL_USE_EUSCIB
#define configBSP430_SERIAL_USE_EUSCIB defined(__MSP430_HAS_EUSCI_B0__)
#endif /* configBSP430_SERIAL_USE_EUSCIB */

/* !BSP430! instance=usci,usci5,euscia */
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

/* END AUTOMATICALLY GENERATED CODE [hal_variant_hpl_macro] */
/* !BSP430! end=hal_variant_hpl_macro */

/* Forward declarations */
struct sBSP430hplUSCI;
struct sBSP430usciHPLAux;
struct sBSP430hplUSCI5;
struct sBSP430hplEUSCIA;
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
    void * any;

    /** Access to the HPL pointer as a 2xx/4xx USCI peripheral */
    volatile struct sBSP430hplUSCI * usci;

    /** Access to the HPL pointer as a 5xx/6xx USCI peripheral */
    volatile struct sBSP430hplUSCI5 * usci5;

    /** Access to the HPL pointer as a 5xx/6xx eUSCI type A peripheral */
    volatile struct sBSP430hplEUSCIA * euscia;
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

  /** Location to store a single incoming character when #rx_queue
   * is undefined. */
  uint8_t rx_byte;

  /** Location to store a single outgoing character when #tx_queue
   * is undefined.  This is probably not very useful. */
  uint8_t tx_byte;

  /** The callback chain to invoke when a byte is received */
  const struct sBSP430halISRCallbackVoid * rx_callback;

  /** The callback chain to invoke when space is available in the
   * transmission buffer */
  const struct sBSP430halISRCallbackVoid * tx_callback;

  /** Total number of received octets */
  unsigned long num_rx;

  /** Total number of transmitted octets */
  unsigned long num_tx;

  /** @cond DOXYGEN_EXCLUDE */
  const struct sBSP430serialDispatch * const dispatch;
  /** @endcond */
} sBSP430halSERIAL;

/** Handle for a serial HAL instance */
typedef struct sBSP430halSERIAL * hBSP430halSERIAL;

/** @cond DOXYGEN_EXCLUDE */
struct sBSP430serialDispatch {
  hBSP430halSERIAL (* openUART) (hBSP430halSERIAL hal,
                                 unsigned int control_word,
                                 unsigned long baud);
  hBSP430halSERIAL (* openSPI) (hBSP430halSERIAL hal,
                                unsigned int control_word,
                                unsigned int prescaler);
  int (* configureCallbacks) (hBSP430halSERIAL hal,
                              const struct sBSP430halISRCallbackVoid * rx_callback,
                              const struct sBSP430halISRCallbackVoid * tx_callback);
  int (* close) (hBSP430halSERIAL hal);
  void (* wakeupTransmit_ni) (hBSP430halSERIAL hal);
  void (* flush_ni) (hBSP430halSERIAL hal);
  int (* transmitByte_ni) (hBSP430halSERIAL hal, int c);
  int (* transmitData_ni) (hBSP430halSERIAL hal,
                           const uint8_t * data,
                           size_t len);
  int (* transmitASCIIZ_ni) (hBSP430halSERIAL hal, const char * str);
};
/** @endcond */

/** Request and configure a serial device in UART mode.
 *
 * If callbacks had been associated with this device using
 * #iBSP430serialConfigureCallbacks(), behavior with respect to
 * interrupts is as if those callbacks were associated during this
 * call.
 *
 * @param periph The peripheral device identifier for the USCI device
 * that is being requested. E.g., #BSP430_PERIPH_USCI_A0.
 *
 * @param control_word The configuration to be written to the device's
 * ctlw0 word.  If the device does not support a ctlw0 register, the
 * lower byte is written to ctl0 and the upper byte to ctl1.  Most bit
 * fields will be assigned from this value, but UCSYNC will be
 * cleared, and UCSSELx will be set based on baud rate.
 *
 * @note In almost all cases, a value of zero is appropriate.  If you
 * need to configure parity and other control word features, be aware
 * that #sBSP430hplUSCI5 and #sBSP430hplEUSCIA have the ctl0 and
 * ctl1 fields swapped relative to their order in #sBSP430hplUSCI.
 * Since these peripherals have a ctlw0 field, that will be used,
 * resulting in the lower word being written to ctl1 and the upper
 * word to ctl0.  This can result in confusion, since constants like
 * @c UCSYNC are relative to ctl0 for the USCI and USCI5 variants, but
 * are defined for the ctlw0 position in eUSCI.  Take this up with
 * Texas Instruments; they defined the register map and the header
 * constants.
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
 * peripheral-specific implementation. */
static __inline__
hBSP430halSERIAL xBSP430serialOpenUART (hBSP430halSERIAL hal,
                                        unsigned int control_word,
                                        unsigned long baud)
{
  return hal->dispatch->openUART(hal, control_word, baud);
}

hBSP430halSERIAL xBSP430serialOpenSPI (hBSP430halSERIAL hal,
                                       unsigned int control_word,
                                       unsigned int prescaler);

/** Assign callbacks for transmission and reception.
 *
 * Callbacks are assigned within a critical section.  The code waits
 * until the device becomes idle, then places it in reset mode.  After
 * configuration, the receive interrupt is enabled if a receive
 * callback is registered.  The critical section restores
 * interruptibility state to its entry state on exit.
 *
 * @param hal a serial HAL handle.  If (non-null) callbacks are
 * already associated with this device, an error will be returned and
 * the previous configuration left unchanged.
 *
 * @param rx_callback a pointer to the head of a callback chain to be
 * used for receiving.  A non-null value enables interrupt-driven
 * reception, and data will be provided to the callbacks on
 * receiption.
 *
 * @param tx_callback a pointer to the head of a callback chain to be
 * used for transmission.  A non-null value enables interrupt-driven
 * transmission, and the chain will be invoked as necessary.  The
 * infrastructure must be made aware of data to be transmitted, so it
 * can enable the interrupt that will request the data through the
 * callback.  This is done using vBSP430serialWakeupTransmit_ni().
 * The callback function should return
 * #BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN if data to transmit is
 * available, and #BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT if it
 * is known that there will not be data available after the
 * transmission.
 *
 * @return zero if the configuration was successful, a negative value
 * if something went wrong.
 *
 * @note #iBSP430serialClose() does not disassociate the callbacks
 * from the device.  If the disassociation is desired, it can be done
 * manually either before or after closing the device by invoking this
 * function with null callback pointers.
 *
 * @delegated This function exists only as an inline delegate to a
 * peripheral-specific implementation.
 */
static __inline__
int iBSP430serialConfigureCallbacks (hBSP430halSERIAL hal,
                                     const struct sBSP430halISRCallbackVoid * rx_callback,
                                     const struct sBSP430halISRCallbackVoid * tx_callback)
{
  return hal->dispatch->configureCallbacks(hal, rx_callback, tx_callback);
}

/** Release a serial device.
 *
 * This places the device into reset mode and resets the peripheral
 * pins to port function.  It does not release or disassociate any
 * queues that were provided through #iBSP430usciConfigureQueues.
 *
 * @param hal a serial HAL handle previously obtained through
 * xBSP430serialOpenUART() or xBSP430serialOpenSPI()
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
 * is disabled when the transmit queue is emptied.  When this has
 * happened, it must be told that more data has been added and the
 * infrastructure re-enabled.
 *
 * For efficiency, this should only be called if it is believed that
 * data is present in the transmit queue but that the transmission
 * infrastructure may be idle.
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

/** Transmit a byte over the device.
 *
 * This routine should only be invoked when there is no transmit
 * callback registered.  If a callback is present, it is expected to
 * be used to provide data for transmission.
 *
 * @param c a data byte to be transmitted
 *
 * @param hal the serial device over which the data should be
 * transmitted
 *
 * @return the input character @a c if transmitted, or -1 if an error
 * occurred
 *
 * @delegated This function exists only as an inline delegate to a
 * peripheral-specific implementation. */
static __inline__
int iBSP430serialTransmitByte_ni (hBSP430halSERIAL hal, int c)
{
  return hal->dispatch->transmitByte_ni(hal, c);
}

/** Transmit a block of data over the device.
 *
 * This routine should only be invoked when there is no transmit
 * callback registered.  If a callback is present, it is expected to
 * be used to provide data for transmission.
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
int iBSP430serialTransmitData_ni (hBSP430halSERIAL hal,
                                  const uint8_t * data,
                                  size_t len)
{
  return hal->dispatch->transmitData_ni(hal, data, len);
}


/** Transmit a sequence of characters over the device.
 *
 * This routine should only be invoked when there is no transmit
 * callback registered.  If a callback is present, it is expected to
 * be used to provide data for transmission.
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
int iBSP430serialTransmitASCIIZ_ni (hBSP430halSERIAL hal, const char * str)
{
  return hal->dispatch->transmitASCIIZ_ni(hal, str);
}

#if configBSP430_SERIAL_USE_USCI - 0
#include <bsp430/periph/usci.h>
#endif /* configBSP430_SERIAL_USE_USCI */
#if configBSP430_SERIAL_USE_USCI5 - 0
#include <bsp430/periph/usci5.h>
#endif /* configBSP430_SERIAL_USE_USCI5 */
#if configBSP430_SERIAL_USE_EUSCIA - 0
#include <bsp430/periph/euscia.h>
#endif /* configBSP430_SERIAL_USE_EUSCIA */

/** Get the HAL handle for a specific serial peripheral.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_USCI_A0.
 *
 * @return the HAL handle for the peripheral.  A null pointer is
 * returned if the handle does not correspond to a serial peripheral
 * for which the HAL interface has been enabled.
 */
hBSP430halSERIAL xBSP430halLookupSERIAL (tBSP430periphHandle periph);

/** Get a human-readable identifier for the serial peripheral
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_USCI_A0.
 *
 * @return The short name of the port, e.g. "USCI_A0".  If the peripheral
 * is not recognized as a serial device, a null pointer is returned. */
const char * xBSP430serialName (tBSP430periphHandle periph);

#endif /* BSP430_SERIAL_H */
