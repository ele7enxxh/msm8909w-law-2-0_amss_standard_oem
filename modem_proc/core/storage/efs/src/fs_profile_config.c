/***********************************************************************
 * fs_profile_config.c
 *
 * The EFS profile configuration.
 * Copyright (C) 2012 QUALCOMM Technologies, Inc.
 *
 * This odule populates and provides the configuration required by the
 * fs_profile module.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_profile_config.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-12-26   nr    Implement support of Extensive profiling.
2012-11-28   nr    Create

===========================================================================*/

#include <string.h>
#include "fs_profile_config.h"

static struct fs_profile_config fs_profile_config_struct;

struct fs_profile_config*
fs_profile_config_get_config (void)
{
  memset (&fs_profile_config_struct, 0, (sizeof (fs_profile_config_struct)));

  fs_profile_config_struct.enable_api_time_profile =
                                            FS_PROFILE_ENABLE_API_TIME_PROFILE;

  fs_profile_config_struct.enable_api_flash_profile =
                                           FS_PROFILE_ENABLE_API_FLASH_PROFILE;

  fs_profile_config_struct.enable_extensive_profiling =
                                           FS_PROFILE_ENABLE_EXTENSIVE_PROFILE;

  fs_profile_config_struct.enable_task_name_log =
                                               FS_PROFILE_ENABLE_TASK_NAME_LOG;

  fs_profile_config_struct.max_log_file_count = FS_PROFILE_MAX_LOG_FILE_COUNT;

  fs_profile_config_struct.max_string_log_length =
                                              FS_PROFILE_MAX_STRING_LOG_LENGTH;

  return &fs_profile_config_struct;
}

