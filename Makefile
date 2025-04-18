# Project Name
TARGET        := dsi_shop
BUILD         := build
SOURCES       := source
DATA          := data
INCLUDES      := include

# List of libraries to link
LIBS          := -lfat -lnds9

# Tools
DEVKITARM := $(shell echo $$DEVKITARM)

CC := $(DEVKITARM)/bin/arm-eabi-gcc
CXX := $(DEVKITARM)/bin/arm-eabi-g++
AS := $(DEVKITARM)/bin/arm-eabi-as
LD := $(DEVKITARM)/bin/arm-eabi-ld
AR := $(DEVKITARM)/bin/arm-eabi-ar
OBJCOPY := $(DEVKITARM)/bin/arm-eabi-objcopy
OBJDUMP := $(DEVKITARM)/bin/arm-eabi-objdump


CFLAGS        := -Wall -O2 -mthumb -mthumb-interwork -ffunction-sections -fdata-sections \
                 -I$(INCLUDES) -I$(DEVKITPRO)/libnds/include -I$(DEVKITPRO)/libfat/include -std=c99
CFLAGS        += -DARM9
CXXFLAGS      := $(CFLAGS)

LDFLAGS       := -mthumb -mthumb-interwork -specs=ds_arm9.specs \
                 -Wl,-Map,$(TARGET).map \
                 -Wl,--gc-sections \
                 -L$(DEVKITPRO)/libnds/lib -L$(DEVKITPRO)/libfat/lib

# Source files
CFILES        := $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))
OFILES        := $(patsubst %.c,$(BUILD)/%.o,$(CFILES))

# Rules
.PHONY: all clean

all: $(TARGET).nds

$(TARGET).nds: $(TARGET).elf
	@echo "[binary] -> $@"
	@ndstool -c $@ -9 $< -b icon.bmp -t banner.bin

$(TARGET).elf: $(OFILES)
	@echo "[link] -> $@"
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "[cc] -> $<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "[clean]"
	@rm -rf $(BUILD) $(TARGET).elf $(TARGET).nds $(TARGET).map
