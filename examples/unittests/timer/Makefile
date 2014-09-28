PLATFORM ?= exp430f5529lp
# Restrict to platforms that are likely to use NTP and have the memory
# for the test code
TEST_PLATFORMS=exp430f5529lp trxeb
MODULES=$(MODULES_PLATFORM)
MODULES += $(MODULES_CONSOLE)
MODULES += utility/unittest
MODULES += utility/uptime
MODULES += periph/timer
SRC=main.c
include $(BSP430_ROOT)/make/Makefile.common
