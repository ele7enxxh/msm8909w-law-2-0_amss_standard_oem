/**********************************************************************

 fs_pm_gc.c

 Garbage collector.

 Copyright (c) 2003-2010, 2012-2013 by QUALCOMM Technologies, Inc.
 All Rights Reserved.

***********************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_pm_gc.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2013-07-11    dks     Added NAND Flash scrub support.
2012-03-13    wek     Remove redundant header file inclusion fs_efs2_cust.
2010-05-10    dks     Added SPI_NOR device type.
2009-05-18    rp      Removed obsolete feature FS_FEATURE_RMTEFS_SERVER.
2009-02-06    yg      Update bad block counts, read page only for Page table.
2009-01-20    yg      Allow configured number of bad blocks.
2008-11-03    rp      Introduce device abstraction.
2008-10-17    yog     Replaced ERR_FATAL to FS_ERR_FATAL in fs_gc_init_finish.
2008-05-07    rp      Fix compiler warning for wrong enum assignment.
2008-04-18    yg      Fix build error.
2008-03-24    sch     Added code to handle bad blocks in ptable region
2007-12-12    yg      Increase GC critical threshold limit to 2 blocks.
2007-12-11    yg      Added some comments.
2007-10-18    rp      Featurized QEFS2.
2007-10-08    yg      Configure GC thresholds and bad block limits runtime.
2007-09-20    sch     Added comments and changed logic in fs_pm_init_finish
2007-09-17    sch     Added exit condition for while loop during startup
2007-08-31    yg      Added some code comments.
2007-08-24    yg      Soft limit GC overhead writes to configured value.
2007-08-10    rp      Align buffers passed to flash on a 32-byte boundary.
2007-07-06    yg      GC Urgency is based on calculation.
2007-07-06    yg      Added dual allocators related code.
2007-02-23    dlb     Move read failures into device wrappers.
2006-12-08    dlb     Use scaling transaction size values.
2006-09-20    dlb     Lint cleanup.
2006-09-11    dlb     Use flash driver wrappers.
2005-05-26    sd      Compilation fixes for L4.
2004-10-15    dlb     Update copyright line.
2004-10-07    dlb     Whitespace cleanup.
2004-05-05    dlb     Handle page write failures in data region.
2004-04-20    dlb     Fix problem with startups and no transactions.
2003-06-17    bgc     Added code to log garbage that was created during the
                      last bad powerdown.
2003-06-13     cr     Change fs_gc_do_urgent if statment to a while statement
                      to fix assert(gc->free_count>0) failure in fs_gc_alloc().
2003-03-07    dlb     Created file.
===========================================================================*/

#include "fs_device.h"

#ifdef FEATURE_EFS_QEFS2
#if defined (FEATURE_EFS_EFS2_ON_NAND)     ||   \
    defined (FEATURE_EFS_EFS2_ON_NOR)      ||   \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)

#include <string.h>

#include "assert.h"
#include "fs_err.h"
#include "fs_pm.h"
#include "fs_pm_gc.h"
#include "fs_pm_types.h"

#ifdef FS_UNIT_TEST
 #include <stdio.h>
#endif

/* If used by shared code, need to implement these parts. */
#define ISNAND 1

#define BUMP_PAGE(gc, pvar) \
  do { \
    (pvar)++; \
    if ((pvar) >= (gc)->max_page) \
      (pvar) = (gc)->min_page; \
  } while (0)

#define BUMP_BLOCK(gc, pvar) \
  do { \
    (pvar) = ((pvar) & (gc)->super->block_mask) + \
      (gc)->super->data->block_size; \
    if ((pvar) >= (gc)->max_page) \
      (pvar) = (gc)->min_page; \
  } while (0)


static void
fs_gc_update_notify (fs_gc_t gc)
{
  if (gc->gc_scrub_info.scrub_state == FS_SCRUB_RUNNING)
  {
    if (gc->gc_next == gc->gc_scrub_info.ptbl_scrub_info.ptbl_gc_pos_at_start)
    {
      gc->gc_scrub_info.ptbl_scrub_info.ptbl_gc_finished_one_cycle = 1;
    }
  }
}

static void
fs_gc_bump_gc_block (fs_gc_t gc)
{
  BUMP_BLOCK (gc, gc->gc_next);
  fs_gc_update_notify (gc);
}

void
fs_gc_init (fs_gc_t gc, int fresh_start)
{
  uint32 ptable_region_size;
  uint32 bad_block_limit;

  (void) fresh_start;

  ptable_region_size = gc->max_page - gc->min_page;
  ptable_region_size /= gc->super->data->block_size;

  bad_block_limit = (MAX_BAD_BLOCK_PERCENT_ALLOWED * ptable_region_size) / 100;
  if (bad_block_limit < FS_MIN_PTABLE_BAD_BLOCK_ALLOWED)
    bad_block_limit = FS_MIN_PTABLE_BAD_BLOCK_ALLOWED;

  gc->max_bad_blocks_allowed = bad_block_limit;

  if (fresh_start)
  {
    gc->gc_next = gc->min_page;
    gc->alloc_next = gc->min_page;
    gc->free_count = 0;
    gc->state = FS_GC_HEAD;

    /* Initialize Bad block tracking variables */
    gc->last_reference = gc->min_page;
    gc->bad_blk_count = 0;
    gc->bad_block_acc = 0;

    if (ISNAND)
    {
      /* XXX: For the ptable region, we do not need to pre-erase anything.
       * This will need to be done to kick-start the GC once this code is
       * shared with the data region. */
    } else
    {
      FS_ERR_FATAL ("NOR generic GC code not supported", 0, 0, 0);
    }
  }
  else
  {
    /* Get the initial values from the superblock.  They will be adjusted
     * as logs are replayed. */
  }

  memset (&gc->gc_scrub_info, 0x0, sizeof(struct fs_pm_gc_scrub_info));
  gc->gc_scrub_info.scrub_state = FS_SCRUB_IDLE;
}

static int
fs_gc_urgency (fs_gc_t gc)
{
  if (gc->overhead_write_count > FS_PT_GC_OVERHEAD_WRITES_PER_TXN)
  {
    /* We are crossing the write limit, so do just enough work to get enough
     * free space critical to the operation. May be later we might do some
     * extra work to free up more space. We need to maintain atleast 2 blocks
     * free space, since if any block becomes bad in PT region while doing GC
     * we need atleast another free block so that we can proceed with doing GC
     * without crashing due to no free space to move data and reclaim free
     * space */
    if (gc->free_count <= (gc->super->data->block_size * 2))
      return 80;
    else
      return 0;
  }
  else
  {
    uint32 pt_region_free_pages = (gc->max_page - gc->min_page);

    /* Check if we lost more than half the space to bad blocks.
     * If yes then probably we might thrash doing GC. */
    if ((pt_region_free_pages / (2 * gc->super->data->block_size)) <
        gc->bad_block_acc)
      FS_ERR_FATAL ("Too many bad blocks in the PT region..", 0, 0, 0);

    /* Subtract the bad blocks that have developed, since that space is not
     * usable anymore. */
    pt_region_free_pages -= (gc->super->data->block_size * gc->bad_blk_count);
    pt_region_free_pages = pt_region_free_pages / 2;

    /* If the device size configuration is too small such that PTable region
     * is smaller and if we cannot have over 4 blocks worth free space, then
     * we can try keep atleast half of the region free. Note that if more than
     * 3 blocks go bad in this region, FS will go severe thrashing. */
    if (pt_region_free_pages > (gc->super->data->block_size * 4))
      pt_region_free_pages = (gc->super->data->block_size * 4);

    /* We are still within write limit for this transaction, so we can free
     * up space liberally so that we can cosume that while thrashing. */
    if (gc->free_count < pt_region_free_pages + 3)
      return 80;
    else
      return 0;
  }
}

static void
fs_gc_handle_write_fail (fs_gc_t gc, uint32 dest)
{
  uint32 tmp;
  uint32 args[3];

  if (FS_ISNOR (gc->super))
    FS_ERR_FATAL ("NOR write failure", 0, 0, 0);

  /* Make sure to flush the logs before marking the page bad */
  args[0] = dest;
  fs_log_add (gc->log, FS_LOG_ENTRY_WRITE_FAIL, args);
  fs_log_flush (gc->log);

  gc->set_reverse (gc->priv, dest, FS_RMAP_BLOCK_BAD);

  /* If the first page in the block went bad, physically mark the block
   * bad, since if its the first page in the block then it will not be
   * handled later. */
   if ((dest & ~gc->super->block_mask) == 0)
       (void) fs_device_mark_block_bad (gc->dev,
                                       dest >> gc->super->block_shift);

  /* Increment bad block count now. We will consider this
   * bad block after we moved all good pages out of this block
   * and mark this as bad in flash and reverse page table. Do
   * err_fatal that time. */
  gc->bad_blk_count++;

  /* freecount needs to be updated by subtracting the remaining
     pages in the current block */
  /* Find the page offset in the block */
  tmp = (dest & ~gc->super->block_mask);
  tmp = gc->super->data->block_size - tmp;

  /* Since the count was already reduced in fs_gc_alloc(), adjust
     it here */
  --tmp;

  ASSERT (tmp <= gc->free_count);
  gc->free_count -= tmp;

  /* Bump the allocator to next block. */
  BUMP_BLOCK (gc, gc->alloc_next);
}

static int
fs_gc_count_garbage (fs_gc_t gc, fs_page_id base)
{
  fs_page_id limit = base + gc->super->data->block_size;
  int count = 0;

  for (; base < limit; base++) {
    switch (gc->get_reverse (gc->priv, base)) {
      case FS_RMAP_PAGE_GARBAGE:
      case FS_RMAP_PAGE_LOG:
      case FS_RMAP_PAGE_SUPER:
        count++;
        break;

      default:
        break;
    }
  }

  return count;
}

static void
fs_gc_single (fs_gc_t gc)
{
  fs_page_id pstate;
  fs_page_id dest;
  int count;
  int result;
  int moved;
  fs_page_id stop_page = FS_INVALID_PAGE_ID;
  uint32 args[3];

  while (1)
  {
    /* We need exit condition here, since after the region has developed
     * its maximum possible bad blocks, or we had bad blocks to start with
     * in the first place, then we just attempt to free space that we cannot
     * really attain. */
    if (stop_page == FS_INVALID_PAGE_ID)
      stop_page = gc->gc_next & gc->super->block_mask;
    else
      if ((stop_page == gc->gc_next) && (gc->state == FS_GC_HEAD))
        break;

    switch (gc->state)
    {
      case FS_GC_HEAD:
      {
        /* The head state comes in only when the page is at the beginning
         * of a block. */
        pstate = gc->get_reverse (gc->priv, gc->gc_next);

        if (gc->gc_next < gc->last_reference)
        {
          gc->bad_blk_count = gc->bad_block_acc;
          gc->bad_block_acc = 0;
        }
        gc->last_reference = gc->gc_next;

        switch (pstate)
        {
          case FS_RMAP_BLOCK_UNKNOWN:
          {
            /* Check if this block is good. */
            if (fs_device_bad_block_check (gc->dev,
                  gc->gc_next >> gc->super->block_shift))
            {
              gc->set_reverse (gc->priv, gc->gc_next, FS_RMAP_BLOCK_BAD);
              fs_gc_bump_gc_block (gc);
              gc->bad_block_acc++;
              if (gc->bad_blk_count < gc->bad_block_acc)
                gc->bad_blk_count = gc->bad_block_acc;
            }
            else
            {
              gc->state = FS_GC_ERASE;
            }
          }
          break;

          case FS_RMAP_BLOCK_BAD:
          {
            /* Increment the bad block accumulator */
            gc->bad_block_acc++;
            if (gc->bad_blk_count < gc->bad_block_acc)
              gc->bad_blk_count = gc->bad_block_acc;
            if (gc->bad_block_acc > gc->max_bad_blocks_allowed)
              FS_ERR_FATAL ("Number of bad blocks in ptable" \
                  "region allowed exceeded %d < %d",
                            gc->bad_block_acc,
                            gc->max_bad_blocks_allowed, 0);
            fs_gc_bump_gc_block (gc);
          }
          break;

          default:
          {
            int do_gc = 0;

            /* Count up the garbage in this page. */
            count = fs_gc_count_garbage (gc, gc->gc_next);
            if (count > 0)
              do_gc = 1;
            if (gc->gc_scrub_info.scrub_state == FS_SCRUB_RUNNING)
              do_gc = 1;

            if (do_gc)
              gc->state = FS_GC_MOVING;
            else
              fs_gc_bump_gc_block (gc);
          }
          break;
        }
      } /* case FS_GC_HEAD */
      break;

      case FS_GC_MOVING:
      {
        pstate = gc->get_reverse (gc->priv, gc->gc_next);
        moved = 0;
        switch (pstate)
        {
          case FS_RMAP_PAGE_GARBAGE:
            break;

          case FS_RMAP_BLOCK_BAD:
          {
            /* Mark the block as bad on flash */
            (void) fs_device_mark_block_bad
              (gc->dev, gc->gc_next >> gc->super->block_shift);

            /* Log the change. */
            {
              int block = gc->gc_next >> gc->super->block_shift;
              args[0] = block;
              fs_log_add (gc->log, FS_LOG_ENTRY_ERASE_FAIL, args);
              fs_log_flush (gc->log);
            }

            /* A bad block marked in the middle of a block means that we
             * detected this failure while writing a page.  At this point,
             * we've already copied all good data out of the block, and can
             * just mark the entire block as 'bad'.  It is important that
             * we do not erase it when we are done. */
            gc->set_reverse (gc->priv, gc->gc_next & gc->super->block_mask,
                             FS_RMAP_BLOCK_BAD);

            fs_gc_bump_gc_block (gc);
            gc->state = FS_GC_HEAD;

            /* Accumulate the count for this bad block, and compare if we
             * crossed the pre configured bad block limit */
            gc->bad_block_acc++;
            if (gc->bad_blk_count < gc->bad_block_acc)
              gc->bad_blk_count = gc->bad_block_acc;

            if (gc->bad_block_acc > gc->max_bad_blocks_allowed)
              FS_ERR_FATAL ("Number of bad blocks allowed exceeded %d",
                            gc->bad_block_acc, 0, 0);

            /* Consider this a unit of work.  If anything, it makes getting
             * out of this state easier. */
            return;
          }

          default:
          {
            /* Read the page and move it only if confirmed that its a PTable pg
             * We should not be producing the page read error for a page that
             * is not written. This would help us to differentiate between a
             * real error and page table corruptions. */
            if (FS_RMAP_IS_TABLE (pstate))
            {
              dest = fs_gc_alloc (gc);
              fs_device_read_page (gc->dev, gc->gc_next,
                                   gc->realloc_buffer,
                                   FS_FOP_PTABLE_GC_MOVE,
                                   FS_FLASH_MUST_WORK);

              result = fs_device_write_page (gc->dev, dest, gc->realloc_buffer,
                                             FS_FOP_PTABLE_GC_MOVE);

              while (result != FS_DEVICE_OK)
              {
                fs_gc_handle_write_fail (gc, dest);
                dest = fs_gc_alloc (gc);
                result = fs_device_write_page (gc->dev, dest,
                                               gc->realloc_buffer,
                                               FS_FOP_PTABLE_GC_MOVE);
              }

              /* We just wrote a page, so increase our overhead count */
              gc->overhead_write_count++;
              gc->page_move (gc->priv, pstate, gc->gc_next, dest);
              moved = 1;
            }
            else
            {
              FS_ERR_FATAL ("Unknown block encountered: %08x", pstate, 0, 0);
            }
          }
          break;
        }

        if (((gc->gc_next + 1) & (~gc->super->block_mask)) == 0)
        {
          gc->state = FS_GC_ERASE;
        }
        else
        {
          BUMP_PAGE (gc, gc->gc_next);
        }

        if (moved)
          return;

      } /* case FS_GC_MOVING  */
      break;

      case FS_GC_ERASE:
      {
        fs_log_flush (gc->log);

        args[0] = gc->gc_next >> gc->super->block_shift;
        result = fs_device_erase_block (gc->dev, args[0], FS_FOP_PTABLE);
        if (result != FS_DEVICE_OK)
        {
          (void) fs_device_mark_block_bad (gc->dev, args[0]);
          fs_log_add (gc->log, FS_LOG_ENTRY_ERASE_FAIL, args);
          fs_log_flush (gc->log);
          gc->set_reverse (gc->priv, gc->gc_next & gc->super->block_mask,
                           FS_RMAP_BLOCK_BAD);
          gc->state = FS_GC_HEAD;
          /* Increment the accumumator for bad blocks */
          gc->bad_block_acc++;
          fs_gc_bump_gc_block (gc);
        }
        else
        {
          fs_log_add (gc->log, FS_LOG_ENTRY_ERASE_FINISH, args);

          gc->state = FS_GC_HEAD;

          /* XXX: What to bump the erase count by?  These are quite
           * intertwined. */
          gc->free_count += gc->erase (gc->priv,
                                       gc->gc_next >> gc->super->block_shift);

          fs_gc_bump_gc_block (gc);

          /* A unit of work is done, so return. */
          return;
        }
      } /* case FS_GC_ERASE */
      break;

      default:
      {
        FS_ERR_FATAL ("Internal state error", 0, 0, 0);
      }
      break;
    }
  }
}

fs_page_id
fs_gc_alloc (fs_gc_t gc)
{
  fs_page_id state;
  fs_page_id target, tmp_stop;

  ASSERT (gc->free_count > 0);

  /* We should get a free page within this range */
  tmp_stop = gc->alloc_next & gc->super->block_mask;

  /* Loop until we return a result. */
  while (1) {
    state = gc->get_reverse (gc->priv, gc->alloc_next);
    switch (state) {
      case FS_RMAP_BLOCK_ERASED:
        if (((gc->alloc_next & (~gc->super->block_mask)) == 0
              && 0))
        {
          FS_ERR_FATAL ("NOR CODE", 0, 0, 0);
        } else {
          gc->mark_alloc (gc->priv, gc->alloc_next);
          target = gc->alloc_next;
          BUMP_PAGE (gc, gc->alloc_next);
          gc->free_count--;
          return target;
        }
        break;

      default:
        BUMP_BLOCK (gc, gc->alloc_next);
        if (gc->alloc_next == tmp_stop)
          FS_ERR_FATAL ("No erased blocks found", 0, 0, 0);
    }
  }
}

int
fs_gc_do_urgent (fs_gc_t gc)
{
  int did_something = 0;

  while (fs_gc_urgency (gc) >= 80) {
    fs_gc_single (gc);
    did_something = 1;
  }

  return did_something;
}

void
fs_gc_visit_node (fs_gc_t gc, fs_log_code_t code, uint32 *args)
{
  fs_page_id page;

  switch (code) {
    case LOG_CODE_GROUP_END:
    case FS_LOG_ENTRY_UPPER_DATA:
    case FS_LOG_ENTRY_XACT_START:
    case FS_LOG_ENTRY_XACT_END:
    case FS_LOG_ENTRY_GC_MOVE:
    case FS_LOG_ENTRY_PAGE_MOVE:
    case FS_LOG_ENTRY_NEW_DATA:
    case FS_LOG_ENTRY_KEYRANGE:
    case FS_LOG_ENTRY_GC_DEALLOC:
      break;

    case FS_LOG_ENTRY_GARBAGE:
      page = args[0];
      if (page >= gc->min_page && page < gc->max_page) {
        gc->set_reverse (gc->priv, page, FS_RMAP_PAGE_GARBAGE);
        gc->alloc_next = page;
        BUMP_PAGE (gc, gc->alloc_next);
      }
      break;

    case FS_LOG_ENTRY_ERASE_FINISH:
      /* args[0] = block */
      (void) gc->erase (gc->priv, args[0]);
      page = args[0] << gc->super->block_shift;
      if (page >= gc->min_page && page < gc->max_page) {
        gc->gc_next = page;
        fs_gc_bump_gc_block (gc);
      }
      break;

    case FS_LOG_ENTRY_ERASE_FAIL:
      /* args[0] = block */
      page = args[0] << gc->super->block_shift;
      if (page >= gc->min_page && page < gc->max_page) {
        gc->set_reverse (gc->priv, page, FS_RMAP_BLOCK_BAD);
      }
      break;

    case FS_LOG_ENTRY_WRITE_FAIL:
      /* args[0] = page */
      page = args[0];
      if (page >= gc->min_page && page < gc->max_page) {
        gc->set_reverse (gc->priv, page, FS_RMAP_BLOCK_BAD);
      }
      break;

    case FS_LOG_ENTRY_PTABLE_MOVE:
      /* args[0] = state
       * args[1] = old_place
       * args[2] = new_place
       */
      if (args[2] >= gc->min_page && args[2] < gc->max_page) {
        /* printf ("ptmove %04x %8x %8x\n",
            args[0], args[1], args[2]); */
        gc->page_move (gc->priv, args[0], args[1], args[2]);
        gc->alloc_next = args[2];
        BUMP_PAGE (gc, gc->alloc_next);

        /*
        gc->gc_next = args[1];
        BUMP_PAGE (gc, gc->gc_next);
        */
      } else
        FS_ERR_FATAL ("Unknown region", 0, 0, 0);
      break;

    case FS_LOG_ENTRY_LOG_FLUSH:
      break;

    default:
      FS_ERR_FATAL ("Unknown log code: 0x%02x", code, 0, 0);
      break;
  }
}

/* Note, this assumes it only gets called when we aren't doing a fresh
 * start. */
void
fs_gc_init_finish (fs_gc_t gc)
{
  fs_page_id tmp_page, tmp_stop, state;
  uint32 args[3];

  /* The purpose of this loop is to advance alloc_next (the next
   * page to allocate) until we find a page that is both marked as
   * erased in the reverse page table, and actually erased in the
   * flash.
   */
  while (!fs_device_is_erased (gc->dev, gc->alloc_next))
  {

    /* If we're at the first page of a block, move forward by
     * whole blocks until we find a block that is marked as Erased
     * in the PT.  An erased page at the begining of the block
     * indicates that the whole block is erased, since we write in
     * order.  The loop checks for an erased page at the start of
     * each block. */
    if ((gc->alloc_next & ~gc->super->block_mask) == 0)
    {
      /* Note the start of the current block so that we can abort
       * if we wrap back around to this block. */
      tmp_stop = gc->alloc_next & gc->super->block_mask;

      while (gc->get_reverse (gc->priv, gc->alloc_next)
             != FS_RMAP_BLOCK_ERASED)
      {
        /* This block is not erased.  Move to the next block */
        BUMP_BLOCK (gc, gc->alloc_next);

        /* If we've checked every block in this region and found no
         * erased block, then something is very corrupt and we must
         * abort immediately. */
        if (gc->alloc_next == tmp_stop)
          FS_ERR_FATAL ("No erased block found in the entire region.", 0,0,0);
      }

      /* By now, alloc_next is pointing to an ERASED page in ptable */
    }

    /* There are cases when a page is not perfectly erased in the
     * flash but its status is ERASED in the ptable (such as after
     * power loss during a write operation).  In this case, the page
     * is marked as garbage. */

    /* If alloc_next is pointing to the beginning of the block, that means
     * its state is ERASED in ptable but not necessarily on the flash in
     * case of a bad powerdown. If we mark this page as GARBAGE, we might
     * potentially waste one good page and add an unnecessary log, but this
     * is needed for the case of bad powerdown handling. */
    gc->set_reverse (gc->priv, gc->alloc_next,
                             FS_RMAP_PAGE_GARBAGE);
    args[0] = gc->alloc_next;

    /* Note: we haven't allocated log pages yet, so if too many
     * pages are detected as garbage, we'll overflow the log. */
    fs_log_add (gc->log, FS_LOG_ENTRY_GARBAGE, args);

    BUMP_PAGE (gc, gc->alloc_next);
  }
  /* gc->alloc_next now points to a clean erased page  that is both
   * erased on flash and in the ptable */

  /* Set the GC state appropriately. */
  if ((gc->gc_next & ~gc->super->block_mask) == 0)
    gc->state = FS_GC_HEAD;
  else
    gc->state = FS_GC_MOVING;

  /* Since we made sure that the alloc_next is pointing to a physically
   * erased flash page, we can mark it as erased in the ptable.
   * If alloc_next is pointing to the beginning of the block and
   * if it is already marked as ERASED in ptable, then the following
   * statement becomes redundant. But if alloc_next is not pointing to
   * the beginning of the block then it is needed. */
  gc->set_reverse (gc->priv, gc->alloc_next,
      FS_RMAP_BLOCK_ERASED);

  /* Recompute the free count. */
  gc->free_count = 0;

  tmp_page = gc->alloc_next;
  tmp_stop = gc->gc_next & gc->super->block_mask;
  while (tmp_page != tmp_stop) {
    state = gc->get_reverse (gc->priv, tmp_page);
    if (state == FS_RMAP_BLOCK_ERASED) {
      /* ASSUMES NAND. */
      gc->free_count += gc->super->data->block_size -
        (tmp_page & ~gc->super->block_mask);
    }
    BUMP_BLOCK (gc, tmp_page);
  }
}

static void
fs_gc_scrub_start (fs_gc_t gc)
{
  memset (&gc->gc_scrub_info.ptbl_scrub_info, 0x0,
          sizeof (struct fs_pm_gc_ptbl_scrub_info));
  ASSERT (gc->gc_scrub_info.scrub_state == FS_SCRUB_IDLE);
  gc->gc_scrub_info.scrub_state = FS_SCRUB_RUNNING;
  gc->gc_scrub_info.ptbl_scrub_info.ptbl_gc_pos_at_start = gc->gc_next &
                                             gc->super->block_mask;
}

static int
fs_gc_scrub_update (fs_gc_t gc)
{
  int result = 1;

  fs_gc_single (gc);

  if (gc->gc_scrub_info.ptbl_scrub_info.ptbl_gc_finished_one_cycle == 1)
    result = 0;

  return result;
}

static void
fs_gc_scrub_finish (fs_gc_t gc)
{
  ASSERT (gc->gc_scrub_info.scrub_state == FS_SCRUB_RUNNING);
  gc->gc_scrub_info.scrub_state = FS_SCRUB_IDLE;
}

int
fs_gc_scrub (fs_gc_t gc)
{
  int result = 0;

  switch (gc->gc_scrub_info.scrub_state)
  {
    case FS_SCRUB_IDLE:
    {
      fs_gc_scrub_start (gc);
      result = 1;
    }
    break;

    case FS_SCRUB_RUNNING:
    {
      result = fs_gc_scrub_update (gc);
      if (result == 0)
      {
        fs_gc_scrub_finish (gc);
      }
    }
    break;

    default:
    {
      FS_ERR_FATAL ("%d, Unknown state of ptbl scrub",
                    gc->gc_scrub_info.scrub_state, 0, 0);
    }
    break;
  }

  return result; /* Scrubbing pending */
}

#endif /* defined (FEATURE_EFS_EFS2_ON_NAND) || (FEATURE_EFS_EFS2_ON_NOR) ||
                  (FEATURE_EFS_EFS2_ON_SPI_NOR) */

#endif /* FEATURE_EFS_QEFS2 */

