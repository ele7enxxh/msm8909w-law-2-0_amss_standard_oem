@echo off
REM ======================================================
rem Sets path for ARM Compiler Toolchain 5.01 Build 43, GNU and PERL


rem
rem Generic definitions to make maintenance easier
rem
set ROOT_DRIVE=C:
set APPS_DIR=Apps
set APPS_PATH=%ROOT_DRIVE%\%APPS_DIR%
set UTILROOT=%ROOT_DRIVE%\utils

rem
rem GNU, Perl and Python paths
rem
set GNUPATH=%UTILROOT%\cygwin\bin
set CRMPERL=C:\CRMApps\apps\Perl\bin
set PERLPATH=C:\perl\bin
set PYTHON_PATH=C:\CRMApps\apps\Python262

rem
rem ARM General Definitions used by Qualcomm build goo
rem
set ARMTOOLS=ARMCT5.01
set ARMHOME=%ARMROOT%

rem
rem ARM Include, Lib and Bin Variables (used by compiler toolchain)
rem
set ARMROOT=%APPS_PATH%\ARMCT5.01\43
set ARMBIN=%ARMROOT%\bin
set ARMINC=%ARMROOT%\include
set ARMLIB=%ARMROOT%\lib

rem set ARMCC5BIN=%ARMROOT%\bin
rem set ARMCC5INC=%ARMROOT%\include
rem set ARMCC5LIB=%ARMROOT%\lib


rem
rem Path for ARM, GNU, Python, and Perl tools
rem
set path=%PERLPATH%;%CRMPERL%;%PYTHON_PATH%;%GNUPATH%;%ARMCC5BIN%;%UTILROOT%;%path%
REM ======================================================
