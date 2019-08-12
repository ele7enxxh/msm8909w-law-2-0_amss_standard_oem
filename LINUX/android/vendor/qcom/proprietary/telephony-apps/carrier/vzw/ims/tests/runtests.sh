# Install and run the test.
adb shell pm list instrumentation

TEST_NAME="com.qti.vzw.ims.internal.tests"
TEST_PACKAGE=$TEST_NAME
adb install -r $ANDROID_PRODUCT_OUT/system/app/$TEST_NAME/$TEST_NAME.apk
adb shell am instrument -w $TEST_PACKAGE/android.test.InstrumentationTestRunner
adb shell pm uninstall $TEST_PACKAGE
