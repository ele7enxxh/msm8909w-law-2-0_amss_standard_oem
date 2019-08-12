#===============================================================================
#
# CBSP Buils system
#
# General Description
#    build shell script file.
#
# Copyright (c) 2009-2009 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/bsp/build/build.sh#1 $
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

unamestr=`uname`
if [ "$unamestr" = "Linux" ]; then
   
   # set up local environment
   export_armlmd_license()
   {
     # know where the host is located
     mdb $(echo `hostname`) return site > __temp.out

     # in boulder?
     grep -q "site: boulder" __temp.out
     if [ $? -eq 0 ]
     then
       echo "in boulder"
       export ARMLMD_LICENSE_FILE=8224@redcloud:8224@swiftly:7117@license-wan-arm1
     else
       # in rtp?
       grep -q "site: rtp" __temp.out
       if [ $? -eq 0 ]
       then
         echo "in rtp"
         export ARMLMD_LICENSE_FILE=8224@license-wan-rtp1
       else
         # in hyderabad?
         grep -q "site: hyderabad" __temp.out
         if [ $? -eq 0 ]
         then
           echo "in hyderabad"
           export ARMLMD_LICENSE_FILE=7117@license-hyd1:7117@license-hyd2:7117@license-hyd3
         else
           # in sandiego and others
           echo "in sandiego"
           export ARMLMD_LICENSE_FILE=7117@license-wan-arm1
         fi
       fi
     fi

     rm -f __temp.out
   }

   ARM_COMPILER_PATH=/pkg/qct/software/arm/RVDS/2.2BLD593/RVCT/Programs/2.2/593/linux-pentium
   PYTHON_PATH=/pkg/qct/software/python/2.5.2/bin
   MAKE_PATH=/pkg/gnu/make/3.81/bin
   export ARMTOOLS=RVCT221
   export ARMROOT=/pkg/qct/software/arm/RVDS/2.2BLD593
   export ARMLIB=$ARMROOT/RVCT/Data/2.2/349/lib
   export ARMINCLUDE=$ARMROOT/RVCT/Data/2.2/349/include/unix
   export ARMINC=$ARMINCLUDE
   export ARMCONF=$ARMROOT/RVCT/Programs/2.2/593/linux-pentium
   export ARMDLL=$ARMROOT/RVCT/Programs/2.2/593/linux-pentium
   export ARMBIN=$ARMROOT/RVCT/Programs/2.2/593/linux-pentium
   export PATH=$MAKE_PATH:$PYTHON_PATH:$ARM_COMPILER_PATH:$PATH
   export ARMHOME=$ARMROOT
   export_armlmd_license
fi


#===============================================================================
# Set target enviroment
#===============================================================================
#export BUILD_ID=AABBQMLS
export BUILD_ID=AABBQMAA
export BUILD_VER=1108

export BUILD_ASIC=7X30A
export MSM_ID=7x30
export HAL_PLATFORM=7x30
export TARGET_FAMILY=7600
export CHIPSET=msm7x30

export BUILD_CMD="BUILD_ID=$BUILD_ID BUILD_VER=$BUILD_VER MSM_ID=$MSM_ID HAL_PLATFORM=$HAL_PLATFORM TARGET_FAMILY=$TARGET_FAMILY BUILD_ASIC=$BUILD_ASIC CHIPSET=$CHIPSET $*"

build_dir=`dirname $0`
$build_dir/../build/tools/rename-log.sh
$build_dir/../build/tools/build.sh $BUILD_CMD
