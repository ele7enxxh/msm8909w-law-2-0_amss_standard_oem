#===============================================================================
#
# CBSP Buils system
#
# General Description
#    build shell script file.
#
# Copyright (c) 2015 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/build/ms/build_8956.sh#1 $
# $DateTime: 2015/09/01 00:30:35 $
# $Author: pwbldsvc $
# $Change: 8935858 $
#                      EDIT HISTORY FOR FILE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     -----------------------------------------------------------
# 01/08/15   sc      Changes to generate two Elfs for RUMI and asic
# 11/04/14   bs      Initial draft
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
		BUILD_ID=EAAAANAZ
		;;
	*)
		IMAGES="$IMAGES $1"
		;;
    esac
    shift
done 

#------- if not define, set default value ----
BUILD_ID=${BUILD_ID:-EAASANAA}
BUILD_VER=${BUILD_VER:-40000000}
#export BUILDSPEC=KLOCWORK

BUILD_ASIC=${BUILD_ASIC:-8956A}
MSM_ID=${MSM_ID:-8956}
HAL_PLATFORM=${HAL_PLATFORM:-8956}
TARGET_FAMILY=${TARGET_FAMILY:-8956}
CHIPSET=${CHIPSET:-msm8956}


export BUILD_CMD="BUILD_ID=$BUILD_ID BUILD_VER=$BUILD_VER MSM_ID=$MSM_ID HAL_PLATFORM=$HAL_PLATFORM TARGET_FAMILY=$TARGET_FAMILY BUILD_ASIC=$BUILD_ASIC CHIPSET=$CHIPSET  $IMAGES"

echo $BUILD_CMD

#Create build log, and compile
$TOOLS_SCONS_ROOT/build/rename-log.sh ${LOGNAM%.*}
$TOOLS_SCONS_ROOT/build/build.sh -f target.scons --tcfgf=8956.target.builds  $BUILD_CMD

#if there is an error stop compiling
if [[ $? != 0 ]]; then
    echo -e "Fail to compile 8956. Exiting ....."
    exit 1
fi 

#rename default build log
#mv build-log.txt ${LOGNAM%.*}-log.txt 

#exit 0
