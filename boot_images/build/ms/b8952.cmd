@echo off
rem ==========================================================================
rem
rem  CBSP Buils system
rem
rem  General Description
rem     build batch file.
rem
rem Copyright 2013 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem
rem --------------------------------------------------------------------------
rem
rem $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/build/ms/b8952.cmd#1 $
rem $DateTime: 2015/09/01 00:30:35 $
rem $Author: pwbldsvc $
rem $Change: 8935858 $
rem                      EDIT HISTORY FOR FILE
rem
rem  This section contains comments describing changes made to the module.
rem  Notice that changes are listed in reverse chronological order.
rem
rem when       who     what, where, why
rem --------   ---     ---------------------------------------------------------
rem 02/13/14   sve      Initial version
rem 
rem ==========================================================================


rem ========  If we have a set environment batch file, call it to set KLOCWORK and RVCT version.========
IF EXIST setenv.cmd CALL setenv.cmd

rem ===== Setup Paths=====================================================
SET BUILD_ROOT=%~dp0..\..
SET TOOLS_SCONS_ROOT=%BUILD_ROOT%\tools\build\scons
SET LOGNAM=%~n0

setlocal 
REM Setup Default Values
SET BUILD_ID=SAASANAA
SET BUILD_VER=40000000
REM SET BUILDSPEC=KLOCWORK

REM Common
SET BUILD_ASIC=8952A
SET MSM_ID=8952
SET HAL_PLATFORM=8952
SET TARGET_FAMILY=8952
SET CHIPSET=msm8952
SET IMAGES=%1

setlocal
REM Prase Command Input
:LOOP
IF "%1"=="" GOTO CONTINUE

IF "%1"=="BUILD_ID" (
 SET BUILD_ID=%2
)
IF "%1"=="BUILD_VER" (
  SET BUILD_VER=%2
)
IF "%1"=="MSM_ID" (
  SET MSM_ID=%2
)
IF "%1"=="HAL_PLATFORM" (
  SET  HAL_PLATFORM=%2
)
IF "%1"=="TARGET_FAMILY" (
  SET TARGET_FAMILY=%2
)
IF "%1"=="BUILD_ASIC" (
  SET BUILD_ASIC=%2
)
IF "%1"=="CHIPSET" (
  SET CHIPSET=%2
) 
SHIFT
GOTO LOOP 

:CONTINUE
SET BUILD_CMD=BUILD_ID=%BUILD_ID% BUILD_VER=%BUILD_VER% MSM_ID=%MSM_ID% HAL_PLATFORM=%HAL_PLATFORM% TARGET_FAMILY=%TARGET_FAMILY% BUILD_ASIC=%BUILD_ASIC% CHIPSET=%CHIPSET% --tcfgf=8952.target.builds %*

REM Create build log
rem @echo CALL %TOOLS_SCONS_ROOT%\build\rename-log.cmd %LOGNAM%-log.txt
rem CALL %TOOLS_SCONS_ROOT%\build\rename-log.cmd %LOGNAM%-log.txt

@echo CALL %TOOLS_SCONS_ROOT%\build\build.cmd -f target.scons %BUILD_CMD%
rem CALL %TOOLS_SCONS_ROOT%\build\build.cmd -f target.scons %BUILD_CMD%
CALL %TOOLS_SCONS_ROOT%\build\build.cmd -f %TOOLS_SCONS_ROOT%\build\start.scons %BUILD_CMD%

REM rename default build log
rem REN build-log.txt %LOGNAM%-log.txt

REM If there is an error stop compiling
IF  %ERRORLEVEL% GTR 0 (
  goto ERROR
) ELSE (
  goto END
)

:ERROR
@echo "Fail to compile 8952. Exiting...."
@EXIT /B %ERRORLEVEL% 

:END
REM Pass any overloading variable to the main script, if appilcable
SET BuildEnv=^
  SET BUILD_VER=%BUILD_VER%^&^
  SET MSM_ID=%MSM_ID%^&^
  SET HAL_PLATFORM=%HAL_PLATFORM%^&^
  SET TARGET_FAMILY=%TARGET_FAMILY%^&^
  SET BUILD_ASIC=%BUILD_ASIC%^&^
  SET CHIPSET=%CHIPSET%^&^
  SET BUILD_ID=%BUILD_ID%
  
endlocal & %BuildEnv%
