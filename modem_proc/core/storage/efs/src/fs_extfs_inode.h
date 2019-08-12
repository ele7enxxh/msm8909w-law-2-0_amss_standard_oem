/***********************************************************************
 * fs_extfs_inode.h
 *
 * EXTFS specific members of inode.
 * Copyright (C) 2007 QUALCOMM Technologies, Inc.
 *
 * All the EXTFS specific members of the inode. These members will become part
 * of the vnode.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_extfs_inode.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

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
