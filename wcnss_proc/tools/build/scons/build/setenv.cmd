@echo off
rem ==========================================================================
rem
rem  Set Environment Script for Windows
rem
rem  General Description
rem     Setup Qualcomm-specific environment variables for the build system.
rem
rem Copyright (c) 2009-2012 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem
rem --------------------------------------------------------------------------
rem
rem $Header: //source/qcom/qct/core/pkg/wcnss/pr/rel/2.1/tools/build/scons/build/setenv.cmd#1 $
rem $DateTime: 2013/03/01 13:12:26 $
rem $Change: 3424470 $
rem
rem ==========================================================================

call %1.cmd
set  BUILDSPEC=KLOCWORK
set > setenv.dat
