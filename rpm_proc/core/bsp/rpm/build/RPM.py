#===============================================================================
#
# RPM image build script
#
# GENERAL DESCRIPTION
#    build script
#
# Copyright (c) 2009-2011 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary and Confidential
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/rpm.bf/2.1.1/core/bsp/rpm/build/RPM.py#6 $
#  $DateTime: 2016/01/13 07:43:18 $
#  $Author: pwbldsvc $
#  $Change: 9716131 $
#
#===============================================================================
import os
import glob
import imp
import string
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('RPM Image')

def generate(env):
   #------------------------------------------------------------------------------
   # Decide build steps
   #------------------------------------------------------------------------------
   # That is, if the user has asked for a filtered build, don't try to link.
   filter_opt = env.get('FILTER_OPT')
   do_link = False if filter_opt is not None else True
   do_local_files = False if not do_link and not env.FilterMatch(os.getcwd()) else True

   #-------------------------------------------------------------------------
   # Libs/Objs
   #-------------------------------------------------------------------------
   rpm_libs_path = env.get('INSTALL_LIBPATH')
   rpm_libs = env.get('RPM_LIBS')
   rpm_objs = env.get('RPM_OBJS')      

   rpm_units = [rpm_libs, rpm_objs]

   if do_local_files:
      #---------------------------------------------------------------------------
      # RPM image-level sources
      #---------------------------------------------------------------------------
      env = env.Clone()

      RPM_SRC = "../src"
      env.VariantDir("${BUILD_PATH}", RPM_SRC, duplicate=0)
      env.Append(CFLAGS = '--c99')
      env.Append(CCFLAGS = '--dollar')

      if env['BUILD_VER'].isdigit():
        build_id = str(int(env['BUILD_VER']))
      else:
        build_id = str(zlib.adler32(env['BUILD_VER']) & 0xFFFFFFFF)
      env.Append(CPPDEFINES = [('RPM_BUILD_ID', build_id)])

      if env['MSM_ID'] == '8916':
         env.Append(CPPDEFINES = 'MSM8916_IMAGE_LAYOUT')
      if env['MSM_ID'] == '8936':
         env.Append(CPPDEFINES = 'MSM8936_STUBS')
         env.Append(CPPDEFINES = 'NO_STACK_CHECK') 
      if env['MSM_ID'] == '8909':
         env.Append(CPPDEFINES = 'MSM8909_IMAGE_LAYOUT')
      if env['MSM_ID'] == '8909W':
         env.Append(CPPDEFINES = 'MSM8909_IMAGE_LAYOUT')
      if env['MSM_ID'] == '9x45':
         env.Append(CPPDEFINES = 'MDM9x45_IMAGE_LAYOUT')
         env.Append(CPPDEFINES = 'MDM9x45_PROXY_VOTES')
         env.Append(CPPDEFINES = 'NO_STACK_CHECK') #remove once PRNG driver is delivered... ETA is JUNE
      if env['MSM_ID'] == '8952':
         env.Append(CPPDEFINES = 'MSM8952_STUBS')
         env.Append(CPPDEFINES = 'NO_STACK_CHECK')
		 
      if env['MSM_ID'] == '9x07':
         env.Append(CPPDEFINES = 'MDM9x07_IMAGE_LAYOUT')
         env.Append(CPPDEFINES = 'PMIC_S1_PC_PROXY_VOTE')

      #----------------------------------------------------------------------------
      # Dependencies on CoreBSP code
      #----------------------------------------------------------------------------
      CBSP_API = [
         'BOOT',
         'BUSES',
         'DAL',
         'DEBUGTOOLS',
         'MPROC',
         'POWER',
         'SYSTEMDRIVERS',
         'DEBUGTRACE',
         #'PrngML_API',
         'SERVICES'
      ]

      env.RequirePublicApi(CBSP_API)
      env.RequireRestrictedApi(CBSP_API)

      #----------------------------------------------------------------------------
      # Sources, libraries
      #----------------------------------------------------------------------------

      # stubs and other qcore app files
      RPM_SOURCES = [
         'startup.s',
         'main.c',
         'retarget.c',
         'exceptions.c',
         'coredump.c',
         'time_service.c',
         'xpu_init.c',
         'image_layout.c',
         'stack_protect.c',
      ]
      RPM_SPECIAL_SOURCES = [
         'datetime.c'
      ]
      rpm_special_objs = env.Object(RPM_SPECIAL_SOURCES)
      for so in rpm_special_objs:
         so.set_always_build ()
      rpm_objs.extend(env.Object(RPM_SOURCES))
      rpm_objs.extend(rpm_special_objs)

   # Add CMM Scripts to the menu
   if env['IMAGE_NAME'] in ['RPM']:
      SRCPATHSCRIPTS = env['BUILD_ROOT']+'/core/bsp/rpm/scripts'
      env.AddCMMScripts ('RPM', [SRCPATHSCRIPTS], { 'rpm_dump.cmm' : 'RPM Core Dump'}, 'Kernel')


   if do_link:
      target_scl = env.SclBuilder('${TARGET_NAME}', [
         '../src/rpm.scl', 
      ])

      rpm_elf = env.Program('${TARGET_NAME}', rpm_objs, LIBS=rpm_libs, LIBPATH=rpm_libs_path)
      rpm_elf_2 = env.RpmElfBuilder ('${TARGET_NAME}' + '_rpm', rpm_elf)

      env.Append(USE_MBN_TOOLS = 'True')
      env.AddUsesFlags('USES_MBNTOOLS')
      env.Replace(GLOBAL_DICT = {'IMAGE_KEY_RPM_IMG_DEST_ADDR' : 0xFC100080})
      rpm_mbn = env.MbnBuilder('${TARGET_NAME}', rpm_elf_2, IMAGE_TYPE="rpm", FLASH_TYPE="sdcc", MBN_TYPE="elf");

      env.Depends(rpm_elf, target_scl)
      env.Clean(rpm_elf, env.subst('${TARGET_NAME}.map'))
      env.Clean(rpm_elf, env.subst('${TARGET_NAME}.sym'))

      #----------------------------------------------------------------------------
      # Sectools signing
      #----------------------------------------------------------------------------

      if env['MSM_ID'] in ['8909','8909W']:
         install_unsigned_root = env.SectoolGetUnsignedInstallPath(install_base_dir = '${BUILD_ROOT}/build/ms/bin/${MSM_ID}/${CHIPSET_PMIC}/')
         sectools_base_dir = '${BUILD_ROOT}/build/ms/bin/${MSM_ID}/${CHIPSET_PMIC}'
      else: 
         install_unsigned_root = env.SectoolGetUnsignedInstallPath(install_base_dir = '${BUILD_ROOT}/build/ms/bin/${MSM_ID}/')     
         sectools_base_dir = '${BUILD_ROOT}/build/ms/bin/${MSM_ID}'

      install_rpm_mbn = env.InstallAs(os.path.join(install_unsigned_root, 'rpm.mbn'), rpm_mbn)

      sectools_signed_mbn = env.SectoolBuilder(
         target_base_dir = '${SHORT_BUILDPATH}', 
         source=rpm_mbn, 
         sign_id="rpm",
         msmid = env.subst('${MSM_ID}'),
         sectools_install_base_dir = sectools_base_dir,  
         install_file_name = "rpm.mbn")

      rpm_units = env.Alias('rpm_units', [
         rpm_elf,
         rpm_elf_2,
         rpm_mbn,
         install_rpm_mbn,
         sectools_signed_mbn,
      ])

   # add aliases
   aliases = env.get('IMAGE_ALIASES')
   for alias in aliases:
      env.Alias(alias, rpm_units)
