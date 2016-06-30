BUILD ?= build

include ../py/mkenv.mk
-include mpconfigport.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

CMSIS_DIR=Libraries/CMSIS

CROSS_COMPILE = arm-none-eabi-

INC += -I.
INC += -I..
INC += -I$(BUILD)
INC += -I$(CMSIS_DIR)/Include
INC += -I../lib/mp-readline
INC += -I../stmhal

CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mabi=aapcs-linux -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
CFLAGS = $(INC) -Wall -Werror -ansi -std=gnu99 -nostdlib $(CFLAGS_CORTEX_M4) $(COPT) -O0 -std=c99 -Wall -fms-extensions -DUC_ID=4503 -DXMC4700_F144x2048 -DARM_MATH_CM4 -g3 -fmessage-length=0

OBJS  = $(SRC:.c=.o)
OBJS += src/system_XMC4700.o

# Remove uncalled code from the final image.
CFLAGS += -fdata-sections -ffunction-sections
LDFLAGS += --gc-sections

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
COPT = -O0
else
COPT += -Os -DNDEBUG
endif

LDFLAGS = -nostdlib -T xmc4700.ld -Map=$@.map --cref
LIBS =

SRC_C = \
	main.c \
	uart_core.c \
	uart_extra.c \
        lib/timeutils/timeutils.c \
	lib/utils/printf.c \
	lib/utils/pyexec.c \
	lib/utils/pyhelp.c \
	lib/libc/string0.c \
	lib/libm/roundf.c \
	lib/libm/math.c \
	lib/libm/acoshf.c \
	lib/libm/asinhf.c \
	lib/libm/asinfacosf.c \
	lib/libm/atanf.c \
	lib/libm/atan2f.c \
	lib/libm/sf_sin.c \
	lib/libm/sf_cos.c \
	lib/libm/kf_cos.c \
	lib/libm/kf_sin.c \
	lib/libm/fmodf.c \
	lib/libm/log1pf.c \
	lib/libm/ef_rem_pio2.c \
	lib/libm/erf_lgamma.c \
	lib/libm/kf_rem_pio2.c \
	lib/mp-readline/readline.c \
	$(BUILD)/_frozen_mpy.c \
	Libraries/XMCLib/src/xmc_common.c \
	Libraries/XMCLib/src/xmc_gpio.c \
	Libraries/XMCLib/src/xmc_uart.c \
	Libraries/XMCLib/src/xmc_usic.c \
	Libraries/XMCLib/src/xmc4_scu.c \
	Libraries/XMCLib/src/xmc4_gpio.c \
	Startup/system_XMC4700.c \

SRC_S = \
	Startup/startup_XMC4700.S \
        stmhal/gchelper.S \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.S=.o))
all: $(BUILD)/firmware.elf

firmware.elf: $(OBJS)
	mkdir -p
	$(CROSS_COMPILE)gcc -T xmc4700.ld $(LFLAGS) $(CFLAGS) -o firmware.elf $(OBJS) $(LIBS)

firmware: firmware.elf
	$(CROSS_COMPILE)objcopy $(CPFLAGS) firmware.elf bin/firmware.bin
	$(CROSS_COMPILE)objdump -S firmware.elf > bin/firmware.lst
	$(CROSS_COMPILE)size firmware.elf

$(BUILD)/_frozen_mpy.c: frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)../tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h $< > $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

install: $(BUILD)/firmware.elf
	$(CROSS_COMPILE)gdb $(BUILD)/firmware.elf -ex "target remote :2331" -ex "monitor reset" -ex "load" -ex "monitor reset" -ex "monitor go"

include ../py/mkrules.mk
