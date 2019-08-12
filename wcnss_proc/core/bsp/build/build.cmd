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
rem $Header: //components/rel/core.wcnss/4.2.4/bsp/build/build.cmd#1 $
rem $DateTime: 2015/07/22 20:31:53 $
rem $Author: pwbldsvc $
rem $Change: 8650711 $
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
REM if we have a set environment batch file, call it.
IF EXIST %~dp0..\build\setenv.cmd CALL %~dp0..\build\setenv.cmd


SET BUILD_ID=SCAQBAF
SET BUILD_VER=0111
set _CMDPARA=%*

if /i %2 == 8974              goto SET_8974
if /i %2 == 8x10              goto SET_8x10
if /i %2 == 8x26              goto SET_8x26
if /i %2 == 8962              goto SET_8962
if /i %2 == 8916              goto SET_8916

goto END

:SET_8974
SET BUILD_ASIC=8974
SET MSM_ID=8974
SET HAL_PLATFORM=8974
SET TARGET_FAMILY=8974
SET CHIPSET=msm8974
SET _CMDARGU=%_CMDPARA:8974=%
SET BUILD_CMD=BUILD_ID=%BUILD_ID% BUILD_VER=%BUILD_VER% MSM_ID=%MSM_ID% HAL_PLATFORM=%HAL_PLATFORM% TARGET_FAMILY=%TARGET_FAMILY% BUILD_ASIC=%BUILD_ASIC% CHIPSET=%CHIPSET% %_CMDARGU%
goto END

:SET_8x26
SET BUILD_ID=NCAQBAF
SET BUILD_ASIC=8x26
SET MSM_ID=8x26
SET HAL_PLATFORM=8x26
SET TARGET_FAMILY=8x26
SET CHIPSET=msm8x26
SET _CMDARGU=%_CMDPARA:8x26=%
SET BUILD_CMD=BUILD_ID=%BUILD_ID% BUILD_VER=%BUILD_VER% MSM_ID=%MSM_ID% HAL_PLATFORM=%HAL_PLATFORM% TARGET_FAMILY=%TARGET_FAMILY% BUILD_ASIC=%BUILD_ASIC% CHIPSET=%CHIPSET% %_CMDARGU%
goto END

:SET_8916
SET BUILD_ID=NCAQBAF
SET BUILD_ASIC=8916
SET MSM_ID=8916
SET HAL_PLATFORM=8916
SET TARGET_FAMILY=8916
SET CHIPSET=msm8916
SET _CMDARGU=%_CMDPARA:8916=%
SET BUILD_CMD=BUILD_ID=%BUILD_ID% BUILD_VER=%BUILD_VER% MSM_ID=%MSM_ID% HAL_PLATFORM=%HAL_PLATFORM% TARGET_FAMILY=%TARGET_FAMILY% BUILD_ASIC=%BUILD_ASIC% CHIPSET=%CHIPSET% %_CMDARGU%
goto END

:SET_8x10
SET BUILD_ID=NCAQBAF
SET BUILD_ASIC=8x10
SET MSM_ID=8x10
SET HAL_PLATFORM=8x10
SET TARGET_FAMILY=8x10
SET CHIPSET=msm8x10
SET _CMDARGU=%_CMDPARA:8x10=%
SET BUILD_CMD=BUILD_ID=%BUILD_ID% BUILD_VER=%BUILD_VER% MSM_ID=%MSM_ID% HAL_PLATFORM=%HAL_PLATFORM% TARGET_FAMILY=%TARGET_FAMILY% BUILD_ASIC=%BUILD_ASIC% CHIPSET=%CHIPSET% %_CMDARGU%
goto END

:SET_8962
SET BUILD_ID=NCAQBAF
SET BUILD_ASIC=8962
SET MSM_ID=8962
SET HAL_PLATFORM=8962
SET TARGET_FAMILY=8962
SET CHIPSET=msm8962
SET _CMDARGU=%_CMDPARA:8962=%
SET BUILD_CMD=BUILD_ID=%BUILD_ID% BUILD_VER=%BUILD_VER% MSM_ID=%MSM_ID% HAL_PLATFORM=%HAL_PLATFORM% TARGET_FAMILY=%TARGET_FAMILY% BUILD_ASIC=%BUILD_ASIC% CHIPSET=%CHIPSET% %_CMDARGU%
goto END


:END

CALL %~dp0..\..\..\tools\build\scons\build\rename-log.cmd
CALL %~dp0..\..\..\tools\build\scons\build\build.cmd -f %~dp0..\..\..\Pronto\bsp\build\target.scons %BUILD_CMD%
