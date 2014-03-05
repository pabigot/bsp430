PLATFORM ?= exp430f5529lp
TEST_PLATFORMS = exp430f5529lp exp430fr5969
MODULES=$(MODULES_PLATFORM)
MODULES += $(MODULES_UPTIME)
MODULES += $(MODULES_CONSOLE)
MODULES += periph/port
MODULES += utility/sharplcd
SRC=main.c
include $(BSP430_ROOT)/examples/Makefile.common
