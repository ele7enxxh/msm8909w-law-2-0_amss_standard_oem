/***********************************************************************
 * fs_hotplug_parser.h
 *
 * Partition Table parsers (GPT and MBR)
 * Copyright (C) 2011-2012 Qualcomm Technologies, Inc.
 *
 * Disk are frequently partitioned into smaller chunks called partitions.
 * This module implements the parsers for GPT and MBR. GUID Partition Table
 * (GPT) is an disk partitioning scheme specified by UEFI specification.
 * Master Boot Record (MBR) is an older disk partitioning scheme. This file
 * specifies the APIs to to parse all the partitions in a GPT or a MBR
 * returning information of the partition.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_parser.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-09-20   rp    Fix packing structure with standard syntax.
2011-10-31   wek   Partition parser speed optimizations.
2011-04-29   wek   Fix documentation errors.
2011-05-01   wek   Comply with legacy behavior for MBR parsing.
2011-03-31   wek   Create. Implement GPT/MBR partition table parser.

===========================================================================*/

#ifndef __FS_HOTPLUG_PARSER_H__
#define __FS_HOTPLUG_PARSER_H__

#include "fs_blockdev_i.h"

/*                          GPT Parsing APIs                        */

typedef struct _hotplug_gpt_iter HOTPLUG_GPT_ITER;

#define HOTPLUG_MBR_DOS_EXTENDED_PARTITION  0x05
#define HOTPLUG_MBR_WIN_EXTENDED_PARTITION  0x0F

/* Size of partition name in GPT */
#define HOTPLUG_GPT_PARTITION_NAME_SIZE_BYTES  72

/* Each partition has this structure of information associated with it. */
PACKED struct hotplug_gpt_entry
{
  struct hotplug_guid partition_type_guid;
  struct hotplug_guid partition_guid; /* Each partition has its own GUID */
  uint64 starting_lba;
  uint64 ending_lba;
  uint64 attributes;
  uint8 partition_name[HOTPLUG_GPT_PARTITION_NAME_SIZE_BYTES];
} PACKED_POST;

struct hotplug_sector_cache;

/* Initializes the parser cache.
 *
 * The hotplug partition parser (GPT and MBR) reads many sectors from the
 * device. To reduce the number of reads and make access to the device more
 * efficient we cache some sectors. The cache consists of a set of consecutive
 * sectors. This cache is shared between the three parsers: GPT, MBR and FBR.
 * The cache must be initalized whenever the block device changes. Whenever
 * the cache is initialized sector 0 of the device is automatically cached.
 *
 * param bdev_handle Handle returned by blockdev_open.
 * param bdev Block device to parse.
 *
 * return NULL if it failed to initalize. Valid sector cache handle on success.
 */
struct hotplug_sector_cache *
hotplug_pparser_cache_init (struct block_device *bdev, void *bdev_handle);

/* Opens a block device to be parsed using GPT.
 *
 * This API supports only one client at a time, it does not check if
 * two clients called this API.
 *
 * param data_cache Parameter returned by pparser_cache_init.
 *
 * return NULL on error, valid iterator on success.
 */
HOTPLUG_GPT_ITER *
hotplug_gpt_parse_open (struct hotplug_sector_cache *data_cache);

/* Reads a single partition from GPT.
 *
 * param gpt_iter value returned by hotplug_gpt_parse_open ()
 *
 * return On success pointer to the structure with partition data.
 *        Returns NULL on error and when no more partitions are found.
 */
struct hotplug_gpt_entry *
hotplug_gpt_parse_read (HOTPLUG_GPT_ITER *gpt_ter);

/* Closes the partition parser.
 *
 * param gpt_iter value returned by hotplug_gpt_parse_open ()
 *
 * return 0 on success, -1 on error.
 */
int hotplug_gpt_parse_close (HOTPLUG_GPT_ITER *gpt_iter);





/*                          MBR Parsing APIs                        */

struct hotplug_mbr_partition_data
{
  uint8 bootable;
  uint8 partition_type;
  uint32 start_lba;
  uint32 size_in_blocks;
};

typedef struct _hotplug_mbr_iter HOTPLUG_MBR_ITER;

/* Opens a block device to be parsed using MBR.
 *
 * This API supports only one client at a time, it does not check if
 * two clients called this API.
 *
 * param data_cache Parameter returned by pparser_cache_init.
 *
 * return NULL on error, valid iterator on success.
 */
HOTPLUG_MBR_ITER *
hotplug_mbr_parse_open (struct hotplug_sector_cache *data_cache);

/* Reads a single partition from MBR.
 *
 * param mbr_iter value returned by hotplug_mbr_parse_open ()
 *
 * return On success pointer to the structure with partition data.
 *        Returns NULL on error and when no more partitions are found.
 */
struct hotplug_mbr_partition_data *
hotplug_mbr_read_primary (HOTPLUG_MBR_ITER *mbr_ter);

/* Checks if the latest partition returned by the iterator is a
 * extended partition. It will set up the iterator to iterate through
 * the extended partitions.
 *
 * param mbr_iter value returned by hotplug_mbr_parse_open ()
 *
 * return 0 If it is an extended partition and it is good to call
 *        hotplug_mbr_read_extended (). Returns -1 if not an extended
 *        partition.
 */
int hotplug_mbr_check_extended (HOTPLUG_MBR_ITER *mbr_ter);

/* Iterates through all the extended partitions.
 *
 * If the funciton hotplug_mbr_check_extended () returns success
 * this function should be called to get each extended partition.
 *
 * param mbr_iter value returned by hotplug_mbr_parse_open ()
 *
 * return On success pointer to the structure with partition data.
 *        Returns NULL on error and when no more partitions are found.
 */
struct hotplug_mbr_partition_data *
hotplug_mbr_read_extended (HOTPLUG_MBR_ITER *mbr_iter);

/* Closes the partition parser.
 *
 * param mbr_iter value returned by hotplug_mbr_parse_open ()
 *
 * return 0 on success, -1 on error.
 */
int hotplug_mbr_parse_close (HOTPLUG_MBR_ITER *mbr_iter);

/* Checks if there is a FAT boot record in sector zero of the device.
 *
 * param data_cache Parameter returned by pparser_cache_init.
 *
 * return -1 if there is no FAT Boot record on the first sector of the
 *           hard partition, 0 if there is.*/
int
hotplug_fat_parse_check (struct hotplug_sector_cache *data_cache);

/* Change the buffer used for caching. This is used in off target tests
 * to test different cache sizes.
 *
 * param new_buffer new buffer to be used as a cache. If NULL is given
 *                  default values are restored.
 * param buf_size   Size of the buffer. If 0 is given, default values are
 *                  restored.
 */
void
hotplug_pparser_change_cache (uint8 *new_buffer, uint32 buf_size);

#endif /* not __FS_HOTPLUG_PARSER_H__ */
