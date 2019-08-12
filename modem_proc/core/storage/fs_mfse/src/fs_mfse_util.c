/***********************************************************************
 * fs_mfse_util.c
 *
 * APIs for utility functions use by mfse module.
 * Copyright (C) 2013 Qualcomm Technologies, Inc.
 *
 * Implements utility functions that the mfse service uses.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fs_mfse/src/fs_mfse_util.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-04-03   dks   Create

===========================================================================*/

#include "fs_config_i.h"

#ifdef FEATURE_EFS_MFSE

#include "fs_mfse_util.h"
#include "fs_errno.h"


const char *
fs_mfse_util_get_fs_path_for_id (fs_mfse_filesystem_id fs_id)
{
  switch (fs_id)
  {
    case FS_MFSE_EFS2:
      return "/";
    default:
      return NULL;
  }
}

/* Convert efs_errno value to MFSE IDL defined errno value */
fs_mfse_errno_type
fs_mfse_util_get_mfse_errno (int efs_err)
{
  switch (efs_err)
  {
    case ENOERR:
      return FS_MFSE_ENOERR;
    case EPERM:
      return FS_MFSE_EPERM;
    case ENOENT:
      return FS_MFSE_ENOENT;
    case EEXIST:
      return FS_MFSE_EEXIST;
    case EBADF:
      return FS_MFSE_EBADF;
    case ENOMEM:
      return FS_MFSE_ENOMEM;
    case EACCES:
      return FS_MFSE_EACCES;
    case EBUSY:
      return FS_MFSE_EBUSY;
    case EXDEV:
      return FS_MFSE_EXDEV;
    case ENODEV:
      return FS_MFSE_ENODEV;
    case ENOTDIR:
      return FS_MFSE_ENOTDIR;
    case EISDIR:
      return FS_MFSE_EISDIR;
    case EINVAL:
      return FS_MFSE_EINVAL;
    case EMFILE:
      return FS_MFSE_EMFILE;
    case ETXTBSY:
      return FS_MFSE_ETXTBSY;
    case ENOSPC:
      return FS_MFSE_ENOSPC;
    case ESPIPE:
      return FS_MFSE_ESPIPE;
    case ENAMETOOLONG:
      return FS_MFSE_ENAMETOOLONG;
    case ENOTEMPTY:
      return FS_MFSE_ENOTEMPTY;
    case ELOOP:
      return FS_MFSE_ELOOP;
    case EILSEQ:
      return FS_MFSE_EILSEQ;
    case ESTALE:
      return FS_MFSE_ESTALE;
    case EDQUOT:
      return FS_MFSE_EDQUOT;
    case ENOCARD:
      return FS_MFSE_ENOCARD;
    case EBADFMT:
      return FS_MFSE_EBADFMT;
    case ENOTITM:
      return FS_MFSE_ENOTITM;
    case EROLLBACK:
      return FS_MFSE_EROLLBACK;
    case FS_ERANGE:
      return FS_MFSE_FS_ERANGE;
    case EEOF:
      return FS_MFSE_EEOF;
    case EUNKNOWN_SFAT:
      return FS_MFSE_EUNKNOWN_SFAT;
    case EUNKNOWN_HFAT:
      return FS_MFSE_EUNKNOWN_HFAT;
    case ENOTHINGTOSYNC:
      return FS_MFSE_ENOTHINGTOSYNC;
    default:
      return FS_MFSE_ERRNO_TYPE_MAX_ENUM_VAL;
  }
}

fs_mfse_sync_status
fs_mfse_util_get_mfse_sync_status (enum fs_sync_result sync_status)
{
  switch (sync_status)
  {
    case FS_SYNC_PENDING:
      return FS_MFSE_SYNC_PENDING;
    case FS_SYNC_COMPLETE:
      return FS_MFSE_SYNC_COMPLETE;
    default:
      return FS_MFSE_SYNC_STATUS_MAX_ENUM_VAL;
  }
}

#endif /* FEATURE_EFS_MFSE */

