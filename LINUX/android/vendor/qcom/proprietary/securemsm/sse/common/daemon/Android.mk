##########################################################
# QSEE Connector Service
LOCAL_PATH:= $(call my-dir)

SECUREMSM_SHIP_PATH   := $(QCPATH)/securemsm
SSE_NOSHIP_PATH       := $(SECUREMSM_NOSHIP_PATH)/sse
SSE_SHIP_PATH         := $(SECUREMSM_SHIP_PATH)/sse
COMMON_INCLUDES     := $(SSE_SHIP_PATH)/common/daemon \
                       $(SECUREMSM_SHIP_PATH)/QSEEComAPI \
                       $(BOARD_KERNEL_HEADER_DIR) \

CLIENT_SRC_FILES := IQSEEConnectorService.cpp \
                    BpQSEEConnectorService.cpp \
                    BnQSEEConnectorService.cpp \

SERVER_SRC_FILES := IQSEEConnectorService.cpp \
                    BnQSEEConnectorService.cpp \
                    QSEEConnectorService.cpp \

DAEMON_SRC_FILES := daemon.cpp

##########################################################
##########################################################
# QSEE Connector Service - Daemons
##########################################################
##########################################################

#---------------------------------
# FIDO Sample Authenticator Daemon
#---------------------------------

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(COMMON_INCLUDES) \
                    $(LOCAL_PATH) \

LOCAL_SRC_FILES :=  $(SERVER_SRC_FILES)
LOCAL_SRC_FILES +=  $(DAEMON_SRC_FILES)

LOCAL_SHARED_LIBRARIES += \
        libbinder \
        libandroid_runtime \
        liblog \
        libutils \
        libQSEEComAPI

LOCAL_CFLAGS    := -g -Wno-missing-field-initializers -Wno-format -Wno-unused-parameter

LOCAL_CFLAGS += -DQSEE_CONNECTOR_SERVICE_NAME=com.qualcomm.qti.auth.securesampleauthdaemon
LOCAL_CFLAGS += -DSECURE_APP_PATH=/system/etc/firmware
LOCAL_CFLAGS += -DSECURE_APP_NAME=sampleauth

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE:= sampleauthdaemon
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)


##########################################################
##########################################################
# QSEE Connector Service - Client libraries
##########################################################
##########################################################

#----------------------------
# FIDO Sample Authenticator
#----------------------------

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(COMMON_INCLUDES) \
                    $(LOCAL_PATH) \

LOCAL_SRC_FILES :=  $(CLIENT_SRC_FILES)

LOCAL_SHARED_LIBRARIES += \
        libbinder \
        libandroid_runtime \
        liblog \
        libutils \

LOCAL_CFLAGS    := -g -Wno-missing-field-initializers -Wno-format -Wno-unused-parameter

LOCAL_CFLAGS += -DQSEE_CONNECTOR_SERVICE_NAME=com.qualcomm.qti.auth.securesampleauthdaemon

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE:= libSecureSampleAuthClient
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

##########################################################
# QSEE Connector Service - test client
include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(COMMON_INCLUDES) \
                    $(LOCAL_PATH) \

LOCAL_SRC_FILES :=  client.cpp

LOCAL_SHARED_LIBRARIES += \
        libbinder \
        libandroid_runtime \
        liblog \
        libutils

LOCAL_CFLAGS    := -g -Wno-missing-field-initializers -Wno-format -Wno-unused-parameter
LOCAL_CFLAGS += -DQSEE_CONNECTOR_SERVICE_NAME=com.qualcomm.qti.auth.fidocryptodaemon

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE:= qsee_daemon_client_test
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)




