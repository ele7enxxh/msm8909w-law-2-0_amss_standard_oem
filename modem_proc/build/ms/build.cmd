@echo off
rem ==========================================================================
rem
rem  Image Top-Level Build Script
rem
rem  General Description
rem     build batch file.
rem
rem Copyright (c) 2009-2012 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem
rem --------------------------------------------------------------------------
rem
rem $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/build/ms/build.cmd#1 $
rem $DateTime: 2016/12/13 07:59:25 $
rem $Change: 11985153 $
rem
rem ==========================================================================

rem -- Call script to setup build environment, if it exists.
if exist setenv.cmd call setenv.cmd

rem -- Call the main build command
python build_variant.py %*
@exit /B %ERRORLEVEL%
