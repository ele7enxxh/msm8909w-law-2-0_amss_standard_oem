# ==========================================================================
#
#  RPM build system launcher
#
# Copyright (c) 2012 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
# ==========================================================================
# $Header: //components/rel/rpm.bf/2.1.1/build/build_8936.sh#1 $

export BUILD_ASIC=8936
export MSM_ID=8936
export HAL_PLATFORM=8936
export TARGET_FAMILY=8936
export CHIPSET=msm8936

cd './rpm_proc/build/'

python ./build_common.py $@
