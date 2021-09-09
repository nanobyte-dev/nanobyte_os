include build_scripts/config.mk

.PHONY: all floppy_image kernel bootloader clean always tools_fat

all: floppy_image tools_fat

include build_scripts/toolchain.mk

#
# Floppy image
#
floppy_image: $(BUILD_DIR)/main_floppy.img

$(BUILD_DIR)/main_floppy.img: bootloader kernel
	@dd if=/dev/zero of=$@ bs=512 count=2880 >/dev/null
	@mkfs.fat -F 12 -n "NBOS" $@ >/dev/null
	@dd if=$(BUILD_DIR)/stage1.bin of=$@ conv=notrunc >/dev/null
	@mcopy -i $@ $(BUILD_DIR)/stage2.bin "::stage2.bin"
	@mcopy -i $@ $(BUILD_DIR)/kernel.bin "::kernel.bin"
	@mcopy -i $@ test.txt "::test.txt"
	@mmd -i $@ "::mydir"
	@mcopy -i $@ test.txt "::mydir/test.txt"
	@echo "--> Created: " $@

#
# Bootloader
#
bootloader: stage1 stage2

stage1: $(BUILD_DIR)/stage1.bin

$(BUILD_DIR)/stage1.bin: always
	@$(MAKE) -C src/bootloader/stage1 BUILD_DIR=$(abspath $(BUILD_DIR))

stage2: $(BUILD_DIR)/stage2.bin

$(BUILD_DIR)/stage2.bin: always
	@$(MAKE) -C src/bootloader/stage2 BUILD_DIR=$(abspath $(BUILD_DIR))

#
# Kernel
#
kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: always
	@$(MAKE) -C src/kernel BUILD_DIR=$(abspath $(BUILD_DIR))

#
# Tools
#
tools_fat: $(BUILD_DIR)/tools/fat
$(BUILD_DIR)/tools/fat: always tools/fat/fat.c
	@mkdir -p $(BUILD_DIR)/tools
	@$(MAKE) -C tools/fat BUILD_DIR=$(abspath $(BUILD_DIR))

#
# Always
#
always:
	@mkdir -p $(BUILD_DIR)

#
# Clean
#
clean:
	@$(MAKE) -C src/bootloader/stage1 BUILD_DIR=$(abspath $(BUILD_DIR)) clean
	@$(MAKE) -C src/bootloader/stage2 BUILD_DIR=$(abspath $(BUILD_DIR)) clean
	@$(MAKE) -C src/kernel BUILD_DIR=$(abspath $(BUILD_DIR)) clean
	@rm -rf $(BUILD_DIR)/*
