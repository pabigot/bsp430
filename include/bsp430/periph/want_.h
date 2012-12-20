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
 * @brief Translate functional resource requests to core resource requests.
 *
 * This header is used to translate requests related to functional
 * resources (like #BSP430_UPTIME_TIMER_PERIPH_HANDLE) into requests
 * for the underlying core resource (like #configBSP430_HAL_TB0).  It
 * is intended to be included multiple times after defining
 * #BSP430_WANT_PERIPH_CPPID and other macros in terms of the
 * functional-specific request macros.
 */

/** Identify the core peripheral for which resource requests are needed.
 *
 * This should be defined to a constant such as #BSP430_PERIPH_CPPID_TA0. */
#if defined(BSP430_DOXYGEN)
#define BSP430_WANT_PERIPH_CPPID context specific
#endif /* BSP430_DOXYGEN */

/** Request the @HPL interface for the wanted peripheral.
 * @cppflag */
#if defined(BSP430_DOXYGEN)
#define BSP430_WANT_CONFIG_HPL context specific
#endif /* BSP430_DOXYGEN */

/** Request the @HAL interface for the wanted peripheral.
 * @cppflag */
#if defined(BSP430_DOXYGEN)
#define BSP430_WANT_CONFIG_HAL context specific
#endif /* BSP430_DOXYGEN */

/** Request the @HAL interrupt interface for the wanted peripheral.
 * @cppflag */
#if defined(BSP430_DOXYGEN)
#define BSP430_WANT_CONFIG_HAL_ISR context specific
#endif /* BSP430_DOXYGEN */

/** Request the @HAL CC0 interrupt interface for the wanted peripheral.
 *
 * This is only meaningful if #BSP430_WANT_PERIPH_CPPID identifies a
 * timer peripheral.
 *
 * @cppflag */
#if defined(BSP430_DOXYGEN)
#define BSP430_WANT_CONFIG_HAL_CC0_ISR context specific
#endif /* BSP430_DOXYGEN */

/** @cond DOXYGEN_INTERNAL */

#if BSP430_PERIPH_CCPID_NONE == BSP430_WANT_PERIPH_CPPID
/* !BSP430! instance=@periphs insert=periph_want */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_want] */

#elif BSP430_PERIPH_CPPID_PORT1 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_PORT1 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_PORT1 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_PORT1_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_PORT1_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_PORT2 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_PORT2 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_PORT2 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_PORT2_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_PORT2_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_PORT3 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_PORT3 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_PORT3 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_PORT3_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_PORT3_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_PORT4 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_PORT4 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_PORT4 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_PORT4_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_PORT4_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_PORT5 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_PORT5 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_PORT5 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_PORT5_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_PORT5_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_PORT6 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_PORT6 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_PORT6 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_PORT6_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_PORT6_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_PORT7 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_PORT7 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_PORT7 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_PORT7_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_PORT7_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_PORT8 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_PORT8 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_PORT8 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_PORT8_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_PORT8_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_PORT9 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_PORT9 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_PORT9 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_PORT9_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_PORT9_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_PORT10 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_PORT10 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_PORT10 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_PORT10_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_PORT10_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_PORT11 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_PORT11 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_PORT11 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_PORT11_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_PORT11_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_TA0 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_TA0 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_TA0 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_TA0_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_TA0_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_TA1 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_TA1 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_TA1 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_TA1_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_TA1_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_TA2 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_TA2 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_TA2 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_TA2_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_TA2_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_TA3 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_TA3 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_TA3 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_TA3_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_TA3_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_TB0 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_TB0 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_TB0 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_TB0_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_TB0_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_TB1 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_TB1 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_TB1 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_TB1_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_TB1_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_TB2 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_TB2 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_TB2 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_TB2_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_TB2_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_USCI_A0 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_USCI_A0 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_USCI_A0 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_USCI_A0_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_USCI_A0_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_USCI_A1 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_USCI_A1 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_USCI_A1 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_USCI_A1_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_USCI_A1_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_USCI_B0 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_USCI_B0 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_USCI_B0 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_USCI_B0_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_USCI_B0_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_USCI_B1 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_USCI_B1 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_USCI_B1 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_USCI_B1_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_USCI_B1_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_USCI5_A0 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_USCI5_A0 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_USCI5_A0 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_USCI5_A0_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_USCI5_A0_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_USCI5_A1 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_USCI5_A1 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_USCI5_A1 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_USCI5_A1_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_USCI5_A1_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_USCI5_A2 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_USCI5_A2 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_USCI5_A2 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_USCI5_A2_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_USCI5_A2_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_USCI5_A3 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_USCI5_A3 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_USCI5_A3 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_USCI5_A3_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_USCI5_A3_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_USCI5_B0 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_USCI5_B0 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_USCI5_B0 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_USCI5_B0_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_USCI5_B0_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_USCI5_B1 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_USCI5_B1 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_USCI5_B1 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_USCI5_B1_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_USCI5_B1_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_USCI5_B2 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_USCI5_B2 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_USCI5_B2 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_USCI5_B2_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_USCI5_B2_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_USCI5_B3 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_USCI5_B3 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_USCI5_B3 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_USCI5_B3_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_USCI5_B3_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_EUSCI_A0 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_EUSCI_A0 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_EUSCI_A0 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_EUSCI_A0_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_EUSCI_A0_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_EUSCI_A1 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_EUSCI_A1 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_EUSCI_A1 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_EUSCI_A1_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_EUSCI_A1_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_EUSCI_A2 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_EUSCI_A2 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_EUSCI_A2 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_EUSCI_A2_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_EUSCI_A2_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */

#elif BSP430_PERIPH_CPPID_EUSCI_B0 == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_EUSCI_B0 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_EUSCI_B0 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_EUSCI_B0_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_EUSCI_B0_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */
/* END AUTOMATICALLY GENERATED CODE [periph_want] */
/* !BSP430! end=periph_want */
#endif /* BSP430_WANT_PERIPH_CCPID */

/** @endcond */
