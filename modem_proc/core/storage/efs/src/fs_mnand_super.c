/***********************************************************************
 * fs_mnand_super.c
 *
 * mNand Super module.
 * Copyright (C) 2008, 2010, 2012 QUALCOMM Technologies, Inc.
 *
 * This file contains all the mNand super module logic.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_mnand_super.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-05-16   nr    Make MNAND cluster/page size configurable at runtime.
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2010-11-15   wek   Make mnand indpendent of efs page size macro.
2010-11-08   wek   Remove impractical and unused paranoia module.
2010-10-01   wek   Rename mnand macros from clusters to page.
2008-12-12   rp    Create

===========================================================================*/

#include "fs_device.h"

#if defined (FEATURE_EFS_EFS2_ON_MNAND) &&    \
    defined (FEATURE_EFS_QEFS2)

#include "fs_mnand_super.h"
#include "fs_mnand_config.h"
#include "assert.h"
#include "crc.h"
#include "fs_err.h"
#include <string.h>
#include "fs_mnand_ptbl.h"
#include "fs_mnand_pm.h"
#include "fs_protected_memory.h"

/* There are two magic numbers in the superblock itself.  These are to
 * reduce the likelyhood of deciding alternative data is a superblock.
 * In little endian, this is viewed in ASCII as "EFSSuper". */
#define FS_MNAND_SUPER_MAGIC1    0x53534645
#define FS_MNAND_SUPER_MAGIC2    0x72657075


extern int fs_fresh_start;

/** Aligned superblock instance. */
static FS_ALIGN struct fs_mnand_superblock_data super_block_data;

/* Prototypes. */
static fs_sector_id mnand_find_superblock (fs_mnand_super_t super);
static void mnand_create_superblock (fs_mnand_super_t super);

void
fs_mnand_super_init (fs_mnand_super_t super, efs_device_t dev,
                     int *fresh_start)
{
  uint32 sectors_per_page = dev->priv.mnand_dev.sectors_per_page;

  ASSERT (super != NULL);
  ASSERT (dev != NULL);
  ASSERT (fresh_start != NULL);

  ASSERT (sizeof (struct fs_mnand_superblock_data) ==
                                        FS_MNAND_SUPER_BLOCK_SIZE);

  super->dev = dev;
  super->data = &super_block_data;
  ASSERT (FS_OKAY_FOR_DM (super->data));

  super->super_sector = mnand_find_superblock (super);

  /* Also, if someone with a debugger is requesting a fresh start, do so. */
  if (fs_fresh_start)
    super->super_sector = FS_INVALID_SECTOR_ID;

  /* If we didn't find any superblocks, create an initial one. */
  if (super->super_sector == FS_INVALID_SECTOR_ID)
  {
    mnand_create_superblock (super);
    *fresh_start = 1;
  }

  if (super->data->page_size != fs_device_page_size (dev))
    FS_ERR_FATAL ("[%d != %d]FS MNAND PAGE SIZE mismatch...",
                  super->data->page_size, fs_device_page_size (dev), 0);

  if (super->data->sect_per_clust != sectors_per_page)
    FS_ERR_FATAL ("[%d != %d]FS MNAND SECTORS PER PAGE mismatch...",
                  super->data->sect_per_clust, sectors_per_page, 0);

  if (super->data->block_size != FS_MNAND_PAGES_PER_BLOCK)
    FS_ERR_FATAL ("FS_MNAND_PAGES_PER_BLOCK mismatch...", 0, 0, 0);

}


static int
mnand_superblock_version_compatible (uint32 version)
{
  (void)version;
  return 1;
}

/** Checks the superblocks CRC with the computed one. */
static int
mnand_check_superblock (fs_mnand_superblock_data_t sd)
{
  if (sd == NULL
      || sd->magic1 != FS_MNAND_SUPER_MAGIC1
      || sd->magic2 != FS_MNAND_SUPER_MAGIC2
      || !mnand_superblock_version_compatible (sd->version))
    return 0;

  /* Check 30 bit crc. */
  if (crc_30_calc ((byte *) sd, sizeof(struct fs_mnand_superblock_data)*8-32)
      != sd->crc32)
    return 0;
  else
    return 1;
}

static fs_sector_id
mnand_find_superblock (fs_mnand_super_t super)
{
  fs_sector_id super_sect = FS_INVALID_SECTOR_ID;

  fs_device_read_sectors (super->dev, FS_MNAND_SUPER_SECTOR_ID,
                          super->data, FS_MNAND_SUPER_BLOCK_SECTORS_PER_PAGE,
                          FS_FOP_SUPER);

  if (mnand_check_superblock (super->data))
  {
    super_sect = FS_MNAND_SUPER_SECTOR_ID;
  }

  return super_sect;
}

static void
mnand_create_superblock (fs_mnand_super_t super)
{
  uint32 old_version;
  uint32 i;

  old_version = super->data->version;

  memset (super->data, 0, sizeof (struct fs_mnand_superblock_data));

  /* Fill in all of the obvious information for the superblock. */
  super->data->page_header = 0x53000000;
  super->data->version = old_version;
  super->data->age = 0;
  super->data->magic1 = FS_MNAND_SUPER_MAGIC1;
  super->data->magic2 = FS_MNAND_SUPER_MAGIC2;

  for (i = 0; i < FS_UPPER_DATA_COUNT; i++) {
    super->data->upper_data[i] = FS_INVALID_PAGE_ID;
  }

  fs_mnand_region_create (super->dev, super);
}


void
fs_mnand_super_write_super (fs_mnand_super_t super)
{

  /* Compute and store a 30-bit CRC. */
  super->data->crc32 = crc_30_calc ((byte *)super->data,
                        sizeof (struct fs_mnand_superblock_data) * 8 - 32);

  fs_device_reliable_write_sectors (super->dev, FS_MNAND_SUPER_SECTOR_ID,
                                    super->data,
                                    FS_MNAND_SUPER_BLOCK_SECTORS_PER_PAGE,
                                    FS_FOP_SUPER);
}

/** Erase the super-page, fills the super-page to 0xFF.  */
void
fs_mnand_super_erase_super_page (fs_mnand_super_t super)
{
  memset (super->data, 0xFF, sizeof (struct fs_mnand_superblock_data));
  fs_device_reliable_write_sectors (super->dev, FS_MNAND_SUPER_SECTOR_ID,
                                    super->data,
                                    FS_MNAND_SUPER_BLOCK_SECTORS_PER_PAGE,
                                    FS_FOP_SUPER);
}

#endif /* FEATURE_EFS_EFS2_ON_MNAND */

