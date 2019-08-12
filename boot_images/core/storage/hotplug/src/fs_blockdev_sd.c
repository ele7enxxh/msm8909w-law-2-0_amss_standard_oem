/***********************************************************************
 * fs_blockdev_sd.c
 *
 * Blockdev SD device.
 * Copyright (C) 2009-2010,2012-2014 QUALCOMM Technologies, Inc.
 *
 * Implementation for the blockdev_ops functions specific to
 * the SD driver.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_blockdev_sd.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-04-28   vm    Use FS_MSG macro in place of MSG macro.
2014-01-29   vm    Changes for 64-bit compilation in boot.
2013-11-20   rp    Detect embedded MMC-devices without opening them.
2013-10-09   nr    Replace banned memory functions with safer versions.
2012-11-09   nr    Add dummy slot for rmts on simulator.
2012-01-23   rp    Remove sdcc_iovec array.
2012-01-17   wek   Remove unused header files to compile for boot.
2010-09-30   wek   Remove device iovec interface.
2010-05-07   rp    Allow null buffers to pass to sdcc_handle_read().
2010-05-06   rp    Allow null buffers to pass to sdcc_handle_write().
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG_SD

#include "fs_blockdev_sd_driver.h"
#include "sdcc_api.h"
#include "fs_babylon.h"
#include "assert.h"
#include "fs_err.h"
#include "fs_util.h"

#include <string.h>

#ifdef FS_UNIT_TEST
  #include "sdcc_slot_config.h"
#endif

/* Maximum number of SD-Slots present in this device. */
static int max_sd_slots;

struct sd_device_data
{
  int     driveno;
};

/* Storage for SD specific data */
static struct sd_device_data sd_dev_data[] =
{
  {0 }, /* driveno */
  {1 },
  {2 },
  {3 }
#ifdef FS_UNIT_TEST
  /* Phony mnand device for on target testing */
  ,{FS_SDCC_MNAND_DRIVE_NO }
  ,{FS_SDCC_RMTS_DRIVE_NO }
#endif /* FS_UNIT_TEST */
};


/* Device operations */
static int blockdev_sd_open (struct block_device*, void**, uint32);
static int blockdev_sd_close (struct block_device*, void*);
static int blockdev_sd_read (struct block_device*, void*, uint32,
                       unsigned char*, uint16);
static int blockdev_sd_write (struct block_device*, void*, uint32,
                        unsigned char*, uint16);
static int blockdev_sd_reset (struct block_device*, void*);
static int blockdev_sd_success_stub (struct block_device*, void*);
static int blockdev_sd_erase_fail (struct block_device*, void*, uint32,uint16);
static int blockdev_sd_is_present (struct block_device*, void*);
static int blockdev_sd_get_device_size (struct block_device*, void*,
                                        uint32*,uint16*);
static int blockdev_sd_get_device_info (struct block_device*, void*,
                                  struct block_device_info*);
static int blockdev_sd_reliable_write (struct block_device*, void*, uint32,
                                       uint32*, uint32);
static uint32 blockdev_sd_get_entire_device_id (struct block_device*, void*);
static int blockdev_sd_set_device_idle (struct block_device*, void*);
static int blockdev_sd_is_embedded_device (struct block_device*, void*);
static int blockdev_sd_set_sps_end_point (struct block_device*,void*, void*);
static int blockdev_sd_get_sps_end_point (struct block_device*,void*, void*);
static int blockdev_sd_set_power_on_write_protection (struct block_device*,
                                                    void*, uint32, uint32);


/* List of all device operations */
static blockdev_ops sd_ops =
{
  blockdev_sd_open,
  blockdev_sd_close,
  blockdev_sd_read,
  blockdev_sd_write,
  blockdev_sd_write,
  blockdev_sd_reliable_write,
  blockdev_sd_erase_fail,
  blockdev_sd_reset,
  blockdev_sd_success_stub,
  blockdev_sd_is_present,
  blockdev_sd_get_device_size,
  blockdev_sd_get_device_info,
  blockdev_sd_get_entire_device_id,
  blockdev_sd_set_device_idle,
  blockdev_sd_is_embedded_device,
  blockdev_sd_set_sps_end_point,
  blockdev_sd_get_sps_end_point,
  blockdev_sd_set_power_on_write_protection
};


void
blockdev_sd_device_data_init (void)
{
}

blockdev_ops*
blockdev_sd_get_ops (void)
{
  return &sd_ops;
}

void*
blockdev_sd_get_device_data (uint32 idx)
{
  uint32 total_devices;

  total_devices = sizeof (sd_dev_data) / sizeof (sd_dev_data[0]);
  if (idx >= total_devices)
    FS_ERR_FATAL ("mismatch in device data...",0,0,0);

  return &sd_dev_data[idx];
}

int
blockdev_sd_get_driveno (struct block_device *bdev)
{
  struct sd_device_data *sd_data;

  ASSERT (bdev != NULL);

  sd_data = bdev->device_data;
  ASSERT (sd_data != NULL);

  return sd_data->driveno;
}

static int
blockdev_sd_open (struct block_device *bdev, void **handle, uint32 data)
{
  struct sdcc_device *sdcc_handle;
  struct sd_device_data *sd_data;
  uint32 phy_parti;
  int result;

  ASSERT (bdev != NULL);
  ASSERT (handle != NULL);

  if (*handle != NULL)
    return 0;

  sd_data = bdev->device_data;
  ASSERT (sd_data != NULL);

  phy_parti = data;

  sdcc_handle = sdcc_handle_open (sd_data->driveno, phy_parti);
  *handle = (void *)sdcc_handle;

  result = (sdcc_handle == NULL) ? -1 : 0;
  return result;
}

static int
blockdev_sd_close (struct block_device *bdev, void *handle)
{
  struct sdcc_device *sdcc_handle = (struct sdcc_device *)handle;
  int result;
  SDCC_STATUS sdcc_result;

  ASSERT (bdev != NULL);
  if (sdcc_handle == NULL)
    return 0;

  sdcc_result = sdcc_handle_close (sdcc_handle);

  result = (sdcc_result == SDCC_NO_ERROR) ? 0 : -1;
  return result;
}

static int
blockdev_sd_read (struct block_device *bdev, void *handle, uint32 lba,
            unsigned char *buf, uint16 n_to_read)
{
  struct sdcc_device *sdcc_handle = (struct sdcc_device *)handle;
  int result;
  SDCC_STATUS sdcc_result;

  ASSERT (bdev != NULL);
  ASSERT (sdcc_handle != NULL);

  sdcc_result = sdcc_handle_read (sdcc_handle, lba, buf, n_to_read);

  result = (sdcc_result == SDCC_NO_ERROR) ? 0 : -1;
  return result;
}

static int
blockdev_sd_write (struct block_device *bdev, void *handle, uint32 lba,
             unsigned char *buf, uint16 n_to_write)
{
  struct sdcc_device *sdcc_handle = (struct sdcc_device *)handle;
  int result;
  SDCC_STATUS sdcc_result;

  ASSERT (bdev != NULL);
  ASSERT (sdcc_handle != NULL);

  sdcc_result = sdcc_handle_write (sdcc_handle, lba, buf, n_to_write);

  result = (sdcc_result == SDCC_NO_ERROR) ? 0 : -1;
  return result;
}

static int
blockdev_sd_reliable_write (struct block_device *bdev, void *handle,
                            uint32 lba, uint32 *buf, uint32 num_sectors)
{
#ifdef FEATURE_EFS_BOOT_ENV
  (void) bdev; (void) handle; (void) lba; (void) buf; (void) num_sectors;
  return -1;
#else
  struct sdcc_device *sdcc_handle = (struct sdcc_device *)handle;
  sdcc_memory_iovec_data_desc_type sdcc_iovec[1];
  int result;
  SDCC_STATUS sdcc_result;

  ASSERT (bdev != NULL);
  ASSERT (sdcc_handle != NULL);

  sdcc_iovec[0].sector_address = lba;
  sdcc_iovec[0].buffer = buf;
  sdcc_iovec[0].num_sector = num_sectors;

  EFS_BAB_START (EFS_BAB_SDCC_WRITE_REL);
  EFS_BAB_VALUE (EFS_BAB_SDCC_WRITE_REL, lba, TRUE, FALSE, TRUE);
  EFS_BAB_VALUE (EFS_BAB_SDCC_WRITE_REL, num_sectors, TRUE, FALSE, FALSE);

  sdcc_result = sdcc_handle_write_iovec (sdcc_handle, sdcc_iovec, 1,
                                         SDCC_MEMORY_IOVEC_RELIABLE_WRITE);

  EFS_BAB_END (EFS_BAB_SDCC_WRITE_REL);

  if (sdcc_result != SDCC_NO_ERROR)
    FS_MSG_HIGH_0 ("sdcc_write_iovec failed...");

  result = (sdcc_result == SDCC_NO_ERROR) ? 0 : -1;
  return result;
#endif
}

static int
blockdev_sd_erase_fail (struct block_device *bdev, void *handle, uint32 lba,
                        uint16 erase_len)
{
  (void) bdev; (void) handle; (void) lba; (void) erase_len;
  return -1;                    /* Unsupported */
}


/* Get the total number of SD-Slots present in this target, If we can't
   figure out this using the SDCC API, then we shall assume that there
   is just 1 SD slot. */
static int
blockdev_sd_get_total_slots (void)
{
  sdcc_config_info_type sdcc_pconfig;
  int num_slots;

  /* Find out the total number of SD slots that are present on-target. */
  if (sdcc_get_config_info (&sdcc_pconfig) != SDCC_NO_ERROR)
    FS_ERR_FATAL ("sdcc_get_config_info() failed...",0,0,0);

  num_slots = (int )sdcc_pconfig.num_slots;
  return num_slots;
}


static int
blockdev_sd_reset (struct block_device *bdev, void *handle)
{
  struct sd_device_data *sd_data;

  (void) handle;

  ASSERT (bdev != NULL);
  sd_data = bdev->device_data;
  ASSERT (sd_data != NULL);

  max_sd_slots = blockdev_sd_get_total_slots ();
  if (sd_data->driveno >= max_sd_slots)
  {
    /* This slot is not present, hence return -1 so that the hotplug-manager
       marks this device as DEV_UNUSABLE and will not poll this device in the
       future. */
    return -1;
  }

  /* If this slot is enabled, but is not compatible for memory card
   * then also mark this device as not usable. */
  if (sdcc_slot_has_support (sd_data->driveno, SDCC_SLOT_MEMORY) == FALSE)
    return -1;

  return 0;
}

static int
blockdev_sd_success_stub (struct block_device *bdev, void *handle)
{
  (void) bdev; (void) handle;
  return 0;
}

static int
blockdev_sd_is_present (struct block_device *bdev, void *handle)
{
  struct sdcc_device *sdcc_handle = (struct sdcc_device *)handle;
  boolean is_present;
  int result;

  ASSERT (bdev != NULL);
  ASSERT (sdcc_handle != NULL);

  is_present = sdcc_handle_is_mem_present (sdcc_handle);
  result = (is_present == TRUE) ? 1 : 0;
  return result;
}


static int
blockdev_sd_get_device_size (struct block_device *bdev, void *handle,
                             uint32 *blocks, uint16 *bytes_per_block)
{
  struct sdcc_device *sdcc_handle = (struct sdcc_device *)handle;
  sdcc_mem_info_type mem_info;
  SDCC_STATUS sdcc_result;
  int result;

  ASSERT (bdev != NULL);
  ASSERT (sdcc_handle != NULL);
  ASSERT (blocks != NULL);
  ASSERT (bytes_per_block != NULL);

  sdcc_result = sdcc_handle_mem_get_device_info (sdcc_handle, &mem_info);
  if (sdcc_result == SDCC_NO_ERROR)
  {
    *blocks = mem_info.card_size_in_sectors;
    *bytes_per_block = mem_info.block_len;
  }

  result = (sdcc_result == SDCC_NO_ERROR) ? 0 : -1;
  return result;
}

static int
blockdev_sd_get_device_info (struct block_device *bdev, void *handle,
                             struct block_device_info *dev_info)
{
  struct sdcc_device *sdcc_handle = (struct sdcc_device *)handle;
  int result;
  sdcc_mem_info_type sdcc_info;
  SDCC_STATUS sdcc_res;

  ASSERT (bdev != NULL);
  ASSERT (sdcc_handle != NULL);

  sdcc_res = sdcc_handle_mem_get_device_info (sdcc_handle, &sdcc_info);
  if (sdcc_res == SDCC_NO_ERROR)
  {
    memset (dev_info, 0, sizeof (*dev_info));
    dev_info->manufacturer_id = sdcc_info.mfr_id;
    dev_info->oem_id = sdcc_info.oem_id;
    dev_info->prod_rev = sdcc_info.prod_rev;
    dev_info->prod_serial_num = sdcc_info.prod_serial_num;
    dev_info->card_size_in_sectors = sdcc_info.card_size_in_sectors;
    dev_info->bytes_per_sector = sdcc_info.block_len;
    dev_info->phy_partition_type = sdcc_info.phy_partition_type;
    fs_memscpy (dev_info->product_name, sizeof (dev_info->product_name),
                sdcc_info.prod_name,
                MIN (sizeof (dev_info->product_name),
                sizeof (sdcc_info.prod_name)));

    fs_memscpy (dev_info->manufactured_date,
                sizeof (dev_info->manufactured_date),
                sdcc_info.mfr_date,
                MIN (sizeof (dev_info->manufactured_date),
                sizeof (sdcc_info.mfr_date)));
  }

  result = (sdcc_res == SDCC_NO_ERROR) ? 0 : -1;
  return result;
}


uint32
blockdev_sd_get_entire_device_id (struct block_device *bdev, void *handle)
{
  (void) bdev; (void) handle;
  return SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE;
}

static int
blockdev_sd_set_device_idle (struct block_device *bdev, void *handle)
{
  int result = -1;
  SDCC_STATUS sdcc_res;
  struct sd_device_data *dev = (struct sd_device_data *)bdev->device_data;

  (void) handle;
  ASSERT (dev->driveno < max_sd_slots);

  sdcc_res = sdcc_activity_timeout_event(dev->driveno);
  result = (sdcc_res == SDCC_NO_ERROR) ? 0 : -1;
  return result;
}

static int
blockdev_sd_is_embedded_device (struct block_device *bdev, void *handle)
{
  int is_embedded_device;
  SDCC_STATUS sdcc_res;

  (void) handle;

  ASSERT (bdev != NULL);

  is_embedded_device = -1;

#ifdef FEATURE_SDCC_GET_SLOT_TYPE_SUPPORTED
{
  struct sd_device_data *dev = (struct sd_device_data *)bdev->device_data;
  sdcc_slot_mem_type mem_slot;
  sdcc_slot_access_type dev_locn;

  sdcc_res = sdcc_get_slot_type (dev->driveno, &mem_slot, &dev_locn);
  if (sdcc_res != SDCC_NO_ERROR)
    goto End;

  if ((mem_slot == SDCC_SLOT_EMMC) || (mem_slot == SDCC_SLOT_SD))
  {
    switch (dev_locn)
    {
      case SDCC_SLOT_INTERNAL:
        is_embedded_device = 1;
        break;

      case SDCC_SLOT_EXTERNAL:
        is_embedded_device = 0;
        break;

      case SDCC_SLOT_INVALID:
      default:
        break;
    }
  }
}
#else /* FEATURE_SDCC_GET_SLOT_TYPE_SUPPORTED */
{
  struct sdcc_device *sdcc_handle = (struct sdcc_device *)handle;
  sdcc_mem_info_type sdcc_info;

  if (sdcc_handle == NULL)
    goto End;

  sdcc_res = sdcc_handle_mem_get_device_info (sdcc_handle, &sdcc_info);
  if (sdcc_res != SDCC_NO_ERROR)
    goto End;

  is_embedded_device = (sdcc_info.is_embedded_device == TRUE) ? 1 : 0;
}
#endif /* FEATURE_SDCC_GET_SLOT_TYPE_SUPPORTED */

End:
  return is_embedded_device;
}


static int
blockdev_sd_set_sps_end_point (struct block_device *bdev, void *handle,
                               void *param)
{
#ifdef FEATURE_EFS_BOOT_ENV
  (void) bdev; (void) handle; (void) param;
  return -1;
#else
  struct sdcc_device *sdcc_handle = (struct sdcc_device *)handle;
  SDCC_STATUS sdcc_res;
  int result;

  ASSERT (bdev != NULL);
  if (sdcc_handle == NULL)
    return -1;

  sdcc_res = sdcc_handle_set_sps_end_point (sdcc_handle, param);
  result = (sdcc_res == SDCC_NO_ERROR) ? 0 : -1;
  return result;
#endif
}

static int
blockdev_sd_get_sps_end_point (struct block_device *bdev, void *handle,
                               void *param)
{
#ifdef FEATURE_EFS_BOOT_ENV
  (void) bdev; (void) handle; (void) param;
  return -1;
#else
  struct sdcc_device *sdcc_handle = (struct sdcc_device *)handle;
  SDCC_STATUS sdcc_res;
  int result;

  ASSERT (bdev != NULL);
  if (sdcc_handle == NULL)
    return -1;

  sdcc_res = sdcc_handle_get_sps_end_point (sdcc_handle, param);
  result = (sdcc_res == SDCC_NO_ERROR) ? 0 : -1;
  return result;
#endif
}

static int
blockdev_sd_set_power_on_write_protection (struct block_device *bdev,
                 void *handle, uint32 start_block, uint32 num_blocks)
{
  struct sdcc_device *sdcc_handle = (struct sdcc_device *)handle;
  SDCC_STATUS sdcc_res;
  int result;

  ASSERT (bdev != NULL);
  if (sdcc_handle == NULL)
    return -1;

  sdcc_res = sdcc_handle_set_power_on_write_protection (sdcc_handle,
                                           start_block, num_blocks);
  result = (sdcc_res == SDCC_NO_ERROR) ? 0 : -1;
  return result;
}


#endif /* FEATURE_FS_HOTPLUG_SD */
