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
rem $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/ms/build.cmd#1 $
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
rem
rem ==========================================================================

rem
rem PLEASE note that this script MUST be called with the target and options at
rem then end of command.
rem Syntax: build.cmd BUILD_ID=XYZ CHIPSET=ABC tz --verbose=2
rem We first parse out the chipset build options then everything else must come
rem later.
rem

setlocal

rem ============== Call the set environment batch file ===================
CALL setenv.cmd

rem ===== Setup Paths=====================================================
SET BUILD_ROOT=%~dp0..\..
SET CORE_BSP_ROOT=%BUILD_ROOT%\core\bsp
SET TOOLS_SCONS_ROOT=%BUILD_ROOT%\tools\build\scons
SET CMD_ARGS=%*

setlocal
:LOOP

IF /I "%1" equ "" GOTO CONTINUE

IF /I "%1" equ "BUILD_ID" (
 SET BUILD_ID=%2
 GOTO SHIFT2
)
IF /I "%1" equ "BUILD_VER" (
  SET BUILD_VER=%2
  GOTO SHIFT2
)
IF /I "%1" equ "MSM_ID" (
  SET MSM_ID=%2
  GOTO SHIFT2
)
IF /I "%1" equ "HAL_PLATFORM" (
  SET  HAL_PLATFORM=%2
  GOTO SHIFT2
)
IF /I "%1" equ "TARGET_FAMILY" (
  SET TARGET_FAMILY=%2
  GOTO SHIFT2
)
IF /I "%1" equ "BUILD_ASIC" (
  SET BUILD_ASIC=%2
  GOTO SHIFT2
)
IF /I "%1" equ "CHIPSET" (
  SET CHIPSET=%2
  GOTO SHIFT2
)

SHIFT
GOTO LOOP

:SHIFT2
SHIFT
SHIFT
GOTO LOOP

:CONTINUE
IF /I "%MSM_ID%" equ "8916" (
  set _result=%CMD_ARGS:*8916=%
  goto SET_8916
)
IF /I "%CHIPSET%" equ "msm8916" (
  set _result=%CMD_ARGS:*msm8916=%
  goto SET_8916
)
IF /I "%HAL_PLATFORM%" equ "8916" (
  set _result=%CMD_ARGS:*8916=%
  goto SET_8916
)
IF /I "%TARGET_FAMILY%" equ "8916" (
  set _result=%CMD_ARGS:*8916=%
  goto SET_8916
)
IF /I "%MSM_ID%" equ "8936" (
  set _result=%CMD_ARGS:*8936=%
  goto SET_8936
)
IF /I "%CHIPSET%" equ "msm8936" (
  set _result=%CMD_ARGS:*msm8936=%
  goto SET_8936
)
IF /I "%HAL_PLATFORM%" equ "8936" (
  set _result=%CMD_ARGS:*8936=%
  goto SET_8936
)
IF /I "%TARGET_FAMILY%" equ "8936" (
  set _result=%CMD_ARGS:*8936=%
  goto SET_8936
)
IF /I "%MSM_ID%" equ "8909" (
  set _result=%CMD_ARGS:*8909=%
  goto SET_8909
)
IF /I "%CHIPSET%" equ "msm8909" (
  set _result=%CMD_ARGS:*msm8909=%
  goto SET_8909
)
IF /I "%HAL_PLATFORM%" equ "8909" (
  set _result=%CMD_ARGS:*8909=%
  goto SET_8909
)
IF /I "%TARGET_FAMILY%" equ "8909" (
  set _result=%CMD_ARGS:*8909=%
  goto SET_8909
  
)
IF /I "%MSM_ID%" equ "8909w" (
  set _result=%CMD_ARGS:*8909w=%
  goto SET_8909w
)
IF /I "%CHIPSET%" equ "msm8909w" (
  set _result=%CMD_ARGS:*msm8909w=%
  goto SET_8909w
)
IF /I "%HAL_PLATFORM%" equ "8909w" (
  set _result=%CMD_ARGS:*8909w=%
  goto SET_8909w
)
IF /I "%TARGET_FAMILY%" equ "8909w" (
  set _result=%CMD_ARGS:*8909w=%
  goto SET_8909w
)
IF /I "%MSM_ID%" equ "9x07" (
  set _result=%CMD_ARGS:*9x07=%
  goto SET_9x07
)
IF /I "%CHIPSET%" equ "mdm9x07" (
  set _result=%CMD_ARGS:*mdm9x07=%
  goto SET_9x07
)
IF /I "%HAL_PLATFORM%" equ "9x07" (
  set _result=%CMD_ARGS:*9x07=%
  goto SET_9x07
)
IF /I "%TARGET_FAMILY%" equ "9x07" (
  set _result=%CMD_ARGS:*9x07=%
  goto SET_9x07
)
IF /I "%MSM_ID%" equ "9x09" (
  set _result=%CMD_ARGS:*9x09=%
  goto SET_9x09
)
IF /I "%CHIPSET%" equ "mdm9x09" (
  set _result=%CMD_ARGS:*msm8909=%
  goto SET_9x09
)
IF /I "%HAL_PLATFORM%" equ "9x09" (
  set _result=%CMD_ARGS:*9x09=%
  goto SET_9x09
)
IF /I "%TARGET_FAMILY%" equ "9x09" (
  set _result=%CMD_ARGS:*9x09=%
  goto SET_9x09
)

echo "Please Specify the CHIPSET to build"
EXIT /B


:SET_8909
rem Below BUILD_ID is temporary until we have full details on 8909 mem config
SET BUILD_ID=MAZAANAA
SET BUILD_VER=40000000
SET BUILD_ASIC=8909A
SET MSM_ID=8909
SET HAL_PLATFORM=8909
SET TARGET_FAMILY=8909
SET CHIPSET=msm8909
SET _tempresult=%_result:hdcpapp=%

IF NOT "%_result%" == "%_tempresult%" (
  SET _tempresult=%_result:-c=%
  IF NOT "%_result%" == "%_tempresult%" (
    DEL %BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%\dxhdcp2*"
  ) ELSE (
    MKDIR %BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%
    COPY "%CORE_BSP_ROOT%\trustzone\qsapps\hdcpapp\build\*.*" "%BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%\*.*"
  )
)


SET _result=%_result:hdcpapp=%
goto DO_BUILD

:SET_8909w
rem Below BUILD_ID is temporary until we have full details on 8909w mem config         
SET BUILD_ID=MATAANAA
SET BUILD_VER=40000000
SET BUILD_ASIC=8909A
SET MSM_ID=8909w
SET HAL_PLATFORM=8909w
SET TARGET_FAMILY=8909w
SET CHIPSET=msm8909w
SET _tempresult=%_result:hdcpapp=%

IF NOT "%_result%" == "%_tempresult%" (
  SET _tempresult=%_result:-c=%
  IF NOT "%_result%" == "%_tempresult%" (
    DEL %BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%\dxhdcp2*"
  ) ELSE (
    MKDIR %BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%
    COPY "%CORE_BSP_ROOT%\trustzone\qsapps\hdcpapp\build\*.*" "%BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%\*.*"
  )
)


SET _result=%_result:hdcpapp=%
goto DO_BUILD

:SET_9x07
rem Below BUILD_ID is temporary until we have full details on 9x07 mem config
SET BUILD_ID=MARAANAA
SET BUILD_VER=40000000
SET BUILD_ASIC=9x07A
SET MSM_ID=9x07
SET HAL_PLATFORM=9x07
SET TARGET_FAMILY=9x07
SET CHIPSET=mdm9x07
SET _tempresult=%_result:hdcpapp=%

IF NOT "%_result%" == "%_tempresult%" (
  SET _tempresult=%_result:-c=%
  IF NOT "%_result%" == "%_tempresult%" (
    DEL %BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%\dxhdcp2*"
  ) ELSE (
    MKDIR %BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%
    COPY "%CORE_BSP_ROOT%\trustzone\qsapps\hdcpapp\build\*.*" "%BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%\*.*"
  )
)


SET _result=%_result:hdcpapp=%
goto DO_BUILD

:SET_9x09
rem Below BUILD_ID is temporary until we have full details on 9x09 mem config
SET BUILD_ID=NAYAANAA
SET BUILD_VER=40000000
SET BUILD_ASIC=9x09A
SET MSM_ID=9x09
SET HAL_PLATFORM=9x09
SET TARGET_FAMILY=9x09
SET CHIPSET=mdm9x09
SET _tempresult=%_result:hdcpapp=%

IF NOT "%_result%" == "%_tempresult%" (
  SET _tempresult=%_result:-c=%
  IF NOT "%_result%" == "%_tempresult%" (
    DEL %BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%\dxhdcp2*"
  ) ELSE (
    MKDIR %BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%
    COPY "%CORE_BSP_ROOT%\trustzone\qsapps\hdcpapp\build\*.*" "%BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%\*.*"
  )
)


SET _result=%_result:hdcpapp=%
goto DO_BUILD

:SET_8936
SET BUILD_ID=MAWAANAA
SET BUILD_VER=40000000
SET BUILD_ASIC=8936A
SET MSM_ID=8936
SET HAL_PLATFORM=8936
SET TARGET_FAMILY=8936
SET CHIPSET=msm8936
SET _tempresult=%_result:hdcpapp=%

IF NOT "%_result%" == "%_tempresult%" (
  SET _tempresult=%_result:-c=%
  IF NOT "%_result%" == "%_tempresult%" (
    DEL %BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%\dxhdcp2*"
  ) ELSE (
    MKDIR %BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%
    COPY "%CORE_BSP_ROOT%\trustzone\qsapps\hdcpapp\build\*.*" "%BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%\*.*"
  )
)


SET _result=%_result:hdcpapp=%
goto DO_BUILD

:SET_8916
rem Below BUILD_ID is temporary until we have full details on 8916 mem config
SET BUILD_ID=MAUAANAA
SET BUILD_VER=40000000
SET BUILD_ASIC=8916A
SET MSM_ID=8916
SET HAL_PLATFORM=8916
SET TARGET_FAMILY=8916
SET CHIPSET=msm8916
SET _tempresult=%_result:hdcpapp=%

IF NOT "%_result%" == "%_tempresult%" (
  SET _tempresult=%_result:-c=%
  IF NOT "%_result%" == "%_tempresult%" (
    DEL %BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%\dxhdcp2*"
  ) ELSE (
    MKDIR %BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%
    COPY "%CORE_BSP_ROOT%\trustzone\qsapps\hdcpapp\build\*.*" "%BUILD_ROOT%\build\ms\bin\PIL_IMAGES\SPLITBINS_%BUILD_ID%\*.*"
  )
)


SET _result=%_result:hdcpapp=%
goto DO_BUILD


:DO_END
EXIT /B

SET IMAGES=%1
REM SET BUILDSPEC=KLOCWORK

:DO_BUILD
SET BUILD_CMD=BUILD_ID=%BUILD_ID% BUILD_VER=%BUILD_VER% MSM_ID=%MSM_ID% HAL_PLATFORM=%HAL_PLATFORM% TARGET_FAMILY=%TARGET_FAMILY% BUILD_ASIC=%BUILD_ASIC% CHIPSET=%CHIPSET% %_result%

CALL %TOOLS_SCONS_ROOT%\build\rename-log.cmd

@echo CALL %TOOLS_SCONS_ROOT%\build\build.cmd -f target.scons %BUILD_CMD%

CALL %TOOLS_SCONS_ROOT%\build\build.cmd -f target.scons %BUILD_CMD%

