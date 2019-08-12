/***********************************************************************
 * fs_blockdev_sd_cprm.c
 *
 * Support for CPRM partition on SD Cards
 * Copyright (C) 2009-2010, 2014 QUALCOMM Technologies, Inc.
 *
 * All Secure Digital media cards contain a protected partition that
 * is accessible only through the use of CPRM.  This partition appears
 * as a unique block device.
 *
 ***********************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev_sd_cprm.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-01-29   vm    Changes for 64-bit compilation in boot.
2010-09-30   wek   Remove device iovec interface.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG_SD_CPRM

#include "fs_blockdev_sd_cprm_driver.h"
#include "fs_blockdev_sd_driver.h"
#include "assert.h"
#include "fs_err.h"
#include "fs_blockdev_i.h"
#include "sdcc_cprm.h"
#include "sdcc_api.h"
#include "CprmInterface.h"
#include "CprmProtectedFileSystem.h"
#include <string.h>

#ifdef FS_UNIT_TEST
  #include "sdcc_slot_config.h"
#endif


/* All blocks are 512 bytes, always */
#define SDCC_PROTECTED_BLOCK_SIZE 512

/* Maximum number of blocks to read or write in a single transaction.
   Because the data is being passed over RPC, we need a limit on how
   many blocks are passed in one call. */
#define MAX_READ_BLOCKS  1
#define MAX_WRITE_BLOCKS 1

/* On-target, the CPRM partition shares a physical device number with
 * the user portion of the SD card, and the CPRM calls are distinct.
 * Off-target, where we use the same calls for both, we use a
 * different devno to tell them apart. */
#ifdef FS_UNIT_TEST
  #define CPRM_DEVNO FS_SDCC_CPRM_DRIVE_NO
#else
  #define CPRM_DEVNO 0
#endif

/* Storage for SD specific data */
static sd_device_data sd_cprm_dev_data[] =
{
  {CPRM_DEVNO, 0, 0, NULL} /* driveno, is_open, was_present, handle */
};


typedef enum
{
  BLOCKDEV_SD_CPRM_MODE_READ_ANY_MKB  = 0,
  BLOCKDEV_SD_CPRM_MODE_READ_SAME_MKB = 1
} BLOCKDEV_SD_CPRM_MODE;


/* Device functions */
static int blockdev_sd_cprm_open (struct block_device*, void **, uint32);
static int blockdev_sd_cprm_close (struct block_device *, void*);
static int blockdev_sd_cprm_is_present (struct block_device *, void*);
static int blockdev_sd_cprm_read (struct block_device*, void*, uint32,
                            uint8*, uint16);
static int blockdev_sd_cprm_write_with_mode (struct block_device*, uint32,
                                        uint8*, uint16, BLOCKDEV_SD_CPRM_MODE);
static int blockdev_sd_cprm_write (struct block_device*, void*, uint32,
                                   uint8*, uint16);
static int blockdev_sd_cprm_user_write (struct block_device*, void*, uint32,
                                        uint8*, uint16);
static int blockdev_sd_cprm_get_size (struct block_device*, void*, uint32*,
                                      uint16*);
static int blockdev_sd_cprm_format (struct block_device*, void*);
static int blockdev_sd_cprm_reset (struct block_device*, void*);
static int blockdev_sd_cprm_reliable_write_fail (struct block_device*, void*,
                                                 uint32, uint32 *, uint32);
static int blockdev_sd_cprm_erase_fail (struct block_device*, void*, uint32,
                                        uint16);
static int blockdev_sd_cprm_get_device_info (struct block_device*, void*,
                                       struct block_device_info*);
static int blockdev_sd_cprm_device_idle_fail (struct block_device*, void*);
static uint32 blockdev_sd_cprm_get_entire_device_stub (struct block_device*,
                                                       void*);
static int blockdev_sd_cprm_is_embedded_device (struct block_device*, void*);
static int blockdev_sd_cprm_set_sps_end_point (struct block_device*, void*,
                                               void*);
static int blockdev_sd_cprm_get_sps_end_point (struct block_device*, void*,
                                               void*);
static int blockdev_sd_cprm_set_power_on_write_protection(struct block_device*,
                                                       void *, uint32, uint32);

static blockdev_ops sd_cprm_ops =
{
  blockdev_sd_cprm_open,                 /* open */
  blockdev_sd_cprm_close,                /* close */
  blockdev_sd_cprm_read,                 /* read */
  blockdev_sd_cprm_write,                /* write */
  blockdev_sd_cprm_user_write,           /* uwrite */
  blockdev_sd_cprm_reliable_write_fail,  /* reliable_write */
  blockdev_sd_cprm_erase_fail,           /* erase */
  blockdev_sd_cprm_reset,                /* reset */
  blockdev_sd_cprm_format,               /* device_format */
  blockdev_sd_cprm_is_present,           /* is_present */
  blockdev_sd_cprm_get_size,             /* get_device_size */
  blockdev_sd_cprm_get_device_info,      /* get_device_info */
  blockdev_sd_cprm_get_entire_device_stub, /* get_entire_device_id */
  blockdev_sd_cprm_device_idle_fail,     /* set_device_idle */
  blockdev_sd_cprm_is_embedded_device,   /* is_embedded_device */
  blockdev_sd_cprm_set_sps_end_point,
  blockdev_sd_cprm_get_sps_end_point,
  blockdev_sd_cprm_set_power_on_write_protection
};


void
blockdev_sd_cprm_device_data_init (void)
{
  uint32 total_devices, i;

  total_devices = sizeof (sd_cprm_dev_data) / sizeof (sd_cprm_dev_data[0]);
  for (i=0; i<total_devices; ++i)
  {
    sd_cprm_dev_data[i].is_open = 0;
    sd_cprm_dev_data[i].was_present = 0;
    sd_cprm_dev_data[i].handle = 0;
  }
}

blockdev_ops*
blockdev_sd_cprm_get_ops (void)
{
  return &sd_cprm_ops;
}

void*
blockdev_sd_cprm_get_device_data (uint32 idx)
{
  uint32 total_devices;

  total_devices = sizeof (sd_cprm_dev_data) / sizeof (sd_cprm_dev_data[0]);
  if (idx >= total_devices)
    FS_ERR_FATAL ("mismatch in device data...",0,0,0);

  return &sd_cprm_dev_data[idx];
}


/*======================================================================
 * FUNCTION     sd_cprm_open_device
 *
 * DESCRIPTION  Open the SD-card protected area.  Do nothing if it
 *    is already open.
 *
 * DEPENDENCIES     None
 *
 * PARAMETERS       bdev - Block device object
 *
 * RETURN VALUE     0  - Successful, or device was already open
 *                  -1 - Failed
 *
 * SIDE EFFECTS     Device is opened, and reads and writes will now work
 *
 *======================================================================*/
static int
blockdev_sd_cprm_open (struct block_device *bdev, void **handle, uint32 data)
{
  sd_device_data *dev = (sd_device_data *)bdev->device_data;
  uint32 partition = data;

  (void) handle;

  if (partition != 0)
    return -1;

  if (! dev->is_open)
  {
    /* The drive is not open.  Attempt to open it. */

    /* Check if there is an SD card in the drive */
    if (sdcc_cprm_does_sd_card_exist (dev->driveno))
    {
      /* Attempt to open this card */
      dev->is_open =
        (CprmInterfaceIsEnabled (dev->driveno) ==
         CPRMINTERFACE_SUCCESS) ? 1 : 0;
    }
  }

  if (dev->is_open)
    *handle = (void *)1;

  return dev->is_open ? 0 : -1;
}

/*======================================================================
 * FUNCTION     sd_cprm_close_device
 *
 * DESCRIPTION  Close the SD-card protected area.  Do nothing if it
 *    is already closed.
 *
 * DEPENDENCIES     None
 *
 * PARAMETERS       bdev - Block device object
 *
 * RETURN VALUE     0  - Successful, or device was already closed
 *                  -1 - Failed
 *
 * SIDE EFFECTS     Device is closed, and reads or writes to it will no
 *                  longer work
 *
 *======================================================================*/
static int
blockdev_sd_cprm_close (struct block_device *bdev, void *handle)
{
  sd_device_data *dev = (sd_device_data *)bdev->device_data;

  (void) handle;

  /* Do nothing, just close */
  dev->is_open = 0;

  return 0;
}

/*======================================================================
 * FUNCTION     sd_cprm_is_present
 *
 * DESCRIPTION  This will return that the protected area of the SD-card
 *  is present only if it has been successfully enabled for CPRM, at
 *  which point, CprmProtectedFileSystemIsEnabled will return TRUE.
 *
 * DEPENDENCIES     None
 *
 * PARAMETERS       bdev - Block device object
 *
 * RETURN VALUE     1 if the SD card exists
 *
 * SIDE EFFECTS     None
 *
 *======================================================================*/
static int
blockdev_sd_cprm_is_present (struct block_device *bdev, void *handle)
{
  sd_device_data *dev = (sd_device_data *)bdev->device_data;
  int cprm_enabled = CprmProtectedFileSystemIsEnabled (dev->driveno);

  (void) handle;

  /* If CPRM is enabled, also make sure that there is an SD card in the slot
     before reporting that the SD card is present. */
  if (cprm_enabled && !sdcc_cprm_does_sd_card_exist (dev->driveno))
  {
    /* If there was no SD card, disable CPRM so we don't keep checking */
    CprmProtectedFileSystemDisable (dev->driveno);
    cprm_enabled = 0;
  }

  /* If the card is present, and is not open, attempt to open it now.
     HFAT will make read calls to the card without calling open otherwise */
  if (cprm_enabled && !dev->is_open)
  {
    if (blockdev_sd_cprm_open (bdev, NULL, 0) != 0)
    {
      /* Something failed in the open call. */
      cprm_enabled = 0;
    }
  }

  return cprm_enabled ? 1 : 0;
}

/*======================================================================
 * FUNCTION     blockdev_sd_cprm_read
 *
 * DESCRIPTION  Read some data from the protected area of the SD-card
 *
 * DEPENDENCIES     The device must have been opened with blockdev_sd_cprm_open
 *
 * PARAMETERS       bdev - Block device object
 *
 *                  sector_number - Number of the block to read.  The
 *                     first block number is 0.
 *
 *                  p_buffer - Buffer to read the data into
 *
 *                  sector_count - Number of blocks to read.
 *
 * RETURN VALUE     0  - Successful
 *                  -1 - Failed
 *
 * SIDE EFFECTS     p_buffer receives the data read from the SD card
 *
 *======================================================================*/
static int
blockdev_sd_cprm_read (struct block_device *bdev, void *handle,
                  uint32 sector_number, uint8 * p_buffer, uint16 sector_count)
{
  int result = -1;
  uint32 block_offset = 0;
  sd_device_data *dev = (sd_device_data *)bdev->device_data;

  (void) handle;

  if (dev->is_open)
  {
    while (sector_count)
    {
      uint32 blocks_to_read = sector_count;

      /* Since this is going over RPC, we don't want to transfer a lot
         of blocks at once.  So limit it to MAX_BLOCKS_TO_WRITE per
         call. */
      if (blocks_to_read > MAX_READ_BLOCKS)
        blocks_to_read = MAX_READ_BLOCKS;

      // Address apparantly requires a block number
      result = CprmInterfaceProtectedRead
        (dev->driveno, // drive number
         sector_number + block_offset, // address
         p_buffer + block_offset * SDCC_PROTECTED_BLOCK_SIZE, // p_data
         blocks_to_read) ? 0 : -1;    // blockCount

      /* Exit early if any read failed */
      if (result == -1)
        break;

      sector_count -= blocks_to_read;
      block_offset += blocks_to_read;
    }

  }

  return result;
}

/*======================================================================
 * FUNCTION     blockdev_sd_cprm_write_with_mode
 *
 * DESCRIPTION  Write some data from the protected area of the SD-card
 *    This function allows specifying whether to allow reading what
 *    was written with any MKB that can get to the protected area,
 *    or only allowing the same MKB used to write it to read the data
 *    that is being written.
 *
 * DEPENDENCIES     blockdev_sd_cprm_open should have been called already
 *
 * PARAMETERS       bdev - Block device object
 *
 *                  sector_number - Number of the block to write.  The
 *                     first block number is 0.
 *
 *                  p_buffer - Buffer containing the data to write
 *
 *                  sector_count - Number of blocks to write.
 *
 * RETURN VALUE     0  - Successful
 *                  -1 - Failed
 *
 * SIDE EFFECTS     Data is written to the disk
 *
 *======================================================================*/
static int
blockdev_sd_cprm_write_with_mode (struct block_device *bdev,
                            uint32 sector_number,
                            uint8 * p_buffer,
                            uint16 sectors_to_write,
                            BLOCKDEV_SD_CPRM_MODE mode)
{
  int result = -1;
  uint32 block_offset = 0;
  sd_device_data *dev = (sd_device_data *)bdev->device_data;

  if (dev->is_open)
  {
    while (sectors_to_write)
    {
      uint32 blocks_to_write = sectors_to_write;

      /* Since this is going over RPC, we don't want to transfer a lot
         of blocks at once.  So limit it to MAX_BLOCKS_TO_WRITE per
         call. */
      if (blocks_to_write > MAX_WRITE_BLOCKS)
        blocks_to_write = MAX_WRITE_BLOCKS;

      result = CprmInterfaceProtectedWrite
        (dev->driveno,         //drive number
         sector_number + block_offset, //address
         p_buffer + block_offset * SDCC_PROTECTED_BLOCK_SIZE, //p_data
         blocks_to_write,       //blockCount
         mode) ? 0 : -1;        //mode

      /*Exit early if any write failed */
      if (result == -1)
        break;

      sectors_to_write -= blocks_to_write;
      block_offset += blocks_to_write;
    }
  }

  return result;
}


/*======================================================================
 * FUNCTION     blockdev_sd_cprm_write
 *
 * DESCRIPTION  Write some data from the protected area of the SD-card
 *   This defaults to allowing any MKB to read the data that was written.
 *   This is the correct setting for file system data or directory data,
 *   but writes of data to files should use sd_cprm_write_with_mode
 *   and specify READ_SAME_MKB as the mode.
 *
 * DEPENDENCIES     sd_cprm_open should have been called already
 *
 * PARAMETERS       bdev - Block device object
 *
 *                  sector_number - Number of the block to write.  The
 *                     first block number is 0.
 *
 *                  p_buffer - Buffer containing the data to write
 *
 *                  sector_count - Number of blocks to write.
 *
 * RETURN VALUE     0  - Successful
 *                  -1 - Failed
 *
 * SIDE EFFECTS     Data is written to the disk
 *
 *======================================================================*/
static int
blockdev_sd_cprm_write (struct block_device *bdev, void *handle,
                        uint32 sector_number, uint8 *p_buffer,
                        uint16 sectors_to_write)
{
  (void) handle;
  // By default, do writes that allow read back with any MKB
  return blockdev_sd_cprm_write_with_mode (bdev,
                                     sector_number,
                                     p_buffer,
                                     sectors_to_write,
                                     BLOCKDEV_SD_CPRM_MODE_READ_ANY_MKB);
}

/*======================================================================
 * FUNCTION     blockdev_sd_cprm_user_write
 *
 * DESCRIPTION  Write some data from the protected area of the SD-card
 *              This function writes data that can only be read with same MKB
 *
 * DEPENDENCIES     blockdev_sd_cprm_open should have been called already
 *
 * PARAMETERS       bdev - Block device object
 *
 *                  sector_number - Number of the block to write.  The
 *                     first block number is 0.
 *
 *                  p_buffer - Buffer containing the data to write
 *
 *                  sector_count - Number of blocks to write.
 *
 * RETURN VALUE     0  - Successful
 *                  -1 - Failed
 *
 * SIDE EFFECTS     Data is written to the disk
 *
 *======================================================================*/
static int
blockdev_sd_cprm_user_write (struct block_device *bdev, void *handle,
                       uint32 sector_number, uint8 *p_buffer,
                       uint16 sectors_to_write)
{
  (void) handle;
  // Do writes that allow read back with same MKB
  return blockdev_sd_cprm_write_with_mode (bdev,
                                     sector_number,
                                     p_buffer,
                                     sectors_to_write,
                                     BLOCKDEV_SD_CPRM_MODE_READ_SAME_MKB);
}

/*======================================================================
 * FUNCTION     sd_cprm_get_size
 *
 * DESCRIPTION  Get the size of the SD-card's protected area.  The size
 *   is returned in the parameters p_blocks and p_bytes_per_block
 *
 * PARAMETERS       bdev - Block device object
 *
 *                  p_blocks - Gets the number of blocks in the protected area
 *
 *                  p_bytes_per_block - Gets the block size in bytes
 *
 * RETURN VALUE     0  - Successful
 *                  -1 - Failed
 *
 *                  Fails if there is a problem reading the protected area
 *                  size, or if the size is not a multiple of the block size.
 *
 * SIDE EFFECTS     p_blocks and p_bytes_per_block recieve the size information
 *
 *======================================================================*/
static int
blockdev_sd_cprm_get_size (struct block_device *bdev, void *handle,
                           uint32 *p_blocks, uint16 *p_bytes_per_block)
{
  int result;
  uint32 protected_area_size;   //in bytes
  sd_device_data *dev = (sd_device_data *)bdev->device_data;

  (void) handle;

  /* Fail if either pointer is NULL */
  if (!p_blocks || !p_bytes_per_block)
    return -1;

  /* Fail if the SD card does not exist */
  if (!sdcc_cprm_does_sd_card_exist (dev->driveno))
    return -1;

  /* Get the size of the protected area */
  result = sdcc_cprm_get_protected_area_size (dev->driveno,
                                              &protected_area_size);

  /* Check that the result is valid and divisible by the block size */
  if (!result || ((protected_area_size % SDCC_PROTECTED_BLOCK_SIZE) != 0))
    return -1;

  /* Protected area blocks are always 512 bytes */
  *p_bytes_per_block = SDCC_PROTECTED_BLOCK_SIZE;

  /* The size was returned in bytes */
  *p_blocks = protected_area_size / SDCC_PROTECTED_BLOCK_SIZE;

  /* Successful */
  return 0;
}

/*======================================================================
 * FUNCTION     blockdev_sd_cprm_format
 *
 * DESCRIPTION  This callback checks rather or not it is ok to format
 *   the device.  It does not actually format it.  We will always
 *   return failure since we do not want the user to be able to
 *   format the device.
 *
 * PARAMETERS       bdev - Block device object
 *
 * RETURN VALUE     0  - Ok to format
 *                  -1 - Not Ok to format
 *
 *======================================================================*/
static int
blockdev_sd_cprm_format (struct block_device *bdev, void *handle)
{
  (void) bdev; (void) handle;
   return -1; /* Do not allow formatting the protected area of the SD card */
}

static int
blockdev_sd_cprm_reset (struct block_device *bdev, void *handle)
{
  (void) bdev; (void) handle;
  return 0;
}

static int
blockdev_sd_cprm_reliable_write_fail (struct block_device *bdev, void *handle,
                                  uint32 lba, uint32 *buf, uint32 num_sectors)
{
  (void) bdev; (void) handle; (void) lba;
  (void) buf; (void) num_sectors;
  return -1;
}

static int
blockdev_sd_cprm_erase_fail (struct block_device *bdev, void *handle,
                             uint32 lba, uint16 n_to_erase)
{
  (void) bdev; (void) handle; (void) lba; (void) n_to_erase;
  return -1;
}

static int
blockdev_sd_cprm_get_device_info (struct block_device *bdev, void *handle,
                                  struct block_device_info *info)
{
  uint32 total_blocks;
  uint16 block_size;
  int result;

  (void) bdev; (void) handle; (void) info;

  memset (info, 0, sizeof (struct block_device_info));

  result = blockdev_sd_cprm_get_size (bdev, handle, &total_blocks,
                                      &block_size);
  if (result == 0)
  {
    info->card_size_in_sectors = total_blocks;
    info->bytes_per_sector = block_size;
  }

  return result;
}

static uint32
blockdev_sd_cprm_get_entire_device_stub (struct block_device *bdev,
                                         void *handle)
{
  (void) bdev; (void) handle;
  return 0;
}

static int
blockdev_sd_cprm_device_idle_fail (struct block_device *bdev, void *handle)
{
  (void) bdev; (void) handle;
  return -1;
}

static int
blockdev_sd_cprm_is_embedded_device (struct block_device *bdev, void *handle)
{
  (void) bdev; (void) handle;
  return 0; /* External device. */
}

static int
blockdev_sd_cprm_set_sps_end_point (struct block_device *bdev, void *handle,
                              void *param)
{
  (void) bdev; (void) handle; (void) param;
  return -1;
}

static int
blockdev_sd_cprm_get_sps_end_point (struct block_device *bdev, void *handle,
                              void *param)
{
  (void) bdev; (void) handle; (void) param;
  return -1;
}

static int
blockdev_sd_cprm_set_power_on_write_protection (struct block_device *bdev,
                      void *handle, uint32 start_block, uint32 num_blocks)
{
  (void) bdev; (void) handle; (void) start_block; (void) num_blocks;
  return -1;
}


#endif /* BLOCKDEV_SD_CPRM */

