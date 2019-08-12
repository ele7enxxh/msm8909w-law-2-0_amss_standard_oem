#============================================================================
#
# EFS Build flags configurations
#
# GENERAL DESCRIPTION
#   EFS/HFAT/Hotplug/FS Tar can be configured to support different features.
#   The support of the features are controlled by compile time flags. This
#   file implements the method to add all the relevant compile time flags
#   depending on which feature is enabled.
#
# Copyright (c) 2012-2014 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/build/fs_build_config.py#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $ $Change: 8935858 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when        who  what, where, why
# ----------  ---  ----------------------------------------------------------
# 2014-04-01  vm   Add support for dual-OS partitioni sets.
# 2013-09-30  wek  Remove the feature that disables MBR checks.
# 2013-06-17  wek  Increase the number of file descriptors for Bootloaders.
# 2013-05-24  rp   Add UFS support to Hotplug.
# 2013-06-10  nr   Fix Klockwork errors.
# 2013-05-07  wek  Add EFS2 Read Only support.
# 2013-04-03  dks  Add QMI messages to force sync from Apps processor
# 2013-04-02  dks  Make FSG write available in non-customer fusion.
# 2013-03-27  dks  Featurize writing to FSG for golden copy creation.
# 2013-03-08  dks  Add apps sync control, remove timeout in read/write-iovec.
# 2013-03-06  nr   Add build flags to enable efs profiling.
# 2013-02-14  wek  Remove definition of QC_ZLIB, it is being redefined.
# 2013-02-08  dks  Correct the condition check in disabling write-cache.
# 2013-01-17  wek  Featurize the profile module so it can be turned off.
# 2012-09-20  dks  Update rmts layout to support write-cache.
# 2012-08-07  wek  Add zlib compile time cpp flags when bulding zlib.
# 2012-04-09  dks  Support storing/reading cookies to/from emmc partitions.
# 2012-04-12  wek  Prepare the file system modules to move away from custefs.
# 2012-03-18  wek  Reduce hotplug size for bootloaders.
# 2012-03-08  wek  Create.
#
#============================================================================

#----------------------------------------------------------------------------
# Adds CPP defines depending on the flags defined in environment.
#----------------------------------------------------------------------------
def fs_add_cpp_defines (env):
  cpp_flags = fs_get_build_defines (env)
  env.Append (CPPDEFINES = cpp_flags);
  return cpp_flags

#----------------------------------------------------------------------------
# Adds CPP defines depending on the flags defined in environment and creates
# a file with the cpp defines.
#----------------------------------------------------------------------------
def fs_build_cpp_defines (source_obj, sources, env):
  cpp_flags = fs_add_cpp_defines (env)

  env.LoadToolScript('${BUILD_ROOT}/core/storage/efs/build/' \
                     'fs_write_list_builder.py')
  efs_cpp_flags = env.FsWriteList ('${BUILDPATH}/fs_cpp_flags.txt',
                                   sources, FS_WRITE_LIST=cpp_flags)
  env.Depends (source_obj, efs_cpp_flags);

#----------------------------------------------------------------------------
# Returns a list of CPP Defines based on build flags.
#----------------------------------------------------------------------------
def fs_get_build_defines (env):
  cpp_flags = []
  cpp_flags += ['MSG_BT_SSID_DFLT=MSG_SSID_FS']

  if 'BUILD_BOOT_CHAIN' in env:
    cpp_flags += ['FEATURE_EFS_BOOT_ENV']
    cpp_flags += ['FEATURE_EFS_COMPILE_WITHOUT_MD5']
    cpp_flags += ['FEATURE_EFS_COMPILE_WITHOUT_REX']
    cpp_flags += ['FEATURE_EFS_COMPILE_WITHOUT_DIAG']
    cpp_flags += ['FEATURE_EFS_DISABLE_PROFILE']
    cpp_flags += ['FEATURE_EFS_DISABLE_TIMETICK']
    cpp_flags += ['FEATURE_FS_HOTPLUG_DISABLE_TASK']
    cpp_flags += ['HOTPLUG_HAVE_PARTITION_SUPPORT']
    cpp_flags += ['FS_HOTPLUG_NOTIFY_MAX_CLIENTS=1']
    cpp_flags += ['FS_HOTPLUG_MAX_ITERATORS=1']
    cpp_flags += ['FS_HOTPLUG_MAX_HDEVS=4']
    cpp_flags += ['FS_HOTPLUG_MAX_SOFT_PARTITIONS=64']
    cpp_flags += ['EFS_MAX_FILE_DESCRIPTORS=32']
    cpp_flags += ['FS_MAX_ITERATORS=2']
    cpp_flags += ['FS_MAX_EXTFS_MOUNTS=2']
    cpp_flags += ['FEATURE_EFS2']
    cpp_flags += ['FS_PATH_MAX=300']
    cpp_flags += ['FS_NAME_MAX=136']
    cpp_flags += ['FS_VNODE_POOL_SIZE=16']
    if 'USES_EFS_READ_ONLY' in env:
      cpp_flags += ['FEATURE_EFS_EFS2_ON_NAND']
      cpp_flags += ['FEATURE_EFS2_READONLY']
    elif 'USES_HOTPLUG_WITHOUT_EFS' in env:
      env.PrintDebugInfo ('efs', 'HOTPLUG without EFS variant\n')
      cpp_flags += ['FEATURE_HOTPLUG_COMPILE_WITHOUT_EFS']
    else:
      env.PrintDebugInfo ('efs', 'EFS ROMFS in BOOT variant\n')
      cpp_flags += ['FEATURE_EFS_ROMFS']

      cpp_flags += ['FEATURE_EFS_EXTFS']
      cpp_flags += ['FEATURE_EFS_EXTFS_HFAT']

      # Tune for RAM usage
      cpp_flags += ['FS_HFAT_MAXVOLUME_COUNT=2']
      cpp_flags += ['FS_HFAT_MAX_OPEN_FILES=8']
      cpp_flags += ['HFAT_MAX_PATH_UTF16=90']
      cpp_flags += ['HFAT_MAX_PATH_UTF16_ALLOW=89']
  else:
    env.PrintDebugInfo ('efs', 'EFS in AMSS variant')

  if 'USES_FS_HOTPLUG_SD' in env:
    cpp_flags += ['FEATURE_FS_HOTPLUG_SD']

  if 'USES_FS_HOTPLUG_UFS' in env:
    cpp_flags += ['FEATURE_FS_HOTPLUG_UFS']

  if 'USES_FS_FTL' in env:
    cpp_flags += ['FEATURE_EFS_FTL']
    cpp_flags += ['FEATURE_FS_HOTPLUG_FTL']

  if 'USES_FS_HOTPLUG_SD_CPRM' in env:
    cpp_flags += ['FEATURE_FS_HOTPLUG_SD_CPRM']

  if 'USES_FS_HOTPLUG_USB' in env:
    cpp_flags += ['FEATURE_FS_HOTPLUG_USB']

  if ('USES_FS_HOTPLUG_SD' in env)        or \
     ('USES_FS_HOTPLUG_UFS' in env)       or \
     ('USES_FS_HFAT' in env)              or \
     ('USES_FS_FTL' in env)               or \
     ('USES_FS_HOTPLUG_SD_CPRM' in env)   or \
     ('USES_FS_HOTPLUG_USB' in env):
    cpp_flags += ['FEATURE_FS_HOTPLUG']
    if ('USES_FS_NO_EFS2' in env) :
      cpp_flags += ['FEATURE_HOTPLUG_COMPILE_WITHOUT_EFS']

  if 'USES_FS_HFAT' in env:
    cpp_flags += ['FEATURE_EFS_EXTFS']
    cpp_flags += ['FEATURE_EFS_EXTFS_HFAT']

  if 'USES_EFS_SETUP_DEVICE' in env:
    cpp_flags += ['FEATURE_EFS_SETUP_DEVICE']

  if 'USES_EFS_OTP' in env:
    cpp_flags += ['FEATURE_EFS_OTP']

  # Enable TAR/ZIP TAR.
  if ('USES_STORAGE_REMOTEFS_FUSION' in env) or \
     ('USES_FS_ZIP_TAR'              in env):
    cpp_flags += ['FEATURE_EFS_ZIP']
    cpp_flags += ['FEATURE_FS_TAR']
    cpp_flags += ['FEATURE_FS_ZIP_TAR']
  elif ('USES_STORAGE_REMOTEFS' in env) or \
       ('USES_FS_TAR'           in env):
    cpp_flags += ['FEATURE_FS_TAR']

  if 'USES_RCINIT' in env:
    cpp_flags += ['FEATURE_EFS_USE_RCINIT']

  if 'USES_FS_ASYNC_PUT' in env:
    cpp_flags += ['FEATURE_FS_ASYNC_PUT']

  if 'USES_STORAGE_NAND' in env:
    cpp_flags += ['FEATURE_EFS_EFS2_ON_NAND']

  if 'USES_STORAGE_NOR' in env:
    cpp_flags += ['FEATURE_EFS_EFS2_ON_NOR']

  if ('USES_STORAGE_NOR_SPI' in env) or \
     ('USES_NOR_SPI' in env):
    cpp_flags += ['FEATURE_EFS_EFS2_ON_SPI_NOR']

  if 'USES_STORAGE_MNAND' in env:
    cpp_flags += ['FEATURE_EFS_EFS2_ON_MNAND']

  if 'USES_STORAGE_RAMFS' in env:
    cpp_flags += ['FEATURE_EFS_EFS2_ON_RAMFS']

  if ('USES_STORAGE_REMOTEFS' in env) or \
     ('USES_STORAGE_REMOTEFS_FUSION' in env):
    cpp_flags += ['FEATURE_EFS_EFS2_ON_RMTS']
    if ('USES_EFS_DISABLE_WRITE_CACHE' in env) :
      cpp_flags += ['EFS_RMTS_DISABLE_WRITE_CACHE']

  if ('USES_STORAGE_REMOTEFS_FUSION' in env):
    cpp_flags += ['FEATURE_EFS_RMTS_FUSION']

  if ('USES_WRITE_TO_FSG_FOR_GOLDEN_COPY' in env) :
    cpp_flags += ['FEATURE_EFS_RMTS_WRITE_TO_FSG_FOR_GOLDEN_COPY']

  if ('USES_APPS_CONTROLS_EFS_SYNC' in env):
    cpp_flags += ['FEATURE_EFS_APPS_CONTROLS_EFS_SYNC']
	
  if ('USES_APPS_CONTROLS_EFS_SYNC' in env) or \
     ('USES_EFS_MFSE' in env):
    cpp_flags += ['FEATURE_EFS_MFSE']

  if ('USES_EFS_PROFILE_SECURITY_HOLE' in env):
    cpp_flags += ['FS_PROFILE_ENABLE_EXTENSIVE_PROFILE=1']
    cpp_flags += ['FS_PROFILE_ENABLE_TASK_NAME_LOG=1']
    cpp_flags += ['FS_PROFILE_EXTENSIVE_MAX_LOG_FILE_SIZE=2560']

  if ('USES_STORAGE_NAND' in env)     or \
     ('USES_STORAGE_NOR' in env)      or \
     ('USES_STORAGE_NOR_SPI' in env)  or \
     ('USES_STORAGE_MNAND' in env)    or \
     ('USES_STORAGE_RAMFS' in env)    or \
     ('USES_STORAGE_REMOTEFS' in env) or \
     ('USES_STORAGE_REMOTEFS_FUSION' in env):
    # Enable safe benchmark.
    cpp_flags += ['FEATURE_FS_BENCHMARK']

    if env.has_key('APPS_PROC'):
      cpp_flags += ['EFS_DIAG_USE_THIS_SUBSYS_ID=DIAG_SUBSYS_FS_ALTERNATE']

  #--------------------------------------------------------------------------
  # Supress all EFS1 warnings on EFS modules.
  #--------------------------------------------------------------------------
  cpp_flags += ['FEATURE_EFS_SUPPRESS_EFS1_WARNINGS']

  return cpp_flags

