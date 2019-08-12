/**********************************************************************
 * fs_pm_ptable_nor.c
 *
 * Copyright (C) 2002-2013 QUALCOMM Technologies, Inc.
 * Page table management.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_pm_ptable_nor.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2013-10-09    nr      Replace banned memory functions with safer versions.
2013-06-19    dks     Added NAND Flash scrub support.
2013-03-29    nr      Fix Klocwork warnings.
2012-03-20    wek     Cleanup customer header file inclusion.
2012-03-13    wek     Remove redundant header file inclusion fs_efs2_cust.
2011-06-16    wek     Remove dynamic buffer allocation for read pointer.
2011-06-15    dks     Fix read_pointer buffer to not get written before read.
2011-05-13    dks     Add read_pointer buffer to device layer.
2010-11-12    wek     Make nand/nor PM independent of page size macro.
2010-09-01    wek     Remove compile time calculations based on page size.
2010-07-30    wek     Dynamically allocate all buffers dependent on page size.
2010-06-25    dks     Added counters for partial-write, begin & suspend-erase.
2010-05-10    dks     Added change to support EFS on SPI-NOR device.
2009-05-18    rp      Removed obsolete feature FS_FEATURE_RMTEFS_SERVER.
2008-11-03    rp      Introduce device abstraction.
2008-02-08    np      Recover all possible log pages in the log-queue.
2007-10-18    rp      Featurized QEFS2.
2007-09-19    yg      Changed NULL Tag name.
2007-08-31    yg      Changed numericals to constants.
2007-08-10    rp      Align buffers passed to flash on a 32-byte boundary.
2007-07-06    yg      Added code to handle tags.
2007-05-25    umr     Added support for EFS memory protection feature.
2006-09-20    dlb     Lint cleanup.
2006-09-11    dlb     Use flash driver wrappers.
2006-08-23    dlb     Only use sequence number when log page is valid.
2006-08-22    dlb     Allow logging of garbagified pages.
2006-08-17    dlb     Add ability to upgrade logs on FS upgrade.
2006-07-03    sh      Moved dog & lcd_message prototypes to fs_pm.c
2006-06-26     yg     Memory reduction effort
2006-03-31    sh      Lint cleanup
2006-02-17    sh      Silence compiler warning for uninitialized field_size
2006-01-31    dlb     Cleanup from review.
2006-01-24    dlb     Assure values read from paired-bit NOR are correct.
2006-01-17    dlb     Support multiple NOR write styles.
2006-01-13    dlb     Broken handling of some corrupt log pages.
2005-12-14    dlb     Handle NOR partial writes on bad powerdown.
2005-05-26    sd      Compilation fixes for L4.
2005-04-26    dlb     Allow magic obliteration failures.
2005-01-27    dlb     Allow standalone builds.
2005-01-04    dlb     Update copyright line.
2004-12-30    dlb     Remove excess infiltration of factory image code.
2004-10-15    dlb     Update copyright header.
2004-10-08    dlb     Fix NOR fresh start.
2004-10-07    dlb     Whitespace cleanup.
2004-08-14    dlb     Faster initial start on NOR.
2004-05-14     gr     Suppress lcd message if FEATURE_UI_CORE_REMOVED is on.
2003-06-17    jkl     Clean up code.
2003-06-12    adm     Add support for factory start.
2003-04-24    bgc     Added support for partial erase.  pt->begin_erase()
                      is used to invalidate the page table and mark it as
                      garbage.  pt->erase() marks the ptable page as valid.
2003-04-22     gr     Split off NOR and NAND page managers into separate files.
2003-03-07    dlb     Add support for NAND.
2003-03-05     gr     Put a message on the screen when doing a fresh start.
2003-03-04     gr     Added a call to delay watchdog monitoring of tasks if EFS
                      is doing a fresh start. Initialization takes longer in
                      this case.  Also displays an initialization message.
2003-02-28    jkl     Clean up code.
2003-01-24    bgc     Added handling of FS_LOG_ENTRY_GC_DEALLOC.
2002-11-15    dlb     Write NOR version.
2002-08-08    drh     Created by dlb.  NAND version.

===========================================================================*/

#include "fs_device.h"

#ifdef FEATURE_EFS_QEFS2

#if defined (FEATURE_EFS_EFS2_ON_NOR)       ||  \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assert.h"
#include "fs_config_i.h"
#include "fs_sys_types.h"
#include "fs_err.h"
#include "fs_util.h"

#include "fs_pm_ptable.h"
#include "fs_pm_super.h"
#include "fs_pm_log.h"
#include "fs_logr.h"
#include "fs_upgrade.h"
#include "fs_malloc.h"
#include "fs_protected_memory.h"

#ifdef FS_STANDALONE
  /* Spurious defines coming from system includes. */
  #undef minor
  #undef major
#endif

/* The marker that a ptable is valid is a magic number in the final
   ptable entry.  It needs to be a value that will never appear
   during an erase */
#define FS_NOR_PTABLE_MAGIC 0xF0F0E1E1

/* Similar value for the paired-bits flash devices.  This must be the same
 * as the constant FS_NOR_PTABLE_MAGIC, after going through the spreading
 * operation described in ptable_write_entry_paired. */
#define FS_NOR_PTABLE_PAIRED_MAGIC1     0xFC03FC03
#define FS_NOR_PTABLE_PAIRED_MAGIC2     0xFF00FF00

#ifdef FS_UNIT_TEST
extern void utest_show_one (fs_page_id page, fs_page_id state);
extern void utest_show_range (fs_page_id a, fs_page_id b, fs_page_id state);
#endif

static fs_page_id empty_log_pages[FS_LOG_TOP_OFF_LIMIT];
/* static void pcache_test (fs_ptable_t pt); */
static void setup_ptables (fs_ptable_nor_t npt);
static void restart_ptables (fs_ptable_t pt);
static void find_log_pages (fs_ptable_nor_t npt);


/**********************************************************************
 * Private declarations.  These are the entry points for this
 * implementation of ptable. */
static void ptable_write_entry_simple (fs_ptable_nor_t npt, fs_page_id page,
                                fs_page_id state, int ignore_error);
static void ptable_write_entry_paired (fs_ptable_nor_t npt, fs_page_id page,
                                fs_page_id state, int ignore_error);

static void nor_log_iterate (fs_ptable_t pt, fs_log_t log,
    void (*visit) (void *priv, fs_log_code_t code, uint32 *args),
    void *priv);
static fs_page_id nor_get_forward (fs_ptable_t pt, fs_cluster_id cluster);
static fs_cluster_id nor_get_reverse_simple (fs_ptable_t pt, fs_page_id page,
                                          uint32* tagp);
static fs_cluster_id nor_get_reverse_paired (fs_ptable_t pt, fs_page_id page,
                                          uint32* tagp);
static void nor_set_forward (fs_ptable_t pt, fs_cluster_id cluster,
    fs_page_id page);
static void nor_set_reverse (fs_ptable_t pt, fs_page_id page,
    fs_page_id state, uint32 tag);
static int nor_erase (fs_ptable_t pt, fs_block_id block);
static void nor_begin_erase (fs_ptable_t pt, fs_block_id block);
static void nor_mark_alloc (fs_ptable_t pt, fs_page_id page);
static void nor_flush (fs_ptable_t pt);
static int nor_do_gc (fs_ptable_t pt);
static int nor_scrub (fs_ptable_t pt);
static void nor_post_init (fs_ptable_t pt);
static void nor_super_update (fs_ptable_t pt);

/* Initialize the page table structures.  Call the superblock
 * initialization.  If this is the first time running on this flash,
 * initialize ourselves afresh.  Otherwise read in the log and build up a
 * journal based on what the last operations were. */
void
fs_ptable_init_nor (
    fs_ptable_t         pt,
    fs_log_t            log,
    int                *fresh_start)
{
  fs_ptable_nor_t npt = (fs_ptable_nor_t) pt;
  unsigned tmp;
  unsigned field_size = 0; /* Initialize to avoid compiler warning */
  uint32 page_size = pt->super.data->page_size;

  (void) log;

  /* Initialize the methods. */
  pt->get_forward = nor_get_forward;
  pt->set_forward = nor_set_forward;
  pt->set_reverse = nor_set_reverse;
  pt->erase = nor_erase;
  pt->begin_erase = nor_begin_erase;
  pt->mark_alloc = nor_mark_alloc;
  pt->flush = nor_flush;
  pt->log_iterate = nor_log_iterate;
  pt->do_gc = nor_do_gc;
  pt->scrub = nor_scrub;
  pt->post_init = nor_post_init;
  pt->mid_init = restart_ptables;
  pt->super_update = nor_super_update;

  npt->rcache.count = 0;
  npt->rcache.size = FS_PTABLE_RCACHE_BASE_SIZE *
                     (page_size / FS_MIN_PAGE_SIZE);
  npt->rcache.data = fs_malloc (npt->rcache.size *
                                sizeof (struct fs_ptable_rcache_entry));

  /* Determine operations specialized to each flash type. */
  switch (pt->super.data->u.nor.style) {
    case FS_DEVICE_WRITES_SIMPLE:
      field_size = page_size >> 2;
      pt->get_reverse = nor_get_reverse_simple;
      npt->ptable_write_entry = ptable_write_entry_simple;
      break;

    case FS_DEVICE_WRITES_PAIRED_BITS:
      field_size = page_size >> 3;
      pt->get_reverse = nor_get_reverse_paired;
      npt->ptable_write_entry = ptable_write_entry_paired;
      break;

    default:
      FS_ERR_FATAL ("Unsupported NOR flash device write style", 0, 0, 0);
  }

  /* pcache_test (pt); */
  /* XXX: Move into NOR specific function. */

  /* Mask to mask of the low bits for the number of entries that will fit
   * in a page. */
  npt->minor_mask = field_size - 1;

  /* Compute a shift to determine which page a table entry belongs in. */
  tmp = npt->minor_mask;
  npt->major_shift = 0;
  while (tmp != 0) {
    tmp >>= 1;
    npt->major_shift++;
  }

  /* How far into each block does the table (the reserved part) start. */
  npt->reserved_offset =
    pt->super.data->block_size -
    ((pt->super.data->block_size + npt->minor_mask) >> npt->major_shift);

  /* end XXX */

  npt->in_startup_scan = 1;

  /* Setup properly if this is a fresh start. */
  if (*fresh_start) {
    setup_ptables (npt);
  } else {
    find_log_pages (npt);
  }

#ifdef FS_UNIT_TEST
  {
    fs_page_id page;

    for (page = 0; page < pt->super.total_pages; page++) {
      utest_show_one (page, pt->get_reverse (pt, page, NULL));
    }
  }
#endif
}

/* Tacky, that this is here, but it is only used by startup, and can be
 * shared between different invocations of the FS.  As long as the
 * different devices are started sequentially, then this won't hurt. */
#define MAX_REPLAY_LOGS     (256 + 32)
static fs_page_id replay_log_pages[MAX_REPLAY_LOGS];
static unsigned replay_log_count;
static uint32 replay_highest_seqno;

/* Setup the initial ptable for a fresh NOR flash.  Construct the proper
 * tables. */
static void
setup_ptables (fs_ptable_nor_t npt)
{
  fs_ptable_t pt = &npt->parent;
  fs_page_id page;

  if (pt->super.total_pages > FS_MAX_NOR_PAGES) {
    FS_ERR_FATAL ("EFS configured too small for flash size", 0, 0, 0);
  }

  /* Fill in the entire forward page table to be empty. */
  for (page = 0; page < pt->super.total_pages; page++) {
    npt->ptable[page] = FS_INVALID_PAGE_ID;
  }
}

/* Compare two sequence numbers.  Divides them into segments and handles
 * the wrap of segments properly.  Returns true if [a] < [b] logically. */
static int
seqno_is_lesseq (uint32 a, uint32 b)
{
  if (a < 0x40000000 && b >= 0xc0000000)
    return 0;
  if (b < 0x40000000 && a >= 0xc0000000)
    return 1;
  return a <= b;
}

/* Preliminary scan of the ptable.  A bad NOR powerdown can cause ptable
 * entries to be partially written.  This first scan only looks for log
 * entries so that we can scan through the log to be able to fix up any
 * partially written ptable entries. */
static void
find_log_pages (fs_ptable_nor_t npt)
{
  fs_ptable_t pt = &npt->parent;
  uint32 *buf;
  fs_page_id page, state;
  uint32 initial_seqno = 0;
  uint32 highest_seqno;
  int no_log = 0;
  int use_log_page;
  int is_valid;

  if (pt->super.data->log_head == FS_INVALID_PAGE_ID) {
    FS_ERR_FATAL ("Superblock doesn't contain a log page", 0, 0, 0);
    /* Not enough initialization happened before and no log page was
     * written. */
  }

  replay_log_count = 0;

  buf = fs_device_read_pointer (pt->dev, pt->super.data->log_head,
                                FS_FOP_LOG);
  ASSERT (buf != NULL);

  if (*buf == FS_INVALID_PAGE_ID ||
      !fs_log_is_valid ((void *) buf, pt->dev, pt->super.data->log_head))
  {
    no_log = 1;
    highest_seqno = FS_INVALID_PAGE_ID;
  } else {
    initial_seqno = *buf;
    highest_seqno = initial_seqno;
  }

  for (page = 0; page < pt->super.total_pages; page++)
  {
    state = pt->get_reverse (pt, page, NULL);
    if (state == FS_RMAP_PAGE_LOG)
    {
      /* This is indicated as a log.  As long as the sequence number is in
       * the right range, include it in the list.  Also include those that
       * haven't been written, so that they can be used this time up. */

      buf = fs_device_read_pointer (pt->dev, page, FS_FOP_LOG);
      ASSERT (buf != NULL);

      is_valid = fs_log_is_valid ((void *) buf, pt->dev, page);

      /* Be sure to only add log page if the entire log is erased, not
       * partially written. */
      use_log_page = 0;
      if (*buf == FS_INVALID_PAGE_ID) {
        if (fs_device_is_erased (pt->dev, page)) {
          use_log_page = 1;
        }
      } else if (!no_log && seqno_is_lesseq (initial_seqno, *buf)) {
        if (is_valid) {
          use_log_page = 1;
        }
      }

      if (use_log_page)
      {
        if (replay_log_count == MAX_REPLAY_LOGS) {
          FS_ERR_FATAL ("Too many logs to replay", 0, 0, 0);
        }
        replay_log_pages[replay_log_count] = page;
        replay_log_count++;
      }
      if (is_valid && *buf != FS_INVALID_PAGE_ID &&
          (highest_seqno == FS_INVALID_PAGE_ID ||
           seqno_is_lesseq (highest_seqno, *buf)))
        highest_seqno = *buf;
    }
  }

  replay_highest_seqno = highest_seqno;

  fs_upgrade_finished (FS_UPGRADE_LOG_ZERO_AFTER_CRC);
}

/* Reload the ptables from the contents of flash.  The superblock has been
 * loaded from flash.  The first log replay should have found the partially
 * written values, and corrected them. */
static void
restart_ptables (fs_ptable_t pt)
{
  fs_ptable_nor_t npt = (fs_ptable_nor_t) pt;
  fs_page_id page, state;

  npt->in_startup_scan = 0;

  /* printf ("Log start = 0x%x\n", pt->super.data.log_head); */

  /* TODO: Redundant. */
  if (pt->super.data->log_head == FS_INVALID_PAGE_ID) {
    /* "Not enough initialization happened to write a log page." */
    FS_ERR_FATAL ("Superblock doesn't contain a log page", 0, 0, 0);
  }

  /* XXX: This can be sped up quite a bit by directly reading the flash
   * here. */

  /* First, zero out all of the existing page tables. */
  if (pt->super.total_pages > FS_MAX_NOR_PAGES) {
    FS_ERR_FATAL ("EFS configured too small for flash size", 0, 0, 0);
  }
  memset (npt->ptable, 0xFF, pt->super.total_pages * sizeof (fs_page_id));

  /* Scan through each rtable entry, and insert the appropriate ones into
   * the ptable. */
  for (page = 0; page < pt->super.total_pages; page++) {
    state = pt->get_reverse (pt, page, NULL);
    if (!FS_RMAP_IS_SPECIAL (state)) {
      ASSERT (state < FS_MAX_NOR_PAGES);
      if (npt->ptable[state] != FS_INVALID_PAGE_ID) {
        FS_ERR_FATAL ("XXX: Handle duplicate page", 0, 0, 0);
      }

      if (state >= pt->super.total_pages)
        FS_ERR_FATAL ("Out of bounds ptable entry", 0, 0, 0);

      npt->ptable[state] = page;
    }
  }
}

/* Called by the upper layer to replay the log.  This is here, because we
 * are the ones that found where the logs are in the first place.  This is
 * fairly complex, but needed to handle all of the special cases that the
 * log ends up in. */
static void
nor_log_iterate (fs_ptable_t pt, fs_log_t log,
    void (*visit) (void *priv, fs_log_code_t code, uint32 *args),
    void *priv)
{
  fs_ptable_nor_t npt = (fs_ptable_nor_t) pt;
  unsigned a, b;
  int result;
  uint32 header[2];
  fs_log_code_t code;
  uint32 args[4];
  int done;
  int xact_depth = 0;
  uint32 state, clust, tag;
  unsigned tmp;
  unsigned empty_log_pages_count;

  /* Setup the log playing to start with the right sequence number. */
  fs_log_set_sequence (log, replay_highest_seqno);

  for (a = 0; a < replay_log_count; a++) {
    if (replay_log_pages[a] == pt->super.data->log_head)
      break;
  }

  /* If the log isn't found something is seriously wrong.  This also
   * catches zero logs being found. */
  if (a == replay_log_count) {
    /* This happens with a bunch of consecutive bad powerdowns.  The GC
     * doesn't make much progress, usually just copying data and then
     * erasing it. */
    return;
    /* FS_ERR_FATAL ("Unable to find log for replay", 0, 0, 0); */
  }

  /* Now iterate through each of these pages. */
  b = a;
  empty_log_pages_count = 0;
  do {
    header[0] = FS_INVALID_PAGE_ID;
    result = fs_log_iter_set_page (log, replay_log_pages[b], header);
    /* printf ("Log: 0x%04x, result = %d, header[0] = 0x%x, "
               "header[1] = 0x%x\n",
        replay_log_pages[b], result, header[0], header[1]); */

    if (result == FS_LOG_ITER_GOOD) {
      /* Loop through the contents of the log. */
      done = 0;
      while (!done) {
        fs_log_iter_step (log, &code, args);

        switch (code) {
          case LOG_CODE_GROUP_END:
            done = 1;
            break;

          case FS_LOG_ENTRY_GC_DEALLOC:
            /* Mark this page as garbage */
            if (header[1] == 0xFFFFFFFF) {
              clust = args[0] & CLUSTER_MASK;
              /* Make sure cluster is within bounds. */
              ASSERT (clust < FS_MAX_NOR_PAGES);

              tag = (args[0] >> TAG_SHIFT) & TAG_MASK;
              npt->ptable[clust] = FS_INVALID_PAGE_ID;
              if (pt->get_reverse (pt, args[1], NULL) != FS_RMAP_PAGE_GARBAGE)
                pt->set_reverse (pt, args[1], FS_RMAP_PAGE_GARBAGE,
                                 FS_PM_NULL_TAG);
            }
            break;

          case FS_LOG_ENTRY_GC_MOVE:
            /* If the changes are not reflected in ptable, redo the move
             * related changes in ptable now. */
            if (header[1] == 0xFFFFFFFF) {
              clust = args[0] & CLUSTER_MASK;
              /* Make sure cluster is within bounds. */
              ASSERT (clust < FS_MAX_NOR_PAGES);

              tag = (args[0] >> TAG_SHIFT) & TAG_MASK;
              npt->ptable[clust] = args[2];
              if (pt->get_reverse (pt, args[1], NULL) != FS_RMAP_PAGE_GARBAGE)
                pt->set_reverse (pt, args[1], FS_RMAP_PAGE_GARBAGE,
                                 FS_PM_NULL_TAG);
              if (pt->get_reverse (pt, args[2], NULL) != clust) {
                pt->set_reverse (pt, args[2], clust, tag);
              }
            }
            /*printf ("gc move: %04x: %04x -> %04x\n",
                args[0], args[1], args[2]);*/
            break;

          case FS_LOG_ENTRY_ERASE_FINISH:
            /* printf ("Erase finish: %d (%04x)\n",
                args[0], args[0] << pt->super.block_shift); */
            break;

          case FS_LOG_ENTRY_XACT_START:
            xact_depth++;
            /* printf ("< start transaction\n"); */
            break;

          case FS_LOG_ENTRY_XACT_END:
            xact_depth--;
            /* printf ("> end transaction>\n"); */
            break;

          case FS_LOG_ENTRY_NEW_DATA:
            if (header[1] == 0xFFFFFFFF) {
              clust = args[0] & CLUSTER_MASK;
              /* Make sure cluster is within bounds. */
              ASSERT (clust < FS_MAX_NOR_PAGES);

              tag = (args[0] >> TAG_SHIFT) & TAG_MASK;
              npt->ptable[clust] = args[1];
              if (pt->get_reverse (pt, args[1], NULL) != clust)
                pt->set_reverse (pt, args[1], clust, tag);
            }
            /* printf (" new data: %04x: %04x\n", args[0], args[1]); */
            break;

          case FS_LOG_ENTRY_PAGE_MOVE:
            if (header[1] == 0xFFFFFFFF) {
              clust = args[0] & CLUSTER_MASK;
              /* Make sure cluster is within bounds. */
              ASSERT (clust < FS_MAX_NOR_PAGES);

              tag = (args[0] >> TAG_SHIFT) & TAG_MASK;
              npt->ptable[clust] = args[2];
              if (pt->get_reverse (pt, args[1], NULL) != FS_RMAP_PAGE_GARBAGE)
                pt->set_reverse (pt, args[1], FS_RMAP_PAGE_GARBAGE,
                                 FS_PM_NULL_TAG);
              if (pt->get_reverse (pt, args[2], NULL) != clust)
                pt->set_reverse (pt, args[2], clust, tag);
            }
            /* printf (" page move: %04x: %04x -> %04x\n",
                args[0], args[1], args[2]); */
            break;

          case FS_LOG_ENTRY_UPPER_DATA:
            if (args[0] < FS_UPPER_DATA_COUNT)
              pt->super.data->upper_data[args[0]] = args[1];
            break;

          case FS_LOG_ENTRY_LOG_ALLOC:
            /* Detect partially written log ptable entries.  If we have
             * already missed this log in the startup scan, then turn the
             * page into garbage, otherwise into a log.  It is important to
             * actually check the log list, since partial writes can cause
             * differing reads. */
            state = pt->get_reverse (pt, args[0], NULL);
            switch (state) {
              case FS_RMAP_PAGE_GARBAGE:
              case FS_RMAP_PAGE_LOG:
                break;

              default:
                for (tmp = 0; tmp < replay_log_count; tmp++) {
                  if (replay_log_pages[tmp] == args[0])
                    break;
                }
                if (tmp < replay_log_count) {
                  pt->set_reverse (pt, args[0], FS_RMAP_PAGE_LOG,
                                           FS_PM_NULL_TAG);
                }
                else {
                  /* The addition of an erased log-page for which a log
                   * activity of (FS_LOG_ENTRY_LOG_ALLOC, log-page) is found
                   * is added here. The addition of the log-page to the queue
                   * is delayed to maintain the original sequence of the
                   * log-page allocation before power-down. Log-page indexes
                   * are saved in an array and they are added to the queue
                   * later. Filling the log-queue upto FS_LOG_TOP_OFF_LIMIT
                   * number of empty logpages should be sufficient.
                   */
                  if ((empty_log_pages_count < FS_LOG_TOP_OFF_LIMIT) &&
                      (fs_device_is_erased (pt->dev, args[0]))) {
                  /* If the page is entirely erased it should be added to
                   * the empty_log_pages array so that it can be queued
                   * later.
                   */
                    pt->set_reverse (pt, args[0], FS_RMAP_PAGE_LOG,
                                             FS_PM_NULL_TAG);
                    empty_log_pages[empty_log_pages_count++] = args[0];
                  }
                  else {
                    pt->set_reverse (pt, args[0], FS_RMAP_PAGE_GARBAGE,
                                             FS_PM_NULL_TAG);
                  }
                }
            }
            break;

          case FS_LOG_ENTRY_GARBAGE:
            /* Garbage detected by the startup code.  Make sure the ptable
             * entry was written correctly. */
            if (pt->get_reverse (pt, args[0], NULL) != FS_RMAP_PAGE_GARBAGE) {
              pt->set_reverse (pt, args[0], FS_RMAP_PAGE_GARBAGE,
                               FS_PM_NULL_TAG);
            }
            break;

          default:
            FS_ERR_FATAL ("Unknown log code encountered: 0x%02x",
                code, 0, 0);
        }

        if (code != LOG_CODE_GROUP_END)
          visit (priv, code, args);
      }

      /* If it has not had its header changed, then add it to the queue, so
       * that it will get flushed properly. */
      if (header[1] == 0xFFFFFFFF) {
        fs_log_add_written_page (log, replay_log_pages[b]);
      }
    } else if (result == FS_LOG_ITER_CORRUPT) {
      /* It is effectively now garbage. */
    } else /* result == FS_LOG_ITER_ERASED */ {
      /* It is marked as a log already, so just add it to the queue.  We're
       * doing them in the right order, so things should be OK. */
      fs_log_add_page (log, replay_log_pages[b]);
    }

    /* Increment mod the count. */
    b++;
    if (b == replay_log_count)
      b = 0;
  } while (b != a);

  /* Empty log pages are added to the queue.
   */
  for (tmp = 0; tmp < empty_log_pages_count; tmp++) {
    fs_log_add_page (log, empty_log_pages[tmp]);
  }
  if (xact_depth != 0) {
    FS_ERR_FATAL ("Unterminated transaction detected", 0, 0, 0);
  }
}

static void
nor_post_init (fs_ptable_t pt)
{
  (void) pt;
}

static void
nor_super_update (fs_ptable_t pt)
{
  (void) pt;
}

/**********************************************************************
 * Rtable cache operations.  The RTABLE cache is a write cache of changes
 * to the rtable.  These operations cannot be written to flash until the
 * log page describing the change has been written (so that they can be
 * completed).  Also, they cannot be flushed in the middle of a
 * transaction.  This queue is kept sorted to allow fast lookups.
 */

/* Lookup the entry in the queue.  Returns the place in the queue where the
 * entry should be inserted. */
static int
rcache_lookup (struct fs_ptable_rcache *rc, fs_page_id entry)
{
  int a, b, c;
  fs_page_id tmp;

  a = 0;
  b = rc->count;

  while (b > a) {
    c = (a + b) >> 1;
    tmp = rc->data[c].page;
    if (entry == tmp)
      return c;
    if (entry < tmp)
      b = c;
    else
      a = c + 1;
  }

  return b;
}

/**********************************************************************
 * Public interface.
 */
static fs_page_id
nor_get_forward (fs_ptable_t pt, fs_cluster_id cluster)
{
  fs_ptable_nor_t npt = (fs_ptable_nor_t) pt;
  return npt->ptable[cluster];
}

/* Reverse page table reading for simple flash devices. */
static fs_cluster_id
nor_get_reverse_simple (fs_ptable_t pt, fs_page_id page, uint32* tagp)
{
  fs_page_id *pt_pdata;
  fs_page_id *pdata = NULL;
  fs_ptable_nor_t npt = (fs_ptable_nor_t) pt;
  int index = rcache_lookup (&npt->rcache, page);

  fs_page_id block_base;
  unsigned page_offset;
  unsigned last_page_offset;
  unsigned minor, pt_minor;
  unsigned major, pt_major;
  fs_page_id tmp;

  if (index < npt->rcache.count && npt->rcache.data[index].page == page)
  {
    fs_cluster_id clust = npt->rcache.data[index].state;
    if (!FS_RMAP_IS_SPECIAL (clust)) {
      if (tagp){
        *tagp = (clust >> TAG_SHIFT) & TAG_MASK;
      }
      clust = clust & CLUSTER_MASK;
    }
    return clust;
  }

  block_base = page & pt->super.block_mask;

  page_offset = page - block_base;
  last_page_offset = pt->super.data->block_size-1;

  if (page_offset >= npt->reserved_offset)
    return FS_RMAP_PAGE_RESERVED;

  /* Major is the offset in pages from the beginning of the block to the
     ptable page that contains the page_offset */
  major = npt->reserved_offset +(page_offset >> npt->major_shift);
  pt_major = npt->reserved_offset + (last_page_offset >> npt->major_shift);

  /* Minor is the offset within that major page of the ptable entry
     for page_offset. */
  minor = page_offset & npt->minor_mask;
  pt_minor = last_page_offset & npt->minor_mask;

  if (major == pt_major)
  {
    /* the magic pattern and the reverse ptable entry for the page are in
       the same page; read it */
    pdata = (fs_page_id *) fs_device_read_pointer (pt->dev, block_base + major,
                                                   FS_FOP_PTABLE);
    ASSERT (pdata != NULL);
    pt_pdata = pdata;
  }
  else
  {
    /* the magic pattern and the reverse ptable entry for the page are in
       different pages. Read page containing magic pattern */
    pt_pdata = (fs_page_id *) fs_device_read_pointer (pt->dev,
                                                      block_base + pt_major,
                                                      FS_FOP_PTABLE);
    ASSERT (pt_pdata != NULL);
  }

  /* If the ptable is not blessed with the proper value then this
     block is garbage */
  if (pt_pdata[pt_minor] != FS_NOR_PTABLE_MAGIC)
    return FS_RMAP_PAGE_GARBAGE;

  if (pdata == NULL)
  {
    /* the magic pattern and the reverse ptable entry for the page are in
       different pages. Read page containing rptable entry */
    pdata = (fs_page_id *) fs_device_read_pointer (pt->dev, block_base + major,
                                                   FS_FOP_PTABLE);
  }

  ASSERT (pdata != NULL);
  tmp = pdata[minor];
  if (tmp == 0)
    return FS_RMAP_PAGE_GARBAGE;
  else if (tmp == 0xFFFFFFFF)
    return FS_RMAP_BLOCK_ERASED;
  else
  {
    if (tagp){
      *tagp = ((tmp >> TAG_SHIFT) & TAG_MASK);
    }
    if (!FS_RMAP_IS_SPECIAL (tmp))
      tmp = tmp & CLUSTER_MASK;
    return tmp;
  }
}

/* Reverse page table reading for paired-bit flash devices. */
static fs_cluster_id
nor_get_reverse_paired (fs_ptable_t pt, fs_page_id page, uint32* tagp)
{
  fs_page_id *pt_pdata;
  fs_page_id *pdata = NULL;
  fs_ptable_nor_t npt = (fs_ptable_nor_t) pt;
  int index = rcache_lookup (&npt->rcache, page);

  fs_page_id block_base;
  unsigned page_offset;
  unsigned last_page_offset;
  unsigned minor, pt_minor;
  unsigned major, pt_major;
  fs_page_id tmp, tmp2;

  if (index < npt->rcache.count && npt->rcache.data[index].page == page)
  {
    fs_cluster_id clust = npt->rcache.data[index].state;
    if (!FS_RMAP_IS_SPECIAL (clust)) {
      if (tagp){
        *tagp = (clust >> TAG_SHIFT) & TAG_MASK;
      }
      clust = clust & CLUSTER_MASK;
    }
    return clust;
  }

  block_base = page & pt->super.block_mask;

  page_offset = page - block_base;
  last_page_offset = pt->super.data->block_size - 1;

  if (page_offset >= npt->reserved_offset)
    return FS_RMAP_PAGE_RESERVED;

  /* Major is the offset in pages from the beginning of the block to the
     ptable page that contains the page_offset */
  major = npt->reserved_offset +(page_offset >> npt->major_shift);
  pt_major = npt->reserved_offset + (last_page_offset >> npt->major_shift);

  /* Minor is the offset within that major page of the ptable entry
     for page_offset. */
  minor = page_offset & npt->minor_mask;
  pt_minor = last_page_offset & npt->minor_mask;

  if (major == pt_major)
  {
    /* the magic pattern and the reverse ptable entry for the page are in
       the same page; read it */
    pdata = (fs_page_id *) fs_device_read_pointer (pt->dev, block_base + major,
                                                   FS_FOP_PTABLE);
    ASSERT (pdata != NULL);
    pt_pdata = pdata;
  }
  else
  {
    /* the magic pattern and the reverse ptable entry for the page are in
       different pages. Read page containing magic pattern */
    pt_pdata = (fs_page_id *) fs_device_read_pointer (pt->dev,
                                                      block_base + pt_major,
                                                      FS_FOP_PTABLE);
    ASSERT (pt_pdata != NULL);
  }

  /* If the ptable is not blessed with the proper value then this
     block is garbage */
  if (pt_pdata[2*pt_minor]   != FS_NOR_PTABLE_PAIRED_MAGIC1 ||
      pt_pdata[2*pt_minor+1] != FS_NOR_PTABLE_PAIRED_MAGIC2)
  {
    /* printf ("%08x %08x\n", pt_pdata[2*pt_minor+1], pt_pdata[2*pt_minor]); */
    return FS_RMAP_PAGE_GARBAGE;
  }

  if (pdata == NULL)
  {
    /* the magic pattern and the reverse ptable entry for the page are in
       different pages. Read page containing rptable entry */
    pdata = (fs_page_id *) fs_device_read_pointer (pt->dev, block_base + major,
                                                   FS_FOP_PTABLE);
    ASSERT (pdata != NULL);
  }

  /* Please see the full description of this code in
   * ptable_write_entry_paired.  Take the 64-bit 'spread' number, and
   * compress it back into a sane result. */
  tmp = pdata[2*minor];
  ASSERT (npt->in_startup_scan || ((tmp ^ (tmp >> 1)) & 0x55555555U) == 0);
  tmp = ((tmp & 0x44444444) >> 1) | (tmp & 0x11111111);
  tmp = ((tmp & 0x30303030) >> 2) | (tmp & 0x03030303);
  tmp = ((tmp & 0x0f000f00) >> 4) | (tmp & 0x000f000f);
  tmp = ((tmp & 0x00ff0000) >> 8) | (tmp & 0x000000ff);

  tmp2 = pdata[2*minor+1];
  ASSERT (npt->in_startup_scan || ((tmp2 ^ (tmp2 >> 1)) & 0x55555555U) == 0);
  tmp2 = ((tmp2 & 0x44444444) >> 1) | (tmp2 & 0x11111111);
  tmp2 = ((tmp2 & 0x30303030) >> 2) | (tmp2 & 0x03030303);
  tmp2 = ((tmp2 & 0x0f000f00) >> 4) | (tmp2 & 0x000f000f);
  tmp2 = ((tmp2 & 0x00ff0000) >> 8) | (tmp2 & 0x000000ff);

  tmp |= tmp2 << 16;

  if (tmp == 0)
    return FS_RMAP_PAGE_GARBAGE;
  else if (tmp == 0xFFFFFFFF)
    return FS_RMAP_BLOCK_ERASED;
  else
  {
    if (tagp){
      *tagp = ((tmp >> TAG_SHIFT) & TAG_MASK);
    }
    if (!FS_RMAP_IS_SPECIAL (tmp))
      tmp = tmp & CLUSTER_MASK;
    return tmp;
  }
}

static void
nor_set_forward (fs_ptable_t pt, fs_cluster_id cluster, fs_page_id page)
{
  fs_ptable_nor_t npt = (fs_ptable_nor_t) pt;

  npt->ptable[cluster] = page;
}

static void
nor_set_reverse (fs_ptable_t pt, fs_page_id page, fs_page_id state, uint32 tag)
{
  fs_ptable_nor_t npt = (fs_ptable_nor_t) pt;
  struct fs_ptable_rcache *rc = &npt->rcache;
  int index;

#ifdef FS_UNIT_TEST
  utest_show_one (page, state);
#endif
  if (!FS_RMAP_IS_SPECIAL (state)) {
    state = state | ((tag & TAG_MASK) << TAG_SHIFT);
  }

  index = rcache_lookup (rc, page);

  ASSERT ((page & ~pt->super.block_mask) < npt->reserved_offset);
  ASSERT (state != FS_RMAP_BLOCK_ERASED);

  if (index < rc->count && rc->data[index].page == page)
    rc->data[index].state = state;
  else {
    if (rc->count + 1 > rc->size) {
      FS_ERR_FATAL ("Ptable rcache overflow", 0, 0, 0);
    }

    if (index < rc->count) {
      fs_memsmove (&rc->data[index + 1],
                   (rc->count - index) * sizeof(struct fs_ptable_rcache_entry),
                   &rc->data[index],
                   (rc->count - index)*sizeof(struct fs_ptable_rcache_entry));
    }

    rc->count++;
    rc->data[index].page = page;
    rc->data[index].state = state;
  }
}

/* Mark the last page of this block with a page table entry that invalidates
   the entire block.  This prevents a block that is being erased from being
   confused as real data */
static void
nor_begin_erase (fs_ptable_t pt, fs_block_id block)
{
  fs_ptable_nor_t npt = (fs_ptable_nor_t) pt;
  fs_page_id block_end = (block << pt->super.block_shift)
    + pt->super.data->block_size;

  npt->ptable_write_entry (npt, block_end-1, 0, TRUE);
}

/* Clean out rcache for pages in the erased block.  Write
   the magic value to validate the page table. Returns the
   number of erased pages. */
static int
nor_erase (fs_ptable_t pt, fs_block_id block)
{
  fs_ptable_nor_t npt = (fs_ptable_nor_t) pt;
  fs_page_id block_start, block_end;
  fs_page_id tmp;
  int i;

#ifdef FS_UNIT_TEST
  utest_show_range (block << pt->super.block_shift,
      (block << pt->super.block_shift) + npt->reserved_offset - 1,
      FS_RMAP_BLOCK_ERASED);
#endif

  /* Scan through the rcache, and update any entries that are in this
   * block. */
  block_start = block << pt->super.block_shift;
  block_end = block_start + pt->super.data->block_size;

  for (i = 0; i < npt->rcache.count; i++) {
    tmp = npt->rcache.data[i].page;
    if (tmp >= block_start && tmp < block_end)
      npt->rcache.data[i].state = FS_RMAP_BLOCK_ERASED;
  }

  /* Initialize the ptable is valid entry */
  npt->ptable_write_entry (npt, block_end-1, FS_NOR_PTABLE_MAGIC, FALSE);

  return npt->reserved_offset;
}

static void
nor_mark_alloc (fs_ptable_t pt, fs_page_id page)
{
  (void) pt;
  (void) page;
  /* This doesn't do anything for NOR flash, since the rtable got erased
   * automatically by the erase. */
}

/* Flush a single page table entry. */
static void
ptable_write_entry_simple (fs_ptable_nor_t npt, fs_page_id page,
    fs_page_id state, int ignore_error)
{
  fs_page_id block_base, page_offset, minor, major;
  int result;

  block_base = page & npt->parent.super.block_mask;
  page_offset = page - block_base;

  minor = page_offset & npt->minor_mask;
  major = npt->reserved_offset + (page_offset >> npt->major_shift);

  if (state == FS_RMAP_PAGE_GARBAGE)
    state = 0;

  result = fs_device_partial_write (npt->parent.dev, block_base + major,
      &state, minor * sizeof (fs_page_id), sizeof (fs_page_id), FS_FOP_PTABLE);
  if (!ignore_error && result != FS_DEVICE_OK)
    FS_ERR_FATAL ("Unable to write to flash", 0, 0, 0);
}

/* Ptable writing for 'paired-bit' flash devices.
 *
 * These flash devices store each pair of bits in a single cell, with 4
 * different charge levels representing the values of the two bits.
 * Because of this, these devices typically aren't able to re-issue a write
 * if there is a power-failure mid-write.
 *
 * To compensate for this, we "spread" the 32-bit entry across 64 bits,
 * making each pair of bits always have the same value. */
static void
ptable_write_entry_paired (fs_ptable_nor_t npt, fs_page_id page,
                           fs_page_id state, int ignore_error)
{
  fs_page_id block_base, page_offset, minor, major;
  uint32 a, b;
  int result;

  block_base = page & npt->parent.super.block_mask;
  page_offset = page - block_base;

  minor = page_offset & npt->minor_mask;
  major = npt->reserved_offset + (page_offset >> npt->major_shift);

  if (state == FS_RMAP_PAGE_GARBAGE)
    state = 0;

  /* Spread data.  Does so with a divide and conquer approach.  This really
   * does work.  The first line moves 8 bits into the other 16-bit part.
   * Then we move 4 bits of each of those parts up by 4 bits.  Progressing
   * all the way down to a single bit move ends will all of the bits spread
   * out.  The final shift and or makes every pair of bits have the same
   * value. */
  a = state;
  a = ((a & 0x0000ff00) << 8) | (a & 0x000000ff);
  a = ((a & 0x00f000f0) << 4) | (a & 0x000f000f);
  a = ((a & 0x0c0c0c0c) << 2) | (a & 0x03030303);
  a = ((a & 0x22222222) << 1) | (a & 0x11111111);
  a |= a << 1;

  b = state >> 16;
  b = ((b & 0x0000ff00) << 8) | (b & 0x000000ff);
  b = ((b & 0x00f000f0) << 4) | (b & 0x000f000f);
  b = ((b & 0x0c0c0c0c) << 2) | (b & 0x03030303);
  b = ((b & 0x22222222) << 1) | (b & 0x11111111);
  b |= b << 1;

  result = fs_device_partial_write (npt->parent.dev, block_base + major,
      &a, minor * 2 * sizeof (fs_page_id), sizeof (fs_page_id), FS_FOP_PTABLE);
  if (!ignore_error && result != FS_DEVICE_OK)
    FS_ERR_FATAL ("Unable to write to flash", 0, 0, 0);

  result = fs_device_partial_write (npt->parent.dev, block_base + major,
      &b, (minor * 2 + 1) * sizeof (fs_page_id), sizeof (fs_page_id),
      FS_FOP_PTABLE);
  if (!ignore_error && result != FS_DEVICE_OK)
    FS_ERR_FATAL ("Unable to write to flash", 0, 0, 0);
}

/* Flush the rcache out to flash.  This is performed in three passes.
 * First the entries for logs are flushed out.  This way, the other entries
 * will always be finished, if partially done.  Then, the garbage changes
 * are written.  Lastly the rest of the movement is written.
 *
 * There is a problem if power is lost while writing a log entry.
 * This location could be randomly written to another value, causing
 * collisions.  To solve this, we carefully choose the value of the log
 * page type such that a partial write could never be confused with any
 * other type of value.
 *
 * This routine should only be called if the buffered log is empty,
 * otherwise entries will be flushed before their log pages are flushed. */
static void
nor_flush (fs_ptable_t pt)
{
  fs_ptable_nor_t npt = (fs_ptable_nor_t) pt;
  fs_page_id state;
  int i;

  /* First loop through, and write the log entries. */
  for (i = 0; i < npt->rcache.count; i++) {
    state = npt->rcache.data[i].state;
    if (state == FS_RMAP_PAGE_LOG)
      npt->ptable_write_entry (npt, npt->rcache.data[i].page, state, FALSE);
  }

  /* Loop through all of the pcache, and write out all of the change to
   * garbage entries. */
  for (i = 0; i < npt->rcache.count; i++) {
    state = npt->rcache.data[i].state;
    if (state == FS_RMAP_PAGE_GARBAGE)
      npt->ptable_write_entry (npt, npt->rcache.data[i].page, state, FALSE);
  }

  /* Now go through all of the entries, and flush the rest out. */
  for (i = 0; i < npt->rcache.count; i++) {
    state = npt->rcache.data[i].state;
    if (state != FS_RMAP_BLOCK_ERASED && state != FS_RMAP_PAGE_GARBAGE &&
        state != FS_RMAP_PAGE_LOG)
      npt->ptable_write_entry (npt, npt->rcache.data[i].page, state, FALSE);
  }

  npt->rcache.count = 0;
}

/* There is no GC to do in nor ptable. */
static int
nor_do_gc (fs_ptable_t pt)
{
  (void) pt;

  return 0;
}

static int
nor_scrub (fs_ptable_t pt)
{
  (void) pt;
  return -1;
}

#endif  /* FEATURE_EFS_EFS2_ON_NOR || FEATURE_EFS_EFS2_ON_SPI_NOR*/

#endif /* FEATURE_EFS_QEFS2 */
