/****************************************************************************
 * flash_scrub_util.c
 *
 * Utility functions used by several modules.
 * Copyright (C) 2013-2014 QUALCOMM Technologies, Inc.
 *
 * Definitions of utility functions uses by different modules inside Scrub.
 *
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_util.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ----------------------------------------------------------
2016-05-02   mj    Remove function calls with ASSERT Macro.
2014-04-28   vm    Use correct MSG macros to save RAM.
2013-09-16   wek   Remove page size and block size arguments from read/write.
2013-09-08   rp    Fix bad blocks handling in code-partitions.
2013-09-05   rp    Avoid erasing bad-blocks.
2013-08-15   rp    Fix compilation error seen in target when debug is enabled.
2013-08-15   rp    Prefix flash_scrub to all non-static functions.
2013-08-15   rp    Make all target file names to start with flash_scrub.
2013-08-14   rp    Include "scrub_config_i.h" before any other includes.
2013-08-14   rp    Add debug err-fatal to catch unknown read-page failures.
2013-08-08   rp    Memory-Align flash-buffers.
2013-08-04   rp    Add debug err-fatal and debug f3-messages.
2013-08-04   rp    Add MD5-support to verify code-partition scrubbing.
2013-07-17   wek   Allow RAM buffer to be borrowed during initialization.
2013-07-09   wek   Create.

===========================================================================*/

#include "flash_scrub_config_i.h"
#include "flash_scrub_flash_dal_shim.h"
#include "flash_scrub_err.h"
#include "flash_scrub_util.h"

#include "assert.h"

#include <string.h>

FLASH_SCRUB_ALIGN static uint8 copy_buffer[FLASH_SCRUB_MAX_PAGE_SIZE];
static uint8 copy_buffer_borrowed = 0;

#ifdef SCRUB_READ_BACK_VERIFY
FLASH_SCRUB_ALIGN static uint8 verify_buffer[FLASH_SCRUB_MAX_PAGE_SIZE];

/* Given a flash device, block and page number, it compares the contents
 * of the page with the data in the supplied 'cmp_buffer' argument.
 * If the read fails or the comparison fails, err_fatal. */
static void
read_back_verify (flash_scrub_shim_handle flash_handle, uint32 block_num,
                  uint32 page_num, uint8 *cmp_buffer,
                  uint32 page_size)

{
  int result;

  /* Verifying that the copy page has succeeded */
  memset((void *)verify_buffer, 0, FLASH_SCRUB_MAX_PAGE_SIZE);
  result = flash_scrub_shim_read_page (flash_handle, verify_buffer,
                                       block_num, page_num);
  if (result != FLASH_DEVICE_DONE)
  {
    FLASH_SCRUB_ERR_FATAL ("[%d, %d, %d] Reading back failed.",
                           result, block_num, page_num);
  }

  if (memcmp (cmp_buffer, verify_buffer, page_size) != 0)
  {
    FLASH_SCRUB_ERR_FATAL ("[%d, %d] Data written != data read back.",
                           block_num, page_num, 0);
  }
}
#endif /* SCRUB_READ_BACK_VERIFY */


/* Copies the data from the source block to destination block.
 * Replicates all the data and erased pages in source block in destination.*/
int
flash_scrub_copy_block (flash_scrub_shim_handle src_handle, uint32 src_blk,
                        flash_scrub_shim_handle dest_handle, uint32 dest_blk)
{
  int result;
  uint32 curr_page, page_size, dest_page_size;
  uint32 block_size, dest_block_size;
  int copy_count = 0, is_bad_block;

  ASSERT (src_handle != NULL);
  ASSERT (dest_handle != NULL);

  /* Make sure the logic is sound and no body is using the borrowed buffer.*/
  ASSERT (copy_buffer_borrowed == 0);

  is_bad_block = flash_scrub_shim_is_block_bad (dest_handle, dest_blk);
  if (is_bad_block)
  {
    result = FLASH_SCRUB_EDEVWRITE;
    goto cleanup;
  }

  is_bad_block = flash_scrub_shim_is_block_bad (src_handle, src_blk);
  if (is_bad_block)
  {
    result = FLASH_SCRUB_EBADBLOCK;
    goto cleanup;
  }

  /* Get and validate the page size. */
  page_size = flash_scrub_shim_get_page_size (src_handle);
  dest_page_size = flash_scrub_shim_get_page_size (dest_handle);
  ASSERT (page_size == dest_page_size);
  ASSERT (page_size <= FLASH_SCRUB_MAX_PAGE_SIZE);

  /* Get the number of pages in a block. */
  block_size = flash_scrub_shim_get_block_size (src_handle);
  dest_block_size = flash_scrub_shim_get_block_size (dest_handle);
  ASSERT (block_size == dest_block_size);

  result = flash_scrub_shim_erase_block (dest_handle, dest_blk);
  if (result != FLASH_DEVICE_DONE)
  {
    FLASH_SCRUB_MSG_ERR_2 ("[%d] Failed to erase block %d", result, dest_blk);
    FLASH_SCRUB_DEBUG_ERR_FATAL ("[%d, %d, %d] erase block failed.",
                                 result, dest_blk, 0);
    flash_scrub_shim_mark_block_bad (dest_handle, dest_blk);
    return FLASH_SCRUB_EDEVERASE;
  }

  for(curr_page = 0; curr_page < block_size; curr_page++)
  {
    /* Read one page from source handle */
    result = flash_scrub_shim_read_page (src_handle, copy_buffer, src_blk,
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

    /* Write the data into destination. */
    result = flash_scrub_shim_write_page (dest_handle, copy_buffer,
                                          dest_blk,    curr_page);

    /* If it fails, erase the destination block and mark it as bad.*/
    if (result != FLASH_DEVICE_DONE)
    {
      flash_scrub_shim_erase_block (dest_handle, dest_blk);
      FLASH_SCRUB_DEBUG_ERR_FATAL ("[%d, %d, %d] erase block failed.",
                                   result, dest_blk, curr_page);
      flash_scrub_shim_mark_block_bad (dest_handle, dest_blk);
      result = FLASH_SCRUB_EDEVWRITE;
      goto cleanup;
    }

    copy_count++;

#ifdef SCRUB_READ_BACK_VERIFY
    read_back_verify (dest_handle, dest_blk, curr_page, copy_buffer,
                      page_size);
#endif

  }

  /* If no pages were copied, return error. If at least one page was copied,
   * return the number of pages successfully copied. */
  if (copy_count == 0)
    result = FLASH_SCRUB_EDEVREAD;
  else
    result = copy_count;

cleanup:
  return result;
}


void*
flash_scrub_util_borrow_buffer (uint32 buffer_size)
{
  ASSERT (sizeof (copy_buffer) >= buffer_size);
  ASSERT (copy_buffer_borrowed == 0);
  copy_buffer_borrowed = 1;
  return copy_buffer;
}

void
flash_scrub_util_borrow_buffer_free (void *buffer)
{
  (void) buffer;
  ASSERT (copy_buffer_borrowed == 1);
  copy_buffer_borrowed = 0;
}

int
flash_scrub_util_compare_strings (const char *str1, const char *str2)
{
  size_t len1, len2;
  int result;

  len1 = strlen (str1);
  len2 = strlen (str2);
  if (len1 != len2)
  {
    return -1;
  }

  result = strncmp (str1, str2, len1);
  return result;
}

void
flash_scrub_util_compute_md5 (flash_scrub_shim_handle parti_handle,
                        uint32 start_block, uint32 end_block,
                        MD5_CTX *md5_ctx, uint8 *md5_hash)
{
  uint32 curr_block, curr_page, page_size, block_size;
  int result, is_bad_block;

  ASSERT (parti_handle != NULL);

  /* Make sure the logic is sound and no body is using the borrowed buffer.*/
  ASSERT (copy_buffer_borrowed == 0);

  /* Get and validate the page size. */
  page_size = flash_scrub_shim_get_page_size (parti_handle);
  ASSERT (page_size <= FLASH_SCRUB_MAX_PAGE_SIZE);

  /* Get the number of pages in a block. */
  block_size = flash_scrub_shim_get_block_size (parti_handle);

  MD5Init (md5_ctx);

  for (curr_block = start_block; curr_block < end_block; ++curr_block)
  {
    is_bad_block = flash_scrub_shim_is_block_bad (parti_handle, curr_block);
    if (is_bad_block)
    {
      continue;
    }

    for(curr_page = 0; curr_page < block_size; ++curr_page)
    {
      result = flash_scrub_shim_read_page (parti_handle, copy_buffer,
                                           curr_block, curr_page);
      if (result != FLASH_DEVICE_DONE)
      {
        if (result != FLASH_DEVICE_FAIL_PAGE_ERASED)
        {
          FLASH_SCRUB_DEBUG_ERR_FATAL ("[%d,%d,%d] read failed.",
                                       result, curr_block, curr_page);
        }

        continue; /* On error, skip to the next page. */
      }

      MD5Update (md5_ctx, copy_buffer, page_size);
    }
  }

  MD5Final (md5_hash, md5_ctx);
}

