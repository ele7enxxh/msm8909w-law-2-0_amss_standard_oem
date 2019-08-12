/***********************************************************************
 * flash_scrub_log.c
 *
 * Flash Scrub Log module.
 * Copyright (C) 2013-2014 QUALCOMM Technologies, Inc.
 *
 * Flash Scrub Log module.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_log.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-04-28   vm    Use correct MSG macros to save RAM.
2013-09-06   rp    Fix compiler warnings seen in target.
2013-09-04   rp    Cleanup the Scrub Log Module.
2013-08-21   rp    Fix the newline-character to be in the correct spot.
2013-08-15   rp    Make all target file names to start with flash_scrub.
2013-08-14   rp    Fix tab.
2013-08-04   rp    Create

===========================================================================*/

#include "flash_scrub_config_i.h"
#include "flash_scrub_log.h"
#include "flash_scrub_err.h"

#include "fs_public.h"
#include "fs_fcntl.h"
#include "fs_errno.h"

#include <stdio.h>
#include <string.h>

#define FLASH_SCRUB_LOG_EFS_OFLAG  (O_CREAT | O_APPEND | O_AUTODIR | O_WRONLY)
#define FLASH_SCRUB_LOG_EFS_MODE   (0644)

struct flash_scrub_log_info_type
{
  uint32 max_files_count;
  uint32 latest_file_number;
  uint32 efs_err_count;
  int last_efs_errno;

  const char *base_dir_name;
  const char *latest_file_name;
  char log_file_name[FLASH_SCRUB_LOG_MAX_FILE_PATH_SIZE_IN_BYTES];

  uint8 line_buffer[FLASH_SCRUB_LOG_MAX_LINE_BUFF_SIZE_IN_BYTES];
};

static struct flash_scrub_log_info_type flash_scrub_log_err_info;
static struct flash_scrub_log_info_type flash_scrub_log_msg_info;


static void
flash_scrub_log_init_helper (struct flash_scrub_log_info_type *log_info,
    const char *base_dir_name, const char *latest_file_name,
    uint32 max_files_count)
{
  int result, update_file = 0;
  uint32 file_number;

  memset (log_info, 0, sizeof (struct flash_scrub_log_info_type));

  log_info->max_files_count = max_files_count;
  log_info->base_dir_name = base_dir_name;
  log_info->latest_file_name = latest_file_name;

  /* If latest log-file-number exists then read it or else create it. */
  result = efs_get (latest_file_name, &file_number, sizeof (file_number));
  if (result == sizeof (file_number))
  {
    log_info->latest_file_number = file_number;
  }
  else
  {
    update_file = 1;
  }

  if (log_info->latest_file_number > log_info->max_files_count)
  {
    log_info->latest_file_number = 0;
    update_file = 1;
  }

  if (update_file)
  {
    result = efs_put (latest_file_name, &log_info->latest_file_number,
                      sizeof (log_info->latest_file_number),
                      FLASH_SCRUB_LOG_EFS_OFLAG, FLASH_SCRUB_LOG_EFS_MODE);
    if (result != 0)
    {
      ++log_info->efs_err_count;
      log_info->last_efs_errno = efs_errno;
    }
  }
}


void
flash_scrub_log_init (void)
{
  flash_scrub_log_init_helper (&flash_scrub_log_err_info,
                               FLASH_SCRUB_LOG_ERR_LOGS_DIRECTORY_NAME,
                               FLASH_SCRUB_LOG_ERR_LOGS_LATEST_LOG_FILE,
                               FLASH_SCRUB_LOG_MAX_ERR_LOG_FILES_COUNT);

  flash_scrub_log_init_helper (&flash_scrub_log_msg_info,
                               FLASH_SCRUB_LOG_MSG_LOGS_DIRECTORY_NAME,
                               FLASH_SCRUB_LOG_MSG_LOGS_LATEST_LOG_FILE,
                               FLASH_SCRUB_LOG_MAX_MSG_LOG_FILES_COUNT);
}

static void
flash_scrub_log_file_helper (struct flash_scrub_log_info_type *log_info,
          const char *fmt_string, uint32 arg1, uint32 arg2, uint32 arg3)
{
  int buf_size, line_buf_size;
  struct fs_stat sbuf;
  int fd = -1, result, oflag, update_file_number = 0;

  /*--------------------------------------------------------------------------
   * Store the supplied format-string and args inside the line-buffer.
  --------------------------------------------------------------------------*/
  line_buf_size = sizeof (log_info->line_buffer) - 1;

  memset (log_info->line_buffer, 0, line_buf_size);

  buf_size = snprintf ((char *)log_info->line_buffer, line_buf_size,
                       fmt_string, arg1, arg2, arg3);
  if (buf_size > line_buf_size)
  {
    buf_size = line_buf_size;
  }
  log_info->line_buffer[buf_size++] = '\n';


  /*--------------------------------------------------------------------------
   * In case if the LOGS directory-path is deleted, recreate it.
  --------------------------------------------------------------------------*/
  result = efs_stat (log_info->latest_file_name, &sbuf);
  if (result < 0 && efs_errno == ENOENT)
  {
    log_info->latest_file_number = 0;
    result = efs_put (log_info->latest_file_name,
                      &log_info->latest_file_number,
                      sizeof (log_info->latest_file_number),
                      FLASH_SCRUB_LOG_EFS_OFLAG, FLASH_SCRUB_LOG_EFS_MODE);
    if (result != 0)
    {
      ++log_info->efs_err_count;
      log_info->last_efs_errno = efs_errno;
      goto End;
    }
  }

  /*--------------------------------------------------------------------------
   * Check if the current log-file has space for this line buffer.
  --------------------------------------------------------------------------*/
  snprintf (log_info->log_file_name, sizeof (log_info->log_file_name),
      "%s/%02lu.log", log_info->base_dir_name, log_info->latest_file_number);

  oflag = (O_AUTODIR | O_WRONLY | O_APPEND);
  sbuf.st_size = 0;
  result = efs_stat (log_info->log_file_name, &sbuf);
  if (result < 0)
  {
    oflag |= (O_CREAT | O_TRUNC);
  }

  if ((sbuf.st_size + buf_size) > FLASH_SCRUB_LOG_MAX_FILE_SIZE_IN_BYTES)
  {
    oflag |= (O_CREAT | O_TRUNC);

    ++log_info->latest_file_number;
    if (log_info->latest_file_number > log_info->max_files_count)
    {
      log_info->latest_file_number = 0;
    }

    update_file_number = 1;
    snprintf (log_info->log_file_name, sizeof (log_info->log_file_name),
        "%s/%02lu.log", log_info->base_dir_name, log_info->latest_file_number);
  }

  if (update_file_number)
  {
    result = efs_put (log_info->latest_file_name,
                      &log_info->latest_file_number,
                      sizeof (log_info->latest_file_number),
                      FLASH_SCRUB_LOG_EFS_OFLAG, FLASH_SCRUB_LOG_EFS_MODE);
    if (result != 0)
    {
      FLASH_SCRUB_MSG_HIGH_1 ("%d, efs_put failed.", efs_errno);
      ++log_info->efs_err_count;
    }
  }

  /*--------------------------------------------------------------------------
   * Append the line-buffer to the EFS file.
  --------------------------------------------------------------------------*/
  fd = efs_open (log_info->log_file_name, oflag, FLASH_SCRUB_LOG_EFS_MODE);
  if (fd < 0)
  {
    FLASH_SCRUB_MSG_HIGH_1 ("%d, efs_open failed.", efs_errno);
    ++log_info->efs_err_count;
    log_info->last_efs_errno = efs_errno;
    goto End;
  }

  result = efs_write (fd, log_info->line_buffer, buf_size);
  if (result != buf_size)
  {
    FLASH_SCRUB_MSG_HIGH_1 ("%d, efs_write failed.", efs_errno);
    ++log_info->efs_err_count;
    log_info->last_efs_errno = efs_errno;
    goto End;
  }

End:
  if (fd >= 0)
  {
    result = efs_close (fd);
    if (result != 0)
    {
      FLASH_SCRUB_MSG_HIGH_1 ("%d, efs_close failed.", efs_errno);
      ++log_info->efs_err_count;
    }
  }
}

void
flash_scrub_log_err (const char *fmt, uint32 arg1, uint32 arg2, uint32 arg3)
{
  flash_scrub_log_file_helper (&flash_scrub_log_err_info,
                               fmt, arg1, arg2, arg3);
}

void
flash_scrub_log_msg (const char *fmt, uint32 arg1, uint32 arg2, uint32 arg3)
{
  flash_scrub_log_file_helper (&flash_scrub_log_msg_info,
                               fmt, arg1, arg2, arg3);
}
