@ECHO OFF
REM Name
REM 	t32start.cmd
REM
REM Description
REM 	Startup batch file that initializes the T32 session
REM
REM Copyright 2012 - 2014 Qualcomm Inc
REM 05/29/2013  AJCheriyan		Change for command line launch
REM 04/03/2013	AJCheriyan		Added support for newer T32start location
REM 02/06/2013	AJCheriyan		Fixed issue with multi-chipset config
REM 01/30/2012	AJCheriyan		Added support for multi-chipset configs 
REM 10/01/2012	AJCheriyan		Fixed issue with command line support
REM 07/28/2012	AJCheriyan		Added support to be called from Command line
REM 07/19/2012	AJCheriyan		First version for MSM8974



REM The only arguments it will accept would be
REM ARG0 - Chipset name
REM ARG0 - PARSE or NOPARSE - Whether to parse the meta build xml to generate the configuration
REM ARG1 - STARTCORE - Whether any cores have to be started or not. Default is to start none.
REM ARGn - n > 1 - Used only if ARG1 is STARTCORE. Indicates which cores to start

REM Set the defaults first

SET CHIPSET=%1
SET PARSEOPT=%2
SET COREOPT=%3

REM Find the chipset specific file. 
IF /i "%CHIPSET%"=="" (
	FOR /F "tokens=1 delims=" %%A IN ('dir /s /b /a-d %~dp0*.ts2') DO (
		SET CHIPSET=%%A
	)
)
IF /i "%CHIPSET%"=="" GOTO NOT32CONFIG
IF /i "%PARSEOPT%"=="" SET PARSEOPT=PARSE
IF /i "%COREOPT%"=="" SET COREOPT=NONE

ECHO "Parse option: %PARSEOPT% , Core option: %COREOPT%"

REM Set other variables
SET SCRIPTSDIR=%~dp0..\..\tools\cmm
SET CMMFILENAME=gen_buildinfo.cmm

REM Generate the build information
REM Delete remnants of the last run first
IF EXIST %TEMP%\%CMMFILENAME% DEL /Q %TEMP%\%CMMFILENAME%

REM If the option is NOPARSE, we prompt for the cmm file that contains the information we need
IF /i %PARSEOPT% == NOPARSE (
SET /P META_CMM=Enter the meta build configuration file press Enter:
REM Then copy that over to the temp directory
ECHO %META_CMM% %TEMP%\%CMMFILENAME%
COPY /Y %META_CMM% %TEMP%\%CMMFILENAME%
)

REM If the option is parse, we parse the meta build xml
IF /i "%PARSEOPT%" == "PARSE" (
REM Check if python is installed
FOR /F "tokens=1 delims= " %%A IN ('python --version') DO IF ERRORLEVEL 1 GOTO NOPYTHON
REM Now call the parser script
call python %~dp0..\..\tools\meta\gen_cmm_data3.py ..\..\.. "%TEMP%\%CMMFILENAME%" -paths_only
IF NOT EXIST %TEMP%\%CMMFILENAME% GOTO NOCMMGEN
)

REM Find the T32 installation now. Exit if not found
SET T32SYS=C:\T32
IF NOT EXIST %T32SYS% (
ECHO T32 installation not found.
SET /P T32SYS="Enter location of the T32 installation:"
)

REM We have the CMM generated now. Check if any core has to be opened
IF /i %COREOPT% == STARTCORE (
:START_T32_1
IF EXIST %T32SYS%\t32start.exe (
start %T32SYS%\bin\windows64\t32start.exe -runcfgfile %CHIPSET% -runitem "//Root/DAP"
GOTO EXIT
)
:START_T32_2
IF EXIST %T32SYS%\bin\windows\t32start.exe (
start %T32SYS%\bin\windows64\t32start.exe -runcfgfile %CHIPSET% -runitem "//Root/DAP"
GOTO EXIT
)
)

REM We don't have any particular core to start. Just open the main user interface
IF /i "%COREOPT%"=="NONE" (
:START_T32_3
IF EXIST %T32SYS%\t32start.exe (
start %T32SYS%\bin\windows64\t32start.exe -runcfgfile %CHIPSET%
GOTO EXIT
)
:START_T32_4
IF EXIST %T32SYS%\bin\windows\t32start.exe (
start %T32SYS%\bin\windows64\t32start.exe -runcfgfile %CHIPSET%
GOTO EXIT
)
)

:DONE
GOTO EXIT

REM Error Conditions to check for 
:NOPYTHON
COLOR 0C
ECHO Python Installation missing. Install python 2.6.2 or greater.
GOTO EXIT

:NOCMMGEN
COLOR 0C
ECHO Build Information not generated. Cannot start.
GOTO EXIT

:NOT32CONFIG
COLOR 0C
ECHO T32 Config file could not be found.

:FATALEXIT
SLEEP 50

:EXIT



