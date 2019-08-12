#===============================================================================
#
# SECUREMSM Libs
#
# GENERAL DESCRIPTION
#    build script
#
# Copyright (c) 2015 Qualcomm Technologies Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/rpm.bf/2.1.1/core/securemsm/cryptodrivers/prng/chipset/build/mdm9x07.SConscript#1 $
#  $DateTime: 2015/08/17 04:40:06 $
#  $Author: pwbldsvc $
#  $Change: 8824267 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 09/26/11   nk      Intial checkin
#===============================================================================
import os
Import('env')
env = env.Clone()

# 9x07 interface redirection
CHIPSETREDIRECTION = 'mdm9x07'

script_sconfname = None
if os.path.exists('../' + CHIPSETREDIRECTION + '/build/SConscript') :
   script_sconfname = '../' + CHIPSETREDIRECTION + '/build/SConscript'
else :
   print "Directory: [%s] doesnot exist"%script_sconfname
   raise RuntimeError, "mdm9x07  [%s] does not exist"%script_sconfname

#-------------------------------------------------------------------------------
# Load sub scripts
#-------------------------------------------------------------------------------
script_scon = None
script_scon = '../' + CHIPSETREDIRECTION + '/build'

env.SConscript(dirs=[script_scon], exports='env')
