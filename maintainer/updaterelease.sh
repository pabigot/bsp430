#!/bin/sh

REL=${REL:-`date +%Y%m%d`}

sed -i \
  -e '/^PROJECT_NUMBER/s@\s*=\s*[0-9][0-9]*$@'" = ${REL}@" \
  doc/doxygen.cfg

sed -i \
  -e '/define\s\s*BSP430_VERSION/s@\s\s*[0-9][0-9]*$@'" ${REL}@" \
  include/bsp430/core.h

sed -i \
  -e '/^Release:/s@\s\s*[0-9][0-9]*$@'" ${REL}@" \
  README.rst
