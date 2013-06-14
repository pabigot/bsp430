# Uses BOOSTXL-SENSHUB boosterpack
PLATFORM ?= exp430g2
TEST_PLATFORMS_EXCLUDE=em430 surf wolverine
MODULES=$(MODULES_PLATFORM)
MODULES += $(MODULES_CONSOLE) $(MODULES_UPTIME)
MODULES += $(MODULES_TIMER)
MODULES += periph/port
MODULES += periph/sys
SRC=main.c
include $(BSP430_ROOT)/examples/Makefile.common
