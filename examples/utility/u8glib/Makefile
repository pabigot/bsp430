TEST_PLATFORMS=trxeb exp430f5438 exp430f5529
U8GLIB_ROOT ?= /opt/u8glib
U8GLIB_CSRC = $(U8GLIB_ROOT)/csrc
AUX_CPPFLAGS = -I$(U8GLIB_CSRC)
PLATFORM ?= trxeb
MODULES=$(MODULES_PLATFORM)
MODULES += $(MODULES_UPTIME)
MODULES += $(MODULES_CONSOLE)
MODULES += $(MODULES_SERIAL)
MODULES += periph/port
U8GLIB_SRC = \
    $(U8GLIB_CSRC)/u8g_com_api.c \
    $(U8GLIB_CSRC)/u8g_ll_api.c \
    $(U8GLIB_CSRC)/u8g_font.c \
    $(U8GLIB_CSRC)/u8g_clip.c \
    $(U8GLIB_CSRC)/u8g_page.c \
    $(U8GLIB_CSRC)/u8g_state.c \
    $(U8GLIB_CSRC)/u8g_pb.c \
    $(PLATFORM_U8G_PB_C) \
    $(U8GLIB_ROOT)/fntsrc/u8g_font_10x20.c
SRC = $(U8GLIB_SRC) u8g_bsp430.c main.c
include $(BSP430_ROOT)/examples/Makefile.common
