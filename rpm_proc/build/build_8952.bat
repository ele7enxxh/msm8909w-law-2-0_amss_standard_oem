@echo off
rem ==========================================================================
rem
rem  RPM build system launcher
rem
rem Copyright (c) 2012 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem ==========================================================================
rem $Header: //components/rel/rpm.bf/2.1.1/build/build_8952.bat#1 $

SET BUILD_ASIC=8952
SET MSM_ID=8952
SET HAL_PLATFORM=8952
SET TARGET_FAMILY=8952
SET CHIPSET=msm8952

python build_common.py %*
