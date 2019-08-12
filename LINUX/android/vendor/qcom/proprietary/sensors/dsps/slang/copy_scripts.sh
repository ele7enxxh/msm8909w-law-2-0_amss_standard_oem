# ----------------------------------------------------------
# Copyright (c) 2015 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
# ----------------------------------------------------------
# how can the .sli scripts get pushed as part of flashing the build ?
#
adb remount
adb shell rm /system/bin/slang
adb shell rm /data/misc/sensors/*
adb push $ANDROID_BUILD_TOP/out/target/product/msm8996/system/bin/slang  /system/bin
adb shell chmod 777 /system/bin/slang
adb push $ANDROID_BUILD_TOP/vendor/qcom/proprietary/sensors/dsps/slang/DIAG.cfg /sdcard/diag_logs
adb push $ANDROID_BUILD_TOP/vendor/qcom/proprietary/sensors/dsps/slang/scripts/async.sli /data/misc/sensors
adb push $ANDROID_BUILD_TOP/vendor/qcom/proprietary/sensors/dsps/slang/scripts/batch.sli /data/misc/sensors
adb push $ANDROID_BUILD_TOP/vendor/qcom/proprietary/sensors/dsps/slang/scripts/bind.sli /data/misc/sensors
adb push $ANDROID_BUILD_TOP/vendor/qcom/proprietary/sensors/dsps/slang/scripts/blast.sli /data/misc/sensors
adb push $ANDROID_BUILD_TOP/vendor/qcom/proprietary/sensors/dsps/slang/scripts/drafthorse.sli /data/misc/sensors
adb push $ANDROID_BUILD_TOP/vendor/qcom/proprietary/sensors/dsps/slang/scripts/fio.sli /data/misc/sensors
adb push $ANDROID_BUILD_TOP/vendor/qcom/proprietary/sensors/dsps/slang/scripts/ind.sli /data/misc/sensors
adb push $ANDROID_BUILD_TOP/vendor/qcom/proprietary/sensors/dsps/slang/scripts/reg_read.sli /data/misc/sensors
adb push $ANDROID_BUILD_TOP/vendor/qcom/proprietary/sensors/dsps/slang/scripts/samhorse.sli /data/misc/sensors
adb push $ANDROID_BUILD_TOP/vendor/qcom/proprietary/sensors/dsps/slang/scripts/selftest.sli /data/misc/sensors
adb push $ANDROID_BUILD_TOP/vendor/qcom/proprietary/sensors/dsps/slang/scripts/sensor_info.sli /data/misc/sensors
