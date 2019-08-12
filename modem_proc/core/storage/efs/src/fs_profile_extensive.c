/***********************************************************************
 * fs_profile_extensive.c
 *
 * The EFS Extensive profiling module.
 * Copyright (C) 2012-2013 QUALCOMM Technologies, Inc.
 *
 * The EFS Extensive profiling module provides api's to allow for extensive
 * profiling and logging of EFS module usage.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_profile_extensive.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-10-09   nr    Replace banned memory functions with safer versions.
2013-05-06   nr    Add support to enable and disable profiling.
2013-03-06   nr    Make profile dir and file size configurable.
2013-02-07   nr    Use unix time instead of cdma time as time stamp.
2012-12-26   nr    Create

===========================================================================*/
#include "fs_profile_extensive.h"
#include "fs_profile_config.h"
#include "fs_public.h"
#include "fs_util.h"
#include "fs_priv_funcs.h"

#include "assert.h"
#include "fs_errno.h"
#include "fs_osal.h"

#include <string.h>
#include "stringl/stringl.h"


#define FS_PROFILE_EXTENSIVE_META_FILE \
                  FS_PROFILE_EXTENSIVE_LOG_DIR "fs_profile_meta"

#define FS_PROFILE_EXTENSIVE_LOG_FILE_PREFIX \
                               FS_PROFILE_EXTENSIVE_LOG_DIR "fs_profile_log_"

#define FS_PROFILE_EXTENSIVE_LOG_FILE_SUFFIX ".epl"

#define FS_PROFILE_MAX_TASK_NAME_LEN 15

#define FS_PROFILE_EXTENSIVE_ID_MAX_ENCODED_BYTES 2
#define FS_PROFILE_EXTENSIVE_ID_ENCODING_MASK 0x80

#define FS_PROFILE_EXTENSIVE_MAX_STRING_COUNT 3
#define FS_PROFILE_EXTENSIVE_MAX_INT_COUNT 15

#define FS_PROFILE_STRING_COUNT_FLAG_SHIFT_SIZE 6
#define FS_PROFILE_TASK_NAME_FLAG_BIT_MASK 0x20

#define FS_PROFILE_EXTENSIVE_MAX_SIZE_OF_RECORD \
                                   (FS_PROFILE_EXTENSIVE_MAX_LOG_FILE_SIZE / 2)

#ifdef FS_UNIT_TEST
  uint8 fs_profile_extensive_allow_log_flush = 0;
#else
  uint8 fs_profile_extensive_allow_log_flush = 1;
#endif

PACKED struct fs_profile_extensive_basic_record
{
  uint8  flags; /* The flags to be used in the record. */
  uint32 record_timestamp_offset_in_ms; /* Offset in ms from start of file */
  uint32 api_execution_time_in_us; /* Time of execution for the api. */
}PACKED_POST;


struct fs_profile_extensive_record_handle
{
  uint8 are_integers_allowed; /* If integers are allowed to be logged. */
  uint8 are_strings_allowed;/* If strings are allowed to be logged. */
  uint8 integer_count; /* The integers logged so far. */
  uint8 string_count; /* The strings logged so far. */
  char task_name[FS_PROFILE_MAX_TASK_NAME_LEN]; /* Buffer to hold task name. */
  uint8 is_task_name_logged; /* If the task name had been logged.  */
  uint32 encoded_profile_id_size; /* The size in bytes of the encoded id */
  struct fs_profile_extensive_basic_record *basic_record; /* minimal record */
  uint32 total_record_size; /* Total record size so far. */
  uint8 *integer_encode_flag; /* The current encode flag for integer encoding*/
  uint8 is_inited; /* If the handle ha been initiliazed. */
};

struct fs_profile_extensive_buffer_manager fs_profile_extensive_buf_man_inst;

static struct fs_profile_extensive_record_handle
                                       fs_profile_extensive_record_handle_inst;

/* The actual log buffer. */
static uint8
fs_profile_extensive_log_buffer[FS_PROFILE_EXTENSIVE_MAX_LOG_FILE_SIZE];

/* Get a new record handle if available. */
static struct fs_profile_extensive_record_handle*
fs_profile_extensive_get_new_record_handle (void)
{
  struct fs_profile_extensive_record_handle* handle;
  handle = &(fs_profile_extensive_record_handle_inst);

  ASSERT (handle->is_inited == 0);
  memset (&fs_profile_extensive_record_handle_inst, 0,
          sizeof (fs_profile_extensive_record_handle_inst));

  return handle;
}

/* Get the buffer manager instance. */
static struct fs_profile_extensive_buffer_manager*
fs_profile_extensive_get_buffer_manager (void)
{
  return &(fs_profile_extensive_buf_man_inst);
}

/* Get the next file number to use for the logging on an overflow. */
static uint8
fs_profile_extensive_get_next_file_num (uint8 current_file_num)
{
  uint8 next_file_num;
  struct fs_profile_extensive_buffer_manager *bmanager;

  bmanager = fs_profile_extensive_get_buffer_manager ();

  next_file_num = ++current_file_num;

  if (next_file_num >= bmanager->max_log_file_count)
  {
    next_file_num = 0;
  }
  return next_file_num;
}

/* Write the given buffer to a file. */
static void
fs_profile_extensive_write_file (char *path, void *buffer, uint32 buffer_size)
{
  int fd, result;

  if (fs_profile_extensive_allow_log_flush == 0)
  {
    return;
  }

  fd = efs_open(path, O_RDWR | O_CREAT | O_TRUNC, 777);

  if (fd < 0)
  {
    ASSERT (efs_errno == ENOSPC);
    return;
  }

  while (buffer_size > 0)
  {
    result = efs_write (fd, buffer, buffer_size);

    if (result < 0)
    {
      ASSERT (efs_errno == ENOSPC);
      break;
    }
    buffer = (void *) ((uint8 *) buffer + result);
    buffer_size -= result;
  }

  /* And now don't forget to close the file */
  (void) efs_close (fd);
}

/* Get the last saved file number so we dont overwrite the older records on
 * a reboot. */
static uint8
fs_profile_extensive_get_saved_file_num (void)
{
  int result;
  uint8 file_num = ~(0);
  struct fs_profile_extensive_buffer_manager *bmanager;

  bmanager = fs_profile_extensive_get_buffer_manager ();

  bmanager->disable_profiling = 1;

  result = efs_get (FS_PROFILE_EXTENSIVE_META_FILE, &file_num,
                    sizeof(file_num));
  if (result <= 0)
  {
    file_num = 0; /* Start log file from zero. */
    if (fs_profile_extensive_allow_log_flush != 0)
    {
      result = efs_auto_mkdir (FS_PROFILE_EXTENSIVE_LOG_DIR);
      if ((result != 0) && (efs_errno == EEXIST))
      {
        result = 0;
      }

      if (result == 0)
      {
        fs_profile_extensive_write_file (FS_PROFILE_EXTENSIVE_META_FILE,
                                         &file_num, sizeof (file_num));
      }
      else
      {
        ASSERT ((efs_errno == ENOSPC));
      }
    }

    bmanager->disable_profiling = 0;
  }
  return file_num;
}

void
fs_profile_extensive_enable (void)
{
  struct fs_profile_extensive_buffer_manager *bmanager;

  bmanager = fs_profile_extensive_get_buffer_manager ();
  bmanager->disable_profiling = 0;
}

void
fs_profile_extensive_disable (void)
{
  struct fs_profile_extensive_buffer_manager *bmanager;

  bmanager = fs_profile_extensive_get_buffer_manager ();
  bmanager->disable_profiling = 1;
}

/* Check if  profillign has been disabled. */
static int8
fs_profile_is_profiling_disabled (void)
{
  struct fs_profile_extensive_buffer_manager *bmanager;

  bmanager = fs_profile_extensive_get_buffer_manager ();
  return (bmanager->disable_profiling);
}

/* Initialize all the buffers (see header for usage) */
void
fs_profile_extensive_init (uint16 max_string_len, uint8 max_log_file_count)
{
  struct fs_profile_extensive_buffer_manager *bmanager;

  memset (&fs_profile_extensive_buf_man_inst, 0,
                                   sizeof (fs_profile_extensive_buf_man_inst));

  memset (&fs_profile_extensive_record_handle_inst, 0,
                             sizeof (fs_profile_extensive_record_handle_inst));

  bmanager = fs_profile_extensive_get_buffer_manager ();

  bmanager->buffer_start = fs_profile_extensive_log_buffer;
  bmanager->total_buffer_size = sizeof (fs_profile_extensive_log_buffer);

  bmanager->current_size_of_log_file = 0;
  bmanager->log_file_num = fs_profile_extensive_get_saved_file_num ();
  bmanager->max_string_length = max_string_len;
  bmanager->max_log_file_count = max_log_file_count;

  bmanager->size_of_remaining_buffer = 0;
  bmanager->disable_profiling = 0;
}

/* Encode the profile id using simple variable length encoding scheme.
 * If the id is less then 127 it is encoded directly
 * else it is written as a big endian with the most significat bit of the
 * first byte set to one.
 * Thus when decoding we read the first byte and if its most significat bit is
 * set we ignore that bit and read that byte and the next as a big endian
 * uint16 if the bit is not set then it is a uint8 */

static void
fs_profile_extensive_encode_profile_id (enum fs_profile_id profile_id,
                                        uint8 *buffer, uint32 buffer_size,
                                        uint32 *encoding_size)
{
  uint32 profile_id_uint32 = (uint32) profile_id;
  uint8 *profile_id_bytes = (uint8 *) &profile_id_uint32;

  ASSERT (profile_id_uint32 <= FS_PROFILE_MAX_SUPPORTED_ID);
  ASSERT (buffer != NULL);
  ASSERT (buffer_size >= 2);
  ASSERT (encoding_size != NULL);


  if (profile_id_uint32 > FS_PROFILE_LEVEL_1_MAX_ID)
  {
    profile_id_bytes[1] |= FS_PROFILE_EXTENSIVE_ID_ENCODING_MASK;

    buffer[0] = profile_id_bytes[1];
    buffer[1] = profile_id_bytes[0];
    *encoding_size = 2;
  }
  else
  {
    buffer[0] = profile_id_bytes[0];
    *encoding_size = 1;
  }
}

/* Update the log file size in the buffer. */
static void
fs_profile_extensive_update_log_file_size (uint32 size_increase)
{
  struct fs_profile_extensive_buffer_manager *bmanager;

  bmanager = fs_profile_extensive_get_buffer_manager ();
  bmanager->current_size_of_log_file += size_increase;
}

/* Advance the buffer pointers. */
static void
fs_profile_extensive_advance_buffer_ptrs (uint32 bytes_to_advance)
{
  struct fs_profile_extensive_buffer_manager *bmanager;

  bmanager = fs_profile_extensive_get_buffer_manager ();

  ASSERT (bmanager->size_of_remaining_buffer >= bytes_to_advance);

  bmanager->current_buffer_ptr += bytes_to_advance;
  bmanager->size_of_remaining_buffer -= bytes_to_advance;
}

/* Function to flush the log file and advance to the next file if mandated. */
static void
fs_profile_extensive_flush_log_file_helper (int flush_to_new_file)
{
  struct fs_profile_extensive_buffer_manager *bmanager;

  bmanager = fs_profile_extensive_get_buffer_manager ();
  bmanager->disable_profiling = 1;

  snprintf (bmanager->path_buffer, (sizeof (bmanager->path_buffer)),
            FS_PROFILE_EXTENSIVE_LOG_FILE_PREFIX "%03d%s",
            bmanager->log_file_num, FS_PROFILE_EXTENSIVE_LOG_FILE_SUFFIX);

  fs_profile_extensive_write_file (bmanager->path_buffer,
                   bmanager->buffer_start, bmanager->current_size_of_log_file);

  if (flush_to_new_file == 1)
  {
    uint8 meta_log_file_num;
    bmanager->log_file_num =
               fs_profile_extensive_get_next_file_num (bmanager->log_file_num);

    /* Skip one file when we reboot to ensure that we dont potentially
     * overwrite partially flushed data. */
    meta_log_file_num = (bmanager->log_file_num) + 1;
    fs_profile_extensive_write_file (FS_PROFILE_EXTENSIVE_META_FILE,
                             &(meta_log_file_num), sizeof (meta_log_file_num));
  }
  bmanager->disable_profiling = 0;
}

/* Function to allocate the profile buffer and manage the overflow condition
 * if it is unable to allocate space in the buffer without flushing the
 * existing records. */
static void*
fs_profile_extensive_alloc (struct fs_profile_extensive_record_handle *handle,
                            uint32 alloc_size)
{
  uint8 *incomplete_record, *alloc_buffer;
  uint32 size_of_incomplete_record;
  struct fs_profile_extensive_buffer_manager *bmanager;
  uint32 integer_flag_offset = 0;

  bmanager = fs_profile_extensive_get_buffer_manager ();

  if (bmanager->size_of_remaining_buffer < alloc_size)
  {
    incomplete_record = bmanager->buffer_start +
                             bmanager->current_size_of_log_file;

    if (bmanager->current_size_of_log_file == 0)
    {
      size_of_incomplete_record = 0;
    }
    else
    {
      /* We have valid data to flush to a new file*/
      fs_profile_extensive_flush_log_file_helper (1);

      size_of_incomplete_record = ((bmanager->total_buffer_size -
                                    bmanager->size_of_remaining_buffer) -
                                    bmanager->current_size_of_log_file);
      bmanager->current_size_of_log_file = 0;
    }
    ASSERT (handle->total_record_size == size_of_incomplete_record);

    bmanager->file_start_time = fs_unix_time_ms ();

    bmanager->current_buffer_ptr = bmanager->buffer_start;
    bmanager->size_of_remaining_buffer = bmanager->total_buffer_size;

    fs_memscpy (bmanager->current_buffer_ptr,
                bmanager->size_of_remaining_buffer,
                &(bmanager->file_start_time),
                sizeof (bmanager->file_start_time));

    fs_profile_extensive_advance_buffer_ptrs
                 (sizeof (bmanager->file_start_time));

    fs_profile_extensive_update_log_file_size
                                       (sizeof (bmanager->file_start_time));

    ASSERT (bmanager->size_of_remaining_buffer >= size_of_incomplete_record);

    integer_flag_offset = (uint32)(handle->integer_encode_flag) -
                                   (uint32)(handle->basic_record);
    fs_memsmove (bmanager->current_buffer_ptr,
                 bmanager->size_of_remaining_buffer,
                 incomplete_record,
                 size_of_incomplete_record);

    /* Preserve record offsets */
    if (size_of_incomplete_record != 0)
    {
      handle->basic_record = (struct fs_profile_extensive_basic_record *)
              (bmanager->current_buffer_ptr + handle->encoded_profile_id_size);
      handle->integer_encode_flag = (uint8 *)handle->basic_record +
                                     integer_flag_offset;

      fs_profile_extensive_advance_buffer_ptrs (size_of_incomplete_record);
    }

    memset(bmanager->current_buffer_ptr, 0,bmanager->size_of_remaining_buffer);
    ASSERT (bmanager->size_of_remaining_buffer >= alloc_size);

  }

  alloc_buffer = bmanager->current_buffer_ptr;
  fs_profile_extensive_advance_buffer_ptrs (alloc_size);
  handle->total_record_size += alloc_size;
  return alloc_buffer;
}

/* Function to get the task name of the current task context. */
static void
fs_profile_extensive_get_task_name (char *buffer, uint32 buffer_size,
                                    uint32 *length_of_buffer_written)
{
  fs_os_tcb_type *my_tcb;

  my_tcb = fs_os_self ();
  fs_os_get_task_name(my_tcb, buffer, buffer_size, NULL);
  *length_of_buffer_written = strlen (buffer);
}

/* Compute the current time offset from the timestamp of the file. */
static uint32
fs_profile_extensive_get_log_file_time_offset (void)
{
  uint32 time_offset = 0;
  uint64 time_in_ms;
  struct fs_profile_extensive_buffer_manager *bmanager;
  bmanager = fs_profile_extensive_get_buffer_manager ();

  time_in_ms = fs_unix_time_ms ();

  ASSERT (bmanager->current_size_of_log_file != 0);
  time_offset = (uint32) (time_in_ms - bmanager->file_start_time);

  return time_offset;

}

/* Open the handle and log a minimal record to the buffer. */
struct fs_profile_extensive_record_handle*
fs_profile_extensive_open(enum fs_profile_id profile_id,
                          uint64 api_time_in_us, int8 enable_task_name_logging)
{
  uint8 *current_buffer;
  struct fs_profile_extensive_record_handle* handle;
  uint8 encoded_profile_id [FS_PROFILE_EXTENSIVE_ID_MAX_ENCODED_BYTES];
  uint32 total_data_size;
  uint32 task_name_length = 0;

  if (fs_profile_is_profiling_disabled ())
  {
    return NULL;
  }

  handle = fs_profile_extensive_get_new_record_handle ();
  handle->is_inited = 1;
  handle->are_integers_allowed = 1;
  handle->are_strings_allowed = 1;
  handle->integer_count = 0;
  handle->string_count = 0;

  fs_profile_extensive_encode_profile_id (profile_id, encoded_profile_id,
                                          sizeof (encoded_profile_id),
                                          &(handle->encoded_profile_id_size));

  if (enable_task_name_logging != 0)
  {
    fs_profile_extensive_get_task_name (handle->task_name,
                                        sizeof (handle->task_name),
                                        &task_name_length);
    handle->is_task_name_logged = 1;
  }

  total_data_size = (handle->encoded_profile_id_size +
                     sizeof (struct fs_profile_extensive_basic_record) +
                     (task_name_length + 1));

  current_buffer = fs_profile_extensive_alloc (handle, total_data_size);

  fs_memscpy (current_buffer, total_data_size, encoded_profile_id,
              handle->encoded_profile_id_size);

  current_buffer += handle->encoded_profile_id_size;
  handle->basic_record = (struct fs_profile_extensive_basic_record *)
                                                              (current_buffer);

  if (api_time_in_us > (uint64)0xFFFFFFFF)
  {
    api_time_in_us = 0xFFFFFFFF;
  }
  handle->basic_record->api_execution_time_in_us = (uint32) api_time_in_us;

  current_buffer += sizeof (*(handle->basic_record));

  strlcpy ((char *)current_buffer, handle->task_name, (task_name_length + 1));

  return handle;
}

/* Log an integer after encoding.
 * The encoding scheme used here is as follows:
 * 1) Every 4 integers are preceeded by an integer encode flag.
 * 2) Each of the 4 integers is encoded using a 2 bit flag
 * 3) The first integer uses bits 0 and 1, the second uses bits 2 and 3.
 * 4) The third integer uses bits 4 and 5, the fourth uses bits 6 and 7.
 * 5) The value of the 2 bit flag plus 1 indicates the enocding size.
 * 6) The integer is stored as a a little endian number of encoded size.
 */
void
fs_profile_extensive_log_int (uint32 integer,
                             struct fs_profile_extensive_record_handle* handle)
{
  uint8 *encode_buffer;
  uint8 encoded_size, encode_flag, alloc_size, shift_count;

  if (fs_profile_is_profiling_disabled ())
  {
    return;
  }

  ASSERT (handle->is_inited == 1);

  shift_count = ((handle->integer_count % sizeof (integer)) << 1);

  ASSERT (handle->are_integers_allowed == 1);
  ASSERT (handle->integer_count < FS_PROFILE_EXTENSIVE_MAX_INT_COUNT);

  encoded_size = sizeof (integer);

  if (integer <= 0xFF)
  {
    encoded_size = 1;
  }
  else if (integer <= 0xFFFF)
  {
    encoded_size = 2;
  }
  else if (integer <= 0xFFFFFF)
  {
    encoded_size = 3;
  }

  alloc_size = encoded_size;

  if (shift_count == 0)
  {
    alloc_size ++;
  }

  encode_buffer = fs_profile_extensive_alloc (handle, alloc_size);

  if (shift_count == 0)
  {
    handle->integer_encode_flag = encode_buffer;
    encode_buffer ++;
    *(handle->integer_encode_flag) = 0;
  }

  /* Compute the flag to write */
  encode_flag = encoded_size - 1 ;
  ASSERT (encode_flag < 4);

  encode_flag <<= shift_count;

  *(handle->integer_encode_flag) |= encode_flag;
  fs_memscpy (encode_buffer, alloc_size, &integer, encoded_size);

  handle->integer_count++;

  if (handle->integer_count >= FS_PROFILE_EXTENSIVE_MAX_INT_COUNT)
  {
     handle->are_integers_allowed = 0;
  }
}

/* Log a string to the buffer after ignoring the first part of the string if
 * the string length exceeds the max alloed string length. */
void
fs_profile_extensive_log_string (const char *string,
                             struct fs_profile_extensive_record_handle* handle)
{
  uint32 string_len;
  const char *string_start = string;
  char *dest_buf;
  struct fs_profile_extensive_buffer_manager *bmanager;

  if (fs_profile_is_profiling_disabled ())
  {
    return;
  }

  ASSERT (handle->is_inited == 1);

  ASSERT (handle->are_strings_allowed == 1);
  ASSERT (handle->string_count < FS_PROFILE_EXTENSIVE_MAX_STRING_COUNT);

  handle->are_integers_allowed = 0;

  bmanager = fs_profile_extensive_get_buffer_manager ();

  if (string == NULL)
  {
    return;
  }

  string_len = strlen (string);

  if ((string_len == 0) || (bmanager->max_string_length == 0))
  {
    return;
  }

  if (string_len > bmanager->max_string_length)
  {
    /* Truncate the first part of the string so that we can fit within the
       limit. */
    string_start += (string_len - bmanager->max_string_length);
    string_len = bmanager->max_string_length;
  }

  dest_buf = fs_profile_extensive_alloc (handle, (string_len + 1));
  strlcpy (dest_buf, string_start, (string_len + 1));
  handle->string_count++;

  if (handle->string_count >= FS_PROFILE_EXTENSIVE_MAX_STRING_COUNT)
  {
      handle->are_strings_allowed = 0;
  }
}

/* Force a flush of the log file without advancing the logfile number. */
void
fs_profile_extensive_flush_log_file (void)
{
  if (fs_profile_is_profiling_disabled ())
  {
    return;
  }

  fs_profile_extensive_flush_log_file_helper (0);
}

/* Close the handle and update the flags, the log time and the log file size */
void
fs_profile_extensive_close (struct fs_profile_extensive_record_handle* handle)
{
  uint32 stop_time_offset;
  uint8 flags;

  if (fs_profile_is_profiling_disabled ())
  {
    return;
  }

  ASSERT (handle->is_inited == 1);

  /* Encode the record flags */

  flags = handle->integer_count;

  if (handle->is_task_name_logged == 1)
  {
    flags |= FS_PROFILE_TASK_NAME_FLAG_BIT_MASK;
  }

  if (handle->string_count > 0)
  {
    flags |= (handle->string_count << FS_PROFILE_STRING_COUNT_FLAG_SHIFT_SIZE);
  }


  handle->basic_record->flags = flags;
  /* Update the log time at the end to ensure it is corrected to the
   * current log file */
  stop_time_offset = fs_profile_extensive_get_log_file_time_offset ();
  handle->basic_record->record_timestamp_offset_in_ms = stop_time_offset;

  fs_profile_extensive_update_log_file_size (handle->total_record_size);
  handle->is_inited = 0;
}
