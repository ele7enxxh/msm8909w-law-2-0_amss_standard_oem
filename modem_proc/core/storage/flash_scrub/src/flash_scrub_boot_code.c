/***********************************************************************
 * flash_scrub_boot_code.c
 *
 * Scrub image read by PBL.
 *
 * Copyright (C) 2013-2014 QUALCOMM Technologies, Inc.
 *
 * In PBL (or boot ROM) supports reading the boot image from anywhere
 * in the first 15 blocks. The SBL image PBL loads is usually no more than 2
 * blocks. This feature allows the image to be relocated/reprogrammed
 * somewhere in disk within the partition.
 *
 * The requirements of PBL to handle power down safe is that the last page
 * of the first block of the image must be the last page to be programmed.
 * For this reason, the images are written from the last block to the first
 * block. Within a block the data is written sequentially. This guarantees
 * gracefully handling sudden power downs.
 *
 * PBL reads the sectors sequentially. In the diagram below, it will load
 * SBL_A, where SBL_A spans two blocks.
 *  ------------------------------------------------------------------
 * | SBL_A_1 | SBL_A_2 | erased | erased | erased | SBL_B_1 | SBL_B_2 |
 *  ------------------------------------------------------------------
 *
 * Blocks 0 and 1 have to be erased in order to boot from SBL_B. The diagram
 * above will be condensed to:
 * A-E-B: SBL_A followed by erased blocks, then SBL_B.
 * A-E-E: SBL_A all other blocks are erased.
 * E-E-B: SBL_B is at the end of the device, all other blocks are erased.
 *
 * This module can be viewed as a state machine of 2 states. The state is
 * stored in EFS and read back on power up, that way the operation can be
 * resumed. States are:
 * 1. Ready: Finished with previous operation. Ready to initiate the next.
 * 2. Erase oldest SBL copy: Two copies of SBL exist, Erase the oldest.
 *
 * Apart from the 2 states of the state machine, the state contains the
 * block number the newest SBL image and the location of the latest SBL
 * image: Low or High. The location 'Low' means that the next scrub the
 * image needs to be copied to a 'High' block number
 *
 * The 2 states of the machine are cycled in order. The overall algorithm is:
 *
 * 1. State 'Ready', Location 'Low'. (A-E-E)
 * 2. Scrub initiated.
 * 3. Copy the blocks from the beginning of the partition to the end. (A-E-B)
 * 4. Save the state as 'Erase Oldest'. (A-E-B)
 * 5. Erase the first SBL copy.
 * 6. Save state as 'Ready', and location as 'High'. (E-E-B)
 * 7. Scrub initiated.
 * 8. Copy SBL from the end of the device to the beginning.
 * 9. Save the state 'Erase Oldest' (A-E-B)
 * 10. Erase the last copy of SBL.
 * 11. Save state as 'Ready', and location as 'Low'. (A-E-E)
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_boot_code.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-05-21   wek   Initialize variable to get around bogus compiler warning.
2014-04-28    vm   Use correct MSG macros to save RAM.
2013-12-23   wek   Change the preamble size to 8k.
2013-12-09   wek   Add macro to print F3 messages and log to efs file.
2013-10-29   dks   Fix compiler issues.
2013-09-10   wek   Create

===========================================================================*/

#include <stdint.h>
#include "comdef.h"
#include "fs_lib.h"
#include "fs_public.h"
#include "assert.h"

#include "flash_scrub_boot_code.h"
#include "flash_scrub_err.h"
#include "flash_scrub_flash_dal_shim.h"
#include "flash_scrub_log.h"
#include "flash_scrub_util.h"

#define SCRUB_BOOT_STATE_VERSON    (1)
#define SCRUB_BOOT_STATE_MAGIC     "ScrubB"
#define SCRUB_BOOT_PARTITION_NAME  "0:SBL"
#define FLASH_SCRUB_BOOT_MD5_PATH  FLASH_SCRUB_MD5_DIRECTORY "/" \
                                   SCRUB_BOOT_PARTITION_NAME
#define FLASH_SCRUB_MD5_HASH_SIZE  16


/* Boot places a magic preamble byte sequence at specific offsets.
 * The rest of the page will be filled with 0xFF. This 12 byte sequence
 * has 2 parts that this code is concerned about:
 * - Bytes 0-7:  Magic sequence, common for all block sizes
 * - Bytes 8-11: Magic sequence that determines the block size.
 *
 * The preamble is a total of 10Kbytes in size and it contains the magic
 * byte sequence at specific offsets. This offset is typically the minimum
 * page size the device supports. If the minimum page size is 2K, the
 * magic byte sequence will be at 0 bytes, 2048, 4096, 6144 and 8192.
 */

#define SCRUB_BOOT_MAGIC_PREAMBLE1          0x844BDCD1
#define SCRUB_BOOT_MAGIC_PREAMBLE2          0x73D71034
#define SCRUB_BOOT_MAGIC_PREAMBLE_DEFAULT   0x7D0B435A
#define SCRUB_BOOT_MAGIC_PREAMBLE_64        0x7D0B5436
#define SCRUB_BOOT_MAGIC_PREAMBLE_128       0x7D0B6577

#define SCRUB_BOOT_PREAMBLE_SIZE_BYTES      (8 * 1024)

/* The magic byte sequence will be found every 2048 bytes of the first 10K.*/
#define SCRUB_BOOT_PREAMBLE_MAGIC_OFFSET  (2048)

/* State of scrubbing Boot code. */
enum scrub_boot_states
{
  SCRUB_BOOT_STATE_READY, /* Scrub complete, ready for next scrub. */
  SCRUB_BOOT_STATE_ERASE_OLDEST,/* There are 2 SBL images, pending erase of
                                 * the oldest image */
  SCRUB_BOOT_STATE_FIRST_SCRUB, /* No scrub has happened and preamble needs
                                 * to be changed. */
  SCRUB_BOOT_STATE_MAX,  /* For bounds checking. */
};

/* Location of the boot image, either high or low. */
enum scrub_boot_location
{
  SCRUB_BOOT_LOCATION_LOW, /* Latest boot image resides in the low blocks */
  SCRUB_BOOT_LOCATION_HIGH,/* Latest boot image resides in the high blocks */
  SCRUB_BOOT_LOCATION_MAX, /* For bounds checking. */
};

struct scrub_boot_state
{
  uint8  version;
  uint8  magic[6];/* Magic string, set to "ScrubB" */
  uint8  state;   /* See enum scrub_boot_states */
  uint32 scrub_count; /* Number last successful scrub, given by master. */
  uint32 boot_size_blocks; /* Size in blocks of the SBL image, typically<3 */
  uint32 boot_start_block; /* Start block of the latest SBL image. */
  uint32 erase_start_block; /* Start block of SBL image to erase. */
  uint8  boot_img_location; /* see enum scrub_boot_location */
  uint8  err_cnt_efs;    /* Number of errors in accessing EFS. */
  uint8  err_cnt_flash;  /* Number of errors in accessing flash. */
};

struct flash_scrub_boot_handle
{
  struct scrub_boot_state state;
  flash_scrub_shim_handle flash_handle;
  uint8 *page_buffer;
  uint8 state_valid;
};

FLASH_SCRUB_ALIGN static uint8 page_buffer_inst[FLASH_SCRUB_MAX_PAGE_SIZE];
struct flash_scrub_boot_handle flash_scrub_boot_instance;

static void
scrub_boot_partition_close_dev (FLASH_SCRUB_BOOT_HANDLE *handle)
{
  if (handle->flash_handle != NULL)
  {
    flash_scrub_shim_close_partition (handle->flash_handle);
    handle->flash_handle = NULL;
  }
}

/* Open the partition that contains the boot image. */
static int
scrub_boot_partition_open_dev (FLASH_SCRUB_BOOT_HANDLE *handle)
{
  flash_scrub_shim_handle flash_dev;
  uint32 size;

  flash_dev = flash_scrub_shim_open_partition (SCRUB_BOOT_PARTITION_NAME);
  if (flash_dev == NULL)
  {
    FLASH_SCRUB_LOG_ERR_0 ("boot: failed to open SBL");
    return FLASH_SCRUB_EDEVREAD;
  }

  handle->flash_handle = flash_dev;

  size = flash_scrub_shim_get_block_count (handle->flash_handle);

  /* If the partition is zero bytes long is as good as it does not exist. */
  if (size == 0)
  {
    scrub_boot_partition_close_dev (handle);
    return FLASH_SCRUB_EDEVREAD;
  }

  return 0;
}

/* Keep track of the number of EFS errors that have occurred. */
static void
scrub_boot_inc_efs_err (FLASH_SCRUB_BOOT_HANDLE *handle)
{
  uint8 err = handle->state.err_cnt_efs;

  if (err < UINT8_MAX)
  {
    handle->state.err_cnt_efs++;
  }
}

/* Keep track of the number of Flash errors that have occurred. */
static void
scrub_boot_inc_flash_err (FLASH_SCRUB_BOOT_HANDLE *handle)
{
  uint8 err = handle->state.err_cnt_flash;

  if (err < UINT8_MAX)
  {
    handle->state.err_cnt_flash++;
  }
}

/* Commit the state of the Scrub Boot to the file system.
 *
 * If writing the state to the file system fails there isn't much to do.
 * The state of the SBL image in FLASH will be out of sync with the state
 * stored in EFS. If the state in FLASH goes out of sync with the state
 * stored in EFS, then scrubbing boot code will repeat the same operation
 * until it can successfully write to EFS. Not storing the next state
 * of the scrub boot cycle will have the same end result as if no
 * scrubbing happened: Data in NAND will degrade beyond repair. */
static int
scrub_boot_commit_state (FLASH_SCRUB_BOOT_HANDLE *handle)
{
  int result;
  result = efs_put (FLASH_SCRUB_BOOT_STATE_FILE, &handle->state,
                    sizeof (struct scrub_boot_state),
                    O_CREAT | O_AUTODIR, 0644);
  if (result != 0)
  {
    FLASH_SCRUB_LOG_ERR_1 ("[%d] efs_put scrubb failed.", efs_errno);
    scrub_boot_inc_efs_err (handle);
    return FLASH_SCRUB_EEFS;
  }

  return 0;
}

#ifdef FEATURE_FLASH_SCRUB_BOOT_SKIP_PREAMBLE_UPDATE

static int
scrub_boot_update_preamble_in_buffer (uint8 *buf, uint32 block_size)
{
  (void) buf;
  (void) block_size;
  return 0;
}

#else

static int
scrub_boot_update_preamble_in_buffer (uint8 *buf, uint32 block_size)
{
  uint32 *buffer = (uint32 *)buf;
  uint32 *pream;

  /* Check if the buffer starts with the first 8 bytes of magic preamble. */
  if (*buffer == SCRUB_BOOT_MAGIC_PREAMBLE1 &&
      *(buffer + 1) == SCRUB_BOOT_MAGIC_PREAMBLE2)
  {
    pream = (buffer + 2);
    switch (block_size)
    {
    case 64:
      *pream = SCRUB_BOOT_MAGIC_PREAMBLE_64;
      break;
    case 128:
      *pream = SCRUB_BOOT_MAGIC_PREAMBLE_128;
      break;
    default:
      FLASH_SCRUB_LOG_ERR_1 ("[%d]boot: Unsupported block size",
                           block_size);
      FLASH_SCRUB_DEBUG_ERR_FATAL ("[%d, %d] Unsupported block size.",
                                   block_size, 0, 0);
      *pream = SCRUB_BOOT_MAGIC_PREAMBLE_DEFAULT;
      break;
    }

    return 0;
  }

  return -1;
}

#endif /* FEATURE_FLASH_SCRUB_BOOT_SKIP_PREAMBLE_UPDATE */

static int
scrub_boot_write_page (FLASH_SCRUB_BOOT_HANDLE *handle, uint8 *buffer,
                       uint32 block_num, uint32 page_num)
{
  int result = 0;
  flash_scrub_shim_handle flash_handle = handle->flash_handle;

  /* Write the data into destination. */
  result = flash_scrub_shim_write_page (flash_handle, buffer,
                                        block_num,    page_num);

  /* If it fails, erase the destination block and mark it as bad.*/
  if (result != FLASH_DEVICE_DONE)
  {
    flash_scrub_shim_erase_block (flash_handle, block_num);
    flash_scrub_shim_mark_block_bad (flash_handle, block_num);
    result = FLASH_SCRUB_EDEVWRITE;
  }

  return result;
}

static int
scrub_boot_copy_first_block (FLASH_SCRUB_BOOT_HANDLE *handle,
                             uint32 src_blk, uint32 dest_blk)
{
  int result;
  uint32 curr_page, page_size, block_size, last_written_page = 0;
  uint32 pindex; /* preamble index, how much of the 10K have we processed. */
  uint32 offset;
  int copy_count = 0, is_bad_block;
  flash_scrub_shim_handle flash_handle = handle->flash_handle;
  uint8 *buffer = handle->page_buffer;

  is_bad_block = flash_scrub_shim_is_block_bad (flash_handle, dest_blk);
  if (is_bad_block)
    return FLASH_SCRUB_EDEVWRITE;

  is_bad_block = flash_scrub_shim_is_block_bad (flash_handle, src_blk);
  if (is_bad_block)
    return FLASH_SCRUB_EBADBLOCK;

  /* Get and validate the page size. */
  page_size = flash_scrub_shim_get_page_size (flash_handle);
  if (page_size > FLASH_SCRUB_MAX_PAGE_SIZE)
  {
    FLASH_SCRUB_DEBUG_ERR_FATAL ("[%d, %d] Page size too large.",
                                 page_size, FLASH_SCRUB_MAX_PAGE_SIZE, 0);
    return FLASH_SCRUB_ECONFIG;
  }

  /* Get the number of pages in a block. */
  block_size = flash_scrub_shim_get_block_size (flash_handle);

  result = flash_scrub_shim_erase_block (flash_handle, dest_blk);
  if (result != FLASH_DEVICE_DONE)
  {
    FLASH_SCRUB_MSG_ERR_2("[%d] Failed to erase block %d", result, dest_blk);
    FLASH_SCRUB_DEBUG_ERR_FATAL ("[%d, %d, %d] erase block failed.",
                                 result, dest_blk, 0);
    flash_scrub_shim_mark_block_bad (flash_handle, dest_blk);
    return FLASH_SCRUB_EDEVERASE;
  }

  /* Now start copying the data over to the destination.
   * Start with the first 10K is preamble and requires special handling */
  curr_page = 0;
  for (pindex = 0; pindex < SCRUB_BOOT_PREAMBLE_SIZE_BYTES;
       pindex += page_size)
  {
    result = flash_scrub_shim_read_page (flash_handle, buffer, src_blk,
                                         curr_page);
    if (result != FLASH_DEVICE_DONE)
    {
      result = FLASH_SCRUB_EDEVREAD;
      goto error_cleanup;
    }

    /* In this page update the magic byte sequence. */
    for (offset = 0; offset < page_size &&
                    (pindex + offset) < SCRUB_BOOT_PREAMBLE_SIZE_BYTES;
         offset += SCRUB_BOOT_PREAMBLE_MAGIC_OFFSET)
    {
      result = scrub_boot_update_preamble_in_buffer (buffer + offset,
                                                     block_size);
      if (result != 0)
      {
        result = FLASH_SCRUB_EDEVREAD;
        goto error_cleanup;
      }
    }

    result = scrub_boot_write_page (handle, buffer,
                                    dest_blk, curr_page);
    if (result != 0)
    {
      /* If write fails, it would already erased the block, no need to
       * re-erase it */
      goto cleanup;
    }

    /* Read the next page in the sequence. */
    curr_page++;
    copy_count++;
  }

  /* Copy the rest of the block. Current page continues from the previous
   * count.*/
  for(;curr_page < block_size; curr_page++)
  {
    /* Read one page from source handle */
    result = flash_scrub_shim_read_page (flash_handle, buffer, src_blk,
                                         curr_page);
    /* On error, skip to the next page. */
    if (result != FLASH_DEVICE_DONE)
    {
      if (result != FLASH_DEVICE_FAIL_PAGE_ERASED)
      {
        FLASH_SCRUB_DEBUG_ERR_FATAL ("[%d,%d,%d] read failed.",
                                     result, src_blk, curr_page);
      }

      continue;
    }

    result = scrub_boot_write_page (handle, buffer, dest_blk, curr_page);
    if (result != 0)
    {
      /* If write fails, it would already erased the block, no need to
       * re-erase it */
      goto cleanup;
    }

    copy_count++;
    last_written_page = curr_page;
  }

  /* If SBL image is smaller than one block, write some data to the last
   * page of the block. */
  if (last_written_page != block_size - 1)
  {
    memset (buffer, 0x7E, page_size);
    result = scrub_boot_write_page (handle, buffer, dest_blk, block_size - 1);
    if (result != 0)
    {
      /* If write fails, it would already erased the block, no need to
       * re-erase it */
      goto cleanup;
    }
  }

  return copy_count;

error_cleanup:
  /* On failure erase the destination block. */
  if (flash_scrub_shim_erase_block (flash_handle, dest_blk) !=
      FLASH_DEVICE_DONE)
  {
    flash_scrub_shim_mark_block_bad (flash_handle, dest_blk);
  }

cleanup:
  return result;

}

static int
scrub_boot_copy_block (FLASH_SCRUB_BOOT_HANDLE *handle,
                       uint32 src_block, uint32 dest_block)
{
  int result;

  /* The first block of the image has the magic preamble and needs special
   * handling. */
  if (handle->state.state == SCRUB_BOOT_STATE_FIRST_SCRUB &&
      src_block == handle->state.boot_start_block)
  {
    result = scrub_boot_copy_first_block (handle, src_block, dest_block);
  }
  else
  {
    result = flash_scrub_copy_block (handle->flash_handle, src_block,
                                     handle->flash_handle, dest_block);
  }

  return result;
}

/* Reads the MD5 stored in the file and verifies that it matches the one
 * calculated using the data in NAND. */
static int
scrub_boot_verify_md5 (FLASH_SCRUB_BOOT_HANDLE *handle, uint32 start_block,
                       uint32 end_block)
{
  int result;
  MD5_CTX  md5_ctx;
  uint8 md5_now[FLASH_SCRUB_MD5_HASH_SIZE];
  uint8 md5_prev[FLASH_SCRUB_MD5_HASH_SIZE];

  /* Get the MD5 form before we started scrubbing. */
  result = efs_get (FLASH_SCRUB_BOOT_MD5_PATH, md5_prev,
                    sizeof (md5_prev));
  /* The file must of been created earlier if it didn't exit. */
  if (result != sizeof (md5_prev))
  {
    FLASH_SCRUB_ERR_FATAL ("[%d] Boot MD5 file was deleted.", efs_errno,0,0);
  }

  /* Compute the MD5 on the new image. */
  flash_scrub_util_compute_md5 (handle->flash_handle, start_block, end_block,
                                &md5_ctx, md5_now);

  /* Compare the known to be good MD5 (md5_prev) with the one calculated from
   * the new image. */
  result = memcmp (md5_now, md5_prev, sizeof (md5_prev));
  if (result != 0)
  {
    FLASH_SCRUB_LOG_ERR_0 ("boot:MD5 changed at run time");
    result = FLASH_SCRUB_EMD5;
  }

  return result;
}


/* Calculates and updates the MD5 of the SBL image. */
static int
scrub_boot_update_md5 (FLASH_SCRUB_BOOT_HANDLE *handle, uint32 start_block,
                       uint32 end_block)
{
  int result;
  MD5_CTX  md5_ctx;
  uint8 md5_hash[FLASH_SCRUB_MD5_HASH_SIZE];

  /* Compute the MD5 on the new image. */
  flash_scrub_util_compute_md5 (handle->flash_handle, start_block, end_block,
                                &md5_ctx, md5_hash);

  result = efs_put (FLASH_SCRUB_BOOT_MD5_PATH, md5_hash, sizeof (md5_hash),
                    (O_CREAT | O_AUTODIR), 0644);
  if (result < 0)
  {
    FLASH_SCRUB_ERR_FATAL ("%d, put on boot md5 failed.", efs_errno, 0, 0);
  }

  return 0;
}

/* Tries to erase all the blocks between start_block and end_block (both
 * inclusive). */
static void
scrub_boot_erase_good_blocks (FLASH_SCRUB_BOOT_HANDLE *handle,
                              uint32 start_block, uint32 end_block)
{
  int result;
  uint32 block;
  flash_scrub_shim_handle flash_handle = handle->flash_handle;


  for (block = start_block; block <= end_block; block++)
  {
    result = flash_scrub_shim_is_block_bad (flash_handle, block);
    if (result == 0)
    {
      result = flash_scrub_shim_erase_block (flash_handle, block);
      if (result != FLASH_DEVICE_DONE)
      {
        FLASH_SCRUB_MSG_ERR_2 ("[%d] Failed to erase block %d",
                             result, block);
        scrub_boot_inc_flash_err (handle);
        flash_scrub_shim_mark_block_bad (flash_handle, block);
      }
    }
  }
}

/* Returns the next good block after "block_num", it does not check
 * block_num. */
static int
move_back_to_good_block (FLASH_SCRUB_BOOT_HANDLE *handle, uint32 block_num,
                         uint32 *good_block)
{
  int result;

  do
  {
    if (block_num == 0)
      return -1;

    block_num--;

    result = flash_scrub_shim_is_block_bad (handle->flash_handle, block_num);
  } while (result != 0);

  *good_block = block_num;
  return 0;
}

static int
move_forward_n_good_blocks (FLASH_SCRUB_BOOT_HANDLE *handle,
                            uint32 block_start, uint32 block_count,
                            uint32 block_limit, uint32 *out_block)
{
  uint32 block_num;
  int result;

  for (block_num = block_start; block_num < block_limit; block_num++)
  {
    result = flash_scrub_shim_is_block_bad (handle->flash_handle, block_num);
    if (result == 0)
    {
      block_count--;
      if (block_count == 0)
      {
        *out_block = block_num;
        return 0;
      }
    }
  }

  return FLASH_SCRUB_ENOSPC;
}


static int
scrub_boot_copy_low_to_high (FLASH_SCRUB_BOOT_HANDLE *handle)
{
  uint32 src_block, src_start, src_end, dest_block, new_end, new_start;
  uint32 img_size_blk;
  int result;

  src_start = handle->state.boot_start_block;
  dest_block = flash_scrub_shim_get_block_count (handle->flash_handle) - 1;
  img_size_blk = handle->state.boot_size_blocks;

  /* Get the last block of the image. */
  result = move_forward_n_good_blocks (handle, src_start, img_size_blk,
                                       dest_block, &src_block);
  if (result != 0)
  {
    FLASH_SCRUB_LOG_ERR_3 ("[%d,%d,%d] boot: end of image not found.",
                         src_start, img_size_blk, dest_block);
    return FLASH_SCRUB_EBADBLOCK;
  }
  src_end = src_block;
  new_start = new_end = dest_block;

  while (img_size_blk > 0)
  {
    if (dest_block <= src_end)
    {
      result = FLASH_SCRUB_ENOSPC;
      goto cleanup;
    }

    result = scrub_boot_copy_block (handle, src_block, dest_block);

    if (result > 0)
    {
      /* Record the start and end blocks of the new image */
      new_start = dest_block;

      img_size_blk--;
      if (img_size_blk > 0)
      {
        result = move_back_to_good_block (handle, src_block, &src_block);
        if (result != 0)
        {
          result = FLASH_SCRUB_EBADBLOCK;
          goto cleanup;
        }
      }
    }

    dest_block--;
  }

  /* The first ever scrub we have to update the MD5, since the preamble
   * changed. */
  if (handle->state.state == SCRUB_BOOT_STATE_FIRST_SCRUB)
    result = scrub_boot_update_md5 (handle, new_start, new_end + 1);
  else
    result = scrub_boot_verify_md5 (handle, new_start, new_end + 1);

cleanup:
  if (result != 0)
  {
    FLASH_SCRUB_LOG_ERR_3 ("[%d,%d,%d] boot: error boot to high",
                         result, src_block, new_start);
    scrub_boot_erase_good_blocks (handle, new_start, new_end);
  }
  else
  {
    handle->state.erase_start_block = handle->state.boot_start_block;
    handle->state.boot_start_block = new_start;
  }

  return result;
}

static int
scrub_boot_copy_high_to_low_single (FLASH_SCRUB_BOOT_HANDLE *handle)
{

  uint32 src_block, src_start, dest_block, new_end, new_start;
  uint32 img_size_blk, block_count, parti_size;
  int result;

  src_start = handle->state.boot_start_block;
  img_size_blk = handle->state.boot_size_blocks;

  parti_size = flash_scrub_shim_get_block_count (handle->flash_handle);
  /* Get the last block of the source image. */
  result = move_forward_n_good_blocks (handle, src_start, img_size_blk,
                                       parti_size, &src_block);
  if (result != 0)
  {
    return FLASH_SCRUB_ENOSPC;
  }

  /* Get what will be the last block of the new image. */
  result = move_forward_n_good_blocks (handle, 0, img_size_blk, src_start,
                                       &dest_block);
  if (result != 0)
  {
    return FLASH_SCRUB_ENOSPC;
  }

  new_start = new_end = dest_block;

  for (block_count = img_size_blk; block_count > 0; block_count--)
  {
    result = flash_scrub_copy_block (handle->flash_handle, src_block,
                                     handle->flash_handle, dest_block);
    if (result <= 0)
    {
      result = FLASH_SCRUB_EAGAIN;
      goto cleanup;
    }

    new_start = dest_block;

    /* If there are more blocks to process, move the source and destination
     * blocks to the next good blocks. */
    if (block_count > 1)
    {
      /* Block successfully copied, update source and destination for
       * the next iteration. */
      result = move_back_to_good_block (handle, src_block, &src_block);
      if (result != 0)
      {
        result = FLASH_SCRUB_EBADBLOCK;
        goto cleanup;
      }

      result = move_back_to_good_block (handle, dest_block, &dest_block);
      if (result != 0)
      {
        result = FLASH_SCRUB_EAGAIN;
        goto cleanup;
      }
    }
  }

  /* Successfully copied to the new location. Verify MD5 on new location. */
  result = scrub_boot_verify_md5 (handle, new_start, new_end + 1);

cleanup:
  if (result != 0)
  {
    FLASH_SCRUB_MSG_DEBUG_HIGH_3 ("[%d %d, %d] Scrub: error boot copy to low",
                                result, src_block, dest_block);
    scrub_boot_erase_good_blocks (handle, new_start, new_end);
  }
  else
  {
    handle->state.erase_start_block = handle->state.boot_start_block;
    handle->state.boot_start_block = new_start;
  }

  return result;
}

static int
scrub_boot_copy_high_to_low (FLASH_SCRUB_BOOT_HANDLE *handle)
{
  int result = FLASH_SCRUB_ENOSPC;
  uint32 parti_size, i;

  parti_size = flash_scrub_shim_get_block_count (handle->flash_handle);

  /* Try to copy the SBL at high block numbers to a low block numbers.
   * With every call of "copy single" we will either successfully copy
   * the image OR mark one block as bad. Attempt to do this copy at most
   * the size of the partition, since we can't mark the whole partition
   * as bad. */
  for (i = 0; i < parti_size; i++)
  {
    result = scrub_boot_copy_high_to_low_single (handle);

    /* If the error is "EAGAIN" try to copy the image again. On success or
     * any other error, return. */
    if (result != FLASH_SCRUB_EAGAIN)
      break;
  }

  return result;
}

static int
scrub_boot_refresh_verify_image (FLASH_SCRUB_BOOT_HANDLE *handle)
{
  struct scrub_boot_state *state = &handle->state;
  int retry_count = FLASH_SCRUB_MD5_MAX_RETRY_COUNT;
  int result;

  do
  {
    if (state->boot_img_location == SCRUB_BOOT_LOCATION_LOW)
    {
      /* Scrub/verify SBL image residing in the low number blocks. */
      result = scrub_boot_copy_low_to_high (handle);
    }
    else /* (state->boot_img_location == SCRUB_BOOT_LOCATION_HIGH) */
    {
      /* Scrub/verify SBL image residing in the high number blocks. */
      result = scrub_boot_copy_high_to_low (handle);
    }

    retry_count--;
  } while (result == FLASH_SCRUB_EMD5 && retry_count > 0);

  return result;
}

/* Erases the blocks where the old boot image existed, skipping bad blocks.
 * If erase fails, mark the block as bad. */
static void
scrub_boot_erase_old_sbl (FLASH_SCRUB_BOOT_HANDLE *handle)
{
  int block_count, result;
  uint32 block_num, stop_block;
  flash_scrub_shim_handle flash_handle;
  struct scrub_boot_state *state = &handle->state;

  block_num = handle->state.erase_start_block;
  block_count = handle->state.boot_size_blocks;

  flash_handle = handle->flash_handle;

  if (state->boot_img_location == SCRUB_BOOT_LOCATION_LOW)
    stop_block = flash_scrub_shim_get_block_count (flash_handle);
  else /* (state->boot_img_location == SCRUB_BOOT_LOCATION_HIGH) */
    stop_block = handle->state.boot_start_block;

  FLASH_SCRUB_MSG_DEBUG_HIGH_2 ("[%d, %d] Scrub: boot erase sbl",
                              block_num, block_count);

  for (; block_num < stop_block; block_num++)
  {
    /* Check if the block is good, if it is erase it. */
    result = flash_scrub_shim_is_block_bad (flash_handle, block_num);
    if (result == 0)
    {
      result = flash_scrub_shim_erase_block (flash_handle, block_num);
      if (result != FLASH_DEVICE_DONE)
      {
        FLASH_SCRUB_MSG_ERR_2 ("[%d] Failed to erase block %d",
                             result, block_num);
        scrub_boot_inc_flash_err (handle);
        flash_scrub_shim_mark_block_bad (flash_handle, block_num);
      }

      block_count--;
      if (block_count == 0)
        break;
    }
  }

  return;
}

int
flash_scrub_boot (FLASH_SCRUB_BOOT_HANDLE *handle, uint32 scrub_count)
{
  struct scrub_boot_state *state = &handle->state;
  int result;

  if (handle->state_valid == 0)
  {
    FLASH_SCRUB_MSG_ERR_0 ("Failed to find the SBL image, reboot to retry");
    return FLASH_SCRUB_EDEVREAD;
  }

  /* If the same token number from previous call and the state is READY,
   * then there is no scrubbing needed. */
  if (state->scrub_count == scrub_count &&
      state->state == SCRUB_BOOT_STATE_READY)
  {
    FLASH_SCRUB_MSG_ERR_2 ("[%d, %d] same token.",
                         scrub_count, state->scrub_count);
    return 0;
  }

  if (state->scrub_count != scrub_count &&
      state->state == SCRUB_BOOT_STATE_ERASE_OLDEST)
  {
    /* The state erase oldest is an intermediate state between the two idle
     * states. If the device is ever in this state is because power cycle
     * happened while before completely erasing the duplicate SBL image.
     * On reboot, the scrub master module should give the same token
     * number if it is resuming from a previous scrub. If it doesn't then
     * there is some unexpected inconsistency between scrub master and
     * scrub boot. */
    FLASH_SCRUB_MSG_ERR_2 ("[%d, %d] Inconsistent state master != boot",
                         scrub_count, state->scrub_count);
    ASSERT (0);
  }

  result = scrub_boot_partition_open_dev (handle);
  if (result != 0)
  {
    FLASH_SCRUB_MSG_ERR_3 ("[%d,%d,%d] boot partition does not exist.", result,
                         scrub_count, state->scrub_count);
    return result;
  }

  if (state->state == SCRUB_BOOT_STATE_READY ||
      state->state == SCRUB_BOOT_STATE_FIRST_SCRUB)
  {
    result = scrub_boot_refresh_verify_image (handle);

    if (result == 0)
    {
      state->state = SCRUB_BOOT_STATE_ERASE_OLDEST;

      /* Toggle the location of the SBL image. */
      if (state->boot_img_location == SCRUB_BOOT_LOCATION_LOW)
        state->boot_img_location = SCRUB_BOOT_LOCATION_HIGH;
      else /* (state->boot_img_location == SCRUB_BOOT_LOCATION_HIGH) */
        state->boot_img_location = SCRUB_BOOT_LOCATION_LOW;
    }

    state->scrub_count = scrub_count;
    scrub_boot_commit_state (handle);
  }

  if (state->state == SCRUB_BOOT_STATE_ERASE_OLDEST)
  {
    FLASH_SCRUB_MSG_DEBUG_HIGH_2 ("[%d,%d] Scrub: boot state erase",
                                  scrub_count, state->state);
    scrub_boot_erase_old_sbl (handle);

    /* Success! update the state. */
    state->state = SCRUB_BOOT_STATE_READY;

    state->scrub_count = scrub_count;
    scrub_boot_commit_state (handle);

  }

  scrub_boot_partition_close_dev (handle);

  return result;
}

static int
scrub_boot_is_valid_read_page (FLASH_SCRUB_BOOT_HANDLE *handle,
                               uint32 block_num, uint32 page_num)
{
  int is_bad_block, result;
  flash_scrub_shim_handle flash_handle = handle->flash_handle;

  is_bad_block = flash_scrub_shim_is_block_bad (flash_handle, block_num);
  if (is_bad_block != 0)
  {
    return FLASH_SCRUB_EBADBLOCK;
  }

  result = flash_scrub_shim_read_page (flash_handle, handle->page_buffer,
                                       block_num, page_num);
  if (result != FLASH_DEVICE_DONE)
  {
    return FLASH_SCRUB_EDEVREAD;
  }

  return 0;
}

/* Returns 0 if preamble exist, -1 if it does not exist. */
static int
scrub_boot_check_preamble_in_buffer (uint8 *buf)
{
  uint32 *buffer = (uint32 *)buf;
  uint32 pream;

  /* Check if the buffer starts with the first 8 bytes of magic preamble. */
  if (*buffer == SCRUB_BOOT_MAGIC_PREAMBLE1 &&
      *(buffer + 1) == SCRUB_BOOT_MAGIC_PREAMBLE2)
  {
    pream = *(buffer + 2);
    /* Now check if the next 4 bytes is one of the valid ones. */
    if (pream == SCRUB_BOOT_MAGIC_PREAMBLE_DEFAULT ||
        pream == SCRUB_BOOT_MAGIC_PREAMBLE_64      ||
        pream == SCRUB_BOOT_MAGIC_PREAMBLE_128)
    {
      return 0;
    }
  }

  return -1;
}

static int
scrub_boot_check_preamble_in_block (FLASH_SCRUB_BOOT_HANDLE *handle,
                                    uint32 block_num)
{
  int result;
  uint8 *buffer = handle->page_buffer;
  flash_scrub_shim_handle flash_handle = handle->flash_handle;

  uint32 pindex = 0; /* Index inside the 10K preamble. */
  uint32 offset = 0; /* Offset inside the page buffer. */
  uint32 page_num = 0; /* Page number being read inside the block. */
  uint32 page_size; /* Size in bytes of one page. */

  page_size = flash_scrub_shim_get_page_size (flash_handle);

  /* Check if the magic preamble exist at the defined offset. */
  for (pindex = 0; pindex < SCRUB_BOOT_PREAMBLE_SIZE_BYTES;
       pindex += page_size)
  {
    result = flash_scrub_shim_read_page (flash_handle, buffer, block_num,
                                         page_num);
    if (result != FLASH_DEVICE_DONE)
      return -1;

    /* In this page check for the presence of the magic byte sequence. */
    for (offset = 0; offset < page_size &&
                    (pindex + offset) < SCRUB_BOOT_PREAMBLE_SIZE_BYTES;
         offset += SCRUB_BOOT_PREAMBLE_MAGIC_OFFSET)
    {
      result = scrub_boot_check_preamble_in_buffer (buffer + offset);
      if (result != 0)
        return -1;
    }

    /* Read the next page in the sequence. */
    page_num++;
  }

  return 0;
}


/* Find the SBL image by searching for a written block and checking for magic
 * preamble. It will populate boot_start_block, boot_size_blocks
 * erase_start_block. */
static int
scrub_boot_find_img (FLASH_SCRUB_BOOT_HANDLE *handle)
{
  int result;
  uint32 curr_block, partition_size, boot_size;
  flash_scrub_shim_handle flash_handle;

  flash_handle = flash_scrub_shim_open_partition (SCRUB_BOOT_PARTITION_NAME);
  if (flash_handle == NULL)
  {
    FLASH_SCRUB_MSG_ERR_0 ("Failed to open sbl partition");
    return FLASH_SCRUB_EDEVREAD;
  }

  handle->flash_handle = flash_handle;
  partition_size = flash_scrub_shim_get_block_count (flash_handle);

  for (curr_block = 0; curr_block < partition_size; curr_block++)
  {
    result = scrub_boot_is_valid_read_page (handle, curr_block, 0);
    /* If the block is bad or erased, skip it.*/
    if (result != 0)
      continue;

    /* Block is good, first page is written, check for preamble. */
    result = scrub_boot_check_preamble_in_block (handle, curr_block);
    if (result == 0)
      break;
  }

  if (curr_block == partition_size)
  {
    FLASH_SCRUB_MSG_ERR_2 ("[%d, %d]Did not find the SBL image",
                         curr_block, partition_size);
    result = FLASH_SCRUB_EDEVREAD;
    goto cleanup;
  }

  /* Curr_block is the block with the magic preamble. Now find how many
   * blocks are written after this. */
  handle->state.boot_start_block = curr_block;
  boot_size = 1;
  curr_block++;

  for (;curr_block < partition_size; curr_block++)
  {
    result = scrub_boot_is_valid_read_page (handle, curr_block, 0);
    /* skip bad blocks. */
    if (result == FLASH_SCRUB_EBADBLOCK)
      continue;

    /* On any other error means we've reached the end of the SBL image. */
    if (result != 0)
      break;

    boot_size++;
  }

  handle->state.boot_size_blocks = boot_size;

  /* This function assumes the SBL image is closer to the start of the
   * partition than the end of the partition. */
  handle->state.erase_start_block = partition_size - 1;

  result = 0;

cleanup:
  flash_scrub_shim_close_partition (flash_handle);
  return result;
}

static int
scrub_boot_init_state (FLASH_SCRUB_BOOT_HANDLE *handle, uint32 scrub_count)
{
  struct scrub_boot_state *state = &handle->state;
  int result;

  /* Do run time sanity checking of state values. This makes sure new code
   * does not make a mistake in the state configuration. */
  if (sizeof (state->magic) != strlen (SCRUB_BOOT_STATE_MAGIC) ||
      SCRUB_BOOT_STATE_MAX > UINT8_MAX ||
      SCRUB_BOOT_LOCATION_MAX > UINT8_MAX)
  {
    FLASH_SCRUB_ERR_FATAL ("Boot state structure misconfigured",
                           sizeof (state->magic),
                           SCRUB_BOOT_STATE_MAX, SCRUB_BOOT_LOCATION_MAX);
  }

  state->version = SCRUB_BOOT_STATE_VERSON;
  memcpy (state->magic, SCRUB_BOOT_STATE_MAGIC, sizeof (state->magic));
  state->err_cnt_flash = 0;
  state->err_cnt_efs = 0;
  state->scrub_count = scrub_count;

  state->state = SCRUB_BOOT_STATE_FIRST_SCRUB;

  /* Assume the image is at the beginning of the device. This value can be
   * deduced at run time by counting the number of good blocks before and
   * after the image. */
  state->boot_img_location = SCRUB_BOOT_LOCATION_LOW;

  result = scrub_boot_find_img (handle);
  return result;
}

static int
scrub_boot_get_state (FLASH_SCRUB_BOOT_HANDLE *handle, uint32 scrub_count)
{
  struct scrub_boot_state *state = &handle->state;
  int result;

  result = efs_get (FLASH_SCRUB_BOOT_STATE_FILE, state,sizeof (handle->state));
  if (result == sizeof (handle->state))
  {
    /* Sanity check the state file. */
    if (state->version == SCRUB_BOOT_STATE_VERSON &&
        state->state < SCRUB_BOOT_STATE_MAX &&
        state->boot_img_location < SCRUB_BOOT_LOCATION_MAX &&
        memcmp (state->magic, SCRUB_BOOT_STATE_MAGIC, sizeof (state->magic))
        == 0)
    {
      /* Fields match the build, return success. */
      return 0;
    }
  }

  result = scrub_boot_init_state (handle, scrub_count);
  if (result == 0)
  {
    result = efs_put (FLASH_SCRUB_BOOT_STATE_FILE, state,
                      sizeof (handle->state), O_CREAT | O_AUTODIR, 0644);
    if (result != 0)
    {
      FLASH_SCRUB_MSG_ERR_2 ("[%d,%d] efs_put failed in scrub boot init.",
                           efs_errno, sizeof (handle->state));
    }
  }

  return result;
}

FLASH_SCRUB_BOOT_HANDLE*
flash_scrub_boot_init (uint32 scrub_count)
{
  FLASH_SCRUB_BOOT_HANDLE *handle;
  int result;

  handle = &flash_scrub_boot_instance;
  handle->page_buffer = page_buffer_inst;
  result = scrub_boot_get_state (handle, scrub_count);
  if (result == 0)
  {
    handle->state_valid = 1;
  }
  else
  {
    handle->state_valid = 0;
  }

  return handle;
}


int
flash_scrub_boot_refresh_md5 (FLASH_SCRUB_BOOT_HANDLE *handle)
{
  int result, update_md5 = 0;
  struct scrub_boot_state *state = &handle->state;
  MD5_CTX  md5_ctx;
  uint8 md5_now[FLASH_SCRUB_MD5_HASH_SIZE];
  uint8 md5_prev[FLASH_SCRUB_MD5_HASH_SIZE];
  uint32 boot_end_block, parti_size;

  if (handle->state_valid == 0)
  {
    FLASH_SCRUB_MSG_ERR_0 ("Failed to find the SBL image, reboot to retry");
    return FLASH_SCRUB_EDEVREAD;
  }

  result = scrub_boot_partition_open_dev (handle);
  if (result != 0)
  {
    FLASH_SCRUB_MSG_ERR_0 ("Failed to open sbl partition");
    return FLASH_SCRUB_EDEVREAD;
  }

  parti_size = flash_scrub_shim_get_block_count (handle->flash_handle);
  result = move_forward_n_good_blocks (handle, state->boot_start_block,
                                       state->boot_size_blocks, parti_size,
                                       &boot_end_block);
  if (result != 0)
  {
    FLASH_SCRUB_MSG_ERR_2 ("[%d,%d] Could not find the last block of image.",
                         state->boot_start_block, state->boot_size_blocks);
    scrub_boot_partition_close_dev (handle);
    return FLASH_SCRUB_EDEVREAD;
  }

  /* The end block in MD5 API is one greater than the last block of the
   * image. For example, a one block image at block 8, would have start
   * of 8 and end of 9, but block 9 will not be used in MD5 calculation. */
  flash_scrub_util_compute_md5 (handle->flash_handle, state->boot_start_block,
                                boot_end_block + 1, &md5_ctx, md5_now);

  scrub_boot_partition_close_dev (handle);

  result = efs_get (FLASH_SCRUB_BOOT_MD5_PATH, md5_prev, sizeof(md5_prev));
  if (result == sizeof (md5_prev))
  {
    result = memcmp (md5_now, md5_prev, sizeof (md5_prev));
    if (result != 0)
    {
      FLASH_SCRUB_LOG_ERR_0 ("boot: MD5 changed at init");
      update_md5 = 1;
    }
  }
  else
  {
    update_md5 = 1;
    FLASH_SCRUB_LOG_MSG_0 ("boot: MD5 did not exist.");
  }

  if (update_md5 != 0)
  {
    result = efs_put (FLASH_SCRUB_BOOT_MD5_PATH, md5_now, sizeof (md5_now),
                      (O_CREAT | O_AUTODIR), 0644);
    if (result < 0)
    {
      FLASH_SCRUB_ERR_FATAL ("%d, put on boot md5 failed.", efs_errno, 0, 0);
    }
  }

  return 0;
}

