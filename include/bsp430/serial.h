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
 * the peripheral available on the current platform.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_SERIAL_H
#define BSP430_SERIAL_H

#include <bsp430/common.h>

/** @def configBSP430_SERIAL_EXPOSE_STATE
 *
 * Define to a true value to include the protected header for the
 * selected serial peripheral, rather than the public header.
 *
 * You want to do this if you intend to implement interrupt callbacks
 * that need access to the underlying peripheral or other HAL state.
 *
 * @defaulted */
#ifndef configBSP430_SERIAL_EXPOSE_STATE
#define configBSP430_SERIAL_EXPOSE_STATE 0
#endif /* configBSP430_SERIAL_EXPOSE_STATE */

/** @def configBSP430_SERIAL_USE_USCI
 *
 * Define to true value to select #xBSP430usciHandle as the underlying
 * serial implementation.  This defaults to true iff the MCU supports
 * the USCI device.
 *
 * @defaulted  */
#ifndef configBSP430_SERIAL_USE_USCI
#define configBSP430_SERIAL_USE_USCI defined(__MSP430_HAS_USCI__)
#endif /* configBSP430_SERIAL_USE_USCI */

/** @def configBSP430_SERIAL_USE_USCI5
 *
 * Define to true value to select #xBSP430usci5Handle as the
 * underlying serial implementation.  This defaults to true iff the
 * MCU supports the USCI5 device.
 *
 * @defaulted  */
#ifndef configBSP430_SERIAL_USE_USCI5
#define configBSP430_SERIAL_USE_USCI5 defined(__MSP430_HAS_USCI_A0__)
#endif /* configBSP430_SERIAL_USE_USCI5 */

/** @def configBSP430_SERIAL_USE_EUSCIA
 *
 * Define to true value to select #xBSP430eusciaHandle as the
 * underlying serial implementation.  This defaults to true iff the
 * MCU supports the eUSCI device.
 *
 * @defaulted  */
#ifndef configBSP430_SERIAL_USE_EUSCIA
#define configBSP430_SERIAL_USE_EUSCIA defined(__MSP430_HAS_EUSCI_A0__)
#endif /* configBSP430_SERIAL_USE_EUSCIA */

#if defined(BSP430_DOXYGEN)
/** An alias for the peripheral-specific HAL handle that supports the
 * serial HAL abstraction on this platform.
 *
 * @delegated This typedef selects #xBSP430usciHandle,
 * #xBSP430usci5Handle, #xBSP430eusciaHandle, or another
 * implementation based on peripheral availability. */
typedef xBSP430serialHandle xBSP430serialHandle;
#endif

/** Request and configure a serial device in UART mode.
 *
 * If callbacks had been associated with this device using
 * #iBSP430serialConfigureCallbacks(), behavior with respect to
 * interrupts is as if those callbacks were associated during this
 * call.
 *
 * @param periph The peripheral HPL device identifier for the USCI
 * device that is being requested. E.g., BSP430_PERIPH_USCI_A0.
 *
 * @param control_word The configuration to be written to the device's
 * ctlw0 word.  If the device does not support a ctlw0 register, the
 * lower byte is written to ctl0 and the upper byte to ctl1.  Most bit
 * fields will be assigned from this value, but UCSYNC will be
 * cleared, and UCSSELx will be set based on baud rate.
 *
 * @note In almost all cases, a value of zero is appropriate.  If you
 * need to configure parity and other control word features, be aware
 * that #xBSP430periphUSCI5 and #xBSP430periphEUSCIA have the ctl0 and
 * ctl1 fields swapped relative to their order in #xBSP430periphUSCI.
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
#if defined(BSP430_DOXYGEN)
xBSP430serialHandle xBSP430serialOpenUART (xBSP430periphHandle periph,
    unsigned int control_word,
    unsigned long baud);
#endif /* BSP430_DOXYGEN */

#if defined(BSP430_DOXYGEN)
xBSP430serialHandle xBSP430serialOpenSPI (xBSP430periphHandle xPeriph,
    unsigned int control_word,
    unsigned int prescaler);
#endif /* BSP430_DOXYGEN */

/** Assign callbacks for transmission and reception.
 *
 * Callbacks are assigned within a critical section.  The code waits
 * until the device becomes idle, then places it in reset mode.  After
 * configuration, the receive interrupt is enabled if a receive
 * callback is registered.  The critical section restores
 * interruptibility state to its entry state on exit.
 *
 * @param device a serial HAL handle.  If (non-null) callbacks are
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
 * #BSP430_PERIPH_ISR_CALLBACK_BREAK_CHAIN if data to transmit is
 * available, and #BSP430_PERIPH_ISR_CALLBACK_DISABLE_INTERRUPT if it
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
#if defined(BSP430_DOXYGEN)
int iBSP430serialConfigureCallbacks (xBSP430serialHandle device,
                                     const struct xBSP430periphISRCallbackVoid * rx_callback,
                                     const struct xBSP430periphISRCallbackVoid * tx_callback);
#endif /* BSP430_DOXYGEN */

/** Release a serial device.
 *
 * This places the device into reset mode and resets the peripheral
 * pins to port function.  It does not release or disassociate any
 * queues that were provided through #iBSP430usciConfigureQueues.
 *
 * @param device a serial HAL handle previously obtained through
 * xBSP430serialOpenUART() or xBSP430serialOpenSPI()
 *
 * @return 0 if the close occurred without error.
 *
 * @delegated This function exists only as an inline delegate to a
 * peripheral-specific implementation. */
#if defined(BSP430_DOXYGEN)
int iBSP430serialClose (xBSP430serialHandle device);
#endif /* BSP430_DOXYGEN */

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
 * @param device a serial HAL handle
 *
 * @delegated This function exists only as an inline delegate to a
 * peripheral-specific implementation. */
#if defined(BSP430_DOXYGEN)
void vBSP430serialWakeupTransmit_ni (xBSP430serialHandle device);
#endif /* BSP430_DOXYGEN */

/** Spin until any in-progress transmission or reception is complete.
 *
 * This is used to ensure the device is idle prior to reconfiguring it.
 *
 * @param device a serial HAL handle
 *
 * @delegated This function exists only as an inline delegate to a
 * peripheral-specific implementation. */
#if defined(BSP430_DOXYGEN)
void vBSP430serialFlush_ni (xBSP430serialHandle device);
#endif /* BSP430_DOXYGEN */

/** Transmit a byte over the device.
 *
 * This routine should only be invoked when there is no transmit
 * callback registered.  If a callback is present, it is expected to
 * be used to provide data for transmission.
 *
 * @param c a data byte to be transmitted
 *
 * @param device the serial device over which the data should be
 * transmitted
 *
 * @return the input character @a c if transmitted, or -1 if an error
 * occurred
 *
 * @delegated This function exists only as an inline delegate to a
 * peripheral-specific implementation. */
#if defined(BSP430_DOXYGEN)
int iBSP430serialTransmitByte_ni (xBSP430serialHandle device, int c);
#endif /* BSP430_DOXYGEN */

/** Transmit a block of data over the device.
 *
 * This routine should only be invoked when there is no transmit
 * callback registered.  If a callback is present, it is expected to
 * be used to provide data for transmission.
 *
 * @param device the serial device over which the data should be
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
#if defined(BSP430_DOXYGEN)
int iBSP430serialTransmitData_ni (xBSP430serialHandle device,
                                  const uint8_t * data,
                                  size_t len);
#endif /* BSP430_DOXYGEN */

/** Transmit a sequence of characters over the device.
 *
 * This routine should only be invoked when there is no transmit
 * callback registered.  If a callback is present, it is expected to
 * be used to provide data for transmission.
 *
 * @param device the serial device over which the data should be
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
#if defined(BSP430_DOXYGEN)
int iBSP430serialTransmitASCIIZ_ni (xBSP430serialHandle device, const char * str);
#endif /* BSP430_DOXYGEN */

/** @cond DOXYGEN_EXCLUDE */

#define _DELEGATE_HANDLE(_serial)                       \
  typedef xBSP430##_serial##Handle xBSP430serialHandle;

#define _DELEGATE_OPEN(_serial)                                         \
  static __inline__                                                     \
  xBSP430serialHandle                                                   \
  xBSP430serialOpen (xBSP430periphHandle xPeriph,                       \
                     unsigned int control_word,                         \
                     unsigned long baud)                                \
  {                                                                     \
    return xBSP430##_serial##OpenUART(xPeriph, control_word, baud);     \
  }

#define _DELEGATE_CONFIGURE_CALLBACKS(_serial)                          \
  static __inline__                                                     \
  int                                                                   \
  iBSP430serialConfigureCallbacks (xBSP430serialHandle device,          \
                                   const struct xBSP430periphISRCallbackVoid * rx_callback, \
                                   const struct xBSP430periphISRCallbackVoid * tx_callback) \
  {                                                                     \
    return iBSP430##_serial##ConfigureCallbacks(device, rx_callback, tx_callback); \
  }

#define _DELEGATE_CLOSE(_serial)                        \
  static __inline__                                     \
  int                                                   \
  iBSP430serialClose (xBSP430serialHandle device)       \
  {                                                     \
    return iBSP430##_serial##Close(device);             \
  }

#define _DELEGATE_WAKEUP_TRANSMIT_NI(_serial)                   \
  static __inline__                                             \
  void                                                          \
  vBSP430serialWakeupTransmit_ni (xBSP430serialHandle device)   \
  {                                                             \
    vBSP430##_serial##WakeupTransmit_ni(device);                \
  }

#define _DELEGATE_FLUSH_NI(_serial)                     \
  static __inline__                                     \
  void                                                  \
  vBSP430serialFlush_ni (xBSP430serialHandle device)    \
  {                                                     \
    vBSP430##_serial##Flush_ni(device);                 \
  }

#define _DELEGATE_TRANSMIT_BYTE_NI(_serial)                             \
  static __inline__                                                     \
  int                                                                   \
  iBSP430serialTransmitByte_ni (xBSP430serialHandle device, int c)      \
  {                                                                     \
    return iBSP430##_serial##TransmitByte_ni(device, c);                \
  }

#define _DELEGATE_TRANSMIT_DATA_NI(_serial)                             \
  static __inline__                                                     \
  int                                                                   \
  iBSP430serialTransmitData_ni (xBSP430serialHandle device,             \
                                const uint8_t * data,                   \
                                size_t len)                             \
  {                                                                     \
    return iBSP430##_serial##TransmitData_ni(device, data, len);        \
  }

#define _DELEGATE_TRANSMIT_ASCIIZ_NI(_serial)                           \
  static __inline__                                                     \
  int                                                                   \
  iBSP430serialTransmitASCIIZ_ni (xBSP430serialHandle device, const char * str) \
  {                                                                     \
    return iBSP430##_serial##TransmitASCIIZ_ni(device, str);            \
  }

#define _DELEGATE(_serial)                      \
  _DELEGATE_HANDLE(_serial)                     \
  _DELEGATE_OPEN(_serial)                       \
  _DELEGATE_CONFIGURE_CALLBACKS(_serial)        \
  _DELEGATE_CLOSE(_serial)                      \
  _DELEGATE_WAKEUP_TRANSMIT_NI(_serial)         \
  _DELEGATE_FLUSH_NI(_serial)                   \
  _DELEGATE_TRANSMIT_BYTE_NI(_serial)           \
  _DELEGATE_TRANSMIT_DATA_NI(_serial)           \
  _DELEGATE_TRANSMIT_ASCIIZ_NI(_serial)

#if configBSP430_SERIAL_USE_USCI - 0
#if configBSP430_SERIAL_EXPOSE_STATE - 0
#include <bsp430/periph/usci_.h>
#else /* configBSP430_SERIAL_EXPOSE_STATE */
#include <bsp430/periph/usci.h>
#endif /* configBSP430_SERIAL_EXPOSE_STATE */

_DELEGATE(usci)
#endif /* configBSP430_SERIAL_USE_USCI */

#if configBSP430_SERIAL_USE_USCI5 - 0

#if configBSP430_SERIAL_EXPOSE_STATE - 0
#include <bsp430/periph/usci5_.h>
#else /* configBSP430_SERIAL_EXPOSE_STATE */
#include <bsp430/periph/usci5.h>
#endif /* configBSP430_SERIAL_EXPOSE_STATE */

_DELEGATE(usci5)

#endif /* configBSP430_SERIAL_USE_USCI5 */

#if configBSP430_SERIAL_USE_EUSCIA - 0

#if configBSP430_SERIAL_EXPOSE_STATE - 0
#include <bsp430/periph/euscia_.h>
#else /* configBSP430_SERIAL_EXPOSE_STATE */
#include <bsp430/periph/euscia.h>
#endif /* configBSP430_SERIAL_EXPOSE_STATE */

_DELEGATE(euscia)

#endif /* configBSP430_SERIAL_USE_EUSCI */

#undef _DELEGATE
#undef _DELEGATE_TRANSMIT_ASCIIZ_NI
#undef _DELEGATE_TRANSMIT_DATA_NI
#undef _DELEGATE_TRANSMIT_BYTE_NI
#undef _DELEGATE_FLUSH_NI
#undef _DELEGATE_WAKEUP_TRANSMIT_NI
#undef _DELEGATE_CLOSE
#undef _DELEGATE_CONFIGURE_CALLBACKS
#undef _DELEGATE_OPEN
#undef _DELEGATE_HANDLE

/** @endcond */

#endif /* BSP430_SERIAL_H */
