/***********************************************************************
 * fs_nodev.h
 *
 * No device filesystem.
 * Copyright (C) 2006 QUALCOMM Technologies, Inc.
 *
 * A simple filesystem that always returns nodev.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_nodev.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2006-05-08   dlb   Create

===========================================================================*/

#ifndef __FS_NODEV_H__
#define __FS_NODEV_H__

#include "fs_mount.h"

struct fs_mount_nodevfs {
  struct fs_mount       parent;

  struct fs_vnode *root;
};

void fs_nodevfs_init (void);

#endif /* not __FS_NODEV_H__ */
