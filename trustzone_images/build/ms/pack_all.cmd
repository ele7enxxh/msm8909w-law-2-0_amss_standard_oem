@echo off
rem ==========================================================================
rem
rem  CBSP Buils system
rem
rem  General Description
rem     build batch file.
rem
rem Copyright (c) 2009-2009 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem
rem --------------------------------------------------------------------------
rem
rem $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/ms/pack_all.cmd#1 $
rem $DateTime: 2016/06/17 14:31:11 $
rem $Author: pwbldsvc $
rem $Change: 10727476 $
rem                      EDIT HISTORY FOR FILE
rem
rem  This section contains comments describing changes made to the module.
rem  Notice that changes are listed in reverse chronological order.
rem
rem when       who     what, where, why
rem --------   ---     ---------------------------------------------------------
rem 10/17/13   ib      Add support for securemm for apq8084
rem ==========================================================================

setlocal

rem ===== Setup Paths=====================================================
SET BUILD_ROOT=%~dp0..\..
SET PACK_ROOT=%~dp0..\..\..

IF "%1"=="CHIPSET" (
  IF "%2" == "msm8916" (
    del %PACK_ROOT%\HY11_8916
    goto PACK_8916_build
  )
  IF "%2" == "msm8936" (
    del %PACK_ROOT%\HY11_8936
    goto PACK_8936_build
  )
  echo Unknown chipset...
  EXIT /B
)

IF "%1" == "" (
  del %PACK_ROOT%\HY11_8916
  del %PACK_ROOT%\HY11_8936
  goto PACK_all
)

echo Please specify a valid chipset e.g. CHIPSET=msm8916
EXIT /B

:PACK_8916_build
CD %BUILD_ROOT%\build\ms
CALL build.cmd CHIPSET=msm8916 tz sampleapp commonlib
IF EXIST %BUILD_ROOT%\build\ms\bin\MABAANAA\cmnlib.mbn GOTO PACK_8916_deepclean
echo **Build errors...
echo One or more images failed to build for msm8916. Please refer to build log for compilation errors.
EXIT /B

:PACK_8916_deepclean
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8916 tz sampleapp commonlib USES_FLAGS=USES_CLEAN_PACK -c --implicit-deps-unchanged
GOTO PACK_8916_copy
echo **Build errors...
echo Deepclean failed for msm8916. Please refer to build log for errors.
EXIT /B

:PACK_8916_copy
CALL xcopy %PACK_ROOT%\trustzone_images %PACK_ROOT%\HY11_8916\trustzone_images /s/e/i
IF EXIST %PACK_ROOT%\HY11_8916 GOTO PACK_8916_cleanpack
echo **Build errors...
echo HY11_8916 creation failed for msm8916. Please refer to build log for errors.
EXIT /B

:PACK_8916_cleanpack
CD %PACK_ROOT%\HY11_8916\trustzone_images\build\ms
CALL build.cmd CHIPSET=msm8916 tz sampleapp commonlib -c --implicit-deps-unchanged --cleanpack
GOTO PACK_8916_HY11CompileTest
echo **Build errors...
echo Clean pack failed for msm8916. Please refer to build log for errors.
EXIT /B

:PACK_8916_HY11CompileTest
CALL %PACK_ROOT%\HY11_8916\trustzone_images\build\ms\build.cmd CHIPSET=msm8916 tz sampleapp commonlib
IF EXIST %PACK_ROOT%\HY11_8916\trustzone_images\build\ms\bin\MABAANAA\cmnlib.mbn GOTO PACK_8916_success
echo **Build errors...
echo One or more images failed to build for msm8916. Please refer to build log for compilation errors.
EXIT /B

:PACK_8916_SUCCESS
echo ==============================================================================
echo PACK_ALL.cmd build summary for 8916
echo ==============================================================================
echo. 2>PackSuccess_8916.txt
GOTO DONE

:PACK_8936_build
CD %BUILD_ROOT%\build\ms
CALL build.cmd CHIPSET=msm8936 tz sampleapp commonlib
IF EXIST %BUILD_ROOT%\build\ms\bin\MAWAANAA\cmnlib.mbn GOTO PACK_8936_deepclean
echo **Build errors...
echo One or more images failed to build for msm8936. Please refer to build log for compilation errors.
EXIT /B

:PACK_8936_deepclean
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8936 tz sampleapp commonlib USES_FLAGS=USES_CLEAN_PACK -c --implicit-deps-unchanged
GOTO PACK_8936_copy
echo **Build errors...
echo Deepclean failed for msm8936. Please refer to build log for errors.
EXIT /B

:PACK_8936_copy
CALL xcopy %PACK_ROOT%\trustzone_images %PACK_ROOT%\HY11_8936\trustzone_images /s/e/i
IF EXIST %PACK_ROOT%\HY11_8936 GOTO PACK_8936_cleanpack
echo **Build errors...
echo HY11_8936 creation failed for msm8936. Please refer to build log for errors.
EXIT /B

:PACK_8936_cleanpack
CD %PACK_ROOT%\HY11_8936\trustzone_images\build\ms
CALL build.cmd CHIPSET=msm8936 tz sampleapp commonlib -c --implicit-deps-unchanged --cleanpack
GOTO PACK_8936_HY11CompileTest
echo **Build errors...
echo Clean pack failed for msm8936. Please refer to build log for errors.
EXIT /B

:PACK_8936_HY11CompileTest
CALL %PACK_ROOT%\HY11_8936\trustzone_images\build\ms\build.cmd CHIPSET=msm8936 tz sampleapp commonlib
IF EXIST %PACK_ROOT%\HY11_8936\trustzone_images\build\ms\bin\MAWAANAA\cmnlib.mbn GOTO PACK_8936_success
echo **Build errors...
echo One or more images failed to build for msm8936. Please refer to build log for compilation errors.
EXIT /B

:PACK_8936_SUCCESS
echo ==============================================================================
echo PACK_ALL.cmd build summary for 8936
echo ==============================================================================
echo. 2>PackSuccess_8936.txt
GOTO DONE

:PACK_all
CALL %BUILD_ROOT%\build\ms\pack_all.cmd CHIPSET=msm8916
IF NOT EXIST %PACK_ROOT%\HY11_8916\trustzone_images\build\ms\bin\MABAANBA\cmnlib.mbn GOTO PACK_all_error
CALL %BUILD_ROOT%\build\ms\pack_all.cmd CHIPSET=msm8936
IF NOT EXIST %PACK_ROOT%\HY11_8936\trustzone_images\build\ms\bin\MAWAANBA\cmnlib.mbn GOTO PACK_all_error
GOTO DONE_all

:DONE
echo ***PACKING COMPLETE***
endlocal
EXIT /B

:DONE_all
echo ***PACKING COMPLETE FOR ALL TARGETS***
echo. 2>PackAllTargetsSuccess.txt
endlocal
EXIT /B

:BUILD_all_error
echo **Build errors...
echo Please refer to build log for build errors and check the image bin folder(s).
endlocal
EXIT /B
