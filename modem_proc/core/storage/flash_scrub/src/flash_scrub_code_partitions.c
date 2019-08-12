/***********************************************************************
 * flash_scrub_code_partitions.c
 *
 * This file implements the scrubbing of code partitions.
 *
 * Copyright (C) 2013-2014 QUALCOMM Technologies, Inc.
 * Verbose Description
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_code_partitions.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-04-28   vm    Use correct MSG macros to save RAM.
2013-12-09   wek   Add macro to print F3 messages and log to efs file.
2013-10-08   vm    Add support for retrieving scrub code result in simulator.
2013-10-03   vm    Remove err_fatal upon partition table update failure.
2013-09-10   rp    Do not compute MD5 for SCRUB-partition.
2013-09-09   rp    Add more debug F3 logs when MD5 verification fails.
2013-09-08   rp    Fix bad blocks handling in code-partitions.
2013-09-06   rp    Add more debug F3 messages and logs.
2013-09-02   vm    Make scrub info file creation independent of config file.
2013-08-27   vm    Fix the condition to check for no space in scrub partition.
2013-08-29   rp    Recompute MD5 due to FOTA.
2013-08-22   vm    Add retries when there is md5 failure.
2013-08-21   rp    Add timetick support to do timing-profile.
2013-08-20   vm    Close the partitions while returning FLASH_SCRUB_ENOSPC.
2013-08-16   vm    Store code scrubbing result in one of the global variables.
2013-08-15   rp    Add debug variables to view in T32 and QXDM.
2013-08-15   rp    Prefix flash_scrub to all non-static functions.
2013-08-15   rp    Make all target file names to start with flash_scrub.
2013-08-14   rp    Include "scrub_config_i.h" before any other includes.
2013-08-12   vm    Fix terminating condition for one partition copy.
2013-08-05   vm    Compute MD5 checksum before updating MIBIB.
2013-08-04   rp    Add debug err-fatal and debug f3-messages.
2013-08-04   rp    Add MD5-support to verify code-partition scrubbing.
2013-08-04   rp    Scrub Code partition module cleanup.
2013-08-02   rp    scrub_flash_dal_shim_partition_table_update name change.
2013-07-28   vm    Add init scrub info for first time bootup.
2013-07-25  dks    Fix target compilation issues
2013-07-23   vm    Fix the direction to scrub the code partitions.
2013-05-24   vm    Create the file to implement NAND scrubbing feature.

===========================================================================*/

#include "flash_scrub_config_i.h"
#include "flash_scrub_err.h"
#include "flash_scrub_flash_dal_shim.h"
#include "flash_scrub_code_partitions.h"
#include "flash_scrub_util.h"
#include "flash_scrub_log.h"
#include "flash_scrub_timetick.h"

#include "fs_public.h"
#include "err.h"
#include "assert.h"
#include "stringl/stringl.h"
#include "flash.h"
#include "flash_miparti.h"

#include <string.h>
#include <stdlib.h>



/*==========================================================================
 * SCRUB PROCESS
 *
 *             direction of scrub: LOW_TO_HIGH
 *  ----------> SCRUB partition begins with lower address -------------->
 *         and acquires higher address as scrubbing progresses
 * --------------------------------------------------------------------------
 *    |          |         |         |        |        |
 *    | 0:SCRUB  | TZ      |         |        | APPS   |
 *    |          |         |         |        |        |
 * --------------------------------------------------------------------------
 *   0x10       0x20      0x30       0x40     0x50     0x60
 *
 * ==========================================================================
 *
 *              direction of scrub: HIGH_TO_LOW
 *  <---------- SCRUB partition begins with higher address <--------------
 *           and acquires lower addresses as scrubbing progresses
 * -------------------------------------------------------------------------
 *     |          |         |         |        |        |
 *     |          | TZ      |         | APPS   |0:SCRUB |
 *     |          |         |         |        |        |
 *---------------------------------------------------------------------------
 *    0x10       0x20      0x30       0x40     0x50     0x60
 *
 *
 *==========================================================================*/

enum scrub_direction_type
{
  SCRUB_DIRECTION_LOW_TO_HIGH = 0,
  SCRUB_DIRECTION_HIGH_TO_LOW
};


#define EFS_PARTITION_NAME "0:EFS2"

#define SCRUB_CODE_EFS_OFLAG       (O_CREAT | O_RDWR | O_AUTODIR)
#define SCRUB_CODE_EFS_MODE        (0644)

#define SCRUB_CODE_MD5_HASH_SIZE      (16)
#define SCRUB_CODE_TEMP_BUFFER_SIZE   (128)

/*---------------------------------------------------------------------------
 * Scrub-Code-Configuration file:
 * This file is created on the first ever-boot of the phone and is ONLY read
 * there-after.
---------------------------------------------------------------------------*/
#define SCRUB_CODE_CONFIG_FILE_VERSION           (1)
#define SCRUB_CODE_CONFIG_FILE_MAGIC_STRING      "scrubC-C"
#define SCRUB_CODE_CONFIG_FILE_MAGIC_STRING_LEN  (8)

PACKED struct scrub_code_config_file_type
{
  uint32 version;
  uint8  magic_string[8];
  char   first_code_parti_name[FLASH_PART_NAME_LENGTH];
  char   last_code_parti_name[FLASH_PART_NAME_LENGTH];
} PACKED_POST;


/*---------------------------------------------------------------------------
 * Scrub-Code-Information file:
 * This file is updated everytime one code-partitions scrubbing completes.
---------------------------------------------------------------------------*/
#define SCRUB_CODE_INFO_FILE_VERSION             (1)
#define SCRUB_CODE_INFO_FILE_MAGIC_STRING        "scrubC-I"
#define SCRUB_CODE_INFO_FILE_MAGIC_STRING_LEN    (8)

PACKED struct scrub_code_info_file_type
{
  uint32 version;
  uint8  magic_string[8];
  uint32 direction; /* Direction of the next scrubbing operation. */
  uint32 seq_num;   /* Sequence number/ID of completed scrub operation*/
} PACKED_POST;


/*---------------------------------------------------------------------------
 * Scrub-Code-main structure.
---------------------------------------------------------------------------*/

struct flash_scrub_code_debug_info_type
{
  const char *curr_code_parti_name;
  uint32 num_blocks;
  uint32 empty_blocks_count;
  uint32 copied_blocks_count;
  uint32 bad_blocks_count;
  uint32 src_bad_blocks_count;
  uint32 copied_pages_count;

  uint32 old_code_parti_start;
  uint32 old_code_parti_end;
  uint32 old_code_parti_len;

  uint32 old_scrub_parti_start;
  uint32 old_scrub_parti_end;
  uint32 old_scrub_parti_len;

  uint32 new_code_parti_start;
  uint32 new_code_parti_end;
  uint32 new_code_parti_len;

  uint32 new_scrub_parti_start;
  uint32 new_scrub_parti_end;
  uint32 new_scrub_parti_len;
};

struct scrub_code_info_type
{
  int is_config_okay;

  struct scrub_code_config_file_type config_file;
  struct scrub_code_info_file_type   info_file;

  enum scrub_direction_type scrub_direction;

  struct flash_partition_table mibib_parti_table;
  MD5_CTX  md5_ctx;
  uint8    md5_hash1[SCRUB_CODE_MD5_HASH_SIZE];
  uint8    md5_hash2[SCRUB_CODE_MD5_HASH_SIZE];

  char    temp_buffer[SCRUB_CODE_TEMP_BUFFER_SIZE];
  struct flash_scrub_code_debug_info_type debug_info;
  int    scrub_code_result;
};

static struct scrub_code_info_type scrub_code_info_inst;

#ifdef FLASH_SCRUB_UNIT_TEST
  /* For testing in simulator */
  int scrub_code_parti_result;
#endif

static void scrub_code_create_info_file (struct scrub_code_info_type *scrubc);

/*---------------------------------------------------------------------------
 * This function searches the given partition-name in the partition table and
 * returns the partition-entry and the array-indes of the the partiton when
 * a match is found. NULL is returned when there is no partition by this name.
---------------------------------------------------------------------------*/
static flash_partentry_t
scrub_parti_get_entry (struct flash_partition_table *partition_table,
                       const char *parti_name, uint32 *scrub_index)
{
  flash_partentry_t parti;
  int result;
  uint32 i;

  ASSERT (parti_name != NULL);
  ASSERT (scrub_index != NULL);

  for (i = 0; i < partition_table->numparts; ++i)
  {
    parti = &partition_table->part_entry[i];

    result = flash_scrub_util_compare_strings (parti->name, parti_name);
    if (result == 0)
    {
      *scrub_index = i;
      return parti;
    }
  }

  *scrub_index = 0;
  return NULL;
}


static void
scrub_code_compute_md5_for_all_code (struct scrub_code_info_type *scrubc)
{
  uint32 i;
  int processing_code_partitions = 0, result, save_md5_in_efs;
  struct flash_partition_entry *parti_entry;
  flash_scrub_shim_handle parti_handle;
  uint64 start_tick, stop_tick, diff_tick, time_in_us;
  uint32 time_in_ms;

  start_tick = flash_scrub_timetick_get_timetick ();

  for (i = 0; scrubc->mibib_parti_table.numparts; ++i)
  {
    parti_entry = &scrubc->mibib_parti_table.part_entry[i];

    /* Avoid MD5 for SCRUB-partition. */
    result = flash_scrub_util_compare_strings (parti_entry->name,
                                               FLASH_SCRUB_PARTITION_NAME);
    if (result == 0)
    {
      continue;
    }

    /* Check if we have hit the first code-partition. */
    result = flash_scrub_util_compare_strings (parti_entry->name,
                      scrubc->config_file.first_code_parti_name);
    if (result == 0)
    {
      processing_code_partitions = 1;
    }

    if (processing_code_partitions)
    {
      /* compute md5 for this partition and save it in EFS-file. */
      parti_handle = flash_scrub_shim_open_partition (parti_entry->name);
      if (parti_handle == NULL)
      {
        FLASH_SCRUB_ERR_FATAL ("open-fail. name = %d%d%d",parti_entry->name[2],
                               parti_entry->name[3],parti_entry->name[4]);
      }

      memset (scrubc->md5_hash1, 0, sizeof (scrubc->md5_hash1));
      flash_scrub_util_compute_md5 (parti_handle, 0, parti_entry->length,
                                    &scrubc->md5_ctx, scrubc->md5_hash1);

      result = flash_scrub_shim_close_partition (parti_handle);
      ASSERT (result == 0);

      save_md5_in_efs = 1;

      snprintf (scrubc->temp_buffer, sizeof (scrubc->temp_buffer),
                "%s/%s", FLASH_SCRUB_MD5_DIRECTORY, parti_entry->name);

      result = efs_get (scrubc->temp_buffer, scrubc->md5_hash2,
                        sizeof (scrubc->md5_hash2));
      if (result == sizeof (scrubc->md5_hash2))
      {
        result = memcmp (scrubc->md5_hash1, scrubc->md5_hash2,
                         sizeof (scrubc->md5_hash1));
        if (result == 0)
        {
          save_md5_in_efs = 0;
        }
        else
        {
          FLASH_SCRUB_LOG_ERR_3 ("MD5 changed for parti=%c%c%c",
             parti_entry->name[2], parti_entry->name[3], parti_entry->name[4]);
        }
      }

      if (save_md5_in_efs)
      {
        result = efs_put (scrubc->temp_buffer, scrubc->md5_hash1,
                          sizeof (scrubc->md5_hash1),
                          SCRUB_CODE_EFS_OFLAG, SCRUB_CODE_EFS_MODE);
        if (result != 0)
        {
          FLASH_SCRUB_ERR_FATAL ("%d, efs_put failed.", efs_errno, 0, 0);
        }
      }

    }

    /* Stop at the last code-partition. */
    result = flash_scrub_util_compare_strings (parti_entry->name,
                   scrubc->config_file.last_code_parti_name);
    if (result == 0)
    {
      break;
    }
  }

  stop_tick = flash_scrub_timetick_get_timetick ();
  diff_tick = stop_tick - start_tick;
  time_in_us = flash_scrub_timetick_convert_to_us (diff_tick);
  time_in_ms = (uint32)(time_in_us / 1000);

  FLASH_SCRUB_LOG_MSG_1 ("scrubc: ms = %d, md5 computation timings",
                        time_in_ms);
}

/*---------------------------------------------------------------------------
 * Scrub-Code-Configuration file:
---------------------------------------------------------------------------*/
static int
scrub_code_load_config_file (struct scrub_code_info_type *scrubc)
{
  int result = FLASH_SCRUB_EEFS, temp_res;

  result = efs_get (FLASH_SCRUB_CODE_CONFIG_FILEPATH, &scrubc->config_file,
                    sizeof (scrubc->config_file));
  if (result != sizeof (scrubc->config_file))
  {
    goto End;
  }

  if (scrubc->config_file.version != SCRUB_CODE_CONFIG_FILE_VERSION)
  {
    goto End;
  }

  temp_res = memcmp (scrubc->config_file.magic_string,
                     SCRUB_CODE_CONFIG_FILE_MAGIC_STRING,
                     SCRUB_CODE_CONFIG_FILE_MAGIC_STRING_LEN);
  if (temp_res != 0)
  {
    goto End;
  }

  result = 0;

End:
  return result;
}

static int
scrub_code_create_config_file (struct scrub_code_info_type *scrubc)
{
  uint32 parti_index;
  int num_bytes;
  flash_partentry_t code_parti, temp_parti;

  memset (&scrubc->config_file, 0, sizeof (scrubc->config_file));

  scrubc->config_file.version = SCRUB_CODE_CONFIG_FILE_VERSION;

  memcpy (scrubc->config_file.magic_string,
          SCRUB_CODE_CONFIG_FILE_MAGIC_STRING,
          sizeof (scrubc->config_file.magic_string));

  /*---------------------------------------------------------------------------
   * Figure out the first-code-partition.
   * Let's assume the first code-partition to be right after EFS2-partition.
  ---------------------------------------------------------------------------*/
  temp_parti = scrub_parti_get_entry (&scrubc->mibib_parti_table,
                                      EFS_PARTITION_NAME, &parti_index);
  if (temp_parti == NULL)
  {
    FLASH_SCRUB_LOG_ERR_0 ("scrubc: no EFS parti");
    return FLASH_SCRUB_ENOENT;
  }

  ASSERT ((parti_index + 1) < FLASH_NUM_PART_ENTRIES);
  code_parti = &scrubc->mibib_parti_table.part_entry[parti_index + 1];
  strlcpy (scrubc->config_file.first_code_parti_name, code_parti->name,
           sizeof (scrubc->config_file.first_code_parti_name));

  /*---------------------------------------------------------------------------
   * Figure out the last-code-partition.
   * Let's assume the last-code partition to be right above SCRUB-partition
  ---------------------------------------------------------------------------*/
  temp_parti = scrub_parti_get_entry (&scrubc->mibib_parti_table,
                                      FLASH_SCRUB_PARTITION_NAME,&parti_index);
  if (temp_parti == NULL)
  {
    FLASH_SCRUB_LOG_ERR_0 ("scrubc: no SCRUB parti");
    return FLASH_SCRUB_ENOENT;
  }

  ASSERT (parti_index < FLASH_NUM_PART_ENTRIES);
  code_parti = &scrubc->mibib_parti_table.part_entry[parti_index - 1];
  strlcpy (scrubc->config_file.last_code_parti_name, code_parti->name,
           sizeof (scrubc->config_file.last_code_parti_name));

  scrub_code_compute_md5_for_all_code (scrubc);

  num_bytes = efs_put (FLASH_SCRUB_CODE_CONFIG_FILEPATH, &scrubc->config_file,
                       sizeof (scrubc->config_file),
                       SCRUB_CODE_EFS_OFLAG, SCRUB_CODE_EFS_MODE);
  if (num_bytes < 0)
  {
    FLASH_SCRUB_ERR_FATAL ("%d, efs_put failed.", efs_errno, 0, 0);
  }

  FLASH_SCRUB_LOG_MSG_3 ("scrubC config-file first = %c%c%c",
                       scrubc->config_file.first_code_parti_name[2],
                       scrubc->config_file.first_code_parti_name[3],
                       scrubc->config_file.first_code_parti_name[4]);

  FLASH_SCRUB_LOG_MSG_3 ("scrubC config-file last = %c%c%c",
                       scrubc->config_file.last_code_parti_name[2],
                       scrubc->config_file.last_code_parti_name[3],
                       scrubc->config_file.last_code_parti_name[4]);

  FLASH_SCRUB_LOG_MSG_0 ("scrubc : Created Config file");
  return 0;
}


/*---------------------------------------------------------------------------
 * Scrub-Code-Information file:
---------------------------------------------------------------------------*/
static int
scrub_code_load_info_file (struct scrub_code_info_type *scrubc)
{
  int result = FLASH_SCRUB_EEFS, temp_res;

  result = efs_get (FLASH_SCRUB_CODE_INFO_FILEPATH, &scrubc->info_file,
                    sizeof (scrubc->info_file));
  if (result != sizeof (scrubc->info_file))
  {
    goto End;
  }

  if (scrubc->info_file.version != SCRUB_CODE_INFO_FILE_VERSION)
  {
    goto End;
  }

  temp_res = memcmp (scrubc->info_file.magic_string,
                     SCRUB_CODE_INFO_FILE_MAGIC_STRING,
                     SCRUB_CODE_INFO_FILE_MAGIC_STRING_LEN);
  if (temp_res != 0)
  {
    goto End;
  }

  if (scrubc->info_file.direction > SCRUB_DIRECTION_HIGH_TO_LOW)
  {
    goto End;
  }

  scrubc->scrub_direction =
                (enum scrub_direction_type) scrubc->info_file.direction;
  result = 0;

End:
  return result;
}

static void
scrub_code_save_info_file (struct scrub_code_info_type *scrubc)
{
  int result;

  result = efs_put (FLASH_SCRUB_CODE_INFO_FILEPATH, &scrubc->info_file,
                    sizeof (scrubc->info_file), SCRUB_CODE_EFS_OFLAG,
                    SCRUB_CODE_EFS_MODE);
  if (result != 0)
  {
    FLASH_SCRUB_ERR_FATAL ("%d, efs_put failed.", efs_errno, 0, 0);
  }
}

static void
scrub_code_create_info_file (struct scrub_code_info_type *scrubc)
{
  memset (&scrubc->info_file, 0, sizeof (scrubc->info_file));

  scrubc->info_file.version = SCRUB_CODE_INFO_FILE_VERSION;

  memcpy (scrubc->info_file.magic_string,
          SCRUB_CODE_INFO_FILE_MAGIC_STRING,
          sizeof (scrubc->info_file.magic_string));

  scrubc->info_file.version = 1;
  scrubc->info_file.direction = SCRUB_DIRECTION_HIGH_TO_LOW;
  scrubc->info_file.seq_num = 0;

  scrub_code_save_info_file (scrubc);
  FLASH_SCRUB_LOG_MSG_0 ("scrubc : created info file");
}

static void
scrub_code_read_mibib_partition_table (struct scrub_code_info_type *scrubc)
{
  int result;
  flash_scrub_shim_handle mibib_handle;

  memset ((void *)&scrubc->mibib_parti_table, 0,
           sizeof(scrubc->mibib_parti_table));

  mibib_handle =
            flash_scrub_shim_open_partition (FLASH_SCRUB_MIBIB_PARTITION_NAME);
  if (mibib_handle == NULL)
  {
    FLASH_SCRUB_ERR_FATAL (" MIBIB open-parti fail", 0, 0, 0);
  }

  result = flash_scrub_shim_partition_table_get (mibib_handle,
                                                 &scrubc->mibib_parti_table);
  if (result != FLASH_DEVICE_DONE)
  {
    FLASH_SCRUB_ERR_FATAL ("[%d] parti-tbl-get failed", result, 0, 0);
  }

  result = flash_scrub_shim_close_partition (mibib_handle);
  if (result != FLASH_DEVICE_DONE)
  {
    FLASH_SCRUB_ERR_FATAL ("[%d] mibib close-parti fail", result, 0, 0);
  }

  return;
}

int
flash_scrub_code_initialize (void)
{
  struct scrub_code_info_type *scrubc;
  int result = 0;

  memset (&scrub_code_info_inst, 0, sizeof (scrub_code_info_inst));
  scrubc = &scrub_code_info_inst;

  /* Read the Nand-partition table from the MIBIB partition */
  scrub_code_read_mibib_partition_table (scrubc);

  result = scrub_code_load_config_file (scrubc);
  if (result != 0)
  {
    result = scrub_code_create_config_file (scrubc);
  }

  if (result < 0)
  {
    goto End;
  }

  result = scrub_code_load_info_file (scrubc);
  if (result != 0)
  {
    scrub_code_create_info_file (scrubc);
    result = scrub_code_load_info_file (scrubc);
  }

  if (result < 0)
  {
    goto End;
  }

  /* Config-file and Info-file looks okay, we can start doing code-scrubbing.*/
  scrub_code_info_inst.is_config_okay = 1;

End:
  return result;
}



static int scrub_code_partition_set (struct scrub_code_info_type *scrubc,
                const char *scrub_name, const char *code_limit_name, int incr);

static int scrub_code_one_partition(struct scrub_code_info_type *scrubc,
                         flash_partentry_t scrub_entry,
                         flash_partentry_t code_entry,
                         uint32 scrub_parti_index,
                         uint32 code_parti_index,
                         int incr);

static void scrub_finish_scrub_process (struct scrub_code_info_type *scrubc,
                                        uint32 seqno);

/*
 * This function opens the MIBIB partition and reads the partition table
 * into the global variable nand_partition_table.
 * Then MIBIB partition is closed.
 * Get the direction of scrubbing from the scrub_info.
 * Compare the sequence number of the scrub operation in scrub_info with
 * the parameter passed. If the sequence number passed as parameter is same
 * than the value in the EFS file, it implies the request has come for already
 * finished operation. Hence return the scrub complete status. If the
 * parameter value is different from the seq_num in EFS file, we have received
 * a new scrubbing request. Hence continue with scrubbing operation.
 * In the end, update the EFS file indicating the direction for next scrub
 * operation and sequence number for finished scrub operation.
 */
int
flash_scrub_all_code_partitions (uint32 sequence_num)
{
  struct scrub_code_info_type *scrubc;
  const char *first_code_parti_name, *last_code_parti_name;
  int result, increment_factor;

  scrubc = &scrub_code_info_inst;

  if (!scrub_code_info_inst.is_config_okay)
  {
    FLASH_SCRUB_LOG_ERR_1 ("scrubc: inv cfg. seq = %d", sequence_num);
    return FLASH_SCRUB_EINVALPARTI;
  }

  first_code_parti_name = scrubc->config_file.first_code_parti_name;
  last_code_parti_name  = scrubc->config_file.last_code_parti_name;

  /*
   * The scrub_info.seq_num in EFS file contains the ID of the scrub
   * operation completed successfully. If the sequence_num, which is
   * the ID of current scrub request, matches the scrub_info.seq_num,
   * return 0 indicating the status for the finished scrub process.
   */

  if (sequence_num == scrubc->info_file.seq_num)
  {
    return 0;
  }

  switch (scrubc->scrub_direction)
  {
    case SCRUB_DIRECTION_LOW_TO_HIGH:
    {
      /* The scrub direction of LOW_TO_HIGH implies scrub partition address
       * changes from lower values to higher address values. Hence continue
       * scrubbing until the last code partition is reached. */
        increment_factor = 1;
        result = scrub_code_partition_set (scrubc, FLASH_SCRUB_PARTITION_NAME,
                                           last_code_parti_name,
                                           increment_factor);
    }
    break;

    case SCRUB_DIRECTION_HIGH_TO_LOW:
    {
      /* The scrub direction of HIGH_TO_LOW implies scrub partition address
       * changes from higher values to lower address values. Hence continue
       * scrubbing until the first code partition is reached. */
      increment_factor = -1;
      result = scrub_code_partition_set (scrubc, FLASH_SCRUB_PARTITION_NAME,
                                         first_code_parti_name,
                                         increment_factor);
    }
    break;

    default:
    {
      FLASH_SCRUB_ERR_FATAL ("%d, Invalid scrub direction",
                             scrubc->scrub_direction, 0, 0);
    }
    break;
  }

  if (result == 0)
  {
    scrub_finish_scrub_process (scrubc, sequence_num);
  }

  scrubc->scrub_code_result = result;

#ifdef FLASH_SCRUB_UNIT_TEST
  scrub_code_parti_result = result;
#endif

  return result;
}

/* After completing the scrubbing of one partition, this function swaps the
 * entries in the global variable partition table, that matches with the
 * flash partitions layout.
 */
static void
scrub_parti_swap_entry (struct scrub_code_info_type *scrubc,
                        uint32 code_index, uint32 scrub_index)
{
  struct flash_partition_entry temp_parti;

  memcpy (&temp_parti, &scrubc->mibib_parti_table.part_entry[code_index],
          sizeof(struct flash_partition_entry));

  memcpy (&scrubc->mibib_parti_table.part_entry[code_index],
          &scrubc->mibib_parti_table.part_entry[scrub_index],
          sizeof (struct flash_partition_entry));

  memcpy (&scrubc->mibib_parti_table.part_entry[scrub_index],
          &temp_parti, sizeof (struct flash_partition_entry));

  return;
}

/* This function updates the offset and length values for a partition,
 * passed as part_entry parameter, after the scrubbing of that partition
 * has been completed.
 */
static void
scrub_parti_update_limit (struct scrub_code_info_type *scrubc,
                          flash_partentry_t part_entry,
                          uint32 offset, uint32 parti_index)
{
  part_entry->offset = offset;
  scrubc->mibib_parti_table.part_entry[parti_index].offset = offset;
  scrubc->mibib_parti_table.part_entry[parti_index].length =
                                                        part_entry->length;
}

/* This function scrubs a set of partitions beginning with the partition
 * next to scrub_name partition until the partition with code_limit_name
 * is reached.
 * The incr parameter is either +1 or -1, depending on the direction of
 * scrubbing.
 */
static int
scrub_code_partition_set (struct scrub_code_info_type *scrubc,
                          const char *scrub_name, const char *code_limit_name,
                          int incr)
{
  uint32 code_parti_index, scrub_parti_index, code_limit_parti_index;
  flash_partentry_t code_entry, code_limit_entry, scrub_entry;
  int result = 0;
  uint32 scrub_complete = 0, retry_cnt = 0;
  size_t len1, len2;

  while (1)
  {
    /* Get the index of scrub partition in the partition table */
    scrub_entry = scrub_parti_get_entry (&scrubc->mibib_parti_table,
                                         scrub_name, &scrub_parti_index);
    if (scrub_entry == NULL)
    {
      FLASH_SCRUB_ERR_FATAL ("%d, SCRUB-parti missing", scrub_parti_index,0,0);
    }

    /* Get the index of last code partition in the partition table */
    code_limit_entry = scrub_parti_get_entry (&scrubc->mibib_parti_table,
                                              code_limit_name,
                                              &code_limit_parti_index);
    if (code_limit_entry == NULL)
    {
      FLASH_SCRUB_ERR_FATAL ("%d, CODE-parti missing",
                             code_limit_parti_index, 0, 0);
    }

    /* Check to determine if scrubbing is completed. Useful in case of
       a power loss */
    if ((incr < 0) && (code_limit_parti_index > scrub_parti_index))
    {
      FLASH_SCRUB_LOG_ERR_3 ("Scrub already complete. [%d,%d,%d]",
                           incr, code_limit_parti_index, scrub_parti_index);
      return 0;
    }

    if ((incr > 0) && (code_limit_parti_index < scrub_parti_index))
    {
      FLASH_SCRUB_LOG_ERR_3 ("Scrub already complete. [%d,%d,%d]",
                           incr, code_limit_parti_index, scrub_parti_index);
      return 0;
    }

    code_parti_index = scrub_parti_index;
    if (incr < 0)
      --code_parti_index;
    else
      ++code_parti_index;

    code_entry = &scrubc->mibib_parti_table.part_entry[code_parti_index];

    len1 = strlen (code_entry->name);
    len2 = strlen (code_limit_name);
    if (len1 == len2)
    {
      if (strncmp (code_limit_name, code_entry->name, len1) == 0)
      {
        scrub_complete = 1;
      }
    }

    result = scrub_code_one_partition (scrubc, scrub_entry, code_entry,
                            scrub_parti_index, code_parti_index, incr);

    while ((result == FLASH_SCRUB_EMD5) &&
           (retry_cnt < FLASH_SCRUB_MD5_MAX_RETRY_COUNT))
    {
      ++retry_cnt;
      FLASH_SCRUB_LOG_ERR_1 ("MD5-mismatch. Redo scrub, count = %d",
                           retry_cnt);
      result = scrub_code_one_partition (scrubc, scrub_entry, code_entry,
                            scrub_parti_index, code_parti_index, incr);
    }

    if (result != 0)
    {
      break;
    }

    if (scrub_complete)
    {
      break;
    }

  } /* end of while  */

  return result;
}

/*
 * This function scrubs one code partition.
 * 1. Opens the code and scrub partitions.
 * 2. Makes sure the scrub partition is having space to accomodate the
 *    code partition.
 * 3. Copy the blocks from code partition to scrub partition.
 * 4. Compute MD5 checksum on the newly copied data to ensure data
 *    data is correctly copied from source location.
 * 5. Update the partitions offsets and lengths in the RAM variable
 *    for partition table.
 * 6. Swap the positions of partition entries in the RAM variable.
 * 7. Close the code and scrub partitions and open the MIBIB partition.
 * 8. Call the flash function to update the partition table in MIBIB
 *    partition, with the contents of the RAM variable.
 * 9. Close MIBIB partition.
 */
static int
scrub_code_one_partition(struct scrub_code_info_type *scrubc,
                         flash_partentry_t scrub_entry,
                         flash_partentry_t code_entry,
                         uint32 scrub_parti_index,
                         uint32 code_parti_index,
                         int incr)
{
  flash_scrub_shim_handle code_handle, scrub_handle, mibib_handle;
  uint32 scrub_new_beg_blk = 0;
  int32 scrub_beg_blk, scrub_end_blk;
  uint32 code_new_beg_blk = 0;
  uint32 code_block, scrub_block, md5_beg_blk;
  uint32 scrub_len, retry_count = 0;
  int result, code_end_blk, code_beg_blk, skip_mibib_update = 0, error = 0;
  uint64 start_tick, stop_tick, diff_tick, time_in_us;
  uint32 time_in_ms;
  int move_code_block, move_scrub_block;
  flash_partentry_t new_code_entry, new_scrub_entry;

  start_tick = flash_scrub_timetick_get_timetick ();

  scrub_beg_blk = 0;
  scrub_end_blk = scrub_entry->length - 1;
  code_beg_blk = 0;
  code_end_blk = code_entry->length - 1;
  scrub_len = scrub_entry->length;

  FLASH_SCRUB_LOG_MSG_3  ("scrubc: start %c%c%c",
                        code_entry->name[2],
                        code_entry->name[3],
                        code_entry->name[4]);

  memset (&scrubc->debug_info, 0, sizeof (scrubc->debug_info));
  scrubc->debug_info.curr_code_parti_name = code_entry->name;
  scrubc->debug_info.num_blocks = code_entry->length;

  scrubc->debug_info.old_code_parti_start = code_entry->offset;
  scrubc->debug_info.old_code_parti_end =
                 (code_entry->offset + code_entry->length - 1);
  scrubc->debug_info.old_code_parti_len = code_entry->length;
  FLASH_SCRUB_LOG_MSG_3  ("Old-code : S = %d, E = %d, L = %d",
                        scrubc->debug_info.old_code_parti_start,
                        scrubc->debug_info.old_code_parti_end,
                        scrubc->debug_info.old_code_parti_len);


  scrubc->debug_info.old_scrub_parti_start = scrub_entry->offset;
  scrubc->debug_info.old_scrub_parti_end =
                 (scrub_entry->offset + scrub_entry->length - 1);
  scrubc->debug_info.old_scrub_parti_len = scrub_entry->length;
  FLASH_SCRUB_LOG_MSG_3  ("Old-Scrub : S = %d, E = %d, L = %d",
                        scrubc->debug_info.old_scrub_parti_start,
                        scrubc->debug_info.old_scrub_parti_end,
                        scrubc->debug_info.old_scrub_parti_len);

  FLASH_SCRUB_LOG_MSG_3 ("CI = %d, SI = %d, T = %d,one code-parti start",
                        code_parti_index, scrub_parti_index,
                        code_entry->length);

  code_handle = flash_scrub_shim_open_partition (code_entry->name);
  if (code_handle == NULL)
  {
    FLASH_SCRUB_ERR_FATAL ("open-parti failed. parti = %d%d%d",
                           code_entry->name[2], code_entry->name[3],
                           code_entry->name[4]);
  }

  scrub_handle = flash_scrub_shim_open_partition (scrub_entry->name);
  if (scrub_handle == NULL)
  {
    FLASH_SCRUB_ERR_FATAL ("open-parti failed. parti = %d%d%d",
                           scrub_entry->name[2], scrub_entry->name[3],
                           scrub_entry->name[4]);
  }

  if (scrub_entry->length < code_entry->length)
  {
    FLASH_SCRUB_DEBUG_ERR_FATAL ("%d,%d, no spc in scrub", scrub_entry->length,
                           code_entry->length, 0);
    FLASH_SCRUB_LOG_ERR_2 ("%d,%d, no spc in scrub", scrub_entry->length,
                         code_entry->length);
    error = FLASH_SCRUB_ENOSPC;
    skip_mibib_update = 1;
    goto End;
  }

  if (incr < 0)
  {
    scrub_block = scrub_end_blk;
    code_block = code_end_blk;
    scrub_new_beg_blk = code_entry->offset;
  }
  else
  {
    scrub_block = scrub_beg_blk;
    code_block = code_beg_blk;
    code_new_beg_blk = scrub_entry->offset;
  }

  while (code_end_blk >= code_beg_blk)
  {
    if (scrub_beg_blk > scrub_end_blk)
    {
      FLASH_SCRUB_DEBUG_ERR_FATAL ("%d,%d, no spc in scrub",
                                   scrub_end_blk - (scrub_beg_blk - 1),
                                   (code_end_blk - code_beg_blk) + 1,
                                   0);
      FLASH_SCRUB_LOG_ERR_2 ("%d,%d, no spc in scrub",
                           scrub_end_blk - (scrub_beg_blk - 1),
                           (code_end_blk - code_beg_blk) + 1);
      error = FLASH_SCRUB_ENOSPC;
      skip_mibib_update = 1;
      goto End;
    }

    move_code_block = 0;
    move_scrub_block = 0;

    /* Call the scrub API to copy the partition blocks */
    result = flash_scrub_copy_block (code_handle, code_block,
                                     scrub_handle, scrub_block);
    if (result > 0)
    {
      /* code block is successfully copied onto scrub block
       * move code block
       * move scrub block */
      move_code_block = 1;
      move_scrub_block = 1;
      ++scrubc->debug_info.copied_blocks_count;
      scrubc->debug_info.copied_pages_count += (uint32)result;
    }
    else
    {
       switch (result)
       {
         case FLASH_SCRUB_EDEVREAD:
         {
           /* code block is either erased or has no good written-pages.
            * move code block
            * move scrub block */
           move_code_block = 1;
           move_scrub_block = 1;
          ++scrubc->debug_info.empty_blocks_count;
         }
         break;

         case FLASH_SCRUB_EDEVWRITE:
         case FLASH_SCRUB_EDEVERASE:
         {
           /* problem in the scrub-partition
            * move only scrub block
            * reduce scrub-parti length by 1
            * increase code-parti length by 1 */
           move_scrub_block = 1;
           scrub_entry->length--;
           code_entry->length++;
          ++scrubc->debug_info.bad_blocks_count;
         }
         break;

         case FLASH_SCRUB_EBADBLOCK:
         {
           /* problem in the code-partition
            * move only code block
            * reduce code-parti length by 1
            * increase scrub-parti length by 1 */
           move_code_block = 1;
           code_entry->length--;
           scrub_entry->length++;
          ++scrubc->debug_info.src_bad_blocks_count;
         }
         break;

         default:
         {
           FLASH_SCRUB_ERR_FATAL ("%d, unknown result", result, 0, 0);
         }
         break;
       }
    }

    if (move_code_block)
    {
      if (incr < 0)
      {
        code_block--;
        code_end_blk--;
      }
      else
      {
        code_block++;
        code_beg_blk++;
      }
    }

    if (move_scrub_block)
    {
      if (incr < 0)
      {
        scrub_block--;
        scrub_end_blk--;
      }
      else
      {
        scrub_block++;
        scrub_beg_blk++;
      }
    }

  } /* end of for loop */

  FLASH_SCRUB_LOG_MSG_3 ("C = %d, E = %d, B = %d,one code-parti complete1",
                  scrubc->debug_info.copied_blocks_count,
                  scrubc->debug_info.empty_blocks_count,
                  scrubc->debug_info.bad_blocks_count);
  FLASH_SCRUB_LOG_MSG_2 ("B = %d, P = %d,one code-parti complete2",
                  scrubc->debug_info.src_bad_blocks_count,
                  scrubc->debug_info.copied_pages_count);


  /*---------------------------------------------------------------------------
   * Verify md5 for the newly copied code partition.
  ---------------------------------------------------------------------------*/
  if (incr < 0)
  {
    md5_beg_blk = scrub_len - code_entry->length;
  }
  else
  {
    md5_beg_blk = 0;
  }

  flash_scrub_util_compute_md5 (scrub_handle, md5_beg_blk,
                          md5_beg_blk + code_entry->length,
                          &scrubc->md5_ctx,
                          scrubc->md5_hash1);

  snprintf (scrubc->temp_buffer, sizeof (scrubc->temp_buffer),
           "%s/%s", FLASH_SCRUB_MD5_DIRECTORY, code_entry->name);

  result = efs_get (scrubc->temp_buffer, scrubc->md5_hash2,
                    sizeof (scrubc->md5_hash2));
  if (result != sizeof (scrubc->md5_hash2))
  {
    error = FLASH_SCRUB_EMD5;
    skip_mibib_update = 1;
    FLASH_SCRUB_DEBUG_ERR_FATAL ("%d, efs_get failed.", efs_errno, 0, 0);
    FLASH_SCRUB_LOG_ERR_1 ("%d, efs_get failed.", efs_errno);
    goto End;
  }

  result = memcmp (scrubc->md5_hash1, scrubc->md5_hash2,
                   sizeof (scrubc->md5_hash1));
  if (result != 0)
  {
    error = FLASH_SCRUB_EMD5;
    skip_mibib_update = 1;
    FLASH_SCRUB_DEBUG_ERR_FATAL ("MD5 mismatch : %d, %d", 0, 0, 0);
    FLASH_SCRUB_LOG_ERR_2 ("MD5 mismatch : %d, %d",
                         code_parti_index, scrub_parti_index);
    goto End;
  }

  /* Update the offset and length values for code partition and scrub partition
   * in the partition table copy and then update the system partition table in
   * MIBIB. */
  if (incr < 0)
  {
    code_new_beg_blk = scrub_new_beg_blk + scrub_entry->length;
    scrub_parti_update_limit (scrubc, code_entry, code_new_beg_blk,
                              code_parti_index);
    scrub_parti_update_limit (scrubc, scrub_entry, scrub_new_beg_blk,
                              scrub_parti_index);
  }
  else
  {
    scrub_new_beg_blk = code_new_beg_blk + code_entry->length;
    scrub_parti_update_limit (scrubc, code_entry, code_new_beg_blk,
                              code_parti_index);
    scrub_parti_update_limit (scrubc, scrub_entry, scrub_new_beg_blk,
                              scrub_parti_index);
  }

  scrub_parti_swap_entry (scrubc, code_parti_index, scrub_parti_index);

  new_code_entry = &scrubc->mibib_parti_table.part_entry[scrub_parti_index];
  new_scrub_entry = &scrubc->mibib_parti_table.part_entry[code_parti_index];

  scrubc->debug_info.new_code_parti_start = new_code_entry->offset;
  scrubc->debug_info.new_code_parti_end =
                 (new_code_entry->offset + new_code_entry->length - 1);
  scrubc->debug_info.new_code_parti_len = new_code_entry->length;
  FLASH_SCRUB_LOG_MSG_3  ("New-code : S = %d, E = %d, L = %d",
                        scrubc->debug_info.new_code_parti_start,
                        scrubc->debug_info.new_code_parti_end,
                        scrubc->debug_info.new_code_parti_len);


  scrubc->debug_info.new_scrub_parti_start = new_scrub_entry->offset;
  scrubc->debug_info.new_scrub_parti_end =
                 (new_scrub_entry->offset + new_scrub_entry->length - 1);
  scrubc->debug_info.new_scrub_parti_len = new_scrub_entry->length;
  FLASH_SCRUB_LOG_MSG_3  ("New-Scrub : S = %d, E = %d, L = %d",
                        scrubc->debug_info.new_scrub_parti_start,
                        scrubc->debug_info.new_scrub_parti_end,
                        scrubc->debug_info.new_scrub_parti_len);



End:
  result = flash_scrub_shim_close_partition (scrub_handle);
  ASSERT (result == FLASH_DEVICE_DONE);

  result = flash_scrub_shim_close_partition (code_handle);
  ASSERT (result == FLASH_DEVICE_DONE);

  if (!skip_mibib_update)
  {
    mibib_handle =
            flash_scrub_shim_open_partition (FLASH_SCRUB_MIBIB_PARTITION_NAME);
    if (mibib_handle == NULL)
    {
      FLASH_SCRUB_ERR_FATAL ("MIBIB open-parti failed", 0, 0, 0);
    }

    result = FLASH_DEVICE_FAIL;
    while ((result != FLASH_DEVICE_DONE) &&
           (retry_count < FLASH_SCRUB_MIBIB_UPDATE_MAX_RETRY_COUNT))
    {
      result = flash_scrub_shim_partition_table_update (mibib_handle,
                                         &scrubc->mibib_parti_table);
      if (result != FLASH_DEVICE_DONE)
      {
        FLASH_SCRUB_LOG_ERR_3 (
                              "scrubc: partition table update fail [%d,%d,%d]",
                              result, code_parti_index, scrub_parti_index);
        ++retry_count;
      }
    }

    if (result != FLASH_DEVICE_DONE)
    {
      error = FLASH_SCRUB_EUPDATEPARTI;
      FLASH_SCRUB_DEBUG_ERR_FATAL ("%d, update-parti failed.", result, 0, 0);

      /* Revert the changes in the partition table global variable */
      result = flash_scrub_shim_partition_table_get (mibib_handle,
                                                 &scrubc->mibib_parti_table);
      if (result != FLASH_DEVICE_DONE)
      {
        FLASH_SCRUB_ERR_FATAL ("[%d] parti-tbl-get failed", result, 0, 0);
      }
    }

    result = flash_scrub_shim_close_partition (mibib_handle);
    ASSERT (result == FLASH_DEVICE_DONE);
  }

  stop_tick = flash_scrub_timetick_get_timetick ();
  diff_tick = stop_tick - start_tick;
  time_in_us = flash_scrub_timetick_convert_to_us (diff_tick);
  time_in_ms = (uint32)(time_in_us / 1000);

  FLASH_SCRUB_LOG_MSG_1 ("took ms = %d, one code-parti complete3",
                        time_in_ms);

  return error;
}

static void
scrub_finish_scrub_process (struct scrub_code_info_type *scrubc, uint32 seqno)
{
  /* Current one scrub-code-cylcle is complete, now toggle scrub direction */
  switch (scrubc->scrub_direction)
  {
    case SCRUB_DIRECTION_LOW_TO_HIGH:
    {
      scrubc->scrub_direction = SCRUB_DIRECTION_HIGH_TO_LOW;
    }
    break;

    case SCRUB_DIRECTION_HIGH_TO_LOW:
    {
      scrubc->scrub_direction = SCRUB_DIRECTION_LOW_TO_HIGH;
    }
    break;

    default:
    {
      FLASH_SCRUB_ERR_FATAL ("%d, Invalid scrub direction",
                             scrubc->scrub_direction, 0, 0);
    }
    break;
  }

  /* Mark the current scrub as complete and record it in the info file. */
  scrubc->info_file.direction = (uint32)scrubc->scrub_direction;
  scrubc->info_file.seq_num = seqno;
  scrub_code_save_info_file (scrubc);
}


int
flash_scrub_code_refresh_md5 (void)
{
  struct scrub_code_info_type *scrubc;

  scrubc = &scrub_code_info_inst;

  if (!scrub_code_info_inst.is_config_okay)
  {
    return FLASH_SCRUB_EINVALPARTI;
  }

  scrub_code_compute_md5_for_all_code (scrubc);

  return 0;
}

