This directory contains text files that define maps among MCU port pins,
experimenter board header pins, RF evaluation module functional pins,
and booster-pack pins.

The definitive source for MCU port names is the MCU datasheet.  MCU
ports are identified as Px.y being pin y on port x.  Here y ranges from
0 through 7.

The definitive source for experimenter board header pins is the board
user's guide and schematics.  Board pins are normally identified by Jx.y
where Jx is a header block and y is the pin number in that block
(starting with 1).

The definitive source for the RFEM's three headers RF1, RF2, and RF3 is
http://processors.wiki.ti.com/index.php/CC256X_MSP:EXP430F5438#RF1_and_RF2_Connectors.

The definitive source for the BoosterPack headers A and B (20-pin) and C
and D (40-pin "XL") is
http://processors.wiki.ti.com/index.php/BoosterPack_Design_Guide

Subdirectory rfem maps radio-specific functional identifiers to RFEM
header pin identifiers, except where the radio is hard-wired to MCU pins
in which case port identifiers are used.

Subdirectory mcu maps port pin identifiers to peripheral functional
identifiers.  These are used to determine whether a particular pin
supports (for example) timer capture/clock or USCI capabilities.

Subdirectory platform maps experimenter board headers (including RFEM
headers) to MCU port pin identifiers.  Where the board presents a
booster-pack compatible interface the booster-pack standard header names
are used in preference to the board header names, though the mapping
between these names is documented in a comment.

Subdirectory boosterpack maps boosterpack standard BoosterPack headers A
though D to functional capabilities.  Where the function corresponds to
an RFEM header the RF header identifiers are used.

Where a map target is informational (e.g., not connected, VDD, 3V3, GND,
or other signals not supported in BSP430) it is prefixed by a single
underscore, which causes the template expansion to skip that mapping.
