# Copyright Statement:
#

LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_DEVICE),)
TARGET_DEVICE := $(TARGET_PRODUCT)
endif

ifeq ($(TARGET_DEVICE), $(filter abc123 foraker, $(TARGET_DEVICE)))
TARGET_DEVICE := abc123
endif

# if WIFI_DRIVER_MODULE_PATH is set empty by device.mk,
# driver module will be loaded by rc file, not by framework
# in this case, driver makefile will refer to WIFI_DRIVER_MODULE_PATH_FOR_DRV
# for driver path location
ifeq ($(WIFI_DRIVER_MODULE_PATH),)
WIFI_DRIVER_MODULE_PATH_IN_DRV := $(WIFI_DRIVER_MODULE_PATH_FOR_DRV)
else
WIFI_DRIVER_MODULE_PATH_IN_DRV := $(WIFI_DRIVER_MODULE_PATH)
endif

ifeq ($(WIFI_DRIVER_MODULE_NAME),)
WIFI_DRIVER_MODULE_NAME_IN_DRV := $(WIFI_DRIVER_MODULE_NAME_FOR_DRV)
else
WIFI_DRIVER_MODULE_NAME_IN_DRV := $(WIFI_DRIVER_MODULE_NAME)
endif

include $(CLEAR_VARS)
WIFI_PROJ_CONFIG_FILE := $(LOCAL_PATH)/config/$(TARGET_DEVICE).config

local_path_full := $(shell pwd)/$(LOCAL_PATH)
wifi_module_out_path := $(PRODUCT_OUT)$(WIFI_DRIVER_MODULE_PATH_IN_DRV)
wifi_module_target := $(wifi_module_out_path)

#Default enable prealloc memory
CFG_MTK_PREALLOC_DRIVER := y

ifneq ($(filter abc123 abc123,$(TARGET_DEVICE)),)
MTK_STRIP_DRIVER := n
else
MTK_STRIP_DRIVER := y
endif

LOCAL_KERNEL_CROSS_COMPILE := $(KERNEL_CROSS_COMPILE)
# Due to compile error when doing stripe command for abc123/abc123/abc123/abc123, the cross_compile need to be redefined
ifeq ($(TARGET_BOARD_PLATFORM), $(filter m7322 m7632 m7332 mt5889, $(TARGET_BOARD_PLATFORM)))
LOCAL_KERNEL_CROSS_COMPILE := $(ROOTDIR)/prebuilts/gcc/linux-x86/aarch64/linaro-aarch64_linux-2014.09/bin/aarch64-linux-gnu-
endif

#current parameter name for target arch on VSB is $(TARGET_ARCH)
ifeq ($(TARGET_KERNEL_ARCH),)
TARGET_KERNEL_ARCH := $(TARGET_ARCH)
endif

#avoid $(KERNEL_OUT)is not defined
ifeq ($(KERNEL_OUT),)
KERNEL_OUT := $(PRODUCT_OUT)/obj/KERNEL_OBJ
endif
ifeq ($(TARGET_BOARD_PLATFORM), $(filter m7322 m7632 m7332 mt8695 mt5889, $(TARGET_BOARD_PLATFORM)))
WIFI_KERNEL_KO_OUT := $(PRODUCT_OUT)/vendor/lib/modules/
else
WIFI_KERNEL_KO_OUT := $(PRODUCT_OUT)/obj/lib_vendor/
endif

LOCAL_MODULE := $(WIFI_DRIVER_MODULE_NAME_IN_DRV)
LOCAL_MODULE_TAGS := optional
LOCAL_ADDITIONAL_DEPENDENCIES := $(wifi_module_target)

include $(BUILD_PHONY_PACKAGE)

$(LOCAL_ADDITIONAL_DEPENDENCIES): PRIVATE_DRIVER_LOCAL_DIR := $(local_path_full)
$(LOCAL_ADDITIONAL_DEPENDENCIES): $(INSTALLED_KERNEL_TARGET)
	$(hide) rm -rf $(PRIVATE_DRIVER_LOCAL_DIR)/.config
	$(hide) cp -f $(WIFI_PROJ_CONFIG_FILE) $(PRIVATE_DRIVER_LOCAL_DIR)/.config
	$(MAKE) -C $(KERNEL_OUT) M=$(PRIVATE_DRIVER_LOCAL_DIR) ARCH=$(TARGET_KERNEL_ARCH) CROSS_COMPILE=$(KERNEL_CROSS_COMPILE) CFG_MTK_PREALLOC_DRIVER=$(CFG_MTK_PREALLOC_DRIVER) modules
	$(MAKE) -C $(KERNEL_OUT) M=$(PRIVATE_DRIVER_LOCAL_DIR) ARCH=$(TARGET_KERNEL_ARCH) CROSS_COMPILE=$(KERNEL_CROSS_COMPILE) CFG_MTK_PREALLOC_DRIVER=$(CFG_MTK_PREALLOC_DRIVER) INSTALL_MOD_PATH=$(PRODUCT_OUT)/obj/KERNEL_OBJ modules_install
ifeq ($(MTK_STRIP_DRIVER), y)
	$(LOCAL_KERNEL_CROSS_COMPILE)strip -g $(PRIVATE_DRIVER_LOCAL_DIR)/$(WIFI_DRIVER_MODULE_NAME_IN_DRV).ko
	$(LOCAL_KERNEL_CROSS_COMPILE)strip -g $(PRIVATE_DRIVER_LOCAL_DIR)/$(WIFI_DRIVER_MODULE_NAME_IN_DRV)_prealloc.ko
endif
	$(hide) mkdir -p $(WIFI_KERNEL_KO_OUT)
	$(hide) cp -f $(PRIVATE_DRIVER_LOCAL_DIR)/$(WIFI_DRIVER_MODULE_NAME_IN_DRV).ko $(WIFI_KERNEL_KO_OUT)
ifeq ($(CFG_MTK_PREALLOC_DRIVER), y)
	$(hide) cp -f $(PRIVATE_DRIVER_LOCAL_DIR)/$(WIFI_DRIVER_MODULE_NAME_IN_DRV)_prealloc.ko $(WIFI_KERNEL_KO_OUT)
endif
	$(MAKE) -C $(KERNEL_OUT) M=$(PRIVATE_DRIVER_LOCAL_DIR) ARCH=$(TARGET_KERNEL_ARCH) CROSS_COMPILE=$(KERNEL_CROSS_COMPILE) clean
local_path_full :=
wifi_module_out_path :=
wifi_module_target :=
