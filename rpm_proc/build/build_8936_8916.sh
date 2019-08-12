# ==========================================================================
#
#  RPM build system launcher
#
# Copyright (c) 2012 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
# ==========================================================================
# $Header: //components/rel/rpm.bf/2.1.1/build/build_8936_8916.sh#1 $

cd './rpm_proc/build/'

export SECPOLICY=USES_SEC_POLICY_DEFAULT_SIGN
export BUILD_ASIC=8916
export MSM_ID=8916
export HAL_PLATFORM=8916
export TARGET_FAMILY=8916
export CHIPSET=msm8916

python ./build_common.py $@

export BUILD_ASIC=8936
export MSM_ID=8936
export HAL_PLATFORM=8936
export TARGET_FAMILY=8936
export CHIPSET=msm8936

python ./build_common.py $@

