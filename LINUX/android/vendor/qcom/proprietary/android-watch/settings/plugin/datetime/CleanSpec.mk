$(call add-clean-step, rm -rf $(OUT_DIR)/target/common/obj/APPS/watchdatetime_intermediates)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/obj/APPS/watchdatetime_intermediates)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/priv-app/watchdatetime.apk)

# ************************************************
# NEWER CLEAN STEPS MUST BE AT THE END OF THE LIST
# ************************************************
