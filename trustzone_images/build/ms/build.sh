#===============================================================================
#
# CBSP Buils system
#
# General Description
#    build shell script file.
#
# Copyright (c) 2009-2012 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/ms/build.sh#1 $
# $DateTime: 2016/06/17 14:31:11 $
# $Author: pwbldsvc $
# $Change: 10727476 $
#                      EDIT HISTORY FOR FILE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     -----------------------------------------------------------
#
#===============================================================================
#===============================================================================
# Set up default paths
# Need to get working on Linux
#===============================================================================


build_dir=`dirname $0`
cd $build_dir
#export OKL4_SDK=../../core/kernel/sdk
export BUILD_ROOT=../..
export CORE_BSP_ROOT=$BUILD_ROOT/core/bsp
export TOOLS_SCONS_ROOT=$BUILD_ROOT/tools/build/scons

if [ -e setenv.sh ]; then
source setenv.sh
fi


#===============================================================================
# Set target enviroment
#===============================================================================

chipset="$1"

if [ "$chipset" = "CHIPSET=msm8916" ]; then
  export BUILD_ID=MAUAANAA
  export BUILD_VER=40000000
  export BUILDSPEC=KLOCWORK

  export BUILD_ASIC=8916
  export MSM_ID=8916
  export HAL_PLATFORM=8916
  export TARGET_FAMILY=8916
  export CHIPSET=msm8916
  export IMAGES=""
elif [ "$chipset" = "CHIPSET=msm8936" ]; then
  export BUILD_ID=MAWAANAA
  export BUILD_VER=40000000
  export BUILDSPEC=KLOCWORK

  export BUILD_ASIC=8936
  export MSM_ID=8936
  export HAL_PLATFORM=8936
  export TARGET_FAMILY=8936
  export CHIPSET=msm8936
  export IMAGES=""
elif [ "$chipset" = "CHIPSET=msm8909" ]; then
  export BUILD_ID=MAZAANAA
  export BUILD_VER=40000000
  export BUILDSPEC=KLOCWORK

  export BUILD_ASIC=8909
  export MSM_ID=8909
  export HAL_PLATFORM=8909
  export TARGET_FAMILY=8909
  export CHIPSET=msm8909
  export IMAGES=""
elif [ "$chipset" = "CHIPSET=msm8909w" ]; then
  export BUILD_ID=MATAANAA
  export BUILD_VER=40000000
  export BUILDSPEC=KLOCWORK

  export BUILD_ASIC=8909w
  export MSM_ID=8909w
  export HAL_PLATFORM=8909w
  export TARGET_FAMILY=8909w
  export CHIPSET=msm8909w
  export IMAGES=""
elif [ "$chipset" = "CHIPSET=mdm9x07" ]; then
  export BUILD_ID=MARAANAA
  export BUILD_VER=40000000
  export BUILDSPEC=KLOCWORK

  export BUILD_ASIC=9x07
  export MSM_ID=9x07
  export HAL_PLATFORM=9x07
  export TARGET_FAMILY=9x07
  export CHIPSET=mdm9x07
  export IMAGES=""
elif [ "$chipset" = "CHIPSET=mdm9x09" ]; then
  export BUILD_ID=NAYAANAA
  export BUILD_VER=40000000
  export BUILDSPEC=KLOCWORK

  export BUILD_ASIC=9x09
  export MSM_ID=9x09
  export HAL_PLATFORM=9x09
  export TARGET_FAMILY=9x09
  export CHIPSET=mdm9x09
  export IMAGES=""

else
  echo "Please enter a valid chipset"
fi

shift

export BUILD_CMD="BUILD_ID=$BUILD_ID BUILD_VER=$BUILD_VER MSM_ID=$MSM_ID HAL_PLATFORM=$HAL_PLATFORM TARGET_FAMILY=$TARGET_FAMILY BUILD_ASIC=$BUILD_ASIC CHIPSET=$CHIPSET $IMAGES $*"
$TOOLS_SCONS_ROOT/build/rename-log.sh
$TOOLS_SCONS_ROOT/build/build.sh -f target.scons $BUILD_CMD
