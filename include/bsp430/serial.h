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

#define _DELEGATE_PUT_BYTE_NI(_serial)                          \
  static __inline__                                             \
  int                                                           \
  iBSP430serialPutByte_ni (int c, xBSP430serialHandle device)   \
  {                                                             \
    return iBSP430##_serial##PutByte_ni(c, device);             \
  }

#define _DELEGATE_PUT_ASCIIZ_NI(_serial)                                \
  static __inline__                                                     \
  int                                                                   \
  iBSP430serialPutASCIIZ_ni (const char * str, xBSP430serialHandle device) \
  {                                                                     \
    return iBSP430##_serial##PutASCIIZ_ni(str, device);                 \
  }

#define _DELEGATE(_serial)                      \
  _DELEGATE_HANDLE(_serial)                     \
  _DELEGATE_OPEN(_serial)                       \
  _DELEGATE_CONFIGURE_CALLBACKS(_serial)        \
  _DELEGATE_CLOSE(_serial)                      \
  _DELEGATE_WAKEUP_TRANSMIT_NI(_serial)         \
  _DELEGATE_FLUSH_NI(_serial)                   \
  _DELEGATE_PUT_BYTE_NI(_serial)                \
  _DELEGATE_PUT_ASCIIZ_NI(_serial)

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
#undef _DELEGATE_PUT_ASCIIZ_NI
#undef _DELEGATE_PUT_BYTE_NI
#undef _DELEGATE_FLUSH_NI
#undef _DELEGATE_WAKEUP_TRANSMIT_NI
#undef _DELEGATE_CLOSE
#undef _DELEGATE_CONFIGURE_CALLBACKS
#undef _DELEGATE_OPEN
#undef _DELEGATE_HANDLE

/** @endcond */

#endif /* BSP430_SERIAL_H */
