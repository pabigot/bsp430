PLATFORM ?= trxeb
TEST_PLATFORMS=trxeb
MODULES=$(MODULES_PLATFORM)
MODULES += $(MODULES_UPTIME)
MODULES += $(MODULES_CONSOLE)
MODULES += periph/port

VPATH += $(BSP430_ROOT)/src/sensors
MODULES += sensors/skytraq

CPPFLAGS += -Ifragpool/include

SRC=main.c fragpool/src/fragpool.c
include $(BSP430_ROOT)/examples/Makefile.common
