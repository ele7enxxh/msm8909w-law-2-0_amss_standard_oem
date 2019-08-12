include $(call all-subdir-makefiles)

#----------------------------------------------------------------------
# Copy public.libraries.txt to /system/vendor/etc
#----------------------------------------------------------------------
$(shell mkdir -p $(TARGET_OUT_VENDOR_ETC))
$(shell cat device/qcom/$(TARGET_PRODUCT)/public.libraries.vendor.txt > $(TARGET_OUT_VENDOR_ETC)/public.libraries.txt)
$(shell cat $(QC_PROP_ROOT)/common/config/public.libraries.txt >> $(TARGET_OUT_VENDOR_ETC)/public.libraries.txt)
#----------------------------------------------------------------------
