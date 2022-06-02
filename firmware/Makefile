TINYUSB=../../tinyusb

include ${TINYUSB}/tools/top.mk
include ${TINYUSB}/examples/make.mk

INC += \
  src \
  $(TOP)/hw \

# Example source
EXAMPLE_SOURCE += $(wildcard src/*.c)
SRC_C += $(addprefix $(CURRENT_PATH)/, $(EXAMPLE_SOURCE))

CFLAGS_OPTIMIZED ?= -O2

include ${TINYUSB}/examples/rules.mk
