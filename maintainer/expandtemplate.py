import sys
import re
import os.path
import argparse

templates = {
    'hal_decl' : '''/** @def configBSP430_HAL_%(INSTANCE)s
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c %(INSTANCE)s peripheral HAL interface.  This defines a global
 * object xBSP430%(periph)sHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_%(INSTANCE)s to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_%(INSTANCE)s
#define configBSP430_HAL_%(INSTANCE)s 0
#endif /* configBSP430_HAL_%(INSTANCE)s */

/** BSP430 HAL handle for %(INSTANCE)s.
 *
 * The handle may be used only if #configBSP430_PERIPH_%(INSTANCE)s
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_%(INSTANCE)s - 0)
extern xBSP430%(periph)sHandle const xBSP430%(periph)s_%(INSTANCE)s;
#endif /* configBSP430_HAL_%(INSTANCE)s */
''',

    'hpl_ba_decl' : '''/** @def configBSP430_PERIPH_%(INSTANCE)s
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c %(INSTANCE)s peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_%(INSTANCE)s defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_%(INSTANCE)s
#define configBSP430_PERIPH_%(INSTANCE)s (configBSP430_HAL_%(INSTANCE)s - 0)
#endif /* configBSP430_PERIPH_%(INSTANCE)s */

#if (configBSP430_HAL_%(INSTANCE)s - 0) && ! (configBSP430_PERIPH_%(INSTANCE)s - 0)
#warning configBSP430_HAL_%(INSTANCE)s requested without configBSP430_PERIPH_%(INSTANCE)s
#endif /* HAL and not HPL */

/** Handle for the raw %(INSTANCE)s device.
 *
 * The handle may be used only if #configBSP430_PERIPH_%(INSTANCE)s
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_%(INSTANCE)s - 0)
#define BSP430_PERIPH_%(INSTANCE)s ((xBSP430periphHandle)(_BSP430_PERIPH_%(INSTANCE)s_BASEADDRESS))
#endif /* configBSP430_PERIPH_%(INSTANCE)s */

/** Pointer to the peripheral register map for %(INSTANCE)s.
 *
 * The pointer may be used only if #configBSP430_PERIPH_%(INSTANCE)s
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_%(INSTANCE)s - 0)
static volatile xBSP430periph%(PERIPH)s * const xBSP430periph_%(INSTANCE)s = (volatile xBSP430periph%(PERIPH)s *)_BSP430_PERIPH_%(INSTANCE)s_BASEADDRESS;
#endif /* configBSP430_PERIPH_%(INSTANCE)s */
''',
    
    'hal_isr_decl' : '''/** @def configBSP430_HAL_%(INSTANCE)s_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c %(INSTANCE)s, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_%(INSTANCE)s defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_%(INSTANCE)s
 * also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_%(INSTANCE)s_ISR
#define configBSP430_HAL_%(INSTANCE)s_ISR (configBSP430_HAL_%(INSTANCE)s - 0)
#endif /* configBSP430_HAL_%(INSTANCE)s_ISR */

#if (configBSP430_HAL_%(INSTANCE)s_ISR - 0) && ! (configBSP430_HAL_%(INSTANCE)s - 0)
#warning configBSP430_HAL_%(INSTANCE)s_ISR requested without configBSP430_HAL_%(INSTANCE)s
#endif /* HAL_ISR and not HAL */
''',

    'hal_usci_isr_decl' : '''/** @def configBSP430_HAL_USCIAB%(INSTANCE)sRX_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c USCI_A%(INSTANCE)s or @c
 * USCI_B%(INSTANCE)s, but want to define your own interrupt service
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
 * @defaulted */
#ifndef configBSP430_HAL_USCIAB%(INSTANCE)sRX_ISR
#define configBSP430_HAL_USCIAB%(INSTANCE)sRX_ISR ((configBSP430_HAL_USCI_A%(INSTANCE)s - 0) | (configBSP430_HAL_USCI_B%(INSTANCE)s - 0))
#endif /* configBSP430_HAL_USCIAB%(INSTANCE)sRX_ISR */

#if ((configBSP430_HAL_%(INSTANCE)s_ISR - 0) \\
     && ! ((configBSP430_HAL_USCI_A%(INSTANCE)sRX - 0) | (configBSP430_HAL_USCI_B%(INSTANCE)s - 0)))
#warning configBSP430_HAL_USCIAB%(INSTANCE)sRX_ISR requested without configBSP430_HAL_USCI_A%(INSTANCE)s or configBSP430_HAL_USCI_B%(INSTANCE)s
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_USCIAB%(INSTANCE)sTX_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c USCI_A%(INSTANCE)s or @c
 * USCI_B%(INSTANCE)s, but want to define your own interrupt service
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
 * @defaulted */
#ifndef configBSP430_HAL_USCIAB%(INSTANCE)sTX_ISR
#define configBSP430_HAL_USCIAB%(INSTANCE)sTX_ISR ((configBSP430_HAL_USCI_A%(INSTANCE)s - 0) | (configBSP430_HAL_USCI_B%(INSTANCE)s - 0))
#endif /* configBSP430_HAL_USCIAB%(INSTANCE)sTX_ISR */

#if ((configBSP430_HAL_%(INSTANCE)s_ISR - 0) \\
     && ! ((configBSP430_HAL_USCI_A%(INSTANCE)sTX - 0) | (configBSP430_HAL_USCI_B%(INSTANCE)s - 0)))
#warning configBSP430_HAL_USCIAB%(INSTANCE)sTX_ISR requested without configBSP430_HAL_USCI_A%(INSTANCE)s or configBSP430_HAL_USCI_B%(INSTANCE)s
#endif /* HAL_ISR and not HAL */
''',

    'hal_timer_isr_decl' : '''/** @def configBSP430_HAL_%(INSTANCE)s_CC0_ISR
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c %(INSTANCE)s, but want to define your
 * own interrupt service routine for CC0 interrupts.
 *
 * Enabling #configBSP430_HAL_%(INSTANCE)s defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_%(INSTANCE)s
 * also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_%(INSTANCE)s_CC0_ISR
#define configBSP430_HAL_%(INSTANCE)s_CC0_ISR (configBSP430_HAL_%(INSTANCE)s - 0)
#endif /* configBSP430_HAL_%(INSTANCE)s_CC0_ISR */

#if (configBSP430_HAL_%(INSTANCE)s_CC0_ISR - 0) && ! (configBSP430_HAL_%(INSTANCE)s - 0)
#warning configBSP430_HAL_%(INSTANCE)s_CC0_ISR requested without configBSP430_HAL_%(INSTANCE)s
#endif /* HAL_CC0_ISR and not HAL */

/** @def configBSP430_HAL_%(INSTANCE)s_ISR
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c %(INSTANCE)s, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_%(INSTANCE)s defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_%(INSTANCE)s
 * also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_%(INSTANCE)s_ISR
#define configBSP430_HAL_%(INSTANCE)s_ISR (configBSP430_HAL_%(INSTANCE)s - 0)
#endif /* configBSP430_HAL_%(INSTANCE)s_ISR */

#if (configBSP430_HAL_%(INSTANCE)s_ISR - 0) && ! (configBSP430_HAL_%(INSTANCE)s - 0)
#warning configBSP430_HAL_%(INSTANCE)s_ISR requested without configBSP430_HAL_%(INSTANCE)s
#endif /* HAL_ISR and not HAL */
''',

    'hal_ba_defn' : '''#if configBSP430_HAL_%(INSTANCE)s - 0
static struct xBSP430%(periph)sState state_%(INSTANCE)s_ = {
  .%(periph)s = (xBSP430periph%(PERIPH)s *)_BSP430_PERIPH_%(INSTANCE)s_BASEADDRESS
};

xBSP430%(periph)sHandle const xBSP430%(periph)s_%(INSTANCE)s = &state_%(INSTANCE)s_;
#endif /* configBSP430_HAL_%(INSTANCE)s */
''',

    'hal_isr_defn' : '''#if configBSP430_HAL_%(INSTANCE)s_ISR - 0
static void
__attribute__((__interrupt__(%(BASEINSTANCE)s_VECTOR)))
isr_%(INSTANCE)s (void)
{
  int rv = %(periph)s_isr(xBSP430%(periph)s_%(INSTANCE)s);
  BSP430_PERIPH_ISR_CALLBACK_TAIL(rv);
}
#endif /* configBSP430_HAL_%(INSTANCE)s_ISR */
''',

    'hal_timer_isr_defn' : '''#if configBSP430_HAL_T%(TYPE)s%(INSTANCE)s_CC0_ISR - 0
static void
__attribute__((__interrupt__(TIMER%(INSTANCE)s_%(TYPE)s0_VECTOR)))
isr_cc0_T%(TYPE)s%(INSTANCE)s (void)
{
  xBSP430%(periph)sHandle timer = xBSP430%(periph)s_T%(TYPE)s%(INSTANCE)s;
  int rv = iBSP430callbackInvokeISRVoid_ni(&timer->cc0_callback, timer, 0);
  BSP430_PERIPH_ISR_CALLBACK_TAIL(rv);
}
#endif /* configBSP430_HAL_T%(TYPE)s%(INSTANCE)s_CC0_ISR */

#if configBSP430_HAL_T%(TYPE)s%(INSTANCE)s_ISR - 0
static void
__attribute__((__interrupt__(TIMER%(INSTANCE)s_%(TYPE)s1_VECTOR)))
isr_T%(TYPE)s%(INSTANCE)s (void)
{
  xBSP430%(periph)sHandle timer = xBSP430%(periph)s_T%(TYPE)s%(INSTANCE)s;
  int iv = T%(TYPE)s%(INSTANCE)sIV;
  int rv = 0;
  if (0 != iv) {
    if (T%(TYPE)s_OVERFLOW == iv) {
      ++timer->overflow_count;
      rv = iBSP430callbackInvokeISRVoid_ni(&timer->overflow_callback, timer, rv);
    } else {
      int cc = (iv - 4) / 2;
      rv = iBSP430callbackInvokeISRIndexed_ni(cc + timer->cc_callback, timer, 1+cc, rv);
    }
  }
  BSP430_PERIPH_ISR_CALLBACK_TAIL(rv);
}
#endif /* configBSP430_HAL_T%(TYPE)s%(INSTANCE)s_ISR */
''',
    
    'periph_hal_demux' : '''#if configBSP430_HAL_%(INSTANCE)s - 0
  if (BSP430_PERIPH_%(INSTANCE)s == periph) {
    return xBSP430%(periph)s_%(INSTANCE)s;
  }
#endif /* configBSP430_PERIPH_%(INSTANCE)s */
''',

    'hal_port_5xx_defn' : '''#if configBSP430_HAL_%(INSTANCE)s - 0
static struct xBSP430portState state_%(INSTANCE)s = {
  .port = (volatile xBSP430periphPORTIE *)_BSP430_PERIPH_%(INSTANCE)s_BASEADDRESS,
};
xBSP430portHandle const xBSP430port_%(INSTANCE)s = &state_%(INSTANCE)s;
#endif /* configBSP430_PERIPH_%(INSTANCE)s */
''',

    'hal_port_5xx_isr_defn' : '''#if configBSP430_HAL_%(INSTANCE)s_ISR - 0
static void
__attribute__((__interrupt__(%(INSTANCE)s_VECTOR)))
isr_%(INSTANCE)s (void)
{
  int rv = port_isr(xBSP430port_%(INSTANCE)s, P%(#)sIV);
  BSP430_PERIPH_ISR_CALLBACK_TAIL(rv);
}
#endif /* configBSP430_HAL_%(INSTANCE)s_ISR */
''',

    'periph_ba_hpl_demux' : '''#if configBSP430_PERIPH_%(INSTANCE)s - 0
  if (BSP430_PERIPH_%(INSTANCE)s == periph) {
    return xBSP430periph_%(INSTANCE)s;
  }
#endif /* configBSP430_PERIPH_%(INSTANCE)s */
''',

    'platform_decl' : '''/** @def BSP430_PLATFORM_%(INSTANCE)s
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/msp-$(instance)s">MSP-%(INSTANCE)s</a> platform.
 * This causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/%(instance)s.h>.
 * If you include that header directly, #BSP430_PLATFORM_%(INSTANCE)s will be
 * defined for you.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_%(INSTANCE)s
#define BSP430_PLATFORM_%(INSTANCE)s 0
#endif /* BSP430_PLATFORM_%(INSTANCE)s */

#if defined(BSP430_DOXYGEN) || (BSP430_PLATFORM_%(INSTANCE)s - 0)
#include <bsp430/platform/%(instance)s.h>
#endif /* BSP430_PLATFORM_%(INSTANCE)s */
''',

    'platform_bsp430_config' : '''#if defined(BSP430_DOXYGEN) || (BSP430_PLATFORM_%(INSTANCE)s - 0)
#include <bsp430/platform/%(instance)s_bsp430_config.h>
#endif /* BSP430_PLATFORM_%(INSTANCE)s */
''',

    }

directive_re = re.compile('!BSP430!\s*(?P<keywords>.*)$')
idmap = {}

def expandTemplate (tplname, idmap):
    global templates

    template = templates[tplname]
    periph = idmap['periph']
    subst_map = { 'template' : tplname,
                  'periph': periph.lower(),
                  'PERIPH': periph.upper() }
    subst_keys = idmap.get('subst')
    if subst_keys is not None:
        for sm in subst_keys.split(','):
            subst_map[sm] = idmap[sm]
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

    (path, filename) = os.path.split(fn)
    bfn = os.path.join(path, filename + '.bak')
    try:
        os.unlink(bfn)
    except OSError:
        pass
    os.rename(fn, bfn)
    file(fn, 'w').write(''.join(new_contents))
