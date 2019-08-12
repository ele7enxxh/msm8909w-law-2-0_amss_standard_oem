@echo off
rem ==========================================================================
rem
rem  RPM build system launcher
rem
rem Copyright (c) 2012 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem ==========================================================================
rem $Header: //components/rel/rpm.bf/2.1.1/build/build_8909.bat#2 $

SET BUILD_ASIC=8909
SET MSM_ID=8909
SET HAL_PLATFORM=8909
SET TARGET_FAMILY=8909
SET CHIPSET=msm8909
SET CHIPSET_PMIC=pm8909
SET SECPOLICY=USES_SEC_POLICY_DEFAULT_SIGN
python build_common.py %*

SET BUILD_ASIC=8909
SET MSM_ID=8909
SET HAL_PLATFORM=8909
SET TARGET_FAMILY=8909
SET CHIPSET=msm8909
SET CHIPSET_PMIC=pm8916

python build_common.py %*
