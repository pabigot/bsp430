import sys
import re
import os.path
import argparse

templates = {
    'hal_decl' : '''/** Control inclusion of the @HAL interface to #BSP430_PERIPH_%(INSTANCE)s
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hal%(PERIPH)s structure reference is
 * available as #BSP430_HAL_%(INSTANCE)s.
 *
 * It may also be obtained using
 * #hBSP430%(periph)sLookup(#BSP430_PERIPH_%(INSTANCE)s).
 *
 * @cppflag
 * @affects #configBSP430_HPL_%(INSTANCE)s is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_%(INSTANCE)s
#define configBSP430_HAL_%(INSTANCE)s 0
#endif /* configBSP430_HAL_%(INSTANCE)s */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_%(INSTANCE)s - 0
/* You do not need to know about this */
extern sBSP430hal%(PERIPH)s xBSP430hal_%(INSTANCE)s_;
#endif /* configBSP430_HAL_%(INSTANCE)s */
/** @endcond */

/** sBSP430hal%(PERIPH)s HAL handle for #BSP430_PERIPH_%(INSTANCE)s.
 *
 * This pointer may be used only if #configBSP430_HAL_%(INSTANCE)s
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_%(INSTANCE)s */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_%(INSTANCE)s - 0)
#define BSP430_HAL_%(INSTANCE)s (&xBSP430hal_%(INSTANCE)s_)
#endif /* configBSP430_HAL_%(INSTANCE)s */
''',

    'periph_decl' : '''/** Control inclusion of the @HPL interface to #BSP430_PERIPH_%(INSTANCE)s
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hpl%(PERIPH)s structure reference is
 * available as #BSP430_HPL_%(INSTANCE)s.
 *
 * It may also be obtained using
 * #xBSP430hplLookup%(PERIPH)s(#BSP430_PERIPH_%(INSTANCE)s).
 *
 * @cppflag
 * @ingroup grp_config_core
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

    'hpl_decl' : '''/** sBSP430hpl%(PERIPH)s HPL pointer for #BSP430_PERIPH_%(INSTANCE)s.
 *
 * This pointer to a volatile structure overlaying the %(INSTANCE)s
 * peripheral register map may be used only if
 * #configBSP430_HPL_%(INSTANCE)s is defined to a true value.
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
    
    'hal_isr_decl' : '''/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_%(INSTANCE)s
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_%(INSTANCE)s is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_%(INSTANCE)s.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_%(INSTANCE)s_ISR
#define configBSP430_HAL_%(INSTANCE)s_ISR (configBSP430_HAL_%(INSTANCE)s - 0)
#endif /* configBSP430_HAL_%(INSTANCE)s_ISR */

#if (configBSP430_HAL_%(INSTANCE)s_ISR - 0) && ! (configBSP430_HAL_%(INSTANCE)s - 0)
#warning configBSP430_HAL_%(INSTANCE)s_ISR requested without configBSP430_HAL_%(INSTANCE)s
#endif /* HAL_ISR and not HAL */
''',

    'hal_port_isr_decl' : '''/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_%(INSTANCE)s
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_%(INSTANCE)s is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_%(INSTANCE)s.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_%(INSTANCE)s_ISR
#define configBSP430_HAL_%(INSTANCE)s_ISR ((configBSP430_HAL_%(INSTANCE)s - 0) && defined(PORT%(#)s_VECTOR))
#endif /* configBSP430_HAL_%(INSTANCE)s_ISR */

#if (configBSP430_HAL_%(INSTANCE)s_ISR - 0) && ! (configBSP430_HAL_%(INSTANCE)s - 0)
#warning configBSP430_HAL_%(INSTANCE)s_ISR requested without configBSP430_HAL_%(INSTANCE)s
#endif /* HAL_ISR and not HAL */
''',

    'hal_usci_isr_decl' : '''/** Control inclusion of the @HAL receive interrupt handler for #BSP430_PERIPH_USCI_A%(INSTANCE)s and #BSP430_PERIPH_USCI_B%(INSTANCE)s
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when either or
 * both #configBSP430_HAL_USCI_A%(INSTANCE)s or
 * #configBSP430_HAL_USCI_B%(INSTANCE)s are set, but it may be
 * explicitly disabled if you intend to provide your own
 * implementation or will not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_USCI_A%(INSTANCE)s or #BSP430_HAL_USCI_B%(INSTANCE)s.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI_AB%(INSTANCE)sRX_ISR
#define configBSP430_HAL_USCI_AB%(INSTANCE)sRX_ISR ((configBSP430_HAL_USCI_A%(INSTANCE)s - 0) || (configBSP430_HAL_USCI_B%(INSTANCE)s - 0))
#endif /* configBSP430_HAL_USCI_AB%(INSTANCE)sRX_ISR */

#if ((configBSP430_HAL_%(INSTANCE)s_ISR - 0) \\
     && ! ((configBSP430_HAL_USCI_A%(INSTANCE)sRX - 0) | (configBSP430_HAL_USCI_B%(INSTANCE)s - 0)))
#warning configBSP430_HAL_USCI_AB%(INSTANCE)sRX_ISR requested without configBSP430_HAL_USCI_A%(INSTANCE)s or configBSP430_HAL_USCI_B%(INSTANCE)s
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the @HAL transmit interrupt handler for #BSP430_PERIPH_USCI_A%(INSTANCE)s and #BSP430_PERIPH_USCI_B%(INSTANCE)s
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when either or
 * both #configBSP430_HAL_USCI_A%(INSTANCE)s or
 * #configBSP430_HAL_USCI_B%(INSTANCE)s are set, but it may be
 * explicitly disabled if you intend to provide your own
 * implementation or will not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_USCI_A%(INSTANCE)s or #BSP430_HAL_USCI_B%(INSTANCE)s.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI_AB%(INSTANCE)sTX_ISR
#define configBSP430_HAL_USCI_AB%(INSTANCE)sTX_ISR ((configBSP430_HAL_USCI_A%(INSTANCE)s - 0) | (configBSP430_HAL_USCI_B%(INSTANCE)s - 0))
#endif /* configBSP430_HAL_USCI_AB%(INSTANCE)sTX_ISR */

#if ((configBSP430_HAL_%(INSTANCE)s_ISR - 0) \\
     && ! ((configBSP430_HAL_USCI_A%(INSTANCE)sTX - 0) | (configBSP430_HAL_USCI_B%(INSTANCE)s - 0)))
#warning configBSP430_HAL_USCI_AB%(INSTANCE)sTX_ISR requested without configBSP430_HAL_USCI_A%(INSTANCE)s or configBSP430_HAL_USCI_B%(INSTANCE)s
#endif /* HAL_ISR and not HAL */
''',

    'hal_timer_isr_decl' : '''/** Control inclusion of the secondary @HAL interrupt handler for #BSP430_PERIPH_%(INSTANCE)s
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is not enabled.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_%(INSTANCE)s.
 *
 * @cppflag
 * @nodefault
 * @ingroup grp_config_core
 * @dependency #configBSP430_HAL_%(INSTANCE)s */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_%(INSTANCE)s_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_%(INSTANCE)s_CC0_ISR - 0) && ! (configBSP430_HAL_%(INSTANCE)s - 0)
#warning configBSP430_HAL_%(INSTANCE)s_CC0_ISR requested without configBSP430_HAL_%(INSTANCE)s
#endif /* HAL_CC0_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_%(INSTANCE)s
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_%(INSTANCE)s is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_%(INSTANCE)s.
 *
 * @cppflag
 * @defaulted
 * @ingroup grp_config_core
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
BSP430_CORE_DECLARE_INTERRUPT(%(BASEINSTANCE)s_VECTOR)
isr_%(INSTANCE)s (void)
{
  int rv = %(periph)s_isr(BSP430_HAL_%(INSTANCE)s);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_%(INSTANCE)s_ISR */
''',

    'hal_timer_isr_defn' : '''#if configBSP430_HAL_T%(TYPE)s%(INSTANCE)s_CC0_ISR - 0
BSP430_CORE_DECLARE_INTERRUPT(TIMER%(INSTANCE)s_%(TYPE)s0_VECTOR)
isr_cc0_T%(TYPE)s%(INSTANCE)s (void)
{
  hBSP430hal%(PERIPH)s timer = BSP430_HAL_T%(TYPE)s%(INSTANCE)s;
  int rv = iBSP430callbackInvokeISRIndexed_ni(0 + timer->cc_cbchain_ni, timer, 0, 0);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_T%(TYPE)s%(INSTANCE)s_CC0_ISR */

#if configBSP430_HAL_T%(TYPE)s%(INSTANCE)s_ISR - 0
BSP430_CORE_DECLARE_INTERRUPT(TIMER%(INSTANCE)s_%(TYPE)s1_VECTOR)
isr_T%(TYPE)s%(INSTANCE)s (void)
{
  hBSP430hal%(PERIPH)s timer = BSP430_HAL_T%(TYPE)s%(INSTANCE)s;
  unsigned int iv = T%(TYPE)s%(INSTANCE)sIV;
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
BSP430_CORE_DECLARE_INTERRUPT(%(INSTANCE)s_VECTOR)
isr_%(INSTANCE)s (void)
{
  int idx = 0;
  int rv;
#if BSP430_CORE_FAMILY_IS_5XX - 0
  unsigned int iv = P%(#)sIV;

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

    'periph_want' : '''
#elif BSP430_PERIPH_CPPID_%(INSTANCE)s == BSP430_WANT_PERIPH_CPPID
#if (BSP430_WANT_CONFIG_HAL - 0)
#define configBSP430_HAL_%(INSTANCE)s 1
#endif /* HAL */
#if (BSP430_WANT_CONFIG_HPL - 0)
#define configBSP430_HPL_%(INSTANCE)s 1
#endif /* HPL */
#ifdef BSP430_WANT_CONFIG_HAL_ISR
#if (BSP430_WANT_CONFIG_HAL_ISR - 0)
#define configBSP430_HAL_%(INSTANCE)s_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_ISR */
#define configBSP430_HAL_%(INSTANCE)s_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_ISR */
#endif /* ISR */''',

    'periph_want_cc0_isr' : '''
#elif BSP430_PERIPH_CPPID_%(INSTANCE)s == BSP430_WANT_PERIPH_CPPID
#ifdef BSP430_WANT_CONFIG_HAL_CC0_ISR
#if (BSP430_WANT_CONFIG_HAL_CC0_ISR - 0)
#define configBSP430_HAL_%(INSTANCE)s_CC0_ISR 1
#else /* BSP430_WANT_CONFIG_HAL_CC0_ISR */
#define configBSP430_HAL_%(INSTANCE)s_CC0_ISR 0
#endif /* BSP430_WANT_CONFIG_HAL_CC0_ISR */
#endif /* CC0_ISR */''',

    'periph_sethandle' : '''
#elif BSP430_%(FUNCTIONAL)s_PERIPH_CPPID == BSP430_PERIPH_CPPID_%(INSTANCE)s
#define BSP430_%(FUNCTIONAL)s_PERIPH_HANDLE BSP430_PERIPH_%(INSTANCE)s''',
    }


# Generate a list of the core resource tags for all instances of core
# resource groups.
import itertools
# Ports
periph_ports = map(''.join, itertools.product(('PORT',), map(str, range(1, 12))))
# Timers
periph_timers = map(''.join, itertools.product(('TA',), map(str, range(4))))
periph_timers.extend(map(''.join, itertools.product(('TB',), map(str, range(3)))))
# Serial serial
periph_usci = map(''.join, itertools.product(('USCI_',), ('A', 'B'), ('0', '1')))
periph_usci5 = map(''.join, itertools.product(('USCI5_',), ('A', 'B'), map(str, range(4))))
periph_eusci = map(''.join, itertools.product(('EUSCI_',), ('A0', 'A1', 'A2', 'B0')))
periph_serial = []
periph_serial.extend(periph_usci)
periph_serial.extend(periph_usci5)
periph_serial.extend(periph_eusci)
    
periphs = []
periphs.extend(periph_ports)
periphs.extend(periph_timers)
periphs.extend(periph_serial)

instance_atmap = { 'periphs' : periphs,
                   'timers' : periph_timers,
                   'ports' : periph_ports,
                   'serial' : periph_serial,
                   'usci' : periph_usci,
                   'usci5' : periph_usci5,
                   'eusci' : periph_eusci }

# Generate an expansion that provides unique positive constants that
# uniquely identify each core resource instance.
periph_cppid = []
for i in xrange(len(periphs)):
    periph_cppid.append('''
/** Constant to identify #BSP430_PERIPH_%(periph)s in preprocessor checks */
#define BSP430_PERIPH_CPPID_%(periph)s %(ordinal)s''' % { 'periph': periphs[i], 'ordinal': str(i+1) })
templates['periph_cppid'] = '\n'.join(periph_cppid)

directive_re = re.compile('!BSP430!\s*(?P<keywords>.*)$')
# Directives:
#  insert=<name> : identify the template to be expanded
#  subst=<name,...> : populate subst_map from idmap entries for name,...

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

    instance_str = idmap.get('instance')
    if instance_str: # Non-empty value
        if instance_str.startswith('@'):
            instances = instance_atmap[instance_str[1:]]
        else:
            instances = instance_str.split(',')
        for i in instances:
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
    else:
        try:
            text.append(template % subst_map)
        except TypeError as e:
            print 'Exception expanding %s with %s: %s' % (insert_name, subst_map, e)
            print template
            raise
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
    idmap = {}
    while contents:
        line = contents.pop(0)
        if insertable is None:
            new_contents.append(line)
        mo = directive_re.search(line)
        if mo:
            kw = filter(lambda _s: 0 < _s.find('='), mo.group('keywords').strip().split())
         
            for (k, v) in [ _s.split('=', 1) for _s in kw ]:
                if 0 < k.find(':'):
                    (pfx, op) = k.split(':', 1)
                    if 'pp' == op:
                        (port, pin) = v.split('.', 1)
                        idmap['%s_port' % (pfx,) ] = 'PORT%s' % (port,)
                        idmap['%s_pin' % (pfx,) ] = 'BIT%s' % (pin,)
                        continue
                idmap[k] = v
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
