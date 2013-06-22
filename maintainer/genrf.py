import re
import os
import os.path

tag = 'CC2500'

rf_def_re = re.compile('^#define\s*BSP430_RF_(?P<tag>[^_]*)_(?P<tail>\S*)')

class SignalType (object):
    Registry = []
    _match_re = None
    template = None

    @classmethod
    def Match (cls, tail):
        for st in cls.Registry:
            signal = st.extractSignal(tail)
            if signal is not None:
                return (signal, st)
        return None

    def __init__ (self, idtype, template, customize=False):
        self.customize = customize
        self.template = template
        self._idtype = idtype
        self.template += '#define BSP430_RF_%(tag)s_%(signal)s_%(idtype)s include <bsp430/platform.h>'
        self._match_re = re.compile('^(?P<signal>\S*)_%s' % (idtype,))
        self.Registry.append(self)

    def extractSignal (self, text):
        mo = self._match_re.match(text)
        if mo is None:
            return None
        return mo.group('signal')

    def expand (self, tag, signal):
        return self.template % { 'tag': tag, 'signal': signal, 'idtype': self._idtype }

SignalType('PORT_PERIPH_HANDLE', '''
/** BSP430 peripheral handle for port on which %(signal)s is placed.
 *
 * The %(signal)s signal is ***FILL IN HERE***
 */
''', customize=True)
SignalType('PORT_PERIPH_CPPID', '''
/** Preprocessor-compatible identifier corresponding to #BSP430_RF_%(tag)s_%(signal)s_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
''')
SignalType('PORT_BIT', '''
/** Port bit on #BSP430_RF_%(tag)s_%(signal)s_PORT_PERIPH_HANDLE for %(signal)s.
 *
 * This is the bit mask, not a bit position.
 */
''')
SignalType('TIMER_PERIPH_HANDLE', '''
/** BSP430 peripheral handle for timer connected to %(signal)s.
 *
 * This macro is defined only if %(signal)s is connected to a port that
 * supports capture of a timer counter based on signal changes.
 */
''')
SignalType('TIMER_PERIPH_CPPID', '''
/** Preprocessor-compatible identifier corresponding to #BSP430_RF_%(tag)s_%(signal)s_TIMER_PERIPH_HANDLE.
 *
 * This macro is defined only if %(signal)s is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
''')
SignalType('TIMER_CCIDX', '''
/** The capture/compare index on #BSP430_RF_%(tag)s_%(signal)s_TIMER_PERIPH_HANDLE
 * corresponding to %(signal)s.
 *
 * This macro is defined only if %(signal)s is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_%(tag)s_%(signal)s_TIMER_CCIS
 */
''')
SignalType('TIMER_CCIS', '''
/** The capture/compare input selector necessary to capture any signal
 * changes associated with %(tag)s %(signal)s.
 *
 * This macro is defined only if %(signal)s is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_%(tag)s_%(signal)s_TIMER_CCIDX
 */
''')

emksignals = {}

for fn in ( 'emk_bsp430_config.h', 'emk_platform.h' ):
    for ln in file(os.path.join(os.environ['BSP430_ROOT'], 'include', 'bsp430', 'rf', fn)):
        mo = rf_def_re.match(ln)
        if mo is None:
            continue
        (tag, tail) = mo.groups()
        signals = emksignals.setdefault(tag, {})
        pair = SignalType.Match(tail)
        if pair is None:
            continue
        (signal, sigtype) = pair
        signals.setdefault(signal, []).append(sigtype)

for (tag, signals) in emksignals.iteritems():

    customized = []
    defaulted = ['']
    for s in sorted(signals.keys()):
        for st in signals[s]:
            exp = st.expand(tag, s)
            if st.customize:
                customized.append(exp)
            else:
                defaulted.append(exp)
    outf = file('%s.decl' % (tag.lower(),), 'w')
    outf.write('\n'.join(customized))
    outf.write('\n/* *** END OF CUSTOMIZED DECLARATIONS */\n')
    outf.write('\n'.join(defaulted))


