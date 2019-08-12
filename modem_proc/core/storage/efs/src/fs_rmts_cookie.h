/***********************************************************************
 * fs_rmts_cookie.h
 *
 * Header for rmts cookie related data.
 * Copyright (C) 2013 QUALCOMM Technologies, Inc.
 *
 * Structures and APIs for rmts cookie related data.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_rmts_cookie.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-01-17   dks   Featurize compilation of RMTS files.
2012-12-18   dks   Create

===========================================================================*/

#ifndef __FS_RMTS_COOKIE_H__
#define __FS_RMTS_COOKIE_H__

#include "fs_config_i.h"

#ifdef FEATURE_EFS_EFS2_ON_RMTS

/* Backup cookie payload structure */
PACKED struct fs_backup_cookie_payload
{
  uint8 modem_type;         /* One of the values of enum fs_rmts_modem_type
                               specifying Standlone or Fusion. */
  uint8 src_partition;      /* One of the values of enum fs_rmts_partition_id
                               specifying which partition is the source */
  uint8 dest_partition;     /* One of the values of enum fs_rmts_partition_id
                               specifying which partition is the source */
  uint8 second_src_partition; /* One of the values of enum fs_rmts_partition_id
                                 specifying the second partition that has the
                                 previously synced EFS copy. */
  uint8 corrupt_src_partitions; /* Specifiy whether source partitions need to
                                   be corrupted or not. */
}PACKED_POST;

#endif /* FEATURE_EFS_EFS2_ON_RMTS */

#endif /* not __FS_RMTS_COOKIE_H__ */
