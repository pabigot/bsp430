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
 * @brief Abstracted UART interface.
 *
 * Selects from supported devices and provides an adaptation layer so
 * that other modules can be ignorant of the specific underlying UART
 * device type.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UART_H
#define BSP430_UART_H

#include <bsp430/common.h>

/** @def configBSP430_UART_EXPOSE_STATE
 *
 * Define to a true value to include the protected header for the
 * selected UART, rather than the public header.
 *
 * You want to do this if you intend to implement interrupt callbacks
 * that need access to the underlying peripheral or other HAL state.
 *
 * @defaulted */
#ifndef configBSP430_UART_EXPOSE_STATE
#define configBSP430_UART_EXPOSE_STATE 0
#endif /* configBSP430_UART_EXPOSE_STATE */

/** @def configBSP430_UART_USE_USCI
 *
 * Define to true value to select #xBSP430usciHandle as the underlying
 * UART implementation.  This defaults to true iff the MCU supports
 * the USCI device.
 *
 * @defaulted  */
#ifndef configBSP430_UART_USE_USCI
#define configBSP430_UART_USE_USCI defined(__MSP430_HAS_USCI__)
#endif /* configBSP430_UART_USE_USCI */

/** @def configBSP430_UART_USE_USCI5
 *
 * Define to true value to select #xBSP430usci5Handle as the underlying
 * UART implementation.  This defaults to true iff the MCU supports
 * the USCI5 device.
 *
 * @defaulted  */
#ifndef configBSP430_UART_USE_USCI5
#define configBSP430_UART_USE_USCI5 defined(__MSP430_HAS_USCI_A0__)
#endif /* configBSP430_UART_USE_USCI5 */

/** @def configBSP430_UART_USE_EUSCIA
 *
 * Define to true value to select #xBSP430eusciaHandle as the underlying
 * UART implementation.  This defaults to true iff the MCU supports
 * the eUSCI device.
 *
 * @defaulted  */
#ifndef configBSP430_UART_USE_EUSCIA
#define configBSP430_UART_USE_EUSCIA defined(__MSP430_HAS_EUSCI_A0__)
#endif /* configBSP430_UART_USE_EUSCIA */

/** @cond DOXYGEN_EXCLUDE */

#define _DELEGATE_HANDLE(_uart)                         \
  typedef xBSP430##_uart##Handle xBSP430uartHandle;

#define _DELEGATE_OPEN(_uart)                                           \
  static __inline__                                                     \
  xBSP430uartHandle                                                     \
  xBSP430uartOpen (xBSP430periphHandle xPeriph,                         \
                   unsigned int control_word,                           \
                   unsigned long baud)                                  \
  {                                                                     \
    return xBSP430##_uart##OpenUART(xPeriph, control_word, baud);       \
  }

#define _DELEGATE_CONFIGURE_CALLBACKS(_uart)                            \
  static __inline__                                                     \
  int                                                                   \
  iBSP430uartConfigureCallbacks (xBSP430uartHandle uart,                \
                                 const struct xBSP430periphISRCallbackVoid * rx_callback, \
                                 const struct xBSP430periphISRCallbackVoid * tx_callback) \
  {                                                                     \
    return iBSP430##_uart##ConfigureCallbacks(uart, rx_callback, tx_callback); \
  }

#define _DELEGATE_CLOSE(_uart)                  \
  static __inline__                             \
  int                                           \
  iBSP430uartClose (xBSP430uartHandle xUART)    \
  {                                             \
    return iBSP430##_uart##Close(xUART);        \
  }

#define _DELEGATE_WAKEUP_TRANSMIT_NI(_uart)                     \
  static __inline__                                             \
  void                                                          \
  vBSP430uartWakeupTransmit_ni (xBSP430uartHandle device)       \
  {                                                             \
    vBSP430##_uart##WakeupTransmit_ni(device);                  \
  }

#define _DELEGATE_FLUSH_NI(_uart)                       \
  static __inline__                                     \
  void                                                  \
  vBSP430uartFlush_ni (xBSP430uartHandle device)        \
  {                                                     \
    vBSP430##_uart##Flush_ni(device);                   \
  }

#define _DELEGATE_PUT_BYTE_NI(_uart)                            \
  static __inline__                                             \
  int                                                           \
  iBSP430uartPutByte_ni (int c, xBSP430uartHandle device)       \
  {                                                             \
    return iBSP430##_uart##PutByte_ni(c, device);               \
  }

#define _DELEGATE_PUT_ASCIIZ_NI(_uart)                                  \
  static __inline__                                                     \
  int                                                                   \
  iBSP430uartPutASCIIZ_ni (const char * str, xBSP430uartHandle device)  \
  {                                                                     \
    return iBSP430##_uart##PutASCIIZ_ni(str, device);                   \
  }

#define _DELEGATE(_uart) \
  _DELEGATE_HANDLE(_uart)                       \
  _DELEGATE_OPEN(_uart)                         \
  _DELEGATE_CONFIGURE_CALLBACKS(_uart)          \
  _DELEGATE_CLOSE(_uart)                        \
  _DELEGATE_WAKEUP_TRANSMIT_NI(_uart)           \
  _DELEGATE_FLUSH_NI(_uart)                     \
  _DELEGATE_PUT_BYTE_NI(_uart)                  \
  _DELEGATE_PUT_ASCIIZ_NI(_uart)

#if configBSP430_UART_USE_USCI - 0
#if configBSP430_UART_EXPOSE_STATE - 0
#include <bsp430/periph/usci_.h>
#else /* configBSP430_UART_EXPOSE_STATE */
#include <bsp430/periph/usci.h>
#endif /* configBSP430_UART_EXPOSE_STATE */

_DELEGATE(usci)
#endif /* configBSP430_UART_USE_USCI */

#if configBSP430_UART_USE_USCI5 - 0

#if configBSP430_UART_EXPOSE_STATE - 0
#include <bsp430/periph/usci5_.h>
#else /* configBSP430_UART_EXPOSE_STATE */
#include <bsp430/periph/usci5.h>
#endif /* configBSP430_UART_EXPOSE_STATE */

_DELEGATE(usci5)

#endif /* configBSP430_UART_USE_USCI5 */

#if configBSP430_UART_USE_EUSCIA - 0

#if configBSP430_UART_EXPOSE_STATE - 0
#include <bsp430/periph/euscia_.h>
#else /* configBSP430_UART_EXPOSE_STATE */
#include <bsp430/periph/euscia.h>
#endif /* configBSP430_UART_EXPOSE_STATE */

_DELEGATE(euscia)

#endif /* configBSP430_UART_USE_EUSCI */

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

#endif /* BSP430_UART_H */
