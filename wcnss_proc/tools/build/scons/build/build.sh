#!/bin/bash
#===============================================================================
#
# CBSP Buils system
#
# General Description
#    build shell script file.
#
# Copyright (c) 2009-2009 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //source/qcom/qct/core/pkg/wcnss/pr/rel/2.1/tools/build/scons/build/build.sh#1 $
# $DateTime: 2013/03/01 13:12:26 $
# $Author: coresvc $
# $Change: 3424470 $
#                      EDIT HISTORY FOR FILE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     -----------------------------------------------------------
#
#===============================================================================

setenv=`export`
starttime="$(date +%s)"
starttimefmt=`date --date='@'$starttime`

echo "Start Time = $starttimefmt" > build-log.txt
echo "#-------------------------------------------------------------------------------" >> build-log.txt
echo "# ENVIRONMENT BEGIN" >> build-log.txt
echo "#-------------------------------------------------------------------------------" >> build-log.txt
export >> build-log.txt
echo "#-------------------------------------------------------------------------------" >> build-log.txt
echo "# ENVIRONMENT END" >> build-log.txt
echo "#-------------------------------------------------------------------------------" >> build-log.txt

echo "#-------------------------------------------------------------------------------" >> build-log.txt
echo "# BUILD BEGIN" >> build-log.txt
echo "#-------------------------------------------------------------------------------" >> build-log.txt
scriptdir=`dirname $0`
echo "$scriptdir/../SCons/scons $*" >> build-log.txt
echo "$scriptdir/../SCons/scons $*"
chmod +x $scriptdir/../SCons/scons
$scriptdir/../SCons/scons $* 2>&1 | tee -a build-log.txt
pipe_status=${PIPESTATUS[0]}

echo "#-------------------------------------------------------------------------------" >> build-log.txt
echo "# BUILD END" >> build-log.txt
echo "#-------------------------------------------------------------------------------" >> build-log.txt
endtime="$(date +%s)"
endtimefmt=`date --date='@'$endtime`
elapsedtime=$(expr $endtime - $starttime)
echo
echo "Start Time = $starttimefmt - End Time = $endtimefmt" >> build-log.txt
echo "Elapsed Time = $elapsedtime seconds" >> build-log.txt

echo "Start Time = $starttimefmt - End Time = $endtimefmt"
echo "Elapsed Time = $elapsedtime seconds"

if [ "${pipe_status}" != "0" ] ; then
    echo SCons Result: ${pipe_status}
    exit ${pipe_status}
fi

export sa_bin_file=/prj/qct/asw/StaticAnalysis/Linux/bin/SAGenConfig.pl
if [ -f "$sa_bin_file" ]
then
    echo "$sa_bin_file found."
    echo $BUILD_ROOT
    #perl $sa_bin_file -r $BUILD_ROOT/../ --env wcnss_proc/core/bsp/build/setenv.sh    
    if [ -f "$BUILD_ROOT/../static_analysis/sa_manifest.txt" ]
    then
        echo "ENVIRONMENTSETTER = wcnss_proc/core/bsp/build/setenv.sh" >> $BUILD_ROOT/../static_analysis/sa_manifest.txt
    fi
else
    echo "$sa_bin_file not found."
fi

