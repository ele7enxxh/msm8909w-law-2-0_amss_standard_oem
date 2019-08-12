/***********************************************************************
 * fs_task.h
 *
 * FS Task place holder.
 * Copyright (C) 2009-2010 Qualcomm Technologies, Inc.
 *
 * The FS-task is as of now in EFS1 VU, when we finally kill EFS1, we have
 * to move the FS-task from EFS1 to this file.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/storage/fs_task.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2010-07-12   rp   Added defines for major and minor number for API files.
2010-04-28   vr   Relocate fs_task from EFS1 to EFS2
2009-11-16   rp   Create

===========================================================================*/

#ifndef __FS_TASK_H__
#define __FS_TASK_H__

#include "comdef.h"

/** Identifies the major version number of this API file. */
#define VER_FS_TASK_H_MAJOR     1
/** Identifies the minor version number of this API file. */
#define VER_FS_TASK_H_MINOR     1

/**
 *
 * The file system task.
 *
 * Main file systems task that initializes EFS and handles signals from
 * the TMC task.
 *
 * @param parm
 *   parameter from rex that is ignored.
 *
 * This function never returns.
 *
 **************************************************************************/
void fs_task (dword parm);


#endif /* not __FS_TASK_H__ */
