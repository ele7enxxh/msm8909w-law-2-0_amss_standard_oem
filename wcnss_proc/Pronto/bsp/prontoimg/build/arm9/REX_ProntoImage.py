#===============================================================================
#
# CoreBSP Pronto CPU build script
#
# GENERAL DESCRIPTION
#    build script
#
# Copyright (c) 2009,2015 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //source/qcom/qct/core/pkg/wcnss/pr/rel/2.1/Pronto/bsp/prontoimg/build/arm9/REX_ProntoImage.py#3 $
#  $DateTime: 2013/07/03 18:30:20 $
#  $Author: coresvc $
#  $Change: 4043138 $
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

import subprocess

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('Pronto Image')

def generate(env):
   #-------------------------------------------------------------------------
   # Libs/Objs
   #-------------------------------------------------------------------------
   prontoimg_libs_path = env.get('INSTALL_LIBPATH')
   prontoimg_libs = env.get('PRONTOIMG_LIBS')
   prontoimg_objs = env.get('PRONTOIMG_OBJS')      

   prontoimg_units = [prontoimg_libs, prontoimg_objs]
   
   # filter is not define or filter is current target files only, ok to build misc files
   if 'IMAGE_BUILD_LOCAL_FILES' in env:   
      #=========================================================================
      # Pronto Image section
      #=========================================================================
   
      # make our clone so we won't mess the lib rules, it shoun't because they 
      # should of clone, but just to be on the safe side.
      env = env.Clone()
      
      #-------------------------------------------------------------------------
      # Source PATH
      #-------------------------------------------------------------------------
      #ProntoIMG_SRC = "${COREBSP_ROOT}/bsp/coreimg/src"

      #env.VariantDir("${BUILDPATH}", ProntoIMG_SRC, duplicate=0)

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
      if (str(env['MSM_ID'])=='8909'):
        target_scl = env.SclBuilder('${SHORT_BUILDPATH}/${TARGET_NAME}_pre_reloc_changes', ['${BUILD_MS_ROOT}/wcnss_pronto_image_Q3',])
        print "\n BIT using Q3 SCL file for 8909 Target Only"
        env.Replace(USES_QSHRINK2 = False)
      else:
        target_scl = env.SclBuilder('${SHORT_BUILDPATH}/${TARGET_NAME}_pre_reloc_changes', ['${BUILD_MS_ROOT}/wcnss_pronto_image',])
        print "\n BIT using  DEFAULT "
      prontoimg_units += target_scl
      
   # the following steps can only be performed if there is no filters in effect
   if 'IMAGE_BUILD_LINK' in env:   
        #-------------------------------------------------------------------------
        # Build env for Pronto image
        #-------------------------------------------------------------------------
        prontoimg_elf_pre_reloc_changes = env.AddProgram("${SHORT_BUILDPATH}/${TARGET_NAME}_pre_reloc_changes", prontoimg_objs, LIBS=prontoimg_libs)

        if 'USES_RELOC_ENABLE_AND_ALIGN' in env:
            prontoimg_elf = env.ModifyRelocatableFlagsBuilder( '${SHORT_BUILDPATH}/M${TARGET_FAMILY}${BUILD_ID}${BUILD_VER}.elf', prontoimg_elf_pre_reloc_changes )
        else:
            prontoimg_elf = prontoimg_elf_pre_reloc_changes
             
        env.Depends(prontoimg_elf, target_scl)

        # copy elf and reloc to needed locations for AMSS tools to load on target
        if 'USES_PRONTO_MEMORY_REDUCED' in env:
            install_elf_name = '${BUILD_MS_ROOT}/${MSM_ID}_PRONTO_NODEVCFG_MR.elf'
        else:
            install_elf_name = '${BUILD_MS_ROOT}/${MSM_ID}_PRONTO_NODEVCFG.elf'
    
        #-------------------------------------------------------------------------
        # Install ELF, reloc files
        #-------------------------------------------------------------------------      
        if 'USES_QSHRINK2' in env:
            env.LoadToolScript("${BUILD_ROOT}/build/myps/qshrink/Qshrink20.py")  

            prontoimg_final_elf = env.Qshrink20(
                   "${SHORT_BUILDPATH}/${TARGET_NAME}_qsr2",
                   [prontoimg_elf, "${BUILD_ROOT}/build/myps/qshrink/msg_hash.txt"])

        else:
            prontoimg_final_elf = prontoimg_elf

        # copy elf and reloc to needed locations for AMSS tools to load on target
        install_target_elf = env.InstallAs(install_elf_name, prontoimg_final_elf)

        #add builder for computing memory statistics from map/elf
        env.LoadToolScript("${BUILD_ROOT}/Pronto/bsp/build/imageStats.py")  
        imageStats_ok = env.ImageStats(
                "${BUILD_MS_ROOT}/${BUILD_ID}_imageSize.ok",
                [prontoimg_final_elf])

        #============================================================================
        # Define targets needed RIVA QCOREIMG
        prontoimg_units = env.Alias ('pronto_img_units', [
          install_target_elf,
          imageStats_ok
        ])

   #=========================================================================
   # Finish up...
   env.BindAliasesToTargets(prontoimg_units)