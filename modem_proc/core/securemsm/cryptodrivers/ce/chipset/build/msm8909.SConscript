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
#  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/cryptodrivers/ce/chipset/build/msm8909.SConscript#1 $
#  $DateTime: 2016/12/13 07:59:23 $
#  $Author: mplcsds1 $
#  $Change: 11985146 $
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

# 8909 interface redirection
CHIPSETREDIRECTION = 'msm8909'

script_sconfname = None
if os.path.exists('../' + CHIPSETREDIRECTION + '/build/SConscript') :
   script_sconfname = '../' + CHIPSETREDIRECTION + '/build/SConscript'
else :
   print "Directory: [%s] doesnot exist"%script_sconfname
   raise RuntimeError, "msm8909  [%s] does not exist"%script_sconfname

DEL_FILES = [
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/build/msm8x10.SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/build/msm8x26.SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/build/msm8926.SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/build/mdm9x25.SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/build/mdm9609.SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/build/mdm9607.SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/build/msm8916.SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/build/msm8936.SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/build/msm8937.SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/build/msm8952.SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/build/msm8962.SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/build/msm8974.SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/build/msm8976.SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/msm8976/build/SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/msm8974/build/SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/msm8962/build/SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/msm8952/build/SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/msm8937/build/SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/msm8936/build/SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/msm8916/build/SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/mdm9607/build/SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/mdm9609/build/SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/msm8x26/build/SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/msm8926/build/SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/msm8x10/build/SConscript',
    '${BUILD_ROOT}/core/securemsm/cryptodrivers/ce/chipset/mdm9x25/build/SConscript',
]

env.CleanPack(['SINGLE_IMAGE', 'CBSP_SINGLE_IMAGE',
      'MODEM_IMAGE',  'CBSP_MODEM_IMAGE',
      'APPS_IMAGE',   'CBSP_APPS_IMAGE',
      'QDSP6_SW_IMAGE', 'CBSP_QDSP6_SW_IMAGE',
      'MBA_CORE_SW'
     ], DEL_FILES)

#-------------------------------------------------------------------------------
# Load sub scripts
#-------------------------------------------------------------------------------
script_scon = None
script_scon = '../' + CHIPSETREDIRECTION + '/build'

env.SConscript(dirs=[script_scon], exports='env')
