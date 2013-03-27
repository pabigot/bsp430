PLATFORM ?= exp430f5438
TEST_PLATFORMS=exp430f5438 trxeb
MODULES=$(MODULES_PLATFORM)
MODULES += $(MODULES_UPTIME)
MODULES += $(MODULES_CONSOLE)
MODULES += periph/port
MODULES += periph/sys
MODULES += periph/flash
MODULES += utility/cli

# Set this to where you installed the cc3000 library
CC3000_ROOT ?= /usr/local/msp430-cc3000-dev

# Tell BSP430's build infrastructure about the library.
AUX_CPPFLAGS=-I$(CC3000_ROOT)/include
AUX_LDFLAGS=-L$(CC3000_ROOT)/lib
AUX_LDLIBS=-lcc3000

SRC=cc3000spi.c main.c
include $(BSP430_ROOT)/examples/Makefile.common
