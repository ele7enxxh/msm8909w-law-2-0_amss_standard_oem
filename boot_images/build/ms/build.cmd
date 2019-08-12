@echo off
rem ==========================================================================
rem
rem  CBSP Buils system
rem
rem  General Description
rem     build batch file.
rem
rem Copyright (c) 2015 by QUALCOMM Technologies, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem
rem --------------------------------------------------------------------------
rem
rem                      EDIT HISTORY FOR FILE
rem
rem  This section contains comments describing changes made to the module.
rem  Notice that changes are listed in reverse chronological order.
rem
rem when       who     what, where, why
rem --------   ---     ---------------------------------------------------------
rem 04/21/15   yps     Add support pm8916 for DDI
rem 03/20/15   sve     Added compilation support for Nand sbl and flash tools
rem 01/22/15   sk  	   Code changes for merging firehose DeviceProgrammer into SBL
rem 01/08/15   sc      Changes to generate two Elfs for RUMI and asic
rem 01/02/15   moh     Appended deviceprogrammer flags for external build flavor
rem 12/17/14   sve	   Code changes for merging firehose DeviceProgrammer into SBL
rem 12/15/14   yps     Fixed 8909 DDI prod compilation problem
rem 11/25/14   yps     Added 8909 DDI build cmd
rem 11/21/14   sk      changes for creating two elfs for RUMI and ASIC
rem 10/30/14   aus     Remove emmcbld compilation and NAND build support
rem 09/29/14   lm      Added 8952 emmc build support
rem 08/05/14   jz      Added 9x45 emmc build support
rem 07/08/14   mrb     Removing 8916/8936
rem 06/06/14   sb      Added 9x45 build flashtools support
rem 02/24/14   jz      Added 9x45 build support (without flashtools)
rem 02/13/14   sve     Added 8936 build command
rem 10/08/13   ck      Fixed 8916 default build id to match .builds file
rem 10/07/13   ck      Removed targets that are not in Bear family
rem 09/09/13   jz      Added jtagprogrammer, deviceprogrammer for 8084
rem 09/05/13   jz      Added jsdcc, emmcbld compilation for 8084
rem 08/20/13   sl      Added 8x10 compilation for DDR Debug Mode
rem 08/15/13   sl      Added 8x26 compilation for DDR Debug Mode
rem 09/12/13   ck      Added 8916 support
rem 07/23/13   plc     Added 8994 support
rem 07/03/13   ajs     Added deviceprogrammer to 8974, 8x10 and 8x26 targets
rem 06/26/13   plc     Added 8092 support
rem 06/13/13   kpa     Support 9x35 source compilation.
rem 05/13/13    sy     Fixing 9x35 builds
rem 05/06/13   plc     Added 8x62 support and fix comments for 8084
rem 04/30/13    jz     Fixed issue of missing jsdcc build
rem 03/27/13    dh     Remove jsdcc compilation for 8084
rem 04/08/13    sg     adding emmcbld for 8x10 and 8x26
rem 03/05/13    kedara Fix bootlog-log.txt
rem 03/05/13    sy     Fixing 9x25 Pack issue  
rem 02/19/13    sy     Fixing --prod bug
rem 01/28/13    sg     adding prod build for 9x25 and other features
rem 09/28/12    sy     combine alias in one compilation
rem 09/12/12    plc    Added 8x10 support
rem 08/22/12    jz     Updated to make it generic to compile any target
rem 08/08/12    sy     Enable warning=error at the top level build script 
rem 08/01/12    sy     Adding jsdcc 
rem 07/20/12    kpa    Add Nand Tools compilation.
rem 06/11/12    sy     Error out without TARGET_FAMILY 
rem 05/14/12    kpa    Added support to compile 9x25.
rem 03/30/12    sy     Fixing Build ID
rem 01/26/12    dh     Enable warning as error compiler option on 8974 build
rem 01/19/12    sy     Initial version
rem ==========================================================================

setlocal

rem GOTO USAGE

:BEGIN 
rem ========  If we have a set environment batch file, call it to set KLOCWORK and RVCT version.========
IF EXIST setenv.cmd CALL setenv.cmd

rem ===== Setup Paths=====================================================
SET BUILD_ROOT=%~dp0..\..
SET CORE_BSP_ROOT=%BUILD_ROOT%\core\bsp
SET TOOLS_SCONS_ROOT=%BUILD_ROOT%\tools\build\scons
SET TARGET_FAMILY=
SET BUILD_ID=
SET isProd=0
SET  cmd=

setlocal 

IF "%1"=="" (
  @ECHO %1 !!!!!!---- ERR: TARGET_FAMILY is missing. See build.cmd -usage for usage-----!!!!!!
  GOTO USAGE
)
IF "%1"=="-usage" GOTO USAGE

:LOOP
IF "%1"=="" GOTO CONTINUE 

IF "%1"=="TARGET_FAMILY" (
  SET TARGET_FAMILY=%2
) 
IF "%1"=="BUILD_ID" (
  SET BUILD_ID=%2
)
IF "%1"=="--prod" (
  SET isProd=1
)
SHIFT
GOTO LOOP 

:CONTINUE 
REM Create build log
rem @echo CALL %TOOLS_SCONS_ROOT%\build\rename-log.cmd  buildlog
rem CALL %TOOLS_SCONS_ROOT%\build\rename-log.cmd buildlog
rem echo Start Date=%startdate%, Time=%starttime% > buildlog-log.txt

rem Adding  warning = error flags 
SET "cmd=%* CFLAGS=--diag_error=warning"
IF %isProd%== 1 SET "cmd=%cmd:--prod =% USES_FLAGS=USES_NO_STRIP_NO_ODM,USES_NO_DEBUG"


rem set build command base on target name
SET BUILD_CMD="b%TARGET_FAMILY%.cmd"
  

IF /I "%TARGET_FAMILY%" == "8909" ( 
IF "%BUILD_ID%" == "" (
    IF %isProd% == 1 (  
      rem compiling prod build for 8909
      CALL %BUILD_CMD% boot flashtools BUILD_ID=DAATANAZ %cmd%
      rem TYPE build-log.txt > buildlog-nand.txt      
      CALL %BUILD_CMD% boot jsdcc  BUILD_ID=DAASANAZ  %cmd% 
      CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=DAADANAZ %cmd%,USES_DEVPRO_DDR 
      CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=DAADANAZ %cmd%,USES_DEVPRO_DDR_SEC  
      CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=DAADANAZ %cmd%,USES_DEVPRO_LITE 
      CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=DAADANAZ %cmd%,USES_DEVPRO_LITE_SEC
      TYPE build-log.txt >> buildlog-emmc.txt          
      CALL %BUILD_CMD% ddr_debug BUILD_ID=DAASANAZ %cmd%
      TYPE build-log.txt >> buildlog-ddi-pm8909.txt 
      CALL %BUILD_CMD% ddr_debug BUILD_ID=DAASANAZ USES_FLAGS=USES_BOOT_DDR_DEBUG_USE_PM8916 %cmd% 
      TYPE build-log.txt >> buildlog-ddi-pm8916.txt 
    )ELSE ( rem  normal flow/default 8909 build
      CALL %BUILD_CMD% boot flashtools BUILD_ID=DAATANAA %cmd%
      rem TYPE build-log.txt > buildlog-nand.txt
      CALL %BUILD_CMD% boot jsdcc BUILD_ID=DAASANAA %cmd%
      CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=DAADANAA USES_FLAGS=USES_DEVPRO_DDR %cmd% 
      CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=DAADANAA USES_FLAGS=USES_DEVPRO_DDR_SEC %cmd% 
      CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=DAADANAA USES_FLAGS=USES_DEVPRO_LITE %cmd% 
      CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=DAADANAA USES_FLAGS=USES_DEVPRO_LITE_SEC %cmd% 
      TYPE build-log.txt >> buildlog-emmc.txt
      CALL %BUILD_CMD% ddr_debug BUILD_ID=DAASANAA %cmd%
      TYPE build-log.txt >> buildlog-ddi-pm8909.txt  
      CALL %BUILD_CMD% ddr_debug BUILD_ID=DAASANAA USES_FLAGS=USES_BOOT_DDR_DEBUG_USE_PM8916 %cmd% 
      TYPE build-log.txt >> buildlog-ddi-pm8916.txt 
    )
  )ELSE (  rem compile base on Build ID
    goto  %BUILD_ID%
:DAATANAZ
   rem compiling DAATANAZ 8909-prod nand
   CALL %BUILD_CMD% boot flashtools BUILD_ID=%BUILD_ID% %cmd%
   goto END
:DAASANAZ
   rem compiling daasanaa 8909-prod emmc
   CALL %BUILD_CMD% boot jsdcc BUILD_ID=%BUILD_ID% %cmd% 
   CALL %BUILD_CMD% ddr_debug BUILD_ID=%BUILD_ID% %cmd%
   CALL %BUILD_CMD% ddr_debug BUILD_ID=%BUILD_ID% USES_FLAGS=USES_BOOT_DDR_DEBUG_USE_PM8916 %cmd%
   goto END
:DAADANAZ
   CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=%BUILD_ID% %cmd%,USES_DEVPRO_DDR 
   CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=%BUILD_ID% %cmd%,USES_DEVPRO_DDR_SEC  
   CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=%BUILD_ID% %cmd%,USES_DEVPRO_LITE 
   CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=%BUILD_ID% %cmd%,USES_DEVPRO_LITE_SEC
   goto END
:DAATANAA
   rem compiling daatanaa nand internal
   CALL %BUILD_CMD% boot flashtools BUILD_ID=%BUILD_ID% %cmd%
   goto END
:DAASANAA
   rem compiling daasanaa emmc internal
   CALL %BUILD_CMD% boot jsdcc BUILD_ID=%BUILD_ID% %cmd% 
   CALL %BUILD_CMD% ddr_debug BUILD_ID=%BUILD_ID% %cmd%
   CALL %BUILD_CMD% ddr_debug BUILD_ID=%BUILD_ID% USES_FLAGS=USES_BOOT_DDR_DEBUG_USE_PM8916 %cmd%
   goto END
:DAADANAA
   CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=%BUILD_ID% USES_FLAGS=USES_DEVPRO_DDR %cmd% 
   CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=%BUILD_ID% USES_FLAGS=USES_DEVPRO_DDR_SEC %cmd% 
   CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=%BUILD_ID% USES_FLAGS=USES_DEVPRO_LITE %cmd% 
   CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=%BUILD_ID% USES_FLAGS=USES_DEVPRO_LITE_SEC %cmd%  
   goto END
  )  
  TYPE %BUILD_CMD%-log.txt >> buildlog-log.txt
  GOTO END
)

IF /I "%TARGET_FAMILY%" == "8909w" ( 
IF "%BUILD_ID%" == "" (
    IF %isProd% == 1 (  
      rem compiling prod build for 8909w
      CALL %BUILD_CMD% boot flashtools BUILD_ID=WAATANAZ %cmd%
      rem TYPE build-log.txt > buildlog-nand.txt      
      CALL %BUILD_CMD% boot jsdcc  BUILD_ID=WAASANAZ  %cmd% 
      CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=WAADANAZ %cmd%,USES_DEVPRO_DDR 
      CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=WAADANAZ %cmd%,USES_DEVPRO_DDR_SEC  
      CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=WAADANAZ %cmd%,USES_DEVPRO_LITE 
      CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=WAADANAZ %cmd%,USES_DEVPRO_LITE_SEC
      TYPE build-log.txt >> buildlog-emmc.txt          
      CALL %BUILD_CMD% ddr_debug BUILD_ID=WAASANAZ %cmd%
      TYPE build-log.txt >> buildlog-ddi-pm8909w.txt 
      CALL %BUILD_CMD% ddr_debug BUILD_ID=WAASANAZ USES_FLAGS=USES_BOOT_DDR_DEBUG_USE_PM8916 %cmd% 
      TYPE build-log.txt >> buildlog-ddi-pm8916.txt 
    )ELSE ( rem  normal flow/default 8909w build
      CALL %BUILD_CMD% boot flashtools BUILD_ID=WAATANAA %cmd%
      rem TYPE build-log.txt > buildlog-nand.txt
      CALL %BUILD_CMD% boot jsdcc BUILD_ID=WAASANAA %cmd%
      CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=WAADANAA USES_FLAGS=USES_DEVPRO_DDR %cmd% 
      CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=WAADANAA USES_FLAGS=USES_DEVPRO_DDR_SEC %cmd% 
      CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=WAADANAA USES_FLAGS=USES_DEVPRO_LITE %cmd% 
      CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=WAADANAA USES_FLAGS=USES_DEVPRO_LITE_SEC %cmd% 
      TYPE build-log.txt >> buildlog-emmc.txt
      CALL %BUILD_CMD% ddr_debug BUILD_ID=WAASANAA %cmd%
      TYPE build-log.txt >> buildlog-ddi-pm8909w.txt  
      CALL %BUILD_CMD% ddr_debug BUILD_ID=WAASANAA USES_FLAGS=USES_BOOT_DDR_DEBUG_USE_PM8916 %cmd% 
      TYPE build-log.txt >> buildlog-ddi-pm8916.txt 
    )
  )ELSE (  rem compile base on Build ID
    goto  %BUILD_ID%
:WAATANAZ
   rem compiling WAATANAZ 8909w-prod nand
   CALL %BUILD_CMD% boot flashtools BUILD_ID=%BUILD_ID% %cmd%
   goto END
:WAASANAZ
   rem compiling WAAsanaa 8909w-prod emmc
   CALL %BUILD_CMD% boot jsdcc BUILD_ID=%BUILD_ID% %cmd% 
   CALL %BUILD_CMD% ddr_debug BUILD_ID=%BUILD_ID% %cmd%
   CALL %BUILD_CMD% ddr_debug BUILD_ID=%BUILD_ID% USES_FLAGS=USES_BOOT_DDR_DEBUG_USE_PM8916 %cmd%
   goto END
:WAADANAZ
   CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=%BUILD_ID% %cmd%,USES_DEVPRO_DDR 
   CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=%BUILD_ID% %cmd%,USES_DEVPRO_DDR_SEC  
   CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=%BUILD_ID% %cmd%,USES_DEVPRO_LITE 
   CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=%BUILD_ID% %cmd%,USES_DEVPRO_LITE_SEC
   goto END
:WAATANAA
   rem compiling WAAtanaa nand internal
   CALL %BUILD_CMD% boot flashtools BUILD_ID=%BUILD_ID% %cmd%
   goto END
:WAASANAA
   rem compiling WAAsanaa emmc internal
   CALL %BUILD_CMD% boot jsdcc BUILD_ID=%BUILD_ID% %cmd% 
   CALL %BUILD_CMD% ddr_debug BUILD_ID=%BUILD_ID% %cmd%
   CALL %BUILD_CMD% ddr_debug BUILD_ID=%BUILD_ID% USES_FLAGS=USES_BOOT_DDR_DEBUG_USE_PM8916 %cmd%
   goto END
:WAADANAA
   CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=%BUILD_ID% USES_FLAGS=USES_DEVPRO_DDR %cmd% 
   CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=%BUILD_ID% USES_FLAGS=USES_DEVPRO_DDR_SEC %cmd% 
   CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=%BUILD_ID% USES_FLAGS=USES_DEVPRO_LITE %cmd% 
   CALL %BUILD_CMD% deviceprogrammer_ddr BUILD_ID=%BUILD_ID% USES_FLAGS=USES_DEVPRO_LITE_SEC %cmd%  
   goto END
  )  
  TYPE %BUILD_CMD%-log.txt >> buildlog-log.txt
  GOTO END
)

:USAGE   
rem =========  Usage ====================================================
@ECHO.
@ECHO =============== USAGE : build.cmd  ========================================
@ECHO # This script compiles boot binaries for ONE of the B family e.g. 8974.  
@ECHO # Target must be specific by TARGET_FAMILY
@ECHO .
@ECHO build.cmd -h                         # Showing this help menu
@ECHO build.cmd                            # Error - Missing Target family
@ECHO build.cmd  TARGET_FAMILY=9x45        # compiling 9x45
@ECHO build.cmd  -c TARGET_FAMILY=9x45     # clean 9x45 binaries
@ECHO build.cmd  TARGET_FAMILY=9x45 --prod # compiling 9x45 production build
@ECHO . 
@ECHO # To build individual module , use the target specific build comand
@ECHO .
@ECHO b9x45.cmd sbl1                       # build sbl1 for 9x45
@ECHO . 
@ECHO ===============================================================

rem === Delay 3 seconds  ==== 
rem choice /c delay /d y /t 3  >null

GOTO END

:END 
rem @echo Start Date=%startdate%, Time=%starttime% - End Date=%enddate%, Time=%endtime% >> buildlog-log.txt
endlocal






