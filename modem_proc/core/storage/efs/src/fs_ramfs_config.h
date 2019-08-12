/***********************************************************************
 * fs_ramfs_config.h
 *
 * Configuration file for RamFS.
 * Copyright (C) 2010, 2012 QUALCOMM Technologies, Inc.
 *
 * Configuration file RamFS.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_ramfs_config.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-05-25   nr    Allow RAMFS and RMTS to have configurable page size.
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2010-12-06   rp    Allow FS_RAMFS_SIZE to be overridden.
2010-11-16   wek   Remove dependency with page size macro.
2010-05-07   rp    Create

===========================================================================*/

#ifndef __FS_RAMFS_CONFIG_H__
#define __FS_RAMFS_CONFIG_H__

#include "fs_config_i.h"

#ifdef FEATURE_EFS_EFS2_ON_RAMFS

/*-----------------------------------------------------------------------------
  Configure the RamFS size.
-----------------------------------------------------------------------------*/
#ifdef FEATURE_EFS_RAMFS_SIZE_IN_BYTES
  #define FS_RAMFS_SIZE                 FEATURE_EFS_RAMFS_SIZE_IN_BYTES
#else
  #define FS_RAMFS_SIZE                 (1024*1024) /* Default is 1MB. */
#endif


/*-----------------------------------------------------------------------------
  Configure the cluster size.
  The default value is 512 bytes. The reasons are: 1. this is a well tested
  value in the file system 2. It will better divide the storage space, e.g.
  Storing a 200 byte file we will waste 312 bytes, as opposed to 1948 bytes
  if cluster size is 2k.
-----------------------------------------------------------------------------*/
#ifndef FS_RAMFS_PAGE_SIZE_IN_BYTES
  #define FS_RAMFS_PAGE_SIZE_IN_BYTES         (512)
#endif


#else /* (!FEATURE_EFS_EFS2_ON_RAMFS) */

  /* Keep lint happy. */
  extern int  __dont_complain_about_empty_file;

#endif /* FEATURE_EFS_EFS2_ON_RAMFS */


#endif /* not __FS_RAMDISK_CONFIG_H__ */
