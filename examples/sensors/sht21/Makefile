# Uses BOOSTXL-SENSHUB boosterpack
PLATFORM ?= exp430g2
TEST_PLATFORMS_EXCLUDE=em430 surf wolverine exp430fr5969
MODULES=$(MODULES_PLATFORM)
MODULES += $(MODULES_CONSOLE) $(MODULES_UPTIME)
MODULES += $(MODULES_TIMER)
MODULES += periph/port
MODULES += periph/sys

VPATH += $(BSP430_ROOT)/src/sensors
MODULES += sensors/sht21

SRC=main.c
include $(BSP430_ROOT)/examples/Makefile.common
