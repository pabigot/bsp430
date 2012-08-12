BSP430 is a set of C language adaptation layers simplifying cross-MCU
library and application development on Texas Instruments MSP430
microcontrollers.  It features:

* A @link bsp430_config configuration infrastructure @endlink allowing you
  to include only those resources and capabilities required by your
  application;

* A @link layer set of abstraction layers @endlink enabling efficient
  identification and manipulation of peripheral capabilities at both
  register and functional levels;

* A @link callbacks callback infrastructure @endlink allowing unrelated
  capabilities to share resources including port and timer interrupt
  handlers;

* A platform model that enables rapid development on common hardware
  solutions including six TI experimenter boards as well as custom boards;

* Support for watchdog, low power mode execution

* A @link buildenv build infrastructure @endlink designed to simplify
  creating new applications, currently focused on the MSPGCC toolchain in a
  POSIX command-line environment supported by GNU Make

BSP430 is released as open source under the `BSD-3-Clause`_ license.

Get a copy with GIT::

 git clone 

Download a tar file of current release

.. _giturl:
.. _homepage: 
.. _BSD-3-Clause: http://www.opensource.org/licenses/BSD-3-Clause
