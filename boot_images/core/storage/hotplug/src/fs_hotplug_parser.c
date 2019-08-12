/***********************************************************************
 * fs_hotplug_parser.c
 *
 * Partition Table parsers (GPT and MBR)
 * Copyright (C) 2011-2013 QUALCOMM Technologies, Inc.
 *
 * GUID Partition Table (GPT) is an disk partitioning scheme specified
 * by UEFI specification. This file implements a way to parse all the
 * partitions in a GPT returning information of the partition.
 *
 * This code makes the following assumptions:
 * - It is running in little endian system.
 * - Partition has been parsed with UEFI standard version 2.3.
 *
 * The Master Boot Record is an older specification where the disk can
 * contain primary partitions with an extended partition, which is divided
 * into more partitions.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_parser.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-09-30   wek   Remove the feature that disables MBR checks.
2013-05-24   rp    Add UFS support to Hotplug.
2012-07-30   wek   Handle partition entries larger than the block size.
2012-03-29   nr    New MBR detection Scheme.
2012-03-18   wek   Reduce hotplug size for bootloaders.
2012-01-17   wek   Remove unused crc header file.
2011-12-09   wek   Remove check for enforcing protective MBR in GPT parser.
2011-10-31   wek   Partition parser speed optimizations.
2011-10-06   wek   Return uint32 for block count.
2011-05-01   wek   Comply with legacy behavior for MBR parsing.
2011-03-31   wek   Create. Implement GPT/MBR partition table parser.

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG

#include "fs_err.h"
#include "fs_hotplug_parser.h"
#include "assert.h"

#include <string.h>
#include <limits.h>

#define HOTPLUG_PARTITION_ID_GPT  0xEE
#define GPT_SIGNATURE     "EFI PART"
#define GPT_SIGNATURE_SIZE     8

/* The UEFI Specification indicates that the partition entry size is a
 * multiple of 128. */
#define GPT_PARTITION_ENTRY_BASE_SIZE_BYTES 128

PACKED struct hotplug_mbr_partition_record
{
  uint8 bootable;
  uint8 start_CHS[3]; /* Cylinder, Head, Sector */
  uint8 partition_type;
  uint8 end_CHS[3];
  uint32 start_lba;   /* Start Logical Block Address */
  uint32 size_in_blocks;
} PACKED_POST;

/* Typical MBR layout in a disk, used by MBR and GPT */
PACKED struct hotplug_mbr_layout
{
  uint8 boot_code[440];
  uint8 disk_signature[4];
  uint8 pad[2];       /* There is no specified use for this field. */
  struct hotplug_mbr_partition_record records[4];
  uint8 signature[2]; /* Should be set to 0xAA and 0x55 */
} PACKED_POST;



PACKED struct hotplug_gpt_header
{
  uint8 signature[GPT_SIGNATURE_SIZE]; /* Should be "EFI PART" */
  uint32 revision;    /* Header revision number, this header is 1.0 */
  uint32 header_size; /* Size of this header in bytes. */
  uint32 header_crc32; /* CRC 32 of this header, set to zero for calculations*/
  uint32 reserved1;
  uint64 my_lba;  /* Logical Block Address of this header. */
  uint64 alternate_lba; /* Logical Block Address of alternate header. */
  uint64 first_usable_lba; /* Min LBA for any partition. */
  uint64 last_usable_lba; /* Max LBA for any partition. */
  struct hotplug_guid disk_guid;
  uint64 partition_entry_lba; /* Partiton entry table LBA. */
  uint32 number_of_partition_entries; /* Number of entires in partition array*/
  uint32 size_of_partition_entry; /* Size of each partition entry. */
  uint32 partition_entries_crc32; /* CRC32 of partition entry array. */
  /* The rest of the block is reserved. The header size should be 92 bytes. */
} PACKED_POST;


/* Read cache info. */
struct hotplug_sector_cache
{
  struct block_device *bdev; /* Block device to parse */
  void *bdev_handle; /* Handle returned by blockdev_open */
  uint16 block_size_bytes; /* Size of a Logical Block in bytes */
  uint32 block_count; /* Number of blocks in the device */

  uint8 *buffer; /* Points to the buffer that holds the logical block data. */
  uint32 buffer_size_blocks; /* Number of Logical Blocks that can fit in the
                                buffer. */
  uint32 cache_size_blocks; /* Number of blocks in the cache. */
  uint64 first_cached_lba; /* LBA of the first cached block in the buffer. */
};

/* All temporary data used to keep track of iteration is stored in this
 * structure. */
struct _hotplug_gpt_iter
{
  uint32 block_size_bytes; /* Size of a Logical Block in bytes */

  /* these values keep track of the iterator state, what is the next value. */
  uint32 remaining_entries; /* Number of entries we still need to parse. */
  uint64 curr_entry_array_lba; /* Block we are parsing for partitions. */
  uint64 last_entry_array_lba; /* Last LBA for the partition entry array. */
  uint32 size_of_partition_entry; /* Size of each partition entry. */
  uint32 current_entry_offset; /* Offset in the block of the current partition
                                  entry. */
  struct hotplug_sector_cache *data_cache;
};

/* Certain configurations (e.i. Boot) have size restrictions.
 * Allow the hotplug parser cache size to be configurable. */
#ifndef FS_HOTPLUG_PARSER_CACHE_SIZE_BYTES
  #define FS_HOTPLUG_PARSER_CACHE_SIZE_BYTES HOTPLUG_LOGICAL_BLOCK_SIZE_BYTES
#endif
#if FS_HOTPLUG_PARSER_CACHE_SIZE_BYTES < HOTPLUG_LOGICAL_BLOCK_SIZE_BYTES
  #error "Hotplug parser cache must be at least 512 bytes."
#endif

/* Local variable declarations. */
/* _gpt_iter is used to return a iter handle to the caller */
static HOTPLUG_GPT_ITER _gpt_iter;
/* Holds the sector cache information, shared among GPT and MBR iterators. */
static struct hotplug_sector_cache _pparser_cache;
/* Buffer to hold the read data. */
static uint8 _raw_data_buffer[FS_HOTPLUG_PARSER_CACHE_SIZE_BYTES];
/* Use variables to reference the buffer, it allows off target tests to
 * change the cache size. */
static uint8 *raw_data_buffer = _raw_data_buffer;
static uint32 raw_data_buffer_size = FS_HOTPLUG_PARSER_CACHE_SIZE_BYTES;

/* CRC32 calculation data and implementation. */
#define CRC32_SEED   0

static const uint32  crc32_table[256] = {
  0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F,
  0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
  0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2,
  0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
  0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
  0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
  0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
  0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
  0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423,
  0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
  0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
  0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
  0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D,
  0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
  0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
  0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
  0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7,
  0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
  0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA,
  0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
  0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
  0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
  0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84,
  0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
  0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
  0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
  0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E,
  0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
  0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55,
  0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
  0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28,
  0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
  0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F,
  0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
  0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
  0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
  0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69,
  0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
  0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC,
  0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
  0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693,
  0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
  0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

/* Calculates CRC32 (Cyclic redundancy check) over the number of bytes
 * specified.
 *
 * param data   Buffer to use to calculate crc32
 * param nbytes Calculate CRC32 over this number of bytes.
 * param seed   CRC32 seed. Used to chain CRC32 calculations.
 *
 * return CRC32 over the buffer */
static uint32
gpt_calc_crc32 (const uint8 *data, uint32 nbytes, uint32 seed)
{
  uint32  crc = 0;
  uint32  pos;
  const uint8   *pbyte;

  if ((data != NULL) && (nbytes != 0))
  {
    crc = 0xFFFFFFFF ^ seed;
    for (pos = 0, pbyte = data; pos < nbytes; pos++, pbyte++)
    {
      crc = (crc >> 8) ^ crc32_table[(uint8) crc ^ *pbyte];
    }
    crc ^= 0xFFFFFFFF;
  }
  return  crc;
}


/* These set of functions will swap from little endian to host, ARM is little
 * endian, so no need to implement them for now. */
static uint32
little_to_host32 (uint32 value)
{
  return value;
}
static uint64
little_to_host64 (uint64 value)
{
  return value;
}

static void
gpt_header_little_to_host (struct hotplug_gpt_header *header_in,
                           struct hotplug_gpt_header *header_out)
{
  /* The current architecture is little endian, data can be read directly
   * from disk with no conversion. If we ever need to compile this module
   * for big endian it should be modified to translate the input gpt
   * header to the output gpt header. */
  (void) header_out;
  (void) little_to_host32 (header_in->revision);
  (void) little_to_host32 (header_in->header_size);
  (void) little_to_host32 (header_in->header_crc32);
  (void) little_to_host32 (header_in->reserved1);
  (void) little_to_host64 (header_in->my_lba);
  (void) little_to_host64 (header_in->alternate_lba);
  (void) little_to_host64 (header_in->first_usable_lba);
  (void) little_to_host64 (header_in->last_usable_lba);
  (void) little_to_host64 (header_in->partition_entry_lba);
  (void) little_to_host32 (header_in->number_of_partition_entries);
  (void) little_to_host32 (header_in->size_of_partition_entry);
  (void) little_to_host32 (header_in->partition_entries_crc32);

  (void) header_in->disk_guid;
}

static void
gpt_partition_entry_little_to_host (struct hotplug_gpt_entry *entry_in,
                                    struct hotplug_gpt_entry *entry_out)
{
  (void) entry_out;
  (void) little_to_host64 (entry_in->starting_lba);
  (void) little_to_host64 (entry_in->ending_lba);
  (void) little_to_host64 (entry_in->attributes);
}

static uint16
pparser_cache_get_block_size (struct hotplug_sector_cache *data_cache)
{
  return data_cache->block_size_bytes;
}

static uint32
pparser_cache_get_block_count (struct hotplug_sector_cache *data_cache)
{
  return data_cache->block_count;
}

/* Reads the number of sectors specified by blk_cnt into the cache. Reads start
 * from the given Logical Block Address (lba).*/
static uint8 *
pparser_cache_read (uint64 lba, struct hotplug_sector_cache *data_cache,
                    uint32 blk_cnt)
{
  struct block_device *bdev = data_cache->bdev;
  void *bdev_handle = data_cache->bdev_handle;
  uint8 *buffer = data_cache->buffer;
  uint32 max_blk_cnt, dev_size_blocks;
  int result;

  /* There is something wrong with upper layers if they supply zero. */
  ASSERT (blk_cnt != 0);

  if (blk_cnt > data_cache->buffer_size_blocks)
    blk_cnt = data_cache->buffer_size_blocks;

  dev_size_blocks = pparser_cache_get_block_count (data_cache);
  /* The LBA should be less than the device size */
  if (lba >= dev_size_blocks)
    return NULL;

  max_blk_cnt = dev_size_blocks - ((uint32)lba);
  if (blk_cnt > max_blk_cnt)
    blk_cnt = max_blk_cnt;

  data_cache->cache_size_blocks = 0;
  result = blockdev_read (bdev, bdev_handle, (uint32) lba, buffer, blk_cnt);
  if (result != 0)
    return NULL;

  data_cache->first_cached_lba = lba;
  data_cache->cache_size_blocks = blk_cnt;

  return buffer;
}

/* Change the buffer used for caching. This is used in off target tests
 * to test different cache sizes.
 *
 * param new_buffer new buffer to be used as a cache. If NULL is given
 *                  default values are restored.
 * param buf_size   Size of the buffer. If 0 is given, default values are
 *                  restored.
 */
void
hotplug_pparser_change_cache (uint8 *new_buffer, uint32 buf_size)
{
  if (new_buffer == NULL || buf_size == 0)
  {
    raw_data_buffer = _raw_data_buffer;
    raw_data_buffer_size = sizeof (_raw_data_buffer);
  }
  else
  {
    raw_data_buffer = new_buffer;
    raw_data_buffer_size = buf_size;
  }
}

/* Initalizes the read cache. */
struct hotplug_sector_cache *
hotplug_pparser_cache_init (struct block_device *bdev, void *bdev_handle)
{
  struct hotplug_sector_cache *data_cache = &_pparser_cache;
  uint8 *buf;
  uint32 buffer_size_blocks;
  uint32 block_cnt = 0;
  uint16 bytes_per_block = 0;
  int result;

  /* Get and validate device sizes. */
  result = blockdev_get_device_size (bdev, bdev_handle, &block_cnt,
                                     &bytes_per_block);
  if (result != 0)
    return NULL;

  if (bytes_per_block > HOTPLUG_LOGICAL_BLOCK_SIZE_BYTES ||
      bytes_per_block == 0 || block_cnt == 0)
    return NULL;

  data_cache->bdev = bdev;
  data_cache->bdev_handle = bdev_handle;
  data_cache->block_size_bytes = bytes_per_block;
  data_cache->block_count = block_cnt;
  data_cache->buffer = raw_data_buffer;

  /* If the cache size in blocks is greater than the number of blocks in
   * the device, then use the device block count as the cache size.
   * We can't cache more sectors than what is in disk. */
  buffer_size_blocks = raw_data_buffer_size / bytes_per_block;
  if (buffer_size_blocks > block_cnt)
    buffer_size_blocks = block_cnt;
  data_cache->buffer_size_blocks = buffer_size_blocks;

  buf = pparser_cache_read (0ULL, data_cache, 1);
  if (buf == NULL)
    return NULL;

  return data_cache;
}

/* Checks if we have the LBA cached.
 * returns pointer to the buffer if we do, NULL if the lba is not cached. */
static uint8 *
pparser_read_cache_check (uint64 lba, struct hotplug_sector_cache *data_cache)
{
  uint64 first_cached_lba = data_cache->first_cached_lba;

  if (first_cached_lba <= lba)
  {
    uint64 lba_offset = lba - first_cached_lba;
    uint64 cached_cnt = ((uint64) data_cache->cache_size_blocks) & UINT_MAX;
    if (cached_cnt > lba_offset)
    {
      uint8 *buffer = data_cache->buffer;
      /* we have the lba cached, retrurn it. */
      buffer += (lba_offset * data_cache->block_size_bytes);
      return buffer;
    }
  }

  return NULL;
}

static uint8 *
pparser_read_lba (uint64 lba, uint32 read_ahead_hint,
                  struct hotplug_sector_cache *data_cache)
{
  uint8 *buffer;

  buffer = pparser_read_cache_check (lba, data_cache);

  /* if the lba is not cached, cache it */
  if (buffer == NULL)
  {
    buffer = pparser_cache_read (lba, data_cache, read_ahead_hint);
    if (buffer != NULL)
      buffer = pparser_read_cache_check (lba, data_cache);
  }

  return buffer;
}

static int
check_mbr_signature (struct hotplug_sector_cache *data_cache)
{
  struct hotplug_mbr_layout *mbr;
  uint8 *buffer;
  uint32 start_lba_list[4] = {0}; /* There are atmost 4 records in sector 0 */
  uint8 valid_record_count = 0;
  uint32 total_card_block_count = pparser_cache_get_block_count (data_cache);
  uint8 i, j;

  buffer = pparser_read_lba (0ULL, 1, data_cache);
  if (buffer == NULL)
    return -1;

  mbr = (struct hotplug_mbr_layout *) buffer;
  if (mbr->signature[0] != 0x55 || mbr->signature[1] != 0xAA)
  {
    return -1;
  }

  /* Check if this looks like a FAT boot Sector. If this is an FBS the sector
   * size in bytes 11 and 12 should be 512 i.e. 0x200 but an MBR could also
   * have the same values at these bytes so this check is more like a hint
   * to determine the need for further checks. */
  if (mbr->boot_code[11] == 0x00 && mbr->boot_code[12] == 0x02)
  {
    /* This could be an FBS so performing additional checks.
     * Check for alteast one valid record in the 4 slots. */
    for (i = 0; i < 4; i++)
    {
      if ((mbr->records[i].partition_type != 0) &&
          (mbr->records[i].size_in_blocks > 0))
      {
        /* Start LBA cannot be zero and should be within bounds for a valid
         * record in an MBR or it isnt an MBR. */
        if ((mbr->records[i].start_lba == 0) ||
            (mbr->records[i].start_lba > total_card_block_count))
        {
          return -1;
        }
        /* Proper record found so put it in the list. */
        start_lba_list[valid_record_count++] = mbr->records[i].start_lba;
      }
    }

    /* If no valid record is found then it is not an MBR. return */
    if (valid_record_count == 0)
    {
      return -1;
    }

    /* Check that the start LBA of all valid records are unique. */
    for (i = 0; i < (valid_record_count - 1); i++)
    {
      for (j = (i + 1); j < valid_record_count; j++)
      {
        if (start_lba_list[i] == start_lba_list[j])
        {
          return -1;
        }
      }
    }

  }

  /* Avoid warnings. */
  (void) mbr->disk_signature;
  (void) mbr->pad;
  (void) mbr->records[0].bootable;
  (void) mbr->records[0].start_CHS;
  (void) mbr->records[0].end_CHS;

  return 0;
}

/* Checks if the protective MBR required for GPT exists.
 *
 * This is a relaxed check since we don't verify all the fields in the
 * partition table, it only checks if the GPT partition type exists.
 * @return 0 if MBR is a valid protective MBR, -1 if not valid protective MBR.
 */
static int
gpt_check_protective_mbr (struct hotplug_sector_cache *data_cache)
{
  int i;
  struct hotplug_mbr_layout *mbr;
  uint8 *buffer;

  buffer = pparser_read_lba (0ULL, 1, data_cache);
  if (buffer == NULL)
    return -1;

  mbr = (struct hotplug_mbr_layout *) buffer;

  /* Go through all the partition entries in the MBR searching for partition
   * type 0xEE. This is a relaxed check for the protective MBR since we
   * are not checking if the size of the partition is correct and all
   * other partition entries are empty. */
  for (i = 0; i < 4; i++)
  {
    if (mbr->records[i].partition_type == HOTPLUG_PARTITION_ID_GPT)
      return 0;
  }

  return -1;
}

/* Does all the checks necessary to verify GPT is valid. It will udpate the
 * iterator structure to the appropiate values to parse the partitiont table
 *
 * param gpt_lba     LBA of the GPT Header.
 * param alternate_gpt_lba [out] Return value of the LBA of the alternate
 *                         GPT header according to the header at gpt_lba.
 * param gpt_iter    Iterator structure used by the GPT parser.
 *
 * return 0 if a valid GPT header is found, -1 if it failed to find a header.
 */
static int
gpt_check_header (uint64 gpt_lba, uint64 *alternate_gpt_lba,
                  HOTPLUG_GPT_ITER *gpt_iter)
{
  struct hotplug_gpt_header *gpt_header;
  uint8 *buffer;
  uint32 original_crc;
  uint32 crc;
  int result;
  uint64 entries_lba;
  uint32 partition_entry_count;
  uint32 partition_entry_size_bytes;
  uint32 entry_array_size_bytes;
  uint32 entry_lba_count;
  uint32 dev_size_blocks;

  buffer = pparser_read_lba (gpt_lba, 1, gpt_iter->data_cache);
  if (buffer == NULL)
    return -1;

  gpt_header = (struct hotplug_gpt_header *) buffer;
  result = memcmp (gpt_header->signature, GPT_SIGNATURE, GPT_SIGNATURE_SIZE);
  if (result != 0)
    return -1;

  original_crc = gpt_header->header_crc32;
  gpt_header->header_crc32 = 0;
  /* The header should be 92 bytes. */
  ASSERT (sizeof (struct hotplug_gpt_header) == 92);
  crc = gpt_calc_crc32 (buffer, sizeof (struct hotplug_gpt_header),CRC32_SEED);
  /* Return the buffer to the original state. */
  gpt_header->header_crc32 = original_crc;
  if (crc != original_crc)
    return -1;

  /* For now this function does nothing.*/
  gpt_header_little_to_host (gpt_header, NULL);
  if (gpt_lba != gpt_header->my_lba)
    return -1;

  *alternate_gpt_lba = gpt_header->alternate_lba;

  /* There has to be at least 1 partition entry */
  partition_entry_count = gpt_header->number_of_partition_entries;
  if (partition_entry_count == 0)
    return -1;

  partition_entry_size_bytes = gpt_header->size_of_partition_entry;
  if (partition_entry_size_bytes == 0 ||
      (partition_entry_size_bytes % GPT_PARTITION_ENTRY_BASE_SIZE_BYTES) != 0)
    return -1;

  entry_array_size_bytes = partition_entry_count * partition_entry_size_bytes;
  entry_lba_count =
    ((entry_array_size_bytes - 1) / gpt_iter->block_size_bytes) + 1;
  entries_lba = gpt_header->partition_entry_lba;

  /* store the imporant values in our iterator structure */
  gpt_iter->remaining_entries = gpt_header->number_of_partition_entries;
  gpt_iter->current_entry_offset = 0;
  gpt_iter->size_of_partition_entry = gpt_header->size_of_partition_entry;
  gpt_iter->curr_entry_array_lba = gpt_header->partition_entry_lba;
  gpt_iter->last_entry_array_lba = gpt_header->partition_entry_lba +
                                   entry_lba_count - 1;
  dev_size_blocks = pparser_cache_get_block_count (gpt_iter->data_cache);
  /* Check for wrap around of last_entry_lba */
  if (gpt_iter->last_entry_array_lba < gpt_header->partition_entry_lba ||
      gpt_iter->last_entry_array_lba > dev_size_blocks)
    return -1;

  /* Calculate CRC 32 over the partition entry array */
  original_crc = gpt_header->partition_entries_crc32;
  crc = CRC32_SEED;
  result = -1;

  while (entry_array_size_bytes > 0)
  {
    uint32 crc_size;
    uint32 read_ahead;

    read_ahead = (uint32)(gpt_iter->last_entry_array_lba - entries_lba + 1);
    buffer = pparser_read_lba (entries_lba, read_ahead, gpt_iter->data_cache);
    if (buffer == NULL)
      break;

    entries_lba++;
    crc_size = MIN (gpt_iter->block_size_bytes, entry_array_size_bytes);
    crc = gpt_calc_crc32 (buffer, crc_size, crc);
    entry_array_size_bytes -= crc_size;
  }

  if (crc == original_crc)
    result = 0;

  return result;
}


HOTPLUG_GPT_ITER *
hotplug_gpt_parse_open (struct hotplug_sector_cache *data_cache)
{
  int result = 0;
  int mbr_sig;
  HOTPLUG_GPT_ITER *gpt_iter;

  /* The read portion of the gpt iterator saves memory space by casting
   * and returning the read buffer to the caller. This is OK because the
   * gpt entry is less than 128 bytes; therefore we are guaranteed to
   * always have 128 bytes worth of data in the buffer. If the size of
   * the GPT entry structure changes (e.g. new UEFI Specifications) we
   * have to handle the cases when parsing the GPT entry table the
   * entry is split across sectors. For example, GPT entry size is 378,
   * the second entry in the table will have the data split across
   * sectors. The typical solution is to add an instance of struct
   * hotplug_gpt_entry into the HOTPLUG_GPT_ITER, during reads this
   * structure is populated and returned to the caller. This is done in
   * efs for directory iterators. */
  ASSERT (sizeof (struct hotplug_gpt_entry) == 128);

  gpt_iter = &_gpt_iter;
  /* Initialize the read cache. Failure means device read failed. */
  gpt_iter->data_cache = data_cache;

  mbr_sig = check_mbr_signature (gpt_iter->data_cache);
  /* If there is a valid MBR, it must be a protective MBR to be GPT. */
  if (mbr_sig == 0)
    result = gpt_check_protective_mbr (gpt_iter->data_cache);

  if (result == 0)
  {
    uint64 alternate_gpt_lba, gpt_lba;

    gpt_iter->block_size_bytes =
      pparser_cache_get_block_size (gpt_iter->data_cache);

    /* Check the primary GPT */
    gpt_lba = 1;
    result = gpt_check_header (gpt_lba, &alternate_gpt_lba, gpt_iter);

    /* If the primary GPT is bad, check the last sector in the device for a
     * valid GPT. */
    if (result != 0)
    {
      uint32 block_cnt;
      block_cnt = pparser_cache_get_block_count (gpt_iter->data_cache);

      gpt_lba = block_cnt - 1;
      result = gpt_check_header (gpt_lba, &alternate_gpt_lba, gpt_iter);
    }
  }

  if (result != 0)
    gpt_iter = NULL;

  return gpt_iter;
}

/* Checks if a given gpt_entry is valid.
 * Checks that is doing:
 *    - Is the type GUID not zero. */
static int
gpt_is_entry_valid (struct hotplug_gpt_entry *gpt_entry)
{
  uint32 *buffer;
  uint32 size;
  int i;
  int result = 0;

  /* Check if Type GUID is zeros */
  buffer = (uint32 *)&gpt_entry->partition_type_guid.data1;
  size = sizeof (gpt_entry->partition_type_guid);
  for (i = size; i > 0; i -= sizeof (uint32))
  {
    if (*buffer != 0)
    {
      result = -1;
      break;
    }
    buffer++;
  }

  return result;
}

/* Returns the current partition entry pointed by the iterator and
 * sets up the iterator status fileds to point to the next entry. */
static struct hotplug_gpt_entry *
gpt_get_next_partition_entry (HOTPLUG_GPT_ITER *gpt_iter)
{
  uint8 *buffer;
  struct hotplug_gpt_entry *gpt_entry;
  uint32 offset;
  uint32 read_ahead;

  /* Check if we are done. */
  if (gpt_iter->remaining_entries == 0)
    return NULL;

  read_ahead = (uint32)(gpt_iter->last_entry_array_lba -
                        gpt_iter->curr_entry_array_lba + 1);
  buffer = pparser_read_lba (gpt_iter->curr_entry_array_lba, read_ahead,
                             gpt_iter->data_cache);
  /* this is very strange since this values should be valid */
  if (buffer == NULL)
    return NULL;

  offset = gpt_iter->current_entry_offset;
  gpt_entry = (struct hotplug_gpt_entry *) (buffer + offset);

  /* Entry succesfully read, prepare for next entry. */
  gpt_iter->remaining_entries -= 1;
  gpt_iter->current_entry_offset += gpt_iter->size_of_partition_entry;

  if (gpt_iter->current_entry_offset >= gpt_iter->block_size_bytes)
  {
    gpt_iter->curr_entry_array_lba +=
                  gpt_iter->current_entry_offset / gpt_iter->block_size_bytes;

    gpt_iter->current_entry_offset =
                  gpt_iter->current_entry_offset % gpt_iter->block_size_bytes;
  }

  return gpt_entry;
}

struct hotplug_gpt_entry *
hotplug_gpt_parse_read (HOTPLUG_GPT_ITER *gpt_iter)
{
  struct hotplug_gpt_entry *gpt_entry;
  int result;

  if (gpt_iter == NULL)
    return NULL;

  do
  {
    gpt_entry = gpt_get_next_partition_entry (gpt_iter);
    if (gpt_entry == NULL)
      break;

    /* Check if this entry is used by checking the type guid is not zero. */
    result = gpt_is_entry_valid (gpt_entry);
  } while (result == 0);

  if (gpt_entry != NULL)
  {
    /* For now this function does nothing.*/
    gpt_partition_entry_little_to_host (gpt_entry, NULL);
  }

  return (gpt_entry);
}

int
hotplug_gpt_parse_close (HOTPLUG_GPT_ITER *gpt_iter)
{
  if (gpt_iter == NULL)
    return -1;

  /* No cleanup needed at this time, since we have only one iterator. */
  return 0;
}



struct _hotplug_mbr_iter
{
  uint32 next_ext_sector;  /* Sector of the next extended partition */
  uint32 start_ext_partition; /* LBA of the start of extended partition. */
  uint8 parsing_extended; /* Boolean, true when parsing, flase when done
                              parsing or invalid extended partition. */

  uint8  current_index; /* Wich of the 4 MBR entries are we parsing. */
  uint8  next_index; /* Wich of the 4 MBR entries are we parsing next. */
  uint64 mbr_sector; /* Sector we are currently parsing */

  struct hotplug_mbr_partition_data partition_data;

  struct hotplug_sector_cache *data_cache; /* Holds the raw data from sectors
                                            * read from the device. */

};

static HOTPLUG_MBR_ITER _mbr_iter;

HOTPLUG_MBR_ITER *
hotplug_mbr_parse_open (struct hotplug_sector_cache *data_cache)
{
  HOTPLUG_MBR_ITER *mbr_iter;
  int result;

  mbr_iter = &_mbr_iter;
  mbr_iter->data_cache = data_cache;

  /* Check if there is a valid MBR. */
  result = check_mbr_signature (mbr_iter->data_cache);

  if (result != 0)
    return NULL;

  mbr_iter->mbr_sector = 0;
  mbr_iter->current_index = 0;
  mbr_iter->next_index = 0;

  return mbr_iter;
}

struct hotplug_mbr_partition_data *
hotplug_mbr_read_primary (HOTPLUG_MBR_ITER *mbr_iter)
{
  uint8 *buffer;
  struct hotplug_mbr_layout *mbr;
  struct hotplug_mbr_partition_record *precord;
  uint32 index;

  if (mbr_iter == NULL)
    return NULL;

  /* Check if we have parsed all the partitions. */
  if (mbr_iter->next_index >= 4)
    return NULL;

  buffer = pparser_read_lba (mbr_iter->mbr_sector, 1, mbr_iter->data_cache);
  if (buffer == NULL)
    return NULL;

  mbr = (struct hotplug_mbr_layout *)buffer;

  index = mbr_iter->next_index;

  precord = &mbr->records[index];
  while (precord->partition_type == 0)
  {
    index++;
    /* Check if we have parsed all the primary partitions. */
    if (index >= 4)
      return NULL;
    precord = &mbr->records[index];
  }

  mbr_iter->current_index = index;
  mbr_iter->next_index = index + 1;

  /* Copy the data over from the partition record. */
  mbr_iter->partition_data.bootable = precord->bootable;
  mbr_iter->partition_data.partition_type = precord->partition_type;
  mbr_iter->partition_data.start_lba = precord->start_lba;
  mbr_iter->partition_data.size_in_blocks = precord->size_in_blocks;

  return &mbr_iter->partition_data;
}

int hotplug_mbr_check_extended (HOTPLUG_MBR_ITER *mbr_iter)
{
  uint8 *buffer;
  struct hotplug_mbr_layout *mbr;
  struct hotplug_mbr_partition_record *precord;
  uint32 index;

  if (mbr_iter == NULL)
    return -1;

  mbr_iter->parsing_extended = 0;

  /* Check if we have parsed all the partitions. */
  if (mbr_iter->current_index >= 4)
    return -1;

  buffer = pparser_read_lba (mbr_iter->mbr_sector, 1, mbr_iter->data_cache);
  if (buffer == NULL)
    return -1;

  mbr = (struct hotplug_mbr_layout *)buffer;

  index = mbr_iter->current_index;
  precord = &mbr->records[index];

  /* if this is not an extended partition, return error. */
  if (precord->partition_type != HOTPLUG_MBR_DOS_EXTENDED_PARTITION &&
      precord->partition_type != HOTPLUG_MBR_WIN_EXTENDED_PARTITION)
    return -1;

  /* set up the iterator to iterate these extended partitions. */
  mbr_iter->next_ext_sector = precord->start_lba;
  mbr_iter->start_ext_partition = precord->start_lba;
  mbr_iter->parsing_extended = 1;

  return 0;
}

struct hotplug_mbr_partition_data *
hotplug_mbr_read_extended (HOTPLUG_MBR_ITER *mbr_iter)
{
  uint8 *buffer;
  struct hotplug_mbr_layout *ebr;
  struct hotplug_mbr_partition_record *precord;

  if (mbr_iter == NULL)
    return NULL;

  /* Check if we are parsing extended partitions. */
  if (mbr_iter->parsing_extended == 0)
    return NULL;

  buffer = pparser_read_lba ((uint64) mbr_iter->next_ext_sector, 1,
                             mbr_iter->data_cache);
  if (buffer == NULL)
    goto done_parsing_extended;

  ebr = (struct hotplug_mbr_layout *)buffer;
  /* Check if valid Extended Boot Record (EBR) */
  if (ebr->signature[0] != 0x55 || ebr->signature[1] != 0xAA)
    goto done_parsing_extended;

  /* Validate the partition record */
  precord = &ebr->records[0];
  if (precord->partition_type == 0 || precord->size_in_blocks == 0)
    goto done_parsing_extended;

  /* Copy the data over from the partition record. */
  mbr_iter->partition_data.bootable = precord->bootable;
  mbr_iter->partition_data.partition_type = precord->partition_type;
  mbr_iter->partition_data.start_lba = precord->start_lba +
                                       mbr_iter->next_ext_sector;
  mbr_iter->partition_data.size_in_blocks = precord->size_in_blocks;

  /* Setup the next extended partition record. */
  precord = &ebr->records[1];

  /* If the next partition record is empty, quit on next call. */
  if (precord->size_in_blocks == 0 ||
      precord->partition_type != HOTPLUG_MBR_DOS_EXTENDED_PARTITION)
    mbr_iter->parsing_extended = 0;
  else
    mbr_iter->next_ext_sector = precord->start_lba +
                                mbr_iter->start_ext_partition;

  return &mbr_iter->partition_data;

done_parsing_extended:
  mbr_iter->parsing_extended = 0;
  return NULL;
}


int hotplug_mbr_parse_close (HOTPLUG_MBR_ITER *mbr_iter)
{
  if (mbr_iter == NULL)
    return -1;

  return 0;
}

static int
check_fat_signature (struct hotplug_sector_cache *data_cache)
{
  struct hotplug_mbr_layout *mbr;
  uint8 *buffer;

  buffer = pparser_read_lba (0ULL, 1, data_cache);
  if (buffer == NULL)
    return -1;

  mbr = (struct hotplug_mbr_layout *) buffer;
  if (mbr->signature[0] != 0x55 || mbr->signature[1] != 0xAA)
    return -1;

  /* Success if this is a FAT boot recrod. */
  if (mbr->boot_code[11] == 0x00 && mbr->boot_code[12] == 0x02)
    return 0;

  return -1;
}

int
hotplug_fat_parse_check (struct hotplug_sector_cache *data_cache)
{
  int result;

  result = check_fat_signature (data_cache);

  if (result != 0)
    return -1;

  /* FAT exists instead of MBR, return success */
  return 0;
}

#endif /* FEATURE_FS_HOTPLUG */
