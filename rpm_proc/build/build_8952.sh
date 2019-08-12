# ==========================================================================
#
#  RPM build system launcher
#
# Copyright (c) 2012 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
# ==========================================================================
# $Header: //components/rel/rpm.bf/2.1.1/build/build_8952.sh#1 $

export BUILD_ASIC=8952
export MSM_ID=8952
export HAL_PLATFORM=8952
export TARGET_FAMILY=8952
export CHIPSET=msm8952

cd './rpm_proc/build/'

python ./build_common.py $@
