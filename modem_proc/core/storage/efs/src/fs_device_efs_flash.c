/***********************************************************************
 * fs_device_efs_flash.c
 *
 * Device layer abstration for EFS on flash device.
 * Copyright (C) 2008-2011,2013-2014 QUALCOMM Technologies, Inc.
 *
 * Device layer abstration for EFS on flash device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_device_efs_flash.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-10-13   dks   Changes to enable EFS over NAND on apps processor.
2013-08-08   wek   Init flash dal shim layer at run time.
2013-04-29   wek   Fix compile erros in flash dal shim for boot.
2013-04-26   wek   Move all buffers to DDR when compiling for boot.
2011-03-30   dks   Check for failure status after begin erase.
2011-03-08   dks   Changed erase wait time increment to 20.
2010-11-24   wek   Remove dependency with page size macro.
2010-11-08   wek   Remove impractical and unused paranoia module.
2010-09-30   wek   Remove device iovec interface.
2010-06-25   dks   Fixed bug in is_erased.
2010-05-21   rp    Fix compiler warnings seen on target.
2010-05-10   dks   Added change to support EFS on SPI-NOR devices.
2010-05-10   rp    Removed #define FEATURE_FS_REMOTE_APPS_PROC
2009-10-04   dks   Added translation of driver to device layer device type
2008-12-15   rp    Remove reference to obsolete flash APIs.
2008-12-12   rp    Introducing i/o-vec interface.
2008-11-06   rp    Remove old fashioned flash probing.
2008-11-03   rp    Create

===========================================================================*/

#include "fs_config.h"
#include "fs_device.h"
#include "fs_device_types.h"
#include "fs_util.h"
#include "fs_err.h"
#include "assert.h"

#if defined (FEATURE_EFS_EFS2_ON_NAND) ||   \
    defined (FEATURE_EFS_EFS2_ON_NOR) || \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)

#include "fs_device_efs_flash.h"
#include "fs_err.h"
#include "fs_util.h"
#include "fs_protected_memory.h"

/* Make sure we have the flash_open() APIs. */
#ifndef HAVE_FLASH_OPEN_AND_CLOSE
  #error "HAVE_FLASH_OPEN_AND_CLOSE is missing..."
#endif /* HAVE_FLASH_OPEN_AND_CLOSE */



#ifndef EFS_PARTITION_NAME
  /* If this is undefined for some reason use default */
  #ifndef MIBIB_EFS2_MODEM_PARTI_NAME
    #define MIBIB_EFS2_MODEM_PARTI_NAME "0:EFS2"
  #endif

  #if defined IMAGE_APPS_PROC && !defined FEATURE_EFS_BOOT_ENV
    #define EFS_PARTITION_NAME MIBIB_EFS2_APPS_PARTI_NAME
  #else
    #define EFS_PARTITION_NAME MIBIB_EFS2_MODEM_PARTI_NAME
  #endif
#endif  /* EFS_PARTITION_NAME defined */



#define FS_DEVICE_FLASH_MAX_DEVICES        (1)


/* Max Flash instances to encapsulate the flash-device. */
static struct efs_device fs_base_flash_device[FS_DEVICE_FLASH_MAX_DEVICES];


void
fs_device_efs_flash_init (void)
{
#ifdef FLASH_USES_DAL
  fs_flash_dal_shim_init ();
#endif

  memset (fs_base_flash_device, 0, sizeof(fs_base_flash_device));
}


efs_device_t
fs_device_efs_flash_open (uint32 which_device)
{
  fs_device_t flash_dev;
  efs_device_t base_dev;
  uint32 flash_dev_type;
  uint32 page_size, tmp;

  if (which_device >= FS_DEVICE_FLASH_MAX_DEVICES)
    FS_ERR_FATAL ("Max device limit exceeded...",0,0,0);

  base_dev = &fs_base_flash_device[which_device];

#ifdef FLASH_USES_DAL
  flash_dev = flash_shim_open (EFS_PARTITION_NAME, 0);
#else
  flash_dev = flash_open (EFS_PARTITION_NAME, 0);
#endif

  if (flash_dev == FS_NO_DEVICE)
    return NULL;

  flash_dev_type = flash_dev->device_type (flash_dev);

  switch (flash_dev_type)
  {
    case FLASH_NOR:
      base_dev->efs_device_type = FS_DEVICE_TYPE_EFS_NOR;
      break;

    case FLASH_NAND:
    case FLASH_ONENAND:
      base_dev->efs_device_type = FS_DEVICE_TYPE_EFS_NAND;
      break;

    case FLASH_NOR_SPI:
      base_dev->efs_device_type = FS_DEVICE_TYPE_EFS_SPI_NOR;
      break;

    default:
      FS_ERR_FATAL ("invalid write-style...",0,0,0);
      break;
  }

  base_dev->priv.flash.flash_dev = flash_dev;

  fs_device_efs_flash_init_ops (base_dev);

  /* If device page size is smaller than the FS_MIN_PAGE_SIZE configured,
     calculate the number of device pages that make up one FS_MIN_PAGE_SIZE.*/
  page_size = flash_dev->page_size (flash_dev);

  /* SPI-NOR device has a device size of 256 bytes and to support such page
     sizes without many changes to already well tested configurations, we use
     FS_MIN_PAGE_SIZE to be the smallest page size that PM will see. This PM
     page size will then be mapped to appropriate number of device pages using
     the dev_pages_per_efs_page variable. */
  base_dev->priv.flash.dev_page_size = page_size;
  tmp = ((page_size < FS_MIN_PAGE_SIZE) ? FS_MIN_PAGE_SIZE / page_size : 1);
  base_dev->priv.flash.dev_pages_per_efs_page = tmp;

  return base_dev;
}


int
fs_device_efs_flash_close (efs_device_t base_dev)
{
  int result = -1;

#ifdef FLASH_USES_DAL
  result = flash_shim_close (base_dev->priv.flash.flash_dev);
#else
  result = flash_close (base_dev->priv.flash.flash_dev);
#endif
  base_dev->priv.flash.flash_dev = NULL;
  return result;
}


static char*
fs_device_flash_device_name (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->device_name (flash_dev);
}

static int
fs_device_flash_device_maker_id (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->device_maker_id (flash_dev);
}

static int
fs_device_flash_device_id (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->device_id (flash_dev);
}

static uint32
fs_device_flash_block_count (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->block_count (flash_dev);
}

static uint32
fs_device_flash_block_size (efs_device_t base_dev)
{
  uint32 block_size;

  /* If we manipulated the page size, to make it at least FS_MIN_PAGE_SIZE
     then manipulate the block size to apporpriate size interms of
     FS_MIN_PAGE_SIZE  */
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  block_size = flash_dev->block_size (flash_dev) /
               base_dev->priv.flash.dev_pages_per_efs_page;

  return block_size;
}

static uint32
fs_device_flash_page_size (efs_device_t base_dev)
{
  uint32 dev_page_size;
  uint32 dev_pages_per_efs_page;

  /* If the device page size is less than FS_MIN_PAGE_SIZE, we want to return
     FS_MIN_PAGE_SIZE since a smaller device size is not well tested.
     If the device page size is greater than the FS_MIN_PAGE_SIZE we
     expose it as is. */
  dev_page_size = base_dev->priv.flash.dev_page_size;
  if (dev_page_size < FS_MIN_PAGE_SIZE)
  {
    dev_pages_per_efs_page = base_dev->priv.flash.dev_pages_per_efs_page;
    ASSERT (dev_pages_per_efs_page * dev_page_size == FS_MIN_PAGE_SIZE);
    dev_page_size = FS_MIN_PAGE_SIZE;
  }

  return dev_page_size;
}

static uint32
fs_device_flash_total_page_size (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->total_page_size (flash_dev);
}

static uint32
fs_device_flash_device_type (efs_device_t base_dev)
{
  return base_dev->efs_device_type;
}

static int
fs_device_flash_bad_block_check (efs_device_t base_dev, fs_block_id block)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->bad_block_check (flash_dev, block);
}

static int
fs_device_flash_mark_block_bad (efs_device_t base_dev, fs_block_id block)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->mark_block_bad (flash_dev, block);
}

static int
fs_device_flash_write_page (efs_device_t base_dev, fs_page_id page, void *data)
{
  uint32 i;
  int result = FS_DEVICE_FAIL;
  fs_page_id num_pages;
  uint8 *data_ptr;
  uint32 dev_page_size;
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;

  dev_page_size = base_dev->priv.flash.dev_page_size;
  num_pages = base_dev->priv.flash.dev_pages_per_efs_page;

  page *= base_dev->priv.flash.dev_pages_per_efs_page;
  data_ptr = (uint8 *)data;

  for (i = 0; i < num_pages; ++i)
  {
    result = flash_dev->write_page (flash_dev, page, data_ptr);
    if (result != FS_DEVICE_OK)
      break;

    data_ptr += dev_page_size;
    ++page;
  }

  return result;
}

static int
fs_device_flash_erase_block (efs_device_t base_dev, fs_block_id block)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->erase_block (flash_dev, block);
}

static void*
fs_device_flash_read_pointer (efs_device_t base_dev, fs_page_id page)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->read_pointer (flash_dev, page);
}

static int
fs_device_flash_read_page (efs_device_t base_dev, fs_page_id page, void *data)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  fs_page_id num_pages = base_dev->priv.flash.dev_pages_per_efs_page;

  page *= base_dev->priv.flash.dev_pages_per_efs_page;

  return flash_dev->read_mpages (flash_dev, page, num_pages, data, NULL);
}

static int
fs_device_flash_is_erased (efs_device_t base_dev, fs_page_id page)
{
  int result = FS_DEVICE_FAIL;
  uint32 index;
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;

  page *= base_dev->priv.flash.dev_pages_per_efs_page;

  for (index = 0; index < base_dev->priv.flash.dev_pages_per_efs_page; ++index)
  {
    result = flash_dev->is_erased (flash_dev, page);
    if (result != 1)
      break;

    page++;
  }
  return result;
}

static int
fs_device_flash_partial_write (efs_device_t base_dev, fs_page_id page,
            void *data, int offset, int length)
{
  int page_size, offset_page, bytes_to_write, result = FS_DEVICE_FAIL;
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  uint8 *data_ptr;

  /* calculate the page number on device to start writing into  based on the
     offset in the page on PM. If device page size is 256bytes and PM page size
     is 1024 bytes, writing 750 bytes starting at offset 300, means we need to
     write to the 2nd, 3rd and 4th device pages conatined in that PM page. */

  page *= base_dev->priv.flash.dev_pages_per_efs_page;
  page_size = flash_dev->page_size (flash_dev);
  offset_page = offset / page_size;
  page += offset_page;   /* device page to start writing from */
  offset %= page_size;   /* offset in that page to start writing from */
  data_ptr = data;

  /* write to each page starting at page, until we have written length bytes */
  while (length > 0)
  {
    /* calcuate bytes to be written in each page based on remaining bytes to be
       written. If its more than the device page size then write the whole page
       else its the last page to be written to and write the remaining bytes */
    if ((offset + length) < page_size)
      bytes_to_write = length;
    else
      bytes_to_write = (page_size - offset);

    result = flash_dev->partial_write (flash_dev, page, data_ptr,
                                       offset, bytes_to_write);
    if (result != FS_DEVICE_OK)
    {
      FS_ERR_FATAL ("partial write failed %d,%d", page, offset, 0);
    }

    /* update remaining length and position in data_ptr to copy data from */
    length -= bytes_to_write;
    data_ptr += bytes_to_write;
    offset = 0;
    page ++;
  }

  return result;
}

static int
fs_device_flash_begin_erase_block (efs_device_t base_dev, fs_block_id block)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->begin_erase_block (flash_dev, block);
}

static int
fs_device_flash_suspend_erase (efs_device_t base_dev)
{
  int result;
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;

  result = flash_dev->suspend_erase (flash_dev);
  if (result == FS_DEVICE_NOT_SUPPORTED)
  {
    /* Device does not support suspending an erase, so lets wait until the
       erase gets done. */
    do
    {
      fs_util_wait (20);
      result = flash_dev->erase_status (flash_dev);
    } while (result == FS_DEVICE_BUSY);

    if (result != FS_DEVICE_DONE)
    {
      /* Begin erase returned success, so when we check back the erase status
         it should either be BUSY or DONE. We do not expect erase to fail. If
         the NOR device returns anything other than DONE then the part is no
         longer working and we cannot recover from it. */
      FS_ERR_FATAL ("Flash erase failed :%d", result, 0, 0);
    }
    result = FS_DEVICE_OK;
  }

  return result;
}

static int
fs_device_flash_resume_erase (efs_device_t base_dev)
{
  int result;
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;

  result = flash_dev->resume_erase (flash_dev);

  if (result == FS_DEVICE_NOT_SUPPORTED)
  {
    /* Device does not support resuming an erase, but we shouldn't really be
       calling resume because it will also not support suspend device */
    FS_ERR_FATAL ("resume erase was called...",0,0,0);
  }

  return result;
}

static int
fs_device_flash_erase_status (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->erase_status (flash_dev);
}

static int
fs_device_flash_is_block_erased (efs_device_t base_dev, fs_block_id block)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->is_block_erased (flash_dev, block);
}

static int
fs_device_flash_read_spare_bytes (efs_device_t base_dev, fs_page_id page,
           uint32 spare_offset, uint32 len, void *data)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->read_spare_bytes (flash_dev, page, spare_offset,
                                      len, data);
}

static int
fs_device_flash_read_mpages (efs_device_t base_dev, fs_page_id start_page,
                             fs_page_id num_pages, void *data,
                             void (*wdog_kick_fn_ptr)(void))
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  start_page *= base_dev->priv.flash.dev_pages_per_efs_page;
  num_pages *= base_dev->priv.flash.dev_pages_per_efs_page;
  return flash_dev->read_mpages (flash_dev, start_page, num_pages, data,
                                 wdog_kick_fn_ptr);
}

static int
fs_device_flash_copy_page (efs_device_t base_dev, fs_page_id src_page,
                           fs_page_id dest_page)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->copy_page (flash_dev, src_page, dest_page);
}

static int
fs_device_flash_lock_ops (efs_device_t base_dev, fs_page_id start_page,
                          fs_page_id end_page, uint32 opcode)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->lock_ops (flash_dev, start_page, end_page, opcode);
}

static int
fs_device_flash_get_spare_corrected_bytes (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->get_spare_corrected_bytes (flash_dev);
}

static int
fs_device_flash_write_page_and_udata (efs_device_t base_dev, fs_page_id page,
           void *page_data, void *spare_udata, uint32 spare_udata_blen)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->write_page_and_udata (flash_dev, page, page_data,
                                          spare_udata, spare_udata_blen);
}

static int
fs_device_flash_read_spare_udata (efs_device_t base_dev, fs_page_id page,
           void *spare_udata, uint32 spare_udata_blen)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->read_spare_udata (flash_dev, page,
                                      spare_udata, spare_udata_blen);
}

static int
fs_device_flash_read_page_and_udata (efs_device_t base_dev, fs_page_id page,
           void *data, void *spare, uint32 spare_udata_blen)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->read_page_and_udata (flash_dev, page, data,
                                         spare, spare_udata_blen);
}

static int
fs_device_flash_open_partition (efs_device_t base_dev, char *partition_name)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->open_partition (flash_dev, partition_name);
}

static void
fs_device_flash_set_ecc_state (efs_device_t base_dev, int state)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  flash_dev->set_ecc_state (flash_dev, state);
}

static fs_device_write_style_t
fs_device_flash_get_write_style (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.flash.flash_dev;
  return flash_dev->get_write_style (flash_dev);
}


void
fs_device_efs_flash_init_ops (efs_device_t base_dev)
{
  fs_device_init_invalid_ops (base_dev);

  base_dev->device_name = fs_device_flash_device_name;
  base_dev->device_maker_id = fs_device_flash_device_maker_id;
  base_dev->device_id = fs_device_flash_device_id;
  base_dev->block_count = fs_device_flash_block_count;
  base_dev->block_size = fs_device_flash_block_size;
  base_dev->page_size = fs_device_flash_page_size;
  base_dev->total_page_size = fs_device_flash_total_page_size;
  base_dev->device_type = fs_device_flash_device_type;
  base_dev->bad_block_check = fs_device_flash_bad_block_check;
  base_dev->mark_block_bad = fs_device_flash_mark_block_bad;
  base_dev->write_page = fs_device_flash_write_page;
  base_dev->erase_block = fs_device_flash_erase_block;
  base_dev->read_pointer = fs_device_flash_read_pointer;
  base_dev->read_page = fs_device_flash_read_page;
  base_dev->is_erased = fs_device_flash_is_erased;
  base_dev->partial_write = fs_device_flash_partial_write;
  base_dev->begin_erase_block = fs_device_flash_begin_erase_block;
  base_dev->suspend_erase = fs_device_flash_suspend_erase;
  base_dev->resume_erase = fs_device_flash_resume_erase;
  base_dev->erase_status = fs_device_flash_erase_status;
  base_dev->is_block_erased = fs_device_flash_is_block_erased;
  base_dev->read_spare_bytes = fs_device_flash_read_spare_bytes;
  base_dev->read_mpages = fs_device_flash_read_mpages;
  base_dev->copy_page = fs_device_flash_copy_page;
  base_dev->lock_ops = fs_device_flash_lock_ops;
  base_dev->get_spare_corrected_bytes =
                        fs_device_flash_get_spare_corrected_bytes;
  base_dev->write_page_and_udata = fs_device_flash_write_page_and_udata;
  base_dev->read_spare_udata = fs_device_flash_read_spare_udata;
  base_dev->read_page_and_udata = fs_device_flash_read_page_and_udata;
  base_dev->open_partition = fs_device_flash_open_partition;
  base_dev->set_ecc_state = fs_device_flash_set_ecc_state;
  base_dev->get_write_style = fs_device_flash_get_write_style;

  base_dev->read_sectors = NULL;
  base_dev->write_sectors = NULL;
  base_dev->reliable_write_sectors = NULL;
}

#endif /* (FEATURE_EFS_EFS2_ON_NAND) || FEATURE_EFS_EFS2_ON_NOR)
          || (FEATURE_EFS_EFS2_ON_SPI_NOR) */

