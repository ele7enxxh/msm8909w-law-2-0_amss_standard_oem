/***********************************************************************
 * fs_rmts_guid.c
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

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_rmts_guid.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-03-31   dks   Support Dual-OS feature.
2013-05-24   wek   Make GUID constant.
2013-01-17   dks   Featurize compilation of RMTS files.
2012-12-18   dks   Update golden copy strategy for new rmts layout.
2012-10-03   nr    Create

===========================================================================*/

#include "fs_config_i.h"

#ifdef FEATURE_EFS_EFS2_ON_RMTS

#include "fs_rmts_guid.h"
#include "assert.h"
#include <string.h>

/*****************************************************************************
 * Standalone modem GUID list.
 ****************************************************************************/
/* GPT GUID type for standalone modem FS1.
 * {EBBEADAF-22C9-E33B-8F5D-0E81686A68CB} */
const struct fs_guid_type fs_rmts_fs1_guid =
{0xEBBEADAF, 0x22C9, 0xE33B, {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}};

/* GPT GUID type for standalone modem FS2.
 * {0A288B1F-22C9-E33B-8F5D-0E81686A68CB} */
const struct fs_guid_type fs_rmts_fs2_guid =
{0x0A288B1F, 0x22C9, 0xE33B, {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}};

/* GPT GUID type for standalone modem FS Golden Copy.
 * {638FF8E2-22C9-E33B-8F5D-0E81686A68CB} */
const struct fs_guid_type fs_rmts_fsg_guid =
{0x638FF8E2, 0x22C9, 0xE33B, {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}};

/* GPT GUID type for standalone modem FS Cookie.
 * {57B90A16-22C9-E33B-8F5D-0E81686A68CB} */
const struct fs_guid_type fs_rmts_fsc_guid =
{0x57B90A16, 0x22C9, 0xE33B, {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}};


/*****************************************************************************
 * Fusion modem GUID list.
 ****************************************************************************/

/* GPT GUID type for Fusion Modem FS1.
 * {2290BE64-22C9-E33B-8F5D-0E81686A68CB} */
const struct fs_guid_type fs_rmts_fs1_fusion_guid =
{0x2290BE64, 0x22C9, 0xE33B, {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}};

/* GPT GUID type for Fusion Modem FS2.
 * {346C26D1-22C9-E33B-8F5D-0E81686A68CB} */
const struct fs_guid_type fs_rmts_fs2_fusion_guid =
{0x346C26D1, 0x22C9, 0xE33B, {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}};

/* GPT GUID type for Fusion Modem FS Golden Copy.
 * {BF64FB9C-22C9-E33B-8F5D-0E81686A68CB} */
const struct fs_guid_type fs_rmts_fsg_fusion_guid =
{0xBF64FB9C, 0x22C9, 0xE33B, {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}};

/* GPT GUID type for Fusion Modem FS Cookie.
 * {5CB43A64-22C9-E33B-8F5D-0E81686A68CB} */
const struct fs_guid_type fs_rmts_fsc_fusion_guid =
{0x5CB43A64, 0x22C9, 0xE33B, {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}};


/*****************************************************************************
 * Structure array to hold GUID mapping.
 ****************************************************************************/

/* Set the array size so that we can catch any extra id's or if we miss any */
const struct fs_rmts_guid_map
       fs_rmts_guid_map[FS_RMTS_MODEM_TYPE_MAX][FS_RMTS_PARTITION_ID_MAX] =
{
  { /* Standalone GUID List */
    {
      FS_RMTS_MODEM_TYPE_STANDALONE,
      FS_RMTS_PARTITION_ID_FS1,
      &fs_rmts_fs1_guid,
      FS_RMTS_MBR_PARTITION_ID_FS1_STANDLONE,
    },
    {
      FS_RMTS_MODEM_TYPE_STANDALONE,
      FS_RMTS_PARTITION_ID_FS2,
      &fs_rmts_fs2_guid,
      FS_RMTS_MBR_PARTITION_ID_FS2_STANDLONE,
    },
    {
      FS_RMTS_MODEM_TYPE_STANDALONE,
      FS_RMTS_PARTITION_ID_FSG,
      &fs_rmts_fsg_guid,
      FS_RMTS_MBR_PARTITION_ID_FSG_STANDLONE,
    },
    {
      FS_RMTS_MODEM_TYPE_STANDALONE,
      FS_RMTS_PARTITION_ID_FSC,
      &fs_rmts_fsc_guid,
      FS_RMTS_MBR_PARTITION_ID_FSC_STANDLONE,
    },
  },

  { /* Fusion GUID List */
    {
      FS_RMTS_MODEM_TYPE_FUSION,
      FS_RMTS_PARTITION_ID_FS1,
      &fs_rmts_fs1_fusion_guid,
      FS_RMTS_MBR_PARTITION_ID_FS1_FUSION,
    },
    {
      FS_RMTS_MODEM_TYPE_FUSION,
      FS_RMTS_PARTITION_ID_FS2,
      &fs_rmts_fs2_fusion_guid,
      FS_RMTS_MBR_PARTITION_ID_FS2_FUSION,
    },
    {
      FS_RMTS_MODEM_TYPE_FUSION,
      FS_RMTS_PARTITION_ID_FSG,
      &fs_rmts_fsg_fusion_guid,
      FS_RMTS_MBR_PARTITION_ID_FSG_FUSION,
    },
    {
      FS_RMTS_MODEM_TYPE_FUSION,
      FS_RMTS_PARTITION_ID_FSC,
      &fs_rmts_fsc_fusion_guid,
      FS_RMTS_MBR_PARTITION_ID_FSC_FUSION,
    },
  }
};


/*****************************************************************************/

const struct fs_guid_type*
fs_rmts_guid_get_partition_guid (enum fs_rmts_partition_id partition_id,
                                 enum fs_rmts_modem_type modem_type)
{
  const enum fs_rmts_partition_id *entry_parti_id;
  const enum fs_rmts_modem_type *entry_modem_type;

  ASSERT (modem_type < FS_RMTS_MODEM_TYPE_MAX);
  ASSERT (partition_id < FS_RMTS_PARTITION_ID_MAX);

  entry_modem_type = &(fs_rmts_guid_map[modem_type][partition_id].modem_type);
  entry_parti_id = &(fs_rmts_guid_map[modem_type][partition_id].partition_id);

  ASSERT (*entry_modem_type == modem_type);
  ASSERT (*entry_parti_id == partition_id);

  return fs_rmts_guid_map[modem_type][partition_id].guid;
}

#endif /* FEATURE_EFS_EFS2_ON_RMTS */
