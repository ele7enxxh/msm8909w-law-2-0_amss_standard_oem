::/*============================================================================
::  FILE: module_build.cmd
::
::  This file contains the build script for individual modules
::  Usage: <build or clean> <module_name> <build_image_type>
::
::  For every new CRM build, compare and update the USES_FLAGS from
::  adsp_proc\build\bsp\adsp_proc_img\build\adsp_proc.scons
::
::  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
::  Qualcomm Technologies Proprietary and Confidential.
::============================================================================*/
::
::/*============================================================================
::  EDIT HISTORY FOR MODULE
::
::  This section contains comments describing changes made to the module.
::  Notice that changes are listed in reverse chronological order.
::
::  when        who  what, where, why
::  ----------  ---  -----------------------------------------------------------
::  2013-02-05  vh   Changed to compensate changes in build.cmd
::  2013-01-22  vh   Changed to compensate changes in build.cmd
::  2012-11-05  vh   Changed to compensate changes in build.cmd
::  2012-09-25  vh   Changed to compensate changes in build.cmd
::  2012-08-28  vh   Eliminated Environment Setting Errors
::  2012-08-22  vh   Initial version
::
::============================================================================*/

@echo off

set start_time=%time%

::set build environments
set BUILD_ID=AAAAAAAA
set BUILD_VER=1234
set VERBOSE=2

::validate input
if "%1"=="build" goto arg2
if "%1"=="clean" goto arg2
goto error

:arg2
if "%2"=="Sensors" goto arg3
if "%2"=="core" goto arg3
goto error

:arg3
::multi-pd image
if "%3"=="all" goto mpd
if "%3"=="mpd" (
:mpd
  set USES_FLAGS=USES_MULTI_PD
  set BUILD_IMAGE=msm8974_MPD
  goto rem_bin
)
::single-pd image
if "%3"=="spd" (
  set USES_FLAGS=SPD
  set BUILD_IMAGE=msm8974_SPD
  goto rem_bin
)
goto error

:: remove the binaries
:rem_bin
rm -rf ..\..\build\bsp\adsp_proc_img\build\%BUILD_ID%
rm -rf ..\..\build\bsp\devcfg_img\build\%BUILD_ID%
rm -rf ..\..\build\bsp\adsp_link\build\%BUILD_ID%
rm -f ..\..\dsp.elf
cd ..\..\obj\qdsp6v5_ReleaseG\LA\system\etc\firmware
rm -f *
cd ..\..\..\..\..\..\build\ms
rm -rf .\bin\%BUILD_ID%
rm -f *.elf

::select build or Clean
if "%1"=="build" goto build
if "%1"=="clean" goto clean
goto error

::build the image
:build
echo building %2 code
call build.cmd %BUILD_IMAGE% BUILD_ID=%BUILD_ID% BUILD_VER=%BUILD_VER% --filter=%2 --verbose=%VERBOSE% USES_FLAGS=%USES_FLAGS%

::link the image
echo linking %2 code
call build.cmd %BUILD_IMAGE% BUILD_ID=%BUILD_ID% BUILD_VER=%BUILD_VER% --verbose=%VERBOSE% USES_FLAGS=%USES_FLAGS%
cd ..\..\Sensors\build

::split the image
echo splitting the image
cd ..\..\obj\qdsp6v5_ReleaseG\LA\system\etc\firmware
python ..\..\..\..\..\..\qdsp6\scripts\pil-splitter.py ..\..\..\..\..\..\dsp.elf adsp
cd ..\..\..\..\..\..\Sensors\build
goto end

::clean the image
:clean
echo cleaning %2 code
cd ..\..\build\ms
call build.cmd %BUILD_IMAGE% BUILD_ID=%BUILD_ID% BUILD_VER=%BUILD_VER% --filter=%2 --verbose=%VERBOSE% -c
cd ..\..\Sensors\build
goto end

::error message
:error
echo "usage : build/clean Sensors/core mpd/spd/all"

:end
set end_time=%time%
echo %2 %1 start time = %start_time%
echo %2 %1 end time   = %end_time%
