# Install and run imstests.apk.
adb shell pm list instrumentation

#Common
VENDOR_APPS_PATH="/system/vendor/app"

#Install the target app.
TARGET_MODULE_NAME="ims"
TARGET_PACKAGE_NAME="org.codeaurora.ims"
adb root
adb wait-for-device
adb remount
adb wait-for-device
adb shell rm "$VENDOR_APPS_PATH/$TARGET_MODULE_NAME/$TARGET_MODULE_NAME.apk"
adb push $ANDROID_PRODUCT_OUT/$VENDOR_APPS_PATH/$TARGET_MODULE_NAME/$TARGET_MODULE_NAME.apk $VENDOR_APPS_PATH/$TARGET_MODULE_NAME


# Install the test app.
TEST_MODULE_NAME="imsTests"
TEST_PACKAGE_NAME="com.qualcomm.qti.ims.tests"
adb install -r $ANDROID_PRODUCT_OUT/$VENDOR_APPS_PATH/$TEST_MODULE_NAME/$TEST_MODULE_NAME.apk

adb shell am instrument -w $TEST_PACKAGE_NAME/android.test.InstrumentationTestRunner

adb shell pm uninstall $TEST_PACKAGE_NAME