/***********************************************************************
 * fs_profile.c
 *
 * The EFS profiling module.
 * Copyright (C) 2012-2013 QUALCOMM Technologies, Inc.
 *
 * This file provides api to help profile the efs api usage and parameters.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_profile.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-05-06   nr    Add support to enable and disable profiling.
2013-01-17   wek   Featurize the profile module so it can be turned off.
2012-12-26   nr    Implement support of Extensive profiling.
2012-11-28   nr    Create

===========================================================================*/

#include "assert.h"

#include "fs_api_counters.h"
#include "fs_profile.h"
#include "fs_profile_config.h"
#include "fs_profile_extensive.h"

#include <string.h>

int8 fs_profile_init_complete = 0;

#ifndef FEATURE_EFS_DISABLE_PROFILE

struct fs_profile_config *fs_profile_config;

void
fs_profile_init (void)
{
  ASSERT (FS_PROFILE_ID_LEVEL_1_MAX <= (FS_PROFILE_LEVEL_1_MAX_ID + 1));
  ASSERT (FS_PROFILE_ID_LEVEL_2_MAX <= (FS_PROFILE_MAX_SUPPORTED_ID + 1));

  fs_profile_config = fs_profile_config_get_config ();
  /* Now we should have a valid structure */
  ASSERT (fs_profile_config != NULL);

  fs_api_counters_init ();

  if (fs_profile_config->enable_extensive_profiling == 1)
  {
    fs_profile_extensive_init (fs_profile_config->max_string_log_length,
                               fs_profile_config->max_log_file_count);
  }

  /* Init is now complete. */
  fs_profile_init_complete = 1;
}

void
fs_profile_open (struct fs_profile_handle *handle,
                 enum fs_profile_id profile_id)
{
  memset (handle, 0, sizeof (struct fs_profile_handle));
  handle->profile_id = profile_id;
  handle->in_use = 1;

}

void
fs_profile_start (struct fs_profile_handle *handle)
{
  ASSERT (handle != NULL);
  ASSERT (handle->in_use == 1);

  if (fs_profile_init_complete == 0)
  {
    return;
  }

  if (fs_profile_config->enable_api_flash_profile == 1)
  {
    fs_api_counters_start_flash_profile (handle->profile_id);
  }

  /*
   * This should be the last call as we need to measure as close as possible.
   */
  if ((fs_profile_config->enable_api_time_profile == 1) ||
      (fs_profile_config->enable_extensive_profiling == 1))
  {
    handle->time.start_timetick = fs_timetick_get_timetick ();
  }
}

void
fs_profile_stop (struct fs_profile_handle *handle)
{
  fs_timetick net_timeticks;

  ASSERT (handle != NULL);
  ASSERT (handle->in_use == 1);

  if (fs_profile_init_complete == 0)
  {
    return;
  }

  /*
   * This should be the first call as we need to measure as close as possible.
   */

  if ((fs_profile_config->enable_api_time_profile == 1) ||
      (fs_profile_config->enable_extensive_profiling == 1))
  {
    net_timeticks = fs_timetick_get_timetick ();
    net_timeticks -= handle->time.start_timetick;
    handle->time.time_in_us = fs_timetick_timetick_diff_to_us (net_timeticks);
  }

  if (fs_profile_config->enable_api_time_profile == 1)
  {
    fs_api_counters_update_time (handle->profile_id, handle->time.time_in_us);
  }


  if (fs_profile_config->enable_api_flash_profile == 1)
  {
    fs_api_counters_stop_flash_profile (handle->profile_id);
  }

  if (fs_profile_config->enable_extensive_profiling == 1)
  {
    handle->extensive_handle = fs_profile_extensive_open(handle->profile_id,
                                      handle->time.time_in_us,
                                      fs_profile_config->enable_task_name_log);
  }

  fs_api_counters_update_count (handle->profile_id);
}

void
fs_profile_log_int (struct fs_profile_handle *handle, uint32 value)
{
  ASSERT (handle != NULL);
  ASSERT (handle->in_use == 1);

  if (fs_profile_init_complete == 0)
  {
    return;
  }

  if (fs_profile_config->enable_extensive_profiling == 1)
  {
    fs_profile_extensive_log_int (value, handle->extensive_handle);
  }

}
void
fs_profile_log_string (struct fs_profile_handle *handle, const char *string)
{
  ASSERT (handle != NULL);
  ASSERT (handle->in_use == 1);

  if (fs_profile_init_complete == 0)
  {
    return;
  }

  if (fs_profile_config->enable_extensive_profiling == 1)
  {
    fs_profile_extensive_log_string (string, handle->extensive_handle);
  }

}

void
fs_profile_close (struct fs_profile_handle *handle)
{
  ASSERT (handle != NULL);
  ASSERT (handle->in_use == 1);

  if (fs_profile_init_complete == 0)
  {
    return;
  }

  if (fs_profile_config->enable_extensive_profiling == 1)
  {
    fs_profile_extensive_close (handle->extensive_handle);
  }

  handle->in_use = 0;
}

void
fs_profile_flush_logs (void)
{
  if (fs_profile_config->enable_extensive_profiling == 1)
  {
    fs_profile_extensive_flush_log_file ();
  }
}

void
fs_profile_enable (void)
{
  if (fs_profile_config->enable_extensive_profiling == 1)
  {
    fs_profile_extensive_enable ();
  }
}

void
fs_profile_disable (void)
{
  if (fs_profile_config->enable_extensive_profiling == 1)
  {
    fs_profile_extensive_disable ();
  }
}

#endif /* FEATURE_EFS_DISABLE_PROFILE */
