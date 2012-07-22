import sys
import re
import os.path

templates = {
    'hpl_ba_decl' : '''/** @def configBSP430_PERIPH_%(INSTANCE)s
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c %(INSTANCE)s peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_%(INSTANCE)s__) */
#ifndef configBSP430_PERIPH_%(INSTANCE)s
#define configBSP430_PERIPH_%(INSTANCE)s 0
#endif /* configBSP430_PERIPH_%(INSTANCE)s */

/** @def configBSP430_PERIPH_%(INSTANCE)s_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c %(INSTANCE)s.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_%(PERIPH)s_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_%(INSTANCE)s_ISR
#define configBSP430_PERIPH_%(INSTANCE)s_ISR 1
#endif /* configBSP430_PERIPH_%(INSTANCE)s_ISR */

/** Handle for the raw %(INSTANCE)s device.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_%(INSTANCE)s is defined to a true
 * value. */
#define BSP430_PERIPH_%(INSTANCE)s ((xBSP430periphHandle)(__MSP430_BASEADDRESS_%(INSTANCE)s__))
''',
    
    'hal_decl' : '''/** FreeRTOS HAL handle for %(INSTANCE)s.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_%(INSTANCE)s is defined to a true
 * value. */
extern xBSP430%(periph)sHandle const xBSP430%(periph)s_%(INSTANCE)s;
''',

    'hpl_ba_defn' : '''#if configBSP430_PERIPH_%(INSTANCE)s - 0
static struct xBSP430%(periph)sState state_%(INSTANCE)s_ = {
	.%(periph)s = (xBSP430periph%(PERIPH)s *)__MSP430_BASEADDRESS_%(INSTANCE)s__
};

xBSP430%(periph)sHandle const xBSP430%(periph)s_%(INSTANCE)s = &state_%(INSTANCE)s_;

#if configBSP430_PERIPH_%(INSTANCE)s_ISR - 0
#if ! (configBSP430_%(PERIPH)s_SHARE_ISR - 0)
#error Shared periphal HAL ISR disabled
#endif /* configBSP430_%(PERIPH)s_SHARE_ISR */
static void
__attribute__((__interrupt__(%(INSTANCE)s_VECTOR)))
isr_%(INSTANCE)s (void)
{
	%(periph)s_isr(xBSP430%(periph)s_%(INSTANCE)s);
}
#endif /* configBSP430_%(PERIPH)s_%(INSTANCE)s_ISR */

#endif /* configBSP430_PERIPH_%(INSTANCE)s */
''',

    'hpl_hal_demux' : '''#if configBSP430_PERIPH_%(INSTANCE)s - 0
	if (BSP430_PERIPH_%(INSTANCE)s == periph) {
		return xBSP430%(periph)s_%(INSTANCE)s;
	}
#endif /* configBSP430_PERIPH_%(INSTANCE)s */
''',

    'hpl_port_5xx' : '''
/** @cond DOXYGEN_INTERNAL */
#if defined(__MSP430_HAS_%(PORTA)s_R__)
#define _BSP430_PERIPH_%(PORT1)s_BASEADDRESS __MSP430_BASEADDRESS_%(PORTA)s_R__
#define _BSP430_PERIPH_%(PORT2)s_BASEADDRESS (1+__MSP430_BASEADDRESS_%(PORTA)s_R__)
#else /* %(PORTA)s_R */
#define _BSP430_PERIPH_%(PORT1)s_BASEADDRESS __MSP430_BASEADDRESS_%(PORTA)s__
#define _BSP430_PERIPH_%(PORT2)s_BASEADDRESS (1+__MSP430_BASEADDRESS_%(PORTA)s__)
#endif /* %(PORTA)s_R */
/** @endcond */ /* DOXYGEN_INTERNAL */

/** Handle for the raw %(PORT1)s device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_%(PORT1)s
 * is defined to a true value. */
#define BSP430_PERIPH_%(PORT1)s ((xBSP430periphHandle)(_BSP430_PERIPH_%(PORT1)s_BASEADDRESS))

/** Handle for the raw %(PORT2)s device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_%(PORT2)s
 * is defined to a true value. */
#define BSP430_PERIPH_%(PORT2)s ((xBSP430periphHandle)(_BSP430_PERIPH_%(PORT2)s_BASEADDRESS))

/** Handle for the raw %(PORTA)s device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_%(PORTA)s
 * is defined to a true value. */
#define BSP430_PERIPH_%(PORTA)s ((xBSP430periphHandle)(1 + _BSP430_PERIPH_%(PORT1)s_BASEADDRESS))

/** @def configBSP430_PERIPH_%(PORT1)s
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c %(PORT1)s peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_%(PORT1)s__ or
 * @c __MSP430_HAS_%(PORT1)s_R__) */
#ifndef configBSP430_PERIPH_%(PORT1)s
#define configBSP430_PERIPH_%(PORT1)s 0
#endif /* configBSP430_PERIPH_%(PORT1)s */

/** @def configBSP430_PERIPH_%(PORT1)s_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c %(PORT1)s.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_PORT_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_%(PORT1)s_ISR
#define configBSP430_PERIPH_%(PORT1)s_ISR 0
#endif /* configBSP430_PERIPH_%(PORT1)s_ISR */

/** @def configBSP430_PERIPH_%(PORT2)s
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c %(PORT2)s peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_%(PORT2)s__ or
 * @c __MSP430_HAS_%(PORT2)s_R__) */
#ifndef configBSP430_PERIPH_%(PORT2)s
#define configBSP430_PERIPH_%(PORT2)s 0
#endif /* configBSP430_PERIPH_%(PORT2)s */

/** @def configBSP430_PERIPH_%(PORT2)s_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c %(PORT2)s.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_PORT_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_%(PORT2)s_ISR
#define configBSP430_PERIPH_%(PORT2)s_ISR 0
#endif /* configBSP430_PERIPH_%(PORT2)s_ISR */
''',

    'hal_port' : '''/** FreeRTOS HAL handle for %(INSTANCE)s.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_%(INSTANCE)s is defined to a true
 * value. */

extern xBSP430portHandle const xBSP430port_%(INSTANCE)s;
''',

    'hal_port_5xx_defn' : '''#if configBSP430_PERIPH_%(INSTANCE)s - 0
static struct xBSP430port5xxState state_%(INSTANCE)s = {
		.port = (volatile xBSP430periphPORT_5XX_8 *)_BSP430_PERIPH_%(INSTANCE)s_BASEADDRESS,
	};
xBSP430portHandle const xBSP430port_%(INSTANCE)s = &state_%(INSTANCE)s;
#endif /* configBSP430_PERIPH_%(INSTANCE)s */
''',

    'hal_port_5xx_isr_defn' : '''#if configBSP430_PERIPH_%(INSTANCE)s_ISR - 0
static void
__attribute__((__interrupt__(%(INSTANCE)s_VECTOR)))
isr_%(INSTANCE)s (void)
{
	int rv = port_isr(xBSP430port_%(INSTANCE)s, P%(#)sIV);
	__bic_status_register_on_exit(rv & BSP430_PORT_ISR_BIC_MASK);
	portYIELD_FROM_ISR((rv & BSP430_PORT_ISR_YIELD) ? pdTRUE : pdFALSE);
}
#endif /* configBSP430_PERIPH_%(INSTANCE)s_ISR */
''',

    'hpl_timer' : '''/** @def configBSP430_PERIPH_%(INSTANCE)s
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c %(INSTANCE)s peripheral HPL or HAL interface.  If the MCU
 * does not support this timer, a compile-time error will probably
 * be produced. */
#ifndef configBSP430_PERIPH_%(INSTANCE)s
#define configBSP430_PERIPH_%(INSTANCE)s 0
#endif /* configBSP430_PERIPH_%(INSTANCE)s */

/** @def configBSP430_PERIPH_%(INSTANCE)s_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c %(INSTANCE)s.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_%(PERIPH)s_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_%(INSTANCE)s_ISR
#define configBSP430_PERIPH_%(INSTANCE)s_ISR 1
#endif /* configBSP430_PERIPH_%(INSTANCE)s_ISR */

/** Handle for the raw %(INSTANCE)s device.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_%(INSTANCE)s is defined to a true
 * value. */
#define BSP430_PERIPH_%(INSTANCE)s ((xBSP430periphHandle)(_BSP430_PERIPH_%(INSTANCE)s_BASEADDRESS))

/** Pointer to the peripheral register map for %(INSTANCE)s.
 */
extern volatile xBSP430periphTIMER * const xBSP430periph_%(INSTANCE)s;
''',

    'periph_ba_hpl_demux' : '''#if configBSP430_PERIPH_%(INSTANCE)s - 0
	if (BSP430_PERIPH_%(INSTANCE)s == periph) {
		return xBSP430periph_%(INSTANCE)s;
	}
#endif /* configBSP430_PERIPH_%(INSTANCE)s */
''',

    'periph_ba_hpl_defn' : '''#if configBSP430_PERIPH_%(INSTANCE)s - 0
volatile xBSP430periph%(PERIPH)s * const xBSP430periph_%(INSTANCE)s = (volatile xBSP430periph%(PERIPH)s *)_BSP430_PERIPH_%(INSTANCE)s_BASEADDRESS;
#endif /* configBSP430_PERIPH_%(INSTANCE)s */
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
    text = []
    text.append('/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [%(template)s] */' % subst_map)

    for i in idmap['instance'].split(','):
        if i.startswith(periph.upper()):
            subst_map.update({ '#' : i[len(periph):] })
        subst_map.update({ 'instance': i.lower(), 'INSTANCE': i.upper() })
        if idmap.get('portexpand', False):
            subst_map.update({ 'PORTA' : 'PORT' + i[0],
                               'PORT1': 'PORT%d' % (1 + 2 * (ord(i[0]) - ord('A')),),
                               'PORT2': 'PORT%d' % (2 + 2 * (ord(i[0]) - ord('A')),) })
        text.append(template % subst_map)
        subst_map.pop('#', None)
    text.append('/* END AUTOMATICALLY GENERATED CODE [%(template)s] */' % subst_map)
    text.append('')
    return '\n'.join(text)

for fn in sys.argv[1:]:
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
