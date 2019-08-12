#==============================================================================
# SCONS Tool for Target-Specific Build Parameters.
#
# Copyright (c) 2012-2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#==============================================================================
#==============================================================================
#
#                        EDIT HISTORY FOR MODULE
#
# $Header: //components/rel/core.wcnss/4.2.4/bsp/build/tbc_core_devcfg.py#1 $
#
# when       who    what, where, why
# -------    ---    -------------------------------------------
# 08/14/12   aa   Initial Revision
#==============================================================================

def generate(env):
   #---------------------------------------------------------------------------
   # Add Device Configuration specific defines
   #---------------------------------------------------------------------------
   # Reason for doing this is because python does 
   # not like the 'x' in targets like '9x15'
   if env['MSM_ID'] in ['9x25']:
      env.Replace(DAL_CONFIG_TARGET_ID = '0x9025')
   elif env['MSM_ID'] in ['8x26']:
      env.Replace(DAL_CONFIG_TARGET_ID = '0x8026')
   elif env['MSM_ID'] in ['8x92']:
      env.Replace(DAL_CONFIG_TARGET_ID = '0x8092') 
   elif env['MSM_ID'] in ['8x94']:
      env.Replace(DAL_CONFIG_TARGET_ID = '0x8094') 
   elif env['MSM_ID'] in ['8x10']:
      env.Replace(DAL_CONFIG_TARGET_ID = '0x8010') 	  
   else:
      env.Replace(DAL_CONFIG_TARGET_ID = '0x${MSM_ID}')

   env.Append(CPPDEFINES = [
      "DAL_CONFIG_IMAGE_WCN",
      "DAL_CONFIG_TARGET_ID=${DAL_CONFIG_TARGET_ID}"
   ])

def exists(env):
    return env.Detect('tbc_core_devcfg')
