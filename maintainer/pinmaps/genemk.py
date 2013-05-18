import pinmap
import re

tag = 'CC2500'
filepath = 'rfem/cc2500emk.pinmap'
gpio_re = re.compile('^GDO(?P<idx>\d)$')

gdo0 = pinmap.Signal('GDO0', 'GDO0 on CC2500', with_timer=True)
print gdo0.defines('RF_CC2500', 'BSP430_RFEM_RF1P16')
gdo1 = pinmap.Signal('SO', 'SO on CC2520', port=pinmap.Port(1,7))
print gdo1.defines('RF_CC2520')
