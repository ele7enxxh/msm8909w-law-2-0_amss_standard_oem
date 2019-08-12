/***********************************************************************
 * fs_rmts_super.c
 *
 * Remote Storage Superblock module.
 * Copyright (C) 2009-2010, 2012-2013 QUALCOMM Technologies, Inc.
 *
 * The remote storage page manager uses superblock to start interpreting the
 * rest of the partition data. This is the first region it decrypts to learn
 * about the layout of the filesystem. This module implements logic to create
 * and manage superblock data.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_rmts_super.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-07-22   rp    Add new EFS API to store data in SuperBlock.
2012-11-09   nr    Update rmts to use new api's.
2012-10-19   dks   Make superblock age run to uint32_max.
2012-09-20   dks   Update rmts layout to support write-cache.
2012-08-09   wek   Move the superblock metadata to the end of the superblock.
2012-08-03   wek   Allow copying fsg into fs1 and fs2.
2012-05-25   nr    Allow RAMFS and RMTS to have configurable page size.
2010-10-15   wek   Add page size ops for all efs devices.
2010-03-31   wek   Added authentication of EFS copies.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_rmts_config.h"


#if defined (FEATURE_EFS_EFS2_ON_RMTS) && \
    defined (FEATURE_EFS_QEFS2)

#include "fs_rmts_super.h"
#include "assert.h"
#include "fs_err.h"
#include <string.h>
#include "crc.h"


/* There are two magic numbers in the superblock itself.  These are to
 * reduce the likelyhood of deciding alternative data is a superblock.
 * In little endian, this is viewed in ASCII as "EFSSuper". */
#define FS_RMTS_SUPER_MAGIC1    (0x53534645)
#define FS_RMTS_SUPER_MAGIC2    (0x72657075)

#define FS_RMTS_SUPER_VERSION   (4)

/**************************************************************************
               HISTORY OF CHANGE FOR EACH VERSION OF SUPERBLOCK

  Version 1:
             struct fs_rmts_superblock
             {
               uint32              header;
               uint32              version;
               uint32              magic1;
               uint32              magic2;
               uint32              age;

               uint32              blk_size;
               uint32              blk_cnt;
               uint32              cluster_size;

               uint32              upper_data[FS_UPPER_DATA_COUNT];
               uint8               pad[FS_RMTS_SUPERBLOCK_PAD_SIZE];

               uint32              crc32;
             };

  Version 2: Added authentication of EFS copies.
          uint8   data_hash[FS_RMTS_HASH_SIZE];
          uint8   super_hash[FS_RMTS_HASH_SIZE];

  Version 3: Moved the superblock metadata to the end of the superblock due to
             changes in crypto API that no longer allows jump address inside
             the block to be authenticated and encrypted.

  Version 4: RMTS layout changed to support a write-cache. Since super CRC was
             no longer useful, removed CRC field. With changes to increment age
             upto unit_max size, additional fields were added to help debug
             issues that can arise out of copying FSG data into FS1 or FS2.
    Added new fields:
          uint32 cluster_size
          uint8 which_partition;
          uint8 i_am_golden;
          struct fs_rmts_region write_cache_region;
          struct fs_rmts_region data_region;
    Removed fields:
          uint8 data_hash[FS_RMTS_HASH_SIZE];
          uint32 crc32;
    Renamed field:
          uint8 super_hash[FS_RMTS_HASH_SIZE];
      to
          uint8 super_meta_data[FS_RMTS_META_DATA_SIZE];

**************************************************************************/

void
fs_rmts_super_init_superblock_sync_fields (struct fs_rmts_superblock *sb)
{
  ASSERT (sb != NULL);

  sb->age = FS_RMTS_SUPERBLOCK_AGE_MAX; /* At the time of first sync, age will
                                           be incremented to 0.*/
  sb->which_partition = FS_RMTS_PARTITION_ID_FS2; /* Makes first sync to FS1.*/
  sb->i_am_golden = 0;
  sb->data_region.sync_counter = FS_RMTS_NUM_EFS_COPIES;
}

void
fs_rmts_super_create_superblock (struct fs_rmts_superblock *sb,
                                 efs_device_t dev)
{
  uint32 i;

  ASSERT (sb != NULL);
  memset (sb, 0xFF, sizeof (struct fs_rmts_superblock));

  sb->header = 0x53000000;
  sb->version = FS_RMTS_SUPER_VERSION;
  sb->magic1 = FS_RMTS_SUPER_MAGIC1;
  sb->magic2 = FS_RMTS_SUPER_MAGIC2;

  /* Store device information in superblock. */
  sb->sector_cnt = dev->block_count (dev);
  sb->sector_size = FS_RMTS_SECTOR_SIZE;
  sb->cluster_size = dev->page_size (dev);

  for (i = 0; i < FS_UPPER_DATA_COUNT; i++)
  {
    sb->upper_data[i] = FS_INVALID_PAGE_ID;
  }

  fs_rmts_super_init_superblock_sync_fields (sb);

  memset (sb->oem_data, 0, sizeof (sb->oem_data));
}

int
fs_rmts_validate_superblock (struct fs_rmts_superblock *sb)
{
  if (sb == NULL)
    return -1;

  if (sb->magic1 != FS_RMTS_SUPER_MAGIC1 ||
      sb->magic2 != FS_RMTS_SUPER_MAGIC2)
     return -1;

  if (sb->version != FS_RMTS_SUPER_VERSION)
    return -1;

  return 0;
}

void
fs_rmts_verify_build_with_superblock (efs_device_t dev,
                                      struct fs_rmts_superblock *sb)
{
  uint32 sector_cnt, sector_size;

  ASSERT (sb!= NULL);

  sector_cnt = dev->block_count (dev);
  if (sb->sector_cnt != sector_cnt)
  {
    FS_ERR_FATAL ("Build mismatch with superblock %d != %d",
                  sb->sector_cnt, sector_cnt, 0);
  }

  sector_size = FS_RMTS_SECTOR_SIZE;
  if (sb->sector_size != sector_size)
  {
    FS_ERR_FATAL ("Build mismatch with superblock %d != %d",
                  sb->sector_size, sector_size, 0);
  }

  if (sb->cluster_size != dev->page_size (dev))
  {
    FS_ERR_FATAL ("Build mismatch with superblock %d != %d",
                  sb->cluster_size, dev->page_size (dev), 0);
  }
}

int
fs_rmts_get_latest_superblock (struct fs_rmts_superblock *sb0,
                                     struct fs_rmts_superblock *sb1)
{
  ASSERT (sb0 != NULL && sb1 != NULL);

  /* Handle overflow case separately. Partitions should have been verified for
     odd and even ages by now. Should suffice to check for sb1->age as MAX. */
  if (sb1->age == FS_RMTS_SUPERBLOCK_AGE_MAX)
  {
    if (sb0->age == 0)
      return 0;
  }

  return ((sb1->age > sb0->age) ? 1 : 0);
}

int
fs_rmts_super_validate_age (struct fs_rmts_superblock *sb0,
                            struct fs_rmts_superblock *sb1)
{
  uint32 age_in_seq, age_diff;

  ASSERT (sb0 != NULL || sb1 != NULL); /* At least one good superblock. */

  if (sb0 != NULL && (sb0->age % 2 != 0))
  {
    FS_ERR_FATAL ("%d, FS1 age not even", sb0->age, 0, 0);
  }

  if (sb1 != NULL && (sb1->age % 2 != 1))
  {
    FS_ERR_FATAL ("%d, FS2 age not odd", sb1->age, 0, 0);
  }

  if (sb0 == NULL || sb1 == NULL)
  {
    /* Since we don't have both superblocks, we cannot check age sequence. */
    return 1;
  }

  /* Both superblock are good, check age sequences. */
  if(sb1->age == FS_RMTS_SUPERBLOCK_AGE_MAX && sb0->age == 0)
  {
    return 1;
  }

  age_diff = (sb1->age > sb0->age) ? (sb1->age - sb0->age) :
                                     (sb0->age - sb1->age);
  age_in_seq = (age_diff == 1) ? 1 : 0;

  /* The ages need not be in sequence in only one scenario.
     1. Both FS1 and FS2's non-superblock regions alone get corrupted.
     2. We do a fresh start and after one sync the phone is reset. Now FS1 has
        age  0 and FS2 has corrupted data but a good superblock with some age.
     So if ages are not in sequence then FS1's age has to be 0. We cannot
     tolerate any other sequence without running the risk of not knowing if we
     lost any filesystem data; So err-fatal. */
  if (age_in_seq != 1 && sb0->age != 0)
  {
    FS_ERR_FATAL ("%d,%d,Superblk age not in sequence", sb0->age, sb1->age, 0);
  }

  return age_in_seq;
}


void
fs_rmts_super_increment_age (struct fs_rmts_superblock *sb)
{
  /* If we move to a 64-bit machine, then age greater than current MAX will
     be supported. To always maintain constant MAX between images, check
     overflow explicitely. */
  if (sb->age == FS_RMTS_SUPERBLOCK_AGE_MAX)
    sb->age = 0;
  else
    ++sb->age;
}

#endif /* FEATURE_EFS_EFS2_ON_RMTS */
