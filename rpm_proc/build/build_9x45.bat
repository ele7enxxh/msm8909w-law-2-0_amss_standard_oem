@echo off
rem ==========================================================================
rem
rem  RPM build system launcher
rem
rem Copyright (c) 2012 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem ==========================================================================
rem $Header: //components/rel/rpm.bf/2.1.1/build/build_9x45.bat#1 $

SET BUILD_ASIC=9x45
SET MSM_ID=9x45
SET HAL_PLATFORM=9x45
SET TARGET_FAMILY=9x45
SET CHIPSET=mdm9x45
SET SECPOLICY=USES_SEC_POLICY_DEFAULT_SIGN

python build_common.py %*
