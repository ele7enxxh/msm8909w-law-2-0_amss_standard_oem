/***********************************************************************
 * fs_sync.c
 *
 * EFS Sync module.
 * Copyright (C) 2009-2014 QUALCOMM Technologies, Inc.
 *
 * EFS Sync module will do periodic efs_sync() once every configured sync
 * timer.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_sync.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-04-26   vm    Use correct FS_MSG macros to save RAM.
2013-11-01   wek   Allow syncs after creation of golden copy.
2013-05-02   dks   Fix initial value of last_success_token to sync_counter.
2013-01-11   nr    Allow toleration of timeout during an efs_sync.
2012-06-12   dks   Make sync lock be initialized once in efs_initialize only.
2012-01-07   wek   Move remotefs write iovec outside efs global lock.
2011-12-04   dks   Initialize all locks before using them.
2010-05-13   wek   Make sync_no_wait non blocking.
2010-05-10   rp    Removed inclusion of EFS1 deprecated files.
2010-04-28   vr    Relocate fs_task from EFS1 to EFS2
2010-03-15   rp    Optimize the efs sync timer.
2010-03-05   vr    Add support for QEFS2 in BOOT
2009-11-16   rp    Create

===========================================================================*/

#include "fs_sync.h"
#include "fs_public.h"
#include "fs_task_i.h"
#include "task.h"
#include "assert.h"
#include "fs_err.h"
#include "fs_errno.h"

/* Sync Lock, protects writing to the shared RAM. */
static fs_os_crit_sect_type   fs_sync_cs;

/* Prototypes. */
static void fs_sync_timer_init (struct fs_sync_data *sync_data);

void fs_sync_init (struct fs_sync_data *sync_data)
{
  sync_data->sync_task = fs_os_self ();
  /* our next sync will be the first */
  sync_data->sync_counter = 1;
  sync_data->last_success_token = 1;

  fs_sync_timer_init (sync_data);

}

void
fs_sync_lock_init (void)
{
  fs_os_init_crit_sect (&fs_sync_cs);
}

void
fs_sync_lock (void)
{
  fs_os_enter_crit_sect (&fs_sync_cs);
}

void
fs_sync_unlock (void)
{
  fs_os_leave_crit_sect (&fs_sync_cs);
}


static void
fs_sync_timer_init (struct fs_sync_data *sync_data)
{
  fs_os_tcb_type *sync_tcb = sync_data->sync_task;

  sync_data->sync_timer_interval = 0;
  fs_os_def_timer (&sync_data->sync_timer, sync_tcb, FS_EFS_SYNC_SIG);
}


int
fs_sync_start_timer (struct fs_sync_data *sync_data)
{
  uint32 msecs = sync_data->sync_timer_interval;
  int result = -1;

  if (msecs != 0)
  {
    (void) fs_os_clr_timer (&sync_data->sync_timer);
    (void) fs_os_set_timer (&sync_data->sync_timer, msecs);
    result = 0;
  }

  return result;
}

void
fs_sync_stop_timer (struct fs_sync_data *sync_data)
{
  (void) fs_os_clr_timer (&sync_data->sync_timer);
}

static void
fs_sync_signal_task (struct fs_sync_data *sync_data)
{
  /* if timer is running stop it since we are going to signal. */
  fs_sync_stop_timer (sync_data);

  /* Signal task to do sync */
  (void) fs_os_set_sigs (sync_data->sync_task, FS_EFS_SYNC_SIG);
}

void
fs_sync_set_sync_timer_interval (struct fs_sync_data *sync_data,
                                 uint32 msecs)
{
  sync_data->sync_timer_interval = msecs;

  if (msecs == 0)
  {
    /* 0 passed, disable sync timer. */
    fs_sync_stop_timer (sync_data);
  }
}

void
fs_sync_timer_signal_received (void)
{
  int result, error;
  /* The only error codes accepted are ENODEV or ETIMEDOUT*/
  result = efs_sync ("/");
  if (result != 0)
  {
    error = efs_errno;
    FS_MSG_HIGH_1 ("%d error for implicit efs_sync", error);

    ASSERT ((efs_errno == ENODEV) || (efs_errno == ETIMEDOUT) ||
            (efs_errno == EPERM));
  }
}

void
fs_sync_start (struct fs_sync_data *sync_data)
{
  /* Increment sync counter to mark this sync request. */
  sync_data->sync_counter++;
}

void
fs_sync_complete (struct fs_sync_data *sync_data)
{
  /* Successful sync occurred, update the last success token. */
  sync_data->last_success_token = sync_data->sync_counter;
}

fs_sync_token_t
fs_sync_request_sync (struct fs_sync_data *sync_data)
{
  fs_sync_token_t sync_token;
  /* Return the next token */
  sync_token = sync_data->sync_counter + 1;

  fs_sync_signal_task (sync_data);

  return sync_token;
}

fs_sync_token_t
fs_sync_get_completed_token (struct fs_sync_data *sync_data)
{
  return sync_data->sync_counter;
}

enum fs_sync_result
fs_sync_get_status (struct fs_sync_data *sync_data,
                    fs_sync_token_t sync_token)
{
  enum fs_sync_result result;
  int diff;

  /* use the difference between the tokens to account for wrap around. */
  diff = (int) (sync_data->last_success_token - sync_token);
  if (diff >= 0)
    result = FS_SYNC_COMPLETE;
  else
    result = FS_SYNC_PENDING;

  return result;
}
