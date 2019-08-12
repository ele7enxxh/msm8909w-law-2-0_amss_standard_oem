#!/bin/bash
#==========================================================================
# Copyright (c) 2013 Qualcomm Atheros, Inc.
# All Rights Reserved.
# Qualcomm Atheros Confidential and Proprietary.
#
# Copyright (c) 2015 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#==========================================================================
#
#==========================================================================
#
# Usage:
#     mksdkjar.sh [output-path]
#
# Note, this script requires the existence of the build binary -
# out/target/common/obj/JAVA_LIBRARIES/framework_intermediates/classes.jar
# Also, this script should be called at the root directory.
#
#==========================================================================

DIR=${0%/*}
CMD=${0##*/}
OUT="/tmp/izatsdk"

if [ -n "$1" ]
then
    if [ "$1" == "-h" ] || [ "$1" == "--help" ]
    then
        echo "$0 [output-path]"
        echo "  e.g. $0"
        echo "       $0 /tmp/sdk"
        exit
    else
        OUT=$1
        if [ ! -d $OUT ]
        then
            mkdir -p $OUT
        fi
    fi
fi

if [ ! -e $DIR/../../../../../../../out/target/common/obj/JAVA_LIBRARIES/framework_intermediates/classes.jar ]
then
    echo "this script requires Android frameworks to have been built"
    exit
fi

mkdir -p $OUT/classes
#cd $DIR
javac -cp $DIR/../../../../../../../out/target/common/obj/JAVA_LIBRARIES/frameworks-core-util-lib_intermediates/classes.jar:$DIR/../../../../../../../out/target/common/obj/JAVA_LIBRARIES/framework_intermediates/classes.jar -sourcepath $DIR/../java -d $OUT/classes/ $DIR/../java/com/qti/location/sdk/*.java
jar cvf $OUT/izatsdk.jar -C $OUT/classes/ .

mkdir -p $OUT/docs
javadoc -d $OUT/docs -sourcepath $DIR/../java/ -exclude com.qualcomm.services.location -classpath $DIR/../../../../../../../out/target/common/obj/JAVA_LIBRARIES/frameworks-core-util-lib_intermediates/classes.jar:$DIR/../../../../../../../out/target/common/obj/JAVA_LIBRARIES/framework_intermediates/classes.jar com.qti.location.sdk

rm -rf $OUT/classes

tar cjvf $OUT/docs.tbz -C $OUT/ docs

echo "Java docs for sdk available at $OUT"
