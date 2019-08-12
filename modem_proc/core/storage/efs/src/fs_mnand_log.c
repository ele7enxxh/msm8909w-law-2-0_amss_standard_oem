/***********************************************************************
 * fs_mnand_log.c
 *
 * Log module for mnand.
 * Copyright (C) 2008,2010,2012-2013 QUALCOMM Technologies, Inc.
 *
 * This file maintains the log-region for the mnand. The PM will keep adding
 * log-entries to a log-page during a transaction and finally flush the
 * log-page at the end of the transaction.
 *
 * This file also keeps track of the written-logs and free-logs in a log-Q.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_mnand_log.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-10-09   nr    Replace banned memory functions with safer versions.
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2010-11-08   wek   Remove impractical and unused paranoia module.
2008-12-12   rp    Create

===========================================================================*/
#include "fs_device.h"

#if defined (FEATURE_EFS_EFS2_ON_MNAND) &&    \
    defined (FEATURE_EFS_QEFS2)

#include "fs_mnand_log.h"
#include "fs_mnand_config.h"
#include "assert.h"
#include "crc.h"
#include "fs_mnand_pm.h"
#include "fs_err.h"
#include "fs_util.h"
#include "fs_protected_memory.h"
#include "fs_babylon.h"


/** How big can be one log-page: [4:Seqno + 1:log-end-marker + 4:crc] */
#define FS_MNAND_LOG_MAX_TRANS_BYTES   (FS_MNAND_LOG_PAGE_SIZE - (4+1+4))

/** How big can be one log-entry: [1:log-code + 4:uint32-args] */
#define FS_MNAND_LOG_MAX_ENTRY_SIZE (1+(4*4))


/** Increment Log-Queue variable handling the wrapping. */
#define FS_MNAND_LOG_Q_INCR(x)              \
  do                                        \
  {                                         \
    if ((++x) >= FS_MNAND_LOG_QUEUE_SIZE)   \
      ((x) = 0);                            \
  } while (0)


/** Advance a Log-Page value with proper wrapping. */
#define FS_MNAND_LOG_ADV_PAGE(log, pg)          \
  do                                            \
  {                                             \
    (pg) += FS_MNAND_LOG_SECTORS_PER_PAGE ;     \
    if ((pg) >= (log)->super->data->log.end)    \
      (pg) = (log)->super->data->log.start;     \
  } while (0)



/* One log-page instance. */
static FS_ALIGN struct fs_mnand_log_page mnand_log_page;


static void fs_mnand_log_flush (fs_mnand_log_t log);

void
fs_mnand_log_init (fs_mnand_log_t log, efs_device_t dev,
                   fs_mnand_super_t super, int fresh_start)
{
  log->dev = dev;
  log->super = super;

  log->log_page = &mnand_log_page;
  ASSERT (FS_OKAY_FOR_DM (log->log_page));
  ASSERT (sizeof (mnand_log_page) == FS_MNAND_LOG_PAGE_SIZE);

  if (fresh_start)
  {
    /* Fresh start, so fill the log-region. */
    super->data->log_head = super->data->log.start;
    fs_mnand_region_fill (dev, super->data->log.start,
                          super->data->log.end, FS_FOP_LOG);
  }

  /* Initialize the Log-Q */
  memset (log->queue, 0, sizeof(log->queue));
  log->free = log->flush = log->alloc = 0;

  /* Initialize the current log-buffer. */
  memset (log->log_page, 0xFF, FS_MNAND_LOG_PAGE_SIZE);
  log->buff_idx = log->start_txn_idx = log->inside_txn = 0;
  log->seqno = 1;

  /* Next log-page to write is wherever super-block is telling. */
  log->write_page = super->data->log_head;
}


/** Get how many free log-pages are left in the queue. */
static uint32
fs_mnand_log_free_cnt (fs_mnand_log_t log)
{
  uint32 free_cnt;

  /* Get the distance between [Flush] and [Alloc] */
  if (log->alloc >= log->flush)
    free_cnt = log->alloc - log->flush;
  else
    free_cnt = (FS_MNAND_LOG_QUEUE_SIZE - log->flush) + log->alloc;

  return free_cnt;
}

uint32
fs_mnand_log_written_cnt (fs_mnand_log_t log)
{
  uint32 written_cnt;

  /* Get the distance between [Free] and [Flush] */
  if (log->flush >= log->free)
    written_cnt = log->flush - log->free;
  else
    written_cnt = (FS_MNAND_LOG_QUEUE_SIZE - log->free) + log->flush;

  return written_cnt;
}

fs_sector_id
fs_mnand_log_peek_free (fs_mnand_log_t log)
{
  return log->queue[log->free];
}

fs_sector_id
fs_mnand_log_peek_flush (fs_mnand_log_t log)
{
  return log->queue[log->flush];
}

fs_sector_id
fs_mnand_log_get_written (fs_mnand_log_t log)
{
  fs_sector_id log_page;
  log_page = log->queue[log->free];
  FS_MNAND_LOG_Q_INCR (log->free);
  return log_page;
}

static void
fs_mnand_log_add_written_page (fs_mnand_log_t log, fs_sector_id log_pg)
{
  log->flush = log->alloc;
  log->queue[log->flush] = log_pg;
  FS_MNAND_LOG_Q_INCR (log->alloc);
}

void
fs_mnand_log_start_transaction (fs_mnand_log_t log)
{
  /* No nested transactions please */
  ASSERT (log->inside_txn == 0);

  log->inside_txn = 1;
  log->start_txn_idx = log->buff_idx;
}

void
fs_mnand_log_end_transaction (fs_mnand_log_t log)
{
  /* confirm we are inside a trans */
  ASSERT (log->inside_txn == 1);

  fs_mnand_log_flush (log);
  log->inside_txn = 0;
}

/** Moves the entire current transaction to the start of the buffer. */
static void
fs_mnand_start_txn_in_new_buffer (fs_mnand_log_t log)
{
  uint32 curr_txn_size;
  uint8 *log_buff;

  ASSERT (log->start_txn_idx != 0);
  ASSERT (log->start_txn_idx <= log->buff_idx);

  curr_txn_size = log->buff_idx - log->start_txn_idx;
  ASSERT (curr_txn_size <= FS_MNAND_LOG_MAX_TRANS_BYTES);

  log_buff = &log->log_page->buff[0];
  fs_memsmove (log_buff, curr_txn_size, (log_buff + log->start_txn_idx),
              curr_txn_size);
  memset ((log_buff + curr_txn_size), 0,
          (FS_MNAND_LOG_MAX_TRANS_BYTES-curr_txn_size));

  log->buff_idx = curr_txn_size;
  log->start_txn_idx = 0;

  ++log->seqno;
  if (log->seqno == 0)
    log->seqno = 1;

  FS_MNAND_LOG_Q_INCR (log->flush);
  if (log->flush == log->alloc)
    FS_ERR_FATAL("Ran out of log-pages...",0,0,0);
}

void
fs_mnand_log_add (fs_mnand_log_t log, uint8 log_code,
                     uint32 arg1, uint32 arg2, uint32 arg3, uint32 arg4)
{
  uint8 arg_cnt, i;
  uint32 args[4];

  ASSERT (log->buff_idx <= FS_MNAND_LOG_MAX_TRANS_BYTES);

  if ( (log->buff_idx + FS_MNAND_LOG_MAX_ENTRY_SIZE) >
        FS_MNAND_LOG_MAX_TRANS_BYTES )
  {
    if (log->inside_txn)
    {
      /* We are inside a transaction and the current log-page is not
         sufficient to store the current transaction entirely, so we
         should move the entire current transaction at the beginning
         of the next log-page. */
      fs_mnand_start_txn_in_new_buffer (log);
    }
    else
    {
      /* we are replaying logs, and these log-entries can span many
         log-pages. */
      fs_mnand_log_flush (log);
      memset (log->log_page, 0, FS_MNAND_LOG_PAGE_SIZE);
      log->buff_idx = 0;
      log->start_txn_idx = 0;

      ++log->seqno;
      if (log->seqno == 0)
        log->seqno = 1;

      FS_MNAND_LOG_Q_INCR (log->flush);
      if (log->flush == log->alloc)
        FS_ERR_FATAL("Ran out of log-pages...",0,0,0);
    }
  }

  log->log_page->buff[log->buff_idx++] = (uint8 )log_code;

  args[0] = arg1; args[1] = arg2; args[2] = arg3; args[3] = arg4;
  arg_cnt = FS_MNAND_LOG_CODE_ARGC (log_code);
  for (i=0; i<arg_cnt; ++i)
  {
    fs_memscpy (&log->log_page->buff[log->buff_idx], 4, &args[i], 4);
    log->buff_idx += 4;
  }
}

/** Write the current transaction buffer onto the log-page.  */
static void
fs_mnand_log_flush (fs_mnand_log_t log)
{
  fs_sector_id log_page;
  uint32 crc;

  ASSERT (log->buff_idx <= FS_MNAND_LOG_MAX_TRANS_BYTES);

  /* Write to the page where [flush] is pointing to. */
  log_page = log->queue[log->flush];

  log->log_page->buff[log->buff_idx] = FS_MNAND_LOG_CODE_END_MARKER;
  log->log_page->seqno = log->seqno;

  crc = crc_30_calc ((byte *)log->log_page, ((FS_MNAND_LOG_PAGE_SIZE-4)*8));
  log->log_page->crc = crc;
  fs_device_reliable_write_sectors (log->dev, log_page, log->log_page,
                                   FS_MNAND_LOG_SECTORS_PER_PAGE, FS_FOP_LOG);
}

/** Allocate a single log-page and adds it to log-queue. */
static void
fs_mnand_log_alloc (fs_mnand_log_t log)
{
  log->queue[log->alloc] = log->write_page;
  FS_MNAND_LOG_Q_INCR (log->alloc);

  FS_MNAND_LOG_ADV_PAGE (log, log->write_page);
}

void
fs_mnand_allocate_logs (fs_mnand_log_t log)
{
  while (fs_mnand_log_free_cnt (log) < FS_MNAND_LOG_ALLOCATED_LOGS)
  {
    fs_mnand_log_alloc (log);
  }
}

/** Returns 1 if the computed CRC of a log-page matches with the stored
  * CRC in that log-page, 0 if they don't match. */
static int
fs_mnand_log_is_crc_ok (struct fs_mnand_log_page *log_page)
{
  uint32 crc;

  crc = crc_30_calc ((byte *)log_page, ((FS_MNAND_LOG_PAGE_SIZE-4)*8));

  if (crc == log_page->crc)
    return 1; /* CRC matched. */

  return 0;
}

/** Returns 1 if the log-page is in sequence with the prev_seqno, otherwise 0*/
static int
fs_mnand_log_is_seq_ok (struct fs_mnand_log_page *log_page, uint32 prev_seqno)
{
  uint32 next_seqno;

  /* compute the next seqno. */
  next_seqno = prev_seqno;
  ++next_seqno;

  if (log_page->seqno == next_seqno)
    return 1; /* seqno is in sequence. */

  return 0;
}

uint32
fs_mnand_log_replay_logs (fs_mnand_log_t log, void *pm_data)
{
  fs_sector_id log_pg, prev_log_pg;
  uint8 log_code;
  uint32 args_cnt, args[4], i, idx = 0;
  int inside_xact, skip_first_seq_chk = 1;
  uint32 prev_seqno;
  uint32 replayed_log_pg_cnt = 0;

  EFS_BAB_START (EFS_BAB_MNAND_PM_REPLAY_LOGS);

  /* We will start replaying the logs from the log-head recored
   * in the super-block. */
  prev_seqno = 0;
  log_pg = prev_log_pg = log->super->data->log_head;

  do
  {
    fs_device_read_sectors (log->dev, log_pg, log->log_page,
                            FS_MNAND_LOG_SECTORS_PER_PAGE, FS_FOP_LOG);

    if (!fs_mnand_log_is_crc_ok (log->log_page))
      break;

    if (skip_first_seq_chk == 0)
      if (!fs_mnand_log_is_seq_ok (log->log_page, prev_seqno))
        break;

    ++replayed_log_pg_cnt;

    /* we crossed the first log-page seqno check, from now on check seqno. */
    skip_first_seq_chk = 0;
    prev_seqno = log->log_page->seqno;

    /* Add this log-page to the loq-queue. */
    fs_mnand_log_add_written_page (log, log_pg);

    inside_xact = 0;

    for (idx = 0; idx <= FS_MNAND_LOG_MAX_TRANS_BYTES; )
    {
      log_code = log->log_page->buff[idx++];
      switch (log_code)
      {
        case FS_MNAND_LOG_CODE_XACT_START:
          ASSERT (inside_xact == 0);
          inside_xact = 1;
          break;

        case FS_MNAND_LOG_CODE_XACT_END:
          ASSERT (inside_xact == 1);
          inside_xact = 0;
          break;

        case FS_MNAND_LOG_CODE_END_MARKER:
          ASSERT (inside_xact == 0);
          break;
      }

      if (log_code == FS_MNAND_LOG_CODE_END_MARKER)
        break;

      args[0] = args[1] = args[2] = args[3] = 0;
      args_cnt = FS_MNAND_LOG_CODE_ARGC (log_code);
      for (i=0; i<args_cnt; ++i)
      {
        fs_memscpy (&args[i], sizeof (args[i]),
                   (uint32*)&log->log_page->buff[idx], 4);
        idx += 4;
      }

      fs_mnand_pm_replay_log_entry (pm_data, log_code, args_cnt, args);
    }

    ASSERT (log_code == FS_MNAND_LOG_CODE_END_MARKER);

    prev_log_pg = log_pg;
    FS_MNAND_LOG_ADV_PAGE (log, log_pg);

  }while (log_pg != log->super->data->log_head);

  if (skip_first_seq_chk == 0)
  {
    /* We replayed atleast one log-page. */
    ASSERT (idx >= 1);
    fs_device_read_sectors (log->dev, prev_log_pg, log->log_page,
                            FS_MNAND_LOG_SECTORS_PER_PAGE, FS_FOP_LOG);
    log->buff_idx = idx-1;
    log->write_page = log_pg;
    log->seqno = prev_seqno;
  }
  else
  {
    /* No log-pages were replayed, so start fresh. */
    memset (log->log_page, 0xFF, FS_MNAND_LOG_PAGE_SIZE);
    log->write_page = log->super->data->log.start;
  }

  EFS_BAB_END (EFS_BAB_MNAND_PM_REPLAY_LOGS);

  return replayed_log_pg_cnt;
}

#endif /* FEATURE_EFS_EFS2_ON_MNAND */

