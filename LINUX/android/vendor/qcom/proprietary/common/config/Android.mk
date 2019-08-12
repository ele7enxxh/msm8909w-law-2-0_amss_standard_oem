LOCAL_PATH := $(my-dir)

include $(CLEAR_VARS)

LOCAL_COPY_HEADERS_TO   := common/inc
LOCAL_COPY_HEADERS      := ../inc/armasm.h
LOCAL_COPY_HEADERS      += ../inc/comdef.h
LOCAL_COPY_HEADERS      += ../inc/common_log.h
LOCAL_COPY_HEADERS      += ../inc/customer.h
LOCAL_COPY_HEADERS      += ../inc/msm_ipc.h
LOCAL_COPY_HEADERS      += ../inc/rex.h
LOCAL_COPY_HEADERS      += ../inc/stringl.h
LOCAL_COPY_HEADERS      += ../inc/target.h
LOCAL_COPY_HEADERS      += ../inc/qsocket.h
LOCAL_COPY_HEADERS      += ../inc/qsocket_ipcr.h

include build/core/copy_headers.mk

ifeq ($(TARGET_SUPPORTS_WEARABLES),true)
ifneq ($(TARGET_SUPPORTS_ANDROID_WEAR),true)
#----------------------------------------------------------------------
# Copy public.libraries.txt to /system/vendor/etc
#----------------------------------------------------------------------
$(shell mkdir -p $(TARGET_OUT_VENDOR_ETC))
$(shell cat device/qcom/$(TARGET_PRODUCT)/public.libraries.vendor.txt > $(TARGET_OUT_VENDOR_ETC)/public.libraries.txt)
$(shell cat $(QC_PROP_ROOT)/common/config/public.libraries.txt >> $(TARGET_OUT_VENDOR_ETC)/public.libraries.txt)

#----------------------------------------------------------------------
# Add JNI libraries to /system/vendor/etc/public.libraries.txt
# for userdebug build
#----------------------------------------------------------------------
ifeq ($(TARGET_BUILD_VARIANT),userdebug)
  $(shell cat $(QC_PROP_ROOT)/common/config/public.libraries_debug.txt >> $(TARGET_OUT_VENDOR_ETC)/public.libraries.txt)
endif
endif
endif
#----------------------------------------------------------------------
