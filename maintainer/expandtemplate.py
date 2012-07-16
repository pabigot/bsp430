import sys
import re
import os.path

templates = {
    'hpl_ba_decl' : '''/** @def configBSP430_PERIPH_USE_%(INSTANCE)s
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * %(INSTANCE)s peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check __MSP430_HAS_%(INSTANCE)s__) */
#ifndef configBSP430_PERIPH_USE_%(INSTANCE)s
#define configBSP430_PERIPH_USE_%(INSTANCE)s 0
#endif /* configBSP430_PERIPH_USE_%(INSTANCE)s */

/** @def configBSP430_%(PERIPH)s_NO_%(INSTANCE)s_IRQ
 *
 * Define to a true value in @c FreeRTOSConfig.h to prevent BSP430 from
 * defining the interrupt vector for %(INSTANCE)s.  Do this if you need
 * complete control over how interrupts are handled for the device. */
#ifndef configBSP430_%(PERIPH)s_NO_%(INSTANCE)s_IRQ
#define configBSP430_%(PERIPH)s_NO_%(INSTANCE)s_IRQ 0
#endif /* configBSP430_%(PERIPH)s_NO_%(INSTANCE)s_IRQ */

/** Handle for the raw %(INSTANCE)s device.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_USE_%(INSTANCE)s is defined to a true
 * value. */
#define BSP430_PERIPH_%(INSTANCE)s ((xBSP430periphHandle)(__MSP430_BASEADDRESS_%(INSTANCE)s__))
''',
    
    'hal_decl' : '''/** FreeRTOS HAL handle for %(INSTANCE)s.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_USE_%(INSTANCE)s is defined to a true
 * value. */
extern xBSP430%(periph)sHandle const xBSP430%(periph)s_%(INSTANCE)s;
''',

    'hpl_ba_defn' : '''#if configBSP430_PERIPH_USE_%(INSTANCE)s - 0
static struct xBSP430%(periph)sState state_%(INSTANCE)s_ = {
	.%(periph)s = (xBSP430periph%(PERIPH)s *)__MSP430_BASEADDRESS_%(INSTANCE)s__
};

xBSP430%(periph)sHandle const xBSP430%(periph)s_%(INSTANCE)s = &state_%(INSTANCE)s_;

#if ! (configBSP430_%(PERIPH)s_NO_%(INSTANCE)s_IRQ - 0)
static void
__attribute__((__interrupt__(%(INSTANCE)s_VECTOR)))
irq_%(INSTANCE)s (void)
{
	%(periph)s_irq(xBSP430%(periph)s_%(INSTANCE)s);
}
#endif /* ! configBSP430_%(PERIPH)s_NO_%(INSTANCE)s_IRQ */

#endif /* configBSP430_PERIPH_USE_%(INSTANCE)s */
''',

    'hpl_hal_demux' : '''#if configBSP430_PERIPH_USE_%(INSTANCE)s - 0
	if (BSP430_PERIPH_%(INSTANCE)s == periph) {
		return xBSP430%(periph)s_%(INSTANCE)s;
	}
#endif /* configBSP430_PERIPH_USE_%(INSTANCE)s */
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
        subst_map.update({ 'instance': i.lower(), 'INSTANCE': i.upper() })
        text.append(template % subst_map)
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
