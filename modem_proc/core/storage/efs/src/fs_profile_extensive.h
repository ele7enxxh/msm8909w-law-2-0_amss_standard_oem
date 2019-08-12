/***********************************************************************
 * fs_profile_extensive.h
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

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_profile_extensive.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-05-06   nr    Add support to enable and disable profiling.
2013-02-07   nr    Use unix time instead of cdma time as time stamp.
2012-12-26   nr    Create

===========================================================================*/

#ifndef __FS_PROFILE_EXTENSIVE_H__
#define __FS_PROFILE_EXTENSIVE_H__

#include "fs_profile.h"

/*
 ===========================================================================
   The File format in Human visualizable form
 ===========================================================================

    +--------------------------+
    |  File Time Stamp         |
    |                          |
    +--------------------------+--------------------------------------+
    |  Record 1                                                       |
    |                                                                 |
    +-----------------------------------------------------------------++
    |  Record 2                                                        |
    |                                                                  |
    +----------------------------------------------+-------------------+
    |  Record 3                                    |
    |                                              |
    +----------------------------------------------+--------+
    |  Record 4                                             |
    |                                                       |
    +--------------------------+----------------------------+
    |  Record 5                |
    |                          |
    +--------------------------+---------------------------------------+
    |  Record 6                                                        |
    |                                                                  |
    +------------------------------------------------------------------+


===========================================================================
 Representation of one single record.
===========================================================================
 +----+---+-----+---+-----+-------+------------+-----------------------------+
 | E  | F | R T |E T|T N  | I E F |            |                             |
 | n  | l | e i |x i|a a  | n n l |            |                             |
 | c I| a | c m |c m|s m  | t c a |            |                             |
 | o d| g | o e |e e|k e  | e o g | Integers   |  Strings                    |
 | d  | s | r   |c  |     | g d   |            |                             |
 | e  |   | d   |   |     | e e   |            |                             |
 | d  |   |     |   |     | r     |            |                             |
 |    |   |     |   |     |       |            |                             |
 +----+---+-----+---+-----+-------+------------+-----------------------------+



===========================================================================
 Representation of record Flags.
===========================================================================
+---------+---------+----------+--------+----------+---------+--------+-------+
|         |         |          |        |          |         |        |       |
| Bit 7   | Bit 6   | Bit 5    | Bit 4  | Bit 3    | Bit 2   | Bit 1  |Bit 0  |
|         |         |          |        |          |         |        |       |
+---------+---------+----------+--------+----------+---------+--------+-------+
 *               *       *          *     *                                *
 *               *       *          *     *                                *
 *               *       *          *     *                                *
 *****************       *          *     *                                *
         *               *          *     **********************************
         *               *                                 *
         *         +-----------+  +------+                 *
 +---------------+ |Flag to    |  |R   F |                 *
 |String count   | |Indicate   |  |e   u |          +---------------+
 |(Max 3 )       | |if the task|  |s f t |          |Integer count  |
 +---------------+ |Name was   |  |e o u |          |(Max 15)       |
                   |Logged     |  |r r r |          +---------------+
                   +-----------+  |v   e |
                                  |e     |
                                  |d     |
                                  |      |
                                  +------+


===========================================================================
 Representation of Integer Encoding Flags.
===========================================================================
+---------+---------+----------+--------+----------+---------+--------+-------+
|         |         |          |        |          |         |        |       |
| Bit 7   | Bit 6   | Bit 5    | Bit 4  | Bit 3    | Bit 2   | Bit 1  |Bit 0  |
|         |         |          |        |          |         |        |       |
+---------+---------+----------+--------+----------+---------+--------+-------+
 *               *   *               *   *               *    *               *
 *               *   *               *   *               *    *               *
 *               *   *               *   *               *    *               *
 *****************   *****************   *****************    *****************
         *                   *                   *                    *
         *                   *                   *                    *
         *                   *                   *                    *
 +---------------+   +---------------+   +---------------+    +---------------+
 |4th Integer    |   |3rd Integer    |   |2nd Integer    |    |1st Integer    |
 |encoded size   |   |encoded size   |   |encoded size   |    |encoded size   |
 +---------------+   +---------------+   +---------------+    +---------------+

 */

struct fs_profile_extensive_buffer_manager
{
  uint8 *buffer_start; /* Pointer to the start of the buffer. */
  uint32 total_buffer_size; /* Total size of the buffer. */
  uint8 *current_buffer_ptr; /* The next free space in the buffer. */
  uint32 size_of_remaining_buffer; /* Unused space in bufffer. */
  uint32 current_size_of_log_file; /* The part of buffer that can be flushed */
  uint64 file_start_time; /* The time stamp of the file in unix milliseconds.*/
  uint8 log_file_num; /* The log file number to use in flush. */
  uint8 max_log_file_count; /* Maximum number of log files to use. */
  uint16 max_string_length; /* The max length of string to log */
  char path_buffer[50]; /* Buffer to store log file path. */
  int8 disable_profiling; /* Flag to disable profiling. */
};

/* Opaque handle for extensive profiling. */
struct fs_profile_extensive_record_handle;

/* Initialilize the extensive profillign framework with the max string length
 * to be logged and max number of files to use. A call to this function will
 * clear the current buffer and all the records logged that have not yet been
 * flsuhed  will be lost. */
void fs_profile_extensive_init (uint16 max_string_len,
                                uint8 max_log_file_count);

/* Open a extensive profile handle to use for logging. A start of the record
 * is created with the encoded profile id , the api execution time in us
 * and the calling task's name if enable_task_name_logging flag is set. */
struct fs_profile_extensive_record_handle*
fs_profile_extensive_open(enum fs_profile_id profile_id, uint64 api_time_in_us,
                          int8 enable_task_name_logging);

/* Logs the integer to the record after necessary encoding. */
void fs_profile_extensive_log_int (uint32 integer,
                            struct fs_profile_extensive_record_handle* handle);

/* Logs the string to the record. If the length of the string is greater than
 * the max_String_len passed in  fs_profile_extensive_init function then
 * we skip the start of the string untill the rest of the string is equal to
 * max string length and log that part of the string. */
void fs_profile_extensive_log_string (const char *string,
                            struct fs_profile_extensive_record_handle* handle);

/* Close the handle and finalize the record. This must be called to ensure we
 * have the current flags and record timestamp set. */
void
fs_profile_extensive_close (struct fs_profile_extensive_record_handle* handle);

/* Force a flush the current buffer to the log file. */
void fs_profile_extensive_flush_log_file (void);

/* Enable and disable extensive profiling. */
void fs_profile_extensive_enable (void);
void fs_profile_extensive_disable (void);

#endif /* not __FS_PROFILE_EXTENSIVE_H__ */
