import sys
import re
import os.path
import argparse

templates = {
    'hal_decl' : '''/** @def configBSP430_HAL_%(INSTANCE)s
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c %(INSTANCE)s peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_%(INSTANCE)s that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_%(INSTANCE)s to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_%(INSTANCE)s
#define configBSP430_HAL_%(INSTANCE)s 0
#endif /* configBSP430_HAL_%(INSTANCE)s */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_%(INSTANCE)s - 0
/* You don't need to know about this */
extern sBSP430hal%(PERIPH)s xBSP430hal_%(INSTANCE)s_;
#endif /* configBSP430_HAL_%(INSTANCE)s */
/** @endcond */

/** BSP430 HAL handle for %(INSTANCE)s.
 *
 * The handle may be used only if #configBSP430_HAL_%(INSTANCE)s
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_%(INSTANCE)s */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_%(INSTANCE)s - 0)
#define BSP430_HAL_%(INSTANCE)s (&xBSP430hal_%(INSTANCE)s_)
#endif /* configBSP430_HAL_%(INSTANCE)s */
''',

    'periph_decl' : '''/** @def configBSP430_HPL_%(INSTANCE)s
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c %(INSTANCE)s peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_%(INSTANCE)s defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HPL_%(INSTANCE)s
#define configBSP430_HPL_%(INSTANCE)s (configBSP430_HAL_%(INSTANCE)s - 0)
#endif /* configBSP430_HPL_%(INSTANCE)s */

#if (configBSP430_HAL_%(INSTANCE)s - 0) && ! (configBSP430_HPL_%(INSTANCE)s - 0)
#warning configBSP430_HAL_%(INSTANCE)s requested without configBSP430_HPL_%(INSTANCE)s
#endif /* HAL and not HPL */

/** Handle for the raw %(INSTANCE)s device.
 *
 * The handle may be used only if #configBSP430_HPL_%(INSTANCE)s
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_%(INSTANCE)s */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_%(INSTANCE)s - 0)
#define BSP430_PERIPH_%(INSTANCE)s ((tBSP430periphHandle)(BSP430_PERIPH_%(INSTANCE)s_BASEADDRESS_))
#endif /* configBSP430_HPL_%(INSTANCE)s */
''',

    'hpl_decl' : '''/** HPL pointer for %(INSTANCE)s.
 *
 * Typed pointer to a volatile structure overlaying the %(INSTANCE)s
 * peripheral register map.
 * 
 * The pointer may be used only if #configBSP430_HPL_%(INSTANCE)s
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_%(INSTANCE)s */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_%(INSTANCE)s - 0)
#define BSP430_HPL_%(INSTANCE)s ((volatile sBSP430hpl%(PERIPH)s *)BSP430_PERIPH_%(INSTANCE)s)
#endif /* configBSP430_HPL_%(INSTANCE)s */
''',
    
    'port_hpl_decl' : '''/** HPL pointer for %(INSTANCE)s.
 *
 * Typed pointer to a volatile structure overlaying the %(INSTANCE)s
 * peripheral register map.
 * 
 * The pointer may be used only if #configBSP430_HPL_%(INSTANCE)s
 * is defined to a true value.
 *
 * @note The underlying type is #sBSP430hplPORTIE for ports 1 and 2
 * and ports on 5xx/6xx MCUs, and is #sBSP430hplPORT for ports 3
 * and higher on pre-5xx MCUs.  The generated documentation may not
 * reflect the correct structure.
 *
 * @dependency #configBSP430_HPL_%(INSTANCE)s */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_%(INSTANCE)s - 0)
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (%(#)s <= 2)
#define BSP430_HPL_%(INSTANCE)s ((volatile sBSP430hplPORTIE *)BSP430_PERIPH_%(INSTANCE)s)
#else /* IE */
#define BSP430_HPL_%(INSTANCE)s ((volatile sBSP430hplPORT *)BSP430_PERIPH_%(INSTANCE)s)
#endif /* IE */
#endif /* configBSP430_HPL_%(INSTANCE)s */
''',
    
    'hal_isr_decl' : '''/** @def configBSP430_HAL_%(INSTANCE)s_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c %(INSTANCE)s but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_%(INSTANCE)s defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_%(INSTANCE)s
 * also be true.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_%(INSTANCE)s_ISR
#define configBSP430_HAL_%(INSTANCE)s_ISR (configBSP430_HAL_%(INSTANCE)s - 0)
#endif /* configBSP430_HAL_%(INSTANCE)s_ISR */

#if (configBSP430_HAL_%(INSTANCE)s_ISR - 0) && ! (configBSP430_HAL_%(INSTANCE)s - 0)
#warning configBSP430_HAL_%(INSTANCE)s_ISR requested without configBSP430_HAL_%(INSTANCE)s
#endif /* HAL_ISR and not HAL */
''',

    'hal_port_isr_decl' : '''/** @def configBSP430_HAL_%(INSTANCE)s_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c %(INSTANCE)s but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_%(INSTANCE)s defaults this to true if
 * the port supports interrupts, so you only need to explicitly set it
 * if you do not want to use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_%(INSTANCE)s
 * also be true.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_%(INSTANCE)s_ISR
#define configBSP430_HAL_%(INSTANCE)s_ISR ((configBSP430_HAL_%(INSTANCE)s - 0) && defined(PORT%(#)s_VECTOR))
#endif /* configBSP430_HAL_%(INSTANCE)s_ISR */

#if (configBSP430_HAL_%(INSTANCE)s_ISR - 0) && ! (configBSP430_HAL_%(INSTANCE)s - 0)
#warning configBSP430_HAL_%(INSTANCE)s_ISR requested without configBSP430_HAL_%(INSTANCE)s
#endif /* HAL_ISR and not HAL */
''',

    'hal_usci_isr_decl' : '''/** @def configBSP430_HAL_USCI_AB%(INSTANCE)sRX_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c USCI_A%(INSTANCE)s or @c
 * USCI_B%(INSTANCE)s but want to define your own interrupt service
 * routine for receive operations for the peripheral.
 *
 * Enabling #configBSP430_HAL_USCI_A%(INSTANCE)s or
 * #configBSP430_HAL_USCI_B%(INSTANCE)s defaults this to true, so you
 * only need to explicitly set it if you do not want to use the
 * standard ISR provided by BSP430.
 *
 * @note Enabling this requires that at least one of
 * #configBSP430_HAL_USCI_A%(INSTANCE)s and
 * #configBSP430_HAL_USCI_B%(INSTANCE)s also be true.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_USCI_AB%(INSTANCE)sRX_ISR
#define configBSP430_HAL_USCI_AB%(INSTANCE)sRX_ISR ((configBSP430_HAL_USCI_A%(INSTANCE)s - 0) || (configBSP430_HAL_USCI_B%(INSTANCE)s - 0))
#endif /* configBSP430_HAL_USCI_AB%(INSTANCE)sRX_ISR */

#if ((configBSP430_HAL_%(INSTANCE)s_ISR - 0) \\
     && ! ((configBSP430_HAL_USCI_A%(INSTANCE)sRX - 0) | (configBSP430_HAL_USCI_B%(INSTANCE)s - 0)))
#warning configBSP430_HAL_USCI_AB%(INSTANCE)sRX_ISR requested without configBSP430_HAL_USCI_A%(INSTANCE)s or configBSP430_HAL_USCI_B%(INSTANCE)s
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_USCI_AB%(INSTANCE)sTX_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c USCI_A%(INSTANCE)s or @c
 * USCI_B%(INSTANCE)s but want to define your own interrupt service
 * routine for transmit operations for the peripheral.
 *
 * Enabling #configBSP430_HAL_USCI_A%(INSTANCE)s or
 * #configBSP430_HAL_USCI_B%(INSTANCE)s defaults this to true, so you
 * only need to explicitly set it if you do not want to use the
 * standard ISR provided by BSP430.
 *
 * @note Enabling this requires that at least one of
 * #configBSP430_HAL_USCI_A%(INSTANCE)s and
 * #configBSP430_HAL_USCI_B%(INSTANCE)s also be true.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_USCI_AB%(INSTANCE)sTX_ISR
#define configBSP430_HAL_USCI_AB%(INSTANCE)sTX_ISR ((configBSP430_HAL_USCI_A%(INSTANCE)s - 0) | (configBSP430_HAL_USCI_B%(INSTANCE)s - 0))
#endif /* configBSP430_HAL_USCI_AB%(INSTANCE)sTX_ISR */

#if ((configBSP430_HAL_%(INSTANCE)s_ISR - 0) \\
     && ! ((configBSP430_HAL_USCI_A%(INSTANCE)sTX - 0) | (configBSP430_HAL_USCI_B%(INSTANCE)s - 0)))
#warning configBSP430_HAL_USCI_AB%(INSTANCE)sTX_ISR requested without configBSP430_HAL_USCI_A%(INSTANCE)s or configBSP430_HAL_USCI_B%(INSTANCE)s
#endif /* HAL_ISR and not HAL */
''',

    'hal_timer_isr_decl' : '''/** @def configBSP430_HAL_%(INSTANCE)s_CC0_ISR
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * Define to a true value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c %(INSTANCE)s, and want to BSP430 to provide
 * an interrupt service routine that dispatches CC0 events.
 *
 * @note Unlike #configBSP430_HAL_%(INSTANCE)s_ISR, enabling #configBSP430_HAL_%(INSTANCE)s
 * does not default to enable this feature.
 *
 *
 * @cppflag
 * @nodefault
 * @dependency #configBSP430_HAL_%(INSTANCE)s */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_%(INSTANCE)s_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_%(INSTANCE)s_CC0_ISR - 0) && ! (configBSP430_HAL_%(INSTANCE)s - 0)
#warning configBSP430_HAL_%(INSTANCE)s_CC0_ISR requested without configBSP430_HAL_%(INSTANCE)s
#endif /* HAL_CC0_ISR and not HAL */

/** @def configBSP430_HAL_%(INSTANCE)s_ISR
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This option defaults to true when #configBSP430_HAL_%(INSTANCE)s is
 * enabled.
 *
 * Define it a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c %(INSTANCE)s but want to define your
 * own interrupt service routine for the peripheral.
 *
 * @cppflag
 * @defaulted
 * @dependency #configBSP430_HAL_%(INSTANCE)s */
#ifndef configBSP430_HAL_%(INSTANCE)s_ISR
#define configBSP430_HAL_%(INSTANCE)s_ISR (configBSP430_HAL_%(INSTANCE)s - 0)
#endif /* configBSP430_HAL_%(INSTANCE)s_ISR */

#if (configBSP430_HAL_%(INSTANCE)s_ISR - 0) && ! (configBSP430_HAL_%(INSTANCE)s - 0)
#warning configBSP430_HAL_%(INSTANCE)s_ISR requested without configBSP430_HAL_%(INSTANCE)s
#endif /* HAL_ISR and not HAL */
''',

    'hal_defn' : '''#if configBSP430_HAL_%(INSTANCE)s - 0
sBSP430hal%(PERIPH)s xBSP430hal_%(INSTANCE)s_ = {
  .%(periph)s = BSP430_HPL_%(INSTANCE)s
};
#endif /* configBSP430_HAL_%(INSTANCE)s */
''',

    'hal_isr_defn' : '''#if configBSP430_HAL_%(INSTANCE)s_ISR - 0
static void
__attribute__((__interrupt__(%(BASEINSTANCE)s_VECTOR)))
isr_%(INSTANCE)s (void)
{
  int rv = %(periph)s_isr(BSP430_HAL_%(INSTANCE)s);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_%(INSTANCE)s_ISR */
''',

    'hal_timer_isr_defn' : '''#if configBSP430_HAL_T%(TYPE)s%(INSTANCE)s_CC0_ISR - 0
static void
__attribute__((__interrupt__(TIMER%(INSTANCE)s_%(TYPE)s0_VECTOR)))
isr_cc0_T%(TYPE)s%(INSTANCE)s (void)
{
  hBSP430hal%(PERIPH)s timer = BSP430_HAL_T%(TYPE)s%(INSTANCE)s;
  int rv = iBSP430callbackInvokeISRIndexed_ni(0 + timer->cc_cbchain_ni, timer, 0, 0);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_T%(TYPE)s%(INSTANCE)s_CC0_ISR */

#if configBSP430_HAL_T%(TYPE)s%(INSTANCE)s_ISR - 0
static void
__attribute__((__interrupt__(TIMER%(INSTANCE)s_%(TYPE)s1_VECTOR)))
isr_T%(TYPE)s%(INSTANCE)s (void)
{
  hBSP430hal%(PERIPH)s timer = BSP430_HAL_T%(TYPE)s%(INSTANCE)s;
  int iv = T%(TYPE)s%(INSTANCE)sIV;
  int rv = 0;
  if (0 != iv) {
    if (T%(TYPE)s_OVERFLOW == iv) {
      ++timer->overflow_count;
      rv = iBSP430callbackInvokeISRVoid_ni(&timer->overflow_cbchain_ni, timer, rv);
    } else {
      int cc = iv / 2;
      rv = iBSP430callbackInvokeISRIndexed_ni(cc + timer->cc_cbchain_ni, timer, cc, rv);
    }
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_T%(TYPE)s%(INSTANCE)s_ISR */
''',
    
    'hal_port_defn' : '''#if configBSP430_HAL_%(INSTANCE)s - 0
struct sBSP430halPORT xBSP430hal_%(INSTANCE)s_ = {
  .hal_state = { .cflags =
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (%(#)s <= 2)
    BSP430_PORT_HAL_HPL_VARIANT_PORTIE
#else /* IE */
    BSP430_PORT_HAL_HPL_VARIANT_PORT
#endif /* IE */
#if configBSP430_HAL_%(INSTANCE)s_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_%(INSTANCE)s_ISR */
  },
  .hpl = {
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (%(#)s <= 2)
    .portie
#else /* interrupt-enabled */
    .port
#endif /* interrupt-enabled */
       = BSP430_HPL_%(INSTANCE)s
  },
#if (BSP430_PORT_SUPPORTS_REN - 0) && ! (BSP430_CORE_FAMILY_IS_5XX - 0)
  .renp = &P%(#)sREN,
#endif /* __MSP430_HAS_PORT1_R__ */
};
#endif /* configBSP430_HAL_%(INSTANCE)s */
''',

    'hal_serial_defn' : '''#if configBSP430_HAL_%(INSTANCE)s - 0
struct sBSP430halSERIAL xBSP430hal_%(INSTANCE)s_ = {
  .hal_state = { .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_%(PERIPH)s
#if configBSP430_HAL_%(INSTANCE)s_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_%(INSTANCE)s_ISR */
  },
  .hpl = { .%(periph)s = BSP430_HPL_%(INSTANCE)s },
#if BSP430_SERIAL - 0
  .dispatch = &dispatch_,
#endif /* BSP430_SERIAL */
};
#endif /* configBSP430_HAL_%(INSTANCE)s */
''',

    'hal_variant_hpl_macro' : '''/** True iff the HPL pointer of the %(periph)s HAL is a %(INSTANCE)s variant. */
#define BSP430_%(PERIPH)s_HAL_HPL_VARIANT_IS_%(INSTANCE)s(_hal) (BSP430_%(PERIPH)s_HAL_HPL_VARIANT_%(INSTANCE)s == BSP430_PERIPH_HAL_STATE_CFLAGS_VARIANT(_hal))

/** Get the %(periph)s HPL pointer if the HAL is a %(INSTANCE)s variant.
 *
 * Value is a null pointer if the HAL references a different HPL type. */
#define BSP430_%(PERIPH)s_HAL_GET_HPL_%(INSTANCE)s(_hal) ((BSP430_%(PERIPH)s_HAL_HPL_VARIANT_IS_%(INSTANCE)s(_hal)) ? (_hal)->hpl.%(instance)s : (void *)0)

/** Get the %(periph)s HPL auxiliary pointer if the HAL is a %(INSTANCE)s variant.
 *
 * Value is a null pointer if the HAL references a different HPL type. */
#define BSP430_%(PERIPH)s_HAL_GET_HPLAUX_%(INSTANCE)s(_hal) ((BSP430_%(PERIPH)s_HAL_HPL_VARIANT_IS_%(INSTANCE)s(_hal)) ? (_hal)->hpl_aux.%(instance)s : (void *)0)
''',

    'hal_port_isr_defn' : '''#if configBSP430_HAL_%(INSTANCE)s_ISR - 0
static void
__attribute__((__interrupt__(%(INSTANCE)s_VECTOR)))
isr_%(INSTANCE)s (void)
{
  int idx = 0;
  int rv;
#if BSP430_CORE_FAMILY_IS_5XX - 0
  int iv = P%(#)sIV;

  if (0 == iv) {
    return;
  }
  idx = (iv - 2) / 2;
#else /* CPUX */
  unsigned char bit = 1;

  while (bit && !(bit & P%(#)sIFG)) {
    bit <<= 1;
    ++idx;
  }
  if (! bit) {
    return;
  }
  P%(#)sIFG &= ~bit;
#endif /* CPUX */
  rv = port_isr(BSP430_HAL_%(INSTANCE)s, idx);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_%(INSTANCE)s_ISR */
''',

    'periph_hpl_port_demux' : '''#if (configBSP430_HPL_%(INSTANCE)s - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (%(#)s %(IE_TEST)s 2))
  if (BSP430_PERIPH_%(INSTANCE)s == periph) {
    return BSP430_HPL_%(INSTANCE)s;
  }
#endif /* configBSP430_HPL_%(INSTANCE)s */
''',

    'periph_name_demux' : '''#if configBSP430_HPL_%(INSTANCE)s - 0
  if (BSP430_PERIPH_%(INSTANCE)s == periph) {
    return "%(INSTANCE)s";
  }
#endif /* configBSP430_HPL_%(INSTANCE)s */
''',

    'periph_hpl_demux' : '''#if configBSP430_HPL_%(INSTANCE)s - 0
  if (BSP430_PERIPH_%(INSTANCE)s == periph) {
    return BSP430_HPL_%(INSTANCE)s;
  }
#endif /* configBSP430_HPL_%(INSTANCE)s */
''',

    'periph_hal_demux' : '''#if configBSP430_HAL_%(INSTANCE)s - 0
  if (BSP430_PERIPH_%(INSTANCE)s == periph) {
    return BSP430_HAL_%(INSTANCE)s;
  }
#endif /* configBSP430_HAL_%(INSTANCE)s */
''',

    'platform_decl' : '''/** @def BSP430_PLATFORM_%(INSTANCE)s
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/%(tool)s-%(instance)s">%(TOOL)s-%(INSTANCE)s</a> platform.
 *
 * A true value causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/%(instance)s/platform.h>.
 * If you include that header directly, #BSP430_PLATFORM_%(INSTANCE)s will be
 * defined for you.
 *
 * A true value also causes <bsp430/platform/bsp430_config.h> to
 * include <bsp430/platform/%(instance)s/bsp430_config.h> for you.
 * You should not include that header directly, as it coordinates with
 * the generic platform version.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_%(INSTANCE)s
#define BSP430_PLATFORM_%(INSTANCE)s 0
#endif /* BSP430_PLATFORM_%(INSTANCE)s */

#if BSP430_PLATFORM_%(INSTANCE)s - 0
#include <bsp430/platform/%(instance)s/platform.h>
#endif /* BSP430_PLATFORM_%(INSTANCE)s */
''',

    'platform_decl_url' : '''/** @def BSP430_PLATFORM_%(INSTANCE)s
 * Define to a true value if application is being built for the
 * <a href="%(url)s">%(INSTANCE)s</a> platform.
 *
 * A true value causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/%(instance)s/platform.h>.
 * If you include that header directly, #BSP430_PLATFORM_%(INSTANCE)s will be
 * defined for you.
 *
 * A true value also causes <bsp430/platform/bsp430_config.h> to
 * include <bsp430/platform/%(instance)s/bsp430_config.h> for you.
 * You should not include that header directly, as it coordinates with
 * the generic platform version.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_%(INSTANCE)s
#define BSP430_PLATFORM_%(INSTANCE)s 0
#endif /* BSP430_PLATFORM_%(INSTANCE)s */

#if BSP430_PLATFORM_%(INSTANCE)s - 0
#include <bsp430/platform/%(instance)s/platform.h>
#endif /* BSP430_PLATFORM_%(INSTANCE)s */
''',

    'platform_bsp430_config' : '''#if BSP430_PLATFORM_%(INSTANCE)s - 0
#include <bsp430/platform/%(instance)s/bsp430_config.h>
#endif /* BSP430_PLATFORM_%(INSTANCE)s */
''',


    'feature_ccaclk_decl' : '''#define BSP430_TIMER_CCACLK 1
#define BSP430_TIMER_CCACLK_PERIPH_HANDLE BSP430_PERIPH_%(TIMER)s
#define BSP430_TIMER_CCACLK_CC_INDEX %(CC_INDEX)s
#define BSP430_TIMER_CCACLK_CCIS CCIS_%(CCIS)s
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE BSP430_PERIPH_%(CLK_PORT)s
#define BSP430_TIMER_CCACLK_CLK_PORT_BIT %(CLK_PIN)s''',

    'feature_ccaclk_cfg' : '''#if configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL - 0
#if !defined(configBSP430_HAL_%(TIMER)s)
#define configBSP430_HAL_%(TIMER)s 1
#endif /* configBSP430_HAL_%(TIMER)s */
#if configBSP430_TIMER_CCACLK_USE_DEFAULT_CC0_ISR - 0
#if !defined(configBSP430_HAL_%(TIMER)s_CC0_ISR)
#define configBSP430_HAL_%(TIMER)s_CC0_ISR 1
#endif /* configBSP430_HAL_%(TIMER)s_CC0_ISR */
#endif /* configBSP430_TIMER_CCACLK_USE_DEFAULT_CC0_ISR */
#else /* configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL */
#if !defined(configBSP430_HPL_%(TIMER)s)
#define configBSP430_HPL_%(TIMER)s 1
#endif /* configBSP430_HPL_%(TIMER)s */
#endif /* configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL */

#if configBSP430_TIMER_CCACLK_USE_DEFAULT_PORT_HAL - 0
#if !defined(configBSP430_HAL_%(CLK_PORT)s)
#define configBSP430_HAL_%(CLK_PORT)s 1
#endif /* configBSP430_HAL_%(CLK_PORT)s */
#else /* configBSP430_TIMER_CCACLK_USE_DEFAULT_PORT_HAL */
#if !defined(configBSP430_HPL_%(CLK_PORT)s)
#define configBSP430_HPL_%(CLK_PORT)s 1
#endif /* configBSP430_HPL_%(CLK_PORT)s */
#endif /* configBSP430_TIMER_CCACLK_USE_DEFAULT_PORT_HAL */''',

    'feature_startif' : '''#if ((configBSP430_%(MODULE)s_%(FEATURE)s - 0)                                    \\
     && ((! defined(configBSP430_%(MODULE)s_%(FEATURE)s_USE_DEFAULT_RESOURCE))    \\
         || (configBSP430_%(MODULE)s_%(FEATURE)s_USE_DEFAULT_RESOURCE - 0)))''',

    'feature_endif' : '''#endif /* configBSP430_%(MODULE)s_%(FEATURE)s && need default */''',

    'module_startif' : '''#if ((configBSP430_%(MODULE)s - 0)                                    \\
     && ((! defined(configBSP430_%(MODULE)s_USE_DEFAULT_RESOURCE))    \\
         || (configBSP430_%(MODULE)s_USE_DEFAULT_RESOURCE - 0)))''',

    'module_endif' : '''#endif /* configBSP430_%(MODULE)s && need default */''',

    }

directive_re = re.compile('!BSP430!\s*(?P<keywords>.*)$')
idmap = {}

def expandTemplate (tplname, idmap):
    global templates

    template = templates[tplname]
    subst_map = { 'template' : tplname }
    periph = idmap.get('periph', 'nop')
    subst_map.update({ 'periph': periph.lower(),
                       'PERIPH': periph.upper() })
    subst_keys = idmap.get('subst')
    if subst_keys is not None:
        for sm in subst_keys.split(','):
            subst_map[sm.lower()] = idmap[sm].lower()
            subst_map[sm.upper()] = idmap[sm].upper()
    text = []
    text.append('/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [%(template)s] */' % subst_map)

    for i in idmap['instance'].split(','):
        if i.startswith(periph.upper()):
            subst_map.update({ '#' : i[len(periph):] })
        subst_map.update({ 'instance': i.lower(), 'INSTANCE': i.upper() })
        subst_map.update({ 'baseinstance' : subst_map.get('instance'),
                           'BASEINSTANCE' : subst_map.get('INSTANCE') })
        uscifrom = idmap.get('uscifrom', None)
        if uscifrom is not None:
            subst_map.update({ 'baseinstance' : subst_map.get('instance').replace(uscifrom.lower(), 'usci'),
                               'BASEINSTANCE' : subst_map.get('INSTANCE').replace(uscifrom.upper(), 'USCI') })
        if idmap.get('portexpand', False):
            subst_map.update({ 'PORTA' : 'PORT' + i[0],
                               'PORT1': 'PORT%d' % (1 + 2 * (ord(i[0]) - ord('A')),),
                               'PORT2': 'PORT%d' % (2 + 2 * (ord(i[0]) - ord('A')),) })
        try:
            text.append(template % subst_map)
        except TypeError as e:
            print 'Exception expanding %s with %s: %s' % (insert_name, subst_map, e)
            print template
            raise
        subst_map.pop('#', None)
    text.append('/* END AUTOMATICALLY GENERATED CODE [%(template)s] */' % subst_map)
    text.append('')
    return '\n'.join(text)

aparse = argparse.ArgumentParser(description='Expand templates in BSP430 source code')
aparse.add_argument('files', metavar='FILE', type=str, nargs='+',
                    help='files to be expanded')
aparse.add_argument('-s', '--strip-templates',
                    dest='strip_templates', action='store_true',
                    help='remove existing template expansions from the code')
args = aparse.parse_args()

for fn in args.files:
    try:
        contents = file(fn).readlines()
    except IOError as e:
        print 'ERROR: %s' % (e,)
        continue
    new_contents = []
    insert_name = None
    insertable = None
    while contents:
        line = contents.pop(0)
        if insertable is None:
            new_contents.append(line)
        mo = directive_re.search(line)
        if mo:
            kw = filter(lambda _s: 0 < _s.find('='), mo.group('keywords').strip().split())
            idmap.update([ _s.split('=', 1) for _s in kw ])
            if 'insert' in idmap:
                assert insertable is None
                insert_name = idmap.pop('insert')
                insertable = expandTemplate(insert_name, idmap)
            end_name = idmap.pop('end', None)
            if end_name is not None:
                assert end_name == insert_name, '%s != %s' % (end_name, insert_name)
                if not args.strip_templates:
                    new_contents.append(insertable)
                new_contents.append(line)
                insertable = None
                end_name = None

    (path, filename) = os.path.split(fn)
    bfn = os.path.join(path, filename + '.bak')
    try:
        os.unlink(bfn)
    except OSError:
        pass
    os.rename(fn, bfn)
    file(fn, 'w').write(''.join(new_contents))
