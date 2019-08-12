#===============================================================================
#
# External paths to CoreBSP
#
# GENERAL DESCRIPTION
#    Contains external paths to CoreBSP which are needed by some CoreBSP units
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/build/externalpaths.py#1 $
#  $DateTime: 2015/09/01 00:30:35 $
#  $Author: pwbldsvc $
#  $Change: 8935858 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 05/04/09   cjb     Created
# 05/15/09   wd      Merge from SCMM
#
#===============================================================================
import os
import subprocess
import string
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('defaults')

def generate(env):
   env.Replace(PLATFORM_ROOT = env['PLAT_ROOT'])
   env.Replace(SRCROOT = env['INC_ROOT'])
   env.Replace(ROOT = env['INC_ROOT'])
   
   #print "PLATFORM_ROOT >" + env['PLAT_ROOT'] + "<"
   #print "SRCROOT >" + env['SRCROOT'] + "<"
   #print "ROOT >" + env['ROOT'] + "<"
   
   env.Replace(CS_ROOT = "${PLAT_ROOT}")
   env.Replace(CS_ROOT = "${INC_ROOT}/platform")
   env.Replace(CS_EXTERNAL = [
      "${CS_INC}",
      "${CS_INC_OEM}",
      "${CS_ROOT}/libstd/inc",
      "${PLAT_ROOT}/cs/inc",
      "${PLAT_ROOT}/cs/inc/pro",
      "${CS_OBSOLETE_FS_INC}",
      "${CS_OBSOLETE_PORT}",
      "${CS_OBSOLETE_QIDL_INC}",
      "${PLAT_ROOT}/cs/src/OEM/OEMFS/msm",
      "$(PLAT_ROOT)/cs/imports/settings/inc",
      "${CS_DEPRECATED_BID}",
   ])

   env.Replace(DSM_EXTERNAL = [
      "${INC_ROOT}/modem/api/mcs",
      "${INC_ROOT}/modem/api/utils",
   ])

   # must end in external to be available to UseExternalApi() legacy function
   env.Replace(MODEM_API_EXTERNAL = [
      "${INC_ROOT}/modem/api/rfa",
      "${INC_ROOT}/modem/api/datacommon",
      "${INC_ROOT}/modem/api/rfa",
      "${MODEM_DATACOMMON_API}",
      "${MMODE_API}",
      "${CGPS_API}",      
      "${DATA_API}",      
      "${NAS_API}",
      "${HDR_API}",
      "${API_MCS}",
      "${GERAN_API}",
      "${API_1X}",
      "${UIM_API}",
      "${WCDMA_API}",
      "${MODEM_API_WMS}",
      "${AU_MODEM_API}/data",
      "${AU_MODEM_API}/datacommon",
      "${AU_MODEM_API}/ecall",
      "${AU_MODEM_API}/geran",
      "${AU_MODEM_API}/mmode",
      "${AU_MODEM_API}/nas",
      "${AU_MODEM_API}/pbm",
      "${AU_MODEM_API}/uim",
      "${AU_MODEM_API}/wcdma",
      "${AU_MODEM_API}/wms",
      "${AU_MODEM_API}/utils",
      "${AU_MODEM_API}/mcs",
      "${AU_MODEM_API}/rfa",       
      "${WPLT}",
      "${AU_MODEM_API}/uim",
      "${AU_MODEM_API}/rapi/mmode/inc",
      "${AU_MODEM_API}/rapi/pbm/inc",
      "${AU_MODEM_API}/rapi/data/inc",
      "${AU_MODEM_API}/rapi/gps/inc",
      "${AU_MODEM_API}/rapi/uim/inc",
      "${AU_MODEM_API}/rapi/wms/inc",
      "${AU_MODEM_API}/rapi/hdr/inc",
      "${AU_MODEM_API}/rapi/rfa/inc", 
      "${MODEM_API_GERAN}",
   ])

    
   return None


