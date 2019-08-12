#!/bin/bash
#===============================================================================
#
# CBSP Buils system
#
# General Description
#    build shell script file.
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/tools/build/scons/build/build.sh#1 $
# $DateTime: 2015/09/01 00:30:35 $
# $Author: pwbldsvc $
# $Change: 8935858 $
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
