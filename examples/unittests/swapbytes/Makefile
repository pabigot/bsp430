PLATFORM ?= exp430fr5739
MODULES=$(MODULES_PLATFORM)
MODULES += $(MODULES_CONSOLE)
MODULES += utility/unittest
# There's a cast warning we don't want stopping the test
WITH_WARN_ERROR ?= 0
SRC=main.c bswap.c
include $(BSP430_ROOT)/make/Makefile.common
