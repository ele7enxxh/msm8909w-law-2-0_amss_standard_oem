# ==========================================================================
#
#  RPM build system launcher
#
# Copyright (c) 2012 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
# ==========================================================================
# $Header: //components/rel/rpm.bf/2.1.1/build/build_8909.sh#2 $

export BUILD_ASIC=8909
export MSM_ID=8909
export HAL_PLATFORM=8909
export TARGET_FAMILY=8909
export CHIPSET=msm8909
export CHIPSET_PMIC=pm8909
export SECPOLICY=USES_SEC_POLICY_DEFAULT_SIGN
cd './rpm_proc/build/'

python ./build_common.py $@


export BUILD_ASIC=8909
export MSM_ID=8909
export HAL_PLATFORM=8909
export TARGET_FAMILY=8909
export CHIPSET=msm8909
export CHIPSET_PMIC=pm8916

python ./build_common.py $@