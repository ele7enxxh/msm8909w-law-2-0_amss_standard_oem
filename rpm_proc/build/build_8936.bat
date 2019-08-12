@echo off
rem ==========================================================================
rem
rem  RPM build system launcher
rem
rem Copyright (c) 2012 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem ==========================================================================
rem $Header: //components/rel/rpm.bf/2.1.1/build/build_8936.bat#1 $

SET BUILD_ASIC=8936
SET MSM_ID=8936
SET HAL_PLATFORM=8936
SET TARGET_FAMILY=8936
SET CHIPSET=msm8936

python build_common.py %*
