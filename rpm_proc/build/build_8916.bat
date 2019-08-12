@echo off
rem ==========================================================================
rem
rem  RPM build system launcher
rem
rem Copyright (c) 2012 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem ==========================================================================
rem $Header: //components/rel/rpm.bf/2.1.1/build/build_8916.bat#1 $

SET BUILD_ASIC=8916
SET MSM_ID=8916
SET HAL_PLATFORM=8916
SET TARGET_FAMILY=8916
SET CHIPSET=msm8916
SET SECPOLICY=USES_SEC_POLICY_DEFAULT_SIGN
python build_common.py %*
