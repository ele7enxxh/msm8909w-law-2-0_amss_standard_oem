/***********************************************************************
 * fs_mfse_util.h
 *
 * APIs for utility functions use by mfse module.
 * Copyright (C) 2013 Qualcomm Technologies, Inc.
 *
 * Provides APIs for utility functions that the mfse service uses.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fs_mfse/src/fs_mfse_util.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-04-03   dks   Create

===========================================================================*/

#ifndef __FS_QMI_UTIL_H__
#define __FS_QMI_UTIL_H__

#include "fs_config_i.h"

#ifdef FEATURE_EFS_MFSE

#include "comdef.h"
#include "fs_mfse_std_interface.h"
#include "fs_sys_types.h"

/* Map the MFSE Filesystem ID to a path string */
const char *fs_mfse_util_get_fs_path_for_id (fs_mfse_filesystem_id fs_id);

/* Convert efs_errno value to MFSE IDL defined errno value */
fs_mfse_errno_type fs_mfse_util_get_mfse_errno (int efs_err);

fs_mfse_sync_status fs_mfse_util_get_mfse_sync_status
                                           (
                                             enum fs_sync_result sync_status
                                           );

#endif /* FEATURE_EFS_MFSE */

#endif /* not __FS_QMI_UTIL_H__ */
