################################################################################
# Copyright (c) 2009-2011,2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
################################################################################

# This script is meant to be sourced by test scripts to set up the enviroment
# properly for either either ANDROID or LE builds.

# For ANDROID builds, busybox should be properly installed and its location should
# be included in the PATH since this script requires test [ ] or anything else from
# busybox.

# The script exports the following values
# TEST_TARGET = {ANDROID | CHROMIUM | LE}
# TEST_MNT = Directory used for mounts
# TEST_TMP = Temp directory
# TEST_DEBUG_FS = Directory mounting debugfs rw
# TARGET_TYPE = {7X27 | 7X30 | 8650 | 8650A | 8660 | 8960 | 9615 | 8916 | MSMGOLD |8939 | 8936 | msm8994/8994 | 8909 | 8952 | 8929 | 8976 | 8956 | mdm9607  | mdmcalifornium | 8937 | MSMTITANIUM}

# If any of the TEST_* and TARGET_TYPE are already defined the script will validate them,
# but it will not overwrite them.

# Note that we can't use test [ ] or anything else
# from busybox until we have verified that it is
# properly installed for ANDROID builds

# Default locations for ANDROID and LE
# /proc and /dev are assumed to exist at that location for both build
# types

and_test_mnt="$ANDROID_DATA/mnt"
le_test_mnt="/mnt"

and_test_tmp="$ANDROID_DATA/tmp"
le_test_tmp="/tmp"

and_test_debug="$ANDROID_DATA/debug"
cr_test_debug="/sys/kernel/debug"
le_test_debug="/debug"


# Determine TEST_TARGET
if [ $TEST_TARGET ]; then
    if [ $TEST_TARGET = "ANDROID" ] || [ $TEST_TARGET = "CHROMIUM" ] ||
        [ $TEST_TARGET = "LE" ]; then
	test_target=$TEST_TARGET
    else
	echo "Unrecognized TEST_TARGET: $TEST_TARGET"
	exit 1
    fi
elif [ $ANDROID_ROOT ]; then
    test_target="ANDROID"
else
    test_target="LE"
fi

# Determine TEST_MNT, create directory if it does not exist
if [ $TEST_MNT ]; then
    test_mnt=$TEST_MNT
else
    if [ $test_target = "ANDROID" ]; then
	test_mnt=$and_test_mnt
    else
	test_mnt=$le_test_mnt
    fi
fi

if ! ( [ -e $test_mnt ] || mkdir $test_mnt ); then
    echo "Cannot create $test_mnt for TEST_MNT"
    exit 1
elif ! [ -d $test_mnt ]; then
    echo "$test_mnt is not a directory, cannot be used for TEST_MNT"
    exit 1
elif ! [ -r $test_mnt ] || ! [ -w $test_mnt ] || ! [ -x $test_mnt ]; then
    echo "Incorrect permissions for TEST_MNT: $test_mnt"
    exit 1
fi


# Determine TEST_MNT, create directory if it does not exist
if [ $TEST_TMP ]; then
    test_tmp=$TEST_TMP
else
    if [ $test_target = "ANDROID" ]; then
	test_tmp=$and_test_tmp
    else
	test_tmp=$le_test_tmp
    fi
fi

if ! ( [ -e $test_tmp ] || mkdir $test_tmp ); then
    echo "Cannot create $test_tmp for TEST_TMP"
    exit 1
elif ! [ -d $test_tmp ]; then
    echo "$test_tmp is not a directory, cannot be used for TEST_TMP"
    exit 1
elif ! [ -r $test_tmp ] || ! [ -w $test_tmp ] || ! [ -x $test_tmp ]; then
    echo "Incorrect permissions for TEST_TMP: $test_tmp"
    exit 1
fi


# Determine TEST_DEBUG_FS
if [ $TEST_DEBUG_FS ]; then
    test_debug_fs=$TEST_DEBUG_FS
else
    case "$test_target" in
        ANDROID)    test_debug_fs=$and_test_debug ;;
        CHROMIUM)   test_debug_fs=$cr_test_debug ;;
        *)          test_debug_fs=$le_test_debug ;;
    esac
fi

if ! [ -e $test_debug_fs ]; then
     if ! mkdir $test_debug_fs ; then
	 echo "Failed to create dir $test_debug_fs"
     fi
fi

debug_mnt=`mount | grep -m 1 '^debugfs.*\<rw\>.*' | awk -F'[[:space:]]+(on)?[[:space:]]*' '{print $2}'`

new_mnt="TRUE"

if ! [ $debug_mnt ]; then
    new_mnt="TRUE"
elif [ $test_debug_fs -ef $debug_mnt ]; then
    new_mnt="FALSE"
fi

if [ "$new_mnt" = "TRUE" ]; then
    if ! mount -t debugfs debugfs $test_debug_fs ; then
	echo "Failed to create new debugfs mount on $test_debug_fs"
	exit 1
    fi
fi

#function to get target type
get_target_type(){
# 8650B -> 8650; 8650D -> 8650A 9x15 -> 9615; 9x25 -> 9625; 8910 -> 8610; msmsamarium -> 8962; 8x62 -> 8962;
target_lists="76XXT- 7X27 7X30 8650B 8650D 8660 8960 9x15 8974 9x25 8x10 8910 8610 8226 8x26 8926 8026 8084 msmsamarium 8x62 8916 MSMGOLD 8939 8936 8092 msmkrypton 9x35 msm8994 9640 8909 8952 8992 8929 8996 8976 8956 mdm9607 mdmcalifornium 8937 MSMTITANIUM"

if [ -e /sys/devices/soc0 ]; then
    socinfo="/sys/devices/soc0"
elif [ -e /sys/devices/system/soc/soc0 ]; then
    socinfo="/sys/devices/system/soc/soc0"
else
    echo "socinfo sysfs doesn't exist"
    return 1
fi

if [ -e ${socinfo}/build_id ]; then
    build_id=`cat ${socinfo}/build_id`
else
    echo "soc build_id doesn't exist"
    return 1
fi

if [ -e ${socinfo}/soc_id ]; then
    soc_id=`cat ${socinfo}/soc_id`
else
    echo "soc soc_id doesn't exist"
fi

if [ -e ${socinfo}/machine ]; then
    machine=`cat ${socinfo}/machine`
else
    echo "soc machine doesn't exist"
fi

for i in $target_lists
do
    # check machine first, fall back to the build_id
    check_name=`echo $machine | sed -e "s/.*$i.*//"`
    if [ "$check_name" = "$machine" ]; then
        check_name=`echo $build_id | sed -e "s/.*$i.*//"`
        if [ "$check_name" = "$build_id" ]; then
            continue
        fi
    fi

    if [ "$i" = "76XXT-" ]; then
        target_name="7X27"
    elif [ "$i" = "8650B" ]; then
        target_name="8650"
    elif [ "$i" = "8650D" ]; then
        target_name="8650A"
    elif [ "$i" = "9x15" ]; then
        target_name="9615"
    elif [ "$i" = "9x25" ]; then
        target_name="9625"
    elif [ "$i" = "msmsamarium" -o "$i" = "8x62" ]; then
        target_name="8962"
    elif [ "$i" = "8910" ]; then
        target_name="8610"
    elif [ "$i" = "MSMGOLD" ];then
	target_name="gold"
    elif [ "$i" = "8929" ]; then
        target_name="8939"
    elif [ "$i" = "8936" ]; then
        case "$soc_id" in
            "233")  target_name="8936";;
            *)    target_name="8939";;
        esac
    elif [ "$i" = "msm8994" ]; then
        target_name="8994"
    elif [ "$i" = "msm8992" ]; then
        target_name="8992"
    elif [ "$i" = "msm8976" ];then
        target_name="8976"
    elif [ "$i" = "msm8956" ];then
        target_name="8956"
    elif [ "$i" = "msm8937" ];then
	target_name="8937"
    elif [ "$i" = "MSMTITANIUM" ];then
        target_name="titanium"
    elif [ "$i" = "mdm9607" ]; then
	target_name="mdm9607"
    elif [ "$i" = "mdmcalifornium" ]; then
	target_name="mdmcalifornium"
    else
        target_name=$i
    fi
    return 0
done

echo "can't detect target type"
return 1
}


if [ "$TARGET_TYPE" = "" ]; then
    #get target type (fill in target_name)
    get_target_type

    if [ $? -ne 0 ];then
	target_name=""
    fi
else
    target_name="$TARGET_TYPE"
fi

export TEST_TARGET=$test_target
export TEST_MNT=$test_mnt
export TEST_TMP=$test_tmp
export TEST_DEBUG_FS=$test_debug_fs
export TARGET_TYPE=$target_name
