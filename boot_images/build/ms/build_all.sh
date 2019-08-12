#===============================================================================
#
# CBSP Buils system
#
# General Description
#    build shell script file.
#
# Copyright (c) 2013-2014 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/build/ms/build_all.sh#1 $ 
# $DateTime: 2015/09/01 00:30:35 $
# $Author: pwbldsvc $
# $Change: 8935858 $
#                      EDIT HISTORY FOR FILE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---    -----------------------------------------------------------
# 11/04/14   bs     Added for 8956 build command
# 09/29/14   lm     Added for 8952 build command
# 07/08/14   mrb    removed 8916/8936
# 04/01/14   lm     Added for 8936 build command
# 03/07/14   jz     Added for 9x45 build command
# 10/07/13   ck     Removed targets that are not in Bear family
# 09/12/13   ck     Adding 8916
# 07/23/13   plc    Adding 8994
# 06/26/13   plc    Adding 8092 compilation
# 05/13/12   sy     Adding 9x35  compiilation 
# 05/06/13   plc    Adding 8x62 compilation
# 04/23/12   sy     Adding 8084 and 8974 compilation 
# 04/04/12   sy     Remove 8x10,8x26 compilation
# 04/16/12   dh     Enable 8084
# 09/18/12   sy     Enable 8x10 ,8x26 compilation 
# 07/20/12   sy     Initial Draft 
#===============================================================================
#!/bin/bash
#---- Set up default paths -----------
export BUILD_ROOT=../..
export CORE_BSP_ROOT=$BUILD_ROOT/core/bsp
export TOOLS_SCONS_ROOT=$BUILD_ROOT/tools/build/scons
export PARTITION_TOOLS_ROOT=$BUILD_ROOT/core/storage/tools/nandbootmbn
#-------- set the build dir and  reset the command line parameter ---- 
build_dir=`dirname $0`
cd $build_dir
#if setenv.sh exist, run it
[ -f $BUILD_ROOT/build/ms/setenv.sh ] && source setenv.sh || echo "Warning: SetEnv file not existed"


sh build.sh TARGET_FAMILY=9x45 $@
if [[ $? != 0 ]]; then
    echo -e "Build_All: Fail to compile 9x45. Exiting ....."
    exit 1
fi

sh build.sh TARGET_FAMILY=8909 $@
if [[ $? != 0 ]]; then
    echo -e "Build_All: Fail to compile 8909. Exiting ....."
    exit 1
fi

sh build.sh TARGET_FAMILY=8952 $@
if [[ $? != 0 ]]; then
    echo -e "Build_All: Fail to compile 8952. Exiting ....."
    exit 1
fi

sh build.sh TARGET_FAMILY=8956 $@
if [[ $? != 0 ]]; then
    echo -e "Build_All: Fail to compile 8956. Exiting ....."
    exit 1
fi
