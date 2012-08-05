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
 * @brief Hardware presentation/abstraction for eUSCI_A.
 *
 * Genericized eUCSI_A on 5xx/6xx devices
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

/* !BSP430! periph=euscia */
/* !BSP430! instance=EUSCI_A0,EUSCI_A1,EUSCI_A2 */

#ifndef BSP430_PERIPH_EUSCIA_H
#define BSP430_PERIPH_EUSCIA_H

#include <bsp430/periph.h>
#include <bsp430/serial.h>

#if ! defined(__MSP430_HAS_EUSCI_A0__)
#warning Peripheral not supported by configured MCU
#endif /* __MSP430_HAS_EUSCI_A0__ */

/** Register map for eUSCI_A peripheral hardware presentation layer. */
typedef struct sBSP430hplEUSCIA {
  union {						/* 0x00 */
    unsigned int ctlw0;		/**< UCAxCTLW0 */
    struct {
      unsigned char ctl1; /**< UCAxCTL1 */
      unsigned char ctl0; /**< UCAxCTL0 */
    };
  };
  unsigned int ctlw1;			/**< UCAxCTLW1 */ /* 0x02 */
  unsigned int _unused_0x04;
  union {						/* 0x06 */
    unsigned int brw;		/**< UCAxBRW */
    struct {
      unsigned char br0;	/**< UCAxBR0 */
      unsigned char br1;	/**< UCAxBR1 */
    };
  };
  unsigned int mctlw;			/**< UCAxMCTLW (UART) */ /* 0x08 */
  unsigned int statw;			/**< UCAxSTATW */ /* 0x0A */
  unsigned int rxbuf;			/**< UCAxRXBUF */ /* 0x0C */
  unsigned int txbuf;			/**< UCAxTXBUF */ /* 0x0E */
  unsigned int abctl;			/**< UCAxABCTL (UART) */ /* 0x10 */
  union {						/* 0x12 */
    unsigned int irctl;		/**< UCAxIRCTL (UART) */
    struct {
      unsigned char irtctl; /**< UCAxIRTCTL (UART) */ /* 0x12 */
      unsigned char irrctl; /**< UCAxIRRCTL (UART) */ /* 0x13 */
    };
  };
  unsigned int _unused_0x14;
  unsigned int _unused_0x16;
  unsigned int _unused_0x18;
  unsigned int ie;			/**< UCAxIE */ /* 0x1A */
  unsigned int ifg;			/**< UCAxIFG */ /* 0x1C */
  unsigned int iv;			/**< UCAxIV */ /* 0x1E */
} sBSP430hplEUSCIA;

/** @cond DOXYGEN_INTERNAL */
#define BSP430_PERIPH_EUSCI_A0_BASEADDRESS_ __MSP430_BASEADDRESS_EUSCI_A0__
#define BSP430_PERIPH_EUSCI_A1_BASEADDRESS_ __MSP430_BASEADDRESS_EUSCI_A1__
#define BSP430_PERIPH_EUSCI_A2_BASEADDRESS_ __MSP430_BASEADDRESS_EUSCI_A2__
#define BSP430_PERIPH_EUSCI_A3_BASEADDRESS_ __MSP430_BASEADDRESS_EUSCI_A3__
#define BSP430_PERIPH_EUSCI_B0_BASEADDRESS_ __MSP430_BASEADDRESS_EUSCI_B0__
#define BSP430_PERIPH_EUSCI_B1_BASEADDRESS_ __MSP430_BASEADDRESS_EUSCI_B1__
#define BSP430_PERIPH_EUSCI_B2_BASEADDRESS_ __MSP430_BASEADDRESS_EUSCI_B2__
#define BSP430_PERIPH_EUSCI_B3_BASEADDRESS_ __MSP430_BASEADDRESS_EUSCI_B3__
/** @endcond */ /* DOXYGEN_INTERNAL */

/* !BSP430! periph=serial insert=hal_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_decl] */
/** @def configBSP430_HAL_EUSCI_A0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c EUSCI_A0 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_EUSCI_A0 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_EUSCI_A0 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A0
#define configBSP430_HAL_EUSCI_A0 0
#endif /* configBSP430_HAL_EUSCI_A0 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_EUSCI_A0 - 0
/* You don't need to know about this */
extern sBSP430halSERIAL xBSP430hal_EUSCI_A0_;
#endif /* configBSP430_HAL_EUSCI_A0 */
/** @endcond */

/** BSP430 HAL handle for EUSCI_A0.
 *
 * The handle may be used only if #configBSP430_HAL_EUSCI_A0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_EUSCI_A0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_EUSCI_A0 - 0)
#define BSP430_HAL_EUSCI_A0 (&xBSP430hal_EUSCI_A0_)
#endif /* configBSP430_HAL_EUSCI_A0 */

/** @def configBSP430_HAL_EUSCI_A1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c EUSCI_A1 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_EUSCI_A1 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_EUSCI_A1 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A1
#define configBSP430_HAL_EUSCI_A1 0
#endif /* configBSP430_HAL_EUSCI_A1 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_EUSCI_A1 - 0
/* You don't need to know about this */
extern sBSP430halSERIAL xBSP430hal_EUSCI_A1_;
#endif /* configBSP430_HAL_EUSCI_A1 */
/** @endcond */

/** BSP430 HAL handle for EUSCI_A1.
 *
 * The handle may be used only if #configBSP430_HAL_EUSCI_A1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_EUSCI_A1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_EUSCI_A1 - 0)
#define BSP430_HAL_EUSCI_A1 (&xBSP430hal_EUSCI_A1_)
#endif /* configBSP430_HAL_EUSCI_A1 */

/** @def configBSP430_HAL_EUSCI_A2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c EUSCI_A2 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_EUSCI_A2 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_EUSCI_A2 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A2
#define configBSP430_HAL_EUSCI_A2 0
#endif /* configBSP430_HAL_EUSCI_A2 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_EUSCI_A2 - 0
/* You don't need to know about this */
extern sBSP430halSERIAL xBSP430hal_EUSCI_A2_;
#endif /* configBSP430_HAL_EUSCI_A2 */
/** @endcond */

/** BSP430 HAL handle for EUSCI_A2.
 *
 * The handle may be used only if #configBSP430_HAL_EUSCI_A2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_EUSCI_A2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_EUSCI_A2 - 0)
#define BSP430_HAL_EUSCI_A2 (&xBSP430hal_EUSCI_A2_)
#endif /* configBSP430_HAL_EUSCI_A2 */

/* END AUTOMATICALLY GENERATED CODE [hal_decl] */
/* !BSP430! end=hal_decl */
/* !BSP430! periph=euscia */

/* !BSP430! insert=periph_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_decl] */
/** @def configBSP430_HPL_EUSCI_A0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c EUSCI_A0 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_EUSCI_A0 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HPL_EUSCI_A0
#define configBSP430_HPL_EUSCI_A0 (configBSP430_HAL_EUSCI_A0 - 0)
#endif /* configBSP430_HPL_EUSCI_A0 */

#if (configBSP430_HAL_EUSCI_A0 - 0) && ! (configBSP430_HPL_EUSCI_A0 - 0)
#warning configBSP430_HAL_EUSCI_A0 requested without configBSP430_HPL_EUSCI_A0
#endif /* HAL and not HPL */

/** Handle for the raw EUSCI_A0 device.
 *
 * The handle may be used only if #configBSP430_HPL_EUSCI_A0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_EUSCI_A0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_EUSCI_A0 - 0)
#define BSP430_PERIPH_EUSCI_A0 ((tBSP430periphHandle)(BSP430_PERIPH_EUSCI_A0_BASEADDRESS_))
#endif /* configBSP430_HPL_EUSCI_A0 */

/** @def configBSP430_HPL_EUSCI_A1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c EUSCI_A1 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_EUSCI_A1 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HPL_EUSCI_A1
#define configBSP430_HPL_EUSCI_A1 (configBSP430_HAL_EUSCI_A1 - 0)
#endif /* configBSP430_HPL_EUSCI_A1 */

#if (configBSP430_HAL_EUSCI_A1 - 0) && ! (configBSP430_HPL_EUSCI_A1 - 0)
#warning configBSP430_HAL_EUSCI_A1 requested without configBSP430_HPL_EUSCI_A1
#endif /* HAL and not HPL */

/** Handle for the raw EUSCI_A1 device.
 *
 * The handle may be used only if #configBSP430_HPL_EUSCI_A1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_EUSCI_A1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_EUSCI_A1 - 0)
#define BSP430_PERIPH_EUSCI_A1 ((tBSP430periphHandle)(BSP430_PERIPH_EUSCI_A1_BASEADDRESS_))
#endif /* configBSP430_HPL_EUSCI_A1 */

/** @def configBSP430_HPL_EUSCI_A2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c EUSCI_A2 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_EUSCI_A2 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HPL_EUSCI_A2
#define configBSP430_HPL_EUSCI_A2 (configBSP430_HAL_EUSCI_A2 - 0)
#endif /* configBSP430_HPL_EUSCI_A2 */

#if (configBSP430_HAL_EUSCI_A2 - 0) && ! (configBSP430_HPL_EUSCI_A2 - 0)
#warning configBSP430_HAL_EUSCI_A2 requested without configBSP430_HPL_EUSCI_A2
#endif /* HAL and not HPL */

/** Handle for the raw EUSCI_A2 device.
 *
 * The handle may be used only if #configBSP430_HPL_EUSCI_A2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_EUSCI_A2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_EUSCI_A2 - 0)
#define BSP430_PERIPH_EUSCI_A2 ((tBSP430periphHandle)(BSP430_PERIPH_EUSCI_A2_BASEADDRESS_))
#endif /* configBSP430_HPL_EUSCI_A2 */

/* END AUTOMATICALLY GENERATED CODE [periph_decl] */
/* !BSP430! end=periph_decl */

/* !BSP430! insert=hpl_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hpl_decl] */
/** HPL pointer for EUSCI_A0.
 *
 * Typed pointer to a volatile structure overlaying the EUSCI_A0
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_EUSCI_A0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_EUSCI_A0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_EUSCI_A0 - 0)
#define BSP430_HPL_EUSCI_A0 ((volatile sBSP430hplEUSCIA *)BSP430_PERIPH_EUSCI_A0)
#endif /* configBSP430_HPL_EUSCI_A0 */

/** HPL pointer for EUSCI_A1.
 *
 * Typed pointer to a volatile structure overlaying the EUSCI_A1
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_EUSCI_A1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_EUSCI_A1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_EUSCI_A1 - 0)
#define BSP430_HPL_EUSCI_A1 ((volatile sBSP430hplEUSCIA *)BSP430_PERIPH_EUSCI_A1)
#endif /* configBSP430_HPL_EUSCI_A1 */

/** HPL pointer for EUSCI_A2.
 *
 * Typed pointer to a volatile structure overlaying the EUSCI_A2
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_EUSCI_A2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_EUSCI_A2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_EUSCI_A2 - 0)
#define BSP430_HPL_EUSCI_A2 ((volatile sBSP430hplEUSCIA *)BSP430_PERIPH_EUSCI_A2)
#endif /* configBSP430_HPL_EUSCI_A2 */

/* END AUTOMATICALLY GENERATED CODE [hpl_decl] */
/* !BSP430! end=hpl_decl */

/* !BSP430! insert=hal_isr_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_isr_decl] */
/** @def configBSP430_HAL_EUSCI_A0_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c EUSCI_A0, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_EUSCI_A0 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_EUSCI_A0
 * also be true.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A0_ISR
#define configBSP430_HAL_EUSCI_A0_ISR (configBSP430_HAL_EUSCI_A0 - 0)
#endif /* configBSP430_HAL_EUSCI_A0_ISR */

#if (configBSP430_HAL_EUSCI_A0_ISR - 0) && ! (configBSP430_HAL_EUSCI_A0 - 0)
#warning configBSP430_HAL_EUSCI_A0_ISR requested without configBSP430_HAL_EUSCI_A0
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_EUSCI_A1_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c EUSCI_A1, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_EUSCI_A1 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_EUSCI_A1
 * also be true.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A1_ISR
#define configBSP430_HAL_EUSCI_A1_ISR (configBSP430_HAL_EUSCI_A1 - 0)
#endif /* configBSP430_HAL_EUSCI_A1_ISR */

#if (configBSP430_HAL_EUSCI_A1_ISR - 0) && ! (configBSP430_HAL_EUSCI_A1 - 0)
#warning configBSP430_HAL_EUSCI_A1_ISR requested without configBSP430_HAL_EUSCI_A1
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_EUSCI_A2_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c EUSCI_A2, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_EUSCI_A2 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_EUSCI_A2
 * also be true.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A2_ISR
#define configBSP430_HAL_EUSCI_A2_ISR (configBSP430_HAL_EUSCI_A2 - 0)
#endif /* configBSP430_HAL_EUSCI_A2_ISR */

#if (configBSP430_HAL_EUSCI_A2_ISR - 0) && ! (configBSP430_HAL_EUSCI_A2 - 0)
#warning configBSP430_HAL_EUSCI_A2_ISR requested without configBSP430_HAL_EUSCI_A2
#endif /* HAL_ISR and not HAL */

/* END AUTOMATICALLY GENERATED CODE [hal_isr_decl] */
/* !BSP430! end=hal_isr_decl */

/** eUSCI(A)-specific implementation of hBSP430serialOpenUART() */
hBSP430halSERIAL xBSP430eusciaOpenUART (hBSP430halSERIAL hal,
                                        unsigned char ctl0_byte,
                                        unsigned char ctl1_byte,
                                        unsigned long baud);

/** eUSCI(A)-specific implementation of hBSP430serialOpenSPI() */
hBSP430halSERIAL xBSP430eusciaOpenSPI (hBSP430halSERIAL hal,
                                       unsigned char ctl0_byte,
                                       unsigned char ctl1_byte,
                                       unsigned int prescaler);

/** eUSCI(A)-specific implementation of iBSP430serialConfigureCallbacks() */
int iBSP430eusciaConfigureCallbacks (hBSP430halSERIAL device,
                                     const struct sBSP430halISRCallbackVoid * rx_callback,
                                     const struct sBSP430halISRCallbackVoid * tx_callback);

/** eUSCI(A)-specific implementation of iBSP430serialClose() */
int iBSP430eusciaClose (hBSP430halSERIAL xUSCI);

/** eUSCI(A)-specific implementation of vBSP430serialWakeupTransmit_ni() */
void vBSP430eusciaWakeupTransmit_ni (hBSP430halSERIAL device);

/** eUSCI(A)-specific implementation of vBSP430serialFlush_ni() */
void vBSP430eusciaFlush_ni (hBSP430halSERIAL device);

/** eUSCI(A)-specific implementation of iBSP430serialTransmitByte_ni() */
int iBSP430eusciaTransmitByte_ni (hBSP430halSERIAL device, int c);

/** eUSCI(A)-specific implementation of iBSP430serialTransmitData_ni() */
int iBSP430eusciaTransmitData_ni (hBSP430halSERIAL device, const uint8_t * data, size_t len);

/** eUSCI(A)-specific implementation of iBSP430serialTransmitASCIIZ_ni() */
int iBSP430eusciaTransmitASCIIZ_ni (hBSP430halSERIAL device, const char * str);

/** Get the HPL handle for a specific EUSCIA instance.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_EUSCI_A0.
 *
 * @return A typed pointer that can be used to manipulate the
 * peripheral.  A null pointer is returned if the handle does not
 * correspond to a timer for which the HPL interface been enabled
 * (e.g., with #configBSP430_HPL_EUSCI_A0).
 */
volatile sBSP430hplEUSCIA * xBSP430hplLookupEUSCIA (tBSP430periphHandle periph);

/** Get the HAL handle for a specific EUSCIA instance.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_EUSCI_A0.
 *
 * @return the HAL handle for the peripheral.  A null pointer is
 * returned if the handle does not correspond to a timer for which the
 * HAL interface has been enabled (e.g., with #configBSP430_HAL_EUSCI_A0).
 */
hBSP430halSERIAL hBSP430eusciaLookup (tBSP430periphHandle periph);

/** Get a human-readable identifier for the EUSCI-A peripheral
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_EUSCI_A0.
 *
 * @return The short name of the port, e.g. "EUSCI_A0".  If the peripheral
 * is not recognized as a EUSCI-A device, a null pointer is returned. */
const char * xBSP430eusciaName (tBSP430periphHandle periph);

#endif /* BSP430_PERIPH_EUSCIA_H */

