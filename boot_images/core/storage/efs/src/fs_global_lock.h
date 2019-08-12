/***********************************************************************
 * fs_global_lock.h
 *
 * FS_GLOBAL_LOCK critical section
 * Copyright (C) 2007, 2009 Qualcomm Technologies, Inc.
 *
 * The FS_GLOBAL_LOCK is used to protect the file system data from
 * concurrent access.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_global_lock.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-04-01   ebb   Expose protect/unprotect memory fns for format/checkfat.
2007-08-19   rp    Create

===========================================================================*/

#ifndef __FS_GLOBAL_LOCK_H__
#define __FS_GLOBAL_LOCK_H__

void fs_global_lock_init (void);
void fs_global_lock (void);
void fs_global_unlock (void);
void fs_efs2_unprotect_memory (void);
void fs_efs2_protect_memory (void);

#define FS_GLOBAL_LOCK_INIT() fs_global_lock_init()
#define FS_GLOBAL_LOCK()     fs_global_lock ()
#define FS_GLOBAL_UNLOCK()   fs_global_unlock ()


#endif /* not __FS_GLOBAL_LOCK_H__ */

