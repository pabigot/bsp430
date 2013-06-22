import re

class PinmapBase (object):
    # A dictionary used for template expansion.  The contents are the
    # named subclass constructor arguments plus whatever derived
    # information the subclass adds during construction.
    _dict = {}

    # A compiled regular expression that extracts from a standard peripheral description
    Template_re = None

    @classmethod
    def Create (cls, id):
        mo = cls.Template_re.match(id)
        if mo is None:
            return None
        instance = cls(*mo.groups())
        instance._raw = id
        if instance._dict == cls._dict:
            instance._dict = cls._dict.copy()
        instance._dict.update(map(lambda _t,_m=mo: (_t[0], _m.group(_t[1])), cls.Template_re.groupindex.iteritems()))
        return instance

    def __cmp__ (self, other):
        return cmp(hash(self), hash(other))

class RFEMPin (PinmapBase):
    Template_re = re.compile('RF(?P<hdr>\d+)\.(?P<pin>\d+)$')

    def __hash__ (self):
        return hash(self.astuple())

    def astuple (self):
        return (self.hdr, self.pin)

    def prefix (self):
        return 'RFEM_RF%uP%u' % self.astuple()

    def __init__ (self, hdr, pin):
        self.hdr = int(hdr)
        self.pin = int(pin)

    def __cmp__ (self, other):
        return cmp(self.hdr, other.hdr) or cmp(self.pin, other.pin)

class PeripheralBase (PinmapBase):
    @classmethod
    def ProcessDirective (cls, ln):
        (tag, val) = ln.split()
        setattr(cls, tag, val)

    _ConfigTemplate = None
    _PlatformTemplate = None

    def template (self, is_config):
        if is_config:
            return self._ConfigTemplate
        return self._PlatformTemplate

    def expandTemplate (self, tag, is_config):
        tmap = self._dict.copy()
        tmap['tag'] = tag
        return self.template(is_config) % tmap

class Timer (PeripheralBase):

    Template_re = re.compile('T(?P<variant>.)(?P<instance>\d+)\.(?P<ccidx>\d+)(?P<ccisc>[AB])?$')

    def __init__ (self, variant, instance, ccidx, ccisc):
        self.variant = variant
        self.instance = int(instance)
        self.ccidx = int(ccidx)
        if 'A' == ccisc:
            self.ccis = 'CCIS_0'
        elif 'B' == ccisc:
            self.ccis = 'CCIS_1'
        else:
            self.ccis = None
        self._dict = { 'ccis': self.ccis,
                       'periph' : self.periph() }

    def __str__ (self):
        return 'T%s%u.%u' % (self.variant, self.instance, self.ccidx)

    def periph (self):
        return 'T%s%s' % (self.variant, self.instance)

    _ConfigTemplate = '#define BSP430_%(tag)s_TIMER_PERIPH_CPPID BSP430_PERIPH_CPPID_%(periph)s'

    def template (self, is_config):
        if is_config:
            return self._ConfigTemplate
        rv = [ '#define BSP430_%(tag)s_TIMER_PERIPH_HANDLE BSP430_PERIPH_%(periph)s',
               '#define BSP430_%(tag)s_TIMER_CCIDX %(ccidx)s' ]
        if self.ccis is not None:
            rv.append('#define BSP430_%(tag)s_TIMER_CCIS %(ccis)s')
        return '\n'.join(rv)

    @classmethod
    def TemplateSuffixes (cls, is_config):
        if is_config:
            return ('_TIMER_PERIPH_CPPID',)
        return ( '_TIMER_PERIPH_HANDLE', '_TIMER_CCIDX', '_TIMER_CCIS' )

class Port (PeripheralBase):
    Template_re = re.compile('P(?P<port>\d+)\.(?P<pin>\d)$')

    def __init__ (self, port, pin):
        self.port = int(port)
        self.pin = int(pin)

    def __hash__ (self):
        return hash(self.astuple())

    def astuple (self):
        return (self.port, self.pin)

    def periph (self):
        return 'PORT%u' % (self.port,)

    def __str__ (self):
        return 'P%u.%u' % (self.port, self.pin)

    @classmethod
    def TemplateSuffixes (self, is_config):
        if is_config:
            return ( '_PORT_PERIPH_CPPID', )
        return ( '_PORT_PERIPH_HANDLE', '_PORT_BIT' )

    _ConfigTemplate = '#define BSP430_%(tag)s_PORT_PERIPH_CPPID BSP430_PERIPH_CPPID_PORT%(port)s'
    _PlatformTemplate = '''#define BSP430_%(tag)s_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT%(port)s
#define BSP430_%(tag)s_PORT_BIT BIT%(pin)s'''

class Serial (PeripheralBase):
    Template_re = re.compile('UC(?P<variant>.)(?P<instance>\d)(?P<role>\w+)$')

    Periph = None

    def __init__ (self, variant, instance, role):
        self.variant = variant
        self.instance = int(instance)
        self.role = role
        self._dict = { 'periph': self.periph() }

    def __str__ (self):
        return 'UC%s%u%s' % (self.variant, self.instance, self.role)

    def periph (self):
        return '%s_%s%s' % (self.Periph, self.variant, self.instance)

    _ConfigTemplate = '#define BSP430_%(tag)s_PERIPH_CPPID BSP430_PERIPH_CPPID_%(periph)s'
    _PlatformTemplate = '#define BSP430_%(tag)s_PERIPH_HANDLE BSP430_PERIPH_%(periph)s'

def GenerateLines (path):
    directive_re = re.compile('@(?P<cls>\w*)\.(?P<dir>.*)$')
    inf = file(path)
    for ln in inf:
        ln = ln[:ln.find('#')].strip()
        if ln:
            mo = directive_re.match(ln)
            if mo is not None:
                cls = globals()[mo.group('cls')]
                cls.ProcessDirective(mo.group('dir'))
                continue
            yield ln

class Signal (object):
    withPort = True
    withTimer = False

    def __init__ (self, signal, short_description, with_port=True, with_timer=False, port=None, timer=None):
        self.signal = signal
        self.shortDescription = short_description
        self.withPort = with_port
        self.withTimer = with_timer
        self.port = port
        self.timer = timer

    def defines (self, tag, srctag=None):
        tag = 'BSP430_%s_%s' % (tag, self.signal)
        kw = { 'tag': tag,
               'short': self.shortDescription }
        defs = []
        to = []
        frm = []
        check = []
        if self.withPort:
            kw['port_base'] = '%s_PORT_PERIPH_HANDLE' % (tag,)
            to.append (('%s_PORT_PERIPH_CPPID' % (tag,),
                        '%s_PORT_PERIPH_HANDLE' % (tag,),
                        '%s_PORT_BIT' % (tag,)))
            if self.port is None:
                # Define relative to RFEM
                check.append('%s_PORT_PERIPH_CPPID' % (srctag,))
                frm.append(('%s_PORT_PERIPH_CPPID' % (srctag,),
                             '%s_PORT_PERIPH_HANDLE' % (srctag,),
                             '%s_PORT_BIT' % (srctag,)))
            else:
                check.append(None)
                frm.append(('BSP430_PERIPH_CPPID_%s' % (self.port.periph(),),
                             'BSP430_PERIPH_%s' % (self.port.periph(),),
                             'BIT%u' % (self.port.pin,)))
        if self.withTimer:
            to.append(('%s_TIMER_PERIPH_CPPID' % (tag,),
                       '%s_TIMER_PERIPH_HANDLE' % (tag,),
                       '%s_TIMER_CCIDX' % (tag,),
                       '%s_TIMER_CCIS' % (tag,)))
            if self.timer is None:
                check.append('%s_TIMER_PERIPH_CPPID' % (srctag,))
                frm.append(('%s_TIMER_PERIPH_CPPID' % (srctag,),
                             '%s_TIMER_PERIPH_HANDLE' % (srctag,),
                             '%s_TIMER_CCIDX' % (srctag,),
                             '%s_TIMER_CCIS' % (srctag,)))
            else:
                check.append(None)
                frm.append(('BSP430_PERIPH_CPPID_%s' % (self.timer.periph,),
                             'BSP430_PERIPH_%s' % (self.timer.periph,),
                             '%u' % (self.timer.ccidx,),
                             '%s' % (self.timer.ccis,)))
        for (ts,fs,c) in zip(to, frm, check):
            if c is not None:
                defs.append('#if defined(%s)' % (c,))
            defs.extend(['#define %s %s' % _v for _v in zip(ts,fs)])
            if c is not None:
                defs.append('#endif /* %s */' % (c,))
        return '\n'.join(defs)
