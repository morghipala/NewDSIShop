# Makefile per DSi alternative eShop
include $(DEVKITARM)/ds_rules

# Nome target (senza estensione)
TARGET := NewDSIShop

# Directory sorgenti e oggetti
SOURCE_DIR := source
BUILD_DIR  := build

SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS := $(patsubst $(SOURCE_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
INCLUDES := -I$(SOURCE_DIR)

# Librerie libnds, WiFi, FAT e ws (socket)
LIBS := -lnds9 -lnds9_wifi -lfat -lws -lm

# Regola per compilare ogni .c
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -MMD -c $< -o $@

# Target finale
$(TARGET).nds: $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

# Pulizia
include $(wildcard $(BUILD_DIR)/*.d)
clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d $(TARGET).nds $(TARGET).srl
