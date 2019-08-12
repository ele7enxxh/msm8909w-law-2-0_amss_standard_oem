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
rem $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/ms/setenv.cmd#1 $
rem $DateTime: 2016/06/17 14:31:11 $
rem $Author: pwbldsvc $
rem $Change $
rem                      EDIT HISTORY FOR FILE
rem
rem  This section contains comments describing changes made to the module.
rem  Notice that changes are listed in reverse chronological order.
rem
rem when       who     what, where, why
rem --------   ---     ---------------------------------------------------------
rem  09/19/11  sy    Initial revision
rem 
rem ==========================================================================

rem ========== Call the RVCT version ==========================
IF EXIST C:\Apps\ARMCT5.01\94 (
    CALL \\stone\aswcrm\smart\nt\bin\ARMCT501B94.cmd
) ELSE (
  IF EXIST C:\Apps\ARMCT5.01\43 (
      CALL \\stone\aswcrm\smart\nt\bin\ARMCT501B43.cmd
  ) ELSE (
      ECHO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      ECHO RVCT5 not found!  Please install "ARMUpdater - ARMUpdater ARMCT5.01 install" in "Run Advertised Programs" under "Control Panel"
      ECHO Using RVCT4 instead...
      ECHO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SLEEP 10
      CALL \\stone\aswcrm\smart\nt\bin\RVCT41P713.cmd
  )
)

rem ======== Set Klocwork =============================
SET BUILDSPEC=KLOCWORK

rem Dump environment to stdout so that calling scripts can read it.
set



