# ==========================================================================
#
#  RPM build system launcher
#
# Copyright (c) 2012 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
# ==========================================================================
# $Header: //components/rel/rpm.bf/2.1.1/build/build_9x07.sh#1 $

export BUILD_ASIC=9x07
export MSM_ID=9x07
export HAL_PLATFORM=9x07
export TARGET_FAMILY=9x07
export CHIPSET=mdm9x07
export SECPOLICY=USES_SEC_POLICY_DEFAULT_SIGN
cd './rpm_proc/build/'

python ./build_common.py $@


