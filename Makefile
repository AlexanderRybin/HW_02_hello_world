PWD := $(shell pwd)
KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build

DRV_NAME := hello_world

SRC_DIR := $(PWD)/src
BUILD_DIR := $(PWD)/build
BUILD_FILES := $(SRC_DIR)/*.ko
MODULE_FILES := $(BUILD_DIR)/$(DRV_NAME).ko

CLANG_FORMAT_VERS ?= 19
CLANG_FORMAT := clang-format-$(CLANG_FORMAT_VERS)
CLANG_FORMAT_FLAGS += -i
FORMAT_FILES := $(SRC_DIR)/*.c


$(shell mkdir -p $(BUILD_DIR))

kbuild:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules
	mv $(BUILD_FILES) $(BUILD_DIR)/.

run:
	insmod $(MODULE_FILES)

remove:
	rmmod $(MODULE_FILES)

install:
	cp $(MODULE_FILES) /lib/modules/$(shell uname -r)
	/sbin/depmod -a
	/sbin/modprobe $(DRV_NAME)

uninstall:
	/sbin/modprobe -r $(DRV_NAME)
	rm -f /lib/modules/$(shell uname -r)/$(DRV_NAME).ko
	/sbin/depmod -a

clean: 
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
	
format:
	$(CLANG_FORMAT) $(CLANG_FORMAT_FLAGS) $(FORMAT_FILES)

check:
	sudo python3 ./tests/check.py

.PHONY: build run remove install uninstall clean format check