@echo off
rem ==========================================================================
rem
rem  CBSP Build system
rem
rem  General Description
rem     Build script for TZBSP Test Suite
rem
rem Copyright (c) 2012 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem
rem --------------------------------------------------------------------------
rem
rem $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/ms/build_tztest.cmd#1 $
rem $DateTime: 2016/06/17 14:31:11 $
rem $Author: pwbldsvc $
rem $Change: 10727476 $
rem                      EDIT HISTORY FOR FILE
rem
rem  This section contains comments describing changes made to the module.
rem  Notice that changes are listed in reverse chronological order.
rem
rem when       who     what, where, why
rem --------   ---     ---------------------------------------------------------
rem 06/29/12   jl      Initial revision.
rem ==========================================================================

rem
rem The following script builds TrustZone Test images, e.g.
rem   tzbsp_with_test.mbn
rem   tztestexec.mbn
rem   emmc_appsboot.mbn
rem

setlocal

rem ===== Setup Paths=====================================================
SET BUILD_ROOT=%~dp0..\..
SET ARGS=%*
SET CLEAN=0

rem -- ARGUMENT PARSING --

:LOOP

IF "%1"=="" GOTO CONTINUE

IF "%1"=="CHIPSET" (
  SET CHIPSET=%2
  GOTO SHIFT2
)
IF "%1"=="BUILD_ID" (
  SET BUILD_ID=%2
  GOTO SHIFT2
)
IF "%1"=="-c" (
  SET CLEAN=1
  GOTO SHIFT
)

:SHIFT
SHIFT
GOTO LOOP

:SHIFT2
SHIFT
SHIFT
GOTO LOOP

:CONTINUE

rem -- END ARGUMENT PARSING --

IF %CHIPSET%=="" (
  echo Please specify a chipset e.g. CHIPSET=msm8916
  EXIT /B
)
IF "%CHIPSET%"=="msm8916" (
  SET BUILD_ID=MAUAANAA
  SET VENUS=1
  SET MAPLE=0
  GOTO BUILD
)
IF "%CHIPSET%"=="msm8936" (
  SET BUILD_ID=MAWAANAA
  SET VENUS=1
  SET MAPLE=0
  GOTO BUILD
)
IF "%CHIPSET%"=="msm8909" (
  SET BUILD_ID=MAZAANAA
  SET VENUS=1
  SET MAPLE=0
  GOTO BUILD
)
IF "%CHIPSET%"=="mdm9x07" (
  SET BUILD_ID=MARAANAA
  SET VENUS=0
  SET MAPLE=0
  GOTO BUILD
)

echo Unknown chipset...
EXIT /B

:BUILD

del %BUILD_ROOT%\build\ms\bin\%BUILD_ID%\tzbsp_with_test.mbn
CALL %BUILD_ROOT%\build\ms\build.cmd %ARGS% tzbsp_with_test
IF %CLEAN% EQU 1 GOTO SUCCESS_tzbsp_with_test
IF EXIST %BUILD_ROOT%\build\ms\bin\%BUILD_ID%\tzbsp_with_test.mbn GOTO SUCCESS_tzbsp_with_test
echo **Build errors...
echo tzbsp_with_test.mbn file not found for %CHIPSET%. Please refer to build log for build errors.
EXIT /B
:SUCCESS_tzbsp_with_test
GOTO BUILD_hyp

:BUILD_hyp
del %BUILD_ROOT%\build\ms\bin\%BUILD_ID%\hyp.mbn
CALL %BUILD_ROOT%\build\ms\build.cmd %ARGS% hyp
IF %CLEAN% EQU 1 GOTO SUCCESS_hyp
IF EXIST %BUILD_ROOT%\build\ms\bin\%BUILD_ID%\tzbsp_with_test.mbn GOTO SUCCESS_hyp
echo **Build errors...
echo hyp.mbn file not found for %CHIPSET%. Please refer to build log for build errors.
EXIT /B
:SUCCESS_hyp
GOTO BUILD_tztestexec

:BUILD_tztestexec
CALL %BUILD_ROOT%\build\ms\build.cmd %ARGS% tztestexec
IF %CLEAN% EQU 1 GOTO SUCCESS_tztestexec
IF EXIST %BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%\tztestexec.mdt GOTO SUCCESS_tztestexec
echo **Build errors...
echo tztestexec.mdt file not found for %CHIPSET%. Please refer to build log for build errors.
EXIT /B
:SUCCESS_tztestexec
IF %VENUS% EQU 1 (
  GOTO BUILD_dummy_venus
)
GOTO SUCCESS_dummy_venus

:BUILD_dummy_venus
del %BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%\dummy_venus.mdt
CALL %BUILD_ROOT%\build\ms\build.cmd %ARGS% dummy_venus
IF %CLEAN% EQU 1 GOTO SUCCESS_dummy_venus
IF EXIST %BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%\dummy_venus.mdt GOTO SUCCESS_dummy_venus
echo **Build errors...
echo mod_dummy_venus.mdt file not found for %CHIPSET%. Please refer to build log for build errors.
EXIT /B
:SUCCESS_dummy_venus
IF %MAPLE% EQU 1 (
  GOTO BUILD_dummy_maple
)
GOTO BUILD_tzbsptest

:BUILD_dummy_maple
del %BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%\dummy_maple.mdt
CALL %BUILD_ROOT%\build\ms\build.cmd %ARGS% dummy_maple
IF %CLEAN% EQU 1 GOTO SUCCESS_dummy_maple
IF EXIST %BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%\dummy_maple.mdt GOTO SUCCESS_dummy_maple
echo **Build errors...
echo mod_dummy_maple.mdt file not found for %CHIPSET%. Please refer to build log for build errors.
EXIT /B
:SUCCESS_dummy_maple
GOTO BUILD_tzbsptest

:BUILD_tzbsptest
CALL %BUILD_ROOT%\build\ms\build.cmd %ARGS% tzbsptest as_appsbl=1
IF %CLEAN% EQU 1 GOTO SUCCESS_tzbsptest
IF EXIST %BUILD_ROOT%\build\ms\bin\%BUILD_ID%\emmc_appsboot.mbn GOTO SUCCESS_tzbsptest
echo **Build errors...
echo emmc_appsboot.mbn file not found for %CHIPSET%. Please refer to build log for build errors.
EXIT /B
:SUCCESS_tzbsptest
GOTO BUILD_SUCCESS

:BUILD_SUCCESS
IF %CLEAN% EQU 1 GOTO DONE
echo ==============================================================================
echo BUILD_ALL.cmd build summary
echo ==============================================================================
echo %BUILD_ID%\tzbsp_with_test.mbn
echo PIL_IMAGES\SPLITBINS_%BUILD_ID%\tztestexec.mdt
IF %VENUS% EQU 1 (
  echo PIL_IMAGES\SPLITBINS_%BUILD_ID%\mod_dummy_venus.mdt
)
IF %MAPLE% EQU 1 (
  echo PIL_IMAGES\SPLITBINS_%BUILD_ID%\mod_dummy_maple.mdt
)
echo %BUILD_ID%\emmc_appsboot.mbn
echo ==============================================================================
echo TO LOAD: Boot device to fastboot and execute:
echo   fastboot flash tz tzbsp_with_test.mbn
echo   fastboot flash aboot emmc_appsboot.mbn
echo HAPPY DEBUGGING!
echo ==============================================================================
echo BUILDING COMPLETE

:DONE

endlocal
