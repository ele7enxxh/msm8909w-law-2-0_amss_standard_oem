/***********************************************************************
 * fs_rmts_guid.h
 *
 * The rmts guid module.
 * Copyright (C) 2012-2014 QUALCOMM Technologies, Inc.
 *
 * This module implements the mapping table and functions that are required
 * to get the partition GUID (and MBR) for a given partition and modem type.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_rmts_guid.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-03-31   dks   Support Dual-OS feature.
2013-05-24   wek   Make GUID constant.
2013-01-17   dks   Featurize compilation of RMTS files.
2012-10-03   nr    Create

===========================================================================*/

#ifndef __FS_RMTS_GUID_H__
#define __FS_RMTS_GUID_H__

#include "fs_config_i.h"

#ifdef FEATURE_EFS_EFS2_ON_RMTS

#include "fs_guid_type.h"
#include "fs_rmts.h"

/*****************************************************************************
 * List of MBR partition ids for MSM standalone.
 ****************************************************************************/
#define FS_RMTS_MBR_PARTITION_ID_FS1_STANDLONE     0x4A
#define FS_RMTS_MBR_PARTITION_ID_FS2_STANDLONE     0x4B
#define FS_RMTS_MBR_PARTITION_ID_FSG_STANDLONE     0x58
#define FS_RMTS_MBR_PARTITION_ID_FSC_STANDLONE     0x5E

/*****************************************************************************
 * List of MBR partition ids for fusion mdm.
 ****************************************************************************/
#define FS_RMTS_MBR_PARTITION_ID_FS1_FUSION        0x59
#define FS_RMTS_MBR_PARTITION_ID_FS2_FUSION        0x5A
#define FS_RMTS_MBR_PARTITION_ID_FSG_FUSION        0x5B
#define FS_RMTS_MBR_PARTITION_ID_FSC_FUSION        0x5C

/*****************************************************************************
 * List of GPT partition ids for MSM standalone.
 ****************************************************************************/
extern const struct fs_guid_type fs_rmts_fs1_guid;
extern const struct fs_guid_type fs_rmts_fs2_guid;
extern const struct fs_guid_type fs_rmts_fsg_guid;
extern const struct fs_guid_type fs_rmts_fsc_guid;

/*****************************************************************************
 * List of GPT partition ids for fusion mdm.
 ****************************************************************************/
extern const struct fs_guid_type fs_rmts_fs1_fusion_guid;
extern const struct fs_guid_type fs_rmts_fs2_fusion_guid;
extern const struct fs_guid_type fs_rmts_fsg_fusion_guid;
extern const struct fs_guid_type fs_rmts_fsc_fusion_guid;


/*****************************************************************************
 * Structure array to hold GUID mapping.
 ****************************************************************************/
struct fs_rmts_guid_map
{
  enum fs_rmts_modem_type modem_type;
  enum fs_rmts_partition_id partition_id;
  const struct fs_guid_type *guid;
  uint8 mbr_partition_id;
};

/*****************************************************************************
 * API's to get the correct GUID.
 ****************************************************************************/

/* Get correct Partition GUID */
const struct fs_guid_type* fs_rmts_guid_get_partition_guid
(
  enum fs_rmts_partition_id partition_id,
  enum fs_rmts_modem_type modem_type
);

#endif /* FEATURE_EFS_EFS2_ON_RMTS */

#endif /* not __FS_RMTS_GUID_H__ */

