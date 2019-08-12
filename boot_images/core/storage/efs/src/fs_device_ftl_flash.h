/***********************************************************************
 * fs_device_ftl_flash.h
 *
 * Device layer abstration for FTL on flash device.
 * Copyright (C) 2008 Qualcomm Technologies, Inc.
 *
 * Device layer abstration for FTL on flash device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_device_ftl_flash.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-11-03   rp    Create

===========================================================================*/

#ifndef __FS_DEVICE_FTL_FLASH_H__
#define __FS_DEVICE_FTL_FLASH_H__

#include "fs_config.h"
#include "fs_device.h"
#include "fs_sys_types.h"


#ifdef FEATURE_EFS_FTL

/* FTL Partition name. */
#define FTL_PARTITION_NAME "0:FTL"

typedef struct __ftl_dev_stat_type
{
  uint32 ftl_read_op_count;
  uint32 ftl_write_op_count;
  uint32 ftl_copy_op_count;
  uint32 ftl_erase_op_count;
  uint32 ftl_read_sector_count;
  uint32 ftl_write_sector_count;
  uint32 ftl_copy_sector_count;
}
ftl_dev_stat_type;

/* Types of ftl categorizations. */
typedef enum {
  FTL_OP_RAW,
  FTL_OP_CACHE,
  FTL_OP_FLASH,
  NUM_FTL_OP_CATEGORIES
}fs_ftl_op_category ;


extern void fs_device_ftl_flash_init (void);
extern efs_device_t fs_device_ftl_flash_open (uint32 which_device);
extern int fs_device_ftl_flash_close (efs_device_t base_dev);
extern void fs_device_ftl_flash_init_ops (efs_device_t base_dev);

/* Device counters interface for Cache level and Raw FTL level counters. */
void ftl_device_reset_counters (void);
uint32 ftl_device_write_counter (uint32 sector, uint32 count,
                                 fs_ftl_op_category category);
uint32 ftl_device_read_counter (uint32 sector, uint32 count,
                                fs_ftl_op_category category);
void ftl_device_get_stats (ftl_dev_stat_type* op_stats,
                           fs_ftl_op_category category);


#else /* FEATURE_EFS_FTL */

static inline void
fs_device_ftl_flash_init (void)
{
}

static inline efs_device_t
fs_device_ftl_flash_open (uint32 which_device)
{
  (void) which_device;
  return NULL;
}

static inline int
fs_device_ftl_flash_close (efs_device_t base_dev)
{
  (void) base_dev;
  return -1;
}

static inline void
fs_device_ftl_init_flash_ops (efs_device_t base_dev)
{
  (void) base_dev;
}

#endif /* FEATURE_EFS_FTL */

#endif /* not __FS_DEVICE_FTL_FLASH_H__ */
