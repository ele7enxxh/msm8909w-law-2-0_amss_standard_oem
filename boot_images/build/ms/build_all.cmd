@echo off
rem ==========================================================================
rem
rem  CBSP Buils system
rem
rem  General Description
rem     build batch file.
rem
rem Copyright 2013,2014 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem
rem --------------------------------------------------------------------------
rem
rem $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/build/ms/build_all.cmd#1 $
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
rem 11/04/14   bs      Added for 8956 build command
rem 09/29/14   lm      Added for 8952 build command
rem 07/08/14   mrb     removed 8916/8936
rem 04/01/14   lm      Added for 8936 build command
rem 03/07/14   jz      Added for 9x45 build command
rem 10/07/13   ck      Removing targets that are not in Bear family
rem 09/12/13   ck      Adding 8916
rem 07/23/13   plc     Adding 8994
rem 06/26/13   plc     Adding 8092
rem 05/13/13    sy     Adding 9x35, 8974 
rem 05/06/13   plc     Add 8x62, remove redundant 8x10/8x26
rem 04/03/13    sy     Reduce 8x10/8x26 after branching
rem 04/16/13    dh     Add 8084
rem 06/10/12    sy     Initial version
rem ==========================================================================

SET LOGNAM=%~n0

@echo CALL %TOOLS_SCONS_ROOT%\build\rename-log.cmd 
CALL %TOOLS_SCONS_ROOT%\build\rename-log.cmd 

rem Remove all the pending old logs
del /q /f *-log.txt


CALL build.cmd TARGET_FAMILY=9x45 %*  2>>err-log.txt
CALL build.cmd TARGET_FAMILY=8909 %*  2>>err-log.txt
CALL build.cmd TARGET_FAMILY=8952 %*  2>>err-log.txt
CALL build.cmd TARGET_FAMILY=8956 %*  2>>err-log.txt


rem List all the errors in the log files for active users on the screen
grep -nH -a10 -b3 Exiting...  *.txt
