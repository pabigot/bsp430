PLATFORM ?= exp430f5529lp
TEST_PLATFORMS=exp430f5438 exp430f5529lp trxeb
MODULES=$(MODULES_PLATFORM)
MODULES += $(MODULES_UPTIME)
MODULES += $(MODULES_CONSOLE)
MODULES += periph/port
MODULES += periph/sys

ifeq (exp430f5529lp,$(PLATFORM))
CC3000BOOST ?= 1
endif # PLATFORM exp430f5529lp

# Add the rules to provide external library support for the CC3000
include $(BSP430_ROOT)/make/Makefile.cc3000

SRC += main.c
include $(BSP430_ROOT)/make/Makefile.common
