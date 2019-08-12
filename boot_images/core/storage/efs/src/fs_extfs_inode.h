/***********************************************************************
 * fs_extfs_inode.h
 *
 * EXTFS specific members of inode.
 * Copyright (C) 2007 Qualcomm Technologies, Inc.
 *
 * All the EXTFS specific members of the inode. These members will become part
 * of the vnode.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_extfs_inode.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2007-08-18   rp    Create

===========================================================================*/

#ifndef __FS_EXTFS_INODE_H__
#define __FS_EXTFS_INODE_H__

#ifdef FEATURE_EFS_EXTFS

struct extfs_inode {
  int       fd;
  int       fd_mode;
};

#endif /* FEATURE_EFS_EXTFS */

#endif /* not __FS_EXTFS_INODE_H__ */
