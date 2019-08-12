/***********************************************************************
 * fs_profile_config.h
 *
 * The EFS profile configuration.
 * Copyright (C) 2012-2013 QUALCOMM Technologies, Inc.
 *
 * This odule populates and provides the configuration required by the
 * fs_profile module.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_profile_config.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-03-06   nr    Make profile dir and file size configurable.
2012-12-26   nr    Implement support of Extensive profiling.
2012-11-28   nr    Create

===========================================================================*/

#ifndef __FS_PROFILE_CONFIG_H__
#define __FS_PROFILE_CONFIG_H__

#include "fs_config.h"
#include "comdef.h"
#include "fs_privatedir.h"


#ifndef FS_PROFILE_ENABLE_API_TIME_PROFILE
  #define FS_PROFILE_ENABLE_API_TIME_PROFILE 0
#endif


#ifndef FS_PROFILE_ENABLE_API_FLASH_PROFILE
  #define FS_PROFILE_ENABLE_API_FLASH_PROFILE 0
#endif


#ifndef FS_PROFILE_ENABLE_EXTENSIVE_PROFILE
  #define FS_PROFILE_ENABLE_EXTENSIVE_PROFILE 0
#endif

#ifndef FS_PROFILE_ENABLE_TASK_NAME_LOG
  #define FS_PROFILE_ENABLE_TASK_NAME_LOG 0
#endif

#ifndef FS_PROFILE_MAX_LOG_FILE_COUNT
  #define FS_PROFILE_MAX_LOG_FILE_COUNT 5
#endif

#ifndef FS_PROFILE_MAX_STRING_LOG_LENGTH
  #define FS_PROFILE_MAX_STRING_LOG_LENGTH 50
#endif

#ifndef FS_PROFILE_EXTENSIVE_LOG_DIR
  #define FS_PROFILE_EXTENSIVE_LOG_DIR FS_EFS_PRIVATE_DIR_PATH "/profile/"
#endif

#ifndef FS_PROFILE_EXTENSIVE_MAX_LOG_FILE_SIZE
  #define FS_PROFILE_EXTENSIVE_MAX_LOG_FILE_SIZE 1024
#endif

struct fs_profile_config
{
  int8 enable_api_time_profile; /* If time should be profiled per api. */
  int8 enable_api_flash_profile; /* If flash counters are to be profiled. */
  int8 enable_extensive_profiling; /* If extensive logs are to be collected. */
  int8 enable_task_name_log; /* If task name is to be collected. */
  uint8 max_log_file_count; /* Max number of files to use for logging */
  uint8 max_string_log_length; /* Max length of string in log file. */
};

/* Get the current config for profiling. */
struct fs_profile_config* fs_profile_config_get_config (void);

#endif /* not __FS_PROFILE_CONFIG_H__ */
