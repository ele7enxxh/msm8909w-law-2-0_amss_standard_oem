$(call add-clean-step, rm -rf $(OUT_DIR)/target/common/obj/APPS/ctsintenthandler_intermediates)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/obj/APPS/ctsintenthandler_intermediates)
$(call add-clean-step, rm -rf $(PRODUCT_OUT)/system/priv-app/ctsintenthandler.apk)

# ************************************************
# NEWER CLEAN STEPS MUST BE AT THE END OF THE LIST
# ************************************************
