@echo off
rem ==========================================================================
rem
rem  CBSP Buils system
rem
rem  General Description
rem     SetEnv batch file.
rem
rem Copyright (c) 2011 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem
rem --------------------------------------------------------------------------
rem
rem $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/build/ms/setenv.cmd#1 $
rem $DateTime: 2015/09/01 00:30:35 $
rem $Author: pwbldsvc $
rem $Change $
rem                      EDIT HISTORY FOR FILE
rem
rem  This section contains comments describing changes made to the module.
rem  Notice that changes are listed in reverse chronological order.
rem
rem when       who     what, where, why
rem --------   ---     ---------------------------------------------------------
rem  09/09/12  sy    Switch to use ARMCT5.01 Build 94
rem  03/20/12  dh    Switch to use ARMCT5.01
rem  12/28/11  sy    Adding a local script to overwrite the network script  
rem                  if users have a local copy of the RVCT compiler
rem  09/19/11  sy    Initial revision
rem 
rem ==========================================================================

rem ========== Call the RVCT version ==========================

CALL \\stone\aswcrm\smart\nt\bin\ARMCT501B94.cmd 

rem ======== Set Klocwork =============================
SET BUILDSPEC=KLOCWORK


REM ======================================================
rem Echo the environment to stdout so that calling scripts can capture it:
set
