/***********************************************************************
 * fs_device_otp_flash.c
 *
 * OTP (One-Time-Programmable) EFS Device layer.
 * Copyright (C) 2011 QUALCOMM Technologies, Inc.
 *
 * This file abstracts the OTP device to the EFS module.
 * The fs_use_otp variable can be set to debug the EFS OTP module using
 * the FOTA (Firmware Over the Air) partition.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_device_otp_flash.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2011-08-17   rp    Create

===========================================================================*/

#include "fs_config_i.h"
#include "fs_device_otp_flash.h"

#ifdef FEATURE_EFS_OTP

#include "fs_otp_config.h"
#include "fs_err.h"

#include "assert.h"

#include <string.h>


volatile int fs_use_otp = 1;
#define OTP_FOTA_PARTI_NAME     "0:FOTA"

#define FS_DEVICE_OTP_MAX_DEVICES         1
static struct efs_device fs_otp_devices[FS_DEVICE_OTP_MAX_DEVICES];


void
fs_device_otp_flash_init (void)
{
  memset (fs_otp_devices, 0, sizeof (fs_otp_devices));
}

efs_device_t
fs_device_otp_flash_open (uint32 which_device)
{
  efs_device_t base_dev;
  fs_device_t otp_flash_dev;
  char *parti_name;

  if (which_device >= FS_DEVICE_OTP_MAX_DEVICES)
  {
    FS_ERR_FATAL ("[%d-%d]Max device limit exceeded...", which_device,
                  FS_DEVICE_OTP_MAX_DEVICES, 0);
  }

  parti_name = (fs_use_otp == 1) ? FS_OTP_PARTITION_NAME : OTP_FOTA_PARTI_NAME;
  ASSERT (parti_name != NULL);

  otp_flash_dev = flash_open (parti_name, 0);
  if (otp_flash_dev == FS_NO_DEVICE)
    return NULL;

  base_dev = &fs_otp_devices[which_device];
  base_dev->efs_device_type = FS_DEVICE_TYPE_OTP_FLASH;
  fs_device_otp_flash_init_ops (base_dev);
  base_dev->priv.otp_flash = otp_flash_dev;

  return base_dev;
}

int
fs_device_otp_flash_close (efs_device_t base_dev)
{
  int result = -1;
  result = flash_close (base_dev->priv.otp_flash);
  base_dev->priv.otp_flash = NULL;
  return result;
}

static uint32
fs_device_otp_block_size (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.otp_flash;
  return flash_dev->block_size (flash_dev);
}

static uint32
fs_device_otp_page_size (efs_device_t base_dev)
{
  fs_device_t flash_dev = base_dev->priv.otp_flash;
  return flash_dev->page_size (flash_dev);
}

static int
fs_device_otp_write_page (efs_device_t base_dev, fs_page_id page, void *data)
{
  fs_device_t flash_dev = base_dev->priv.otp_flash;
  ASSERT (FS_OKAY_FOR_DM (data));
  return flash_dev->write_page (flash_dev, page, data);
}

static int
fs_device_otp_read_page (efs_device_t base_dev, fs_page_id page, void *data)
{
  fs_device_t flash_dev = base_dev->priv.otp_flash;
  ASSERT (FS_OKAY_FOR_DM (data));
  return flash_dev->read_page (flash_dev, page, data);
}

void
fs_device_otp_flash_init_ops (efs_device_t base_dev)
{
  fs_device_init_invalid_ops (base_dev);

  base_dev->block_size = fs_device_otp_block_size;
  base_dev->page_size = fs_device_otp_page_size;
  base_dev->write_page = fs_device_otp_write_page;
  base_dev->read_page = fs_device_otp_read_page;
}

#endif /* FEATURE_EFS_OTP */
