@echo off
rem ==========================================================================
rem
rem  RPM build system launcher
rem
rem Copyright (c) 2009-2011 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem ==========================================================================
rem $Header: //components/rel/rpm.bf/2.1.1/build/build.bat#1 $

SET BUILD_ASIC=8974
SET MSM_ID=8974
SET HAL_PLATFORM=8974
SET TARGET_FAMILY=8974
SET CHIPSET=msm8974

python build_common.py %*

python mapread_bfam_splitmem.py ../.. > memory_%CHIPSET%.csv