PLATFORM ?= exp430f5529
TEST_PLATFORMS=exp430f5529
AUX_CPPFLAGS = -Ifatfs/src
MODULES=$(MODULES_PLATFORM)
MODULES += $(MODULES_UPTIME)
MODULES += $(MODULES_CONSOLE)
MODULES += $(MODULES_SERIAL)
MODULES += periph/sys
MODULES += periph/port
MODULES += periph/pmm
SRC=bsp430mmc.c main.c fatfs/src/ff.c
include $(BSP430_ROOT)/make/Makefile.common
