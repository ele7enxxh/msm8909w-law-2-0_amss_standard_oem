/**********************************************************************
 * fs_pm_log.h
 *
 * File System Log.
 * Copyright (C) 2002-2010, 2012 Qualcomm Technologies, Inc.
 *
 * The Filesystem Log records changes made to the filesystem tables that
 * haven't necessarily been committed to the in-flash structures.  This
 * allows us to delay updates to these structures, with the assumption that
 * this delay will allow groups of these updates to be made together.
 *
 * This log is also read upon powerup and used to augment the in-flash
 * structures.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_pm_log.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2012-03-13    wek     Remove redundant header file inclusion fs_efs2_cust.
2010-11-12    wek     Make nand/nor PM independent of page size macro.
2010-08-30    wek     Handle duplicated log pages.
2010-05-10    dks     Added SPI_NOR device type.
2009-04-30    yg      Add variable to track replay log sequence.
2009-04-22    rp      Avoid marking all blocks as bad in the log region.
2008-11-03    rp      Introduce device abstraction.
2008-08-12    yg      Added new function fs_log_flush_if_ovrflows
2008-05-08    rp      Handle write-failure in log-region.
2008-01-04    yg      Reduced the log queue size to previous value (~1 blk).
2007-10-18    rp      Featurized QEFS2.
2007-09-13    sch     Increased the size of log queue
2007-08-31    yg      Increase the log queue size because of 4k page devices.
2007-08-10    rp      Align buffers passed to flash on a 32-byte boundary.
2006-08-17    dlb     Adjust API slightly to allow for log upgrades.
2005-12-14    dlb     Export log checking.
2005-11-04    dlb     Keep track of last written log.
2005-04-26    dlb     Add 2K page support.
2004-10-15    dlb     Update copyright header.
2003-05-07    dlb     Add log grouping.
2003-03-07    dlb     Increase the queue size.
2003-02-13    dlb     Replay logs for NAND operations.
2002-08-08    drh     Created by dlb.  Added history header.

===========================================================================*/


#ifndef __FS_NOR_LOG_H__
#define __FS_NOR_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "fs_device.h"

#ifdef FEATURE_EFS_QEFS2
#if defined (FEATURE_EFS_EFS2_ON_NAND)      ||   \
    defined (FEATURE_EFS_EFS2_ON_NOR)       ||   \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)

#include "comdef.h"
#include "fs_pm_super.h"

struct fs_log_data;
typedef struct fs_log_data *fs_log_t;
typedef void (*fs_log_page_write_cb_t) (void *priv_data, uint8 *log_buffer);

/* The LOG is not specific about the kinds of data that are stored in the
 * LOG, that is the responsibility of the user of this module.
 *
 * However, we do encode in the log-codes, a few bits that indicates how
 * many arguments this particular code has.  All arguments are 32 bit
 * unsigned values.  The log-codes themselves can be constructed out of
 * these macros.
 *
 * Note that we support 0 to 3 arguments. */

typedef uint8   fs_log_code_t;

#define FS_LOG_CODE_MAKE(opcode, argc)   (((argc) << 6) | (opcode))
#define FS_LOG_CODE_ARGC(code)   ((code) >> 6)

/* We reserve a single special code for our use. */
#define LOG_CODE_GROUP_END      0xFE


/* How many consecutive writes can fail in the log-region? */
#define FS_MAX_CONSECUTIVE_LOG_WRITE_FAIL     4

/* Initialize the log structure.  Will use the given device to write out
 * log pages as needed. */
void fs_log_init (fs_log_t log, efs_device_t dev,
                  fs_log_page_write_cb_t log_page_write_cb, void *priv_data);

/* Set the sequence number used for the last page.  The sequence will be
 * incremented before writing out. */
void fs_log_set_sequence (fs_log_t log, uint32 seqno);

/* Return the number of pages queued up for potential logs to be written
 * to. */
int fs_log_page_count (fs_log_t log);

/* Add a page to the log code's queue of pages available. */
void fs_log_add_page (fs_log_t log, fs_page_id page);

/* Add a page to the queue, but act as if it has already been written.  It
 * is an error to call this after calling fs_log_add_page. */
void fs_log_add_written_page (fs_log_t log, fs_page_id page);

/* Return the next page that will be written. */
fs_page_id fs_log_peek_page (fs_log_t log);

/* Return the next page that is needed for logging. */
fs_page_id fs_log_peek_written (fs_log_t log);

/* Append a single log entry.  May cause an additional page to be used.
 * Only the number of arguments appropriate for the code will be used. */
void fs_log_add (fs_log_t log,
    fs_log_code_t       code,
    uint32      *args);

/* Check if we have enough space left in the buffer to write the count
 * logs of the specified log type. If not much space is available then,
 * flush the log buffer to make space and next log writes will go into a
 * new log page. */
void fs_log_flush_if_overflows (fs_log_t log, fs_log_code_t a, uint32 count);

/* Ensure that it is possible to write the two named log codes to the same
 * page of the log.  This effectively assures that they are grouped and
 * treated atomically.  This may cause a flush if there isn't room for both
 * entries in the current page. */
void fs_log_group (fs_log_t log, fs_log_code_t a, fs_log_code_t b);

/* Flush the log to flash.  For some devices (NAND) this will always write
 * an entire page.  Other devices (NOR) may be able to write a page, and
 * not use up one of our allocated pages.  Returns the number of log pages
 * that were consumed by this flush.*/
void fs_log_flush (fs_log_t log);

/* Get the next available log page that has been fully flushed.  Returns
 * FS_INVALID_PAGE_ID if there are not pages that have been flushed but not
 * returned by this function. */
fs_page_id fs_log_get_written (fs_log_t log);

/* Return a count of the number of log pages that have been flushed fully.
 */
int fs_log_written_count (fs_log_t log);

/* Given a pointer to a particular log page, return true if it represents a
 * valid log page.  'dev' must be the device the page resides on (used to
 * determine the type of device).  If 'upgrade_page' is set to a
 * non-FS_INVALID_PAGE_ID value, the page will be upgraded in place, if
 * possible and necessary. */
int fs_log_is_valid (const void *data, efs_device_t dev,
                     fs_page_id upgrade_page);

/* Traverse through all the pages between [flush] and [alloc] and remove all
 * the pages that match the given bad_blk.
 */
void fs_log_remove_bad_pages (fs_log_t log, uint32 blk_shift,
                              fs_block_id bad_blk);

/***********************************************************************
 * Log page iteration.
 */

#define FS_LOG_ITER_GOOD        0
#define FS_LOG_ITER_ERASED      1
#define FS_LOG_ITER_CORRUPT     2
#define FS_LOG_ITER_DUPLICATED  3

/* Read in a specified log page in preparation to iterate through it.
 * Returns various status FS_LOG_ITER_*, defined above.  If non-null, the
 * [header[0]] is set to the 32 bit marker at the head of the page.
 * [header[1]] is set to the following 32 bit marker. */
int fs_log_iter_set_page (fs_log_t log, fs_page_id p, uint32 *header);

/* Peek the current iteration page. */
fs_page_id fs_log_iter_peek_page (fs_log_t log);

/* Once the iterator is setup, step to the next field.  Code will be set to
 * the appropriate LOG code, or LOG_CODE_GROUP_END to indicate that we are
 * at the end of this log page. */
void fs_log_iter_step (fs_log_t log, fs_log_code_t *code, uint32 *args);

/* The iterators also have the ability to go back in time.  Although the
 * iterator will remember the previous page (and go back to it), the caller
 * must also remember what pages will come after this. */
void fs_log_iter_mark (fs_log_t log);
void fs_log_iter_reset (fs_log_t log);

/**********************************************************************
 *  ____       _            _
 * |  _ \ _ __(_)_   ____ _| |_ ___
 * | |_) | '__| \ \ / / _` | __/ _ \
 * |  __/| |  | |\ V / (_| | ||  __/
 * |_|   |_|  |_| \_/ \__,_|\__\___|
 *
 * Internal structure for the log.
 */

/* This is the number of log queue entries that we maintain as free
 * and useable.  Pages are added to the queue until this count is
 * satisfied.  */
#define FS_LOG_TOP_OFF_LIMIT 8

/* We keep track of a queue of log pages, both of pages that are
 * available to be written to, as well as of pages that we will later
 * write a log to.  FS_LOG_QUEUE_SIZE defines the limit of how many of
 * these pages are in the queue.
 *
 * There should be at least enough pages in the queue to fill a flash
 * block, in order to allow for a full block of log entries (as may
 * happen during startup).  The "-1" is because the first page of the
 * log block must be a superblock.
 *
 * Also add the top-off amount to allow for adding queue entries.
 */
#define FS_LOG_QUEUE_SIZE      ((FS_MAX_PAGES_PER_BLOCK - 1) \
                                + FS_LOG_TOP_OFF_LIMIT)

struct fs_log_data {
  efs_device_t          dev;

  /* Next sequence number to write. */
  uint32                seqno;

  /* device page size */
  uint32       page_size;

  /* Queue of pages. */
  fs_page_id       queue[FS_LOG_QUEUE_SIZE];

  /* Parameters for the queue, [alloc] is the next page to allocate
   * (fs_log_add_page), [free] is the last page used (these are like head
   * and tail of a normal queue).  [flush] is where within the queue we've
   * flushed up to.  [count] is the number of pages between free and alloc.
   * Not counting wrapping of the queue, the parameters can be thought of
   * as describing a chain:
   *
   * free       flush             alloc
   *  |           |                 |
   *  V           V                 V
   *  +---+ +---+ +---+ +---+ +---+ +---+
   *  | 1 | | 2 | | 3 | | 4 | | 5 | | 6 |
   *  +---+ +---+ +---+ +---+ +---+ +---+
   *
   *  In this case, node 6 is the next page to be inserted by
   *  fs_log_add_page.  Node 3 is the next page to be written out (either
   *  by fs_log_add of fs_log_flush).  Node 1 is the next page to be given
   *  by fs_log_get_written.
   */
  int   q_alloc;
  int   q_flush;
  int   q_free;
  int   q_count;

  /* Buffer-Pointer to a (aligned) single page worth of log. */
  uint8         *buffer;

  /* Offset within the page where the next entry will be added. */
  uint32        group_offset;

  /* For the iterator, we have a page number and an offset. */
  fs_page_id    iter_page;
  int           iter_offset;

  /* Also remember the mark position. */
  fs_page_id    mark_page;
  int           mark_offset;

  /* To track the progress of the last log seq during playback */
  uint32       last_log_seq;

  /* callback to be invoked for writing log-pages. */
  fs_log_page_write_cb_t     log_page_write_cb;
  void                       *priv_data;

};

#ifdef __cplusplus
}
#endif

#endif /* defined (FEATURE_EFS_EFS2_ON_NAND) || (FEATURE_EFS_EFS2_ON_NOR) ||
                  (FEATURE_EFS_EFS2_ON_SPI_NOR) */

#endif /* FEATURE_EFS_QEFS2 */

#endif /* __FS_NOR_LOG_H__ */
