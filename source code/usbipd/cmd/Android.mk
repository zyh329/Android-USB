LOCAL_PATH := $(call my-dir)



include $(CLEAR_VARS)
LOCAL_MODULE := usbip
LOCAL_SRC_FILES := vhci_attach.c usbip_network.c usbip_network.h

LOCAL_C_INCLUDES := external/usbipd/lib
LOCAL_CFLAGS := -O2 -g -W -Wall -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

LOCAL_MODULE_TAGS :=
LOCAL_SYSTEM_SHARED_LIBRARIES := libc
include $(BUILD_EXECUTABLE)
