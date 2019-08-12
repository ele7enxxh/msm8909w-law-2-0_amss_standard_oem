Follow these steps to include a new driver in the ADSP build with the HD22 package:

1. Copy driver source and header files at location \adsp_proc\Sensors\dd\vendor\src. There are two driver stub files
dd_vendor_1.c (and .h) and dd_vendor_2.c (and .h) at this location. You can either use the same files names to 
add your driver code or replace these files.

2. If you are using different file name(s) then update the following section in file 
\adsp_proc\Sensors\dd\vendor\build\dd_vendors.scons to include your driver
source file names. 

if 'SENSORS_DD_DEV_FLAG' in env:
   DD_VENDOR_SOURCES = [
   "${BUILDPATH}/dd_vendor_1.c",
   "${BUILDPATH}/dd_vendor_2.c",
   ]

3. Build ADSP code using the following command from the root folder (adsp_proc)
build.py

4. Steps to load ADSP binaries:
adb root
adb wait-for-device
adb shell mount -o remount,rw /firmware
adb shell rm /firmware/image/adsp*
adb push <location of ADSP binaries> /firmware/image
adh shell sync
adb reboot

Note - You may have to power cycle the DragonBoard instead of adb reboot.

Limitations:

1. Maximum of two different vendor drivers can be added to the build at one time.
2. The driver API function list name can either be sns_dd_vendor_if_1 or sns_dd_vendor_if_2. 
3. The UUID associated with each API function list is:
sns_dd_vendor_if_1 => {0x61,0xc6,0x95,0x37,0x96,0x75,0x40,0xc1,0xbc,0x46,0xfd,0x1b,0x43,0xb8,0x25,0x0b}
sns_dd_vendor_if_2 => {0xd3,0x65,0x13,0x8b,0x98,0x92,0x49,0xc0,0x9d,0xbd,0x68,0xde,0xfb,0xaa,0x95,0x2c}

