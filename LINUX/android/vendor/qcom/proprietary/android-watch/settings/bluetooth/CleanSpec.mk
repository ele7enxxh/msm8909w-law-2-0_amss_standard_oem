$(call add-clean-step, rm -rf $(OUT_DIR)/target/common/obj/APPS/watchbluetooth_intermediates)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/obj/APPS/watchbluetooth_intermediates)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/priv-app/watchbluetooth.apk)

# ************************************************
# NEWER CLEAN STEPS MUST BE AT THE END OF THE LIST
# ************************************************
