@echo off 
rem Clean Pack
call build.cmd TARGET_FAMILY=%1 USES_FLAGS=USES_CLEAN_PACK -c --implicit-deps-unchanged
rem Remove files not required 
call build.cmd TARGET_FAMILY=%1 -c --implicit-deps-unchanged --cleanpack
rem First build to ensure no error
call build.cmd TARGET_FAMILY=%1 USES_FLAGS=NO_DEBUG
rem IF  %ERRORLEVEL% GTR 0 GOTO ERR

goto END
rem Copy the build

:ERR
echo ---- ERR: Error in Pack build. Compilation terminated --- 

:END
echo ---- Complete Clean Pack  ---
