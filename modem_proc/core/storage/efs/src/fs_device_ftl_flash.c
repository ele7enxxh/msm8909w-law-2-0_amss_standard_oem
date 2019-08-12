/***********************************************************************
 * fs_device_ftl_flash.c
 *
 * Device layer abstration for FTL on flash device.
 * Copyright (C) 2008-2010 QUALCOMM Technologies, Inc.
 *
 * Device layer abstration for FTL on flash device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_device_ftl_flash.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2010-11-08   wek   Remove impractical and unused paranoia module.
2010-09-30   wek   Remove device iovec interface.
2010-03-15   vr    Fix Misaligned FTL Buffer passed to Flash.
2009-11-04   dks   Changed fs_device_ftl_device_type
2008-12-15   rp    Remove reference to obsolete flash APIs.
2008-12-12   rp    Introducing i/o-vec interface.
2008-11-06   rp    Remove old fashioned flash probing.
2008-11-03   rp    Create

===========================================================================*/

#include "fs_config.h"
#include "fs_device.h"

#ifdef FEATURE_EFS_FTL

#include "fs_device_ftl_flash.h"
#include "fs_err.h"
#include "fs_util.h"
#include "assert.h"

/* Check if we have the needed flash driver support included */
#if !defined(HAVE_FLASH_COPY_PAGE) || !defined(HAVE_FLASH_RW_SPARE_UDATA_BYTES)
#error Required updated flash driver features are missing...
#endif


#define FS_DEVICE_FTL_MAX_DEVICES        (MAX_FTL_PARTITIONS)


static char *ftl_partition_names[] = {"0:FTL", "0:FTL2", NULL};

/* Max FTL instances to encapsulate the ftl-device. */
static struct efs_device fs_base_ftl_device[FS_DEVICE_FTL_MAX_DEVICES];




#ifdef FS_UNIT_TEST

static uint32 ftl_read_color_counters[NUM_FTL_OP_CATEGORIES];
static uint32 ftl_write_color_counters[NUM_FTL_OP_CATEGORIES];
static uint32 ftl_copy_color_counters[NUM_FTL_OP_CATEGORIES];
static uint32 ftl_erase_color_counters[NUM_FTL_OP_CATEGORIES];

static uint32 ftl_read_sector_counters[NUM_FTL_OP_CATEGORIES];
static uint32 ftl_write_sector_counters[NUM_FTL_OP_CATEGORIES];
static uint32 ftl_copy_sector_counters[NUM_FTL_OP_CATEGORIES];


void
ftl_device_reset_counters (void)
{
  int i;

  for (i = 0; i < NUM_FTL_OP_CATEGORIES; ++i)
  {
    ftl_write_color_counters[i] = 0;
    ftl_read_color_counters[i] = 0;
    ftl_copy_color_counters[i] = 0;
    ftl_erase_color_counters[i] = 0;

    ftl_write_sector_counters[i] = 0;
    ftl_read_sector_counters[i] = 0;
    ftl_copy_sector_counters[i] = 0;
  }
}

void
ftl_device_get_stats (ftl_dev_stat_type* op_stats,
                      fs_ftl_op_category category)
{
  ASSERT (category < NUM_FTL_OP_CATEGORIES);

  if (op_stats)
  {
    op_stats->ftl_write_op_count = ftl_write_color_counters[category];
    op_stats->ftl_read_op_count = ftl_read_color_counters[category];
    op_stats->ftl_copy_op_count = ftl_copy_color_counters[category];
    op_stats->ftl_erase_op_count = ftl_erase_color_counters[category];

    op_stats->ftl_write_sector_count = ftl_write_sector_counters[category];
    op_stats->ftl_read_sector_count = ftl_read_sector_counters[category];
    op_stats->ftl_copy_sector_count = ftl_copy_sector_counters[category];
  }
}

uint32
ftl_update_read_counter (uint32 count,
                         fs_ftl_op_category category)
{
  ASSERT (category < NUM_FTL_OP_CATEGORIES);
  ftl_read_color_counters[category]++;
  ftl_read_sector_counters[category] += count;
  return ftl_read_color_counters[category];
}

uint32
ftl_update_write_counter (uint32 count,
                          fs_ftl_op_category category)
{
  ASSERT (category < NUM_FTL_OP_CATEGORIES);
  ftl_write_color_counters[category]++;
  ftl_write_sector_counters[category] += count;
  return ftl_write_color_counters[category];
}

uint32
ftl_update_erase_counter (uint32 count,
                          fs_ftl_op_category category)
{
  (void)count;
  ASSERT (category < NUM_FTL_OP_CATEGORIES);
  return ftl_erase_color_counters[category]++;
}

uint32
ftl_update_copy_pg_counter (uint32 count,
                            fs_ftl_op_category category)
{
  ASSERT (category < NUM_FTL_OP_CATEGORIES);
  ftl_copy_color_counters[FTL_OP_FLASH]++;
  ftl_copy_sector_counters[FTL_OP_FLASH] += count;
  return ftl_copy_color_counters[FTL_OP_FLASH];
}

#else   /* FS_UNIT_TEST */

#define ftl_update_read_counter     update_counter
#define ftl_update_write_counter    update_counter
#define ftl_update_erase_counter    update_counter
#define ftl_update_copy_pg_counter  update_counter

uint32
update_counter (uint32 count,
                fs_ftl_op_category category)
{
  (void)count;
  (void)category;
  return 0;
}
#endif   /* FS_UNIT_TEST */


void
fs_device_ftl_flash_init (void)
{
  memset (fs_base_ftl_device, 0, sizeof(fs_base_ftl_device));
}

efs_device_t
fs_device_ftl_flash_open (uint32 which_device)
{
  fs_device_t ftl_flash_dev;
  efs_device_t base_dev;
  uint32 partition_name_cnt;

  if (which_device >= FS_DEVICE_FTL_MAX_DEVICES)
    FS_ERR_FATAL ("Max device limit exceeded...",0,0,0);

  /* Sanity check for number of supported partitions. */
  partition_name_cnt = sizeof (ftl_partition_names) / sizeof (char*);
  if (FS_DEVICE_FTL_MAX_DEVICES >= partition_name_cnt)
    FS_ERR_FATAL ("check FTL partition-names config...",0,0,0);

  base_dev = &fs_base_ftl_device[which_device];

  ftl_flash_dev = flash_open (ftl_partition_names[which_device], 0);
  if (ftl_flash_dev == FS_NO_DEVICE)
    return NULL;

  base_dev->efs_device_type = FS_DEVICE_TYPE_FTL_NAND;
  fs_device_ftl_flash_init_ops (base_dev);
  base_dev->priv.ftl_flash = ftl_flash_dev;

  return base_dev;
}

int
fs_device_ftl_flash_close (efs_device_t base_dev)
{
  int result = -1;

  result = flash_close (base_dev->priv.ftl_flash);
  base_dev->priv.ftl_flash = NULL;
  return result;
}



static char*
fs_device_ftl_device_name (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->device_name (flash_dev);
}

static int
fs_device_ftl_device_maker_id (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->device_maker_id (flash_dev);
}

static int
fs_device_ftl_device_id (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->device_id (flash_dev);
}

static uint32
fs_device_ftl_block_count (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->block_count (flash_dev);
}

static uint32
fs_device_ftl_block_size (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->block_size (flash_dev);
}

static uint32
fs_device_ftl_page_size (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->page_size (flash_dev);
}

static uint32
fs_device_ftl_total_page_size (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->total_page_size (flash_dev);
}

static uint32
fs_device_ftl_device_type (efs_device_t base_dev)
{
  return base_dev->efs_device_type;
}

static int
fs_device_ftl_bad_block_check (efs_device_t base_dev, fs_block_id block)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->bad_block_check (flash_dev, block);
}

static int
fs_device_ftl_mark_block_bad (efs_device_t base_dev, fs_block_id block)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->mark_block_bad (flash_dev, block);
}

static int
fs_device_ftl_write_page (efs_device_t base_dev, fs_page_id page, void *data)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  ftl_update_write_counter (1, FTL_OP_FLASH);
  ASSERT (FS_OKAY_FOR_DM (data));
  return flash_dev->write_page (flash_dev, page, data);
}

static int
fs_device_ftl_erase_block (efs_device_t base_dev, fs_block_id block)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  ftl_update_erase_counter (1, FTL_OP_FLASH);
  return flash_dev->erase_block (flash_dev, block);
}

static void*
fs_device_ftl_read_pointer (efs_device_t base_dev, fs_page_id page)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->read_pointer (flash_dev, page);
}

static int
fs_device_ftl_read_page (efs_device_t base_dev, fs_page_id page, void *data)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  ftl_update_read_counter (1, FTL_OP_FLASH);
  ASSERT (FS_OKAY_FOR_DM (data));
  return flash_dev->read_page (flash_dev, page, data);
}

static int
fs_device_ftl_is_erased (efs_device_t base_dev, fs_page_id page)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->is_erased (flash_dev, page);
}

static int
fs_device_ftl_partial_write (efs_device_t base_dev, fs_page_id page,
            void *data, int offset, int length)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  ASSERT (FS_OKAY_FOR_DM (data));
  return flash_dev->partial_write (flash_dev, page, data, offset, length);
}

static int
fs_device_ftl_begin_erase_block (efs_device_t base_dev, fs_block_id block)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->begin_erase_block (flash_dev, block);
}

static int
fs_device_ftl_suspend_erase (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->suspend_erase (flash_dev);
}

static int
fs_device_ftl_resume_erase (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->resume_erase (flash_dev);
}

static int
fs_device_ftl_erase_status (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->erase_status (flash_dev);
}

static int
fs_device_ftl_is_block_erased (efs_device_t base_dev, fs_block_id block)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->is_block_erased (flash_dev, block);
}

static int
fs_device_ftl_read_spare_bytes (efs_device_t base_dev, fs_page_id page,
           uint32 spare_offset, uint32 len, void *data)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  ASSERT (FS_OKAY_FOR_DM (data));
  return flash_dev->read_spare_bytes (flash_dev, page, spare_offset,
                                      len, data);
}

static int
fs_device_ftl_read_mpages (efs_device_t base_dev, fs_page_id start_page,
                           fs_page_id num_pages,
                           void *data, void (*wdog_kick_fn_ptr)(void) )
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  ASSERT (FS_OKAY_FOR_DM (data));
  return flash_dev->read_mpages (flash_dev, start_page, num_pages,
                                 data, wdog_kick_fn_ptr);
}

static int
fs_device_ftl_copy_page (efs_device_t base_dev, fs_page_id src_page,
                         fs_page_id dest_page)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->copy_page (flash_dev, src_page, dest_page);
}

static int
fs_device_ftl_lock_ops (efs_device_t base_dev, fs_page_id start_page,
                        fs_page_id end_page, uint32 opcode)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->lock_ops (flash_dev, start_page, end_page, opcode);
}

static int
fs_device_ftl_get_spare_corrected_bytes (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->get_spare_corrected_bytes (flash_dev);
}

static int
fs_device_ftl_write_page_and_udata (efs_device_t base_dev, fs_page_id page,
           void *page_data, void *spare_udata, uint32 spare_udata_blen)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  ftl_update_write_counter (1, FTL_OP_FLASH);
  ASSERT (FS_OKAY_FOR_DM (page_data));
  return flash_dev->write_page_and_udata (flash_dev, page, page_data,
                                          spare_udata, spare_udata_blen);
}

static int
fs_device_ftl_read_spare_udata (efs_device_t base_dev, fs_page_id page,
           void *spare_udata, uint32 spare_udata_blen)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->read_spare_udata (flash_dev, page,
                                      spare_udata, spare_udata_blen);
}

static int
fs_device_ftl_read_page_and_udata (efs_device_t base_dev, fs_page_id page,
           void *data, void *spare, uint32 spare_udata_blen)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  ASSERT (FS_OKAY_FOR_DM (data));
  return flash_dev->read_page_and_udata (flash_dev, page, data,
                                         spare, spare_udata_blen);
}

static int
fs_device_ftl_open_partition (efs_device_t base_dev, char *partition_name)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->open_partition (flash_dev, partition_name);
}

static void
fs_device_ftl_set_ecc_state (efs_device_t base_dev, int state)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  flash_dev->set_ecc_state (flash_dev, state);
}

static fs_device_write_style_t
fs_device_ftl_get_write_style (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.ftl_flash;
  return flash_dev->get_write_style (flash_dev);
}

uint32
ftl_device_write_counter (uint32 sector, uint32 count,
                          fs_ftl_op_category category)
{
  (void) sector;
  return ftl_update_write_counter (count, category);
}

uint32
ftl_device_read_counter (uint32 sector, uint32 count,
                         fs_ftl_op_category category)
{
  (void) sector;
  return ftl_update_read_counter (count, category);
}

void
fs_device_ftl_flash_init_ops (efs_device_t base_dev)
{
  fs_device_init_invalid_ops (base_dev);

  base_dev->device_name = fs_device_ftl_device_name;
  base_dev->device_maker_id = fs_device_ftl_device_maker_id;
  base_dev->device_id = fs_device_ftl_device_id;
  base_dev->block_count = fs_device_ftl_block_count;
  base_dev->block_size = fs_device_ftl_block_size;
  base_dev->page_size = fs_device_ftl_page_size;
  base_dev->total_page_size = fs_device_ftl_total_page_size;
  base_dev->device_type = fs_device_ftl_device_type;
  base_dev->bad_block_check = fs_device_ftl_bad_block_check;
  base_dev->mark_block_bad = fs_device_ftl_mark_block_bad;
  base_dev->write_page = fs_device_ftl_write_page;
  base_dev->erase_block = fs_device_ftl_erase_block;
  base_dev->read_pointer = fs_device_ftl_read_pointer;
  base_dev->read_page = fs_device_ftl_read_page;
  base_dev->is_erased = fs_device_ftl_is_erased;
  base_dev->partial_write = fs_device_ftl_partial_write;
  base_dev->begin_erase_block = fs_device_ftl_begin_erase_block;
  base_dev->suspend_erase = fs_device_ftl_suspend_erase;
  base_dev->resume_erase = fs_device_ftl_resume_erase;
  base_dev->erase_status = fs_device_ftl_erase_status;
  base_dev->is_block_erased = fs_device_ftl_is_block_erased;
  base_dev->read_spare_bytes = fs_device_ftl_read_spare_bytes;
  base_dev->read_mpages = fs_device_ftl_read_mpages;
  base_dev->copy_page = fs_device_ftl_copy_page;
  base_dev->lock_ops = fs_device_ftl_lock_ops;
  base_dev->get_spare_corrected_bytes =
                            fs_device_ftl_get_spare_corrected_bytes;
  base_dev->write_page_and_udata = fs_device_ftl_write_page_and_udata;
  base_dev->read_spare_udata = fs_device_ftl_read_spare_udata;
  base_dev->read_page_and_udata = fs_device_ftl_read_page_and_udata;
  base_dev->open_partition = fs_device_ftl_open_partition;
  base_dev->set_ecc_state = fs_device_ftl_set_ecc_state;
  base_dev->get_write_style = fs_device_ftl_get_write_style;

  base_dev->read_sectors = NULL;
  base_dev->write_sectors = NULL;
  base_dev->reliable_write_sectors = NULL;
}

#endif /* FEATURE_EFS_FTL */
