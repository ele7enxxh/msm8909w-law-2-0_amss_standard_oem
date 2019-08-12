/***********************************************************************
 * fs_device.c
 *
 * Flash operation wrappers and counters.
 * Copyright (C) 2006-2013 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_device.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-02-06   rp    Fix compiler warning in partial_write
2013-02-04   rp    Do not err-fatal on partial_write failures, pass the error
2013-01-17   wek   Remove reference to rex signal type.
2013-01-04   dks   Move simulator specific device reset call to simulator file.
2012-12-02   dks   Increase read_pointer_cache size for optimization.
2012-11-09    nr   Update rmts to use new api's.
2012-11-08   wek   Fix compile error on flashless targets in write style.
2012-10-04   wek   Remove all err fatals from unimplemented dev functions.
2012-09-28   wek   Remove ERR_FATALs hit with diag dev info handler.
2012-09-10    nr   Implement new FS Benchmarking scheme.
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2012-01-19   dks   Fix remotefs client init sequence for QMI based transfer.
2011-08-17    rp   Add EFS OTP (One-Time-Programmable) support.
2011-06-16   wek   Use max page size instead of efs page size for local buf.
2011-05-13   dks   Add read_pointer buffer to device layer.
2011-03-25    rp   Support RemoteFS layer for QMI changes.
2011-03-09   dks   Add device setup calls.
2010-11-17    rp   Use enums to refer to FS1 and FS2 eMMC partitions
2010-11-16    vr   Remove dependency on flash for non-flash based targets.
2010-11-11   wek   Remove dependency with page size macro.
2010-09-30   wek   Remove device iovec interface.
2010-08-20    vr   Added logic to load efs partitions in EFS for rmts targets
2010-07-30   wek   Dynamically allocate all buffers dependent on page size.
2010-06-25   dks   Added counters for partial-write, begin and suspend-erase.
2010-05-21    rp   Fix compiler warnings seen on target.
2010-05-13   wek   Make sync_no_wait non blocking.
2010-05-10   dks   Added change to support EFS on SPI-NOR devices.
2010-05-07    rp   Added Ram filesystem.
2010-04-23   yog   Removed compiler warnings.
2010-03-03    rp   Fix crash in efs_sync_no_wait when called the second time.
2010-01-19    vr   Added efs_sync_no_wait and efs_sync_get_status APIs.
2009-07-01    yg   Align the verify buffer.
2008-12-12    rp   Introducing i/o-vec interface and Managed Nand.
2008-11-06    sh   Add Babylon markers to flash.
2008-11-03    rp   Introduce device abstraction.
2008-09-09    yg   Added code to do readback verification for writes.
2008-05-23    rp   Use bencmark counters from fs_counters.h
2007-10-18    rp   Featurized QEFS2.
2007-09-10    rp   Fix documentation for fs_flash_partial_write.
2007-08-10    rp   Ensure buffers to flash are aligned on a 32-byte boundary.
2007-04-23   umr   Added support for ARM MMU DAC memory protection feature.
2007-02-23   dlb   Move read failures into device wrappers.
2006-11-09   dlb   Add dealloc counter.
2006-09-22    sh   More comments.
2006-09-22   dlb   Lint fixes.
2006-09-12   dlb   Change some queries to unsigned values.
2006-09-01   dlb   Create

===========================================================================*/

#include "fs_device.h"
#include "fs_err.h"
#include "assert.h"
#include "fs_protected_memory.h"
#include "fs_babylon.h"
#include "fs_device_efs_flash.h"
#include "fs_device_ftl_flash.h"
#include "fs_device_efs_mnand.h"
#include "fs_device_efs_rmts.h"
#include "fs_device_efs_ramfs.h"
#include "fs_device_otp_flash.h"
#include "fs_malloc.h"
#include "fs_timetick.h"
#include "fs_device_read_pointer_cache.h"
#include <string.h>

#ifdef FS_UNIT_TEST
  #include "utdevice.h"
#endif

/* Enable this flag to do readback verification of flash writes. */
#undef EFS_READ_BACK_VERIFY_PAGE

#ifdef EFS_READ_BACK_VERIFY_PAGE
#include <string.h>
FS_ALIGN static byte verify_buffer[FS_MAX_PAGE_SIZE];
#endif

/* The counters should always be updated, but there might not be any
 * code to reference (read) them.  They are still useful from within a
 * debugger.  We explain it to Lint this way: */
//lint -esym(551, write_color_counters)
//lint -esym(551, read_color_counters)
//lint -esym(551, erase_color_counters)
//lint -esym(551, buffer_fill_counters)

void
fs_device_init (void)
{
  fs_device_efs_flash_init ();
  fs_device_ftl_flash_init ();
  fs_device_efs_mnand_init ();
  fs_device_efs_rmts_init ();
  fs_device_efs_ramfs_init ();
  fs_device_otp_flash_init ();
  fs_device_read_pointer_cache_init ();
}

efs_device_t
fs_device_open (enum fs_device_type device_type, uint32 which_device)
{
  efs_device_t base_dev = NULL;

  switch (device_type)
  {
    case FS_DEVICE_TYPE_EFS_NAND:
    case FS_DEVICE_TYPE_EFS_ONE_NAND:
    case FS_DEVICE_TYPE_EFS_NOR:
    case FS_DEVICE_TYPE_EFS_SPI_NOR:
      base_dev = fs_device_efs_flash_open (which_device);
      break;

    case FS_DEVICE_TYPE_FTL_NAND:
      base_dev = fs_device_ftl_flash_open (which_device);
      break;

    case FS_DEVICE_TYPE_EFS_MNAND:
      base_dev = fs_device_efs_mnand_open (which_device);
      break;

    case FS_DEVICE_TYPE_EFS_RMTS:
      base_dev = fs_device_efs_rmts_open (which_device);
      break;

    case FS_DEVICE_TYPE_EFS_RAMFS:
      base_dev = fs_device_efs_ramfs_open (which_device);
      break;

    case FS_DEVICE_TYPE_OTP_FLASH:
      base_dev = fs_device_otp_flash_open (which_device);
      break;

    default:
      FS_ERR_FATAL ("[%d] Unknown device type...", device_type, 0, 0);
      break;
  }

  if (base_dev == NULL)
    return NULL;

  /* Validate the device-type. */
  switch (base_dev->efs_device_type)
  {
    case FS_DEVICE_TYPE_EFS_NOR:
    case FS_DEVICE_TYPE_EFS_SPI_NOR:
    case FS_DEVICE_TYPE_EFS_NAND:
    case FS_DEVICE_TYPE_EFS_ONE_NAND:
    case FS_DEVICE_TYPE_FTL_NAND:
    case FS_DEVICE_TYPE_EFS_MNAND:
    case FS_DEVICE_TYPE_EFS_RMTS:
    case FS_DEVICE_TYPE_EFS_RAMFS:
    case FS_DEVICE_TYPE_OTP_FLASH:
      break;

    default:
      FS_ERR_FATAL ("[%d] invalid device type...", device_type, 0, 0);
      break;
  }

  return base_dev;
}

int
fs_device_close (efs_device_t base_dev)
{
  int result = -1;

  switch (base_dev->efs_device_type)
  {
    case FS_DEVICE_TYPE_EFS_NAND:
    case FS_DEVICE_TYPE_EFS_ONE_NAND:
    case FS_DEVICE_TYPE_EFS_NOR:
    case FS_DEVICE_TYPE_EFS_SPI_NOR:
      result = fs_device_efs_flash_close (base_dev);
      break;

    case FS_DEVICE_TYPE_FTL_NAND:
      result = fs_device_ftl_flash_close (base_dev);
      break;

    case FS_DEVICE_TYPE_EFS_MNAND:
      result = fs_device_efs_mnand_close (base_dev);
      break;

    case FS_DEVICE_TYPE_EFS_RMTS:
      result = fs_device_efs_rmts_close (base_dev);
      break;

    case FS_DEVICE_TYPE_EFS_RAMFS:
      result = fs_device_efs_ramfs_close (base_dev);
      break;

    case FS_DEVICE_TYPE_OTP_FLASH:
      result = fs_device_otp_flash_close (base_dev);
      break;

    default:
      FS_ERR_FATAL ("[%d] Unknown device type...",
                    base_dev->efs_device_type, 0, 0);
      break;
  }

  if (result != 0)
  {
    FS_ERR_FATAL ("[%d] FS could not close device...", result, 0, 0);
  }

  return result;
}

#ifdef FEATURE_EFS_SETUP_DEVICE

#include "flash_ops_setup.h"

void
fs_device_setup_init (void)
{
  EFS_BAB_START (EFS_BAB_SETUP_INIT);
  flash_ops_init ();
  EFS_BAB_END (EFS_BAB_SETUP_INIT);
}

void
fs_device_setup_start (void)
{
  EFS_BAB_START (EFS_BAB_SETUP_START);
  flash_ops_start ();
  EFS_BAB_END (EFS_BAB_SETUP_START);
}

void
fs_device_setup_done (void)
{
  EFS_BAB_START (EFS_BAB_SETUP_DONE);
  flash_ops_end ();
  EFS_BAB_END (EFS_BAB_SETUP_DONE);
}

#endif /* FEATURE_EFS_SETUP_DEVICE */

int
fs_device_is_nand_device (efs_device_t base_dev)
{
  int is_nand = 0;

  switch (base_dev->efs_device_type)
  {
    case FS_DEVICE_TYPE_EFS_NAND:
    case FS_DEVICE_TYPE_EFS_ONE_NAND:
      is_nand = 1;
      break;

    default:
      is_nand = 0;
  }

  return is_nand;
}

int
fs_device_write_page (efs_device_t base_dev, fs_page_id page, void *data,
                      enum fs_flashop_category category)
{
  int result;
  uint32 write_time;
  fs_timetick start_tick, net_ticks;

  EFS_BAB_START (EFS_BAB_FLASH_WRITE_PAGE);
  EFS_BAB_VALUE (EFS_BAB_FLASH_WRITE_PAGE, page, TRUE, FALSE, TRUE);
  /* Make sure that the address of the buffer is meeting the alignment
     expectations of the DMA. */
  ASSERT (FS_OKAY_FOR_DM (data));

  start_tick = fs_timetick_get_timetick ();

  result = base_dev->write_page (base_dev, page, data);

  net_ticks = fs_timetick_get_timetick () - start_tick;
  write_time = (uint32) fs_timetick_timetick_diff_to_us (net_ticks);

  /* Update the counters.
     In case of SPI-NOR, where device page size is smaller than efs page size,
     the counter indicates the number at PM level and not the device level */
  fs_counters_update (FS_FOP_WRITE, category, 1, write_time);

#ifdef EFS_READ_BACK_VERIFY_PAGE
  if (result == FS_DEVICE_RESULT_SUCCESS)
  {
    uint32 page_size;
    result = base_dev->read_page (base_dev, page, verify_buffer);
    if (result != FS_DEVICE_RESULT_SUCCESS)
    {
      FS_ERR_FATAL ("[%d] Unable to read the written page", page, 0, 0);
    }

    page_size = base_dev->page_size (base_dev);
    if (memcmp (data, verify_buffer, page_size))
    {
      FS_ERR_FATAL ("[%d] Read data is different than written page", page,0,0);
    }
  }
#endif

  /* Update the read pointer cache data if page is cached. */
  fs_device_read_pointer_cache_update_page_data (page, data, 0,
                                             base_dev->page_size (base_dev));
  EFS_BAB_END (EFS_BAB_FLASH_WRITE_PAGE);
  return result;
}

int
fs_device_erase_block (efs_device_t base_dev, fs_block_id block,
    enum fs_flashop_category category)
{
  int result;
  uint32 erase_time;
  fs_timetick start_tick, net_ticks;

  EFS_BAB_START (EFS_BAB_FLASH_ERASE_BLOCK);
  EFS_BAB_VALUE (EFS_BAB_FLASH_ERASE_BLOCK, block, TRUE, FALSE, TRUE);

  start_tick = fs_timetick_get_timetick ();

  result = base_dev->erase_block (base_dev, block);

  net_ticks = fs_timetick_get_timetick () - start_tick;
  erase_time = (uint32) fs_timetick_timetick_diff_to_us (net_ticks);

  fs_counters_update (FS_FOP_ERASE, category, 1, erase_time);

  /* Invalidate the read cache data */
  fs_device_read_pointer_cache_invalidate_cache ();

  EFS_BAB_END (EFS_BAB_FLASH_ERASE_BLOCK);
  return result;
}

int
fs_device_read_page (efs_device_t base_dev, fs_page_id page,
                     void *data, enum fs_flashop_category category,
                     enum fs_flash_failure failure)
{
  int result;
  uint32 read_time;
  fs_timetick start_tick, net_ticks;

  EFS_BAB_START (EFS_BAB_FLASH_READ_PAGE);
  EFS_BAB_VALUE (EFS_BAB_FLASH_READ_PAGE, page, TRUE, FALSE, TRUE);

  /* Make sure that the address of the buffer is meeting the alignment
     expectations of the DMA. */
  ASSERT (FS_OKAY_FOR_DM (data));

  start_tick = fs_timetick_get_timetick ();

  result = base_dev->read_page (base_dev, page, data);

  net_ticks = fs_timetick_get_timetick () - start_tick;
  read_time = (uint32) fs_timetick_timetick_diff_to_us (net_ticks);

  /* Update the counters.
     In case of SPI-NOR, where device page size is smaller than efs page size,
     the counter indicates the number at PM level and not the device level */
  fs_counters_update (FS_FOP_READ, category, 1, read_time);

  if (failure == FS_FLASH_MUST_WORK && result != FS_DEVICE_RESULT_SUCCESS)
  {
    /* Read failures in NAND have several causes.  The most common are that
     * the page has not been written to since the last erase, or that power
     * was lost during the last write.  Because of the filesystem log,
     * there are only a few places in the filesystem where these are
     * expected to occur (such as reading the log), and these calls will
     * use FS_FLASH_FAIL_OK to distinguish this.  If we've gotten to this
     * failure, then the failure is due to a different cause that is
     * unexpected.
     *
     * Almost always, this indicates that the flash device driver is
     * malfunctioning.  It is possible to see this failure at end of life
     * for the part, but generally writes or erases fail first.  First line
     * of debugging is to figure out what is likely wrong with the flash
     * driver. */
    FS_ERR_FATAL ("[%d] Unexpected device read failure", page, 0, 0);
  }
  EFS_BAB_END (EFS_BAB_FLASH_READ_PAGE);
  return result;
}

int
fs_device_read_sectors (efs_device_t base_dev, uint32 sect, void *data,
                        uint32 num_sectors, enum fs_flashop_category category)
{
  fs_counters_update (FS_FOP_READ, category, num_sectors, 0);
  return base_dev->read_sectors (base_dev, sect, data, num_sectors);
}

int
fs_device_write_sectors (efs_device_t base_dev, uint32 sect, void *data,
                        uint32 num_sectors, enum fs_flashop_category category)
{
  fs_counters_update (FS_FOP_WRITE, category, num_sectors, 0);
  return base_dev->write_sectors (base_dev, sect, data, num_sectors);
}


int
fs_device_reliable_write_sectors (efs_device_t base_dev, uint32 sect,
                                  void *data, uint32 num_sectors,
                                  enum fs_flashop_category category)
{
  fs_counters_update (FS_FOP_WRITE, category, num_sectors, 0);
  return base_dev->reliable_write_sectors (base_dev, sect, data, num_sectors);
}

uint32
fs_device_block_size (efs_device_t base_dev)
{
  return base_dev->block_size (base_dev);
}

uint32
fs_device_page_size (efs_device_t base_dev)
{
  return base_dev->page_size (base_dev);
}

uint32
fs_device_block_count (efs_device_t base_dev)
{
  return base_dev->block_count (base_dev);
}

int
fs_device_bad_block_check (efs_device_t base_dev, fs_block_id block)
{
  return base_dev->bad_block_check (base_dev, block);
}

int
fs_device_is_erased (efs_device_t base_dev, fs_page_id page)
{
  return base_dev->is_erased (base_dev, page);
}

int
fs_device_mark_block_bad (efs_device_t base_dev, fs_block_id block)
{
  return base_dev->mark_block_bad (base_dev, block);
}

int
fs_device_begin_erase_block (efs_device_t base_dev, fs_block_id block,
                             enum fs_flashop_category category)
{
  int result;
  EFS_BAB_START (EFS_BAB_FLASH_BEGIN_ERASE);
  EFS_BAB_VALUE (EFS_BAB_FLASH_BEGIN_ERASE, block, TRUE, FALSE, TRUE);
  fs_counters_update (FS_FOP_ERASE_BEGIN, category, 1, 0);

  /* Invalidate the read cache data */
  fs_device_read_pointer_cache_invalidate_cache ();

  result = base_dev->begin_erase_block (base_dev, block);
  EFS_BAB_END (EFS_BAB_FLASH_BEGIN_ERASE);
  return result;
}

int
fs_device_suspend_erase (efs_device_t base_dev,
                         enum fs_flashop_category category)
{
  int result;
  uint32 erase_suspend_time;
  fs_timetick start_tick, net_ticks;

  EFS_BAB_START (EFS_BAB_FLASH_SUSPEND_ERASE);

  start_tick = fs_timetick_get_timetick ();

  result = base_dev->suspend_erase (base_dev);

  net_ticks = fs_timetick_get_timetick () - start_tick;
  erase_suspend_time = (uint32) fs_timetick_timetick_diff_to_us (net_ticks);

  fs_counters_update (FS_FOP_ERASE_SUSPEND, category, 1, erase_suspend_time);

  EFS_BAB_END (EFS_BAB_FLASH_SUSPEND_ERASE);
  return result;
}

int
fs_device_resume_erase (efs_device_t base_dev)
{
  int result;
  EFS_BAB_START (EFS_BAB_FLASH_RESUME_ERASE);

  result = base_dev->resume_erase (base_dev);

  EFS_BAB_END (EFS_BAB_FLASH_RESUME_ERASE);
  return result;
}

int
fs_device_erase_status (efs_device_t base_dev)
{
  return base_dev->erase_status (base_dev);
}


int
fs_device_partial_write (efs_device_t base_dev, fs_page_id page,
    void *data, int offset, int length, enum fs_flashop_category category)
{
  int result;

  EFS_BAB_START (EFS_BAB_FLASH_PARTIAL_WRITE);
  EFS_BAB_VALUE (EFS_BAB_FLASH_PARTIAL_WRITE, page, TRUE, FALSE, TRUE);
  /* In case of SPI-NOR, where device page size is smaller than efs page size,
     the counter indicates the number at PM level and not the device level */

  fs_counters_update (FS_FOP_WRITE_PARTIAL, category, 1, 0);
  /* Partial-writes are only possible on NOR-devices. NOR-devices allow
     partial-writes to a page provided if we are changing only 1's -> to 0's.
     Since we can write at any word in a page the buffer-pointer need not be
     aligned on a 32-byte address. */
  result = base_dev->partial_write (base_dev, page, data, offset, length);

  if (result == FS_DEVICE_RESULT_SUCCESS)
  {
#ifdef EFS_READ_BACK_VERIFY_PAGE
    result = base_dev->read_page (base_dev, page, verify_buffer);
    if (memcmp (verify_buffer + offset, data, length))
    {
      FS_ERR_FATAL ("[%d] Readdata is diff write-parti", page, 0, 0);
    }
#endif

    /* Update data in the read pointer cache as well if present. */
    fs_device_read_pointer_cache_update_page_data (page, data, offset, length);
  }

  EFS_BAB_END (EFS_BAB_FLASH_PARTIAL_WRITE);

  return result;
}


void *
fs_device_read_pointer (efs_device_t base_dev, fs_page_id page,
                        enum fs_flashop_category category)
{
  void *buff;

  EFS_BAB_START (EFS_BAB_FLASH_READ_POINTER);
  EFS_BAB_VALUE (EFS_BAB_FLASH_READ_POINTER, page, TRUE, FALSE, TRUE);

  buff = base_dev->read_pointer (base_dev, page);

  /* read pointer may not be supported on all NOR types, in which case read the
     whole page if not already read */
  if (buff == NULL)
  {
    buff = fs_device_read_pointer_cache_find_page (page);

    if (buff == NULL)
    {
      int result;

      buff = fs_device_read_pointer_cache_find_empty_buff (page);
      ASSERT (buff != NULL);

      result = fs_device_read_page (base_dev, page, buff, category,
               FS_FLASH_FAIL_OK);
      if (result != FS_DEVICE_RESULT_SUCCESS)
      {
        fs_device_read_pointer_cache_invalidate_page (page);
        buff = NULL;
      }
    }
  }

  EFS_BAB_END (EFS_BAB_FLASH_READ_POINTER);
  return buff;
}



static char*
fs_device_invalid_device_name (efs_device_t self)
{
  (void)self;
  return NULL;
}

static int
fs_device_invalid_device_maker_id (efs_device_t self)
{
  (void)self;
  return -1;
}

static int
fs_device_invalid_device_id (efs_device_t self)
{
  (void)self;
  return -1;
}

static uint32
fs_device_invalid_block_count (efs_device_t self)
{
  (void)self;
  return 0;
}

static uint32
fs_device_invalid_block_size (efs_device_t self)
{
  (void)self;
  return 0;
}

static uint32
fs_device_invalid_page_size (efs_device_t self)
{
  (void)self;
  return 0;
}

static uint32
fs_device_invalid_total_page_size (efs_device_t self)
{
  (void)self;
  return 0;
}


static uint32
fs_device_invalid_device_type (efs_device_t self)
{
  (void)self;
  return 0;
}



/* Bad block check for NAND devices */
static int
fs_device_invalid_bad_block_check (efs_device_t self, fs_block_id block)
{
  (void)self; (void)block;
  return -1;
}

/* Mark a block as bad for NAND device */
static int
fs_device_invalid_mark_block_bad (efs_device_t self, fs_block_id block)
{
  (void)self; (void)block;
  return -1;
}

/* Write operations. */
static int
fs_device_invalid_write_page (efs_device_t self, fs_page_id page, void *data)
{
  (void)self; (void)page; (void)data;
  return -1;
}

/* A full erase.  Eventually, will implement NOR-style partial erasing. */
static int
fs_device_invalid_erase_block (efs_device_t self, fs_block_id block)
{
  (void)self; (void)block;
  return -1;
}

/* Get the address of read data.  Returns NULL if this device doesn't
 * support direct mapping of read data. */
static void*
fs_device_invalid_read_pointer (efs_device_t self, fs_page_id page)
{
  (void)self; (void)page;
  return NULL;
}

/* Read the data into a buffer.  Can be used if the above returns NULL. */
static int
fs_device_invalid_read_page (efs_device_t self, fs_page_id page, void *data)
{
  (void)self; (void)page; (void)data;
  return -1;
}

/* Determine erase status of page */
static int
fs_device_invalid_is_erased (efs_device_t self, fs_page_id page)
{
  (void)self; (void)page;
  return -1;
}

static int
fs_device_invalid_partial_write (efs_device_t self, fs_page_id page,
                                 void *data, int offset, int length)
{
  (void)self; (void)page; (void)data; (void)offset; (void)length;
  return -1;
}


static int
fs_device_invalid_begin_erase_block (efs_device_t self, fs_block_id block)
{
  (void)self; (void)block;
  return -1;
}

static int
fs_device_invalid_suspend_erase (efs_device_t self)
{
  (void)self;
  return -1;
}

static int
fs_device_invalid_resume_erase (efs_device_t self)
{
  (void)self;
  return -1;
}

static int
fs_device_invalid_erase_status (efs_device_t self)
{
  (void)self;
  return -1;
}

static int
fs_device_invalid_is_block_erased (efs_device_t self, fs_block_id block)
{
  (void)self; (void)block;
  return -1;
}

static int
fs_device_invalid_read_spare_bytes (efs_device_t self, fs_page_id page,
                           uint32 spare_offset, uint32 len, void *data)
{
  (void)self; (void)page; (void)spare_offset; (void)len; (void)data;
  return -1;
}

static int
fs_device_invalid_read_mpages (efs_device_t self, fs_page_id start_page,
       fs_page_id num_pages, void *data, void (*wdog_kick_fn_ptr)(void))
{
  (void)self; (void)start_page; (void)num_pages; (void)data;
  (void)wdog_kick_fn_ptr;
  return -1;
}

static int
fs_device_invalid_copy_page (efs_device_t self, fs_page_id src_page,
                             fs_page_id dest_page)
{
  (void)self; (void)src_page; (void)dest_page;
  return -1;
}

static int
fs_device_invalid_lock_ops (efs_device_t self, fs_page_id start_page,
                            fs_page_id end_page, uint32 opcode)
{
  (void)self; (void)start_page; (void)end_page; (void)opcode;
  return -1;
}

static int
fs_device_invalid_get_spare_corrected_bytes (efs_device_t self)
{
  (void)self;
  return -1;
}

static int
fs_device_invalid_write_page_and_udata (efs_device_t self, fs_page_id page,
              void *page_data, void *spare_udata, uint32 spare_udata_blen)
{
  (void)self; (void)page; (void)page_data; (void)spare_udata;
  (void)spare_udata_blen;
  return -1;
}

static int
fs_device_invalid_read_spare_udata (efs_device_t self, fs_page_id page,
                            void *spare_udata, uint32 spare_udata_blen)
{
  (void)self; (void)page; (void)spare_udata; (void)spare_udata_blen;
  return -1;
}

static int
fs_device_invalid_read_page_and_udata (efs_device_t self, fs_page_id page,
                          void *data, void *spare, uint32 spare_udata_blen)
{
  (void)self; (void)page; (void)data; (void)spare; (void)spare_udata_blen;
  return -1;
}

static int
fs_device_invalid_open_partition (efs_device_t self, char *partition_name)
{
  (void)self; (void)partition_name;
  return -1;
}

static void
fs_device_invalid_set_ecc_state (efs_device_t self, int state)
{
  (void)self; (void)state;
}

static fs_device_write_style_t
fs_device_invalid_get_write_style (efs_device_t self)
{
  (void)self;
  return FS_DEVICE_WRITES_SIMPLE;
}

static int
fs_device_invalid_read_sectors (efs_device_t self, uint32 sect, void *data,
                                uint32 num_sectors)
{
  (void)self; (void)sect; (void)data; (void)num_sectors;
  return -1;
}

static int
fs_device_invalid_write_sectors (efs_device_t self, uint32 sect, void *data,
                                 uint32 num_sectors)
{
  (void)self; (void)sect; (void)data; (void)num_sectors;
  return -1;
}

/* Reliably write multiple sectors. */
static int
fs_device_invalid_reliable_write_sectors (efs_device_t self, uint32 sect,
                                          void *data, uint32 num_sectors)
{
  (void)self; (void)sect; (void)data; (void)num_sectors;
  return -1;
}

static enum fs_rmts_device_errno
fs_device_invalid_rmts_client_init (efs_device_t self, fs_os_sigs_type sig,
                                    uint32 timeout_in_ms)
{
  (void) self; (void) sig; (void) timeout_in_ms;
  return FS_RMTS_ERRNO_INVALID_OP;
}

static enum fs_rmts_modem_type
fs_device_invalid_rmts_get_rmts_modem_type (efs_device_t self)
{
  (void) self;
  return FS_RMTS_MODEM_TYPE_MAX;
}

static enum fs_rmts_device_errno
fs_device_invalid_rmts_read_iovec (efs_device_t self, fs_iovec_t iovec,
           uint32 iovec_cnt, enum fs_rmts_partition_id which_partition)
{
  (void) self; (void) iovec; (void) iovec_cnt; (void) which_partition;
  return FS_RMTS_ERRNO_INVALID_OP;
}

static enum fs_rmts_device_errno
fs_device_invalid_rmts_write_iovec (efs_device_t self, fs_iovec_t iovec,
            uint32 iovec_cnt, enum fs_rmts_partition_id which_partition)
{
  (void) self; (void) iovec; (void) iovec_cnt; (void) which_partition;
  return FS_RMTS_ERRNO_INVALID_OP;
}

static int
fs_device_invalid_rmts_get_last_errno (efs_device_t self)
{
  (void) self;
  return -1;
}


void
fs_device_init_invalid_ops (efs_device_t self)
{
  self->device_name = fs_device_invalid_device_name;
  self->device_maker_id = fs_device_invalid_device_maker_id;
  self->device_id = fs_device_invalid_device_id;
  self->block_count = fs_device_invalid_block_count;
  self->block_size = fs_device_invalid_block_size;
  self->page_size = fs_device_invalid_page_size;
  self->total_page_size = fs_device_invalid_total_page_size;
  self->device_type = fs_device_invalid_device_type;
  self->bad_block_check = fs_device_invalid_bad_block_check;
  self->mark_block_bad = fs_device_invalid_mark_block_bad;
  self->write_page = fs_device_invalid_write_page;
  self->erase_block = fs_device_invalid_erase_block;
  self->read_pointer = fs_device_invalid_read_pointer;
  self->read_page = fs_device_invalid_read_page;
  self->is_erased = fs_device_invalid_is_erased;
  self->partial_write = fs_device_invalid_partial_write;
  self->begin_erase_block = fs_device_invalid_begin_erase_block;
  self->suspend_erase = fs_device_invalid_suspend_erase;
  self->resume_erase = fs_device_invalid_resume_erase;
  self->erase_status = fs_device_invalid_erase_status;
  self->is_block_erased = fs_device_invalid_is_block_erased;
  self->read_spare_bytes = fs_device_invalid_read_spare_bytes;
  self->read_mpages = fs_device_invalid_read_mpages;
  self->copy_page = fs_device_invalid_copy_page;
  self->lock_ops = fs_device_invalid_lock_ops;
  self->get_spare_corrected_bytes =fs_device_invalid_get_spare_corrected_bytes;
  self->write_page_and_udata = fs_device_invalid_write_page_and_udata;
  self->read_spare_udata = fs_device_invalid_read_spare_udata;
  self->read_page_and_udata = fs_device_invalid_read_page_and_udata;
  self->open_partition = fs_device_invalid_open_partition;
  self->set_ecc_state = fs_device_invalid_set_ecc_state;
  self->get_write_style = fs_device_invalid_get_write_style;
  self->read_sectors = fs_device_invalid_read_sectors;
  self->write_sectors = fs_device_invalid_write_sectors;
  self->reliable_write_sectors = fs_device_invalid_reliable_write_sectors;
  self->rmts_client_init = fs_device_invalid_rmts_client_init;
  self->rmts_get_rmts_modem_type = fs_device_invalid_rmts_get_rmts_modem_type;
  self->rmts_read_iovec = fs_device_invalid_rmts_read_iovec;
  self->rmts_write_iovec = fs_device_invalid_rmts_write_iovec;
  self->rmts_get_last_errno = fs_device_invalid_rmts_get_last_errno;
}
