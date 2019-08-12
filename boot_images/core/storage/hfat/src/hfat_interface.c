/****************************************************************************
 * hfat_interface.c
 *
 * This file has glue functions for HFAT through which it interacts with
 * Hotplug
 *
 * Copyright (C) 2006--2010,2012 Qualcomm Technologies, Inc.
 *
 * Verbose Description
 *
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hfat/src/hfat_interface.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-08   wek   Rename macros and remove header files to compile for boot.
2010-10-25   wek   Fix hotplug_format to format an soft partition.
2009-08-11   wek   Remove hfat drive number coupled to hotplug device index.
2009-05-26   ebb   Removed from protected memory segment.
2009-01-30   wek   Added ERR_FATAL to increase robustness on device init.
2009-01-29   wek   Dynamic allocation of drivers from a pool.
2008-11-20   ebb   Stub out hfat_getstatus to return 0.
2007-05-25   umr   Added support for EFS memory protection feature.
2006-12-19   sch   Initialized udata multiple sector write calls
2006-11-09   sch   Implemented udata write calls
2006-09-27    sh   Purge all references to C/H/S.
2006-07-13   sch   create

===========================================================================*/
#include "fs_hotplug.h"
#include "hfat_fat.h"
#include "hfat_common.h"
#include "fs_hotplug_priv_api.h"

/* This data structure is used to construct a table that specifies
 * the number of heads and sectors per track for various card sizes.
 */
typedef struct card_CHS_t
{
  uint16 numberMegBytes;        /* card size in mega-bytes */
  uint8 numberHeads;            /* number of  Heads   */
  uint8 secPerTrack;            /* sectors per track  */
} card_CHS;

static F_DRIVER hfat_driver_table[FN_MAXVOLUME];

int
hfat_readsector (F_DRIVER * driver, void *data, unsigned long sector)
{
  return hotplug_dev_read (driver->user_ptr, sector, data, 1);
}

int
hfat_writesector (F_DRIVER * driver, void *data, unsigned long sector)
{
  return hotplug_dev_write (driver->user_ptr, sector, data, 1);
}

int
hfat_write_udata_sector (F_DRIVER * driver, void *data, unsigned long sector)
{
  return hotplug_dev_write_udata (driver->user_ptr, sector, data, 1);
}

int
hfat_readmultiplesector (F_DRIVER * driver, void *data, unsigned long sector,
                         int count)
{
  return hotplug_dev_read (driver->user_ptr, sector, data, count);
}

int
hfat_writemultiplesector (F_DRIVER * driver, void *data, unsigned long sector,
                          int count)
{
  return hotplug_dev_write (driver->user_ptr, sector, data, count);
}

int
hfat_write_udata_multiplesector (F_DRIVER * driver, void *data,
                                 unsigned long sector, int count)
{
  return hotplug_dev_write_udata (driver->user_ptr, sector, data, count);
}

int
hfat_getphy (F_DRIVER * driver, F_PHY *phy)
{
  uint32 block_cnt;
  uint16 block_size;

  /* Ask hotplug how big this device is */
  if (hotplug_dev_get_size (driver->user_ptr, &block_cnt, &block_size) != 0)
    return -1;

  /* HFAT is only tested with block sizes of exactly 512 bytes */
  if (block_size != 512)
    return -1;

  /* This value is real.  It is the exact (LBA) count of blocks */
  phy->number_of_sectors = block_cnt;

  /* There is no such thing as CHS any more, so we use max values */
  phy->number_of_heads = 255;      /* Bogus */
  phy->number_of_cylinders = 1024; /* Bogus */
  phy->sector_per_track = 63;      /* Bogus */

  return 0;
}

long
hfat_getstatus (F_DRIVER * driver)
{
  (void) driver;
  return 0;
}

unsigned int
hfat_get_hidden_sectors (F_DRIVER *driver)
{
  return hotplug_get_physical_start_sector (driver->user_ptr);
}

/* Device initialization function */
F_DRIVER *
hfat_device_init (int driveno, const void *hdev)
{
  F_DRIVER *f_driver;

  f_driver = &hfat_driver_table[driveno];

  f_driver->readsector                  = hfat_readsector;
  f_driver->readmultiplesector          = hfat_readmultiplesector;
  f_driver->writesector                 = hfat_writesector;
  f_driver->writemultiplesector         = hfat_writemultiplesector;
  f_driver->write_udata_sector          = hfat_write_udata_sector;
  f_driver->write_udata_multiplesector  = hfat_write_udata_multiplesector;
  f_driver->getphy                      = hfat_getphy;
  f_driver->getstatus                   = hfat_getstatus;
  f_driver->release                     = NULL;
  f_driver->get_hidden_sectors          = hfat_get_hidden_sectors;
  f_driver->user_ptr                    = (void *)hdev;

  return f_driver;
}
