adb root
adb wait-for-devices
adb remount
adb shell mount -t vfat -o remount,rw /dev/block/mmcblk0p1 /firmware/ 
adb shell rm /firmware/image/cmnlib*
adb shell rm /system/etc/firmware/cmnlib*
adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\cmnlib\cmnlib.b00 /firmware/image/cmnlib.b00
adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\cmnlib\cmnlib.b01 /firmware/image/cmnlib.b01
adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\cmnlib\cmnlib.b02 /firmware/image/cmnlib.b02
adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\cmnlib\cmnlib.b03 /firmware/image/cmnlib.b03
adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\cmnlib\cmnlib.mdt /firmware/image/cmnlib.mdt

adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\cmnlib\cmnlib.b00 /system/etc/firmware/cmnlib.b00
adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\cmnlib\cmnlib.b01 /system/etc/firmware/cmnlib.b01
adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\cmnlib\cmnlib.b02 /system/etc/firmware/cmnlib.b02
adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\cmnlib\cmnlib.b03 /system/etc/firmware/cmnlib.b03
adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\cmnlib\cmnlib.mdt /system/etc/firmware/cmnlib.mdt



adb shell rm /firmware/image/macchiato_sample*
adb shell rm /system/etc/firmware/macchiato_sample*
adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\macchiato_sample\macchiato_sample.b00 /firmware/image/macchiato_sample.b00
adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\macchiato_sample\macchiato_sample.b01 /firmware/image/macchiato_sample.b01
adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\macchiato_sample\macchiato_sample.b02 /firmware/image/macchiato_sample.b02
adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\macchiato_sample\macchiato_sample.b03 /firmware/image/macchiato_sample.b03
adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\macchiato_sample\macchiato_sample.mdt /firmware/image/macchiato_sample.mdt

adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\macchiato_sample\macchiato_sample.b00 /system/etc/firmware/macchiato_sample.b00
adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\macchiato_sample\macchiato_sample.b01 /system/etc/firmware/macchiato_sample.b01
adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\macchiato_sample\macchiato_sample.b02 /system/etc/firmware/macchiato_sample.b02
adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\macchiato_sample\macchiato_sample.b03 /system/etc/firmware/macchiato_sample.b03
adb push \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\macchiato_sample\macchiato_sample.mdt /system/etc/firmware/macchiato_sample.mdt


adb shell ls /firmware/image/macchiato_sample*
adb shell ls /system/etc/firmware/macchiato_sample*
chmod 777 /system/etc/firmware/macchiato_sample*

adb shell ls /firmware/image/cmnlib*
adb shell ls /system/etc/firmware/cmnlib*
chmod 777 /system/etc/firmware/cmnlib*


adb shell sync
rem adb reboot bootloader

sleep 4

fastboot flash tz  \\harv-chenxiz\SecTools-Build-2.10\secimage_output\8994\tz\tz.mbn