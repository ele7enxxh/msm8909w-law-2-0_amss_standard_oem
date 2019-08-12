/***********************************************************************
 * fs_rmts_cookie.c
 *
 * Handler functions for all remote storage cookies.
 * Copyright (C) 2010-2014 QUALCOMM Technologies, Inc.
 *
 * Implements all remote storage cookie handler functions and any helper
 * functions needed to read and write to the eMMC card using Hotplug functions.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_rmts_cookie.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-05-12   vm    Support enable/disable multiple Golden Copy creations.
2014-03-31   dks   Support Dual-OS feature.
2013-05-24   wek   Make GUID constant.
2013-02-05   nr    Remove hard requirement for cookie partitions to be present.
2013-02-01   nr    Increase probability of correctly detecting golden copy.
2013-01-17   dks   Fix image source offset calculation for rmts image header.
2013-01-17   dks   Featurize compilation of RMTS files.
2013-01-11   dks   Move writing to Golden partition outside of efs_sync.
2012-12-18   dks   Update golden copy strategy for new rmts layout.
2012-11-09   nr    Remove simulator specific code for cookie handling.
2012-10-18   nr    Fix bug in fs_rmts_cookie_is_golden_copy_present.
2012-10-09   dks   Remove err-fatals from boot while handling backup cookies.
2012-08-24   dks   Fix Klockwork warnings.
2012-08-20   wek   Fix check of buffer size while handling cookies.
2012-08-09   dks   Rename fs_cookie.h to fs_cookie_i.h.
2012-06-12   dks   Cleanup IMEM cookies and cookie APIs.
2012-05-23   dks   Cleanup code to remove Halcyon behavior.
2012-04-09   dks   Support storing/reading cookies to/from emmc partitions.
2011-05-13   wek   Support GPT partitioned disks.
2011-03-25   dks   Support Golden-Copy in RAM for fusion.
2010-11-19   rp    Added support to read FSG from modem if possible.
2010-11-18   dks   Including fs_err.h
2010-11-17   rp    Featurize this file for RMTS only.
2010-11-17   rp    Avoid reset to try FSG on first ever boot.
2010-11-15   rp    Create

===========================================================================*/

#include "fs_config_i.h"

#ifdef FEATURE_EFS_EFS2_ON_RMTS

#include "fs_cookie.h"
#include "fs_cookie_i.h"
#include "fs_rmts_config.h"
#include "fs_rmts_guid.h"
#include "fs_hotplug.h"
#include "fs_err.h"
#include "fs_rmts_cookie.h"
#include "fs_rmts_image_header.h"
#include "fs_rmts_cookie_golden_copy_handler.h"
#include "assert.h"
#include <string.h>


#define FS_RMTS_MBR_PARTITION_ID_INVALID            0x0

/*---------------------------------------------------------------------------
  MBR IDs for the 2nd-MDM. When 2 MDM's are connected to the same APQ.
---------------------------------------------------------------------------*/
#define FS_RMTS_MBR_PARTITION_ID_FS1_FUSION2        0x6C
#define FS_RMTS_MBR_PARTITION_ID_FS2_FUSION2        0x6D
#define FS_RMTS_MBR_PARTITION_ID_FSG_FUSION2        0x6E
#define FS_RMTS_MBR_PARTITION_ID_FSC_FUSION2        0x6F


/*---------------------------------------------------------------------------
  GPT-GUIDs for the 2nd-MDM. When 2 MDM's are connected to the same APQ.
---------------------------------------------------------------------------*/
/* Second Fusion Modem FS1 = {291F742A-22C9-E33B-8F5D-0E81686A68CB} */
static struct fs_guid_type fs_rmts_fs1_fusion2_guid =
{0x291F742A, 0x22C9, 0xE33B, {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}};

/* Second Fusion Modem FS2 = {CC2603E1-22C9-E33B-8F5D-0E81686A68CB} */
static struct fs_guid_type fs_rmts_fs2_fusion2_guid =
{0xCC2603E1, 0x22C9, 0xE33B, {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}};

/* Second Fusion Modem FSG =  {E3EEF434-22C9-E33B-8F5D-0E81686A68CB} */
static struct fs_guid_type fs_rmts_fsg_fusion2_guid =
{0xE3EEF434, 0x22C9, 0xE33B, {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}};

/* second Fusion Modem FSC = {EF506FD9-22C9-E33B-8F5D-0E81686A68CB} */
static struct fs_guid_type fs_rmts_fsc_fusion2_guid =
{0xEF506FD9,0x22C9,0xE33B,{0x8F,0x5D,0x0E,0x81,0x68,0x6A,0x68,0xCB}};


/*---------------------------------------------------------------------------
  MBR-IDs for the LA when we have Dual-OS (LA + WIN) on same eMMC/UFS.
---------------------------------------------------------------------------*/
#define FS_RMTS_MBR_PARTITION_ID_FS1_LA             0x7A
#define FS_RMTS_MBR_PARTITION_ID_FS2_LA             0x7B
#define FS_RMTS_MBR_PARTITION_ID_FSG_LA             0x7C
#define FS_RMTS_MBR_PARTITION_ID_FSC_LA             0x7D


/*---------------------------------------------------------------------------
  GPT-GUIDs for the LA when we have Dual-OS (LA + WIN) on same eMMC/UFS.
---------------------------------------------------------------------------*/
/* Dual-OS LA Modem FS1 = {73F41660-22C9-E33B-8F5D-0E81686A68CB} */
static struct fs_guid_type fs_rmts_fs1_la_guid =
{0x73F41660, 0x22C9, 0xE33B, {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}};

/* Dual-OS LA Modem FS2 = {B02DB6B9-22C9-E33B-8F5D-0E81686A68CB} */
static struct fs_guid_type fs_rmts_fs2_la_guid =
{0xB02DB6B9, 0x22C9, 0xE33B, {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}};

/* Dual-OS LA Modem FSG =  {CE1953DB-22C9-E33B-8F5D-0E81686A68CB} */
static struct fs_guid_type fs_rmts_fsg_la_guid =
{0xCE1953DB, 0x22C9, 0xE33B, {0x8F, 0x5D, 0x0E, 0x81, 0x68, 0x6A, 0x68, 0xCB}};

/* Dual-OS LA Modem FSC = {32D7536E-22C9-E33B-8F5D-0E81686A68CB} */
static struct fs_guid_type fs_rmts_fsc_la_guid =
{0x32D7536E,0x22C9,0xE33B,{0x8F,0x5D,0x0E,0x81,0x68,0x6A,0x68,0xCB}};



/* Buffer used to copy data from one partition to the other. */
static uint8 fs_rmts_cookie_copy_buffer[2048];

/* Buffer used to store cookie partition data. */
static uint8 fs_rmts_cookie_buffer[512];


/* Array of cookie partitions that needs to be read to handle cookies for
   all available processors' file systems. */
static const uint32 fs_cookie_parti_ids[] =
{
  FS_RMTS_MBR_PARTITION_ID_FSC_STANDLONE,
  FS_RMTS_MBR_PARTITION_ID_FSC_FUSION,
  FS_RMTS_MBR_PARTITION_ID_FSC_FUSION2,
  FS_RMTS_MBR_PARTITION_ID_FSC_LA,
};


/*---------------------------------------------------------------------------
  MAP to lookup EFS-partitions for a given Cookie-partition
---------------------------------------------------------------------------*/
struct fs_table_cookie_mbr_to_efs_mbr_map
{
  uint32 fsc_mbr_id; /* Using this Cookie-MBR-ID, Lookup below FS MBR-IDs */
  uint32 fs1_mbr_id;
  uint32 fs2_mbr_id;
  uint32 fsg_mbr_id;
  enum fs_rmts_modem_type modem_type; /* Used for validating cookie-payload*/
};

static const struct fs_table_cookie_mbr_to_efs_mbr_map cookie_to_efs_map[] =
{
  {
    FS_RMTS_MBR_PARTITION_ID_FSC_STANDLONE,
    FS_RMTS_MBR_PARTITION_ID_FS1_STANDLONE,
    FS_RMTS_MBR_PARTITION_ID_FS2_STANDLONE,
    FS_RMTS_MBR_PARTITION_ID_FSG_STANDLONE,
    FS_RMTS_MODEM_TYPE_STANDALONE
  },

  {
    FS_RMTS_MBR_PARTITION_ID_FSC_FUSION,
    FS_RMTS_MBR_PARTITION_ID_FS1_FUSION,
    FS_RMTS_MBR_PARTITION_ID_FS2_FUSION,
    FS_RMTS_MBR_PARTITION_ID_FSG_FUSION,
    FS_RMTS_MODEM_TYPE_FUSION
  },

  {
    FS_RMTS_MBR_PARTITION_ID_FSC_FUSION2,
    FS_RMTS_MBR_PARTITION_ID_FS1_FUSION2,
    FS_RMTS_MBR_PARTITION_ID_FS2_FUSION2,
    FS_RMTS_MBR_PARTITION_ID_FSG_FUSION2,
    FS_RMTS_MODEM_TYPE_FUSION
  },

  {
    FS_RMTS_MBR_PARTITION_ID_FSC_LA,
    FS_RMTS_MBR_PARTITION_ID_FS1_LA,
    FS_RMTS_MBR_PARTITION_ID_FS2_LA,
    FS_RMTS_MBR_PARTITION_ID_FSG_LA,
    FS_RMTS_MODEM_TYPE_STANDALONE
  },
};

/* Get EFS-MBR-id for the given cookie-MBR-id */
static uint32 fs_rmts_guid_get_mbr_partition_id
(
  uint32 fsc_mbr_id,
  enum fs_rmts_partition_id partition_id,
  enum fs_rmts_modem_type modem_type
)
{
  uint32 i, num_cookie_partis, mbr_id;
  const struct fs_table_cookie_mbr_to_efs_mbr_map *slot;

  mbr_id = FS_RMTS_MBR_PARTITION_ID_INVALID;

  num_cookie_partis = sizeof (cookie_to_efs_map) /
                      sizeof (cookie_to_efs_map[0]);

  for ( i = 0; i < num_cookie_partis; ++i )
  {
    slot = &cookie_to_efs_map[i];

    if (slot->fsc_mbr_id == fsc_mbr_id)
    {
      /* Sanity-check to ensure MPSS and SBL-cookie-logic are in sync. */
      ASSERT (modem_type == slot->modem_type);
      switch (partition_id)
      {
        case FS_RMTS_PARTITION_ID_FS1:
          mbr_id = slot->fs1_mbr_id;
          break;

        case FS_RMTS_PARTITION_ID_FS2:
          mbr_id = slot->fs2_mbr_id;
          break;

        case FS_RMTS_PARTITION_ID_FSG:
          mbr_id = slot->fsg_mbr_id;
          break;

        default:
          /* Looking up for unknown partition */
          ASSERT (0);
          break;

      }
      break;
    }
  }

  return mbr_id;
}





/*---------------------------------------------------------------------------
  MAP to lookup GPT-GUID for a given MBR-ID.
---------------------------------------------------------------------------*/
struct fs_table_mbr_gpt_types
{
  uint32 mbr_type;
  const struct fs_guid_type *gpt_type;
};

static const struct fs_table_mbr_gpt_types fs_mbr_gpt_map[] =
{
  /* Stand-alone modem : MBR and GPT GUIDs */
  {FS_RMTS_MBR_PARTITION_ID_FS1_STANDLONE,    &fs_rmts_fs1_guid},
  {FS_RMTS_MBR_PARTITION_ID_FS2_STANDLONE,    &fs_rmts_fs2_guid},
  {FS_RMTS_MBR_PARTITION_ID_FSG_STANDLONE,    &fs_rmts_fsg_guid},
  {FS_RMTS_MBR_PARTITION_ID_FSC_STANDLONE,    &fs_rmts_fsc_guid},

  /* Fusion modem : MBR and GPT GUIDs */
  {FS_RMTS_MBR_PARTITION_ID_FS1_FUSION,       &fs_rmts_fs1_fusion_guid},
  {FS_RMTS_MBR_PARTITION_ID_FS2_FUSION,       &fs_rmts_fs2_fusion_guid},
  {FS_RMTS_MBR_PARTITION_ID_FSG_FUSION,       &fs_rmts_fsg_fusion_guid},
  {FS_RMTS_MBR_PARTITION_ID_FSC_FUSION,       &fs_rmts_fsc_fusion_guid},

  /* 2nd-Fusion modem, when 2 MDS are connected to APQ : MBR and GPT GUIDs */
  {FS_RMTS_MBR_PARTITION_ID_FS1_FUSION2,       &fs_rmts_fs1_fusion2_guid},
  {FS_RMTS_MBR_PARTITION_ID_FS2_FUSION2,       &fs_rmts_fs2_fusion2_guid},
  {FS_RMTS_MBR_PARTITION_ID_FSG_FUSION2,       &fs_rmts_fsg_fusion2_guid},
  {FS_RMTS_MBR_PARTITION_ID_FSC_FUSION2,       &fs_rmts_fsc_fusion2_guid},

  /* LA guids when we have dual-os (LA + Win) : MBR and GPT GUIDs */
  {FS_RMTS_MBR_PARTITION_ID_FS1_LA,            &fs_rmts_fs1_la_guid},
  {FS_RMTS_MBR_PARTITION_ID_FS2_LA,            &fs_rmts_fs2_la_guid},
  {FS_RMTS_MBR_PARTITION_ID_FSG_LA,            &fs_rmts_fsg_la_guid},
  {FS_RMTS_MBR_PARTITION_ID_FSC_LA,            &fs_rmts_fsc_la_guid},
 };


/* Signature of cookie handler functions. */
typedef void (*fs_cookie_handler) (uint32 fsc_mbr_id,struct fs_cookie *cookie);

struct fs_cookie_handler_table
{
  enum fs_cookie_id       cookie_id;
  fs_cookie_handler      cookie_handler;
};

/* BACKUP Cookie handler function */
static void fs_rmts_cookie_backup_handler (uint32 fsc_mbr_id,
                                           struct fs_cookie *cookie);

/* Cookie ID to handler mapping table */
static const struct fs_cookie_handler_table fs_cookie_handler_tbl[] =
{
  {EFS_COOKIE_ID_INVALID,       NULL},
  {EFS_COOKIE_ID_BACKUP,        fs_rmts_cookie_backup_handler},
  {EFS_COOKIE_ID_MAX,           NULL},
};

static const struct fs_guid_type*
fs_rmts_cookie_convert_mbr_to_gpt (uint32 mbr_id)
{
  uint32 i, nitems;

  /* Search for the corresponding GPT ID that corresponds to the MBR ID */
  nitems = sizeof (fs_mbr_gpt_map) / sizeof (struct fs_table_mbr_gpt_types);
  for (i = 0; i < nitems; i++)
  {
    if (fs_mbr_gpt_map[i].mbr_type == mbr_id)
    {
      return fs_mbr_gpt_map[i].gpt_type;
    }
  }

  return NULL;
}

static void
fs_rmts_cookie_copy_rmts_guid_to_hotplug_guid
(
  const struct fs_guid_type *rmts_guid,
  struct hotplug_guid *hotplug_guid_val
)
{
  uint32 i;

  ASSERT (rmts_guid != NULL);
  ASSERT (hotplug_guid_val != NULL);

  memset (hotplug_guid_val, 0, sizeof (struct hotplug_guid));

  hotplug_guid_val->data1 = rmts_guid->data1;
  hotplug_guid_val->data2 = rmts_guid->data2;
  hotplug_guid_val->data3 = rmts_guid->data3;

  for ( i = 0; i < sizeof (hotplug_guid_val->data4); ++i )
  {
    hotplug_guid_val->data4[i] = rmts_guid->data4[i];
  }
}

static struct hotplug_device*
fs_rmts_cookie_open_partition (uint32 mbr_id)
{
  struct hotplug_device *hdev = NULL;
  const struct fs_guid_type *rmts_guid;
  struct hotplug_guid guid_type;

  rmts_guid = fs_rmts_cookie_convert_mbr_to_gpt (mbr_id);
  if (rmts_guid != NULL)
  {
    fs_rmts_cookie_copy_rmts_guid_to_hotplug_guid (rmts_guid, &guid_type);
    hdev = hotplug_open_device_by_gpt_partition_type (HOTPLUG_TYPE_MMC,
                       HOTPLUG_ITER_EMBEDDED_DEVICES_ONLY, &guid_type);
  }

  if (hdev == NULL)
  {
    hdev = hotplug_open_device_by_partition_type (HOTPLUG_TYPE_MMC,
                       HOTPLUG_ITER_EMBEDDED_DEVICES_ONLY, mbr_id);
  }

  return hdev;
}

static int
fs_rmts_cookie_copy_partition_blocks (struct hotplug_device *src_hdev,
                                      struct hotplug_device *dest_hdev,
                                      uint32 src_start_lba,
                                      uint32 dest_start_lba,
                                      uint32 num_blocks, uint32 block_size)
{
  uint32 buff_blocks, blocks_to_copy, blocks_copied;
  int result;

  ASSERT (block_size > 0);
  buff_blocks = sizeof (fs_rmts_cookie_copy_buffer) / block_size;
  blocks_copied = 0;

  while (blocks_copied < num_blocks)
  {
    blocks_to_copy = num_blocks - blocks_copied;
    if (blocks_to_copy > buff_blocks)
      blocks_to_copy = buff_blocks;

    result = hotplug_dev_read (src_hdev, src_start_lba + blocks_copied,
                               fs_rmts_cookie_copy_buffer, blocks_to_copy);
    if (result != 0)
      return blocks_copied;

    result = hotplug_dev_write (dest_hdev, dest_start_lba + blocks_copied,
                                fs_rmts_cookie_copy_buffer, blocks_to_copy);
    if (result != 0)
      return blocks_copied;

    blocks_copied += blocks_to_copy;
  }

  return blocks_copied;
}


static int
fs_rmts_cookie_validate_backup_cookie_payload (
                                       enum fs_rmts_partition_id src_id,
                                       enum fs_rmts_partition_id dest_id,
                                       enum fs_rmts_partition_id second_src_id)
{
  int is_valid = 0;

  switch (src_id)
  {
    case FS_RMTS_PARTITION_ID_FS1:
    {
      if ((dest_id == FS_RMTS_PARTITION_ID_FSG)       &&
          (second_src_id == FS_RMTS_PARTITION_ID_FS2))
      {
        is_valid = 1;
      }
    }
    break;

    case FS_RMTS_PARTITION_ID_FS2:
    {
      if ((dest_id == FS_RMTS_PARTITION_ID_FSG)       &&
          (second_src_id == FS_RMTS_PARTITION_ID_FS1))
      {
        is_valid = 1;
      }
    }
    break;

    default:
    {
      is_valid = 0;
    }
    break;
  }

  return is_valid;
}


/* For Internal-Fusion, we want to corrupt FS1 and FS2 now, so that next boot
   uses FSG. This corruption of FS1 and FS2 is required for following reasons:
   1. Boot code need not create headers for FSG when trying to create Golden
      copy.
   2. efs_initialize forces checks to make sure the superdata fields match the
      partition that they are read from. By setting these fields to FSG for the
      golden copy sync, such checks can be enforced for FSG as well. This will
      help minimize crashes/issues that result from customers copying data from
      FSG to the others.Since the data is written to FS1/FS2, we need to
      corrupt these superblocks so efs_init does not err-fatal on the mismatch.
 */
static void
corrupt_efs_partition (struct hotplug_device *src_hdev, uint8 *buffer)
{
  int result;
  uint32 sector;

  sector = 0; /* Corrupt MBN header */
  result = hotplug_dev_write (src_hdev, sector, buffer, 1);
  if (result == 0)
  {
    sector = 1; /* Corrupt superblock */
    result = hotplug_dev_write (src_hdev, sector, buffer, 1);
  }
  if (result != 0)
  {
    FS_ERR_FATAL ("%d,%d,%d,Golden copy failed:write failed", (uint32)src_hdev,
                  sector, result);
  }
}

/* Checks validity of the cookie and verfies that golden copy has not already
   been created before requesting copying the source to destination. */
static void
fs_rmts_cookie_backup_handler (uint32 fsc_mbr_id,
                               struct fs_cookie *cookie)
{
  enum fs_rmts_modem_type modem_type;
  struct hotplug_device *src_hdev, *dest_hdev = NULL, *second_src_hdev = NULL;
  uint32 src_id, dest_id, second_src_id, cookie_copy_buf_size;
  uint32 src_blk_cnt, dest_blk_cnt, second_src_blk_cnt;
  uint16 src_blk_size, dest_blk_size, second_src_blk_size;
  struct fs_backup_cookie_payload *payload;
  int result, result1, result2;
  enum fs_rmts_partition_id src_parti, dest_parti, second_src_parti;

  ASSERT (cookie != NULL);
  ASSERT (cookie->cookie_id == EFS_COOKIE_ID_BACKUP);
  if (cookie->payload_size != sizeof (struct fs_backup_cookie_payload))
    return;

  payload = (struct fs_backup_cookie_payload *) (cookie->payload);
  modem_type = (enum fs_rmts_modem_type) payload->modem_type;
  src_parti = (enum fs_rmts_partition_id) payload->src_partition;
  dest_parti = (enum fs_rmts_partition_id) payload->dest_partition;
  second_src_parti = (enum fs_rmts_partition_id) payload->second_src_partition;

  /* Since cookie partition is not write protected, any data can be added to
     cause crashes. Hence only after we ensure that the partition IDs specified
     are valid combinations of partitions and the src can be opened, should we
     attempt to create golden copy. */
  if (fs_rmts_cookie_validate_backup_cookie_payload (src_parti, dest_parti,
                                                     second_src_parti) != 1)
  {
    return;
  }

  src_id = fs_rmts_guid_get_mbr_partition_id (fsc_mbr_id,
                                              src_parti, modem_type);
  dest_id = fs_rmts_guid_get_mbr_partition_id (fsc_mbr_id,
                                               dest_parti, modem_type);
  second_src_id = fs_rmts_guid_get_mbr_partition_id(fsc_mbr_id,
                                         second_src_parti, modem_type);

  /* We are not going to use this field for now. This is reserved for future
     use if we want to. */
  (void) payload->corrupt_src_partitions;

  /* We only allow for src_id to fail open. This is because so far we have only
     validated the partititon Ids in the cookie are a valid combination. But if
     the partitions don't exists, for eg. fusion IDs on standalone targets, the
     open will fail. So allow for src to fail. But if src opens, all others
     must follow suite to golden copy to be created successfully. Catch all
     errors right here for easy debugging. */

  src_hdev = fs_rmts_cookie_open_partition (src_id);
  if (src_hdev == NULL)
    goto exit;

  dest_hdev = fs_rmts_cookie_open_partition (dest_id);
  if (dest_hdev == NULL)
  {
    FS_ERR_FATAL ("%d,Golden copy: open failed, id", dest_id, 0, 0);
  }

  result1 = hotplug_dev_get_size (src_hdev, &src_blk_cnt, &src_blk_size);
  if (result1 != 0)
  {
    FS_ERR_FATAL ("%d,%d,Golden copy: dev_get_size failed", result1,
                  (uint32) src_hdev, 0);
  }

  result2 = hotplug_dev_get_size (dest_hdev, &dest_blk_cnt, &dest_blk_size);
  if (result2 != 0)
  {
    FS_ERR_FATAL ("%d,%d,Golden copy: dev_get_size failed", result2,
                  (uint32) dest_hdev, 0);
  }

  if (src_blk_cnt != dest_blk_cnt)
  {
    FS_ERR_FATAL ("%d,%d,Golden copy: src blk_cnt != dest blk cnt",
                  src_blk_cnt, dest_blk_cnt, 0);
  }

  if (src_blk_size!= dest_blk_size)
  {
    FS_ERR_FATAL ("%d,%d,Golden copy: src blk_size != dest blk size",
                  src_blk_size, dest_blk_size, 0);
  }

  second_src_hdev = fs_rmts_cookie_open_partition (second_src_id);
  if (second_src_hdev == NULL)
  {
    FS_ERR_FATAL ("%d,Golden copy: open failed, id, hdev", second_src_id, 0,0);
  }

  result = hotplug_dev_get_size (second_src_hdev, &second_src_blk_cnt,
                                 &second_src_blk_size);
  if (result != 0)
  {
    FS_ERR_FATAL ("%d,%d,Golden copy: dev_get_size failed", result,
                  (uint32) second_src_hdev, 0);
  }

  if (src_blk_cnt != second_src_blk_cnt)
  {
    FS_ERR_FATAL ("%d,%d,Golden copy: src blk_cnt != alt src blk cnt",
                  src_blk_cnt, second_src_blk_cnt, 0);
  }

  if (src_blk_size!= second_src_blk_size)
  {
    FS_ERR_FATAL ("%d,%d,Golden copy: src blk_size != alt src blk size",
                  src_blk_size, second_src_blk_size, 0);
  }

  /* Make sure we write to FSG only once by checking if a golden copy already
     exists. */

  /* Ensure data to be read can fit in copy buffer */
  cookie_copy_buf_size = sizeof (fs_rmts_cookie_copy_buffer);
  ASSERT (cookie_copy_buf_size >= src_blk_size);
  if (dest_blk_size > cookie_copy_buf_size)
  {
    FS_ERR_FATAL ("%d,%d,Copy buffer smaller than blk size", dest_blk_size,
                  cookie_copy_buf_size, 0);
  }


  if (fs_rmts_cookie_is_golden_copy_present (dest_hdev,
                                             fs_rmts_cookie_copy_buffer,
                                             cookie_copy_buf_size) != 0)
    goto exit;

  result = fs_rmts_cookie_copy_partition_blocks (src_hdev, dest_hdev, 0, 0,
                                                 src_blk_cnt, src_blk_size);
  if ((uint32) result != src_blk_cnt)
  {
    FS_ERR_FATAL ("%d,%d,Golden copying failed, requested, resulting",
                   src_blk_cnt, result, 0);
  }

  memset (fs_rmts_cookie_copy_buffer, 0,sizeof (fs_rmts_cookie_copy_buffer));
  corrupt_efs_partition (src_hdev, fs_rmts_cookie_copy_buffer);
  corrupt_efs_partition (second_src_hdev, fs_rmts_cookie_copy_buffer);

exit:
  (void) hotplug_close_device (src_hdev);
  (void) hotplug_close_device (second_src_hdev);
  (void) hotplug_close_device (dest_hdev);
}

/* Handles any cookie present in RAM. Returns TRUE if any cookie was handled
   and 0 otherwise. */
static int
fs_rmts_cookie_handle_cookies (uint32 fsc_mbr_id,
                               uint8 *cookie_buff, uint32 cookie_buff_size)
{
  int cookie_handle, cookie_present = 0;
  struct fs_cookie_iter *cookie_iter;
  struct fs_cookie* cookie_ptr;
  fs_cookie_handler cookie_handler_func;

  cookie_handle = fs_cookie_get_handle_for_ram (cookie_buff, cookie_buff_size);
  if (cookie_handle < 0)
  {
    FS_ERR_FATAL ("%d, %d, %d, Cookie init on RAM failed", cookie_handle,
                (uint32)fs_rmts_cookie_buffer, sizeof (fs_rmts_cookie_buffer));
  }

  cookie_iter = fs_cookie_iter_open (cookie_handle);
  if (cookie_iter == NULL)
  {
    FS_ERR_FATAL ("Cookie iter open failed", 0, 0, 0);
  }

  cookie_ptr = fs_cookie_iter_get_next (cookie_iter);
  if (cookie_ptr != NULL)
    cookie_present = 1;

  while (cookie_ptr != NULL)
  {
    ASSERT (fs_cookie_handler_tbl[cookie_ptr->cookie_id].cookie_id !=
            EFS_COOKIE_ID_INVALID);
    cookie_handler_func =
                  fs_cookie_handler_tbl[cookie_ptr->cookie_id].cookie_handler;
    ASSERT (cookie_handler_func != NULL);
    cookie_handler_func (fsc_mbr_id, cookie_ptr);
    cookie_ptr = fs_cookie_iter_get_next (cookie_iter);
  } /* no more cookies to handle */

  ASSERT (fs_cookie_iter_close (cookie_iter) == 0);

  if (cookie_present)
    fs_cookie_remove_all_cookies (cookie_handle);

  ASSERT (fs_cookie_release_handle (cookie_handle) == 0);

  return cookie_present;
}


void
efs_handle_cookies (void)
{
  uint32 i, num_cookie_partis, fsc_mbr_id;
  int result, cookie_handled, cookie_partis_present = 0;
  struct hotplug_device *hdev = NULL;
  uint16 block_size = 0;
  uint32 block_count = 0;
  uint32 cookie_buf_size = sizeof (fs_rmts_cookie_buffer);

  /* Initialize the cookie module. */
  fs_cookie_init ();

  num_cookie_partis = sizeof (fs_cookie_parti_ids) /
                      sizeof (fs_cookie_parti_ids[0]);

  /* Try  to see if any cookie partitions are present. If they are then we
   * handle the cookies ,if any, in those partitions. If no cookie partitions
   * are found we assume no modem image is present and exit gracefully.
   */
  for (i = 0; i < num_cookie_partis; i++)
  {
    fsc_mbr_id = fs_cookie_parti_ids[i];
    hdev = fs_rmts_cookie_open_partition (fsc_mbr_id);
    if (hdev == NULL)
      continue;

    if (hotplug_dev_get_size (hdev, &block_count, &block_size) != 0)
      goto exit;  /* Hotplug failed to get size, move on to next FS */

    ASSERT (cookie_buf_size >= block_size);
    result = hotplug_dev_read (hdev, 0, fs_rmts_cookie_buffer, 1);
    if (result != 0)
      goto exit;

    cookie_partis_present++;

    cookie_handled = fs_rmts_cookie_handle_cookies (fsc_mbr_id,
                        fs_rmts_cookie_buffer, cookie_buf_size);
    if (cookie_handled)
      (void) hotplug_dev_write (hdev, 0, fs_rmts_cookie_buffer, 1);

  exit:
    (void) hotplug_close_device (hdev);
  }/* No more cookie partis to read. If we have found any cookie partitions
      we have already handled them. */
}

#endif /* FEATURE_EFS_EFS2_ON_RMTS */
