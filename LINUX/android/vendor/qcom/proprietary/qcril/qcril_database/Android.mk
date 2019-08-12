LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

QCRIL_DB_PATH := $(TARGET_OUT_VENDOR)/qcril.db

$(QCRIL_DB_PATH): $(LOCAL_PATH)/qcril.sql | sqlite3
	rm -f $(QCRIL_DB_PATH)
	cat $< | sqlite3 $@
