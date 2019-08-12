# ==========================================================================
#
#  RPM build system launcher
#
# Copyright (c) 2009-2011 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
# ==========================================================================
# $Header: //components/rel/rpm.bf/2.1.1/build/build.sh#1 $

export BUILD_ASIC=8974
export MSM_ID=8974
export HAL_PLATFORM=8974
export TARGET_FAMILY=8974
export CHIPSET=msm8974

cd './rpm_proc/build/'

python ./build_common.py $@

