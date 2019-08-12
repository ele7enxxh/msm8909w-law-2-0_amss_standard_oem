#!/bin/sh
#===============================================================================
#
# Image Top-Level Build Script
#
# General Description
#    Image Top-Level Build Script
#
# Copyright (c) 2009-2012 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/build/ms/build.sh#1 $
# $DateTime: 2016/12/13 07:59:25 $
# $Change: 11985153 $
#
#===============================================================================

cd `dirname $0`

# Call script to setup build environment, if it exists.
if [ -e setenv.sh ]; then
source ./setenv.sh
fi

# Call the main build command
python build_variant.py $*
build_result=$?
if [ "${build_result}" != "0" ] ; then
    exit ${build_result}
fi

