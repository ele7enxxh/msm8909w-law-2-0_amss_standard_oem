/****************************************************************************
 * flash_scrub_master.c
 *
 * The master state machine, keeps track of individual scrub operations.
 * Copyright (C) 2013-2014 QUALCOMM Technologies, Inc.
 *
 * The scrub operation spans several types of data: Code Partitions read by
 * boot loader, Boot code read by Boot Rom and different file system data.
 * This module keeps track of:
 * - Has the scrubbing operation started.
 * - At what stage of scrubbing the system is in, so it can resume on power
 *   cycle.
 * - When was the last scrubbing operation completed
 *
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_master.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-11-20   mj    Delay added after sending notification to Apps.
2015-08-07   mj    Change declaration of timer_group.
2014-04-28   vm    Use correct MSG macros to save RAM.
2013-12-12   rp    Allow disabling SCRUB feature at runtime.
2013-12-09   wek   Add macro to print F3 messages and log to efs file.
2013-11-16   dks   Add flash_ops functions.
2013-10-04   dks   Handle Fota update message from apps.
2013-10-03   dks   Send indication of apps scrub start.
2013-09-17   dks   Add fields in info file to remember if last scrub failed.
2013-09-09   rp    Add more debug F3 messages for FOTA and shutdown.
2013-09-06   rp    Add more debug F3 messages and logs.
2013-09-05   dks   Fix accuracy of scrub timer when no system time available.
2013-09-04   rp    Cleanup the Scrub Log Module.
2013-09-01   wek   Add MD5 verification for SBL partition.
2013-08-29   rp    Recompute MD5 due to FOTA.
2013-08-21   rp    Add timetick support to do timing-profile.
2013-08-20   rp    Correct scrub-timer and system-timer initialization.
2013-08-15   rp    Prefix flash_scrub to all non-static functions.
2013-08-15   rp    Make all target file names to start with flash_scrub.
2013-08-14   rp    Add f3 messages for debug.
2013-08-13   dks   Add hooks for EFS scrubbing.
2013-08-12   rp    Add APIs to change scrub timing values.
2013-08-12   rp    Do not compute MD5-hash at rcinit boot time.
2013-08-04   rp    Add Scrub-Log module.
2013-08-01   rp    Create.

===========================================================================*/
#include "flash_scrub_config_i.h"
#include "flash_scrub_master.h"
#include "flash_scrub_global_lock.h"
#include "flash_scrub.h"
#include "flash_scrub_flash_dal_shim.h"
#include "flash_scrub_err.h"
#include "comdef.h"
#include "assert.h"
#include "fs_public.h"
#include "fs_errno.h"
#include "fs_priv_funcs.h"
#include "fds.h"
#include "rex.h"
#include "time_svc.h"
#include "timer.h"

#include "flash_scrub_boot_code.h"
#include "flash_scrub_code_partitions.h"
#include "flash_scrub_priv_funcs.h"
#include "flash_scrub_log.h"
#include "flash_scrub_timetick.h"

#define SCRUB_MASTER_EFS_OFLAG       (O_CREAT | O_RDWR | O_AUTODIR)
#define SCRUB_MASTER_EFS_MODE        (0644)

/* Delay in seconds where flash_scrub module waits for apps
 * to receive notification */
#define SCRUB_SLEEP_TIMEOUT_IN_SECS 10

/*---------------------------------------------------------------------------
 * Scrub-Master-Configuration file:
 * This file is created on the first ever-boot of the phone and is ONLY read
 * there-after.
---------------------------------------------------------------------------*/
#define SCRUB_MASTER_CONFIG_FILE_VERSION           (1)
#define SCRUB_MASTER_CONFIG_FILE_MAGIC_STRING      "scrubM-C"
#define SCRUB_MASTER_CONFIG_FILE_MAGIC_STRING_LEN  (8)

PACKED struct scrub_master_config_file_type
{
  uint32 version;
  uint8  magic_string[8];
  uint32 forced_minimum_time_in_secs_between_2_scrubs;
  uint32 timer_value_in_secs_for_automatic_scrubs;
  uint32 timer_val_in_secs_for_saving_time_in_efs;
  uint32 is_qmi_cmds_to_trigger_scrub_enabled;
} PACKED_POST;


/*---------------------------------------------------------------------------
 * Scrub-Master-Information file:
 * This file is updated everytime Scrub-Master's state changes.
---------------------------------------------------------------------------*/
#define SCRUB_MASTER_INFO_FILE_VERSION             (1)
#define SCRUB_MASTER_INFO_FILE_MAGIC_STRING        "scrubM-I"
#define SCRUB_MASTER_INFO_FILE_MAGIC_STRING_LEN    (8)

PACKED struct scrub_master_info_file_type
{
  uint32 version;
  uint8  magic_string[8];
  uint32 state;
  uint32 scrub_seqno;
  uint64 system_time_of_last_scrub;
  uint32 recompute_md5_after_fota;
  uint64 time_since_last_scrub_in_ms;
  uint64 last_known_system_time;
  uint32 did_last_scrub_fail;
  uint8 reserved[8]; /* For future use so we don't end up breaking backward
                        compatibility in case we need to add more fields. Since
                        this is stored as an EFS Item file, we need to maintain
                        size of the structure. */
} PACKED_POST;


#define SCRUB_MASTER_DEBUG_FILE_VERSION            1
#define SCRUB_MASTER_DEBUG_FILE_MAGIC_STRING       "scrubM-D"
#define SCRUB_MASTER_DEBUG_FILE_MAGIC_STRING_LEN   (8)
#define SCRUB_MASTER_RESULT_QUEUE_SIZE              (5)

PACKED struct scrub_master_debug_file_type
{
  uint32 version;
  uint8  magic_string[SCRUB_MASTER_DEBUG_FILE_MAGIC_STRING_LEN];
  /* Last few scrub results */
  int boot_scrub_results[SCRUB_MASTER_RESULT_QUEUE_SIZE];
  int code_scrub_results[SCRUB_MASTER_RESULT_QUEUE_SIZE];
  int efs_scrub_results [SCRUB_MASTER_RESULT_QUEUE_SIZE];
  int apps_scrub_results[SCRUB_MASTER_RESULT_QUEUE_SIZE];
  uint8 curr_scrub_res_index;
  uint8 reserved[8]; /* For future use so we don't end up breaking backward
                        compatibility in case we need to add more fields. Since
                        this is stored as an EFS Item file, we need to maintain
                        size of the structure. */
} PACKED_POST;


/*---------------------------------------------------------------------------
 * Scrub-Master State-Machine.

        SCRUB_MASTER_STATE_IDLE_STATE <-----------------|
                      |                                 |
                      |                                 |
        SCRUB_MASTER_STATE_BOOT_SCRUB_STATE             |
                      |                                 |
                      |                                 |
        SCRUB_MASTER_STATE_CODE_SCRUB_STATE             |
                      |                                 |
                      |                                 |
        SCRUB_MASTER_STATE_EFS_SCRUB_STATE              |
                      |                                 |
                      |                                 |
        SCRUB_MASTER_STATE_APPS_FS_SCRUB_STATE          |
                      |                                 |
                      |------------->------------------>|


---------------------------------------------------------------------------*/
enum scrub_master_state_type
{
  SCRUB_MASTER_STATE_IDLE_STATE,
  SCRUB_MASTER_STATE_BOOT_SCRUB_STATE,
  SCRUB_MASTER_STATE_CODE_SCRUB_STATE,
  SCRUB_MASTER_STATE_EFS_SCRUB_STATE,
  SCRUB_MASTER_STATE_APPS_FS_SCRUB_STATE,
};

/*---------------------------------------------------------------------------
 * Scrub-Master-main structure.
---------------------------------------------------------------------------*/
struct scrub_master_info_type
{
  rex_tcb_type *scrub_task_tcb;

  struct scrub_master_config_file_type config_file;
  struct scrub_master_info_file_type   info_file;
  struct scrub_master_debug_file_type  debug_file;

  enum scrub_master_state_type state;
  uint32 curr_scrub_seqno;
  uint32 scrub_count_in_this_session;
  int is_scrub_in_progress;

  int is_scrub_enabled;
  uint32 scrub_parti_count;

  timer_group_type          timer_group;
  timer_type                scrub_timer;
  timer_type                system_timer;
  /* Timer used to periodically update the acquired time
     and time since last scrub information into info file */
  timer_type                save_time_in_efs_timer;

  uint64 scrub_timer_val_in_ms;
  uint64 min_time_between_2scrubs_in_ms;
  uint64 system_timer_val_in_ms;
  uint64 save_time_in_efs_timer_val_in_ms;
  uint64 jan_1_2013_in_ms;
  uint64 boot_timetick;
  uint64 timetick_when_time_saved_in_efs;
  FLASH_SCRUB_BOOT_HANDLE *boot_handle;
};

static struct scrub_master_info_type scrub_master_info_inst;
static void scrub_master_start_scrub_timer (
                  struct scrub_master_info_type *scrubm);
static void scrub_master_save_time_in_efs (
                  struct scrub_master_info_type *scrubm);
static uint64 scrub_master_get_current_time (
                  struct scrub_master_info_type *scrubm);
static void scrub_master_set_time_of_scrub (
                  struct scrub_master_info_type *scrubm, uint64 curr_time);

/*---------------------------------------------------------------------------
 * Scrub-Master-Configuration file:
---------------------------------------------------------------------------*/
static int
scrub_master_load_config_file (struct scrub_master_info_type *scrubm)
{
  int result = FLASH_SCRUB_EEFS, temp_res;

  result = efs_get (FLASH_SCRUB_MASTER_CONFIG_FILEPATH, &scrubm->config_file,
                    sizeof (scrubm->config_file));
  if (result != sizeof (scrubm->config_file))
  {
    goto End;
  }

  if (scrubm->config_file.version != SCRUB_MASTER_CONFIG_FILE_VERSION)
  {
    goto End;
  }

  temp_res = memcmp (scrubm->config_file.magic_string,
                     SCRUB_MASTER_CONFIG_FILE_MAGIC_STRING,
                     SCRUB_MASTER_CONFIG_FILE_MAGIC_STRING_LEN);
  if (temp_res != 0)
  {
    goto End;
  }

  result = 0;

End:
  return result;
}

static void
scrub_master_save_config_file (struct scrub_master_info_type *scrubm)
{
  int result;

  result = efs_put (FLASH_SCRUB_MASTER_CONFIG_FILEPATH, &scrubm->config_file,
                    sizeof (scrubm->config_file), SCRUB_MASTER_EFS_OFLAG,
                    SCRUB_MASTER_EFS_MODE);
  if (result != 0)
  {
    FLASH_SCRUB_ERR_FATAL ("%d, efs_put failed.", efs_errno, 0, 0);
  }
}

static void
scrub_master_create_config_file (struct scrub_master_info_type *scrubm)
{
  uint32 is_qmi_enabled;

  memset (&scrubm->config_file, 0, sizeof (scrubm->config_file));

  scrubm->config_file.version = SCRUB_MASTER_CONFIG_FILE_VERSION;

  memcpy (scrubm->config_file.magic_string,
          SCRUB_MASTER_CONFIG_FILE_MAGIC_STRING,
          sizeof (scrubm->config_file.magic_string));

  scrubm->config_file.forced_minimum_time_in_secs_between_2_scrubs =
                        FLASH_SCRUB_MINIMUM_TIME_IN_SECS_BETWEEN_2_SCRUBS;

  scrubm->config_file.timer_value_in_secs_for_automatic_scrubs =
                        FLASH_SCRUB_TIMER_VALUE_IN_SECS_FOR_AUTOMATIC_SCRUBS;

  scrubm->config_file.timer_val_in_secs_for_saving_time_in_efs =
                      FLASH_SCRUB_TIMER_VALUE_IN_SECS_FOR_SAVING_TIME_IN_EFS;

  is_qmi_enabled = 0;
#ifdef FLASH_SCRUB_QMI_COMMANDS_SUPPORT
  is_qmi_enabled = 1;
#endif
  scrubm->config_file.is_qmi_cmds_to_trigger_scrub_enabled = is_qmi_enabled;

  scrub_master_save_config_file (scrubm);

  FLASH_SCRUB_LOG_MSG_3 ("scrubm config-file qmi = %d, min = %d, timer = %d",
        is_qmi_enabled,
        scrubm->config_file.forced_minimum_time_in_secs_between_2_scrubs,
        scrubm->config_file.timer_value_in_secs_for_automatic_scrubs);
}


/*---------------------------------------------------------------------------
 * Scrub-Master-Information file:
---------------------------------------------------------------------------*/
static int
scrub_master_load_info_file (struct scrub_master_info_type *scrubm)
{
  int result = FLASH_SCRUB_EEFS, temp_res;

  result = efs_get (FLASH_SCRUB_MASTER_INFO_FILEPATH, &scrubm->info_file,
                    sizeof (scrubm->info_file));
  if (result != sizeof (scrubm->info_file))
  {
    goto End;
  }

  if (scrubm->info_file.version != SCRUB_MASTER_INFO_FILE_VERSION)
  {
    goto End;
  }

  temp_res = memcmp (scrubm->info_file.magic_string,
                     SCRUB_MASTER_INFO_FILE_MAGIC_STRING,
                     SCRUB_MASTER_INFO_FILE_MAGIC_STRING_LEN);
  if (temp_res != 0)
  {
    goto End;
  }

  result = 0;

End:
  return result;
}

static void
scrub_master_save_info_file (struct scrub_master_info_type *scrubm)
{
  int result;

  result = efs_put (FLASH_SCRUB_MASTER_INFO_FILEPATH, &scrubm->info_file,
                    sizeof (scrubm->info_file), SCRUB_MASTER_EFS_OFLAG,
                    SCRUB_MASTER_EFS_MODE);
  if (result != 0)
  {
    FLASH_SCRUB_ERR_FATAL ("%d, efs_put failed.", efs_errno, 0, 0);
  }
}

static void
scrub_master_create_info_file (struct scrub_master_info_type *scrubm)
{
  memset (&scrubm->info_file, 0, sizeof (scrubm->info_file));

  scrubm->info_file.version = SCRUB_MASTER_INFO_FILE_VERSION;

  memcpy (scrubm->info_file.magic_string,
          SCRUB_MASTER_INFO_FILE_MAGIC_STRING,
          sizeof (scrubm->info_file.magic_string));

  scrubm->info_file.state = SCRUB_MASTER_STATE_IDLE_STATE;
  scrubm->info_file.scrub_seqno = 0;
  scrubm->info_file.system_time_of_last_scrub = 0;
  scrubm->info_file.recompute_md5_after_fota = 0;
  scrubm->info_file.time_since_last_scrub_in_ms = 0;
  scrubm->info_file.last_known_system_time =
                       scrub_master_get_current_time (scrubm);
  scrub_master_save_info_file (scrubm);
}

/*---------------------------------------------------------------------------
 * Scrub-Master-Debug file:
---------------------------------------------------------------------------*/
static int
scrub_master_load_debug_file (struct scrub_master_info_type *scrubm)
{
  int result = FLASH_SCRUB_EEFS, temp_res;

  result = efs_get (FLASH_SCRUB_MASTER_DEBUG_FILEPATH, &scrubm->debug_file,
                    sizeof (scrubm->debug_file));
  if (result != sizeof (scrubm->debug_file))
  {
    goto End;
  }

  if (scrubm->debug_file.version != SCRUB_MASTER_INFO_FILE_VERSION)
  {
    goto End;
  }

  temp_res = memcmp (scrubm->debug_file.magic_string,
                     SCRUB_MASTER_DEBUG_FILE_MAGIC_STRING,
                     SCRUB_MASTER_DEBUG_FILE_MAGIC_STRING_LEN);
  if (temp_res != 0)
  {
    goto End;
  }

  result = 0;

End:
  return result;
}

static void
scrub_master_save_debug_file (struct scrub_master_info_type *scrubm)
{
  int result;

  result = efs_put (FLASH_SCRUB_MASTER_DEBUG_FILEPATH, &scrubm->debug_file,
                    sizeof (scrubm->debug_file), SCRUB_MASTER_EFS_OFLAG,
                    SCRUB_MASTER_EFS_MODE);
  if (result != 0)
  {
    FLASH_SCRUB_MSG_ERR_1 ("%d, debug file put failed.", efs_errno);
    flash_scrub_log_err ("%d, debug file put failed.", efs_errno, 0, 0);
  }
}

static void
scrub_master_create_debug_file (struct scrub_master_info_type *scrubm)
{
  memset (&scrubm->debug_file, 0, sizeof (scrubm->debug_file));

  scrubm->debug_file.version = SCRUB_MASTER_DEBUG_FILE_VERSION;

  memcpy (scrubm->debug_file.magic_string,
          SCRUB_MASTER_DEBUG_FILE_MAGIC_STRING,
          sizeof (scrubm->debug_file.magic_string));

  scrub_master_save_debug_file (scrubm);
}

static int
scrub_master_is_scrub_enabled (struct scrub_master_info_type *scrubm)
{
  flash_scrub_shim_handle flash_dev;
  int result;
  uint32 block_count;

  ASSERT (scrubm != NULL);
  scrubm->is_scrub_enabled = 0;

  flash_dev = flash_scrub_shim_open_partition (FLASH_SCRUB_PARTITION_NAME);
  if (flash_dev == NULL)
  {
    FLASH_SCRUB_LOG_ERR_0 ("Cannot open SCRUB partition");
    goto End;
  }

  block_count = flash_scrub_shim_get_block_count (flash_dev);

  result = flash_scrub_shim_close_partition (flash_dev);
  if (result != 0)
  {
    FLASH_SCRUB_LOG_ERR_1 ("%d, Cannot close SCRUB partition", result);
  }

  scrubm->scrub_parti_count = block_count;
  if (block_count < FLASH_SCRUB_MIN_SCRUB_PARTITION_BLOCK_COUNT)
  {
    FLASH_SCRUB_LOG_ERR_1 ("%d, SCRUB partition too small.", block_count);
    goto End;
  }

  scrubm->is_scrub_enabled = 1;

End:
  return scrubm->is_scrub_enabled;
}


static uint64
scrub_master_get_current_time (struct scrub_master_info_type *scrubm)
{
  uint64 curr_time;

  curr_time = time_get_ms_native ();

  if ((curr_time < scrubm->jan_1_2013_in_ms) ||
      (curr_time < scrubm->info_file.system_time_of_last_scrub))
  {
    curr_time = 0;
  }

  return curr_time;
}

/*---------------------------------------------------------------------------
 * Simulator callback test function.
---------------------------------------------------------------------------*/
void (*scrub_master_sim_tests_callback_func) (uint32 scrub_seqno,uint32 state);

static void
scrub_master_sim_tests_callback (struct scrub_master_info_type *scrubm)
{
  (void) scrubm;
  (void) scrub_master_sim_tests_callback_func;

#ifdef FLASH_SCRUB_UNIT_TEST
  if (scrub_master_sim_tests_callback_func)
  {
    scrub_master_sim_tests_callback_func (scrubm->curr_scrub_seqno,
                                          (uint32)scrubm->state);
  }
#endif
}


/*---------------------------------------------------------------------------
 * Scrub-Master state-machine functions.
 * All these functions are execute without acquiring data-lock, so none of
 * these functions should update scrub-master variables.
---------------------------------------------------------------------------*/
static int
scrub_master_boot_scrub_state (struct scrub_master_info_type *scrubm)
{
  int result;

  ASSERT (scrubm != NULL);
  ASSERT (scrubm->state == SCRUB_MASTER_STATE_BOOT_SCRUB_STATE);

  result = flash_scrub_boot (scrubm->boot_handle, scrubm->curr_scrub_seqno);

  return result;
}

static int
scrub_master_code_scrub_state (struct scrub_master_info_type *scrubm)
{
  int result;

  ASSERT (scrubm != NULL);
  ASSERT (scrubm->state == SCRUB_MASTER_STATE_CODE_SCRUB_STATE);

  result = flash_scrub_all_code_partitions (scrubm->curr_scrub_seqno);

  return result;
}

static int
scrub_master_efs_scrub_state (struct scrub_master_info_type *scrubm)
{
  int result;

  ASSERT (scrubm != NULL);
  ASSERT (scrubm->state == SCRUB_MASTER_STATE_EFS_SCRUB_STATE);

  result = efs_full_scrub ("/");

  return result;
}

static int
scrub_master_apps_fs_scrub_state (struct scrub_master_info_type *scrubm)
{
  int result;

  ASSERT (scrubm != NULL);
  ASSERT (scrubm->state == SCRUB_MASTER_STATE_APPS_FS_SCRUB_STATE);

  result = fds_scrub_send_apps_scrub_notification (scrubm->curr_scrub_seqno);

  FLASH_SCRUB_MSG_HIGH_0 ("Flash Scrub:Sent Apps Scrub Notification");
  timer_sleep(SCRUB_SLEEP_TIMEOUT_IN_SECS, T_SEC, TRUE);
  return result;
}


static void
scrub_master_set_debug_index (struct scrub_master_info_type *scrubm)
{
  uint8 index, expected_index;

  index = scrubm->debug_file.curr_scrub_res_index;
  expected_index = scrubm->curr_scrub_seqno % SCRUB_MASTER_RESULT_QUEUE_SIZE;

  if (index != expected_index)
  {
    scrubm->debug_file.curr_scrub_res_index = expected_index;
    index = scrubm->debug_file.curr_scrub_res_index;
    scrub_master_save_debug_file (scrubm);
  }
}

static void
scrub_master_save_debug_result (struct scrub_master_info_type *scrubm,
                                int result)
{
  uint32 scrub_result_index;

  scrub_result_index = scrubm->debug_file.curr_scrub_res_index;

  switch (scrubm->state)
  {
    case SCRUB_MASTER_STATE_IDLE_STATE:
      return;

    case SCRUB_MASTER_STATE_BOOT_SCRUB_STATE:
      scrubm->debug_file.boot_scrub_results[scrub_result_index] = result;
    break;

    case SCRUB_MASTER_STATE_CODE_SCRUB_STATE:
      scrubm->debug_file.code_scrub_results[scrub_result_index] = result;
    break;

    case SCRUB_MASTER_STATE_EFS_SCRUB_STATE:
      scrubm->debug_file.efs_scrub_results[scrub_result_index] = result;
    break;

    case SCRUB_MASTER_STATE_APPS_FS_SCRUB_STATE:
      scrubm->debug_file.apps_scrub_results[scrub_result_index] = result;
    break;

    default:
      FLASH_SCRUB_ERR_FATAL ("%d, Incorrect scrub state in save results",
                             scrubm->state, 0, 0);
      break;
  }

  scrub_master_save_debug_file (scrubm);
}


static void
scrub_master_do_scrub (struct scrub_master_info_type *scrubm)
{
  enum scrub_master_state_type next_state;
  uint64 curr_time, start_tick, stop_tick, diff_tick, time_in_us;
  uint32 time_in_ms;
  uint64 scrub_start_tick;
  int run_loop = 1, result;

  ASSERT (scrubm != NULL);
  FLASH_SCRUB_MSG_HIGH_0 ("Inside scrub_master_do_scrub, Going to sleep");
  timer_sleep(SCRUB_SLEEP_TIMEOUT_IN_SECS, T_SEC, TRUE);
  FLASH_SCRUB_MSG_HIGH_0 ("Proceeding with scrub_master_do_scrub");

  scrubm->is_scrub_in_progress = 1;
  curr_time = scrub_master_get_current_time (scrubm);

  scrub_start_tick = flash_scrub_timetick_get_timetick ();

  if (scrubm->state == SCRUB_MASTER_STATE_IDLE_STATE)
  {
    /* Sign-up for this scrub. */
    ++scrubm->curr_scrub_seqno;
    if (scrubm->curr_scrub_seqno == 0)
    {
      scrubm->curr_scrub_seqno = 1;
    }

    scrub_master_sim_tests_callback (scrubm);

    scrubm->state = SCRUB_MASTER_STATE_BOOT_SCRUB_STATE;

    scrub_master_set_debug_index (scrubm);

    scrubm->info_file.scrub_seqno = scrubm->curr_scrub_seqno;
    scrubm->info_file.state = (uint32)scrubm->state;
    scrubm->info_file.did_last_scrub_fail = 0;
    scrub_master_set_time_of_scrub (scrubm, curr_time);
    scrub_master_save_info_file (scrubm);

    FLASH_SCRUB_LOG_MSG_3 ("---- scrubm-start : seq = %d, at = %d,%d",
                         scrubm->curr_scrub_seqno, (uint32)(curr_time >> 32),
                         (uint32)curr_time);
  }
  else
  {
    /* We are resuming scrub started in the previous session. */
    FLASH_SCRUB_LOG_MSG_3 ("--- scrubm-resume : seq = %d, at = %d,%d",
                         scrubm->curr_scrub_seqno, (uint32)(curr_time >> 32),
                         (uint32)curr_time);
  }

  next_state = scrubm->state;

  ++scrubm->scrub_count_in_this_session;

  /* Leave the data-lock to perform the actual scrub. */
  FLASH_SCRUB_GLOBAL_UNLOCK ();

  while (run_loop)
  {
    result = 0;

    scrub_master_sim_tests_callback (scrubm);

    start_tick = flash_scrub_timetick_get_timetick ();

    FLASH_SCRUB_MSG_HIGH_2 ("-- scrubm: seq = %d, state = %d, start--",
                          scrubm->curr_scrub_seqno, scrubm->state);

    switch (scrubm->state)
    {
      case SCRUB_MASTER_STATE_IDLE_STATE:
        run_loop = 0;
      break;

      case SCRUB_MASTER_STATE_BOOT_SCRUB_STATE:
        result = scrub_master_boot_scrub_state (scrubm);
        next_state = SCRUB_MASTER_STATE_CODE_SCRUB_STATE;
      break;

      case SCRUB_MASTER_STATE_CODE_SCRUB_STATE:
        result = scrub_master_code_scrub_state (scrubm);
        next_state = SCRUB_MASTER_STATE_EFS_SCRUB_STATE;
        break;

      case SCRUB_MASTER_STATE_EFS_SCRUB_STATE:
        result = scrub_master_efs_scrub_state (scrubm);
        next_state = SCRUB_MASTER_STATE_APPS_FS_SCRUB_STATE;
        break;

      case SCRUB_MASTER_STATE_APPS_FS_SCRUB_STATE:
        result = scrub_master_apps_fs_scrub_state (scrubm);
        next_state = SCRUB_MASTER_STATE_IDLE_STATE;
        break;

      default:
        FLASH_SCRUB_ERR_FATAL ("%d, Unknown scrub state", scrubm->state, 0, 0);
        break;
    }

    stop_tick = flash_scrub_timetick_get_timetick ();
    diff_tick = stop_tick - start_tick;
    time_in_us = flash_scrub_timetick_convert_to_us (diff_tick);
    time_in_ms = (uint32)(time_in_us / 1000);

    FLASH_SCRUB_MSG_HIGH_2 ("-- scrubm: seq = %d, time ms = %d complete",
                          scrubm->curr_scrub_seqno, time_in_ms);

    /* Update the scrub-info file about this state-change. */
    FLASH_SCRUB_GLOBAL_LOCK ();

    scrub_master_save_debug_result (scrubm, result);

    if (result != 0)
    {
      scrubm->info_file.did_last_scrub_fail = 1;
    }

    scrubm->state = next_state;

    scrubm->info_file.state = (uint32)scrubm->state;

    scrub_master_save_info_file (scrubm);

    FLASH_SCRUB_LOG_MSG_2 ("scrubm: seq = %d, time = %d complete",
                          scrubm->curr_scrub_seqno, time_in_ms);

    FLASH_SCRUB_GLOBAL_UNLOCK ();
  }

  FLASH_SCRUB_GLOBAL_LOCK ();
  ASSERT (scrubm->state == SCRUB_MASTER_STATE_IDLE_STATE);
  scrubm->is_scrub_in_progress = 0;

  stop_tick = flash_scrub_timetick_get_timetick ();
  diff_tick = stop_tick - scrub_start_tick;
  time_in_us = flash_scrub_timetick_convert_to_us (diff_tick);
  time_in_ms = (uint32)(time_in_us / 1000);

  FLASH_SCRUB_LOG_MSG_2 ("---- scrubm: SCRUB-END seq = %d, time ms=%d",
                       scrubm->curr_scrub_seqno, time_in_ms);

  /* This scrub is now complete, set timer for next scrub. */
  scrub_master_start_scrub_timer (scrubm);
}

static void
scrub_master_start_system_timer (struct scrub_master_info_type *scrubm)
{
  timer_error_type tres;
  uint64 stop_tick, diff_tick, time_in_us;
  uint32 time_in_ms, time_in_secs;

  ASSERT (scrubm != NULL);

  stop_tick = flash_scrub_timetick_get_timetick ();
  diff_tick = stop_tick - scrubm->boot_timetick;
  time_in_us = flash_scrub_timetick_convert_to_us (diff_tick);
  time_in_ms = (uint32)(time_in_us / 1000);
  time_in_secs = time_in_ms / 1000;

  FLASH_SCRUB_MSG_HIGH_3 (
                        "Trying sys-time since ms = %d secs = %d, Timer = %d",
                        time_in_ms, time_in_secs,
                        (uint32)scrubm->system_timer_val_in_ms);

  tres = timer_set_64 (&scrubm->system_timer, scrubm->system_timer_val_in_ms,
                       (time_timetick_type) 0, T_MSEC);
  if (tres != TE_SUCCESS)
  {
    FLASH_SCRUB_ERR_FATAL ("%d,%d,%d timer_set_64 failed.", tres,
                           (uint32)(scrubm->system_timer_val_in_ms >> 32),
                           (uint32)scrubm->system_timer_val_in_ms);
  }

  /* Increase the system-timer by 2 times to retry acquiring system-time
   * again next-time*/
  scrubm->system_timer_val_in_ms *= 2;
  if (scrubm->system_timer_val_in_ms >
      FLASH_SCRUB_MASTER_SYSTEM_TIMER_RETRY_IN_MSEC_MAX)
  {
    scrubm->system_timer_val_in_ms =
            FLASH_SCRUB_MASTER_SYSTEM_TIMER_RETRY_IN_MSEC_MAX;
  }
}

static void
scrub_master_set_time_of_scrub (struct scrub_master_info_type *scrubm,
                                uint64 curr_time)
{
  uint64 curr_timetick;

  ASSERT (scrubm != NULL);

  scrubm->info_file.system_time_of_last_scrub = curr_time;
  scrubm->info_file.time_since_last_scrub_in_ms = 0;
  scrubm->info_file.last_known_system_time = curr_time;

  curr_timetick = flash_scrub_timetick_get_timetick ();
  scrubm->timetick_when_time_saved_in_efs = curr_timetick;
}


/*---------------------------------------------------------------------------
  Logic for saving time since last scrub in EFS

  Legend:
     Current Time (CT)    : System time available now? i.e CT != 0 ?
     Scrub Time (ST)      : System time availbale when last scrub happened?
                            i.e ST != 0 ?
     Last available Time  : Was system time available at any time after scrub?
                            i.e LT != 0 ?
     tdiff                : Time since last scrub
     timetick_diff        : Timetick since time was saved in EFS.


   Flow chart:
                              CT > 0?
                                |
                     (Y)-----------------(N)
                      |                  |
                    ST > 0 ?             tdiff += timetick_diff
                      |
        (Y)--------------------(N)
          |                     |
          |                   LT > 0?
  tdiff = CT-ST      (Y)-----------------(N)
   LT = CT            |                   |
                      |                   |
                 tdiff += CT-LT        tdiff += timetick_diff
                 LT = CT               LT = CT
---------------------------------------------------------------------------*/

static void
scrub_master_save_time_in_efs (
                             struct scrub_master_info_type *scrubm)
{
  uint64 curr_time, curr_timetick, timetick_delta, time_delta_ms;

  ASSERT (scrubm != NULL);

  curr_time = scrub_master_get_current_time (scrubm);

  curr_timetick = flash_scrub_timetick_get_timetick ();
  timetick_delta = curr_timetick - scrubm->timetick_when_time_saved_in_efs;
  time_delta_ms = flash_scrub_timetick_convert_to_us (timetick_delta) / 1000;


  if (curr_time != 0) /* System time available. */
  {
    uint64 last_scrub_time = 0, last_known_system_time = 0;

    last_scrub_time = scrubm->info_file.system_time_of_last_scrub;
    last_known_system_time = scrubm->info_file.last_known_system_time;

    if (last_scrub_time != 0)
    {
      scrubm->info_file.time_since_last_scrub_in_ms =
                                        curr_time - last_scrub_time;
    }
    else
    {
      if (last_known_system_time != 0)
      {
        scrubm->info_file.time_since_last_scrub_in_ms +=
                                (curr_time - last_known_system_time);
      }
      else
      {
        scrubm->info_file.time_since_last_scrub_in_ms += time_delta_ms;
      }
    }

    scrubm->info_file.last_known_system_time = curr_time;
  }
  else /* No system time available */
  {
    scrubm->info_file.time_since_last_scrub_in_ms += time_delta_ms;
  }

  scrubm->timetick_when_time_saved_in_efs = curr_timetick;
  scrub_master_save_info_file (scrubm);
}

static void
scrub_master_start_save_time_in_efs_timer (
                                   struct scrub_master_info_type *scrubm)
{
  timer_error_type tres;

  ASSERT (scrubm != NULL);

  FLASH_SCRUB_MSG_HIGH_2 ("%d,%d, Starting timer to save time in efs",
    (uint32)(scrubm->save_time_in_efs_timer_val_in_ms >> 32),
    (uint32)scrubm->save_time_in_efs_timer_val_in_ms);

  tres = timer_set_64 (&scrubm->save_time_in_efs_timer,
                       scrubm->save_time_in_efs_timer_val_in_ms,
                       (time_timetick_type) 0, T_MSEC);
  if (tres != TE_SUCCESS)
  {
    FLASH_SCRUB_ERR_FATAL ("%d,%d,%d timer_set_64 failed.", tres,
          (uint32)(scrubm->save_time_in_efs_timer_val_in_ms >> 32),
          (uint32)scrubm->save_time_in_efs_timer_val_in_ms);
  }
}


static void
scrub_master_update_timer_val_to_save_time_in_efs (
                                   struct scrub_master_info_type *scrubm)
{
  uint64 curr_timetick, timetick_delta, next_timer_val_in_ms, time_delta_ms;
  timer_error_type tres;

  ASSERT (scrubm != NULL);

  curr_timetick = flash_scrub_timetick_get_timetick ();
  timetick_delta = curr_timetick - scrubm->timetick_when_time_saved_in_efs;
  time_delta_ms = flash_scrub_timetick_convert_to_us (timetick_delta) / 1000;

  if (time_delta_ms >= scrubm->save_time_in_efs_timer_val_in_ms)
  {
    rex_set_sigs (scrubm->scrub_task_tcb, FLASH_SCRUB_SAVE_TIME_IN_EFS_SIGNAL);
  }
  else
  {
    next_timer_val_in_ms = scrubm->save_time_in_efs_timer_val_in_ms -
                           time_delta_ms;
    tres = timer_set_64 (&scrubm->save_time_in_efs_timer, next_timer_val_in_ms,
                        (time_timetick_type) 0, T_MSEC);
    if (tres != TE_SUCCESS)
    {
      FLASH_SCRUB_ERR_FATAL ("%d,%d,%d timer_set_64 failed.", tres,
                             (uint32)(next_timer_val_in_ms >> 32),
                             (uint32)next_timer_val_in_ms);
    }
  }
}


static void
scrub_master_start_scrub_timer (struct scrub_master_info_type *scrubm)
{
  uint64 time_delta, next_timer_val_in_ms;
  timer_error_type tres;

  ASSERT (scrubm != NULL);

  if (scrubm->system_timer_val_in_ms == 0)
  {
    /* Automatic timer-based scrubbing is disabled. So bail. */
    return;
  }

  time_delta = scrubm->info_file.time_since_last_scrub_in_ms;
  if (time_delta >= scrubm->scrub_timer_val_in_ms)
  {
    /* time-elapsed since last scrub exceeds threshold, do a scrub now */
    rex_set_sigs (scrubm->scrub_task_tcb, FLASH_SCRUB_START_SCRUB_SIGNAL);
  }
  else
  {
    next_timer_val_in_ms = scrubm->scrub_timer_val_in_ms - time_delta;
    tres = timer_set_64 (&scrubm->scrub_timer, next_timer_val_in_ms,
                        (time_timetick_type) 0, T_MSEC);
    if (tres != TE_SUCCESS)
    {
      FLASH_SCRUB_ERR_FATAL ("%d,%d,%d timer_set_64 failed.", tres,
                             (uint32)(next_timer_val_in_ms >> 32),
                             (uint32)next_timer_val_in_ms);
    }
  }
}

void
flash_scrub_init (void)
{
  struct scrub_master_info_type *scrubm;
  uint64 curr_time;
  int result, is_scrub_enabled;

#ifdef FLASH_SCRUB_UNIT_TEST
  FLASH_SCRUB_GLOBAL_LOCK_INIT ();
#endif

  FLASH_SCRUB_GLOBAL_LOCK ();

  FLASH_SCRUB_MSG_HIGH_0 ("scrubm : Init begin");

  memset (&scrub_master_info_inst, 0, sizeof (scrub_master_info_inst));
  scrubm = &scrub_master_info_inst;

  scrubm->scrub_task_tcb = rex_self ();

  flash_scrub_timetick_init ();
  flash_scrub_log_init ();
  flash_scrub_shim_init ();

  scrubm->boot_timetick = flash_scrub_timetick_get_timetick ();
  scrubm->timetick_when_time_saved_in_efs = scrubm->boot_timetick;

  scrubm->jan_1_2013_in_ms = FLASH_SCRUB_MASTER_JAN_1_2013_TIME_VAL_IN_SECS;
  scrubm->jan_1_2013_in_ms *= 1000;

  result = scrub_master_load_config_file (scrubm);
  if (result != 0)
  {
    scrub_master_create_config_file (scrubm);
  }

  result = scrub_master_load_info_file (scrubm);
  if (result != 0)
  {
    scrub_master_create_info_file (scrubm);
  }

  result =  scrub_master_load_debug_file (scrubm);
  if (result != 0)
  {
    scrub_master_create_debug_file (scrubm);
  }

  is_scrub_enabled = scrub_master_is_scrub_enabled (scrubm);
  if (is_scrub_enabled != 1)
  {
    goto End;
  }

  scrubm->curr_scrub_seqno = scrubm->info_file.scrub_seqno;
  scrubm->state = scrubm->info_file.state;

  scrubm->scrub_timer_val_in_ms =
         scrubm->config_file.timer_value_in_secs_for_automatic_scrubs;
  scrubm->scrub_timer_val_in_ms *= 1000;

  scrubm->min_time_between_2scrubs_in_ms =
     scrubm->config_file.forced_minimum_time_in_secs_between_2_scrubs;
  scrubm->min_time_between_2scrubs_in_ms *= 1000;

  scrubm->system_timer_val_in_ms =
                                FLASH_SCRUB_MASTER_SYSTEM_TIMER_RETRY_IN_MSEC;

  scrubm->save_time_in_efs_timer_val_in_ms = 1000 *
      scrubm->config_file.timer_val_in_secs_for_saving_time_in_efs;

  scrubm->boot_handle = flash_scrub_boot_init (scrubm->info_file.scrub_seqno);
  flash_scrub_code_initialize ();

  timer_group_set_deferrable (&scrubm->timer_group, FALSE);

  timer_def (&scrubm->scrub_timer, &scrubm->timer_group,
             scrubm->scrub_task_tcb, FLASH_SCRUB_SCRUB_TIMER_EXPIRED_SIGNAL,
             NULL, (timer_cb_data_type)0);

  timer_def (&scrubm->system_timer, &scrubm->timer_group,
             scrubm->scrub_task_tcb, FLASH_SCRUB_SYSTEM_TIMER_EXPIRED_SIGNAL,
             NULL, (timer_cb_data_type)0);

  timer_def (&scrubm->save_time_in_efs_timer, &scrubm->timer_group,
             scrubm->scrub_task_tcb,
             FLASH_SCRUB_SAVE_TIME_IN_EFS_SIGNAL,
             NULL, (timer_cb_data_type)0);

  if (scrubm->state != SCRUB_MASTER_STATE_IDLE_STATE)
  {
    /* we were in the middle of a scrub, let's resume it. */
    FLASH_SCRUB_MSG_HIGH_0 ("scrubm : Resuming scrub in reset.");
    rex_set_sigs (scrubm->scrub_task_tcb, FLASH_SCRUB_START_SCRUB_SIGNAL);
  }

  if (scrubm->info_file.recompute_md5_after_fota == 1)
  {
    rex_set_sigs (scrubm->scrub_task_tcb,FLASH_SCRUB_FOTA_NOTIFICATION_SIGNAL);
  }

  curr_time = scrub_master_get_current_time (scrubm);
  if (curr_time == 0)
  {
    /* we do not have system-time, let's acquire system-time. */
    scrub_master_start_system_timer (scrubm);
  }
  else
  {
    /* we have system-time now. */
    FLASH_SCRUB_MSG_HIGH_2 ("system-time acquired. %d, %d",
                          (uint32)(curr_time >> 32), (uint32)curr_time);

    scrub_master_save_time_in_efs (scrubm);
  }

  scrub_master_start_scrub_timer (scrubm);

  scrub_master_start_save_time_in_efs_timer (scrubm);

  FLASH_SCRUB_LOG_MSG_2 ("scrubm-init : seq = %d, state = %d",
                       scrubm->curr_scrub_seqno, scrubm->state);

End:
  FLASH_SCRUB_GLOBAL_UNLOCK ();
}


static int
scrub_master_do_time_check_for_scrub (struct scrub_master_info_type *scrubm)
{
  uint64 curr_timetick, timetick_delta, time_delta_ms;
  uint64 curr_time, time_since_last_scrub_in_ms;
  int result = 0;

  ASSERT (scrubm != NULL);

  curr_time = scrub_master_get_current_time (scrubm);

  curr_timetick = flash_scrub_timetick_get_timetick ();
  timetick_delta = curr_timetick - scrubm->timetick_when_time_saved_in_efs;
  time_delta_ms = flash_scrub_timetick_convert_to_us (timetick_delta) / 1000;

  time_since_last_scrub_in_ms = time_delta_ms +
       scrubm->info_file.time_since_last_scrub_in_ms;

  if ((curr_time == 0) || (scrubm->info_file.system_time_of_last_scrub == 0))
  {
    /* If either of these are 0s, then we cannot say for sure how long
       it has been since previous scrub. Consult the elaspsed time in EFS. */
    if (time_since_last_scrub_in_ms >= scrubm->min_time_between_2scrubs_in_ms)
    {
      result = 0;  /* Insipte of not having exact times, it has been more
                     than mininum allowed time between scrubs. Ok to scrub */
    }
    else
    {
      result = FLASH_SCRUB_ETIME; /* Indicate we dont have system time */
    }
  }
  else  /* We know exactly how long it has been since previous scrub */
  {
    time_delta_ms = curr_time - scrubm->info_file.system_time_of_last_scrub;
    if (time_delta_ms >= scrubm->min_time_between_2scrubs_in_ms)
    {
      result = 0;
    }
    else
    {
      result = FLASH_SCRUB_EAGAIN; /* Indicate it is too soon for next scrub */
    }
  }

  return result;
}


/*---------------------------------------------------------------------------
 * All below functions are like public APIs, they should acquire data-lock
 * and call any functions above.
---------------------------------------------------------------------------*/
void
flash_scrub_system_timer_signal_received (void)
{
  struct scrub_master_info_type *scrubm;
  uint64 curr_time;

  FLASH_SCRUB_GLOBAL_LOCK ();

  scrubm = &scrub_master_info_inst;

  curr_time = scrub_master_get_current_time (scrubm);
  if (curr_time == 0)
  {
    /* we still do not have system-time, restart the system-timer. */
    scrub_master_start_system_timer (scrubm);
  }
  else
  {
    /* we have system-time now, adjust the scrub-timer. */
    FLASH_SCRUB_MSG_HIGH_2 ("system-time acquired. %d, %d",
                          (uint32)(curr_time >> 32), (uint32)curr_time);

    /* Adjust time since last scrub */
    scrub_master_save_time_in_efs (scrubm);

    scrub_master_start_scrub_timer (scrubm);
  }

  FLASH_SCRUB_GLOBAL_UNLOCK ();
}

static int
flash_scrub_start_scrub_helper (void)
{
  struct scrub_master_info_type *scrubm;
  int result;

  FLASH_SCRUB_GLOBAL_LOCK ();

  scrubm = &scrub_master_info_inst;

  if (scrubm->is_scrub_enabled != 1)
  {
    result = FLASH_SCRUB_DISABLED;
    goto End;
  }

  result = 0;

  flash_scrub_shim_ops_start ();

  scrub_master_do_scrub (scrubm);

  flash_scrub_shim_ops_end ();

End:
  FLASH_SCRUB_GLOBAL_UNLOCK ();
  return result;
}

static void
flash_scrub_save_time_in_efs (void)
{
  struct scrub_master_info_type *scrubm;

  FLASH_SCRUB_GLOBAL_LOCK ();

  scrubm = &scrub_master_info_inst;

  scrub_master_save_time_in_efs (scrubm);

  scrub_master_start_save_time_in_efs_timer (scrubm);

  FLASH_SCRUB_GLOBAL_UNLOCK ();
}


int
flash_scrub_do_a_forced_scrub (void)
{
  int result;
  result = flash_scrub_start_scrub_helper ();
  return result;
}

void
flash_scrub_scrub_timer_signal_received (void)
{
  struct scrub_master_info_type *scrubm;

  FLASH_SCRUB_GLOBAL_LOCK ();

  scrubm = &scrub_master_info_inst;

  (void) rex_set_sigs (scrubm->scrub_task_tcb, FLASH_SCRUB_START_SCRUB_SIGNAL);

  FLASH_SCRUB_GLOBAL_UNLOCK ();
}

void
flash_scrub_start_scrub_signal_received (void)
{
  (void) flash_scrub_start_scrub_helper ();
}

void
flash_scrub_save_time_in_efs_signal_received (void)
{
  flash_scrub_save_time_in_efs ();
}

void
flash_scrub_fota_notification_signal_received (void)
{
  struct scrub_master_info_type *scrubm;

  FLASH_SCRUB_GLOBAL_LOCK ();

  scrubm = &scrub_master_info_inst;

  FLASH_SCRUB_MSG_HIGH_0 ("scrubm : FOTA signal recvd");
  flash_scrub_log_err ("scrubm :  FOTA signal recvd", 0, 0, 0);

  flash_scrub_shim_ops_start ();

  flash_scrub_boot_refresh_md5 (scrubm->boot_handle);
  flash_scrub_code_refresh_md5 ();

  /* Don't save md5_compute result until Scrub has started. If there is a
     reset before state transitions out of IDLE state, we wont be retrying
     scrub if recompute_md5 is saved to 0. */
  scrubm->info_file.recompute_md5_after_fota = 0;
  if (scrubm->info_file.did_last_scrub_fail)
  {
    FLASH_SCRUB_MSG_HIGH_0 ("scrubm : Retry scrub on FOTA");
    flash_scrub_log_err ("scrubm:Retry scrub on FOTA", 0, 0, 0);
    scrub_master_do_scrub (scrubm);
  }

  flash_scrub_shim_ops_end ();

  FLASH_SCRUB_GLOBAL_UNLOCK ();
}


int
flash_scrub_start_scrub_request_helper (uint32 *scrub_token, int is_via_qmi)
{
  struct scrub_master_info_type *scrubm;
  int result;

  if (scrub_token == NULL)
  {
    return FLASH_SCRUB_EPARAM;
  }

  FLASH_SCRUB_GLOBAL_LOCK ();

  scrubm = &scrub_master_info_inst;

  if (is_via_qmi)
  {
    if (!scrubm->config_file.is_qmi_cmds_to_trigger_scrub_enabled)
    {
      result = FLASH_SCRUB_EPERM;
      goto End;
    }
  }

  if (scrubm->is_scrub_enabled != 1)
  {
    result = FLASH_SCRUB_DISABLED;
    goto End;
  }

  if (scrubm->state != SCRUB_MASTER_STATE_IDLE_STATE)
  {
    result = FLASH_SCRUB_EBUSY;
    goto End;
  }

  result = scrub_master_do_time_check_for_scrub (scrubm);
  if (result != 0)
  {
    goto End;
  }

  /* Give out the next scrub-token for this scrub. */
  *scrub_token = scrubm->curr_scrub_seqno + 1;
  scrubm->is_scrub_in_progress = 1;

  FLASH_SCRUB_LOG_MSG_2 ("scrub req recd. seqno = %d, qmi = %d",
                       *scrub_token, is_via_qmi);

  (void) rex_set_sigs (scrubm->scrub_task_tcb, FLASH_SCRUB_START_SCRUB_SIGNAL);

  result = 0;

End:
  FLASH_SCRUB_GLOBAL_UNLOCK ();
  return result;
}

int
flash_scrub_start_scrub_request (uint32 *scrub_token)
{
  return flash_scrub_start_scrub_request_helper (scrub_token, 0);
}

int
flash_scrub_start_scrub_request_from_qmi (uint32 *scrub_token)
{
  return flash_scrub_start_scrub_request_helper (scrub_token, 1);
}

int
flash_scrub_get_scrub_status_helper (uint32 scrub_token,
                                     enum flash_scrub_status *scrub_status,
                                     int is_via_qmi)
{
  struct scrub_master_info_type *scrubm;
  enum flash_scrub_status status;
  int result;

  if (scrub_status == NULL)
  {
    return FLASH_SCRUB_EPARAM;
  }

  if (scrub_token == 0)
  {
    return FLASH_SCRUB_EINVAL;
  }

  FLASH_SCRUB_GLOBAL_LOCK ();

  scrubm = &scrub_master_info_inst;

  status = FLASH_SCRUB_STATUS_UNKNOWN;

  if (is_via_qmi)
  {
    if (!scrubm->config_file.is_qmi_cmds_to_trigger_scrub_enabled)
    {
      result = FLASH_SCRUB_EPERM;
      goto End;
    }
  }

  if (scrubm->is_scrub_enabled != 1)
  {
    result = FLASH_SCRUB_DISABLED;
    goto End;
  }

  result = 0;

  if ((scrubm->is_scrub_in_progress) &&
      (scrub_token == (scrubm->curr_scrub_seqno + 1)))
  {
    *scrub_status = FLASH_SCRUB_STATUS_PENDING;
    goto End;
  }

  if (scrub_token == scrubm->curr_scrub_seqno)
  {
    status = (scrubm->is_scrub_in_progress) ? FLASH_SCRUB_STATUS_PENDING :
                                              FLASH_SCRUB_STATUS_COMPLETE;
  }
  else if (scrub_token < scrubm->curr_scrub_seqno)
  {
    status = FLASH_SCRUB_STATUS_COMPLETE;
  }
  else if (scrub_token > scrubm->curr_scrub_seqno)
  {
    status = FLASH_SCRUB_STATUS_UNKNOWN;
    result = FLASH_SCRUB_EINVAL;
  }

  *scrub_status = status;

End:
  FLASH_SCRUB_GLOBAL_UNLOCK ();
  return result;
}

int
flash_scrub_get_scrub_status (uint32 scrub_token,
                              enum flash_scrub_status *scrub_status)
{
  return flash_scrub_get_scrub_status_helper (scrub_token, scrub_status, 0);

}

int
flash_scrub_get_scrub_status_from_qmi (uint32 scrub_token,
                                       enum flash_scrub_status *scrub_status)
{
  return flash_scrub_get_scrub_status_helper (scrub_token, scrub_status, 1);
}


/*----------------------------------------------------------------------------
 * The number of seconds to add to CDMA time to convert to POSIX(TM) time.
 * CDMA time starts at Jan 6, 1980.  POSIX(TM) time starts at Jan 1, 1970.
 * In between those two dates are two years, 1972 and 1976 which are leap
 * years. To figure out the delta, the equation is as follows:
 *
 * 3657 days = 315964800 seconds = 0x12D53D80
----------------------------------------------------------------------------*/
#define CDMA_TIME_TO_POSIX_TIME_DELTA (0x12D53D80)

int
flash_scrub_get_time_status_helper (uint64 *last_scrub_time_in_secs,
                                   uint64 *min_time_in_secs_between_2_scrubs,
                                   uint64 *automatic_scrub_timer_value_in_secs,
                                   int is_via_qmi)
{
  struct scrub_master_info_type *scrubm;
  int result;

  if ((last_scrub_time_in_secs == NULL) ||
      (min_time_in_secs_between_2_scrubs == NULL) ||
      (automatic_scrub_timer_value_in_secs == NULL))
  {
    return FLASH_SCRUB_EPARAM;
  }

  FLASH_SCRUB_GLOBAL_LOCK ();

  scrubm = &scrub_master_info_inst;

  if (is_via_qmi)
  {
    if (!scrubm->config_file.is_qmi_cmds_to_trigger_scrub_enabled)
    {
      result = FLASH_SCRUB_EPERM;
      goto End;
    }
  }

  if (scrubm->is_scrub_enabled != 1)
  {
    result = FLASH_SCRUB_DISABLED;
    goto End;
  }

  result = 0;

  *last_scrub_time_in_secs = scrubm->info_file.system_time_of_last_scrub;
  if (scrubm->info_file.system_time_of_last_scrub != 0)
  {
    *last_scrub_time_in_secs += CDMA_TIME_TO_POSIX_TIME_DELTA;
  }

  *min_time_in_secs_between_2_scrubs =
        scrubm->config_file.forced_minimum_time_in_secs_between_2_scrubs;

  *automatic_scrub_timer_value_in_secs =
            scrubm->config_file.timer_value_in_secs_for_automatic_scrubs;

End:
  FLASH_SCRUB_GLOBAL_UNLOCK ();
  return result;
}

int
flash_scrub_get_time_status (uint64 *last_scrub_time_in_secs,
                             uint64 *min_time_in_secs_between_2_scrubs,
                             uint64 *automatic_scrub_timer_value_in_secs)
{
  return flash_scrub_get_time_status_helper (last_scrub_time_in_secs,
               min_time_in_secs_between_2_scrubs,
               automatic_scrub_timer_value_in_secs, 0);

}

int
flash_scrub_get_time_status_from_qmi (uint64 *last_scrub_time_in_secs,
                            uint64 *min_time_in_secs_between_2_scrubs,
                            uint64 *automatic_scrub_timer_value_in_secs)
{
  return flash_scrub_get_time_status_helper (last_scrub_time_in_secs,
               min_time_in_secs_between_2_scrubs,
               automatic_scrub_timer_value_in_secs, 1);
}


int
flash_scrub_set_automatic_scrub_timer (
      uint64 automatic_scrub_timer_value_in_secs)
{
  struct scrub_master_info_type *scrubm;
  uint64 old_automatic_scrub_timer_value_in_secs;
  int result;

  FLASH_SCRUB_GLOBAL_LOCK ();

  scrubm = &scrub_master_info_inst;

  if (scrubm->is_scrub_enabled != 1)
  {
    result = FLASH_SCRUB_DISABLED;
    goto End;
  }

  if (scrubm->config_file.forced_minimum_time_in_secs_between_2_scrubs >
      automatic_scrub_timer_value_in_secs)
  {
    FLASH_SCRUB_MSG_HIGH_2 ("Err: Cannot change timer.min=%d, new-auto=%d",
          scrubm->config_file.forced_minimum_time_in_secs_between_2_scrubs,
          automatic_scrub_timer_value_in_secs);
    result = FLASH_SCRUB_EPARAM;
    goto End;
  }

  old_automatic_scrub_timer_value_in_secs =
      scrubm->config_file.timer_value_in_secs_for_automatic_scrubs;

  scrubm->config_file.timer_value_in_secs_for_automatic_scrubs =
                        automatic_scrub_timer_value_in_secs;

  scrub_master_save_config_file (scrubm);

  scrubm->scrub_timer_val_in_ms = automatic_scrub_timer_value_in_secs;
  scrubm->scrub_timer_val_in_ms *= 1000;

  scrub_master_start_scrub_timer (scrubm);

  FLASH_SCRUB_LOG_MSG_3 ("changed auto-timer. old = %d, new = %d, min = %d",
       old_automatic_scrub_timer_value_in_secs,
       scrubm->config_file.timer_value_in_secs_for_automatic_scrubs,
       scrubm->config_file.forced_minimum_time_in_secs_between_2_scrubs);

  result = 0;

End:
  FLASH_SCRUB_GLOBAL_UNLOCK ();
  return result;
}


int
flash_scrub_set_timer_value_for_saving_time_in_efs (
                uint64 timer_value_in_secs_for_saving_time_in_efs)
{
  struct scrub_master_info_type *scrubm;
  uint64 old_timer_value_in_secs_for_saving_time_in_efs;
  int result;

  FLASH_SCRUB_GLOBAL_LOCK ();

  scrubm = &scrub_master_info_inst;

  if (scrubm->is_scrub_enabled != 1)
  {
    result = FLASH_SCRUB_DISABLED;
    goto End;
  }

  result = 0;

  old_timer_value_in_secs_for_saving_time_in_efs =
    scrubm->config_file.timer_val_in_secs_for_saving_time_in_efs;

  scrubm->config_file.timer_val_in_secs_for_saving_time_in_efs =
                        timer_value_in_secs_for_saving_time_in_efs;

  scrub_master_save_config_file (scrubm);

  scrubm->save_time_in_efs_timer_val_in_ms = 1000 *
                              timer_value_in_secs_for_saving_time_in_efs;

  scrub_master_update_timer_val_to_save_time_in_efs (scrubm);

  FLASH_SCRUB_LOG_MSG_2 ("changed efs-timer.old = %d, new = %d",
        old_timer_value_in_secs_for_saving_time_in_efs,
        scrubm->config_file.timer_val_in_secs_for_saving_time_in_efs);

End:
  FLASH_SCRUB_GLOBAL_UNLOCK ();
  return result;
}


int
flash_scrub_set_min_time_between_2_scrubs (
      uint64 min_time_in_secs_between_2_scrubs)
{
  struct scrub_master_info_type *scrubm;
  uint64 old_min_time_in_secs_between_2_scrubs;
  int result;

  FLASH_SCRUB_GLOBAL_LOCK ();

  scrubm = &scrub_master_info_inst;

  if (scrubm->is_scrub_enabled != 1)
  {
    result = FLASH_SCRUB_DISABLED;
    goto End;
  }

  if (min_time_in_secs_between_2_scrubs >
      scrubm->config_file.timer_value_in_secs_for_automatic_scrubs)
  {
    result = FLASH_SCRUB_EPARAM;
    FLASH_SCRUB_MSG_HIGH_2 ("Err: Cannot change mintime.auto=%d, new-min=%d",
          scrubm->config_file.timer_value_in_secs_for_automatic_scrubs,
          min_time_in_secs_between_2_scrubs);
    goto End;
  }

  old_min_time_in_secs_between_2_scrubs =
    scrubm->config_file.forced_minimum_time_in_secs_between_2_scrubs;

  scrubm->config_file.forced_minimum_time_in_secs_between_2_scrubs =
                        min_time_in_secs_between_2_scrubs;

  scrub_master_save_config_file (scrubm);

  scrubm->min_time_between_2scrubs_in_ms = min_time_in_secs_between_2_scrubs;
  scrubm->min_time_between_2scrubs_in_ms *= 1000;

  FLASH_SCRUB_LOG_MSG_2 ("Change MinTime. Old = %d, New =%d",
        old_min_time_in_secs_between_2_scrubs,
        scrubm->config_file.forced_minimum_time_in_secs_between_2_scrubs);

  result = 0;

End:
  FLASH_SCRUB_GLOBAL_UNLOCK ();
  return result;
}


int
flash_scrub_notify_fota_update (void)
{
  struct scrub_master_info_type *scrubm;
  int result;

  FLASH_SCRUB_GLOBAL_LOCK ();

  scrubm = &scrub_master_info_inst;

  FLASH_SCRUB_LOG_MSG_0 ("scrubm : FOTA start");

  if (scrubm->is_scrub_enabled != 1)
  {
    result = FLASH_SCRUB_DISABLED;
    goto End;
  }

  result = 0;

  /* FOTA has happened, so remember in EFS that we have to recompute MD5 on
     next scrub. */
  scrubm->info_file.recompute_md5_after_fota = 1;
  scrub_master_save_info_file (scrubm);

  rex_set_sigs (scrubm->scrub_task_tcb, FLASH_SCRUB_FOTA_NOTIFICATION_SIGNAL);

End:
  FLASH_SCRUB_GLOBAL_UNLOCK ();
  return result;
}

void
flash_scrub_rcinit_shutdown_callback (void)
{
  struct scrub_master_info_type *scrubm;

  FLASH_SCRUB_GLOBAL_LOCK ();

  scrubm = &scrub_master_info_inst;

  FLASH_SCRUB_MSG_HIGH_0 ("scrubm : Rcinit shutdown cb");

  if (scrubm->is_scrub_enabled != 1)
  {
    goto End;
  }

  scrub_master_save_time_in_efs (scrubm);

End:
  FLASH_SCRUB_GLOBAL_UNLOCK ();
}

