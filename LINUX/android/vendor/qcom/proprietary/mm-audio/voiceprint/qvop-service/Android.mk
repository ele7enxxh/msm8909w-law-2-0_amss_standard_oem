##########################################################
#
# Makefile for VoicePrint modules
#
##########################################################
# Qvop lib
##########################################################
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
# TODO use the error version when interface source is ready
# TODO Subhash
LOCAL_CFLAGS += -Wno-unused-parameter
#LOCAL_CFLAGS += -Werror -Wno-unused-parameter
# debug flag
LOCAL_CFLAGS +=-DQVOP_DEBUG
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../securemsm/QSEEComAPI \
                    $(TARGET_OUT_HEADERS)/common/inc \
                    $(LOCAL_PATH)/..          \
                    $(LOCAL_PATH)/../../../diag/include \
                    external/connectivity/stlport/stlport \
                    $(BOARD_KERNEL_HEADER_DIR)

LOCAL_SRC_FILES:=
LOCAL_SRC_FILES += qvop_interface.c qvop_qsee_interface.c qvop_qsee.c QvopServiceJni.cpp QvopCommandService.cpp logfile.cpp IQvopService.cpp BpQvopService.cpp BnQvopService.cpp QvopCallback.cpp QvopService.cpp
LOCAL_SHARED_LIBRARIES += libbinder libandroid_runtime libc libcutils libdiag libutils libQSEEComAPI
LOCAL_LDLIBS :=
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE:= libqvop-service
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)
##########################################################
# Qvop Daemon
##########################################################
include $(CLEAR_VARS)
LOCAL_CFLAGS += -Werror -Wno-unused-parameter
LOCAL_CFLAGS +=
LOCAL_C_INCLUDES := $(BOARD_KERNEL_HEADER_DIR) \
                    $(LOCAL_PATH)/../../../securemsm/QSEEComAPI \
                    $(TARGET_OUT_HEADERS)/common/inc \
                    $(LOCAL_PATH)/..          \
                    external/connectivity/stlport/stlport
LOCAL_SRC_FILES:= QvopDaemon.cpp logfile.cpp
LOCAL_SHARED_LIBRARIES := libcutils libutils libbinder libqvop-service liblog
LOCAL_MODULE:= qvop-daemon
LOCAL_MODULE_TAGS := eng
include $(BUILD_EXECUTABLE)
