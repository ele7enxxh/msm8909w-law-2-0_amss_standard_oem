/***********************************************************************
 * fs_privatedir.h
 *
 * This module reserves a folder only to be available for EFS-Module. Other
 * EFS-Module's could create  their own-specific directories inside this
 * folder.
 *
 * Copyright (C) 2006-2007,2010,2012 QUALCOMM Technologies, Inc.
 *
 * EFS-Module will reserve a particular folder for its internal storage, and
 * will prevent diag-users from accessing this folder.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_privatedir.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-11-30    rp   Add the /safe directory protected from Diag-access.
2010-05-06   wek   Implement golden copy for remote storage page manager.
2007-11-16    rp   Added a temp-directory in our private folder.
2006-11-02    rp   Create

===========================================================================*/

#ifndef __FS_PRIVATEDIR_H__
#define __FS_PRIVATEDIR_H__

/* This is EFS module's private folder, EFS's module's meta-data is kept here*/
#define FS_EFS_PRIVATE_DIR_PATH "/.efs_private"

/* For remote storage page manager we support making a golden copy of
 * the file system. This golden copy can be created only once using diag.
 * This item file will indicate when the golden copy has been created. */
#define MADE_GOLDEN_COPY_PATH  FS_EFS_PRIVATE_DIR_PATH"/made_golden_copy"

/* This is our private directory to store symlinks when we iterate through
 * all the items of a given directory in the efs_iter(), This directory will
 * get deleted everytime upon boot. */
#define FS_EFS_PRIVATE_DIR_ITER_PATH FS_EFS_PRIVATE_DIR_PATH"/iter"

/* This /safe directory is protected from Diag-Access by the EFS-module itself.
 * EFS-Clients can safely store their files under this directory and will
 * get the default protection from Diag-access by the EFS-module itself. */
#define FS_EFS_DIAG_SAFE_DIR_PATH     "/safe"

/*=============================================================================
  FUNCTION      efs_privatedir_init

  DESCRIPTION   Register with Diag to prevent access to our special
                '/.efs_private/ folder by Diag users.

  PARAMETERS    None

  RETURN VALUE  None
=============================================================================*/
void efs_privatedir_init (void);


/*=============================================================================
  FUNCTION      efs_privatedir_set_access

  DESCRIPTION   This function will allow/restrict diag users from accessing the
                EFS's /.efs_private folder

  PARAMETERS
    allow_access -> [IN]
                     0 : means restrict access; 1(!=0) : means allow access.

  RETURN VALUE  None
=============================================================================*/
void efs_privatedir_set_access (int allow_access);

#endif /* not __FS_PRIVATEDIR_H__ */
