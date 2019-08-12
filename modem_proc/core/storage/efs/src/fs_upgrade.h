/***********************************************************************
 * fs_upgrade.h
 *
 * EFS2 in-place upgrade management.
 * Copyright (C) 2006 - 2008 QUALCOMM Technologies, Inc.
 *
 * Manage and process the in-place upgrade of EFS2.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_upgrade.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-01-07   yg    Removed unnecessary history comments.
2007-08-31   yg    Added some code comments.
2007-07-06   yg    Add upgrade fix for dual alloc version.
2006-08-31   dlb   Add query for all upgrades finished.
2006-08-15   dlb   Create

===========================================================================*/

#ifndef __FS_UPGRADE_H__
#define __FS_UPGRADE_H__

/* During the boot process, the code will register a particular filesystem
 * that is being visited.  It may then decide that certain upgrades need to
 * be performed.  Later, during various points in the filesystem startup,
 * the upgrades will be checked for, and performed in place. */

void fs_upgrade_init (void);

/* Indicate that future upgrades will reference the specified filesystem.
 * This is not used currently, but XXX: will some kind of mechanism will be
 * needed to support more than one EFS2 filesystem. */
// void fs_upgrade_setup (fs_super_t super);

/* Add the specified upgrade to the list of upgrades that need to be
 * performed. */
void fs_upgrade_add (unsigned kind);

/* Indicate that the specified upgrade has been performed. */
void fs_upgrade_finished (unsigned kind);

/**********************************************************************
 * These are the various upgrades that we now how to do.  Each upgrade has
 * a specific point in time that can be performed at.  The upgrade code
 * itself helps manage this. */

/* Zero the byte following the CRC in any available log pages.  Needed to
 * upgrade to FS_SUPER_VERSION_LOGCRC from an earlier version.  The upgrade
 * will be performed as part of the log page scan on startup. */
#define FS_UPGRADE_LOG_ZERO_AFTER_CRC   1

/* Database nodes did not properly use their 'gid' field resulting
 * in the use of uninitialized gid fields, and possible reservation leaks
 * and assertion fails.  The upgrade will reset all db nodes to the default
 * group.  This will leak data currently in the database, but prevent
 * erroneous deallocations when nodes are freed. */
#define FS_UPGRADE_DBGID_FIX            2

/* Previous version didn't use the field alloc_next[2] and its
 * value was initialized to 0 which is a valid value. Since in
 * newer version storing the Meta Data Allocator in this field,
 * we would not know if the 0 value stored is intended value
 * stored or is carried from previous version. So while upgrading
 * we can assign invalid page id to this, so that PM can reassign
 * proper value later to point to an erased block. */
#define FS_UPGRADE_DUAL_ALLOC_FIX       3

/**********************************************************************
 * Other parts of the code can query the upgrades.
 */

/* Does the specified upgrade need to be done?  Returns a C boolean (0
 * false, 1 true). */
unsigned fs_upgrade_check (unsigned kind);

/* Have all upgrades been performed.  Returns a C boolean. */
unsigned fs_upgrades_all_finished (void);

#endif /* not __FS_UPGRADE_H__ */
