/***********************************************************************
 * fs_rmts_super.h
 *
 * Remote Storage Superblock module.
 * Copyright (C) 2009-2010,2012-2013 QUALCOMM Technologies, Inc.
 *
 * The remote storage page manager uses superblock to start interpreting the
 * rest of the partition data. This is the first region it decrypts to learn
 * about the layout of the filesystem. This module provides functions to
 * create and manage superblock data.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_rmts_super.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-07-22   rp    Add new EFS API to store data in SuperBlock.
2012-12-18   dks   Update golden copy strategy for new rmts layout.
2012-10-19   dks   Make superblock age run to uint32_max.
2012-09-20   dks   Update rmts layout to support write-cache.
2012-08-09   wek   Move the superblock metadata to the end of the superblock.
2010-10-15   wek   Add page size ops for all efs devices.
2010-03-31   wek   Added authentication of EFS copies.
2009-11-16   rp    Create

===========================================================================*/

#ifndef __FS_RMTS_SUPER_H__
#define __FS_RMTS_SUPER_H__


#include "fs_rmts_config.h"

#if defined (FEATURE_EFS_EFS2_ON_RMTS) && \
    defined (FEATURE_EFS_QEFS2)

#include "fs_sys_types.h"
#include "fs_device.h"


/* The Remote Storage Page Manager divides sectors into regions to manage
   encryption and writes more efficiently. Each region is decrypted and
   encrypted as one block. The following structure stores information about
   the region boundaries and information needed to encrypt and decrypt the
   data for the region. */
PACKED struct fs_rmts_region
{
  uint32 start_sector;  /* Sector number in partition where region starts. */
  uint32 num_sectors;
  uint32 encrypted_data_size_bytes;    /* Size of encrypted data in bytes.
                                          Required in case data is zipped. */
  uint8  encryption_meta_data[FS_RMTS_META_DATA_SIZE];
  uint8  sync_counter;  /* Indicates number of syncs pending to make data
                           current in all EFS copies. */
}PACKED_POST;

/** Account for the unused space in the superblock. */
#define FS_RMTS_SUPERBLOCK_PAD_SIZE (FS_RMTS_SECTOR_SIZE  - \
                                    (5 * sizeof (uint32)) - \
                                    (2 * sizeof (uint8))  - \
                                    (3 * sizeof (uint32)) - \
                                    (2 * sizeof (struct fs_rmts_region))    - \
                                    (FS_UPPER_DATA_COUNT * sizeof (uint32)) - \
                                    (1 * sizeof (uint32)) - \
                                   FS_RMTS_SUPERBLOCK_OEM_DATA_SIZE_IN_BYTES -\
                                    FS_RMTS_META_DATA_SIZE)


/** Format of the superblock. */
PACKED struct fs_rmts_superblock
{
  uint32 header;
  uint32 version;
  uint32 magic1;
  uint32 magic2;
  uint32 age;

  uint8  which_partition;  /* Indicates which partition the current copy of EFS
                              is meant to be written into. */
  uint8  i_am_golden;  /* Should be set only for a sync that leads to golden
                          copy creation. */

  uint32 sector_size;  /* Size of a single sector. */
  uint32 sector_cnt;   /* Number of sectors in the device. */
  uint32 cluster_size; /* Fundamental read/write unit for buffer layer. */

  struct fs_rmts_region write_cache_region;
  struct fs_rmts_region data_region;

  uint32 upper_data[FS_UPPER_DATA_COUNT];
  uint32 debug_data_bit_map; /* Bit map of various debugging data. For a list
                                of bit positions and what they stand for, check
                                FS_RMTS_SUPER_*_MASK defines */

  uint8  pad[FS_RMTS_SUPERBLOCK_PAD_SIZE];

  uint8  oem_data[FS_RMTS_SUPERBLOCK_OEM_DATA_SIZE_IN_BYTES];

  uint8  super_meta_data[FS_RMTS_META_DATA_SIZE];
}PACKED_POST;

/** Size of the superblock. */
#define FS_RMTS_SUPERBLOCK_SIZE         sizeof (struct fs_rmts_superblock)

#define FS_RMTS_SUPERBLOCK_AGE_MAX      0xFFFFFFFF

/* Bit fileds that are set in debug_data_bit_map */
#define FS_RMTS_SUPER_GOLDEN_NOT_CREATED_MASK 0x1
#define FS_RMTS_SUPER_DEBUG_MAX_MASK          0x80000000


/*************************************************************************//**
 * Initialize the superblock.
 *
 * @param sb
 *    Pointer to the superblock
 *
 * @param dev
 *    Pointer to the device.
 *
 * @return
 *    None.
*****************************************************************************/
void fs_rmts_super_create_superblock (struct fs_rmts_superblock *sb,
                                      efs_device_t dev);

/*************************************************************************//**
 * Initialize defaults for sync related fields of superblock.
 *
 * @param sb
 *    Pointer to the superblock
 *
 * @return
 *    None.
*****************************************************************************/
void fs_rmts_super_init_superblock_sync_fields (struct fs_rmts_superblock *sb);

/*************************************************************************//**
 * Validate the superblock for correctness and compatibility.
 *
 * @param sb
 *    Pointer to the superblock
 *
 * @return
 *    0 if superblock is good -1 otherwise.
*****************************************************************************/
int fs_rmts_validate_superblock (struct fs_rmts_superblock *sb);


/*************************************************************************//**
 * Compares superblock contents with the current build and will cause
 * err-fatals, if there are any unacceptable mismatches.
 *
 * @param sb
 *    Pointer to the superblock
 *
 * @param dev
 *    Pointer to the device.
 *
 * @return
 *    None.
*****************************************************************************/
void fs_rmts_verify_build_with_superblock (efs_device_t dev,
                                           struct fs_rmts_superblock *sb);

/*************************************************************************//**
 * Compares the 2 supplied superblocks, and returns the more latest superblock.
 *
 * @param sb1
 *    Pointer to the first superblock
 *
 * @param sb2
 *    Pointer to the second superblock
 *
 * @return
 *    sb1 or sb2, whichever is latest.
*****************************************************************************/
int fs_rmts_get_latest_superblock (struct fs_rmts_superblock *sb0,
                                   struct fs_rmts_superblock *sb1);

/*************************************************************************//**
 * Verifies superblock ages are correctly paired as odd and even and are in
 * sequence.
 *
 * @param sb1
 *    Pointer to the first superblock. Can be NULL if sb2 is not NULL.
 *
 * @param sb2
 *    Pointer to the second superblock. Can be NULL if sb1 is not NULL.
 *
 * @return
 *    Returns 1 if superblock ages match partition and are in sequence.
 *    0 othewise.
*****************************************************************************/
int fs_rmts_super_validate_age (struct fs_rmts_superblock *sb0,
                                struct fs_rmts_superblock *sb1);

/*************************************************************************//**
 * Increment the age of the superblock.
 *
 * @param sb
 *    Pointer to the superblock
 *
 * @return
 *    None.
*****************************************************************************/
void fs_rmts_super_increment_age (struct fs_rmts_superblock *sb);

#endif /* FEATURE_EFS_EFS2_ON_RMTS */

#endif /* not __FS_RMTS_SUPER_H__ */
