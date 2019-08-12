@echo off
rem ==========================================================================
rem
rem  RPM build system launcher
rem
rem Copyright (c) 2012 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem ==========================================================================
rem $Header: //components/rel/rpm.bf/2.1.1/build/build_8976.bat#1 $

SET BUILD_ASIC=8976
SET MSM_ID=8976
SET HAL_PLATFORM=8976
SET TARGET_FAMILY=8976
SET CHIPSET=msm8976

python build_common.py %*
