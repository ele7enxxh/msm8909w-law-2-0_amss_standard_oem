LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
        io-p.c\
        io-prefetch/dblayer.c\
        io-prefetch/file_playback.c\
        io-prefetch/maintain_db.c\
        io-prefetch/list_capture.c

LOCAL_SHARED_LIBRARIES := libcutils libsqlite

LOCAL_C_INCLUDES += \
        external/sqlite/dist

LOCAL_CFLAGS += \
        -DSERVER \
        -DCLIENT \
        -g0 \
	-Wno-unused-parameter
	#-Wall\
	#-DQC_DEBUG=1


LOCAL_MODULE := libqti-iop
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := io-p-d.c

LOCAL_SHARED_LIBRARIES := libcutils libsqlite

LOCAL_MODULE := iop

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES += libqti-iop

LOCAL_MODULE_OWNER := qti
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES :=  client.c\
                    io-p.c\
                    io-prefetch/dblayer.c\
                    io-prefetch/file_playback.c\
                    io-prefetch/maintain_db.c\
                    io-prefetch/list_capture.c

LOCAL_MODULE := libqti-iop-client

 LOCAL_C_INCLUDES += external/sqlite/dist

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += \
                -DCLIENT \
                -g0 \
		-Wno-unused-parameter\
		#-Wall\
		#-DQC_DEBUG=1

LOCAL_SHARED_LIBRARIES := libcutils  libsqlite

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
