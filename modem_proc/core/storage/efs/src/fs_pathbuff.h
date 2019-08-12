/***********************************************************************
 * fs_pathbuff.h
 *
 * Provides array of FS_PATH_MAX sized buffers, a lock is also available
 * to mutually access these buffers.
 *
 * Copyright (C) 2006-2007, 2009-2010 QUALCOMM Technologies, Inc.
 *
 * There are an array of fs_path_buff[] buffers provided, use
 * FS_PATH_BUFF_LOCK() and FS_PATH_BUFF_UNLOCK() to exclusively
 * access these arrays.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_pathbuff.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-06-12   ebb   Moved rex_xxx calls to fs_osal.h abstraction.
2007-11-16    rp   Increased buffers from 2 to 4 for efs_chown_recursive().
2007-03-02   umr   Updated copyright year.
2007-01-31    rp   Added fs_temp_buff and g_fs_rollback_file.
2006-11-02    rp   Create

===========================================================================*/

#ifndef __FS_PATHBUFF_H__
#define __FS_PATHBUFF_H__

#include "fs_osal.h"
#include "fs_config_i.h"
#include "fs_rollback.h"


/* How many buffers are available in our fs_path_buff[] array */
#define EFS_MAX_PATH_BUFF_CNT           4

/* How many temporary buffers are available in our fs_temp_buff[] array */
#define EFS_MAX_TEMP_BUFF_CNT           1

/* How big in bytes, each of our temp-buff is */
#define EFS_MAX_TEMP_BUFF_ITEM_SIZE     1024 /* 1-KB */


/* These array of buffers are available for mutual access. The caller
 * has to call FS_PATH_BUFF_LOCK() and FS_PATH_BUFF_UNLOCK() to mutually
 * access these arrays. The size of each member of this array is big
 * enough to store a file-path including the '\0' character.
*/
extern char fs_path_buff[EFS_MAX_PATH_BUFF_CNT][FS_PATH_MAX + 1];

/* These temporary buffers are available for mutual access provided the
   caller calls FS_PATH_BUFF_LOCK() before using the buffer and calls
   FS_PATH_BUFF_UNLOCK() after using it. */
extern uint8 fs_temp_buff[EFS_MAX_TEMP_BUFF_CNT][EFS_MAX_TEMP_BUFF_ITEM_SIZE];

/* An instance fs_rollback_file_info structure, mutual access to this
   structure is provided by the FS_PATH_BUFF_LOCK() and FS_PATH_BUFF_UNLOCK()
   calls. */
extern struct fs_rollback_file_info  g_fs_rbk_file;

#ifdef FS_UNIT_TEST

#define FS_PATH_BUFF_LOCK_INIT()        (void)0
#define FS_PATH_BUFF_LOCK()             (void)0
#define FS_PATH_BUFF_UNLOCK()           (void)0

#else

extern fs_os_crit_sect_type fs_path_buff_crit_sec;

#define FS_PATH_BUFF_LOCK_INIT() \
         fs_os_init_crit_sect (&fs_path_buff_crit_sec)

#define FS_PATH_BUFF_LOCK()   fs_os_enter_crit_sect (&fs_path_buff_crit_sec)

#define FS_PATH_BUFF_UNLOCK() fs_os_leave_crit_sect (&fs_path_buff_crit_sec)

#endif /* FS_UNIT_TEST */




#endif /* not __FS_PATHBUFF_H__ */
