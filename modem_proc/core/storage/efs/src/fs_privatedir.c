/***********************************************************************
 * fs_privatedir.c
 *
 * This module reserves a folder only to be available for EFS-Module. Other
 * EFS-Module's could create  their own-specific directories inside this
 * folder.
 *
 * Copyright (C) 2006-2007,2012 QUALCOMM Technologies, Inc.
 *
 * EFS-Module will reserve a particular folder for its internal storage, and
 * will prevent diag-users from accessing this folder.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_privatedir.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-11-30    rp   Add the /safe directory protected from Diag-access.
2007-11-16    rp   Blow away the temp-directory in our private folder on boot.
2006-11-02    rp   Create

===========================================================================*/

#include "fs_privatedir.h"
#include "fs_diag_access.h"
#include "fs_public.h"

/* List of directories that we will prevent diag-users from accessing */
#define FS_PRIVATE_DIR_COUNT      2
static const char *fs_privatedir_list[FS_PRIVATE_DIR_COUNT];

/* This variable if set to 1 will allow diag to access the EFS private dir */
int efs_privatedir_allow_access = 0;


/* This is the callback function that diag will call when diag-users try to
   access our '/.efs_private' folder. We shall return FALSE to prevent-
   access */
static boolean
efs_privatedir_check_diag_access_cb (char *path, uint32 op_type)
{
  (void) path;
  (void) op_type;

  if (efs_privatedir_allow_access) {
    return TRUE;
  }

  /* return FALSE to prevent access */
  return FALSE;
}

void
efs_privatedir_init (void)
{
  /* Remove any file by this name and then try to create this directory */
  (void) efs_unlink (FS_EFS_PRIVATE_DIR_PATH);
  (void) efs_mkdir (FS_EFS_PRIVATE_DIR_PATH, 0755);

  /* Blow away if there are any contents in the iter directory. */
  (void) efs_deltree (FS_EFS_PRIVATE_DIR_ITER_PATH);

  /* Remove any file by this name and then try to create this directory */
  (void) efs_unlink (FS_EFS_DIAG_SAFE_DIR_PATH);
  (void) efs_mkdir (FS_EFS_DIAG_SAFE_DIR_PATH, 0755);

  /* Register a callback with diag for any attempt to access to our
     '/.efs_private and /safe' folder */
  fs_privatedir_list[0] = FS_EFS_PRIVATE_DIR_PATH;
  fs_privatedir_list[1] = FS_EFS_DIAG_SAFE_DIR_PATH;
  diag_fs_register_access_check (fs_privatedir_list, FS_PRIVATE_DIR_COUNT,
      efs_privatedir_check_diag_access_cb);
}

void
efs_privatedir_set_access (int allow_access)
{
  efs_privatedir_allow_access = allow_access;
}

