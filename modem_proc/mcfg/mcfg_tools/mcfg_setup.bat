REM ******************************************************************************
REM * %header%
REM * %datetime%
REM *
REM * %description%
REM ******************************************************************************
REM *
REM * copyright (c) 2015 qualcomm technologies, inc.
REM * all rights reserved.
REM * qualcomm technologies, inc. confidential and proprietary.
REM *
REM ******************************************************************************
@echo off
rem ==========================================================================
rem
rem  Purpose: 
rem    Copy files from build to local machine for MCFG config generation       
rem  
rem  Usage: 
rem    mcfg_setup.bat [ destination_directory ], 
rem    e.g.   mcfg_setup.bat c:\temp\mcfg_eng_creation
rem         
rem  Note: 
rem    Script will prompt user for a destination if not provided
rem    on the command line. 
rem
rem --------------------------------------------------------------------------
rem Copyright (c) 2009-2015 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem ==========================================================================

perl %~dp0\mcfg_setup.pl %*
PAUSE
