# ==========================================================================
#
#  RPM build system launcher
#
# Copyright (c) 2012 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
# ==========================================================================
# $Header: //components/rel/rpm.bf/2.1.1/build/build_8976.sh#1 $

export BUILD_ASIC=8976
export MSM_ID=8976
export HAL_PLATFORM=8976
export TARGET_FAMILY=8976
export CHIPSET=msm8976

cd './rpm_proc/build/'

python ./build_common.py $@
