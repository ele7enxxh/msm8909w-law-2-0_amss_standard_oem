@echo off

rem ==========================================================================
rem
rem  SU-level build script
rem
rem  General Description
rem     build batch file.
rem
rem Copyright (c) 2009-2009 by QUALCOMM Technologies, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem
rem --------------------------------------------------------------------------
rem
rem $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/build/build.cmd#1 $
rem
rem ==========================================================================

setlocal

rem # This command file can be called two ways:
rem # 1 - From the top level make process -- BUILD variable
rem #     will be defined
rem # 2 - Directly invoked -- BUILD_ID (8-letter form),
rem #     must be specified on the command line

rem # Search for BUILD_ID in calling params, if found, then
rem # ready to execute Core BSP build command

echo %* | findstr BUILD_ID > NUL
if %errorlevel% NEQ 0 (
  rem # BUILD_ID not found on command line
  rem # Now check if BUILD environment variable is set by top level make

  if not defined BUILD (
    echo.
    echo Error -- BUILD_ID [8-letter form] not specified on command line
    echo.
    echo Usage -- %0 %* BUILD_ID=XXXXXXXX
    echo.
    exit /b
  ) else (
    rem # Invoked from top level make command
    rem # Only use the first 8 charactes of BUILD variable
    SET BUILD_ID_STR=BUILD_ID=%BUILD:~0,8%
  )
)

rem # Execute the CoreBSP build script
call %~dp0..\..\..\core\bsp\build\build.cmd %* %BUILD_ID_STR%
