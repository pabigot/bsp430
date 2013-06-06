PLATFORM ?= exp430fr5739
MODULES=$(MODULES_PLATFORM)
MODULES += $(MODULES_CONSOLE)
MODULES += utility/unittest
VPATH += $(BSP430_ROOT)/src/chips
MODULES += chips/ds3231
SRC=main.c
include $(BSP430_ROOT)/examples/Makefile.common
