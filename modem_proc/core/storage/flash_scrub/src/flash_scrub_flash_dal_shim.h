/****************************************************************************
 * flash_scrub_flash_dal_shim.c
 *
 * Flash scrub module device shim layer.
 * Copyright (C) 2009-2013 QUALCOMM Technologies, Inc.
 *
 *
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_flash_dal_shim.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-11-16   dks   Add flash_ops functions.
2013-09-16   wek   Remove page size and block size arguments from read/write.
2013-08-15   rp    Prefix flash_scrub to all non-static functions.
2013-08-15   rp    Make all target file names to start with flash_scrub.
2013-08-14   rp    Include "scrub_config_i.h" before any other includes.
2013-08-02   rp    scrub_flash_dal_shim_partition_table_update name change.
2013-06-28   rp    Create

===========================================================================*/

#ifndef __FLASH_SCRUB_FLASH_DAL_SHIM_H__
#define __FLASH_SCRUB_FLASH_DAL_SHIM_H__

#include "flash_scrub_config_i.h"

#include "flash.h"
#include "flash_miparti.h"

struct flash_scrub_shim_handle_type;
typedef struct flash_scrub_shim_handle_type* flash_scrub_shim_handle;


void flash_scrub_shim_init (void);

flash_scrub_shim_handle flash_scrub_shim_open_partition (
                                        const char *partition_name);

int flash_scrub_shim_close_partition (flash_scrub_shim_handle handle);

uint32 flash_scrub_shim_get_page_size (flash_scrub_shim_handle handle);

uint32 flash_scrub_shim_get_block_size (
                                     flash_scrub_shim_handle handle);

uint32 flash_scrub_shim_get_block_count(
                                      flash_scrub_shim_handle handle);


int flash_scrub_shim_erase_block (flash_scrub_shim_handle handle,
                                      uint32 block_no);

int flash_scrub_shim_write_page (flash_scrub_shim_handle handle,
          uint8 *page_buff, uint32 block_no, uint32 page_no);

int flash_scrub_shim_read_page (flash_scrub_shim_handle handle,
          uint8 *page_buff, uint32 block_no, uint32 page_no);

int flash_scrub_shim_mark_block_bad (flash_scrub_shim_handle handle,
          uint32 block_no);

int flash_scrub_shim_is_block_bad (flash_scrub_shim_handle handle,
          uint32 block_no);

int flash_scrub_shim_partition_table_get (
           flash_scrub_shim_handle handle, flash_partable_t parti_table);

int flash_scrub_shim_partition_table_update (
          flash_scrub_shim_handle handle, flash_partable_t parti_table);

void flash_scrub_shim_ops_start (void);

void flash_scrub_shim_ops_end (void);

#endif /* __FLASH_SCRUB_FLASH_DAL_SHIM_H__ */
