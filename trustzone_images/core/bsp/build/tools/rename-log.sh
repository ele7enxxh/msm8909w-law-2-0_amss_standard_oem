#===============================================================================
#
# CBSP Buils system
#
# General Description
#    build rename log file.
#
# Copyright (c) 2009-2009 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/bsp/build/tools/rename-log.sh#1 $
# $DateTime: 2016/06/17 14:31:11 $
# $Author: pwbldsvc $
# $Change: 10727476 $
#                      EDIT HISTORY FOR FILE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     -----------------------------------------------------------
#
#===============================================================================

log_file_name="build-log"
log_file_ext=".txt"
log_file="$log_file_name$log_file_ext"

if [ -f $log_file ]; then
   for i in {001..1000}; do
      i=`printf '%.03d' $i`
      old_log_file="$log_file_name-$i$log_file_ext"
      
      if [ ! -f $old_log_file ]; then
         break
      fi
   done
fi 

if [ $i != "1000" ]; then
   mv $log_file $old_log_file
fi
