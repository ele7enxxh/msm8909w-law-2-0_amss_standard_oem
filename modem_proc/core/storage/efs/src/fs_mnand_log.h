/***********************************************************************
 * fs_mnand_log.h
 *
 * Log module for mnand.
 * Copyright (C) 2008, 2012 QUALCOMM Technologies, Inc.
 *
 * This file maintains the log-region for the mnand. The PM will keep adding
 * log-entries to a log-pagevduring a transaction and finally flush the
 * log-page at the end of the transaction.
 * This file also keeps track of the written-logs and free-logs in a log-Q.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_mnand_log.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2008-12-12   rp    Create

===========================================================================*/

#ifndef __FS_MNAND_LOG_H__
#define __FS_MNAND_LOG_H__

#include "fs_device.h"

#ifdef FEATURE_EFS_EFS2_ON_MNAND

#include "fs_mnand_config.h"
#include "fs_sys_types.h"
#include "fs_mnand_super.h"
#include "fs_pm.h"

/* Make 8-bits Log-Code: => [ 2-bits (arg cnt) + 6-bits (arg-code) ] */
#define FS_MNAND_LOG_CODE_MAKE(opcode, argc)    (((argc) << 6) | (opcode))
#define FS_MNAND_LOG_CODE_ARGC(code)            ((code) >> 6)

/* Log-Codes. */
#define FS_MNAND_LOG_CODE_END_MARKER         FS_MNAND_LOG_CODE_MAKE(0xfd,0)
#define FS_MNAND_LOG_CODE_XACT_START         FS_MNAND_LOG_CODE_MAKE(0x1,0)
#define FS_MNAND_LOG_CODE_XACT_END           FS_MNAND_LOG_CODE_MAKE(0x2,0)
#define FS_MNAND_LOG_CODE_CLUSTER_MOVE       FS_MNAND_LOG_CODE_MAKE(0x3,3)
#define FS_MNAND_LOG_CODE_UPPER_DATA         FS_MNAND_LOG_CODE_MAKE(0x4,2)
#define FS_MNAND_LOG_CODE_LOG_FLUSH          FS_MNAND_LOG_CODE_MAKE(0x5,1)
#define FS_MNAND_LOG_CODE_KEYRANGE_WALK      FS_MNAND_LOG_CODE_MAKE(0x6,3)

/* This is how one single log-page looks. */
struct fs_mnand_log_page
{
  uint32      seqno;
  uint8       buff[FS_MNAND_LOG_PAGE_SIZE - (2*4)];
  uint32      crc;
};

/* Main structure of the log-module. */
struct fs_mnand_log
{
  efs_device_t                  dev;
  fs_mnand_super_t              super;

  /** Parameters for the Log-queue:
   * [alloc] is the next spot to add a new log-page.
   * [free] is the oldes log-page still needed.
   * [flush] is the current log-page to which we are writing to.
   * [free -> flush] All written log-pages that are still needed.
   * [flush -> Alloc] All new log-pages available for writing.
   *
   * free       flush             alloc
   *  |           |                 |
   *  V           V                 V
   *  +---+ +---+ +---+ +---+ +---+ +---+
   *  | 1 | | 2 | | 3 | | 4 | | 5 | | 6 |
   *  +---+ +---+ +---+ +---+ +---+ +---+
   *
   *  In this case, node 6 is the next page to be inserted by
   *  fs_mnand_allocate_logs.  Node 3 is the next page to be written out by
   *  fs_mnand_log_add.  Node 1 is the next page to be given by
   *  fs_mnand_log_get_written.
   */
  fs_sector_id                  queue[FS_MNAND_LOG_QUEUE_SIZE];
  uint32                        free;
  uint32                        flush;
  uint32                        alloc;

  struct fs_mnand_log_page      *log_page; /**< current log-buffer. */
  uint32                        buff_idx;  /**< index to add next log entry */
  uint32                        start_txn_idx; /**< start index of curr trans*/
  int                           inside_txn; /**< Are we inside a transaction?*/

  fs_sector_id                  write_page; /**< next log-page to write. */
  uint32                        seqno; /**< Log-page sequence number. */
};
typedef struct fs_mnand_log* fs_mnand_log_t;



/*************************************************************************//**
 * Initialize the Log module.
 *
 * @param log
 *    The Log structure.
 *
 * @param dev
 *    The dev pointer to the mnand device.
 *
 * @param super
 *    Superblock structure.
 *
 * @param fresh_start
 *    On a fresh_start, this function will write to all the BirdView pages
 * to initialze all the Ptbl status bits to PAGE_STATUS_NOT_WRITTEN.
 *
*****************************************************************************/
void fs_mnand_log_init (fs_mnand_log_t log, efs_device_t dev,
                        fs_mnand_super_t super, int fresh_start);

/*************************************************************************//**
 * Starts a new transaction in the LOG. The LOG module will try to keep many
 * transactions in one log-page, and when the current transaction doesn't fit
 * in the current log-pages left over space, the entire current transaction
 * will be moved to the next log-page.
 *
 * Note that the transactions cannot be nested.
 *
 * @param log
 *    The Log structure.
 *
*****************************************************************************/
void fs_mnand_log_start_transaction (fs_mnand_log_t log);

/*************************************************************************//**
 * Ends a previously started transaction.
 *
 * @param log
 *    The Log structure.
 *
*****************************************************************************/
void fs_mnand_log_end_transaction (fs_mnand_log_t log);

/*************************************************************************//**
 * Add a singly log-entry to the current transaction log-buffer. In the event
 * that this log-entry will not fit in the current buffer's left over space
 * then the entire current transaction will be moved to the next log-page.
 *
 * @param log
 *    The Log structure.
 *
*****************************************************************************/
void fs_mnand_log_add (fs_mnand_log_t log, uint8 log_code,
                          uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4);

/*************************************************************************//**
 * Get how many written log-pages are present in the queue.
 *
 * @param log
 *    The Log structure.
 *
 * @return
 *    Count of how many written log-pages present in the queue.
 *
*****************************************************************************/
uint32 fs_mnand_log_written_cnt (fs_mnand_log_t log);

/*************************************************************************//**
 * Allocate as many free log-pages as configured. Called from PM at the
 * beginning of a transaction.
 *
 * @param log
 *    The Log structure.
 *
*****************************************************************************/
void fs_mnand_allocate_logs (fs_mnand_log_t log);

/*************************************************************************//**
 * The PM calls this function to replay all the written log-pages from the
 * log-head until the log's sequence number are in increasing sequence and
 * the CRC matches.
 *
 * This function will iterate through all log-entries and will call the PM's
 * fs_mnand_pm_replay_log_entry() function to replay a singly log-entry.
 *
 * After all the log-entried are replayed at the end of this function, the
 * log-queue[] and the journal contents should match exactly how it was in
 * the previous power-cycle session.
 *
 * @param log
 *    The Log structure.
 *
 * @param pm_data
 *    The data to pass to be passed to PM while making a PM function call
 * to replay the log-entry.
 *
 * @return
 *    Total number of log-pages replayed this time.
 *
*****************************************************************************/
uint32 fs_mnand_log_replay_logs (fs_mnand_log_t log, void *pm_data);

/*************************************************************************//**
 * Peek what log-page is at [flush] offset in the log-queue.
 * The PTbl module calls this function to use this log-page as the age of the
 * journal entries.
 *
 * The current transaction will propably be written in the [flush] log-page,
 * so all the new page assignments happening for the current transactions
 * are recorded in the journal against the current [flush] log-page.
 *
 * @param log
 *    The Log structure.
 *
 * @return
 *    The log-page at the [flush] offset in the log-queue.
 *
*****************************************************************************/
fs_sector_id fs_mnand_log_peek_flush (fs_mnand_log_t log);

/*************************************************************************//**
 * Peek what log-page is at [free] offset in the log-queue.
 * The PM module calls this function to update the log-head in the super-block.
 *
 * @param log
 *    The Log structure.
 *
 * @return
 *    The log-page at the [free] offset in the log-queue.
 *
*****************************************************************************/
fs_sector_id fs_mnand_log_peek_free (fs_mnand_log_t log);

/*************************************************************************//**
 * Returned what log-page is at [free] offset in the log-queue and also
 * removes it from the queue.
 * The PM module calls this function to free this log-page when we attain the
 * maximum configured written-log pages limit.
 *
 * The log-page at the [free] offset is the oldest written log-page that is
 * still needed, When we have to make space in the log-region then this
 * log-page must be the one to replayed and removed.
 *
 * @param log
 *    The Log structure.
 *
 * @return
 *    The log-page at the [free] offset in the log-queue.
 *
*****************************************************************************/
fs_sector_id fs_mnand_log_get_written (fs_mnand_log_t log);



#endif /* FEATURE_EFS_EFS2_ON_MNAND */

#endif /* not __FS_MNAND_LOG_H__ */

