LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

contacts_common_dir := ../../../../../packages/apps/ContactsCommon
phone_common_dir := ../../../../../packages/apps/PhoneCommon
contacts_dir := ../../../../../packages/apps/Contacts
chips_dir := ../../../../../frameworks/opt/chips

src_dirs := src $(contacts_common_dir)/src $(phone_common_dir)/src
res_dirs := res $(contacts_common_dir)/res $(phone_common_dir)/res $(chips_dir)/res

src_dirs += $(contacts_common_dir)/src-N $(phone_common_dir)/src-N $(contacts_dir)/src-Wrapper

LOCAL_SRC_FILES := $(call all-java-files-under, $(src_dirs))
LOCAL_RESOURCE_DIR := $(addprefix $(LOCAL_PATH)/, $(res_dirs))


LOCAL_AAPT_FLAGS := \
    --auto-add-overlay \
    --extra-packages com.android.contacts.common \
    --extra-packages com.android.ex.chips \
    --extra-packages com.android.phone.common

LOCAL_JAVA_LIBRARIES := telephony-common
LOCAL_STATIC_JAVA_LIBRARIES := \
    com.android.vcard \
    android-common \
    guava \
    android-support-v13 \
    android-support-v4 \
    libchips \
    libphonenumber \

LOCAL_PACKAGE_NAME := ConferenceDialer
LOCAL_CERTIFICATE := platform
LOCAL_PRIVILEGED_MODULE := false
LOCAL_PROGUARD_FLAG_FILES := proguard.flags

include $(BUILD_PACKAGE)

# Use the following include to make our test apk.
include $(call all-makefiles-under,$(LOCAL_PATH))
