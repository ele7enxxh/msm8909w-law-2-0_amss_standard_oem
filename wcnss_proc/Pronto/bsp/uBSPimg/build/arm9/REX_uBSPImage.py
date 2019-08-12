#===============================================================================
#
# CoreBSP Modem CPU build script
#
# GENERAL DESCRIPTION
#    build script
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.wcnss/4.1/Pronto/bsp/uBSPimg/build/arm9/REX_uBSPImage.py#1 $
#  $DateTime: 2013/11/26 17:03:43 $
#  $Author: coresvc $
#  $Change: 4861727 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
#
#===============================================================================
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('uBSP Image')

def generate(env):
   #-------------------------------------------------------------------------
   # Libs/Objs
   #-------------------------------------------------------------------------
   uBSP_libs_path = env.get('INSTALL_LIBPATH')
   uBSP_libs = env.get('uBSP_LIBS')
   uBSP_objs = env.get('uBSP_OBJS')      

   uBSP_units = [uBSP_libs, uBSP_objs]
   
   # filter is not define or filter is current target files only, ok to build misc files
   if 'IMAGE_BUILD_LOCAL_FILES' in env:   
      #=========================================================================
      # uBSP Image section
      #=========================================================================
   
      # make our clone so we won't mess the lib rules, it shoun't because they 
      # should of clone, but just to be on the safe side.
      env = env.Clone()
      
      #-------------------------------------------------------------------------
      # Source PATH
      #-------------------------------------------------------------------------
      #uBSP_SRC = "${COREBSP_ROOT}/bsp/coreimg/src"

      #env.VariantDir("${BUILDPATH}", uBSP_SRC, duplicate=0)

      #-------------------------------------------------------------------------
      # External depends within CoreBSP
      #-------------------------------------------------------------------------
      env.RequireExternalApi([
         'DSM',
         'MODEM_API',
      ])

      #-------------------------------------------------------------------------
      # Internal depends within CoreBSP
      #-------------------------------------------------------------------------
      CBSP_API = [
         'BOOT',
         'BUSES',
         'DAL',
         'DEBUGTOOLS',
         'HAL',      
         'HWENGINES',
         'IODEVICES',
         'MPROC',
         'POWER',
         'SECUREMSM',
         'SERVICES',
         'STORAGE',
         'SYSTEMDRIVERS',
         'WIREDCONNECTIVITY',
         
         # needs to be last also contains wrong comdef.h
         'KERNEL',
      ]

      env.RequirePublicApi(CBSP_API)
      env.RequireRestrictedApi(CBSP_API)
      
      #=========================================================================
      # Beging building QCOREIMG
      #
      #----------------------------------------------------------------------------
      # Build env scatter load
      #----------------------------------------------------------------------------
      target_scl = env.SclBuilder('${SHORT_BUILDPATH}/${TARGET_NAME}', [
		 '${BUILD_MS_ROOT}/wcnss_ubsp_image',
      ])
      
      uBSP_units += target_scl
      
      #----------------------------------------------------------------------------
      # Sources, libraries
      #----------------------------------------------------------------------------

      # stubs and other qcore app files
      #uBSP_STUBS_SOURCES = [
      #   '${BUILDPATH}/other_stubs.c',    
      #]
      
      #uBSP_objs.extend(env.Object(WCNIMG_STUBS_SOURCES))      

   # the following steps can only be performed if there is no filters in effect
   if 'IMAGE_BUILD_LINK' in env:
      #-------------------------------------------------------------------------
      # Build env for uBSP image
      #-------------------------------------------------------------------------
      uBSP_elf = env.AddProgram("${SHORT_BUILDPATH}/${TARGET_NAME}", uBSP_objs, LIBS=uBSP_libs)
      env.Depends(uBSP_elf, target_scl)
      
      # copy elf and reloc to needed locations for AMSS tools to load on target
      install_target_elf = env.InstallAs(
         #'${BUILD_MS_ROOT}/${CHIPSET}_${IMAGE_NAME}.elf', rivaimg_elf)
		 '${BUILD_MS_ROOT}/${MSM_ID}_uBSP.elf', uBSP_elf)
      
      #-------------------------------------------------------------------------
      # Build env mbn files
      #-------------------------------------------------------------------------
      target_mbn = env.MbnBuilder('${TARGET_NAME}', uBSP_elf, IMAGE_TYPE="wcnss",
         FLASH_TYPE=env['TARGET_FLASH_TYPE'], ENABLE_ENCRYPT=True)

      target_mbn_copy = env.Install("${BUILD_ROOT}/build/ms/bin/${MSM_ID}/", target_mbn)

      #-------------------------------------------------------------------------
      # Run PIL Splitter Tool
      #-------------------------------------------------------------------------
      split_files = env.PilSplitterBuilder('${BUILD_ROOT}/build/ms/bin/${MSM_ID}/PIL_IMAGES/${MSM_ID}_SPLITBINS_${QC_SHORT_BUILDPATH}/wcnss_ubsp.mdt', target_mbn)
		 
      #============================================================================
      # Define targets needed RIVA QCOREIMG
      uBSP_img_units = env.Alias ('uBSP_img_units', [
         install_target_elf,
         target_mbn,
         target_mbn_copy,
         split_files
      ])

   #print env.Dump()

   #=========================================================================
   # Finish up...
   env.BindAliasesToTargets(uBSP_img_units)
