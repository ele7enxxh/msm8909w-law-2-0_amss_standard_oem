/***********************************************************************
 * fs_hotplug_stub.c
 *
 * Stubs for all the hotplug APIs.
 * Copyright (C) 2009-2010,2013-2014 QUALCOMM Technologies, Inc.
 *
 * When Hotplug (FEATURE_FS_HOTPLUG) is turned off, then this file will provide
 * dummy implementation for all the Hotplug APIs.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_stub.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-06-12   wek   Bringup of hotplug for XBL.
2013-05-10   wek   Fix compile time errors when hotplug is turned off.
2010-02-24   rp    Added hotplug_open_device_by_partition_type API.
2010-02-19   rp    Maintain same API files between 1.2 and 2.0 codebases.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-12-10   rp    Do sector translation in write protect function.
2009-11-16   rp    Create

===========================================================================*/

#include "fs_hotplug_config_i.h"
#include "fs_hotplug.h"


#ifndef FEATURE_FS_HOTPLUG

unsigned int
hotplug_set_polling_interval (unsigned int milliseconds)
{
  (void) milliseconds;
  return 0;
}

void
hotplug_init (void)
{
}

int
hotplug_format (const char *dirname)
{
  (void) dirname;
  return -1;
}

int
hotplug_format_dev (struct hotplug_device *hdev)
{
  (void) hdev;
  return -1;
}


int
hotplug_lock_dev (struct hotplug_device *hdev)
{
  (void) hdev;
  return -1;
}

int
hotplug_unlock_dev (struct hotplug_device *hdev)
{
  (void) hdev;
  return -1;
}

int
hotplug_dev_open (struct hotplug_device *hdev)
{
  (void) hdev;
  return -1;
}

int
hotplug_dev_close (struct hotplug_device *hdev)
{
  (void) hdev;
  return -1;
}

struct hotplug_device*
hotplug_open_device (const char *hdev_name)
{
  (void) hdev_name;
  return NULL;
}

int
hotplug_close_device (struct hotplug_device *hdev)
{
  (void) hdev;
  return -1;
}

int
hotplug_dev_read  (struct hotplug_device *hdev, uint32 lba,
                   unsigned char *buf, uint16 block_count)
{
  (void) hdev; (void) lba; (void) buf; (void) block_count;
  return -1;
}

int
hotplug_dev_write (struct hotplug_device *hdev, uint32 lba,
                   unsigned char *buf, uint16 block_count)
{
  (void) hdev; (void) lba; (void) buf; (void) block_count;
  return -1;
}

int
hotplug_dev_write_udata (struct hotplug_device *hdev, uint32 lba,
                        unsigned char *buf, uint16 block_count)
{
  (void) hdev; (void) lba; (void) buf; (void) block_count;
  return -1;
}

int
hotplug_dev_erase (struct hotplug_device *hdev, uint32 lba,
                   uint16 block_count)
{
  (void) hdev; (void) lba; (void) block_count;
  return -1;
}

int
hotplug_dev_get_size (struct hotplug_device *hdev, uint32 *block_count,
                      uint16 *bytes_per_block)
{
  (void) hdev; (void) block_count; (void) bytes_per_block;
  return -1;
}

int
hotplug_dev_is_present (struct hotplug_device *hdev)
{
  (void) hdev;
  return -1;
}

int
hotplug_dev_is_mounted (struct hotplug_device *hdev)
{
  (void) hdev;
  return -1;
}

int
hotplug_set_sps_end_point (struct hotplug_device *hdev, void *param)
{
  (void) hdev; (void) param;
  return -1;
}

int
hotplug_get_sps_end_point (struct hotplug_device *hdev, void *param)
{
  (void) hdev; (void) param;
  return -1;
}


int hotplug_set_power_on_write_protection (struct hotplug_device *hdev,
                                uint32 lba, uint32 block_cnt)
{
  (void) hdev; (void) lba; (void) block_cnt;
  return -1;
}


const char*
hotplug_dev_get_path (struct hotplug_device *hdev)
{
  (void) hdev;
  return NULL;
}

int
hotplug_dev_get_state (struct hotplug_device *hdev, struct hdev_state *state)
{
  (void) hdev; (void) state;
  return -1;
}

int
hotplug_dev_get_device_info (struct hotplug_device *hdev,
                             struct hotplug_device_info *dev_info)
{
  (void) hdev; (void) dev_info;
  return -1;
}

int
hotplug_dev_reliable_write (struct hotplug_device *hdev, uint32 lba,
                            uint32 *buf, uint16 block_cnt)
{
  (void) hdev; (void) lba; (void) buf; (void) block_cnt;
  return -1;
}

hotplug_notify_handle
hotplug_notify_register (hotplug_dev_type device_type,
                         hotplug_notify_event event_mask,
                         hotplug_notify_event_cb user_cb, void *user_data)
{
  (void) device_type; (void) event_mask; (void) user_cb; (void) user_data;
  return HOTPLUG_NOTIFY_INVALID_HANDLE;
}

int
hotplug_notify_unregister (hotplug_notify_handle reg_hdl)
{
  (void) reg_hdl;
  return -1;
}

int
hotplug_checkfat (const char *hdev_name, int mode, int *errors,
                  void (*callback) (void *), void *param)
{
  (void) hdev_name; (void) mode; (void) errors; (void) callback; (void) param;
  return -1;
}

void
hotplug_set_idle_device_event_duration (uint8 minutes)
{
  (void) minutes;
}

void
hotplug_task_init (void)
{
}

struct hotplug_iter*
hotplug_iter_open (hotplug_dev_type dev_type,
                   enum hotplug_iter_type iter_type,
                   enum hotplug_iter_device_location dev_loc)
{
  (void) dev_type; (void) iter_type; (void) dev_loc;
  return NULL;
}

struct hotplug_iter_record*
hotplug_iter_get_next (struct hotplug_iter *hiter)
{
  (void) hiter;
  return NULL;
}

int
hotplug_iter_close (struct hotplug_iter *hiter)
{
  (void) hiter;
  return -1;
}

void
fs_sfat_lock_card (void)
{
}

void
fs_sfat_unlock_card (void)
{
}

struct hotplug_device*
hotplug_hdev (int16 drive_id)
{
  (void) drive_id;
  return NULL;
}

struct hotplug_device*
hotplug_find_dev_by_type (hotplug_dev_type type, struct hotplug_device *last)
{
  (void) type; (void) last;
  return NULL;
}

struct hotplug_device*
hotplug_find_dev_by_path (const char *path)
{
  (void) path;
  return NULL;
}

int
hotplug_dev_get_driveno (struct hotplug_device *hdev)
{
  (void) hdev;
  return -1;
}

void*
hotplug_dev_get_handle (struct hotplug_device *hdev)
{
  (void) hdev;
  return NULL;
}

struct hotplug_device*
hotplug_open_device_by_partition_type (hotplug_dev_type dev_type,
       enum hotplug_iter_device_location dev_loc, uint32 partition_type)
{
  (void) dev_type;
  (void) dev_loc;
  (void) partition_type;
  return NULL;
}


#else
  extern int __dont_complain_about_empty_file;
#endif /* !FEATURE_FS_HOTPLUG */

