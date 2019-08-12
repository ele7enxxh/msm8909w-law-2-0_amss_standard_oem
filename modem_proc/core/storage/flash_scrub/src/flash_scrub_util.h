/****************************************************************************
 * flash_scrub_util.h
 *
 * Utility functions used by several modules.
 * Copyright (C) 2013 QUALCOMM Technologies, Inc.
 *
 * Declarations of utility functions uses by different modules inside Scrub.
 *
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_util.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------  ---  ------------------------------------------------------------
2013-08-15   rp    Prefix flash_scrub to all non-static functions.
2013-08-15   rp    Make all target file names to start with flash_scrub.
2013-08-04   rp    Add MD5-support to verify code-partition scrubbing.
2013-07-17   wek   Allow RAM buffer to be borrowed during initialization.
2013-07-09   wek   Create.

===========================================================================*/

#ifndef __FLASH_SCRUB_UTIL_H__
#define __FLASH_SCRUB_UTIL_H__

#include "flash_scrub_config_i.h"
#include "flash_scrub_flash_dal_shim.h"
#include "md5.h"

/* Copies all the pages in a block from source to destination.
 *
 * Data in NAND is stored in pages, the pages group together into erasable
 * chunks called blocks. This function will take care of:
 *
 * 1. Erasing the destination block.
 * 2. Copying all the pages from the source block to the destination block.
 * 3. If flash erase/program error occurs:
 * 3.1 Erase the block (if required).
 * 3.2 Mark block as bad and return error.
 *
 * If there are erased gaps withing a block, this function will also
 * replicate them.
 *
 * @param src_handle Flash handle for the source partition, returned by open.
 * @param src_block  Block within the source handle to copy.
 * @param dest_handle Flash handle for the destination partition.
 * @param dest_blk    Block number in the destination where to place the data
 *
 * @return The number of pages successfully copied, -1 on error.
 */
int flash_scrub_copy_block (flash_scrub_shim_handle src_handle, uint32 src_blk,
                         flash_scrub_shim_handle dest_handle, uint32 dest_blk);

/* Borrows the buffer used by flash_scrub_copy_block.
 *
 * Other modules in scrub need a buffer to read/write data from/to the
 * device. This function allows to borrow the buffer for a period
 * of time. This buffer should be borrowed only at initialization time
 * or when scrub_copy_block is not going to be called.
 *
 * @param buffer_size  Size of the buffer required, for sanity checking.
 *
 * @return Pointer to the borrowed buffer.
 **/
void* flash_scrub_util_borrow_buffer (uint32 buffer_size);

/* Indicates that the module that borrowed the buffer no longer needs it.
 *
 * @param Pointer to the original buffer.
 *
 */
void flash_scrub_util_borrow_buffer_free (void *buffer);

int flash_scrub_util_compare_strings (const char *str1, const char *str2);

void flash_scrub_util_compute_md5 (flash_scrub_shim_handle parti_handle,
                                   uint32 start_block, uint32 end_block,
                                   MD5_CTX *md5_ctx, uint8 *md5_hash);

#endif /* not __FLASH_SCRUB_UTIL_H__ */
