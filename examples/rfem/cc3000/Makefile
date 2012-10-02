PLATFORM ?= exp430f5438
MODULES=$(MODULES_PLATFORM)
MODULES += $(MODULES_UPTIME)
MODULES += $(MODULES_CONSOLE)
MODULES += periph/port
MODULES += periph/sys
MODULES += utility/cc3000spi
MODULES += utility/cli

CC3000_ROOT ?= /usr/local/msp430-cc3000-236a
AUX_CPPFLAGS=-I$(CC3000_ROOT)/include
AUX_CFLAGS+=-ffunction-sections -fdata-sections
AUX_LDFLAGS=-L$(CC3000_ROOT)/lib -Wl,-gc-sections
AUX_LDLIBS=-lcc3000

SRC=main.c
include $(BSP430_ROOT)/examples/Makefile.common
