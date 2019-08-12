/***********************************************************************
 * fs_api_counters.c
 *
 * The API counters module.
 * Copyright (C) 2012,2014 QUALCOMM Technologies, Inc.
 *
 * This file provides api to measure api flash, timer and usage counters.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_api_counters.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-10-13   dks   Changes to enable EFS over NAND on apps processor.
2012-12-26   nr    Implement support of Extensive profiling.
2012-11-28   nr    Create

===========================================================================*/
#include "fs_api_counters.h"
#include "fs_counters.h"
#include "assert.h"
#include "fs_privatedir.h"
#include "fs_lib.h"

#include <string.h>

#define FS_API_ADVANCED_COUNTERS_MAX_SIZE (FS_PROFILE_LEVEL_0_MAX_ID + 1)

#define FS_API_BASIC_COUNTERS_SIZE  (FS_PROFILE_ID_LEVEL_1_MAX)
#define FS_API_ADVANCED_COUNTERS_SIZE \
             MIN(FS_API_ADVANCED_COUNTERS_MAX_SIZE, FS_API_BASIC_COUNTERS_SIZE)

#define FS_API_COUNTERS_LOG_DIR FS_EFS_PRIVATE_DIR_PATH "/profile/"

#define FS_API_COUNTERS_LOG_FILE FS_API_COUNTERS_LOG_DIR "fs_api_counters"

#define FS_API_ADV_COUNTERS_LOG_FILE \
                                 FS_API_COUNTERS_LOG_DIR "fs_api_adv_counters"

struct fs_api_advanced_counters
{
  struct fs_flash_counters flash_total;
  struct fs_flash_counters snapshot;
  uint64 time_total;
};

struct fs_api_counters
{
  enum fs_profile_id id;
  uint32 api_count;
  struct fs_api_advanced_counters *advanced;
};

struct fs_api_advanced_counters
                       fs_api_advanced_counter[FS_API_ADVANCED_COUNTERS_SIZE];

struct fs_api_counters fs_api_counter[FS_API_BASIC_COUNTERS_SIZE];

void
fs_api_counters_init (void)
{
  uint32 idx;

  ASSERT (FS_API_ADVANCED_COUNTERS_SIZE <= FS_API_BASIC_COUNTERS_SIZE);

  memset (fs_api_counter, 0, sizeof (fs_api_counter));
  memset (fs_api_advanced_counter, 0, sizeof (fs_api_advanced_counter));

  for (idx = 0; idx < FS_API_ADVANCED_COUNTERS_SIZE; idx++)
  {
    fs_api_counter[idx].advanced = &(fs_api_advanced_counter[idx]);
  }

  for (idx = 0; idx < FS_API_BASIC_COUNTERS_SIZE; idx++)
  {
    fs_api_counter[idx].id = (enum fs_profile_id) idx;
  }
}

void
fs_api_counters_start_flash_profile (enum fs_profile_id profile_id)
{
  struct fs_flash_counters *snapshot;
  uint32 idx = profile_id;

  if (profile_id >= FS_API_ADVANCED_COUNTERS_SIZE)
  {
    return;
  }

  ASSERT (fs_api_counter[idx].id == profile_id);
  ASSERT (fs_api_counter[idx].advanced != NULL);

  snapshot = &(fs_api_counter[idx].advanced->snapshot);
  fs_counters_take_snaphot (snapshot);

}

void
fs_api_counters_stop_flash_profile (enum fs_profile_id profile_id)
{
  struct fs_flash_counters *prev, *total;
  struct fs_flash_counters current;
  uint32 idx = profile_id;

  if (profile_id >= FS_API_ADVANCED_COUNTERS_SIZE)
  {
    return;
  }

  ASSERT (fs_api_counter[idx].id == profile_id);
  ASSERT (fs_api_counter[idx].advanced != NULL);


  prev = &(fs_api_counter[idx].advanced->snapshot);
  total = &(fs_api_counter[idx].advanced->flash_total);

  fs_counters_take_snaphot (&current);

  current.flash_write_page_counter -= prev->flash_write_page_counter;
  current.flash_write_partial_counter -= prev->flash_write_partial_counter;
  current.flash_read_page_counter -= prev->flash_read_page_counter;
  current.flash_erase_block_counter -= prev->flash_erase_block_counter;
  current.flash_erase_begin_counter -= prev->flash_erase_begin_counter;
  current.flash_erase_suspend_counter -= prev->flash_erase_suspend_counter;

  total->flash_write_page_counter += current.flash_write_page_counter;
  total->flash_write_partial_counter += current.flash_write_partial_counter;
  total->flash_read_page_counter += current.flash_read_page_counter;
  total->flash_erase_block_counter += current.flash_erase_block_counter;
  total->flash_erase_begin_counter += current.flash_erase_begin_counter;
  total->flash_erase_suspend_counter += current.flash_erase_suspend_counter;
}

void
fs_api_counters_update_count (enum fs_profile_id profile_id)
{
  uint32 idx = profile_id;

  if (profile_id >= FS_API_BASIC_COUNTERS_SIZE)
  {
    return;
  }

  ASSERT (fs_api_counter[idx].id == profile_id);
  fs_api_counter[idx].api_count++;
}

void
fs_api_counters_update_time (enum fs_profile_id profile_id, uint64 api_time)
{
  uint32 idx = profile_id;

  if (profile_id >= FS_API_ADVANCED_COUNTERS_SIZE)
  {
    return;
  }

  ASSERT (fs_api_counter[idx].id == profile_id);
  ASSERT (fs_api_counter[idx].advanced != NULL);

  fs_api_counter[idx].advanced->time_total +=  api_time;
}


void
fs_api_counters_flush_logs (void)
{
  (void) efs_put (FS_API_COUNTERS_LOG_FILE, fs_api_counter,
                  sizeof(fs_api_counter), O_CREAT, 0777);

  (void) efs_put (FS_API_ADV_COUNTERS_LOG_FILE, fs_api_advanced_counter,
                  sizeof(fs_api_advanced_counter), O_CREAT, 0777);

}
