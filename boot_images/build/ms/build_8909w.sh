#===============================================================================
#
# CBSP Buils system
#
# General Description
#    build shell script file.
#
# Copyright (c) 2013 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/build/ms/build_8909w.sh#1 $
# $DateTime: 2015/09/14 05:57:56 $
# $Author: pwbldsvc $
# $Change: 9019630 $
#                      EDIT HISTORY FOR FILE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     -----------------------------------------------------------
# 10/08/13   ck      Fixed 8916 default build id to match .builds file
# 09/09/13   ck      Initial draft
#===============================================================================
#!/bin/bash

#===============================================================================
# Set up default paths
#===============================================================================
build_dir=`dirname $0`
cd $build_dir
export BUILD_ROOT=../..
export TOOLS_SCONS_ROOT=$BUILD_ROOT/tools/build/scons
export PARTITION_TOOLS_ROOT=$BUILD_ROOT/core/storage/tools/nandbootmbn
LOGNAM=`basename $0`

#----- reset images-----
unset IMAGES
#===============================================================================
#Call setenv if existed 
#===============================================================================
if [ -e "setenv.sh" ]; then 
    source setenv.sh
fi 

# -------------------Set target enviroment ----------------
#Parse command input
while [ $# -gt 0 ]
do
    case "$1" in 
	BUILD_ID=*) 
		BUILD_ID=${1#BUILD_ID=}
		;;
	BUILD_VER=*) 
		BUILD_VER=${1#BUILD_VER=}
		 ;;
	MSM_ID=*) 
		MSM_ID=${1#MSM_ID=} 
		;; 
	HAL_PLATFORM=*) 
		HAL_PLATFORM=${1#HAL_PLATFORM=}
		;;
	TARGET_FAMILY=*) 
		TARGET_FAMILY=${1#TARGET_FAMILY=}
		;;
	BUILD_ASIC=*) 
		BUILD_ASIC=${1#BUILD_ASIC=}
		;;
	CHIPSET=*) 
		CHIPSET=${1#CHIPSET=}
		;;
	 --prod)
                cmds=${cmds//--prod/}
		BUILD_ID=AAAAANAZ
		;;
	*)
		IMAGES="$IMAGES $1"
		;;
    esac
    shift
done 

#------- if not define, set default value ----
BUILD_ID=${BUILD_ID:-DAATANAA}
BUILD_VER=${BUILD_VER:-40000000}
#export BUILDSPEC=KLOCWORK

BUILD_ASIC=${BUILD_ASIC:-8909A}
MSM_ID=${MSM_ID:-8909w}
HAL_PLATFORM=${HAL_PLATFORM:-8909}
TARGET_FAMILY=${TARGET_FAMILY:-8909w}
CHIPSET=${CHIPSET:-msm8909w}


export BUILD_CMD="BUILD_ID=$BUILD_ID BUILD_VER=$BUILD_VER MSM_ID=$MSM_ID HAL_PLATFORM=$HAL_PLATFORM TARGET_FAMILY=$TARGET_FAMILY BUILD_ASIC=$BUILD_ASIC CHIPSET=$CHIPSET  $IMAGES"

echo $BUILD_CMD

#Create build log, and compile
$TOOLS_SCONS_ROOT/build/rename-log.sh ${LOGNAM%.*}
#$TOOLS_SCONS_ROOT/build/build.sh -f $TOOLS_SCONS_ROOT/build/start.scons --tcfgf=9x15.target.builds CFLAGS=--mno-pullup $BUILD_CMD 
$TOOLS_SCONS_ROOT/build/build.sh -f target.scons --tcfgf=8909.target.builds  $BUILD_CMD

#if there is an error stop compiling
if [[ $? != 0 ]]; then
    echo -e "Fail to compile 8909. Exiting ....."
    exit 1
fi 

#rename default build log
#mv build-log.txt ${LOGNAM%.*}-log.txt 

#exit 0
