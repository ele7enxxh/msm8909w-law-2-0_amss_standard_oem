@echo off
rem ==========================================================================
rem
rem  CBSP Builds system
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
rem $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/ms/build_all.cmd#1 $
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
rem 10/17/13   ib      Added securemm for apq8084
rem ==========================================================================

setlocal

rem ===== Setup Paths=====================================================
SET BUILD_ROOT=%~dp0..\..

IF "%1"=="CHIPSET" (
  IF "%2" == "msm8916" (
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\tz.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\hyp.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\sampleapp.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\securitytest.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\assurancetest.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\tzbsp_no_xpu.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\playready.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\widevine.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\keymaster.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\tzbsp_with_test.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\tztestexec.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\dummy_venus.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\tzbsptest.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\dxhdcp2.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\dxhdcp2dbg.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\cmnlib.mbn
    goto BUILD_8916_tz
  )
  IF "%2" == "msm8936" (
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\tz.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\hyp.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\sampleapp.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\securitytest.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\assurancetest.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\tzbsp_no_xpu.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\playready.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\widevine.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\keymaster.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\tzbsp_with_test.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\tztestexec.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\dummy_venus.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\tzbsptest.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\dxhdcp2.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\dxhdcp2dbg.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\cmnlib.mbn
    goto BUILD_8936_tz
  )
  IF "%2" == "msm8909" (
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\tz.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\sampleapp.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\assurancetest.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\tzbsp_no_xpu.mbn
    rem del %BUILD_ROOT%\build\ms\bin\MAZAANAA\playready.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\widevine.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\keymaster.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\tzbsp_with_test.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\tztestexec.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\dummy_venus.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\tzbsptest.mbn
    rem del %BUILD_ROOT%\build\ms\bin\MAZAANAA\dxhdcp2.mbn
    rem del %BUILD_ROOT%\build\ms\bin\MAZAANAA\dxhdcp2dbg.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\cmnlib.mbn
    goto BUILD_8909_tz
  )
  IF "%2" == "msm8909w" (
   "%2" == "msm8909w" (
    del %BUILD_ROOT%\build\ms\bin\MATAANAA\tz.mbn
    del %BUILD_ROOT%\build\ms\bin\MATAANAA\sampleapp.mbn
    del %BUILD_ROOT%\build\ms\bin\MATAANAA\assurancetest.mbn
    del %BUILD_ROOT%\build\ms\bin\MATAANAA\tzbsp_no_xpu.mbn
    del %BUILD_ROOT%\build\ms\bin\MATAANAA\playready.mbn
    del %BUILD_ROOT%\build\ms\bin\MATAANAA\widevine.mbn
    del %BUILD_ROOT%\build\ms\bin\MATAANAA\keymaster.mbn
    del %BUILD_ROOT%\build\ms\bin\MATAANAA\tzbsp_with_test.mbn
    del %BUILD_ROOT%\build\ms\bin\MATAANAA\tztestexec.mbn
    del %BUILD_ROOT%\build\ms\bin\MATAANAA\dummy_venus.mbn
    del %BUILD_ROOT%\build\ms\bin\MATAANAA\tzbsptest.mbn
    del %BUILD_ROOT%\build\ms\bin\MATAANAA\dxhdcp2.mbn
    del %BUILD_ROOT%\build\ms\bin\MATAANAA\dxhdcp2dbg.mbn
    del %BUILD_ROOT%\build\ms\bin\MATAANAA\cmnlib.mbn
    goto BUILD_8909w_tz
	
  )
  IF "%2" == "mdm9x07" (
    del %BUILD_ROOT%\build\ms\bin\MARAANAA\tz.mbn
    del %BUILD_ROOT%\build\ms\bin\MARAANAA\sampleapp.mbn
    del %BUILD_ROOT%\build\ms\bin\MARAANAA\tzbsp_no_xpu.mbn
    del %BUILD_ROOT%\build\ms\bin\MARAANAA\keymaster.mbn
    del %BUILD_ROOT%\build\ms\bin\MARAANAA\tzbsp_with_test.mbn
    del %BUILD_ROOT%\build\ms\bin\MARAANAA\tztestexec.mbn
    del %BUILD_ROOT%\build\ms\bin\MARAANAA\tzbsptest.mbn
    del %BUILD_ROOT%\build\ms\bin\MARAANAA\cmnlib.mbn
    goto BUILD_9x07_tz
  )
  IF "%2" == "mdm9x09" (
    del %BUILD_ROOT%\build\ms\bin\NAYAANAA\tz.mbn
    del %BUILD_ROOT%\build\ms\bin\NAYAANAA\hyp.mbn
    del %BUILD_ROOT%\build\ms\bin\NAYAANAA\tzbsp_no_xpu.mbn
    del %BUILD_ROOT%\build\ms\bin\NAYAANAA\tzbsp_with_test.mbn
    del %BUILD_ROOT%\build\ms\bin\NAYAANAA\tztestexec.mbn
    del %BUILD_ROOT%\build\ms\bin\NAYAANAA\dummy_venus.mbn
    del %BUILD_ROOT%\build\ms\bin\NAYAANAA\tzbsptest.mbn
    goto BUILD_9x09_tz
  )
  echo Unknown chipset...
  EXIT /B
)
IF "%1" == "" (
rem ===== 8916 Clean =====================================================
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\tz.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\hyp.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\sampleapp.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\securitytest.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\assurancetest.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\tzbsp_no_xpu.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\playready.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\widevine.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\keymaster.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\tzbsp_with_test.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\tztestexec.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\dummy_venus.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\tzbsptest.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\dxhdcp2.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\dxhdcp2dbg.mbn
    del %BUILD_ROOT%\build\ms\bin\MAUAANAA\cmnlib.mbn
rem ===== 8936 Clean =====================================================
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\tz.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\hyp.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\sampleapp.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\securitytest.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\assurancetest.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\tzbsp_no_xpu.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\playready.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\widevine.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\keymaster.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\tzbsp_with_test.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\tztestexec.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\dummy_venus.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\tzbsptest.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\dxhdcp2.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\dxhdcp2dbg.mbn
    del %BUILD_ROOT%\build\ms\bin\MAWAANAA\cmnlib.mbn
rem ===== 8909 Clean =====================================================
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\tz.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\sampleapp.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\securitytest.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\assurancetest.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\tzbsp_no_xpu.mbn
    rem del %BUILD_ROOT%\build\ms\bin\MAZAANAA\playready.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\widevine.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\keymaster.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\tzbsp_with_test.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\tztestexec.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\dummy_venus.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\tzbsptest.mbn
    rem del %BUILD_ROOT%\build\ms\bin\MAZAANAA\dxhdcp2.mbn
    rem del %BUILD_ROOT%\build\ms\bin\MAZAANAA\dxhdcp2dbg.mbn
    del %BUILD_ROOT%\build\ms\bin\MAZAANAA\cmnlib.mbn
rem ===== 9x09 Clean =====================================================
	del %BUILD_ROOT%\build\ms\bin\NAYAANAA\tz.mbn
    del %BUILD_ROOT%\build\ms\bin\NAYAANAA\hyp.mbn
    del %BUILD_ROOT%\build\ms\bin\NAYAANAA\tzbsp_no_xpu.mbn
    del %BUILD_ROOT%\build\ms\bin\NAYAANAA\tzbsp_with_test.mbn
    del %BUILD_ROOT%\build\ms\bin\NAYAANAA\tztestexec.mbn
    del %BUILD_ROOT%\build\ms\bin\NAYAANAA\dummy_venus.mbn
    del %BUILD_ROOT%\build\ms\bin\NAYAANAA\tzbsptest.mbn
  goto BUILD_all
)

IF "%1" == "-c" (
  CALL %BUILD_ROOT%\build\ms\clean_all.cmd
  endlocal
  EXIT /B
)

echo Please specify a valid chipset e.g. CHIPSET=msm8916
EXIT /B

:BUILD_8916_tz
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8916 tz
IF EXIST %BUILD_ROOT%\build\ms\bin\MAUAANAA\tz.mbn GOTO BUILD_8916_hyp
echo **Build errors...
echo tz.mbn file not found for msm8916. Please refer to build log for build errors.
EXIT /B

:BUILD_8916_hyp
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8916 hyp
IF EXIST %BUILD_ROOT%\build\ms\bin\MAUAANAA\tz.mbn GOTO BUILD_8916_sampleapp
echo **Build errors...
echo hyp.mbn file not found for msm8916. Please refer to build log for build errors.
EXIT /B

:BUILD_8916_sampleapp
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8916 sampleapp
IF EXIST %BUILD_ROOT%\build\ms\bin\MAUAANAA\sampleapp.mbn GOTO BUILD_8916_securitytest
echo **Build errors...
echo sampleapp.mbn file not found for msm8916. Please refer to build log for build errors.
EXIT /B

:BUILD_8916_securitytest
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8916 securitytest
IF EXIST %BUILD_ROOT%\build\ms\bin\MAUAANAA\securitytest.mbn GOTO BUILD_8916_tzbspnoxpu
echo **Build errors...
echo securitytest.mbn file not found for msm8916. Please refer to build log for build errors.
EXIT /B

:BUILD_8916_tzbspnoxpu
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8916 tzbsp_no_xpu
IF EXIST %BUILD_ROOT%\build\ms\bin\MAUAANAA\tzbsp_no_xpu.mbn GOTO BUILD_8916_playready
echo **Build errors...
echo tzbsp_no_xpu.mbn file not found for msm8916. Please refer to build log for build errors.
EXIT /B

:BUILD_8916_playready
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8916 playready
IF EXIST %BUILD_ROOT%\build\ms\bin\MAUAANAA\playready.mbn GOTO BUILD_8916_widevine
echo **Build errors...
echo playready.mbn file not found for msm8916. Please refer to build log for build errors.
EXIT /B

:BUILD_8916_widevine
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8916 widevine
IF EXIST %BUILD_ROOT%\build\ms\bin\MAUAANAA\widevine.mbn GOTO BUILD_8916_keymaster
echo **Build errors...
echo widevine.mbn file not found for msm8916. Please refer to build log for build errors.
EXIT /B

:BUILD_8916_keymaster
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8916 keymaster
IF EXIST %BUILD_ROOT%\build\ms\bin\MAUAANAA\keymaster.mbn GOTO BUILD_8916_tzbspwithtest
echo **Build errors...
echo keymaster.mbn file not found for msm8916. Please refer to build log for build errors.
EXIT /B

:BUILD_8916_tzbspwithtest
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8916 tzbsp_with_test
IF EXIST %BUILD_ROOT%\build\ms\bin\MAUAANAA\tzbsp_with_test.mbn GOTO BUILD_8916_tztestexec
echo **Build errors...
echo tzbsp_with_test.mbn file not found for msm8916. Please refer to build log for build errors.
EXIT /B

:BUILD_8916_tztestexec
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8916 tztestexec
IF EXIST %BUILD_ROOT%\build\ms\bin\MAUAANAA\tztestexec.mbn GOTO BUILD_8916_dummy_venus
echo **Build errors...
echo tzbsp_with_test.mbn file not found for msm8916. Please refer to build log for build errors.
EXIT /B

:BUILD_8916_dummy_venus
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8916 dummy_venus
IF EXIST %BUILD_ROOT%\build\ms\bin\MAUAANAA\dummy_venus.mbn GOTO BUILD_8916_tzbsptest
echo **Build errors...
echo dummy_venus.mbn file not found for msm8916. Please refer to build log for build errors.
EXIT /B

:BUILD_8916_tzbsptest
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8916 tzbsptest
IF EXIST %BUILD_ROOT%\build\ms\bin\MAUAANAA\tzbsptest.mbn GOTO BUILD_8916_dxhdcp2
echo **Build errors...
echo tzbsptest.mbn file not found for msm8916. Please refer to build log for build errors.
EXIT /B

:BUILD_8916_dxhdcp2
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8916 dxhdcp2
IF EXIST %BUILD_ROOT%\build\ms\bin\MAUAANAA\dxhdcp2.mbn GOTO BUILD_8916_dxhdcp2dbg
echo **Build errors...
echo dxhdcp2.mbn file not found for msm8916. Please refer to build log for build errors.
EXIT /B

:BUILD_8916_dxhdcp2dbg
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8916 dxhdcp2dbg
IF EXIST %BUILD_ROOT%\build\ms\bin\MAUAANAA\dxhdcp2dbg.mbn GOTO BUILD_8916_commonlib
echo **Build errors...
echo dxhdcp2dbg.mbn file not found for msm8916. Please refer to build log for build errors.
EXIT /B

:BUILD_8916_commonlib
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8916 commonlib
IF EXIST %BUILD_ROOT%\build\ms\bin\MAUAANAA\cmnlib.mbn GOTO BUILD_8916_SUCCESS
echo **Build errors...
echo cmnlib.mbn file not found for msm8916. Please refer to build log for build errors.
EXIT /B

:BUILD_8916_SUCCESS
echo ==============================================================================
echo BUILD_ALL.cmd build summary for 8916
echo ==============================================================================
echo BUILD SUCCESS - The following images were generated:
echo MAUAANAA\tz.mbn
echo MAUAANAA\sampleapp.mbn
echo MAUAANAA\securitytest.mbn
echo MAUAANAA\tzbsp_no_xpu.mbn
echo MAUAANAA\playready.mbn
echo MAUAANAA\widevine.mbn
echo MAUAANAA\keymaster.mbn
echo MAUAANAA\tzbsp_with_test.mbn
echo PIL_IMAGES\SPLITBINS_MAUAANAA\tztestexec.mdt
echo MAUAANAA\dummy_venus.mbn
echo MAUAANAA\tzbsptest.mbn
echo MAUAANAA\dxhdcp2.mbn
echo MAUAANAA\dxhdcp2dbg.mbn
echo MAUAANAA\cmnlib.mbn
echo. 2>BuildAllSuccess_8916.txt
GOTO DONE

:BUILD_8936_tz
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8936 tz
IF EXIST %BUILD_ROOT%\build\ms\bin\MAWAANAA\tz.mbn GOTO BUILD_8936_hyp
echo **Build errors...
echo tz.mbn file not found for msm8936. Please refer to build log for build errors.
EXIT /B

:BUILD_8936_hyp
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8936 hyp
IF EXIST %BUILD_ROOT%\build\ms\bin\MAWAANAA\tz.mbn GOTO BUILD_8936_sampleapp
echo **Build errors...
echo hyp.mbn file not found for msm8936. Please refer to build log for build errors.
EXIT /B

:BUILD_8936_sampleapp
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8936 sampleapp
IF EXIST %BUILD_ROOT%\build\ms\bin\MAWAANAA\sampleapp.mbn GOTO BUILD_8936_securitytest
echo **Build errors...
echo sampleapp.mbn file not found for msm8936. Please refer to build log for build errors.
EXIT /B

:BUILD_8936_securitytest
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8936 securitytest
IF EXIST %BUILD_ROOT%\build\ms\bin\MAWAANAA\securitytest.mbn GOTO BUILD_8936_tzbspnoxpu
echo **Build errors...
echo securitytest.mbn file not found for msm8936. Please refer to build log for build errors.
EXIT /B

:BUILD_8936_tzbspnoxpu
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8936 tzbsp_no_xpu
IF EXIST %BUILD_ROOT%\build\ms\bin\MAWAANAA\tzbsp_no_xpu.mbn GOTO BUILD_8936_playready
echo **Build errors...
echo tzbsp_no_xpu.mbn file not found for msm8936. Please refer to build log for build errors.
EXIT /B

:BUILD_8936_playready
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8936 playready
IF EXIST %BUILD_ROOT%\build\ms\bin\MAWAANAA\playready.mbn GOTO BUILD_8936_widevine
echo **Build errors...
echo playready.mbn file not found for msm8936. Please refer to build log for build errors.
EXIT /B

:BUILD_8936_widevine
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8936 widevine
IF EXIST %BUILD_ROOT%\build\ms\bin\MAWAANAA\widevine.mbn GOTO BUILD_8936_keymaster
echo **Build errors...
echo widevine.mbn file not found for msm8936. Please refer to build log for build errors.
EXIT /B

:BUILD_8936_keymaster
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8936 keymaster
IF EXIST %BUILD_ROOT%\build\ms\bin\MAWAANAA\keymaster.mbn GOTO BUILD_8936_tzbspwithtest
echo **Build errors...
echo keymaster.mbn file not found for msm8936. Please refer to build log for build errors.
EXIT /B

:BUILD_8936_tzbspwithtest
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8936 tzbsp_with_test
IF EXIST %BUILD_ROOT%\build\ms\bin\MAWAANAA\tzbsp_with_test.mbn GOTO BUILD_8936_tztestexec
echo **Build errors...
echo tzbsp_with_test.mbn file not found for msm8936. Please refer to build log for build errors.
EXIT /B

:BUILD_8936_tztestexec
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8936 tztestexec
IF EXIST %BUILD_ROOT%\build\ms\bin\MAWAANAA\tztestexec.mbn GOTO BUILD_8936_dummy_venus
echo **Build errors...
echo tzbsp_with_test.mbn file not found for msm8936. Please refer to build log for build errors.
EXIT /B

:BUILD_8936_dummy_venus
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8936 dummy_venus
IF EXIST %BUILD_ROOT%\build\ms\bin\MAWAANAA\dummy_venus.mbn GOTO BUILD_8936_tzbsptest
echo **Build errors...
echo dummy_venus.mbn file not found for msm8936. Please refer to build log for build errors.
EXIT /B

:BUILD_8936_tzbsptest
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8936 tzbsptest
IF EXIST %BUILD_ROOT%\build\ms\bin\MAWAANAA\tzbsptest.mbn GOTO BUILD_8936_dxhdcp2
echo **Build errors...
echo tzbsptest.mbn file not found for msm8936. Please refer to build log for build errors.
EXIT /B

:BUILD_8936_dxhdcp2
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8936 dxhdcp2
IF EXIST %BUILD_ROOT%\build\ms\bin\MAWAANAA\dxhdcp2.mbn GOTO BUILD_8936_dxhdcp2dbg
echo **Build errors...
echo dxhdcp2.mbn file not found for msm8936. Please refer to build log for build errors.
EXIT /B

:BUILD_8936_dxhdcp2dbg
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8936 dxhdcp2dbg
IF EXIST %BUILD_ROOT%\build\ms\bin\MAWAANAA\dxhdcp2dbg.mbn GOTO BUILD_8936_commonlib
echo **Build errors...
echo dxhdcp2dbg.mbn file not found for msm8936. Please refer to build log for build errors.
EXIT /B

:BUILD_8936_commonlib
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8936 commonlib
IF EXIST %BUILD_ROOT%\build\ms\bin\MAWAANAA\cmnlib.mbn GOTO BUILD_8936_SUCCESS
echo **Build errors...
echo cmnlib.mbn file not found for msm8936. Please refer to build log for build errors.
EXIT /B

:BUILD_8936_SUCCESS
echo ==============================================================================
echo BUILD_ALL.cmd build summary for 8936
echo ==============================================================================
echo BUILD SUCCESS - The following images were generated:
echo MAWAANAA\tz.mbn
echo MAWAANAA\sampleapp.mbn
echo MAWAANAA\securitytest.mbn
echo MAWAANAA\tzbsp_no_xpu.mbn
echo MAWAANAA\playready.mbn
echo MAWAANAA\widevine.mbn
echo MAWAANAA\keymaster.mbn
echo MAWAANAA\tzbsp_with_test.mbn
echo PIL_IMAGES\SPLITBINS_MAWAANAA\tztestexec.mdt
echo MAWAANAA\dummy_venus.mbn
echo MAWAANAA\tzbsptest.mbn
echo MAWAANAA\dxhdcp2.mbn
echo MAWAANAA\dxhdcp2dbg.mbn
echo MAWAANAA\cmnlib.mbn
echo. 2>BuildAllSuccess_8936.txt
GOTO DONE


:BUILD_8909_tz
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909 tz
IF EXIST %BUILD_ROOT%\build\ms\bin\MAZAANAA\tz.mbn GOTO BUILD_8909_sampleapp
echo **Build errors...
echo tz.mbn file not found for msm8909. Please refer to build log for build errors.
EXIT /B


:BUILD_8909_sampleapp
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909 sampleapp
IF EXIST %BUILD_ROOT%\build\ms\bin\MAZAANAA\sampleapp.mbn GOTO BUILD_8909_tzbspnoxpu
echo **Build errors...
echo sampleapp.mbn file not found for msm8909. Please refer to build log for build errors.
EXIT /B

:BUILD_8909_securitytest
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909 securitytest
IF EXIST %BUILD_ROOT%\build\ms\bin\MAZAANAA\securitytest.mbn GOTO BUILD_8909_tzbspnoxpu
echo **Build errors...
echo securitytest.mbn file not found for msm8909. Please refer to build log for build errors.
EXIT /B

:BUILD_8909_tzbspnoxpu
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909 tzbsp_no_xpu
IF EXIST %BUILD_ROOT%\build\ms\bin\MAZAANAA\tzbsp_no_xpu.mbn GOTO BUILD_8909_widevine
echo **Build errors...
echo tzbsp_no_xpu.mbn file not found for msm8909. Please refer to build log for build errors.
EXIT /B

:BUILD_8909_widevine
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909 widevine
IF EXIST %BUILD_ROOT%\build\ms\bin\MAZAANAA\widevine.mbn GOTO BUILD_8909_keymaster
echo **Build errors...
echo widevine.mbn file not found for msm8909. Please refer to build log for build errors.
EXIT /B


:BUILD_8909_keymaster
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909 keymaster
IF EXIST %BUILD_ROOT%\build\ms\bin\MAZAANAA\keymaster.mbn GOTO BUILD_8909_tzbspwithtest
echo **Build errors...
echo keymaster.mbn file not found for msm8909. Please refer to build log for build errors.
EXIT /B

:BUILD_8909_tzbspwithtest
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909 tzbsp_with_test
IF EXIST %BUILD_ROOT%\build\ms\bin\MAZAANAA\tzbsp_with_test.mbn GOTO BUILD_8909_tztestexec
echo **Build errors...
echo tzbsp_with_test.mbn file not found for msm8909. Please refer to build log for build errors.
EXIT /B

:BUILD_8909_tztestexec
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909 tztestexec
IF EXIST %BUILD_ROOT%\build\ms\bin\MAZAANAA\tztestexec.mbn GOTO BUILD_8909_dummy_venus
echo **Build errors...
echo tzbsp_with_test.mbn file not found for msm8909. Please refer to build log for build errors.
EXIT /B

:BUILD_8909_dummy_venus
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909 dummy_venus
IF EXIST %BUILD_ROOT%\build\ms\bin\MAZAANAA\dummy_venus.mbn GOTO BUILD_8909_tzbsptest
echo **Build errors...
echo dummy_venus.mbn file not found for msm8909. Please refer to build log for build errors.
EXIT /B

:BUILD_8909_tzbsptest
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909 tzbsptest
IF EXIST %BUILD_ROOT%\build\ms\bin\MAZAANAA\tzbsptest.mbn GOTO BUILD_8909_commonlib
echo **Build errors...
echo tzbsptest.mbn file not found for msm8909. Please refer to build log for build errors.
EXIT /B


:BUILD_8909_commonlib
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909 commonlib
IF EXIST %BUILD_ROOT%\build\ms\bin\MAZAANAA\cmnlib.mbn GOTO BUILD_8909_SUCCESS
echo **Build errors...
echo cmnlib.mbn file not found for msm8909. Please refer to build log for build errors.
EXIT /B

:BUILD_8909_SUCCESS
echo ==============================================================================
echo BUILD_ALL.cmd build summary for 8909
echo ==============================================================================
echo BUILD SUCCESS - The following images were generated:
echo MAZAANAA\tz.mbn
echo MAZAANAA\sampleapp.mbn
echo MAZAANAA\securitytest.mbn
echo MAZAANAA\tzbsp_no_xpu.mbn
echo MAZAANAA\widevine.mbn
echo MAZAANAA\keymaster.mbn
echo MAZAANAA\tzbsp_with_test.mbn
echo PIL_IMAGES\SPLITBINS_MAZAANAA\tztestexec.mdt
echo MAZAANAA\dummy_venus.mbn
echo MAZAANAA\tzbsptest.mbn
echo MAZAANAA\cmnlib.mbn
echo. 2>BuildAllSuccess_8909.txt
GOTO DONE

:BUILD_8909w_tz
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909w tz
IF EXIST %BUILD_ROOT%\build\ms\bin\MATAANAA\tz.mbn GOTO BUILD_8909w_sampleapp
echo **Build errors...
echo tz.mbn file not found for msm8909w. Please refer to build log for build errors.
EXIT /B


:BUILD_8909w_sampleapp
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909w sampleapp
IF EXIST %BUILD_ROOT%\build\ms\bin\MATAANAA\sampleapp.mbn GOTO BUILD_8909w_tzbspnoxpu
echo **Build errors...
echo sampleapp.mbn file not found for msm8909w. Please refer to build log for build errors.
EXIT /B

:BUILD_8909w_securitytest
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909w securitytest
IF EXIST %BUILD_ROOT%\build\ms\bin\MATAANAA\securitytest.mbn GOTO BUILD_8909w_tzbspnoxpu
echo **Build errors...
echo securitytest.mbn file not found for msm8909w. Please refer to build log for build errors.
EXIT /B

:BUILD_8909w_tzbspnoxpu
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909w tzbsp_no_xpu
IF EXIST %BUILD_ROOT%\build\ms\bin\MATAANAA\tzbsp_no_xpu.mbn GOTO BUILD_8909w_widevine
echo **Build errors...
echo tzbsp_no_xpu.mbn file not found for msm8909w. Please refer to build log for build errors.
EXIT /B

:BUILD_8909w_widevine
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909w widevine
IF EXIST %BUILD_ROOT%\build\ms\bin\MATAANAA\widevine.mbn GOTO BUILD_8909w_keymaster
echo **Build errors...
echo widevine.mbn file not found for msm8909w. Please refer to build log for build errors.
EXIT /B


:BUILD_8909w_keymaster
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909w keymaster
IF EXIST %BUILD_ROOT%\build\ms\bin\MATAANAA\keymaster.mbn GOTO BUILD_8909w_tzbspwithtest
echo **Build errors...
echo keymaster.mbn file not found for msm8909w. Please refer to build log for build errors.
EXIT /B

:BUILD_8909w_tzbspwithtest
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909w tzbsp_with_test
IF EXIST %BUILD_ROOT%\build\ms\bin\MATAANAA\tzbsp_with_test.mbn GOTO BUILD_8909w_tztestexec
echo **Build errors...
echo tzbsp_with_test.mbn file not found for msm8909w. Please refer to build log for build errors.
EXIT /B

:BUILD_8909w_tztestexec
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909w tztestexec
IF EXIST %BUILD_ROOT%\build\ms\bin\MATAANAA\tztestexec.mbn GOTO BUILD_8909w_dummy_venus
echo **Build errors...
echo tzbsp_with_test.mbn file not found for msm8909w. Please refer to build log for build errors.
EXIT /B

:BUILD_8909w_dummy_venus
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909w dummy_venus
IF EXIST %BUILD_ROOT%\build\ms\bin\MATAANAA\dummy_venus.mbn GOTO BUILD_8909w_tzbsptest
echo **Build errors...
echo dummy_venus.mbn file not found for msm8909w. Please refer to build log for build errors.
EXIT /B

:BUILD_8909w_tzbsptest
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909w tzbsptest
IF EXIST %BUILD_ROOT%\build\ms\bin\MATAANAA\tzbsptest.mbn GOTO BUILD_8909w_commonlib
echo **Build errors...
echo tzbsptest.mbn file not found for msm8909w. Please refer to build log for build errors.
EXIT /B


:BUILD_8909w_commonlib
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=msm8909w commonlib
IF EXIST %BUILD_ROOT%\build\ms\bin\MATAANAA\cmnlib.mbn GOTO BUILD_8909w_SUCCESS
echo **Build errors...
echo cmnlib.mbn file not found for msm8909w. Please refer to build log for build errors.
EXIT /B

:BUILD_8909w_SUCCESS
echo ==============================================================================
echo BUILD_ALL.cmd build summary for 8909w
echo ==============================================================================
echo BUILD SUCCESS - The following images were generated:
echo MATAANAA\tz.mbn
echo MATAANAA\sampleapp.mbn
echo MATAANAA\securitytest.mbn
echo MATAANAA\tzbsp_no_xpu.mbn
echo MATAANAA\widevine.mbn
echo MATAANAA\keymaster.mbn
echo MATAANAA\tzbsp_with_test.mbn
echo PIL_IMAGES\SPLITBINS_MATAANAA\tztestexec.mdt
echo MATAANAA\dummy_venus.mbn
echo MATAANAA\tzbsptest.mbn
echo MATAANAA\cmnlib.mbn
echo. 2>BuildAllSuccess_8909w.txt
GOTO DONE

:BUILD_9x07_tz
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=mdm9x07 tz
IF EXIST %BUILD_ROOT%\build\ms\bin\MARAANAA\tz.mbn GOTO BUILD_9x07_sampleapp
echo **Build errors...
echo tz.mbn file not found for mdm9x07. Please refer to build log for build errors.
EXIT /B


:BUILD_9x07_sampleapp
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=mdm9x07 sampleapp
IF EXIST %BUILD_ROOT%\build\ms\bin\MARAANAA\sampleapp.mbn GOTO BUILD_9x07_tzbspnoxpu
echo **Build errors...
echo sampleapp.mbn file not found for mdm9x07. Please refer to build log for build errors.
EXIT /B

:BUILD_9x07_tzbspnoxpu
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=mdm9x07 tzbsp_no_xpu
IF EXIST %BUILD_ROOT%\build\ms\bin\MARAANAA\tzbsp_no_xpu.mbn GOTO BUILD_9x07_keymaster
echo **Build errors...
echo tzbsp_no_xpu.mbn file not found for mdm9x07. Please refer to build log for build errors.
EXIT /B

:BUILD_9x07_keymaster
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=mdm9x07 keymaster
IF EXIST %BUILD_ROOT%\build\ms\bin\MARAANAA\keymaster.mbn GOTO BUILD_9x07_tzbspwithtest
echo **Build errors...
echo keymaster.mbn file not found for mdm9x07. Please refer to build log for build errors.
EXIT /B

:BUILD_9x07_tzbspwithtest
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=mdm9x07 tzbsp_with_test
IF EXIST %BUILD_ROOT%\build\ms\bin\MARAANAA\tzbsp_with_test.mbn GOTO BUILD_9x07_tztestexec
echo **Build errors...
echo tzbsp_with_test.mbn file not found for mdm9x07. Please refer to build log for build errors.
EXIT /B

:BUILD_9x07_tztestexec
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=mdm9x07 tztestexec
IF EXIST %BUILD_ROOT%\build\ms\bin\MARAANAA\tztestexec.mbn GOTO BUILD_9x07_tzbsptest
echo **Build errors...
echo tzbsp_with_test.mbn file not found for mdm9x07. Please refer to build log for build errors.
EXIT /B

:BUILD_9x07_tzbsptest
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=mdm9x07 tzbsptest
IF EXIST %BUILD_ROOT%\build\ms\bin\MARAANAA\tzbsptest.mbn GOTO BUILD_9x07_commonlib
echo **Build errors...
echo tzbsptest.mbn file not found for mdm9x07. Please refer to build log for build errors.
EXIT /B


:BUILD_9x07_commonlib
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=mdm9x07 commonlib
IF EXIST %BUILD_ROOT%\build\ms\bin\MARAANAA\cmnlib.mbn GOTO BUILD_9x07_SUCCESS
echo **Build errors...
echo cmnlib.mbn file not found for mdm9x07. Please refer to build log for build errors.
EXIT /B

:BUILD_9x07_SUCCESS
echo ==============================================================================
echo BUILD_ALL.cmd build summary for 9x07
echo ==============================================================================
echo BUILD SUCCESS - The following images were generated:
echo MARAANAA\tz.mbn
echo MARAANAA\sampleapp.mbn
echo MARAANAA\tzbsp_no_xpu.mbn
echo MARAANAA\keymaster.mbn
echo MARAANAA\tzbsp_with_test.mbn
echo PIL_IMAGES\SPLITBINS_MARAANAA\tztestexec.mdt
echo MARAANAA\tzbsptest.mbn
echo MARAANAA\cmnlib.mbn
echo. 2>BuildAllSuccess_9x07.txt
GOTO DONE

:BUILD_9x09_tz
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=mdm9x09 tz
IF EXIST %BUILD_ROOT%\build\ms\bin\NAYAANAA\tz.mbn GOTO BUILD_9x09_hyp
echo **Build errors...
echo tz.mbn file not found for mdm9x09. Please refer to build log for build errors.
EXIT /B

:BUILD_9x09_hyp
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=mdm9x09 hyp
IF EXIST %BUILD_ROOT%\build\ms\bin\NAYAANAA\tz.mbn GOTO BUILD_9x09_tzbspnoxpu
echo **Build errors...
echo hyp.mbn file not found for mdm9x09. Please refer to build log for build errors.
EXIT /B

:BUILD_9x09_tzbspnoxpu
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=mdm9x09 tzbsp_no_xpu
IF EXIST %BUILD_ROOT%\build\ms\bin\NAYAANAA\tzbsp_no_xpu.mbn GOTO BUILD_9x09_tzbspwithtest
echo **Build errors...
echo tzbsp_no_xpu.mbn file not found for mdm9x09. Please refer to build log for build errors.
EXIT /B


:BUILD_9x09_tzbspwithtest
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=mdm9x09 tzbsp_with_test
IF EXIST %BUILD_ROOT%\build\ms\bin\NAYAANAA\tzbsp_with_test.mbn GOTO BUILD_9x09_tztestexec
echo **Build errors...
echo tzbsp_with_test.mbn file not found for mdm9x09. Please refer to build log for build errors.
EXIT /B

:BUILD_9x09_tztestexec
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=mdm9x09 tztestexec
IF EXIST %BUILD_ROOT%\build\ms\bin\NAYAANAA\tztestexec.mbn GOTO BUILD_9x09_dummy_venus
echo **Build errors...
echo tzbsp_with_test.mbn file not found for mdm9x09. Please refer to build log for build errors.
EXIT /B

:BUILD_9x09_dummy_venus
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=mdm9x09 dummy_venus
IF EXIST %BUILD_ROOT%\build\ms\bin\NAYAANAA\dummy_venus.mbn GOTO BUILD_9x09_tzbsptest
echo **Build errors...
echo dummy_venus.mbn file not found for mdm9x09. Please refer to build log for build errors.
EXIT /B

:BUILD_9x09_tzbsptest
CALL %BUILD_ROOT%\build\ms\build.cmd CHIPSET=mdm9x09 tzbsptest
IF EXIST %BUILD_ROOT%\build\ms\bin\NAYAANAA\tzbsptest.mbn GOTO BUILD_9x09_SUCCESS
echo **Build errors...
echo tzbsptest.mbn file not found for mdm9x09. Please refer to build log for build errors.
EXIT /B


:BUILD_9x09_SUCCESS
echo ==============================================================================
echo BUILD_ALL.cmd build summary for 9x09
echo ==============================================================================
echo BUILD SUCCESS - The following images were generated:
echo NAYAANAA\tz.mbn
echo NAYAANAA\tzbsp_no_xpu.mbn
echo NAYAANAA\tzbsp_with_test.mbn
echo PIL_IMAGES\SPLITBINS_NAYAANAA\tztestexec.mdt
echo NAYAANAA\dummy_venus.mbn
echo NAYAANAA\tzbsptest.mbn
echo. 2>BuildAllSuccess_9x09.txt
GOTO DONE


:BUILD_all
CALL %BUILD_ROOT%\build\ms\build_all.cmd CHIPSET=msm8916
IF NOT EXIST %BUILD_ROOT%\build\ms\bin\MAUAANAA\tz.mbn GOTO BUILD_all_error
IF NOT EXIST %BUILD_ROOT%\build\ms\bin\MAUAANAA\hyp.mbn GOTO BUILD_all_error

CALL %BUILD_ROOT%\build\ms\build_all.cmd CHIPSET=msm8936
IF NOT EXIST %BUILD_ROOT%\build\ms\bin\MAWAANAA\tz.mbn GOTO BUILD_all_error
IF NOT EXIST %BUILD_ROOT%\build\ms\bin\MAWAANAA\hyp.mbn GOTO BUILD_all_error

CALL %BUILD_ROOT%\build\ms\build_all.cmd CHIPSET=msm8909
IF NOT EXIST %BUILD_ROOT%\build\ms\bin\MAZAANAA\tz.mbn GOTO BUILD_all_error
IF NOT EXIST %BUILD_ROOT%\build\ms\bin\MAZAANAA\hyp.mbn GOTO BUILD_all_error

CALL %BUILD_ROOT%\build\ms\build_all.cmd CHIPSET=msm8909w
IF NOT EXIST %BUILD_ROOT%\build\ms\bin\MATAANAA\tz.mbn GOTO BUILD_all_error
IF NOT EXIST %BUILD_ROOT%\build\ms\bin\MATAANAA\hyp.mbn GOTO BUILD_all_error

CALL %BUILD_ROOT%\build\ms\build_all.cmd CHIPSET=mdm9x07
IF NOT EXIST %BUILD_ROOT%\build\ms\bin\MARAANAA\tz.mbn GOTO BUILD_all_error
IF NOT EXIST %BUILD_ROOT%\build\ms\bin\MARAANAA\hyp.mbn GOTO BUILD_all_error

CALL %BUILD_ROOT%\build\ms\build_all.cmd CHIPSET=mdm9x09
IF NOT EXIST %BUILD_ROOT%\build\ms\bin\NAYAANAA\tz.mbn GOTO BUILD_all_error
IF NOT EXIST %BUILD_ROOT%\build\ms\bin\NAYAANAA\hyp.mbn GOTO BUILD_all_error

GOTO DONE_all

:DONE
echo ***BUILDING COMPLETE***
endlocal
EXIT /B

:DONE_all
echo ***BUILDING COMPLETE FOR ALL TARGETS***
echo. 2>BuildAllTargetsSuccess.txt
endlocal
EXIT /B

:BUILD_all_error
echo **Build errors...
echo Please refer to build log for build errors and check the image bin folder(s).
endlocal
EXIT /B
