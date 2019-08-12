@echo off
rem ==========================================================================
rem
rem  CBSP Buils system
rem
rem  General Description
rem     build batch file.
rem
rem Copyright (c) 2009-2009 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem
rem --------------------------------------------------------------------------
rem
rem $Header: //source/qcom/qct/core/pkg/wcnss/pr/rel/1.0/Pronto/bsp/build/resign_wcnss_build.cmd#1 $
rem $DateTime: 2012/06/13 17:34:39 $
rem $Author: coresvc $
rem $Change: 2501101 $
rem                      EDIT HISTORY FOR FILE
rem
rem  This section contains comments describing changes made to the module.
rem  Notice that changes are listed in reverse chronological order.
rem
rem when       who     what, where, why
rem --------   ---     ---------------------------------------------------------
rem
rem ==========================================================================
setlocal

SET BUILD_ID=SCAQBAF
SET BUILD_VER=0111
set _CMDPARA=%*

if /i %1 == 8974              goto SET_8974
if /i %1 == 8064              goto SET_8064
if /i %1 == 8930              goto SET_8930

                       
goto END

:SET_8974
if /i %2 == pronto_r33d       goto SET_8974_R33D
SET BUILD_ASIC=8974
SET MSM_ID=8974
SET HAL_PLATFORM=8974
SET TARGET_FAMILY=8974
SET CHIPSET=msm8974
SET _CMDARGU=%_CMDPARA:8974=%
SET BUILD_CMD=BUILD_ID=%BUILD_ID% BUILD_VER=%BUILD_VER% MSM_ID=%MSM_ID% HAL_PLATFORM=%HAL_PLATFORM% TARGET_FAMILY=%TARGET_FAMILY% BUILD_ASIC=%BUILD_ASIC% CHIPSET=%CHIPSET% modifycfg_img
goto END

:SET_8974_R33D
SET BUILD_ID=SCARBAF
SET BUILD_ASIC=8974
SET MSM_ID=8974
SET HAL_PLATFORM=8974
SET TARGET_FAMILY=8974
SET CHIPSET=msm8974
SET _CMDARGU=%_CMDPARA:8974=%
SET BUILD_CMD=BUILD_ID=%BUILD_ID% BUILD_VER=%BUILD_VER% MSM_ID=%MSM_ID% HAL_PLATFORM=%HAL_PLATFORM% TARGET_FAMILY=%TARGET_FAMILY% BUILD_ASIC=%BUILD_ASIC% CHIPSET=%CHIPSET% modifycfg_img
goto END

:SET_8930_p2
SET BUILD_ASIC=8960
SET MSM_ID=8960
SET HAL_PLATFORM=8960
SET TARGET_FAMILY=8960
SET CHIPSET=msm8960
SET _CMDARGU=%_CMDPARA:8930_p2=%
SET BUILD_CMD=BUILD_ID=%BUILD_ID% BUILD_VER=%BUILD_VER% MSM_ID=%MSM_ID% HAL_PLATFORM=%HAL_PLATFORM% TARGET_FAMILY=%TARGET_FAMILY% BUILD_ASIC=%BUILD_ASIC% CHIPSET=%CHIPSET% USES_FLAGS=USES_8038_PMIC modifycfg_img
goto END


:SET_8064
SET BUILD_ASIC=8064
SET MSM_ID=8064
SET HAL_PLATFORM=8064
SET TARGET_FAMILY=8064
SET CHIPSET=apq8064
SET _CMDARGU=%_CMDPARA:8064=%
SET BUILD_CMD=BUILD_ID=%BUILD_ID% BUILD_VER=%BUILD_VER% MSM_ID=%MSM_ID% HAL_PLATFORM=%HAL_PLATFORM% TARGET_FAMILY=%TARGET_FAMILY% BUILD_ASIC=%BUILD_ASIC% CHIPSET=%CHIPSET% USES_FLAGS=USES_RUMI,USES_VIRTIO modifycfg_img
goto END


:END

echo %BUILD_CMD%
build.cmd %BUILD_CMD%