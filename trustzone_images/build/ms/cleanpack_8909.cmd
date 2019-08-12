@echo off
rem First build to ensure no error
call build.cmd CHIPSET=msm8909 tz tzbsp_no_xpu sampleapp playready widevine keymaster commonlib USES_FLAGS=USES_PLAYREADY_PACK
rem IF  %ERRORLEVEL% GTR 0 GOTO ERR
rem Copy tzbsp source code to HD11 for RIM prior to cleanpack
call xcopy ..\..\..\trustzone_images\core\securemsm\trustzone\monitor ..\..\..\temp\trustzone_images\core\securemsm\trustzone\monitor /s/e/i
call xcopy ..\..\..\trustzone_images\core\securemsm\trustzone\qsee ..\..\..\temp\trustzone_images\core\securemsm\trustzone\qsee /s/e/i
rem Clean build except binary libraries
call build.cmd CHIPSET=msm8909 tz tzbsp_no_xpu sampleapp playready widevine keymaster commonlib USES_FLAGS=USES_CLEAN_PACK -c --implicit-deps-unchanged
rem Remove the files
call build.cmd CHIPSET=msm8909 tz tzbsp_no_xpu sampleapp playready widevine keymaster commonlib -c --implicit-deps-unchanged --cleanpack
rem Recompile
call build.cmd CHIPSET=msm8909 tz tzbsp_no_xpu sampleapp playready widevine keymaster commonlib
goto END
rem Copy the build

:ERR
echo ---- ERR: Error in Pack build. Compilation terminated --

:END
echo ---- Complete Local Clean pack. Moving to HY11 ---
