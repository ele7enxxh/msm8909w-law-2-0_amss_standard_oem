@echo off
rem ==========================================================================
rem
rem  CBSP Buils system
rem
rem  General Description
rem     build batch file.
rem
rem Copyright (c) 2012 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem
rem --------------------------------------------------------------------------
rem
rem $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/ms/clean_all.cmd#1 $
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

IF "%1"=="CHIPSET" (
  IF "%2" == "msm8916" (
    goto CLEAN_8916_tz
  )
  IF "%2" == "msm8936" (
    goto CLEAN_8936_tz
  )
  IF "%2" == "msm8909" (
    goto CLEAN_8909_tz
  )
   IF "%2" == "mdm9x09" (
    goto CLEAN_9x09_tz
  )
  echo Unknown chipset...
  EXIT /B
)
IF "%1" == "" (
  goto CLEAN_all
)

echo Please specify a valid chipset e.g. CHIPSET=msm8916
EXIT /B

:CLEAN_8916_tz
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8916 tz hyp sampleapp securitytest assurancetest tzbsp_no_xpu playready widevine keymaster tzbsp_with_test tztestexec dummy_venus tzbsptest dxhdcp2 dxhdcp2dbg commonlib -c
GOTO CLEAN_8916_SUCCESS

:CLEAN_8916_SUCCESS
echo ==============================================================================
echo CLEAN_ALL.cmd summary for 8916
echo ==============================================================================
echo CLEAN SUCCESS - The following images were removed:
echo MAUAANAA\tz hyp sampleapp securitytest assurancetest tzbsp_no_xpu playready widevine keymaster tzbsp_with_test tztestexec dummy_venus tzbsptest dxhdcp2 dxhdcp2dbg cmnlib
echo. 2>CleanAllSuccess_8916.txt
GOTO DONE

:CLEAN_8936_tz
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8936 tz hyp sampleapp securitytest assurancetest tzbsp_no_xpu playready widevine keymaster tzbsp_with_test tztestexec dummy_venus tzbsptest dxhdcp2 dxhdcp2dbg commonlib -c
GOTO CLEAN_8936_SUCCESS

:CLEAN_8936_SUCCESS
echo ==============================================================================
echo CLEAN_ALL.cmd summary for 8936
echo ==============================================================================
echo CLEAN SUCCESS - The following images were removed:
echo MAWAANAA\tz hyp sampleapp securitytest assurancetest tzbsp_no_xpu playready widevine keymaster tzbsp_with_test tztestexec dummy_venus tzbsptest dxhdcp2 dxhdcp2dbg cmnlib
echo. 2>CleanAllSuccess_8936.txt
GOTO DONE

:CLEAN_8909_tz
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909 tz hyp sampleapp securitytest assurancetest tzbsp_no_xpu playready widevine keymaster tzbsp_with_test tztestexec dummy_venus tzbsptest dxhdcp2 dxhdcp2dbg commonlib -c
GOTO CLEAN_8909_SUCCESS

:CLEAN_8909_SUCCESS
echo ==============================================================================
echo CLEAN_ALL.cmd summary for 8909
echo ==============================================================================
echo CLEAN SUCCESS - The following images were removed:
echo MAZAANAA\tz hyp sampleapp securitytest assurancetest tzbsp_no_xpu playready widevine keymaster tzbsp_with_test tztestexec dummy_venus tzbsptest dxhdcp2 dxhdcp2dbg commonlib
echo. 2>CleanAllSuccess_8909.txt
GOTO DONE


:CLEAN_9x09_tz
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=mdm9x09 tz hyp  tzbsp_no_xpu  tzbsp_with_test tztestexec dummy_venus tzbsptest -c
GOTO CLEAN_9x09_SUCCESS

:CLEAN_9x09_SUCCESS
echo ==============================================================================
echo CLEAN_ALL.cmd summary for 9x09
echo ==============================================================================
echo CLEAN SUCCESS - The following images were removed:
echo NAYAANAA\tz hyp tzbsp_no_xpu  tzbsp_with_test tztestexec dummy_venus tzbsptest 
echo. 2>CleanAllSuccess_9x09.txt
GOTO DONE

:CLEAN_all
CALL %BUILD_ROOT%\build\ms\clean_all.cmd CHIPSET=msm8916
CALL %BUILD_ROOT%\build\ms\clean_all.cmd CHIPSET=msm8936
CALL %BUILD_ROOT%\build\ms\clean_all.cmd CHIPSET=msm8909
CALL %BUILD_ROOT%\build\ms\clean_all.cmd CHIPSET=msm9x09
GOTO DONE_all

:DONE
echo ***CLEANING COMPLETE***
endlocal
EXIT /B

:DONE_all
echo ***CLEANING COMPLETE FOR ALL TARGETS***
echo. 2>CleanAllTargetsSuccess.txt
endlocal
EXIT /B
