@echo off
rem ==========================================================================
rem
rem  RPM build system launcher
rem
rem Copyright (c) 2015 by QUALCOMM Technologies, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem ==========================================================================
rem $Header: //components/rel/rpm.bf/2.1.1/build/build_8909w.bat#1 $

SET BUILD_ASIC=8909W
SET MSM_ID=8909W
SET HAL_PLATFORM=8909
SET TARGET_FAMILY=8909
SET CHIPSET=msm8909
SET CHIPSET_PMIC=pm8916
SET SECPOLICY=USES_SEC_POLICY_DEFAULT_SIGN
python build_common.py %*

SET BUILD_ASIC=8909W
SET MSM_ID=8909W
SET HAL_PLATFORM=8909
SET TARGET_FAMILY=8909
SET CHIPSET=msm8909
SET CHIPSET_PMIC=pm8909
SET SECPOLICY=USES_SEC_POLICY_DEFAULT_SIGN
python build_common.py %*