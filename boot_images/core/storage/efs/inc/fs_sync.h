/***********************************************************************
 * fs_sync.h
 *
 * EFS Sync module.
 * Copyright (C) 2009-2012 Qualcomm Technologies, Inc.
 *
 * EFS Sync module will do periodic efs_sync() once every configured sync
 * timer.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/inc/fs_sync.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-01-07   wek   Move remotefs write iovec outside efs global lock.
2011-12-04   dks   Initialize all locks before using them.
2010-05-13   wek   Make sync_no_wait non blocking.
2010-03-15   rp    Optimize the efs sync timer.
2009-11-16   rp    Create

===========================================================================*/

#ifndef __FS_SYNC_H__
#define __FS_SYNC_H__

#include "comdef.h"
#include "fs_osal.h"
#include "fs_sys_types.h"

/* Info related with sync module. */
struct fs_sync_data
{
  fs_os_tcb_type   *sync_task;  /* TCB of task doing efs_sync on request */
  fs_os_timer_type  sync_timer;
  uint32            sync_timer_interval;
  fs_sync_token_t   sync_counter; /* monotonically increasing number,
                                     counting the number of syncs. */
  fs_sync_token_t   last_success_token; /* Value of the sync counter when the
                                           last successful sync occured. */
};

/*
 * FUNCTION CALLS IN THIS HEADER FILE THAT DON'T TAKE struct fs_sync_data *
 * ARE CALLED OUTSIDE RMTS PAGE MANGER.
 */


/**
 * Called when the sync-timer has expired.
 *
 */
void fs_sync_timer_signal_received (void);

/**
 * Set the sync timer interval.
 *
 * @param sync_data stucture that holds the sync module data.
 * @param msecs      Time in milliseconds, when sync is needed.
 *
 */
void fs_sync_set_sync_timer_interval (struct fs_sync_data *sync_data,
                                      uint32 msecs);


/**
 * Initialize sync-lock.
 */
void fs_sync_lock_init (void);

/**
 * Acquire the sync-lock.
 */
void fs_sync_lock (void);

/**
 * Release the previously acquired sync-lock.
 */
void fs_sync_unlock (void);



/**
 * Initialize the EFS Sync module.
 */
 void fs_sync_init (struct fs_sync_data *sync_data);

/**
 * Start the sync timer.
 *
 * @param sync_data stucture that holds the sync module data.
 *
 * @return On success 0, when timer not set return -1
 */
int fs_sync_start_timer (struct fs_sync_data *sync_data);

/**
 * Stops the sync timer.
 *
 * @param sync_data stucture that holds the sync module data.
 *
 */
void fs_sync_stop_timer (struct fs_sync_data *sync_data);

/**
 * Prepares sync module to issue a sync.
 *
 * Sync module needs to keep track of when a sync starts and when it ends.
 *
 * @param sync_data stucture that holds the sync module data.
 *
 */
void fs_sync_start (struct fs_sync_data *sync_data);

/**
 * Tells the sync module that sync is done.
 *
 * @param sync_data stucture that holds the sync module data.
 *
 */
void fs_sync_complete (struct fs_sync_data *sync_data);

/**
 * Request to issue an efs_sync asynchronously.
 *
 * @param sync_data stucture that holds the sync module data.
 *
 * @return sync_token return value of the sync the client signs up for.
 */
fs_sync_token_t fs_sync_request_sync (struct fs_sync_data *sync_data);

/**
 * Gets the last token marked as complete. Used to give a valid completed
 * token when there is nothing to sync.
 *
 * @param sync_data stucture that holds the sync module data.
 *
 * @return Last token marked as complete.
 */
fs_sync_token_t
fs_sync_get_completed_token (struct fs_sync_data *sync_data);

/**
 * Get what is the state of the sync_token.
 *
 * @param sync_data stucture that holds the sync module data.
 * @param sync_token Token to find the state of sync.
 *
 * @return Sync status.
 */
enum fs_sync_result fs_sync_get_status (struct fs_sync_data *sync_data,
                                        fs_sync_token_t sync_token);

#endif /* __FS_SYNC_H__ */
