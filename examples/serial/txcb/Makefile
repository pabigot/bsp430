PLATFORM ?= exp430f5438
# Test supports only systems that have USCI5_A0 but don't use it for console
TEST_PLATFORMS=exp430f5438 exp430f5529 trxeb
MODULES=$(MODULES_PLATFORM)
MODULES += $(MODULES_UPTIME)
MODULES += $(MODULES_CONSOLE)
SRC=main.c
include $(BSP430_ROOT)/examples/Makefile.common
