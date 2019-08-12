/**********************************************************************
 * fs_pm.c
 *
 * Copyright (C) 2003-2014, 2016 QUALCOMM Technologies, Inc.
 * Page Manager for EFS.
 */


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_pm.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when          who     what, where, why
--------      ---     ------------------------------------------------------
06/23/16      sb      Remove function calls with ASSERT macro.
03/05/14      wek     Fix compile time warnings seen in boot loaders.
08/21/13      dks     Fix compilation issues on NOR target.
08/02/13      dks     Fix computation of free count at init.
07/22/13      rp      Add new EFS API to store data in SuperBlock.
07/11/13      dks     Added NAND Flash scrub support.
07/08/13      dks     Finish PM init before shred.
06/13/13      dks     Fix Nand PM logic to search erased blocks.
05/02/13      dks     Make fs_pm_init free of flash writes and erases
05/08/13      dks     Fix NOR PM to update super page when not written.
05/07/13      wek     Add EFS2 Read Only support.
04/29/13      wek     Stub out qword functions for bootloaders.
20/02/13      dks     Clean up legacy dog usage, make fs_task not dog monitored
01/11/13      dks     Move writing to Golden partition outside of efs_sync.
18/12/12      dks     Update golden copy strategy for new rmts layout.
07/18/12      wek     Fix corner case of EFS running out of erased pages.
07/11/12      wek     Fix corner case of EFS running out of log pages.
03/29/12      rp      Eliminate lcd_debug_message call.
03/13/12      wek     Remove redundant header file inclusion fs_efs2_cust.
12/01/11      wek     Avoid calling shred when it is not implemented.
10/25/11      dks     Increase usable EFS space for SPI-NOR storage.
10/11/11      nr      Added borrow_ram support to borrow ram from lower layers.
06/22/11      dks     Fix feature guard for lcd_debug_message.
06/14/11      dks     Fix compiler warning for NOR.
05/02/11      dks     Fix fs_pm_single in NOR to GC blocks with no superpage.
03/20/11      dks     Check for failure status after begin erase.
03/08/11       rp     Add support to lock EFS.
11/18/10       rp     Support TAR image Golden-Copy for RMTS.
11/12/10      wek     Make nand/nor PM independent of page size macro.
11/08/10      wek     Remove impractical and unused paranoia module.
10/19/10      wek     Remove dependency with page size macro in super module.
09/02/10      yog     NOR_PM R/W performance optimization.
09/01/10      wek     Remove compile time calculations based on page size.
07/30/10      wek     Dynamically allocate all buffers dependent on page size.
06/25/10      dks     Added counters for partial-write, begin & suspend-erase.
06/01/10      wek     Journal oveflow fix.
05/21/10       rp     Fix compiler warnings seen on target.
05/13/10      wek     Make sync_no_wait non blocking.
05/10/10      dks     Added change to support EFS on SPI-NOR device.
05/06/10      wek     Implement golden copy for remote storage page manager.
01/19/10       vr     Added efs_sync_no_wait and efs_sync_get_status APIs.
08/04/09       sh     Fixed error message when bad block count is over max.
05/18/09       rp     Removed obsolete feature FS_FEATURE_RMTEFS_SERVER.
04/10/09       rp     Initialize the fs_pm_log_fail_recurse_cnt on init.
03/16/09       yg     Cleanup fs_pm_init function to improve readability.
01/20/09       yg     Allow configured number of bad blocks.
01/14/09       yg     Fix to use correct offset of upper data.
12/24/08       yg     Fix potentially overwriting a page after a bad pwrdown.
12/12/08       rp     Introducing i/o-vec interface.
11/03/08       rp     Introduce device abstraction.
10/13/08       yg     Fixed bug that causes not to search for erased blocks.
10/06/08       yg     Remove , after last member in enum.
09/10/08       np     Added efs_shred function that deletes the obsolete data.
08/22/08       yg     Flush NOR log only if less space for a txn in log buffer.
06/02/08       rp     On log-write failures, do not update loghead in super-blk
05/08/08       rp     Handle write-failure in log-region.
03/12/08       yg     Handle unknown page status in GC until root cause fixed.
02/06/08       np     Fix a possible invalid loghead update in the superblock.
01/15/08       yg     Remove break after return since it cannot be reached.
12/28/07       yg     Update some comments and improved readability.
11/01/07       np     Changes to support EFS on min config of 4 blocks on NOR
12/05/07       rp     Moved FS_PARANOIA_CHECKS feature to fs_paranoia.c
12/10/07       yg     Update some comments and rename allocators.
10/29/07       yg     Mark block bad on write error in first page of the block.
10/18/07       rp     Featurized QEFS2.
10/08/07       yg     Configure GC thresholds and bad block limits runtime.
10/04/07       yg     Fixed bad block handling code.
09/20/07      sch     Flush superblock to second block of log region at startup
09/19/07      sch     Used FS_LOG_TOP_OFF_LIMIT instead of hardcoded 8
09/19/07       yg     Changed NULL Tag name.
09/18/07       sh     Minor cleanup.
09/17/07      sch     Added exit conditions for the while loop in fs_pm_init
08/31/07       yg     Added some code comments.
08/24/07       yg     Soft limit GC overhead writes to configured value.
08/14/07      umr     Added check for superblock_data size to be of page size.
08/10/07       rp     Align buffers passed to flash on a 32-byte boundary.
08/08/07       yg     Corrected missing tag in logs while doing GC.
07/06/07       yg     Added Dual allocators to improve write efficiency.
06/29/07      sch     Passed in data tag to fs_pm_page_write
06/27/07       yg     Alloc gets erased blk from list, instead of searching.
06/25/07       yg     CB during counting to findout if cluster is in use.
05/25/07      umr     Added support for EFS memory protection feature.
04/23/07       yg     Simplified startup free span search algorithm.
03/06/07      dlb     Query callback about allocated counts.
02/23/07      dlb     Move read failures into device wrappers.
02/21/07       sh     Correct the comments about NAND reserved space.
12/13/06      dlb     Assert that transactions are used properly.
12/05/06      dlb     Max transaction size configurable.
11/09/06      dlb     Add pm query of freemap.
09/20/06       sh     Only resume the erases that were suspended in read.
09/20/06      dlb     Lint cleanup.
09/14/06       sh     Do not leave NOR suspended after a read.
09/11/06      dlb     Use flash driver wrappers.
08/23/06      dlb     Add code to log all reverse changes at startup.
08/18/06       yg     Fixed Compiler warning for NOR targets.
07/03/06       sh     Moved dog & lcd_message prototypes to fs_pm.c
26/06/06       yg     Memory reduction effort
06/13/06      dlb     Clean up revision history a bit.
05/07/06       sh     Fixed RVCT compiler warnings
01/13/06      dlb     Remove extra test from unit test.
12/14/05      dlb     Handle NOR partial writes on bad powerdown.
10/30/05       sh     Lint cleanup.
11/03/05      dlb     Proper page ordering for large NAND.
08/24/05       sh     Allow 50ms per erase work unit instead of 20ms.
05/26/05      sd      Compilation fixes for L4.
01/04/05      dlb     Update copyright line.
12/30/04      dlb     Remove excess infiltration of factory image code.
10/15/04      dlb     Update copyright line.
10/07/04      dlb     Whitespace cleanup.
08/18/04      dlb     Faster initial start on NOR.
07/09/04      drh     Change "assert" to "ASSERT"
05/20/04      dlb     Restart page manager after factory start.
05/05/04      dlb     Handle page write failures in data region.
04/20/04      dlb     Fix problem with startups and no transactions.
01/14/04      adm     Added free count setting for ptable_gc in case of factory
                      start.
12/11/03      dlb     Enable urgent GC on NOR flash.
11/13/03      dlb     Increase space utilization.
10/17/03       gr     Moved a couple of functions out to fs_util.c.
06/17/03      bgc     Added garbage logging of garbaged incomplete
                      transactions from previous bad powerdown.
06/17/03      dlb     Fixed erase failure handling code.
06/16/03      dlb     Formatting cleanup.
06/15/03       gr     Added support for recovery from erase failures.
06/12/03      adm     Add support for factory start.
05/28/03      bgc     Removed clipping of unfinished work to a block.
                      Cleaned up fs_pm_single().
05/22/03      dlb     Remove an extraneous log flush.
05/15/03      jkl     Return 0 if clock time become negative or too large.
04/24/03      bgc     Added calls to ptable->begin_erase() to invalidate
                      the ptable during the erase.  All pages will be
                      considered garbage until ptable->erase() is called
                      to mark the block as empty and valid.
04/07/03       gr     Added space limit computation for NAND devices.

   Somewhere here, the dates were in a different format.
!03/04/04      bgc     Added support for incremental garbage collection for
!                      NOR devices.  Erases are left running between
!                      operations.  Suspend and resume are used to spread out
!                      erases over several EFS transactions.
!03/03/07      dlb     Add support for NAND.
!03/02/25      bgc     Added more fixes for fs_pm_log_visit_node().
!03/02/25      bgc     Removed dealloc effects in fs_pm_log_visit_node() to
!                      prevent gc_next from skipping to anywhere in the flash.
!03/02/13      dlb     Created from fs_pm_gc.c. Improved bad power down.
===========================================================================*/

#include "fs_device.h"

#ifdef FEATURE_EFS_QEFS2
#if defined (FEATURE_EFS_EFS2_ON_NAND) ||   \
    defined (FEATURE_EFS_EFS2_ON_NOR)  ||   \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)

#include <string.h>

#include "assert.h"

#include "fs_sys_types.h"
#include "fs_err.h"
#include "fs_pm.h"
#include "fs_buffer.h"
#include "fs_util.h"
#include "qw.h"
#include "fs_protected_memory.h"
#include "fs_errno.h"
#include "fs_malloc.h"
/* Need upgrade info whether we have to upgrade from older EFS
 * image in flash */
#include "fs_upgrade.h"

static void cleanup_logs (fs_pm_flash_t gc);

#define DECREMENT_PAGE(gc, pvar) \
  do { \
    if ((pvar) == (gc)->min_page) \
      (pvar) = (gc)->max_page; \
    (pvar)--; \
  } while (0)

#define DECREMENT_BLOCK(gc, pvar) \
  do { \
    (pvar) = (pvar) & (gc)->ptable->super.block_mask;  \
    if ((pvar) == (gc)->min_page) \
      (pvar) = (gc)->max_page; \
    (pvar) = (pvar) - (gc)->ptable->super.data->block_size; \
  } while (0)

#define BUMP_PAGE(gc, pvar) \
  do { \
    (pvar)++; \
    if ((pvar) >= (gc)->max_page) \
      (pvar) = (gc)->min_page; \
  } while (0)

#define BUMP_BLOCK(gc, pvar) \
  do { \
    (pvar) = ((pvar) & (gc)->ptable->super.block_mask) + \
      (gc)->ptable->super.data->block_size; \
    if ((pvar) >= (gc)->max_page) \
      (pvar) = (gc)->min_page; \
  } while (0)

#define BUMP_ALLOCATOR_PAGE(gc, pvar) \
  do {  \
    BUMP_PAGE(gc, pvar);  \
    if ((pvar & (gc->ptable->super.data->block_size - 1)) == 0) \
      pvar = fs_pm_get_erased_blk (gc); \
  } while (0)

/* Get the block-number of the given page. */
#define BLK_NUM(pg) ((pg) >> gc->ptable->super.block_shift)

/*
 * This is the minimum amount of time that we will wait for a NOR
 * Erase operation to proceed before suspending it.
 * Really, this should be defined by the particular NOR flash, but
 * it is thought to be sufficient for all parts.
 * This value was previously 20, which proved too small for AMD NOR
 * parts, which only make progress in 10ms increments.
 */
#define FS_NOR_ERASE_INCREMENT 50

/*
 * This is the maximum number of erased blocks i.e. (free space) between
 * GC-ptr and Alloc-ptr.
 * If this much free space is available then there is no need to do further
 * GC. Garbage collection (GC) invloves page movement and hence by doing the
 * extra unnecessary GC, overall read or write throughput degrades.
 *
 * If the free space is less then FS_NOR_MAX_ERASED_BLOCKS limit then
 * incremental GC is being performed, which is non-blocking in nature.
 *
 * If free space become less then
 *   (2 * Block_size + FS_MAX_TRANSACTION_DATA_PAGES)
 * then urgent garbage collection would need to be performed, which is
 * blocking erase in nature.
 */
#define FS_NOR_MAX_ERASED_BLOCKS 5

#define FS_PM_MAX_TRANSACTION_PAGES   (FS_MAX_TRANSACTION_DATA_PAGES + 5)

/* Calculation for max number of GC Move log entries that can be packed in a
 * log page is as follows:
 *
 * Number of bytes available in a log page for log entries:
 * PAGE_SIZE - HEADERS_SIZE - LOG_PAD => PAGE_SIZE - 8 - 4
 * Here LOG_PAD Includes the CRC and the log end marker
 *
 * So, for 512 page config (which is default for NOR) we can have maximum of
 * (512 - 8 - 4) / GC_MOV_LOG_SIZE => 500 / 13 = 38
 *
 * So there could be 38 GC_MOVE log entries in a log page.
 *
 * When doing GC in the worst case, if we have to move 38 pages while doing
 * urgent garbage collection, log flush will happen after 38 pages are written
 * if power fails after 38 pages move and before log is flushed, all those 38
 * pages will become garbage and hence marked as garbage in the next powerup
 * init. This number would decide on how many pages extra free space we need
 * to maintain in addition to 1 block of free space. So that in the event of a
 * bad power down, we should still be able to recover and proceed doing GC to
 * reclaim free space.
 *
 * Since urgent garbage collection happens before the transaction is written
 * there is still a chance that the urgent garbage collection is not triggered
 * and the transaction consumes max consumable space and on the power up
 * urgent GC we should still be able to recover the bad power down for that we
 * need to add the maximum transaction consumption number of pages for the
 * urgency free count overhead.
 *
 * So having 38 will recover from 1 bad powerdown, consequent similar bad
 * powerdown will result into a situation where we don't have enough freespace
 * to move good pages to do GC which would reclaim freespace, thus crashing.
 * This number would really be a tradeoff between how small a file system we
 * want to be to how much safer we want to be against consequent bad
 * powerdowns in the worst case, even though because of incremental garbage
 * collection its very *unlikely* to get into this situation that easily.
 * Currently in very rare scenarios in full EFS conditions, EFS might lockup
 * because of less free space to do GC which recovers free space. It will be
 * addressed in the coming up release.
 */
#define FS_PM_NOR_URGENCY_FREE_CNT_OVERHEAD (38 + FS_PM_MAX_TRANSACTION_PAGES)


/* How many max consecutive log-page-write failures we can handle. */
static int fs_pm_log_fail_recurse_cnt = 0;

/* Functions that manage the circular array to put and get the erased
 * blocks. */
static int fs_pm_put_erased_blk (fs_page_id blk_page);
static fs_page_id fs_pm_get_erased_blk (fs_pm_flash_t gc);
static uint32 fs_pm_get_erased_blk_count (void);

/* Our static instance of circular array for storing the erased blocks */
static erased_blk_array_type erased_blk_list;

static void fs_pm_init_read_only_ops (struct fs_pm_ops *);
static void fs_pm_init_ops (struct fs_pm_ops *);
static void
fs_pm_log_visit_node (void *vgc, fs_log_code_t code, uint32 *args);

static void fs_pm_inc_gc_start_op (fs_pm_flash_t gc);
static void fs_pm_inc_gc_end_op (fs_pm_flash_t gc);
static void fs_pm_inc_gc_erase (fs_pm_flash_t gc);
static void fs_pm_suspend_erase(fs_pm_flash_t gc, boolean do_work );
static void fs_pm_do_urgent (fs_pm_flash_t gc, boolean allow_gc);
static int fs_pm_top_off_log(fs_pm_flash_t gc);
static void fs_pm_bump_gc_block (fs_pm_flash_t gc);
static void fs_pm_gc_update_nofity (fs_pm_flash_t gc);

uint32 fs_pm_space_limit (fs_pm_t gen_gc);
#ifdef FEATURE_EFS_EFS2_ON_NAND
static void
fs_pm_log_visit_nand_node (void *vgc, fs_log_code_t code, uint32 *args);
static int nand_ptable_do_gc (fs_pm_flash_t gc);

enum update_loghead_type
{
  IGNORE_LOG_HEAD,
  UPDATE_LOG_HEAD
};
static void fs_pm_log_data_write_super_page (fs_pm_flash_t gc,
              enum update_loghead_type update_loghead);
#endif

#if defined (FEATURE_EFS_EFS2_ON_NOR) ||      \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)
void fs_ptable_init_nor (
    fs_ptable_t         pt,
    fs_log_t            log,
    int                *fresh_start);
#endif

#ifdef FS_UNIT_TEST
void fs_pm_validate (fs_pm_t gen_gc);
#endif

/* PM internal data allocation tag type. */
enum data_alloc_tag_type
{
  SOLID_DATA_ALLOC_TAG = 0,
  LIQUID_DATA_ALLOC_TAG = 1
};

/**********************************************************************
 * XXX: May want to support more than one ptable instance eventually.
 * Currently a single union type allows either type of ptable to be used
 * below.  To allow more than one instance, the ptable init should allocate
 * the appropriate type, probably from a pool with compile-time decided
 * sizes. */
static union fs_ptable_combined_data ptable_pool[1];

/* An upper bound on the time it takes (in milliseconds) for initialization on
 * a fresh start.
 */
#define FS_FRESH_START_INIT_TIME 160000

/* The buffers used for ptables have to be aligned and their size depends on
 * device page size. Because ARM compiler can only align variable instances
 * (not definitions) and the buffer size varies we cannot define buffers
 * directly in the structure. Thus we have made the buffers as pointers inside
 * the structure definitions and assign an aligned buffer to them. In this
 * function we hook-up the pointers with their corresponding aligned array. */
static void
fs_ptable_init_aligned_buff (uint32 page_size)
{
  int i;

  /* Hook-up the aligned-buffers with their corresponding pointers, and make
   * sure that the buffers are aligned as per DMA's expectations. */
  for (i=0; i<FS_MAX_CACHED_PTABLE_PAGES; ++i) {
    ptable_pool[0].nand.pcache_data[i] = fs_align_malloc (page_size);
    ASSERT (FS_OKAY_FOR_DM (ptable_pool[0].nand.pcache_data[i]));
  }

  ptable_pool[0].nand.ptable_gc.realloc_buffer = fs_align_malloc (page_size);
  ASSERT (FS_OKAY_FOR_DM (ptable_pool[0].nand.ptable_gc.realloc_buffer));

  ptable_pool[0].nand.parent.super.data = fs_align_malloc (page_size);
  ASSERT (FS_OKAY_FOR_DM (ptable_pool[0].nand.parent.super.data));
}


int
fs_ptable_read_only_init (
    fs_ptable_t         pt,
    efs_device_t        dev,
    fs_log_t            log,
    int                *fresh_start,
    fs_logr_t           logr)
{
  uint32 page_size;
  int result = 0;

  /* Initialize to zero ptable data */
  memset (ptable_pool, 0, sizeof (ptable_pool));

  pt->dev = dev;
  pt->logr = logr;

  page_size = fs_device_page_size (dev);
  fs_ptable_init_aligned_buff (page_size);

  result = fs_super_read_only_init (&pt->super, dev, fresh_start);
  if (result != 0)
    return -1;

  if (FS_ISNOR (&pt->super)){
    /* Read only mode for NOR is currently not supported and it has not been
     * tested. */
    result = -1;
  }
  else{
#ifdef FEATURE_EFS_EFS2_ON_NAND
    fs_ptable_read_only_init_nand (pt, log, fresh_start);
#endif
  }
  return result;
}

void
fs_ptable_init (
    fs_ptable_t         pt,
    efs_device_t        dev,
    fs_log_t            log,
    int                *fresh_start,
    fs_logr_t           logr)
{
  uint32 page_size;

  /* Initialize to zero ptable data */
  memset (ptable_pool, 0, sizeof (ptable_pool));

  pt->dev = dev;
  pt->logr = logr;

  page_size = fs_device_page_size (dev);
  fs_ptable_init_aligned_buff (page_size);

  fs_super_init (&pt->super, dev, fresh_start);

  if (FS_ISNOR (&pt->super)){
#if defined (FEATURE_EFS_EFS2_ON_NOR)      ||   \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)
    fs_ptable_init_nor (pt, log, fresh_start);
#endif
  }
  else{
#ifdef FEATURE_EFS_EFS2_ON_NAND
    fs_ptable_init_nand (pt, log, fresh_start);
#endif
  }
}

static uint32
get_bad_block_allowance (uint32 region_size, uint32 block_size)
{
  uint32 bad_block_allowance;

  /* Check how many blocks we can take out of this region. */
  bad_block_allowance = ((region_size * MAX_BAD_BLOCK_PERCENT_ALLOWED) / 100);

  /* Roundup to consume whole block. Favour small configurations. */
  bad_block_allowance = (bad_block_allowance | (block_size - 1)) + 1;

  /* See if we met our minimum bad block allowance, if not bump it up */
  if (bad_block_allowance < (FS_MIN_DATA_BAD_BLOCK_ALLOWED * block_size))
    bad_block_allowance = (FS_MIN_DATA_BAD_BLOCK_ALLOWED * block_size);

  return bad_block_allowance;
}

/*
 * Calculate the limits of GC Thresholds and Bad block margins.
 *
 * Min_Threshold and Max_Threshold are Thresholds of physically available free
 * pages that are the key factors used to make a decision whether to run GC or
 * not at any instance. Other factors affecting the decision is the build time
 * configuration setting FS_PM_GC_OVERHEAD_WRITES_PER_TXN which specifies how
 * many overhead extra page writes can be done during GC per transaction. This
 * being a softlimit is obeyed only if it is possible. If the free space goes
 * below critical limit then it is ignored.
 *
 * The algorithm for decision is described in function fs_pm_urgency.
 *
 * Bad block margin is how many blocks can go bad, and still EFS should be
 * able to work properly with EFS 100% full and still obey the above limits
 * while doing GC giving reasonable performance.
 *
 * Since we need some operating margin for GC, here is how the data region
 * is logically divided:
 *
 * Data_region = space_limit + Min_Threshold + Bad_Blocks + Max_Threshold.
 *
 * Since space_limit is calculated just once during fresh start, we would
 * take the value whatever that was configured. So rest of the three values
 * need to be calculated, the calculation should result correct usable numbers
 * for the complete wide range configuration size of EFS configurations.
 * Typically the range of Data region is from 32 blocks to more than 2048
 * blocks. Since the threshold values cannot be varied too much in range
 * will have to take some consideration of the size of the region to
 * get optimal values for them. Bad block percent is configurable by the
 * customers, but will have to make sure that it wouldn't lead to 0 bad blocks
 * Since manufacturer specifies sometimes that the device may contain 4% bad
 * blocks already when the device ships from factory and additional bad blocks
 * may develop over usage.
 *
 * Here are the typical range of values and the relation:
 *
 * space_limit = 95% of Data_region - 5 blks; (Existing/old Calculation)
 * Bad_Blocks = Bad Block% of Data_region; (Configurable)
 *
 * Min_Threshold = [3 to 5]; // depending on the Data_region size.
 * Max_Threshold = [Min_Threshold + 1 to 9];
 *
 * Because of this equation, if the region develops bad blocks more than
 * allowed by the preset config, it would cause fatal error, since our
 * GC cannot yield to bad blocks out of its operating margin.
 *
 * These following calculations are needed for GC to run, so for first
 * time GC also, these calculations are needed, so calculate before running the
 * GC for the first time. */

static void
calculate_gc_threshold_limits (fs_pm_flash_t gc)
{
  uint32 data_region_size, data_space_limit, overhead_limit;
  uint32 bad_blk_limit, remaining, block_size;

  /* Get the region dimensions for calculation */
  data_region_size = gc->max_page - gc->min_page;
  data_space_limit = gc->ptable->super.data->upper_data[FS_FIELD_SPACE_LIMIT];

  /* On fresh start we would not have the space limit in upper data. */
  if ((data_space_limit == 0) || (data_space_limit > data_region_size))
    data_space_limit = fs_pm_space_limit (&gc->parent);

  overhead_limit = data_region_size - data_space_limit;

  block_size = gc->ptable->super.data->block_size;

  /* Check max how many blocks could go bad (in units of pages) */
  bad_blk_limit = get_bad_block_allowance (data_region_size, block_size);

  /* Convert page count to block count */
  gc->max_bad_blocks_allowed = bad_blk_limit / block_size;

//  printf ("Data can handle %d bad blks\n", gc->max_bad_blocks_allowed);

  /* We need to have minimum required free space for GC */
  gc->gc_min_threshold = FS_PM_MINIMUM_FREE_BLKS_FOR_GC * block_size;

  /* Bump the threshold if we have enough room.
   * Threshold can jump again based on another step of available count
   * and get to 5. (say if overhead limit crosses 100 blocks) */
  if (overhead_limit > (30 * block_size))
    gc->gc_min_threshold += block_size;

  /* Our overhead_limit should be bigger than needed limits */
  if (overhead_limit < (gc->gc_min_threshold + bad_blk_limit))
  {
    FS_ERR_FATAL ("Overhead limit is smaller than thresholds: %d  %d  %d\n",
                    overhead_limit, gc->gc_min_threshold, bad_blk_limit);
  }

  /* Check if we have enough space remaining after all the maximum
   * possible blocks go bad. */
  remaining = overhead_limit - (gc->gc_min_threshold + bad_blk_limit);

  /* Consume around half of remaining */
  gc->gc_max_threshold = (remaining + 1) / 2;

  /* But limit the threshold difference to 4 blks, anything over 4 blks will
   * start moving too many pages overall on average during GC. */
  if (gc->gc_max_threshold > (4 * block_size))
    gc->gc_max_threshold = (4 * block_size);

  /* Since our max threshold number was relative to Min threshold, now
   * make it absolute. */
  gc->gc_max_threshold += gc->gc_min_threshold;

//  printf ("Min Threshold : %d Max Threshold %d\n",
//                        gc->gc_min_threshold, gc->gc_max_threshold);

  /* Check if the erased block array can hold these many free blocks. */
  if ((gc->gc_max_threshold / block_size) >= MAX_ERASED_BLOCK_ARRAY_SIZE)
    FS_ERR_FATAL ("Erased blocks array size not sufficient", 0, 0, 0);

  /* Now we have the thresholds in the units of pages and bad block limit
   * in the units of blocks to use. */

}

/* Search for the erased blocks from a given starting block and queue these
 * into erased block list. If the region of search has more erased blocks than
 * the queue can hold, it stops searching. */
static void
fs_pm_search_and_queue_erased_blocks (fs_pm_flash_t gc,
                                      fs_page_id start_block_page)
{
  fs_page_id current_page, stop_page;
  uint32 solid_blk, liquid_blk;

  current_page = start_block_page;
  stop_page = start_block_page;
  DECREMENT_BLOCK (gc, stop_page);

  /* Keep both allocators handy, so that if the allocator is already at the
   * erased block that we found, we should not add that into the array, that
   * would re assign the written block later. */
  liquid_blk = FS_INVALID_PAGE_ID;
  if (gc->liquid_data_alloc != FS_INVALID_PAGE_ID)
    liquid_blk = gc->liquid_data_alloc & gc->ptable->super.block_mask;

  solid_blk = FS_INVALID_PAGE_ID;
  if (gc->solid_data_alloc != FS_INVALID_PAGE_ID)
    solid_blk = gc->solid_data_alloc & gc->ptable->super.block_mask;

  /* Find and fill our erased block array */
  while (current_page != stop_page)
  {
    if (gc->ptable->get_reverse (gc->ptable, current_page, NULL)
        == FS_RMAP_BLOCK_ERASED)
    {
      /* If we assigned this block to some allocator already
       * then don't add this to the erased block list. */
      if ((current_page != liquid_blk) && (current_page != solid_blk))
      {
        /* Try to put the erased_blk in the list. If we filled the last
         * available slot, it returns FALSE indicating that we cannot
         * queue anymore blocks.
         */
        if (!fs_pm_put_erased_blk (current_page))
          break;
      }
    }
    BUMP_BLOCK (gc, current_page);
  }

  /* Now we added all the erased blocks into the erased block array */
}


/* Nand PM allocates and writes pages in pure sequential order. This can
   help verify quickly if a block is erased by just looking a few pages in
   the block instead of checking if all pages of the block are erased. For
   robustness, function checks first five pages. */
static int
fs_pm_nand_is_block_erased (fs_pm_flash_t gc, fs_block_id block)
{
  fs_page_id page, stop_page;
  uint32 block_size, pages_to_check = 5;

  block_size = gc->dev->block_size (gc->dev);
  if (pages_to_check > block_size)
    pages_to_check = block_size;

  page = block << gc->ptable->super.block_shift;
  stop_page = page + 5;

  for (; page < stop_page; page++)
  {
    if (!fs_device_is_erased (gc->dev, page))
      return 0;
  }

  return 1;
}


/* Search for the erased blocks from a given starting block until gc_next
 * and force an erase on them. This is to handle the corner case where
 * the file system is moving pages due to GC and power down happens.
 *
 * The scenario is:
 * 1. The file system has to create space, it starts moving pages (GC)
 * 2. When writing the page, the system draws more power.
 * 3. The extra power is not handled by the power supply.
 * 4. The power supply shuts down.
 * 5. The moved page is either written or half written.
 * 6. The power supply has enough power to boot up the system.
 * 7. System starts to boot up, goes again to step 1.
 *
 * After several iterations of the steps above, if the blocks are not
 * erased then the EFS would run out of free pages.
 *
 * Erasing an erased blocks will:
 * - NOT wear out the block.
 * - Take the same amount of power and time as erasing a dirty block.
 * - The first writes to a block that has been erased multiple times
 *   will be slower than the writes to a block that has been erased
 *   once.
 */
static void
fs_pm_search_and_erase_blocks (fs_pm_flash_t gc,
                               fs_page_id start_block_page)
{
  fs_page_id current_page, stop_page;
  uint32 args[3];

  /* Traverse from the start_block_page to GC Next. This is where all the
   * erased blocks would be present. */
  current_page = start_block_page;

  /* Start from current_page, end at gc_next */
  stop_page = gc->gc_next & gc->ptable->super.block_mask;

  while (current_page != stop_page)
  {
    if (gc->ptable->get_reverse (gc->ptable, current_page, NULL)
        == FS_RMAP_BLOCK_ERASED)
    {
      int result, is_erased_on_disk;
      fs_block_id block = BLK_NUM(current_page);

      /* Ptable says block is erased. Check if block is actually erased on
         disk. */
      is_erased_on_disk = fs_pm_nand_is_block_erased (gc, block);

    #ifdef FEATURE_EFS_FORCE_ERASE_BLOCKS_AT_INIT
      is_erased_on_disk = 0;
    #endif

      /* Block not actually erased on disk, so erase it. */
      if (!is_erased_on_disk)
      {
        result = fs_device_erase_block (gc->dev, block,
                                        FS_FOP_DATA_GC_RECOVERY);
        if (result != FS_DEVICE_OK)
        {
          (void) fs_device_mark_block_bad (gc->dev, block);
          args[0] = block;
          fs_log_add (&gc->log, FS_LOG_ENTRY_ERASE_FAIL, args);
          gc->ptable->set_reverse (gc->ptable,
                                   block << gc->ptable->super.block_shift,
                                   FS_RMAP_BLOCK_BAD, FS_PM_NULL_TAG);
        }
      }
    }
    BUMP_BLOCK (gc, current_page);
  }

  /* At this point all blocks that are marked as erased in the page table
   * are guanranteed to be erased in the device. Recovering the lost space
   * from a previous bad power down.
   * fs_pm_init() must do a log flush to guarantee the bad blocks are
   * eventually recorded. */
}

static void
fs_pm_restore_erased_blocks (fs_pm_flash_t gc)
{
  fs_page_id temp_page;
  uint32 solid_blk, liquid_blk;

  /* Try to restore our erased block array. Traverse from the farthest
   * allocator to GC Next which is the place where all the erased blocks
   * would be. Farthest is chosen because if just one allocator is moving
   * then potentially the other allocator could be very close to the gc_next
   * and we might not find any erased blocks at all, even if present.
   * If the allocator is invalid then we can rely on the last written page
   * for the allocator that was logged or known good. */
  liquid_blk = FS_INVALID_PAGE_ID;
  if (gc->liquid_data_alloc != FS_INVALID_PAGE_ID)
    liquid_blk = gc->liquid_data_alloc & gc->ptable->super.block_mask;
  else
    if (gc->known_good_liquid_alloc != FS_INVALID_PAGE_ID)
      liquid_blk = gc->known_good_liquid_alloc & gc->ptable->super.block_mask;

  solid_blk = FS_INVALID_PAGE_ID;
  if (gc->solid_data_alloc != FS_INVALID_PAGE_ID)
    solid_blk = gc->solid_data_alloc & gc->ptable->super.block_mask;
  else
    if (gc->known_good_solid_alloc != FS_INVALID_PAGE_ID)
      solid_blk = gc->known_good_solid_alloc & gc->ptable->super.block_mask;

  /* Start from gc_next until we see both the allocators. */
  temp_page = gc->gc_next & gc->ptable->super.block_mask;

  if ((solid_blk == FS_INVALID_PAGE_ID) && (liquid_blk == FS_INVALID_PAGE_ID))
  {
    /* Both the pointers are invalid then make a full circle to search for
     * the erased pages. Since we are just reading the reverse page table it
     * would not be many reads from the PT. */
    BUMP_BLOCK (gc, temp_page);
  }
  else
  {
    /* Loop temp_page until we hit both the allocators. If we met both the
     * allocators then that would be the farthest point from gc_next */
    while (1)
    {
      if (temp_page == solid_blk)
        solid_blk = FS_INVALID_PAGE_ID;

      if (temp_page == liquid_blk)
        liquid_blk = FS_INVALID_PAGE_ID;

      /* If we saw both the allocators, then its good time to exit from this
       * loop before another decrement happens. Since if the farthest allocator
       * and the GC are just beside each other, then potentially we might not
       * loop at all, since if we decrement one more time, the start block is
       * already at the ending block. Also starting our erased block search
       * from gc_next could lead to other corner problems of block not written
       * ever but has erased status just before starting GC on that block, (so
       * cannot use do..while to search through the entire device also has
       * problems). */
      if ((solid_blk == FS_INVALID_PAGE_ID) &&
          (liquid_blk == FS_INVALID_PAGE_ID))
        break;

      DECREMENT_BLOCK (gc, temp_page);
    }
  }

  /* Search the blocks that are marked as erased in the page tables and erase
   * them. This is to gain back the written pages in these blocks that were
   * dirtied before the power down. */
  fs_pm_search_and_erase_blocks (gc, temp_page);

  /* Now we have temp_page pointing to the farthest allocator from GC next,
   * Lets search and queue erased blocks starting from temp_page or the
   * farthest allocator. */
  fs_pm_search_and_queue_erased_blocks (gc, temp_page);

}

/* If our allocators are pointing to a valid page, make sure it
 * really points to an erased page. A bad power down before flushing
 * log could have written lot of pages which we cannot use and didn't
 * detect so far because of no log. So need to mark them garbage and
 * move the pointers appropriately to erased page or if needed to a new
 * erased block. */
static fs_page_id
reposition_allocator (fs_pm_flash_t gc, fs_page_id data_allocator)
{
  uint32 args[3];

  if (data_allocator == FS_INVALID_PAGE_ID)
  {
    /* If we got any free blocks, then assign that and proceed to
     * test, otherwise just return FS_INVALID_PAGE_ID itself, so that
     * the data allocators gets the free block when GC erases one.
     * This scenario happens while unpacking a factory image. */
    if (fs_pm_get_erased_blk_count () > 0)
      data_allocator = fs_pm_get_erased_blk (gc);
    else
      /* We don't have any erased blocks in erased block list, so
       * bail out later GC will assign an erased block to the allocator */
      return FS_INVALID_PAGE_ID;
  }

  while (!fs_device_is_erased (gc->dev, data_allocator)){

    /* Since we don't just bump the page blindly while replaying logs
     * we should see that the page is erased, since if we cross block
     * boundary we allocate a block from the erased block list. which
     * is exactly what happens when phone is running. So we can safely
     * mark the page garbage and if we again cross boundary we again
     * take block from the erased block list. */
    gc->ptable->set_reverse (gc->ptable, data_allocator,
        FS_RMAP_PAGE_GARBAGE, FS_PM_NULL_TAG);
    args[0] = data_allocator;
    /* Log it so that it helps during next time power up init
     * and the correct way of changing any page's status to handle
     * bad power down. */
    fs_log_add (&gc->log, FS_LOG_ENTRY_GARBAGE, args);

    BUMP_PAGE (gc, data_allocator);

    if ((data_allocator & ~gc->ptable->super.block_mask) == 0)
    {
      /* Even though we are taking block out of erased block list,
       * the block was marked erased in the reverse page table, not
       * sure if it was really erased on the flash, since a bad
       * power down could have written some data, so its ok to
       * lose a page in a block, but we would have the EFS in sane
       * state always. */
      data_allocator = fs_pm_get_erased_blk (gc);
    }
  }

  /* If we moved the pointer, we might be in a situation where
   * the page is not marked as erased. Since in the above code we
   * make sure that the page is erased physically but might not
   * have marked erased in the rev page table. So mark it now. */
  gc->ptable->set_reverse (gc->ptable, data_allocator,
                           FS_RMAP_BLOCK_ERASED, FS_PM_NULL_TAG);

  /* Now the allocator is pointing to a page that is really erased
   * and marked erased in the rev page table. */
  return data_allocator;
}

/* Handle write-failure in the log region. */
static void
fs_pm_log_page_write_fail_handler (void *priv_data, fs_page_id failed_pg)
{
#ifdef FEATURE_EFS_EFS2_ON_NAND
  fs_block_id failed_blk;
  fs_pm_flash_t gc = (fs_pm_flash_t )priv_data;;

  /* In this function, while handling a log-page write-failure, we flush
   * the super-block to record the bad-block in logr_badmap, Now this flushing
   * of super-block can fail and we will recurse back to this function again,
   * So we should limit the number of times to recurse, when the consecutive
   * writes fail in the log-region. */
  /* -1 because, Remember one attempt already failed and that's why we
   * are here.
   */
  if (fs_pm_log_fail_recurse_cnt++ > FS_MAX_CONSECUTIVE_LOG_WRITE_FAIL-1)
  {
    /* Too many consecutive writes failed in the log-region. */
    FS_ERR_FATAL ("Too many bad blocks in log region...",0,0,0);
  }

  ASSERT (gc != NULL);

  /* see which block went bad. */
  failed_blk = failed_pg >> gc->ptable->super.block_shift;

  /* Tell Log-Region to mark this block as bad, only in the RAM-copy badmap.*/
  fs_logr_handle_bad_block (&gc->logr, failed_blk);

  /* Now adjust the LOG queue to discard all the unused pages that were
   * allocated from this bad block. Allocated, but unused log-pages are the
   * pages that are between [flush] and [alloc].
   */
  fs_log_remove_bad_pages (&gc->log, gc->ptable->super.block_shift,
                           failed_blk);

  /* Now allocate new pages to make up for the pages lost in this bad-block */
  fs_pm_top_off_log (gc);

  /* Now flush the super-block to record this bad-block, But do not update
   * the latest log-head now, we have to retain the old log-head until the
   * log-flush succeeds. */
  fs_pm_log_data_write_super_page (gc, IGNORE_LOG_HEAD);

  /* we are done with this bad block, lets be ready for the next one.
   * If we reached here, then we will not recurse back to this function, so
   * lets give the next bad-block its fresh chance to succeed. */
  fs_pm_log_fail_recurse_cnt = 0;
#else
  (void) priv_data;
  (void) failed_pg;
  (void) fs_pm_log_fail_recurse_cnt;
  FS_ERR_FATAL ("no log-write-fail handler support!!!",0,0,0);
#endif /* FEATURE_EFS_EFS2_ON_NAND */
}

static void
fs_pm_log_page_write_handler (void *priv_data, uint8 *log_buffer)
{
#ifdef FEATURE_EFS_EFS2_ON_NAND
  fs_page_id log_page;
  int max_retry_cnt = 0;
  int result = FS_DEVICE_FAIL;
  fs_pm_flash_t gc = (fs_pm_flash_t )priv_data;;

  log_page = fs_log_peek_page (&gc->log);
  while (max_retry_cnt++ <= FS_MAX_CONSECUTIVE_LOG_WRITE_FAIL)
  {
    /*printf ("log-write : %d,%d\n",log_page>>gc->ptable->super.block_shift,
            (log_page & (~gc->ptable->super.block_mask)));*/
    result = fs_device_write_page (gc->dev, log_page, log_buffer, FS_FOP_LOG);
    if (result == FS_DEVICE_OK)
      break;

    fs_pm_log_page_write_fail_handler (priv_data, log_page);
    log_page = fs_log_peek_page (&gc->log);
  }
  if (result != FS_DEVICE_OK)
  {
    /* Too many attempts failed to write the log-page. */
    FS_ERR_FATAL ("Too many bad blocks in log region...",0,0,0);
  }

  /* Write super page now if we just now wrote the last page of the block */
  if ( (log_page & ~gc->ptable->super.block_mask) ==
        (~gc->ptable->super.block_mask) )
  {
    fs_pm_log_data_write_super_page (gc, UPDATE_LOG_HEAD);
  }

#else
  (void) priv_data;
  (void) log_buffer;
  (void) fs_pm_log_page_write_fail_handler;
  FS_ERR_FATAL ("no log-write-fail handler support!!!",0,0,0);
#endif /* FEATURE_EFS_EFS2_ON_NAND */
}

static void
fs_pm_log_invalid_page_write_handler (void *priv_data, uint8 *log_buffer)
{
  (void) priv_data; (void) log_buffer;
}

static void
fs_pm_data_initialize (fs_pm_flash_t  gc, efs_device_t dev)
{
  gc->dev = dev;

  gc->super_head = 0;
  gc->super_tail = 0;
  gc->inside_xact = 0;

  gc->is_used_cb = NULL;
  gc->is_used_data = NULL;
  gc->alloc_count_cb = NULL;
  gc->alloc_count_data = NULL;

  /* Incremental GC init */
  gc->erase_state = FS_PM_ERASE_IDLE;
  gc->unfinished_work = 0;
  gc->erase_factor = 60;
  gc->erase_time = 1000;

  /* Initialize the bad block counting variables. */
  gc->last_reference = gc->min_page;
  gc->bad_blk_count = 0;
  gc->bad_block_acc = 0;

  gc->next_super_page = FS_INVALID_PAGE_ID;

  gc->shred_stop = FS_INVALID_PAGE_ID;
  gc->shred_state = SHRED_IDLE;

  memset (&gc->scrub_info, 0x0, sizeof (struct fs_pm_scrub_info));
  gc->scrub_info.scrub_state = FS_SCRUB_IDLE;

  /* Initialize our circular array, both the enqueue and dequeue pointers
   * are at the same location with count zero. */
  erased_blk_list.dequeue_pos = 0;
  erased_blk_list.enqueue_pos = 0;
  erased_blk_list.item_count = 0;

  gc->ptable = &ptable_pool[0].nor.parent;

  fs_pm_log_fail_recurse_cnt = 0;
}

static void
fs_pm_nor_fresh_start (fs_pm_flash_t  gc)
{
  fs_block_id block;
  int result;

  /* Erase the first two blocks of the flash to give the GC enough
   * space to work with.  The rest of the blocks will be erased as
   * needed by the GC. */
  gc->free_count = 0;
  for (block = 0; block < 2; block++) {
    result = fs_device_erase_block (gc->dev, block, FS_FOP_USER);
    if (result != FS_DEVICE_OK) {
      FS_ERR_FATAL ("Unable to erase blocks to freshen filesystem",
          0, 0, 0);
    }
    gc->free_count += gc->ptable->erase (gc->ptable, block);
  }
  /* Solid data allocator in NOR would be pointing to an already erased
   * block which we just erased here */
  gc->solid_data_alloc = 0;

  gc->gc_next = 2 << gc->ptable->super.block_shift;
}

static void
fs_pm_nand_fresh_start (fs_pm_flash_t  gc)
{
  /* For NAND fresh start, will have both the alloc pointers have
   * invalid page ID, so that GC will fill in appropriate erased
   * block when it makes one available. */
  gc->solid_data_alloc = FS_INVALID_PAGE_ID;

  /* Just assign whatever be the values determined for the
   * allocators. */
  gc->known_good_solid_alloc = gc->solid_data_alloc;
  gc->known_good_liquid_alloc = gc->liquid_data_alloc;

  gc->gc_next = 0;
  gc->free_count = 0;
}

static void
fs_pm_nor_set_next_super_position (fs_pm_flash_t  gc,
                                   fs_page_id block_start_page)
{
  if (block_start_page == gc->solid_data_alloc)
    return;

  if (fs_device_is_erased (gc->dev, block_start_page))
  {
    fs_cluster_id status;

    status = gc->ptable->get_reverse(gc->ptable, block_start_page, NULL);

    if (status == FS_RMAP_PAGE_SUPER)
    {
      gc->next_super_page = block_start_page;
    }
    else if (status == FS_RMAP_BLOCK_ERASED)
    {
      gc->next_super_page = block_start_page;
      gc->ptable->set_reverse (gc->ptable, block_start_page,
                               FS_RMAP_PAGE_SUPER, FS_PM_NULL_TAG);
    }
    else
    {
      FS_ERR_FATAL ("%X,%d, Nor Super page status not good\n", status,
                    block_start_page, 0);
    }
  }
}


static void
fs_pm_nor_powerup_init (fs_pm_flash_t  gc)
{
  fs_page_id tmp_stop, page_state, ptmp, block_start_page;
  uint32 erased_page_count = 0, old_alloc_next;
  int32 scan_page_count;
  uint32 args[3];

  /* Set the GC state appropriately. */
  if ((gc->gc_next & ~gc->ptable->super.block_mask) == 0)
    gc->state = FS_PM_HEAD;
  else
    gc->state = FS_PM_MOVING;

  /* Stop when we reach the allocator */
  old_alloc_next = gc->solid_data_alloc;
  tmp_stop = gc->solid_data_alloc;

  /* We need to check the current alloc page also */
  DECREMENT_PAGE (gc, tmp_stop);

  /* Start from previous block of current GC block */
  ptmp = gc->gc_next & gc->ptable->super.block_mask;
  DECREMENT_PAGE (gc, ptmp);

  /* Check how many pages we might scan from GC to Alloc Next */
  scan_page_count = ptmp - tmp_stop;

  /* Fix the count if GC is in the beginning and
   * Alloc next is at the end */
  if (scan_page_count < 0)
    scan_page_count += gc->max_page;

  /* If we are about to scan more than 2 blocks then limit it to less than
   * 2 blocks. This will start from beginning of the next second block */
  if ((uint32)scan_page_count > (2 * gc->ptable->super.data->block_size))
  {
    ptmp = gc->solid_data_alloc;
    BUMP_BLOCK (gc, ptmp);
    BUMP_BLOCK (gc, ptmp);
  }

  /* printf ("Search from [%4d  to %4d] %4d  Pages\n",
            ptmp, tmp_stop, (ptmp > tmp_stop) ? (ptmp - tmp_stop) :
            (ptmp + gc->max_page - tmp_stop)); */

  /* Loop through and update alloc next to point to the last
   * erased page in the span */
  while (ptmp != tmp_stop)
  {
    page_state = gc->ptable->get_reverse (gc->ptable, ptmp, NULL);

    if (page_state == FS_RMAP_PAGE_RESERVED)
    {
      /* Page tables do not count towards space */
      DECREMENT_PAGE (gc, ptmp);
      continue;
    }

    if (page_state == FS_RMAP_BLOCK_ERASED &&
        fs_device_is_erased (gc->dev, ptmp))
    {
      ++erased_page_count;
      gc->solid_data_alloc = ptmp;
      DECREMENT_PAGE (gc, ptmp);
    }
    else
    {
      break;
    }
  } /* While we are still searching for the erase pages */

  if (erased_page_count == 0)
  {
    FS_ERR_FATAL ("GC did not find any erased pages on restart",
        0,0,0);
  }

  /* Now allocator points to where we really want to allocate new pages.
   * Go through from where our old allocator was after replaying the logs,
   * and mark any pages in between as garbage. */

  block_start_page = (gc->solid_data_alloc & gc->ptable->super.block_mask);

  while (old_alloc_next != gc->solid_data_alloc)
  {

    page_state = gc->ptable->get_reverse (gc->ptable, old_alloc_next,
                                          NULL);

    if ((old_alloc_next != block_start_page) &&
        (page_state == FS_RMAP_BLOCK_ERASED))
    {
      gc->ptable->set_reverse (gc->ptable, old_alloc_next,
                               FS_RMAP_PAGE_GARBAGE, FS_PM_NULL_TAG);

      /* Note that this log is happening before there are any log
       * pages.  This will fail if the single log buffer overflows.
       * This can happen with multiple consecutive bad-powerdowns. */
      args[0] = old_alloc_next;
      fs_log_add (&gc->log, FS_LOG_ENTRY_GARBAGE, args);
    }

    BUMP_PAGE (gc, old_alloc_next);
  }

  fs_pm_nor_set_next_super_position (gc, block_start_page);
}

static void
fs_pm_nand_powerup_init (fs_pm_flash_t  gc)
{                    /* NAND logic */

  fs_pm_top_off_log(gc);

#ifdef FEATURE_EFS_EFS2_ON_NAND
  {
    int log_written_cnt;
    /* Do not allow holes in the log-region:
     * If we got reset after writing last log-page in the block, then we
     * missed our opportunity to update the super-page, so update the
     * super-page now if we are about to the write to the second-page. */
    log_written_cnt = fs_log_written_count (&gc->log);
    if (log_written_cnt > 0)
    {
      if (gc->next_super_page != FS_INVALID_PAGE_ID)
      {
        fs_page_id log_page;
        log_page = fs_log_peek_page (&gc->log);
        if (((log_page & ~gc->ptable->super.block_mask) == 1) &&
            (BLK_NUM (log_page) == BLK_NUM (gc->next_super_page)))
        {
          fs_pm_log_data_write_super_page (gc, UPDATE_LOG_HEAD);
        }
      }
    }
  }
#endif /* FEATURE_EFS_EFS2_ON_NAND */


  /* If we are upgrading from previous version of the FS, then just
   * initialize the Liquid Data allocator to invalid page, PM will
   * handle it further to assign an erased block to it if it has
   * FS_INVALID_PAGE_ID while doing GC or during init, when we make
   * sure these pointers point to an erased page. */
  if (fs_upgrade_check (FS_UPGRADE_DUAL_ALLOC_FIX)) {
    gc->liquid_data_alloc = FS_INVALID_PAGE_ID;
    fs_upgrade_finished (FS_UPGRADE_DUAL_ALLOC_FIX);
  }

  /* Get our free blocks list filled up */
  fs_pm_restore_erased_blocks (gc);

  /* Here *NO* need to do sanity check to see if we got any erased blocks,
   * since for factory image there exists a condition where we would not
   * have any erased blocks, when GC runs for the first time, will create
   * some free blocks and assigns to the allocators. */

  /* printf ("Before AN : 0x%lX, MDAN : 0x%lX\n", gc->solid_data_alloc,
   *                                       gc->liquid_data_alloc); */

  gc->solid_data_alloc = reposition_allocator (gc, gc->solid_data_alloc);
  gc->liquid_data_alloc = reposition_allocator (gc, gc->liquid_data_alloc);

  /* printf ("After AN : 0x%lX, MDAN : 0x%lX\n", gc->solid_data_alloc,
   *                                       gc->liquid_data_alloc); */

  /* Set the GC state appropriately. */
  if ((gc->gc_next & ~gc->ptable->super.block_mask) == 0)
    gc->state = FS_PM_HEAD;
  else
    gc->state = FS_PM_MOVING;
}

static void
fs_pm_nor_compute_free_count (fs_pm_flash_t  gc)
{
  fs_page_id tmp_page, tmp_stop, page_state;

  gc->free_count = 0;

  tmp_page = gc->solid_data_alloc;
  tmp_stop = gc->gc_next & gc->ptable->super.block_mask;
  while (tmp_page != tmp_stop) {
    page_state = gc->ptable->get_reverse (gc->ptable, tmp_page, NULL);
    if (page_state == FS_RMAP_BLOCK_ERASED) {
      /* XXX: Layering violation. */
      fs_ptable_nor_t npt = (fs_ptable_nor_t) gc->ptable;
      gc->free_count += npt->reserved_offset -
        (tmp_page & ~gc->ptable->super.block_mask);
    }
    BUMP_BLOCK (gc, tmp_page);
  }
}

static void
fs_pm_nand_compute_free_count (fs_pm_flash_t  gc)
{
  fs_page_id tmp_page;
  uint32 block_size = gc->ptable->super.data->block_size;

  tmp_page = 0;

  /* Check all blocks in the region for erased status. Allocator has already
     been repositioned to take care of written blocks without status update.
     So no need to check the device itself. */
  do
  {
    if (gc->ptable->get_reverse (gc->ptable, tmp_page, NULL)
        == FS_RMAP_BLOCK_ERASED)
    {
      gc->free_count += block_size;
    }

    BUMP_BLOCK (gc, tmp_page);

  } while (tmp_page != 0);


  /* Gather rest of the pages from half filled allocator blocks as well */
  tmp_page = 0;
  if (gc->solid_data_alloc != FS_INVALID_PAGE_ID)
    tmp_page += gc->ptable->super.data->block_size -
                (gc->solid_data_alloc & (~gc->ptable->super.block_mask));
  if (gc->liquid_data_alloc != FS_INVALID_PAGE_ID)
    tmp_page += gc->ptable->super.data->block_size -
                 (gc->liquid_data_alloc & (~gc->ptable->super.block_mask));

  gc->free_count += tmp_page;
}


/* Initialize the Page Manager module with all writing logic stubbed out. */
fs_pm_t
fs_pm_read_only_init (fs_pm_flash_t  gc, efs_device_t   dev)
{
  int fresh_start;
  int return_val;

  /* Make sure that the realloc-buffer is aligned as per DMA's expectations. */
  ASSERT (FS_OKAY_FOR_DM (gc->realloc_buffer));

  fs_pm_init_read_only_ops (&gc->parent.ops);

  fs_pm_data_initialize (gc, dev);

  fs_log_init (&gc->log, dev, fs_pm_log_invalid_page_write_handler, gc);

  return_val = fs_ptable_read_only_init (gc->ptable, dev, &gc->log,
                                         &fresh_start, &gc->logr);

  if (return_val != 0 || fresh_start != 0)
    return NULL;

  if (FS_ISNOR (&gc->ptable->super))
  {
    /* Read only mode for NOR is currently not supported and it has not been
     * tested. */
    return NULL;
  }

  gc->min_page = 0;
  gc->max_page = gc->ptable->super.data->u.nand.regions[0]
    << gc->ptable->super.block_shift;
  /* Setup the log region. */
  fs_logr_init (&gc->logr, &gc->ptable->super, fresh_start);

  /* Read the initial parameters out of the superblock. */
  gc->gc_next = gc->ptable->super.data->gc_next[0];
  gc->solid_data_alloc = gc->ptable->super.data->alloc_next[0];
  /* Since only 2 of the pointer arrays in superblock out of 4 are being
   * used currently, we could use the free pointer location 2 to store
   * our extra pointer.  */
  gc->liquid_data_alloc = gc->ptable->super.data->alloc_next[2];

  /* These are our known good values of the allocators. If there are
   * logs that have more activity for a given allocators, then the
   * replaying routine also updates these known good values, so we
   * need this assignment before logs are replayed. */
  gc->known_good_solid_alloc = gc->solid_data_alloc;
  gc->known_good_liquid_alloc = gc->liquid_data_alloc;

  /* Replay the log, which can set table entries, as well as adjust these
   * values. */
#ifdef FEATURE_EFS_EFS2_ON_NAND
  fs_logr_iterate (&gc->logr, &gc->log,
                   fs_pm_log_visit_nand_node, (void *) gc);
#endif

  /* Perform mid-point initialization. */
  if (gc->ptable->mid_init != NULL)
    gc->ptable->mid_init (gc->ptable);

  fs_pm_nand_compute_free_count (gc);

  /* Call the appropriate restart. */
  gc->ptable->post_init (gc->ptable);

  /* For NAND we need GC threshold limits and bad block limits before we
   * run the GC. */
  if (FS_ISNAND (&gc->ptable->super))
    calculate_gc_threshold_limits (gc);

  return (&gc->parent);
}

void
fs_pm_init (
    fs_pm_flash_t  gc,
    efs_device_t   dev)
{
  /* Make sure that the realloc-buffer is aligned as per DMA's expectations. */
  ASSERT (FS_OKAY_FOR_DM (gc->realloc_buffer));

  fs_pm_init_ops (&gc->parent.ops);

  fs_pm_data_initialize (gc, dev);

  fs_log_init (&gc->log, dev, fs_pm_log_page_write_handler, gc);

  fs_ptable_init (gc->ptable, dev, &gc->log, &gc->fresh_start, &gc->logr);

  if (FS_ISNOR (&gc->ptable->super))
  {
    gc->min_page = 0;
    gc->max_page = gc->ptable->super.total_pages;
  }
  else /* NAND */
  {
    gc->min_page = 0;
    gc->max_page = gc->ptable->super.data->u.nand.regions[0]
                                          << gc->ptable->super.block_shift;
    /* Setup the log region. */
    fs_logr_init (&gc->logr, &gc->ptable->super, gc->fresh_start);
  }

  /* If this is a fresh start, need to "initialize" the garbage collector
   * parameters. */
  if (gc->fresh_start)
  {
    /* We don't use liquid data allocator in NOR and for NAND it would be
     * initialized to INVALID_PAGE since GC will assign appropriate blk after
     * erasing one */
    gc->liquid_data_alloc = FS_INVALID_PAGE_ID;
    gc->state = FS_PM_HEAD;

    if (FS_ISNOR (&gc->ptable->super))
      fs_pm_nor_fresh_start (gc);
    else
      fs_pm_nand_fresh_start (gc);

    /* For NAND we need GC threshold limits and bad block limits before we
     * run the GC. */
    if (FS_ISNAND (&gc->ptable->super))
      calculate_gc_threshold_limits (gc);

    /* Do Garbage Collection (GC) only when the file system is starting fresh.
     * This is to do the initial setup of the file system state, e.g. position
     * allocators and GC, erase the first blocks.
     *
     * For NON fresh start case the file system should NOT do Garbage
     * Collection (GC) on initialize. When the Page Manager layer does GC, it
     * may need to consult the Buffer layer to check if the cluster
     * has been deleted, this is done by using the function pointer
     * gc->is_used_cb. This function pointer is set when the Buffer
     * layer initializes, which is after PM initializes.
     *
     * Nothing breaks if GC is done during initialization. The consequence of
     * doing GC when PM initializes is that it may skip blocks that have
     * garbage pages, potentially increasing the overhead and reducing the
     * file system performance.
     *
     * In specific situations a log page will be flushed during init. One
     * advantage of doing GC on init is that one log page write will be
     * saved by putting all log data (GC + init data) into a single log. */
      /* Hold off the writing of superblock until the GC is run
       * for the first time */
    fs_pm_do_urgent(gc, TRUE);

    /* fs_pm_do_urgent() call above would have written to the logs, so its ok
       to flush now. */
    fs_log_flush (&gc->log);

    if (FS_ISNAND (&gc->ptable->super))
    {
      uint32 args[3];
      fs_page_id page;
      fs_block_id log_blk, same_blk;

      /* We have not written the first super-page yet, we want to write this
       * super-page now to record the ERASE logs in case if we have to replay
       * them at next startup, But we don't flush super-page until we write
       * till the last-page of the block, so we have to keep filling all the
       * remaining log-pages with FLUSH-logs, When the last log-page in the
       * block gets written, then the super-page will be automatically
       * flushed. */
      page = fs_log_peek_page (&gc->log);
      same_blk = log_blk = page & gc->ptable->super.block_mask;
      while (log_blk == same_blk)
      {
        fs_pm_do_urgent (gc, FALSE);
        page = fs_log_peek_page (&gc->log);
        log_blk = page & gc->ptable->super.block_mask;
        /* FS_LOG_ENTRY_LOG_FLUSH doesn't do anything but to update q_free. So,
         * these logs can be used to pad the rest of the block. */
        args[0] = page;
        fs_log_add (&gc->log, FS_LOG_ENTRY_LOG_FLUSH, args);
        fs_log_flush (&gc->log);
      }
    }
  }
  else      /* NOT a Fresh start */
  {
    /* Read the initial parameters out of the superblock. */
    gc->gc_next = gc->ptable->super.data->gc_next[0];
    gc->solid_data_alloc = gc->ptable->super.data->alloc_next[0];
    /* Since only 2 of the pointer arrays in superblock out of 4 are being
     * used currently, we could use the free pointer location 2 to store
     * our extra pointer.  */
    gc->liquid_data_alloc = gc->ptable->super.data->alloc_next[2];

    /* These are our known good values of the allocators. If there are
     * logs that have more activity for a given allocators, then the
     * replaying routine also updates these known good values, so we
     * need this assignment before logs are replayed. */
    gc->known_good_solid_alloc = gc->solid_data_alloc;
    gc->known_good_liquid_alloc = gc->liquid_data_alloc;

    /* Replay the log, which can set table entries, as well as adjust these
     * values. */
    /*printf ("Pre log iter: solid_alloc=0x%x, liquid_alloc=0x%x, gc=0x%x\n",
        gc->solid_data_alloc, gc->liquid_data_alloc, gc->gc_next);*/
    if (FS_ISNOR (&gc->ptable->super))
    {
      gc->ptable->log_iterate (gc->ptable, &gc->log,
          fs_pm_log_visit_node, (void *) gc);
    }
    else
    {
#ifdef FEATURE_EFS_EFS2_ON_NAND
      fs_logr_iterate (&gc->logr, &gc->log,
          fs_pm_log_visit_nand_node, (void *) gc);
#endif
    }
    /*printf ("Post log iter: solid_alloc=0x%x, liquid_alloc=0x%x, gc=0x%x\n",
        gc->solid_data_alloc, gc->liquid_data_alloc, gc->gc_next);*/

    /* Perform mid-point initialization. */
    if (gc->ptable->mid_init != NULL)
      gc->ptable->mid_init (gc->ptable);

    gc->pm_init_pending = 1;
  } /* fresh start */

#ifdef FS_UNIT_TEST
    // fs_pm_validate (&gc->parent);
#endif
}

/* Function to finish initialization of PM state like alloc etc. Can result
   in writes and erases, but is required to be done only before first write.
   Hence delayed to prevent writes/erases during efs_init. EFS can still
   support reads before this is invoked. */
static void
fs_pm_complete_pending_init (fs_pm_flash_t gc)
{
  if (!gc->pm_init_pending)
    return;

  if (FS_ISNOR (&gc->ptable->super))
    fs_pm_nor_powerup_init (gc);
  else
    fs_pm_nand_powerup_init (gc);

  /* Re-compute the free count. */
  if (FS_ISNOR (&gc->ptable->super))
    fs_pm_nor_compute_free_count (gc);
  else
    fs_pm_nand_compute_free_count (gc);

  /*printf ("Recomputed free count: 0x%x\n", gc->free_count);*/

  /* Call the appropriate restart. */
  gc->ptable->post_init (gc->ptable);

  /* For NAND we need GC threshold limits and bad block limits before we
   * run the GC. */
  if (FS_ISNAND (&gc->ptable->super))
    calculate_gc_threshold_limits (gc);

  /* fs_pm_nand_powerup_init() call above would have written to the logs, so
     it's ok to flush now. */
  fs_log_flush (&gc->log);

  gc->pm_init_pending = 0;

#ifdef FS_UNIT_TEST
  // fs_pm_validate (&gc->parent);
#endif
}

static void
fs_pm_register_free_check_cb (fs_pm_t gen_gc,
    int (*is_used_cb) (void *data, fs_cluster_id cluster), void *data)
{
  fs_pm_flash_t gc = (fs_pm_flash_t) gen_gc;

  /* It is erroneous to register multiple callbacks. */
  ASSERT (gc->is_used_cb == NULL);

  gc->is_used_cb = is_used_cb;
  gc->is_used_data = data;
}

static void
fs_pm_register_alloc_count_cb (fs_pm_t gen_gc,
    uint32 (*alloc_count_cb) (void *data), void *data)
{
  fs_pm_flash_t gc = (fs_pm_flash_t) gen_gc;

  /* It is erroneous to register multiple callbacks. */
  ASSERT (gc->alloc_count_cb == NULL);

  gc->alloc_count_cb = alloc_count_cb;
  gc->alloc_count_data = data;
}

void
fs_pm_shutdown_flush (fs_pm_t gen_gc)
{
  fs_pm_flash_t gc = (fs_pm_flash_t) gen_gc;
  (void) gc;

#ifdef FS_UNIT_TEST
  if (FS_ISNAND (&gc->ptable->super))
    fs_ptable_nand_flush (gc->ptable);
#endif
}

#ifdef FEATURE_EFS_EFS2_ON_NAND
static void
fs_pm_log_visit_nand_node (void *vgc, fs_log_code_t code, uint32 *args)
{
  fs_pm_flash_t gc = (fs_pm_flash_t) vgc;
  fs_ptable_nand_log_visit (gc->ptable, code, args);
  fs_pm_log_visit_node (vgc, code, args);
}
#endif

/* Bump the NAND allocator only if it doesn't cross the block boundary. */
static void
bump_allocator (fs_pm_flash_t gc, fs_page_id page, uint16 tag)
{
  if (FS_ISNAND (&gc->ptable->super))
  {
    /* Backup the page write activity that we could use while searching
     * for the erased blocks. */
    if (tag)
      gc->known_good_liquid_alloc = page;
    else
      gc->known_good_solid_alloc = page;

    /* Here if our page bump moves the pointer to another block, then we
     * don't make that move, since that block could have been allocated to the
     * other allocator, or in subsequent logs it might get allocated to the
     * other allocator, so instead of complicatig the things, keep it simple.
     * In this case it is easy to handle since it is predictable and same for
     * many cases. */
    BUMP_PAGE (gc, page);

    /* We don't do implicit block crossing */
    if ((page & ~gc->ptable->super.block_mask) == 0)
      page = FS_INVALID_PAGE_ID;

    /* Assign to appropriate allocator */
    if (tag)
      gc->liquid_data_alloc = page;
    else
      gc->solid_data_alloc = page;
  }
  else
  {
    /* NOR nothing changes since we use only one allocator for all the tags */
    BUMP_PAGE (gc, page);
    gc->solid_data_alloc = page;
  }
}

static void
fs_pm_bump_gc_block (fs_pm_flash_t gc)
{
  BUMP_BLOCK (gc, gc->gc_next);
  fs_pm_gc_update_nofity (gc);
}

static void
fs_pm_log_visit_node (void *vgc, fs_log_code_t code, uint32 *args)
{
  fs_pm_flash_t gc = (fs_pm_flash_t) vgc;
  fs_page_id page; uint32 tag;

  switch (code) {
    case FS_LOG_ENTRY_PAGE_MOVE:
      /*printf ("page move: 0x%x\n", args[2]);*/
      /* Extract the tag information to move appropriate allocator */
      tag = (args[0] >> TAG_SHIFT) & TAG_MASK;
      page = args[2];
      if (page < gc->min_page || page >= gc->max_page)
        FS_ERR_FATAL ("Inconsistency", 0, 0, 0);
      bump_allocator (gc, page, tag);
      break;

    case FS_LOG_ENTRY_NEW_DATA:
      /*printf ("new data: 0x%x\n", args[1]);*/
      tag = (args[0] >> TAG_SHIFT) & TAG_MASK;
      page = args[1];
      if (page < gc->min_page || page >= gc->max_page)
        FS_ERR_FATAL ("Inconsistency", 0, 0, 0);
      bump_allocator (gc, page, tag);
      break;

    case FS_LOG_ENTRY_GC_MOVE:
      /*printf ("gc next: 0x%x\n", args[1]);*/
      /*
      gc->gc_next = args[1];
      BUMP_PAGE (gc, gc->gc_next);
      */

      tag = (args[0] >> TAG_SHIFT) & TAG_MASK;
      page = args[2];
      if (page < gc->min_page || page >= gc->max_page)
        FS_ERR_FATAL ("Inconsistency", 0, 0, 0);
      bump_allocator (gc, page, tag);
      break;

    case FS_LOG_ENTRY_GARBAGE:
      page = args[0];
      if (page >= gc->min_page && page < gc->max_page) {
        gc->ptable->set_reverse (gc->ptable, page, FS_RMAP_PAGE_GARBAGE,
                                 FS_PM_NULL_TAG);
        if (FS_ISNOR (&gc->ptable->super)) {
          bump_allocator (gc, page, 0);
        }
        else {
          /* On NAND bump the appropiate allocator. */
          if (gc->liquid_data_alloc == page)
            bump_allocator (gc, page, 1);
          else if (gc->solid_data_alloc == page)
            bump_allocator (gc, page, 0);
        }
      }
      break;

    case FS_LOG_ENTRY_ERASE_FINISH:
      page = args[0] << gc->ptable->super.block_shift;
      if (page >= gc->min_page && page < gc->max_page) {
        gc->gc_next = page;
        fs_pm_bump_gc_block (gc);
      }
      break;

    case FS_LOG_ENTRY_ERASE_FAIL:
      page = args[0] << gc->ptable->super.block_shift;
      if (page >= gc->min_page && page < gc->max_page) {
        gc->ptable->set_reverse (gc->ptable, page, FS_RMAP_BLOCK_BAD,
                                 FS_PM_NULL_TAG);
      }
      break;

    case FS_LOG_ENTRY_WRITE_FAIL:
      page = args[0];
      if (page >= gc->min_page && page < gc->max_page) {
        gc->ptable->set_reverse (gc->ptable, page, FS_RMAP_BLOCK_BAD,
                                 FS_PM_NULL_TAG);
      }
      /* Alloc next needs not be advanced, since there isn't yet a log
       * flush, and another alloc will always be tried next. */
      break;

    case FS_LOG_ENTRY_LOG_FLUSH:
      break;

    case FS_LOG_ENTRY_UPPER_DATA:
    case FS_LOG_ENTRY_XACT_START:
    case FS_LOG_ENTRY_XACT_END:
    case FS_LOG_ENTRY_PTABLE_MOVE:
    case FS_LOG_ENTRY_GC_DEALLOC:
    case FS_LOG_ENTRY_KEYRANGE:
      break;

    case FS_LOG_ENTRY_LOG_ALLOC:
      /* Nor allocator should be bumped because LOG_ALLOC entries
       * correspond to a page allocation.
       */
      page = args[0];
      if (page >= gc->min_page && page < gc->max_page)
        bump_allocator (gc, page, FS_PM_NULL_TAG);
      break;

    default:
      FS_ERR_FATAL ("Unhandled log entry: %02x", code, 0, 0);
  }
}
static uint32
get_nor_urgency_free_count (fs_pm_flash_t gc)
{
  uint32 urgency_free_count;
  uint32 block_count = fs_device_block_count (gc->dev);

  /* For NOR device the free space threshold for urgency depends on what
   * is the desired goal and tradeoff to achieve.
   * Refer to the comment for the macro FS_PM_NOR_URGENCY_FREE_CNT_OVERHEAD
   * for more details.
   *
   * Usually garbage collection data move is done by the incremental
   * garbage collector. This urgency garbage collection is done before writing
   * the transaction to make sure that we have enough  free space. If
   * the free space is below the threshold then the GC will run until enough
   * free space is created.
   */

  if (block_count < 8) {
    urgency_free_count = gc->ptable->super.data->block_size
                     + FS_PM_NOR_URGENCY_FREE_CNT_OVERHEAD;
  }
  else {
    urgency_free_count = 2 * gc->ptable->super.data->block_size +
         FS_MAX_TRANSACTION_DATA_PAGES;
  }

  return urgency_free_count;
}

/* Determine the "urgency" of this collect.  Numbers over 80 indicate that
 * this GC is required.
 *
 * If we are within limit of overhead write page count for this transaction,
 * then attempt to do a bit more GC to free up space till its max threshold.
 *
 * If we have exceeded this overhead limit for this transaction, then run GC
 * only if the free space limit goes below min threshold.
 */
static int
fs_pm_urgency (fs_pm_flash_t gc)
{
  if (FS_ISNAND (&gc->ptable->super)) {
    /* Check if we are still within our write limit for this transaction
     * If we are within limit we could free up a bit more space, otherwise
     * if we have already exceeded the limit, do GC only if we are below
     * critical threshold limit. */
    if (gc->overhead_write_count < FS_PM_GC_OVERHEAD_WRITES_PER_TXN)
    {
      if (gc->free_count < gc->gc_max_threshold)
        return 80;
    }
    else
    {
      if (gc->free_count < gc->gc_min_threshold)
        return 80;
    }
    return 0;
  }
  else
  {
    /* No change to the NOR limits. */
    if (gc->free_count < get_nor_urgency_free_count (gc) + 2)
      return 80;
    else
      return 0;
  }
}

/* static void cleanup_superblock_logs (fs_pm_t gc); */
/* Allocate a single page.  Assumes that the GC has been run enough, that
 * there are pages available. */
static fs_page_id
fs_pm_nor_alloc (fs_pm_flash_t gc)
{
  fs_page_id state;
  fs_page_id target;

  ASSERT (gc->free_count > 0);

  /* Loop until we return a result. */
  while (1) {
    state = gc->ptable->get_reverse (gc->ptable, gc->solid_data_alloc, NULL);
    switch (state) {
      case FS_RMAP_BLOCK_ERASED:
        if (((gc->solid_data_alloc & (~gc->ptable->super.block_mask)) == 0
              && FS_ISNOR (&gc->ptable->super)))
        {
          gc->ptable->mark_alloc (gc->ptable, gc->solid_data_alloc);
          gc->ptable->set_reverse (gc->ptable, gc->solid_data_alloc,
              FS_RMAP_PAGE_SUPER, FS_PM_NULL_TAG);
          gc->next_super_page = gc->solid_data_alloc;
          gc->free_count--;
          BUMP_PAGE (gc, gc->solid_data_alloc);
        } else {
          gc->ptable->mark_alloc (gc->ptable, gc->solid_data_alloc);
          target = gc->solid_data_alloc;
          BUMP_PAGE (gc, gc->solid_data_alloc);
          gc->free_count--;

#ifdef FS_UNIT_TEST
          /* This is a bit expensive for on-target, but helps catch errors
           * quickly off target. */
          ASSERT (fs_device_is_erased (gc->dev, target));
#endif
          return target;
        }
        break;

      case FS_RMAP_PAGE_SUPER:
      case FS_RMAP_PAGE_RESERVED:
        BUMP_PAGE (gc, gc->solid_data_alloc);
        break;
      default:
        BUMP_BLOCK(gc, gc->solid_data_alloc);
    }
  }
}

static fs_page_id
fs_pm_alloc (fs_pm_flash_t gc, uint32 tag)
{
  fs_page_id state;
  fs_page_id target;

  ASSERT (gc->free_count > 0);

  /* If this is NOR PM then let the NOR allocator handle in the
   * regular way. */
  if (FS_ISNOR (&gc->ptable->super))
    return fs_pm_nor_alloc (gc);

  /* If it is NAND allocator, then we will have to allocate the
   * page in appropriate block using correct allocator to group
   * similar data items together. */
  if (tag == FS_PM_NULL_TAG)
  {
    target = gc->solid_data_alloc;
    BUMP_ALLOCATOR_PAGE (gc, gc->solid_data_alloc);
  }
  else
  {
    target = gc->liquid_data_alloc;
    BUMP_ALLOCATOR_PAGE (gc, gc->liquid_data_alloc);
  }

  /* Check the page status it should be marked erased. */
  state = gc->ptable->get_reverse (gc->ptable, target, NULL);
  if (state != FS_RMAP_BLOCK_ERASED)
  {
    FS_ERR_FATAL ("Page not erased.. Pg:%lX State:%lX", target, state, 0);
  }

  /* Mark this allocated page as allocated and mark the next page as
   * erased. Decrement the free count, since we just consumed one. */
  gc->ptable->mark_alloc (gc->ptable, target);
  gc->free_count--;

#ifdef FS_UNIT_TEST
  /* This is a bit expensive for on-target, but helps catch errors
   * * quickly off target. */

  if (!fs_device_is_erased (gc->dev, target))
    printf ("Allocated Page is not erased : %X %lX\n", target, state);
  ASSERT (fs_device_is_erased (gc->dev, target));
#endif

  /* Return the allocated page for this tag */
  return target;
}

/* Should be called right after alloc, if the write fails.  Marks the given
 * page with a 'BAD' marker, and advances the allocator to the following
 * block. */
static void
fs_pm_mark_bad (fs_pm_flash_t gc, fs_page_id page, uint16 tag)
{
  uint32 tmp;
  uint32 args[3];

  /* A write failure on NOR either indicates end-of-life device wear, or,
   * more likely, a hardware problem.  Seeing this error during development
   * is a strong indicator of either a hardware problem, or a device driver
   * problem. */
  if (FS_ISNOR (&gc->ptable->super))
    FS_ERR_FATAL ("NOR write failure", 0, 0, 0);

  /* Since we just now encountered a new bad block, add it to our existing
   * count. Do not error Fatal here since if we crash here, we would lose
   * the information of this block becoming bad. So we should defer fatal
   * condition until log is flushed. */
  gc->bad_blk_count++;

  /* Mark the page as bad.  The garbage collector will detect this, and
   * after copying valid data out of the block, mark the entire block as
   * bad. */
  args[0] = page;
  fs_log_add (&gc->log, FS_LOG_ENTRY_WRITE_FAIL, args);
  fs_log_flush (&gc->log);
  gc->ptable->set_reverse (gc->ptable, page, FS_RMAP_BLOCK_BAD,
                           FS_PM_NULL_TAG);

  /* If the first page in the block went bad, physically mark the block
   * bad. */
  if ((page & ~gc->ptable->super.block_mask) == 0) {
    (void) fs_device_mark_block_bad (gc->dev,
                                    page >> gc->ptable->super.block_shift);
  }

  /* Assign next available free block to the appropriate allocator, so
   * that we can proceed operating with what we left with. */
  if (tag == FS_PM_NULL_TAG){
    tmp = gc->solid_data_alloc;
    gc->solid_data_alloc = fs_pm_get_erased_blk (gc);
  }else{
    tmp = gc->liquid_data_alloc;
    gc->liquid_data_alloc = fs_pm_get_erased_blk (gc);
  }

  /* Calculate how many pages were free in this block that we lost from
   * free_count because the block became bad. */
  tmp = (tmp & ~gc->ptable->super.block_mask);
  tmp = gc->ptable->super.data->block_size - tmp;

  /* Since already alloc_page would have reduced free count while allocating
   * for this page before it went bad, adjust for that count. */
  --tmp;

  /* Subtract as many pages we had empty in this blk from free_count. */
  gc->free_count -= tmp;
}

/* Count the number of pages of garbage in the block at the given starting
 * address. */
static int
fs_pm_count_garbage (fs_pm_flash_t gc, fs_page_id base)
{
  fs_page_id limit = base + gc->ptable->super.data->block_size;
  int count = 0;
  fs_page_id stat;

  for (; base < limit; base++) {
    stat = gc->ptable->get_reverse (gc->ptable, base, NULL);
    switch (stat) {
      case FS_RMAP_PAGE_GARBAGE:
      case FS_RMAP_PAGE_LOG:
      case FS_RMAP_BLOCK_ERASED:
      case FS_RMAP_PAGE_SUPER:
      case FS_RMAP_PAGE_RESERVED:
        /* Treat unknown pages the same as garbage. Unknown pages should be
         * rare, but they are possible when the allocator leaves pages unused
         * in the first pass. */
      case FS_RMAP_BLOCK_UNKNOWN:
        count++;
        break;

      /* Make sure if we get a bad block, we do GC on this block so that
       * we can move all good pages out of it and mark it bad on flash */
      case FS_RMAP_BLOCK_BAD:
        count++;
        return count;

      default:
        /* Also check from buffer layer if this cluster is still in use.
         * Because of delete optimization, the clusters are not released
         * immediately, but if the clusters are second or third generation,
         * then if it doesn't have even a single garbage page in this block
         * then GC would never get a chance to GC this block even if none of
         * the pages are really allocated, that is if these are garbages. This
         * call back check will really check from buffer layer, and will do GC
         * on this block also if PM still didn't mark them garbage but are not
         * used in Buffer layer. */
        if ((gc->is_used_cb != NULL) &&
            (gc->is_used_cb (gc->is_used_data, stat) == 0))
          count++;
        break;
    }
  }

  return count;
}

/* Perform a single unit of garbage collection. */
static void
fs_pm_single (fs_pm_flash_t gc, boolean allow_erase_pending)
{
  fs_page_id pstate;
  fs_page_id dest;
  int count;
  int result;
  int moved;
  uint32 args[3];
  uint32 tag;
  uint32 gc_blk, solid_blk, liquid_blk;
  fs_page_id page_stop = FS_INVALID_PAGE_ID;

  while (1)
  {
    switch (gc->state)
    {
      case FS_PM_HEAD:
      {
        /* The head state comes in only when the page is at the beginning of
         * a block. */
        ASSERT ((gc->gc_next & (~gc->ptable->super.block_mask)) == 0);

        /* Safe exit criteria for the while(1) loop to make sure that the
         * garbage collection iterate through the region only once.
         */
        if (page_stop == FS_INVALID_PAGE_ID)
        {
          /* This cannot be assigned before while loop since, in that case
           * the exit condition would be true the first time itself, even
           * before doing anything. */
          page_stop = gc->gc_next & gc->ptable->super.block_mask;
        }
        else if (page_stop == gc->gc_next)
        {
          return;
        }

        if (FS_ISNAND (&gc->ptable->super))
        {
          /* There is a chance that GC might be pointing to the same block as
           * one of the allocation pointers, since each alloc pointer might be
           * moving at a different speed. So check if GC is pointing to the
           * same block as allocation pointer is pointing to and skip them. */
          gc_blk = gc->gc_next & gc->ptable->super.block_mask;
          solid_blk = gc->solid_data_alloc & gc->ptable->super.block_mask;
          liquid_blk = gc->liquid_data_alloc & gc->ptable->super.block_mask;

          while ((gc_blk == solid_blk) || (gc_blk == liquid_blk))
          {
            fs_pm_bump_gc_block (gc);
            gc_blk = gc->gc_next & gc->ptable->super.block_mask;
          }

          /* If we moved back to the starting block (that is backward jump
           * compared to last reference point) because of wrap around in the
           * region, now we would have counted all the back blocks in this
           * region. So update our count of the total bad blocks in this
           * region. */
          if (gc->gc_next < gc->last_reference)
          {
            /* Update our count to latest count. */
            gc->bad_blk_count = gc->bad_block_acc;
            /* Reset our accumulator */
            gc->bad_block_acc = 0;
          }
          gc->last_reference = gc->gc_next;
        }

        pstate = gc->ptable->get_reverse (gc->ptable, gc->gc_next, NULL);
        switch (pstate)
        {
          case FS_RMAP_BLOCK_UNKNOWN:
          {
            /* Check if this block is good. */
            if (fs_device_bad_block_check (gc->dev,
                  gc->gc_next >> gc->ptable->super.block_shift))
            {
              gc->ptable->set_reverse (gc->ptable, gc->gc_next,
                  FS_RMAP_BLOCK_BAD, FS_PM_NULL_TAG);
              fs_pm_bump_gc_block (gc);
            }
            else
            {
              gc->state = FS_PM_ERASE;
              return;
            }
          }
          break;

          case FS_RMAP_BLOCK_BAD:
          {
            /* Accumulate the count for this bad block, and compare if we
             * crossed the pre configured bad block limit, always doing
             * err_fatal here would help catch the excess bad blocks error
             * consistently across multiple reboots. But in this case the
             * GC has to go through the complete flash once in the worst
             * case to hit the error fatal. So at first it might appear
             * that the error went away, but it would come back later.
             * Examining the reverse page table for the bad blocks would
             * reveal if a non consistent reboots are happening. */
            gc->bad_block_acc++;
            if (gc->bad_block_acc > gc->max_bad_blocks_allowed)
            {
              FS_ERR_FATAL ("%d bad blocks! Exceeds the configured max of %d.",
                            gc->bad_block_acc, gc->max_bad_blocks_allowed, 0);
            }

            if (FS_ISNAND (&gc->ptable->super))
            {
              fs_pm_bump_gc_block (gc);
            }
            else
            {
              FS_ERR_FATAL ("%d, Should not be bad blks in NOR.", gc->gc_next,
              0, 0);
            }
          }
          break;

          /* We can get erased blocks here for a really small file system
           * when the alloc pointers are located far away or when the EFS
           * is full and maximum bad blocks have developed, in which case we
           * might find garbage after the alloc pointer also, so proceed
           * further past this erased block also. */
          case FS_RMAP_BLOCK_ERASED:
          {
            if (FS_ISNAND (&gc->ptable->super))
            {
              fs_pm_bump_gc_block (gc);
              break;
            }
            /* If not nand, fall through, so that block gets GC-ed */
          }
          /*lint -fallthrough */

          default:
          {
            int do_gc = 0;
            /* Count the number of garbage pages in this block. */
            count = fs_pm_count_garbage (gc, gc->gc_next);
            if (count > 0)
              do_gc = 1;

            if (gc->scrub_info.scrub_state == FS_SCRUB_RUNNING)
              do_gc = 1; /* We are scrubbing, so force GC.  */

            if (do_gc)
              gc->state = FS_PM_MOVING;
            else
              fs_pm_bump_gc_block (gc);
          }
          break;
        }
      }  /* case FS_PM_HEAD */
      break;

      case FS_PM_MOVING:
      {
        pstate = gc->ptable->get_reverse (gc->ptable, gc->gc_next, &tag);
        moved = 0;
        switch (pstate)
        {
        /* Treat unknown pages the same as garbage. Unknown pages should be
         * rare, but they are possible when the allocator leaves pages unused
         * in the first pass. */
          case FS_RMAP_BLOCK_UNKNOWN:
          case FS_RMAP_PAGE_GARBAGE:
          case FS_RMAP_PAGE_RESERVED:
          case FS_RMAP_PAGE_SUPER:
          case FS_RMAP_PAGE_LOG:

            /* Only valid for NOR. */
          case FS_RMAP_BLOCK_ERASED:
            break;

          case FS_RMAP_BLOCK_BAD:
          {
            /* A bad block marked in the middle of a block means that we
             * detected this failure while writing a page.  At this point,
             * we've already copied all good data out of the block, and can
             * just mark the entire block as 'bad'.*/
            gc->ptable->set_reverse (gc->ptable,
                gc->gc_next & gc->ptable->super.block_mask,
                FS_RMAP_BLOCK_BAD, FS_PM_NULL_TAG);
            /* Log the change. */
            {
              int block = gc->gc_next >> gc->ptable->super.block_shift;
              args[0] = block;
              fs_log_add (&gc->log, FS_LOG_ENTRY_ERASE_FAIL, args);
              fs_log_flush (&gc->log);
            }
            /* Erase the block before marking bad, since after marking the
             * block as bad, it cannot be erased since the information that
             * its bad can be lost on erase. We need to erase the block since
             * it might have some old data from a deleted file which might
             * need to be shredded.
             */
            (void) fs_device_erase_block (gc->dev,
                                  gc->gc_next >> gc->ptable->super.block_shift,
                                  FS_FOP_USER);
            /* Physically mark the block as bad. */
            (void) fs_device_mark_block_bad
              (gc->dev, gc->gc_next >> gc->ptable->super.block_shift);
            fs_pm_bump_gc_block (gc);
            gc->state = FS_PM_HEAD;

            /* Accumulate the count for this bad block, and compare if we
             * crossed the pre configured bad block limit */
            gc->bad_block_acc++;
            if (gc->bad_block_acc > gc->max_bad_blocks_allowed)
              FS_ERR_FATAL ("Number of bad blocks allowed exceeded %d",
                            gc->bad_block_acc, 0, 0);

            /* Consider this a unit of work.  If anything, it makes getting
             * out of this state easier. */
            return;
          }

          default:
          {
            if (!FS_RMAP_IS_SPECIAL (pstate))
            {
              if (gc->is_used_cb == NULL ||
                  gc->is_used_cb (gc->is_used_data, pstate))
              {
                dest = fs_pm_alloc (gc, tag);
                fs_device_read_page (gc->dev, gc->gc_next,
                    gc->realloc_buffer,
                    FS_FOP_DATA_GC_MOVE, FS_FLASH_MUST_WORK);
                /* printf ("gc write: %d\n", dest); */
                /* Since we did write a GC'd page, update our
                 * overhead counter to keep track */
                gc->overhead_write_count++;
                result = fs_device_write_page (gc->dev, dest,
                    gc->realloc_buffer, FS_FOP_DATA_GC_MOVE);
                while (result != FS_DEVICE_OK)
                {
                  /* XXX: Count up errors to handle them. */

                  /* Write error: Mark rest of block bad, and try allocation
                   * at the next available block. */
                  fs_pm_mark_bad (gc, dest, tag);

                  /* Get another page, and try writing it. */
                  dest = fs_pm_alloc (gc, tag);
                  result = fs_device_write_page (gc->dev, dest,
                      gc->realloc_buffer, FS_FOP_DATA_GC_RECOVERY);

                  /* Loop back around to check for error. */
                }

                /* Log the change. */
                args[0] = (pstate & CLUSTER_MASK) | (tag << TAG_SHIFT);
                args[1] = gc->gc_next;
                args[2] = dest;
                fs_log_add (&gc->log, FS_LOG_ENTRY_GC_MOVE, args);

                cleanup_logs (gc);

                gc->ptable->set_forward (gc->ptable, pstate, dest);
                gc->ptable->set_reverse (gc->ptable, gc->gc_next,
                    FS_RMAP_PAGE_GARBAGE, FS_PM_NULL_TAG);
                gc->ptable->set_reverse (gc->ptable, dest, pstate, tag);

                moved = 1;
              }
              else
              {
                /* The page exists, but is no longer in use.  Deallocate
                 * the page, instead of moving it. */
                args[0] = pstate;
                args[1] = gc->gc_next;
                fs_log_add (&gc->log, FS_LOG_ENTRY_GC_DEALLOC, args);

                gc->ptable->set_forward (gc->ptable, pstate,
                    FS_INVALID_PAGE_ID);
                gc->ptable->set_reverse (gc->ptable, gc->gc_next,
                    FS_RMAP_PAGE_GARBAGE, FS_PM_NULL_TAG);

                fs_flash_dealloc_counter++;
              }
            }
            else
            {
              FS_ERR_FATAL ("GC encountered unknown page type %08x", pstate,
                            0, 0);
            }
          }
          break;
        }

        if (((gc->gc_next + 1) & (~gc->ptable->super.block_mask)) == 0)
        {
          gc->state = FS_PM_ERASE;
        }
        else
        {
          BUMP_PAGE (gc, gc->gc_next);
        }

        /* Return even if only one page was considered for move on
         NOR.  Return if one page moved on NAND. */
        if(moved || FS_ISNOR (&gc->ptable->super))
          return;
      } /* case FS_PM_MOVING */
      break;

      case FS_PM_ERASE:
      {
        /* Flush the log since we could be erasing data that is only described
         * in that log. */
        fs_log_flush (&gc->log);
        cleanup_logs (gc);

        if (FS_ISNAND (&gc->ptable->super))
        {
          int block = gc->gc_next >> gc->ptable->super.block_shift;
          args[0] = block;
          result = fs_device_erase_block (gc->dev, block, FS_FOP_USER);
          if (result != FS_DEVICE_OK)
          {
            (void) fs_device_mark_block_bad (gc->dev, block);
            fs_log_add (&gc->log, FS_LOG_ENTRY_ERASE_FAIL, args);
            fs_log_flush (&gc->log);
            gc->ptable->set_reverse (gc->ptable,
                gc->gc_next & gc->ptable->super.block_mask,
                FS_RMAP_BLOCK_BAD, FS_PM_NULL_TAG);
            gc->state = FS_PM_HEAD;
            fs_pm_bump_gc_block (gc);

            break;
          }
          else
          {
            fs_log_add (&gc->log, FS_LOG_ENTRY_ERASE_FINISH, args);
            gc->state = FS_PM_HEAD;
            count = gc->ptable->erase (gc->ptable,
                gc->gc_next >> gc->ptable->super.block_shift);

            /* In some instances allocators will be invalid, so instead of
             * putting in array we can just assign it to the allocator */
            if (gc->solid_data_alloc == FS_INVALID_PAGE_ID)
              gc->solid_data_alloc = gc->gc_next
                                     & gc->ptable->super.block_mask;
            else if (gc->liquid_data_alloc == FS_INVALID_PAGE_ID)
              gc->liquid_data_alloc = gc->gc_next
                                      & gc->ptable->super.block_mask;
            else
              fs_pm_put_erased_blk (gc->gc_next
                                    & gc->ptable->super.block_mask);
            gc->free_count += count;
            gc->trans_start_free_count += count;
            fs_pm_bump_gc_block (gc);
          } /* !NAND */
        }
        else if (allow_erase_pending == FALSE)
        {
          if(gc->erase_state == FS_PM_ERASE_SUSPEND)
          {
            result = fs_device_resume_erase(gc->dev);
            while (result == FS_DEVICE_BUSY)
            {
              fs_util_wait(20);
              result = fs_device_erase_status(gc->dev);
            }
            if (result != FS_DEVICE_DONE)
            {
              /* Begin erase returned success, so when we check back the erase
                 status it should either be BUSY or DONE. We do not expect
                 erase to fail. If the NOR device returns anything other than
                 DONE then the part is no longer working and we cannot recover
                 from it. */
              FS_ERR_FATAL ("Flash erase failed : %d", result, 0, 0);
            }
            gc->erase_state = FS_PM_ERASE_IDLE;
          }
          else
          {
            int block = (gc->gc_next >> gc->ptable->super.block_shift);
            gc->ptable->begin_erase (gc->ptable, block);
            result = fs_device_erase_block (gc->dev, block, FS_FOP_USER);
          }
          if (result != FS_DEVICE_DONE)
          {
            FS_ERR_FATAL ("Unable to erase block : %d", result, 0, 0);
          }

          /* Log the erase as finished. */
          args[0] = gc->gc_next >> gc->ptable->super.block_shift;
          fs_log_add (&gc->log, FS_LOG_ENTRY_ERASE_FINISH, args);

          gc->state = FS_PM_HEAD;

          count = gc->ptable->erase (gc->ptable,
              gc->gc_next >> gc->ptable->super.block_shift);
          gc->free_count += count;
          gc->trans_start_free_count += count;

          fs_pm_bump_gc_block (gc);
        }
        else
        {
          gc->gc_current_erase_block = gc->gc_next >>
            gc->ptable->super.block_shift;
          gc->ptable->begin_erase (gc->ptable, gc->gc_current_erase_block);
          result = fs_device_begin_erase_block (gc->dev,
              gc->gc_current_erase_block, FS_FOP_USER);
          if (result != FS_DEVICE_DONE)
          {
            FS_ERR_FATAL ("Unable to erase block", 0, 0, 0);
          }
          gc->erase_state = FS_PM_ERASE_BUSY;
          fs_time_ms(gc->erase_start);
          gc->erase_countdown = gc->erase_time;
        }

        /* A unit of work is done, so return. */
        return;
      } /* case FS_PM_ERASE */

      default:
      {
        FS_ERR_FATAL ("Internal state error", 0, 0, 0);
      }
      break;
    }
  }
}

/* Returns number of pages added to the log */
static int
fs_pm_top_off_log (fs_pm_flash_t gc)
{
  int did_something = 0;
  fs_page_id tmp;

  while (FS_ISNAND (&gc->ptable->super) &&
      fs_log_page_count (&gc->log) < FS_LOG_TOP_OFF_LIMIT)
  {
    tmp = fs_logr_alloc (&gc->logr);
    if ((tmp & (~gc->ptable->super.block_mask)) == 0) {
      /* Use the first one of each as a superblock. */
      gc->next_super_page = tmp;

      tmp = fs_logr_alloc (&gc->logr);
    }

    fs_log_add_page (&gc->log, tmp);

    did_something++;
  }

  if (FS_ISNOR (&gc->ptable->super) &&
      fs_log_page_count (&gc->log) < FS_LOG_TOP_OFF_LIMIT)
  {
    uint32 args[3];

    tmp = fs_pm_alloc (gc, SOLID_DATA_ALLOC_TAG);

    /* XXX: This new log entry breaks compatibility with previous versions,
     * so can be disabled, but is important for handling all bad-powerdown
     * cases. */
    args[0] = tmp;
    fs_log_add (&gc->log, FS_LOG_ENTRY_LOG_ALLOC, args);

    gc->ptable->set_reverse (gc->ptable, tmp, FS_RMAP_PAGE_LOG,
                             FS_PM_NULL_TAG);

    fs_log_add_page (&gc->log, tmp);

    did_something++;
  }
  return did_something;
}

/* Notify client functions interested in GC pointer updates */
static void
fs_pm_gc_update_nofity (fs_pm_flash_t gc)
{
  /* If shredding is ON and if gc_next is about to cross its shred stop
   * point, flag so that we can detect and stop shredding on our next
   * shred invocation.*/
  if ((gc->shred_state == SHRED_RUNNING) &&
      (gc->gc_next == gc->shred_stop))
  {
    gc->shred_stop = FS_INVALID_PAGE_ID;
  }

  if (gc->scrub_info.scrub_state == FS_SCRUB_RUNNING)
  {
    /* Check if we completed 1 DATA-region GC cycle. */
    if (gc->gc_next == gc->scrub_info.data_scrub_info.gc_pos_at_start)
    {
      gc->scrub_info.data_scrub_info.gc_finished_one_cycle = 1;
    }
  }
}

/* If the allocators are in midway in the block, then there could be some data
 * that is erased in the block. While doing GC, if the allocator doesn't move
 * out of this block, then GC doesn't happen on this block since allocator is
 * pointing to this block. This could cause potentially leaving some data
 * unshredded. So to avoid this problem, move the allocators until they are
 * pointing to the first page in the block. We need to mark the pages that
 * were jumped, as garbage marked logs, since if we don't generate
 * these and move the allocator to next free block, writing a page
 * in the next block and powercycling before log is flushed would cause the
 * allocators to be placed in this same block after power up init, then cause
 * a write failure in the block where write happened. So Generate garbage logs
 * for the pages that we are skipping and flush the logs before shredding.
 */
static void
fs_pm_advance_allocators_to_next_block (fs_pm_flash_t gc)
{
  fs_page_id dummy_allocated_page;
  uint16  marked_garbage_log_count = 0;
  uint32 args[1];

  /* If the allocators are not pointing to the first page in the block,
   * them move them until it happens, marking the pages garbage that are
   * skipped. */
  if (gc->solid_data_alloc != FS_INVALID_PAGE_ID)
  {
    while ((gc->solid_data_alloc &
             (gc->ptable->super.data->block_size - 1)) != 0)
    {
      dummy_allocated_page = fs_pm_alloc (gc, SOLID_DATA_ALLOC_TAG);
      gc->ptable->set_reverse (gc->ptable,
                               dummy_allocated_page,
                               FS_RMAP_PAGE_GARBAGE,
                               FS_PM_NULL_TAG);
      args[0] = dummy_allocated_page;
      /* Log it (the garbage marking) so that it helps during the next power up
       * init and the page status can be recovered.
       */
      fs_log_add (&gc->log, FS_LOG_ENTRY_GARBAGE, args);
      marked_garbage_log_count++;
    }
  }

  /* If the allocators are not pointing to the first page in the block,
   * them move them until it happens, marking the pages garbage that are
   * skipped. */
  if (gc->liquid_data_alloc != FS_INVALID_PAGE_ID)
  {
    while ((gc->liquid_data_alloc &
             (gc->ptable->super.data->block_size - 1)) != 0)
    {
      dummy_allocated_page = fs_pm_alloc (gc, LIQUID_DATA_ALLOC_TAG);
      gc->ptable->set_reverse (gc->ptable,
                               dummy_allocated_page,
                               FS_RMAP_PAGE_GARBAGE,
                               FS_PM_NULL_TAG);
      args[0] = dummy_allocated_page;
      /* Log it (the garbage marking) so that it helps during the next power up
       * init and the page status can be recovered.
       */
      fs_log_add (&gc->log, FS_LOG_ENTRY_GARBAGE, args);
      marked_garbage_log_count++;
    }
  }
  /* Flush the log so that the page status are recovered during an init. */
  if (marked_garbage_log_count > 0)
    fs_log_flush (&gc->log);
}

/* Shred is an iterative garbage collection process which erases the obsolete
 * data pages that were part of the deleted file or updated data of a file.
 * Each shred call will shred a part of the data region and returns if shred
 * has completed or not. If shred has not completed it should be called again
 * until it returns indicating that the shredding has completed.
 *
 * Return Value:
 * 1  : Shred is not completed yet.
 * 0  : Shred is completed.
 * -EPERM : Shreding is not supported.
 */
static int
fs_pm_shred (fs_pm_t gen_gc)
{
  fs_pm_flash_t gc = (fs_pm_flash_t) gen_gc;

  /* Shred is not supported for NOR devices. */
  if (FS_ISNOR (&gc->ptable->super))
    return -EPERM;

  /* Shred is the only PM op that writes to the allocator outside of PM
     transaction. If PM initialize is still pending, finish it first */
  fs_pm_complete_pending_init (gc);

  /* Check if shredding was done as a part of regular garbage collection.
   * If it was completed, then update shred state appropriately and return */
  if ((gc->shred_state == SHRED_RUNNING) &&
      (gc->shred_stop == FS_INVALID_PAGE_ID))
  {
    gc->shred_state = SHRED_IDLE;
    return 0;
  }

  /* If Shred is not started. Initialize and start the process. */
  if (gc->shred_state == SHRED_IDLE)
  {
    /* Make sure that the allocators are at the starting of the block, so
     * that they are not with some data that potentially needed to be
     * shredded */
    fs_pm_advance_allocators_to_next_block (gc);

    /* Now store our shred stop point and update the shred state */
    gc->shred_stop = gc->gc_next & gc->ptable->super.block_mask;
    gc->shred_state = SHRED_RUNNING;
  }

  /* Shred is still running, just progress GC a bit and return */
  fs_pm_single (gc, FALSE);

  /* If GC ran then we might have to do GC in the ptable region */
  fs_pm_do_urgent (gc, FALSE);

  /* Shred is not done yet, come back again */
  return 1;
}

/* Set up LOG region scrub variables and state */
static void
fs_pm_log_region_scrub_start (fs_pm_flash_t gc)
{
  fs_pm_scrub_log_region_info *log_scrub = &(gc->scrub_info.log_scrub_info);

  memset (log_scrub, 0x0, sizeof (fs_pm_scrub_log_region_info));
  log_scrub->log_head_at_start = gc->ptable->super.data->log_head;
  log_scrub->log_flush_at_start = fs_log_peek_page (&gc->log);
}

/* Set up DATA region scrub variables and state */
static void
fs_pm_data_region_scrub_start (fs_pm_flash_t gc)
{
  fs_pm_scrub_data_region_info *data_scrub = &(gc->scrub_info.data_scrub_info);

  memset (data_scrub, 0x0, sizeof (fs_pm_scrub_data_region_info));

  data_scrub->gc_pos_at_start = gc->gc_next & gc->ptable->super.block_mask;

  /* GC skips allocator blocks. So move allocators to next blocks so the
     current allocator blocks also get scrubbed. */
  fs_pm_advance_allocators_to_next_block (gc);
}

/* Set up PTBL region scrub variables and state */
static void
fs_pm_ptable_region_scrub_start (fs_pm_flash_t gc)
{
  fs_pm_scrub_ptbl_region_info *ptbl_scrub = &(gc->scrub_info.ptbl_scrub_info);

  memset (ptbl_scrub, 0x0, sizeof (fs_pm_scrub_ptbl_region_info));
}

static void
fs_pm_scrub_start (fs_pm_flash_t gc)
{
  ASSERT (gc->scrub_info.scrub_state == FS_SCRUB_IDLE);
  gc->scrub_info.scrub_state = FS_SCRUB_RUNNING;

  fs_pm_log_region_scrub_start (gc);
  fs_pm_data_region_scrub_start (gc);
  fs_pm_ptable_region_scrub_start (gc);
}

static int
fs_pm_scrub_log_region (fs_pm_flash_t gc)
{
  int result = 1;
  uint32 min_data_clust_for_log_scrub;

  /* Log region scrubbing is based on the condition of log head having
     crossed flush at the time of scrub start so a reset will not read old log
     pages. Check if we have already written a new log head as expected. */
  if (gc->scrub_info.log_scrub_info.new_log_head_crossed_flush == 1)
  {
    gc->scrub_info.log_scrub_info.log_scrub_complete = 1;
    return 0;
  }

  /* It is rare for FS to have less than 2 blocks of data. If that is the case
     skip log scrubbing, since system is not going to remain in this state for
     long. */
  min_data_clust_for_log_scrub = gc->ptable->super.data->block_size * 2;
  if (gc->ptable->super.data->upper_data[FS_FIELD_NUM_ALLOC] <=
      min_data_clust_for_log_scrub)
  {
    gc->scrub_info.log_scrub_info.log_scrub_complete = 1;
    return 0;
  }

  return result;
}


static int
fs_pm_scrub_data_region (fs_pm_flash_t gc)
{
  int result = 1;

  if (gc->scrub_info.data_scrub_info.gc_finished_one_cycle)
    result = 0;

  return result;
}

static int
fs_pm_scrub_ptbl_region (fs_pm_flash_t gc)
{
  int result;

  result = gc->ptable->scrub (gc->ptable);
  return result;
}

static void
fs_pm_scrub_finish (fs_pm_flash_t gc)
{
  ASSERT (gc->scrub_info.scrub_state == FS_SCRUB_RUNNING);
  gc->scrub_info.scrub_state = FS_SCRUB_IDLE;
}

/* Perform one unit of scrubbing as required by all regions */
static int
fs_pm_scrub_update (fs_pm_flash_t gc)
{
  int result = 1;

  /* Top off logs before attempting GC */
  fs_pm_top_off_log (gc);

  /* Do GC until all the 3 regions says SCRUB-opreration is complete. */
  fs_pm_single (gc, FALSE);

  /* Data GC would have added logs and written to jouranl. Do ptable GC until
     enough free space is reclaimed. */
  fs_pm_do_urgent (gc, FALSE);

  if (!gc->scrub_info.data_scrub_info.data_scrub_complete)
  {
    result = fs_pm_scrub_data_region (gc);
    if (result == 0)
      gc->scrub_info.data_scrub_info.data_scrub_complete = 1;
  }

  if (!gc->scrub_info.ptbl_scrub_info.ptbl_scrub_complete)
  {
    result = fs_pm_scrub_ptbl_region (gc);
    if (result == 0)
      gc->scrub_info.ptbl_scrub_info.ptbl_scrub_complete = 1;
  }

  if (!gc->scrub_info.log_scrub_info.log_scrub_complete)
  {
    result = fs_pm_scrub_log_region (gc);
    if (result == 0)
      gc->scrub_info.log_scrub_info.log_scrub_complete = 1;
  }

  result  = 1; /* Initialize result to scrub pending */

  if (gc->scrub_info.data_scrub_info.data_scrub_complete &&
      gc->scrub_info.ptbl_scrub_info.ptbl_scrub_complete &&
      gc->scrub_info.log_scrub_info.log_scrub_complete)
  {
    fs_pm_scrub_finish (gc);
    result = 0;
  }

  return result;
}


static int
fs_pm_scrub (fs_pm_t gen_gc)
{
  int result = -EACCES;
  fs_pm_flash_t gc = (fs_pm_flash_t) gen_gc;

  /* Scrub is supported only for NAND. */
  if (!FS_ISNAND (&gc->ptable->super))
    return -EPERM;

  /* Since pages will be written, finish any pending init */
  fs_pm_complete_pending_init (gc);

  switch (gc->scrub_info.scrub_state)
  {
    case FS_SCRUB_IDLE:
    {
      fs_pm_scrub_start (gc);
      result = 1;
    }
    break;

    case FS_SCRUB_RUNNING:
    {
      result = fs_pm_scrub_update (gc);
    }
    break;

    default:
    {
      FS_ERR_FATAL ("%d, Unknown state of scrub", gc->scrub_info.scrub_state,
                    0, 0);
    }
    break;
  }

  return result;
}

/* Perform as much garbage collection as is necessary, in light of urgency. */
static void
fs_pm_do_urgent (fs_pm_flash_t gc, boolean allow_gc)
{
  int did_something;
  int new_log_pages = 0;
  int log_pages = 0;

  do
  {
    did_something = 0;

    if((log_pages = fs_pm_top_off_log(gc)) > 0)
    {
      new_log_pages += log_pages;
      did_something=1;
    }

    if(allow_gc && (fs_pm_urgency (gc) >= 80) && (!gc->inside_xact))
    {
      fs_pm_single (gc, FALSE);
      did_something = 1;
    }

#ifdef FEATURE_EFS_EFS2_ON_NAND
    if (FS_ISNAND (&gc->ptable->super))
    {
      did_something |= nand_ptable_do_gc (gc);
    }
#endif
  }
  while (did_something);

  /* Remember where the free count is to count how many pages were consumed
   * during the efs operation */
  gc->trans_start_free_count = gc->free_count + new_log_pages;
}

static void
fs_pm_page_write (fs_pm_t gen_gc, fs_cluster_id cluster,
                            void *buffer, uint16 tag)
{
  fs_pm_flash_t gc = (fs_pm_flash_t) gen_gc;
  fs_page_id old_index;
  fs_page_id new_index;
  int result;
  uint32 args[32];
  int error_count = 0;

  ASSERT (gc->inside_xact);

  old_index = gc->ptable->get_forward (gc->ptable, cluster);
  new_index = fs_pm_alloc (gc, tag);

  /* printf ("Write page: %d  at %X  for %d\n", cluster, new_index, tag); */

  result = fs_device_write_page (gc->dev, new_index, buffer,
      FS_FOP_USER);
  while (result != FS_DEVICE_OK) {
    error_count++;
    if (error_count > 2)
      FS_ERR_FATAL ("Too many write errors, most likely hardware problem",
          0, 0, 0);

    /* Write error.  Mark the rest of this block as bad, and try an
     * allocation at the next available block. */
    fs_pm_mark_bad (gc, new_index, tag);

    /* XXX: If transactions are changed so that they can split across log
     * nodes, add a log_flush here.  Adding it now will split any
     * transaction here.  The advantage to the flush is that a powerdown
     * shortly after a bad page write is more likely to be logged.
     * Otherwise, we may not catch the write failure until next time. */

    /* Get another page, and try writing it. */
    new_index = fs_pm_alloc (gc, tag);
    result = fs_device_write_page (gc->dev, new_index, buffer,
        FS_FOP_USER_RECOVERY);

    /* Loop back around, and check the result again. */
  }

  args[0] = cluster | ((tag & TAG_MASK) << TAG_SHIFT);

  /* Add logs as appropriate. */
  if (old_index == FS_INVALID_PAGE_ID) {
    args[1] = new_index;
    fs_log_add (&gc->log, FS_LOG_ENTRY_NEW_DATA, args);
  } else {
    args[1] = old_index;
    args[2] = new_index;
    fs_log_add (&gc->log, FS_LOG_ENTRY_PAGE_MOVE, args);
  }

  gc->ptable->set_forward (gc->ptable, cluster, new_index);
  if (old_index != FS_INVALID_PAGE_ID)
    gc->ptable->set_reverse (gc->ptable, old_index,
                             FS_RMAP_PAGE_GARBAGE, FS_PM_NULL_TAG);
  gc->ptable->set_reverse (gc->ptable, new_index, cluster, tag);
}

static void
fs_pm_page_read_core (fs_pm_t gen_gc, fs_cluster_id cluster, void *buffer)
{
  fs_pm_flash_t gc = (fs_pm_flash_t) gen_gc;
  fs_page_id page;

  page = gc->ptable->get_forward (gc->ptable, cluster);

  if (page == FS_INVALID_PAGE_ID) {
    memset (buffer, 0xFF, gc->ptable->super.data->page_size);
  } else {
    fs_device_read_page (gc->dev, page, buffer, FS_FOP_USER,
        FS_FLASH_MUST_WORK);
  }

}

static void
fs_pm_page_read (fs_pm_t gen_gc, fs_cluster_id cluster, void *buffer)
{
  fs_pm_flash_t gc = (fs_pm_flash_t) gen_gc;
  int was_suspended_here;

  /* If we are suspending an erase to perform this read, note it
   * so that we can resume later. */
  if (gc->erase_state == FS_PM_ERASE_BUSY) {
    fs_pm_suspend_erase (gc, FALSE);
    was_suspended_here = 1;
  } else {
    was_suspended_here = 0;
  }

  fs_pm_page_read_core (gen_gc, cluster, buffer);

  /* Resume suspended erase (NOR), but only if we did the suspend. */
  if (was_suspended_here && (gc->erase_state == FS_PM_ERASE_SUSPEND))
    fs_pm_inc_gc_erase (gc);
}

static void
fs_pm_start_transaction (fs_pm_t gen_gc)
{
  fs_pm_flash_t gc = (fs_pm_flash_t) gen_gc;

  ASSERT (!gc->inside_xact);

  fs_pm_complete_pending_init (gc);

  if (FS_ISNOR (&gc->ptable->super))
  {
    fs_pm_inc_gc_start_op(gc);
    fs_pm_do_urgent (gc, TRUE);
    /* Check if we have enough room in the log buffer to accomodate the largest
     * possible transaction that is about to begin. Since we don't have way
     * of telling different kinds of log types, just add 1 to the max possible
     * transactions to compensate for the start and end transaction logs. If
     * there is enough room this call just returns, otherwise would flush
     * the log and return. This is only needed for NOR since writes are very
     * costly here and slow. */
    fs_log_flush_if_overflows (&gc->log, FS_LOG_ENTRY_PAGE_MOVE,
                               FS_PM_MAX_TRANSACTION_PAGES + 1);
  }
  else
  {
    fs_ptable_nand_t pt_gc = (fs_ptable_nand_t)gc->ptable;

    /* Now our transaction is starting. So, for NAND, initialize our counter
     * to keep track of how many overhead page writes we do in this txn, so
     * that we can do conscious decision of doing GC appropriately and spread
     * the GC moving load to multiple transactions instead of overloading a
     * single transaction.
     * NOTE here that we are clearing the counters for both data region GC and
     * the PTable region GC. */
    pt_gc->ptable_gc.overhead_write_count = 0;
    gc->overhead_write_count = 0;

    fs_pm_do_urgent (gc, TRUE);
    fs_log_flush (&gc->log);
  }

  fs_log_add (&gc->log, FS_LOG_ENTRY_XACT_START, NULL);
  gc->inside_xact = 1;
}

#ifdef FEATURE_EFS_EFS2_ON_NAND

/* Check if curr_page lies within start_page and end_page */
static int
is_page_in_the_range (fs_page_id curr_page, fs_page_id start_page,
                      fs_page_id end_page)
{
  int is_page_in_range = 0;

  ASSERT(start_page != end_page);

  if (start_page < end_page)
  {
    if ((curr_page > start_page) && (curr_page < end_page))
      is_page_in_range = 1;
  }
  else
  {
    if ((curr_page > start_page) || (curr_page < end_page))
      is_page_in_range = 1;
  }

  return is_page_in_range;
}

/* Notify client functions interested in Super updates */
static void
fs_pm_super_update_notify (fs_pm_flash_t gc)
{
  if (gc->scrub_info.scrub_state == FS_SCRUB_RUNNING)
  {
    fs_page_id curr_page, start_page, end_page, page_inside_range;

    /* Check if new log-head is a newer log than log_flush when we started
       scrub. */
    curr_page = gc->ptable->super.data->log_head;
    start_page = gc->scrub_info.log_scrub_info.log_head_at_start;
    end_page = gc->scrub_info.log_scrub_info.log_flush_at_start;

    if (start_page == curr_page)
    {
      page_inside_range = 1;
    }
    else
    {
      page_inside_range = is_page_in_the_range (curr_page, start_page,
                                                end_page);
    }

    if (!page_inside_range)
    {
      gc->scrub_info.log_scrub_info.new_log_head_crossed_flush = 1;
    }
  }
}

static void
fs_pm_log_data_write_super_page (fs_pm_flash_t gc,
                                 enum update_loghead_type update_loghead)
{
  int result;

  /*printf ("super-write : %d,%d\n",
            gc->next_super_page>>gc->ptable->super.block_shift,
            gc->next_super_page&~gc->ptable->super.block_mask);*/
  ASSERT (gc->next_super_page != FS_INVALID_PAGE_ID);

  if (update_loghead == UPDATE_LOG_HEAD)
  {
    gc->ptable->super.data->log_head = fs_log_peek_written (&gc->log);
  }

  gc->ptable->super.data->alloc_next[0] = gc->solid_data_alloc;
  gc->ptable->super.data->alloc_next[2] = gc->liquid_data_alloc;
  gc->ptable->super.data->gc_next[0] = gc->gc_next;

  /* Make sure other values get updated as well. */
  gc->ptable->super_update (gc->ptable);

  result = fs_super_update (&gc->ptable->super, gc->next_super_page);
  if (result != FS_DEVICE_OK)
  {
    fs_block_id super_blk;

    /*printf("super-page write failed blk[%d] page[%d]\n",
             gc->next_super_page >> gc->ptable->super.block_shift,
             gc->next_super_page);*/

    /* Since super-page is the first page in the block that went bad, we don't
     * need this block anymore, so we can mark it as bad in the flash
     * immediately. */
    super_blk = gc->next_super_page >> gc->ptable->super.block_shift;
    result = fs_device_mark_block_bad (gc->ptable->super.dev, super_blk);
    if (result != FS_DEVICE_OK)
      FS_ERR_FATAL ("Mark block bad failed...",0,0,0);

    fs_pm_log_page_write_fail_handler (gc, gc->next_super_page);
  }
  else
  {
    /* Since we have just now flushed the super-block, log_head might have
     * advanced to the next blocks, if so, then now would be the correct time
     * to mark any bad-blocks that are not needed any more as bad in flash as
     * well. */
    fs_logr_mark_bad_blocks (&gc->logr, gc->next_super_page,
                             gc->ptable->super.data->log_head);
    gc->next_super_page = FS_INVALID_PAGE_ID;

    fs_pm_super_update_notify (gc);
  }
}

static int
nand_ptable_do_gc (fs_pm_flash_t gc)
{
  return gc->ptable->do_gc (gc->ptable);
}
#endif

/* NOR specific cleanup of the logs. */
static void
cleanup_logs (fs_pm_flash_t gc)
{
  fs_page_id page;
  fs_page_id buffer;
  fs_page_id  last_written_page = FS_INVALID_PAGE_ID;
  int result;

  /* The log needs to stay around in NAND until it has been dealt with by
   * the NAND ptable gc code. */
  if (FS_ISNAND (&gc->ptable->super)) {
    return;
  } else {
    while ((page = fs_log_get_written (&gc->log)) != FS_INVALID_PAGE_ID) {
      gc->ptable->flush (gc->ptable);

      /* Now it is safe to change the state of that log page. */
      if (FS_ISNOR (&gc->ptable->super)) {
        buffer = 0xFFFFFF00;
        result = fs_device_partial_write (gc->dev, page, &buffer, 4, 4,
                                          FS_FOP_LOG);
        if (result != 0)
          FS_ERR_FATAL ("Unable to write log state change", 0, 0, 0);
      }
      /* Last written log-page is saved to be used for a possible log-head.
       */
      last_written_page = page;
    }
  }

  /* Check to see if there is an appropriate superblock to write out. */
  if ((last_written_page != FS_INVALID_PAGE_ID) && !gc->inside_xact &&
      gc->next_super_page != FS_INVALID_PAGE_ID)
  {
    /* Last written log-page is the appropiate log head */
    gc->ptable->super.data->log_head = last_written_page;
    gc->ptable->super.data->alloc_next[0] = gc->solid_data_alloc;
    gc->ptable->super.data->alloc_next[2] = gc->liquid_data_alloc;
    gc->ptable->super.data->gc_next[0] = gc->gc_next;

    fs_super_update (&gc->ptable->super, gc->next_super_page);

    gc->next_super_page = FS_INVALID_PAGE_ID;
  }
}

#ifdef FS_UNIT_TEST
void (*end_transaction_hook) (void) = NULL;
#endif

static void
fs_pm_end_transaction (fs_pm_t gen_gc)
{
  fs_pm_flash_t gc = (fs_pm_flash_t) gen_gc;

  ASSERT (gc->inside_xact);

  fs_log_add (&gc->log, FS_LOG_ENTRY_XACT_END, NULL);

  fs_log_flush (&gc->log);
  gc->inside_xact = 0;

  if (FS_ISNOR (&gc->ptable->super)) {
    fs_pm_inc_gc_end_op(gc);
    fs_log_flush (&gc->log);
  }

  cleanup_logs (gc);

  /* Not strictly necessary. */
  if (FS_ISNAND (&gc->ptable->super)) {
    fs_log_flush (&gc->log);
  }

  if (FS_ISNOR (&gc->ptable->super)) {
    fs_pm_inc_gc_erase(gc);
  }

#ifdef FS_UNIT_TEST
  if (end_transaction_hook != NULL)
    end_transaction_hook ();
#endif
}

static void
fs_pm_store_info (fs_pm_t gen_gc, unsigned offset, uint32 data)
{
  fs_pm_flash_t gc = (fs_pm_flash_t) gen_gc;

  uint32 args[2];

  args[0] = offset;
  args[1] = data;
  fs_log_add (&gc->log, FS_LOG_ENTRY_UPPER_DATA, args);

  gc->ptable->super.data->upper_data[offset] = data;
}

static uint32
fs_pm_get_info (fs_pm_t gen_gc, unsigned offset)
{
  fs_pm_flash_t gc = (fs_pm_flash_t) gen_gc;

  return gc->ptable->super.data->upper_data[offset];
}

/* Specifies the percentage of the EFS partition that is used to store data in
 * NOR filesystems when used with SPI-NOR Storage. Can be used for debugging,
 * performance analysis without requiring to recompile a new build. Set this
 * value to the % of the partition to use for data. Set to a default of 70 %.
 */
volatile uint32 fs_usable_data_percentage_debug = 70;

/* Compute the space limit for the lower layer code. This allows enough
 * room for the garbage collector to run, a bad powerdown during a
 * transaction, and to recover. This number is fairly conservative, but
 * gives reasonable performance. The number returned is a count of pages
 * available. */
uint32
fs_pm_space_limit (fs_pm_t gen_gc)
{
  fs_pm_flash_t gc = (fs_pm_flash_t) gen_gc;

  uint32 block_count = fs_device_block_count (gc->dev);
  uint32 block_size = fs_device_block_size (gc->dev);
  uint32 size, nor_free_count;

  if (FS_ISNOR (&gc->ptable->super))
  {
    if (block_count < 8)
    {
      size = (block_size + (block_size / 2));
    }
    else
    {
      size = 2 * block_size;
    }
    size += FS_MAX_TRANSACTION_DATA_PAGES + 2;

    /* Check if we allow more than what we can. We need urgency overhead
     * amount of physical free space. So we should have more reserve than this
     * in the event if the file system is completely filled to 100% we should
     * still be able to do GC. This doesn't include the log pages and some
     * more additional overhead of super page, rev ptable, garbage etc. For
     * that additional 15% is kept as overhead in free space limit so it cannot
     * be occupied. */
    nor_free_count = get_nor_urgency_free_count (gc);
    ASSERT (size >= nor_free_count);

    size = (block_count * block_size) - size;

    /* Having debug varibale larger than 100 can make size bigger than device
       itself. */
    ASSERT (fs_usable_data_percentage_debug < 100);

    /* SPI-NOR devices are slower than usual NOR devices and hence consume
     * more time for Garbage collection. To reduce the amount of urgent garbage
     * collection to be done during a transaction we reduce the fraction of
     * space available for data compared to NOR to increase the possibility of
     * finding contiguous fully erased blocks and hence decreasing the GC
     * activity when FS is close to being full. */
    if (gc->dev->efs_device_type == FS_DEVICE_TYPE_EFS_SPI_NOR)
    {
      size = (size * fs_usable_data_percentage_debug) / 100;
    }
    else /* For NOR use 84.5 % which is the well tested limit. */
    {
      size = (size * 217) >> 8;
    }
  }
  else
  {
    uint32 bad_blk_limit, region_size;

    region_size = gc->ptable->super.data->u.nand.regions[0] * block_size;

    bad_blk_limit = get_bad_block_allowance (region_size, block_size);

    /* On NAND devices, we set the usable space to 95% of the data region,
     * then subtract another five flash blocks which are needed for GC
     * overhead and subtract the amount of bad blocks allowance needed */
    size = ((region_size * 243) >> 8) - (5 * block_size) - bad_blk_limit;

    /* Make sure that the bad block configuration didn't go extreme and consume
     * more than half space for bad blocks and GC overhead. */
    ASSERT (size > (region_size / 2));
  }

  return size;
}

static void fs_pm_finish_erase( fs_pm_flash_t gc )
{
  uint32 args[3];  /* Args for log */
  int free_pages;

  gc->erase_state = FS_PM_ERASE_IDLE;

  /* Finish erase in log */
  args[0] = gc->gc_current_erase_block;
  fs_log_add (&gc->log, FS_LOG_ENTRY_ERASE_FINISH, args);

  /* Update gc->free_count */
  free_pages = gc->ptable->erase (gc->ptable, gc->gc_current_erase_block);
  gc->free_count += free_pages;
  /* Correct error in start free count */
  gc->trans_start_free_count += free_pages;

  /* Advance gc->gc_next to start garbage collecting next block */
  fs_pm_bump_gc_block (gc);
  gc->state = FS_PM_HEAD;
}

/* Converting elapsed time into work is assumes that a block of work happens
 * in gc->erase_time.  One block of pages was erased, and a page erase is
 * equivalent to gc->erase_factor/10 worth of page moves.  A page move is the
 * basic unit of work */
#define TIME_TO_WORK(gc,t,bs) ((t)*(bs)*((gc)->erase_factor)/ \
    (10*((gc)->erase_time)))

/* Compute time since last erase start/resume */
static int
fs_pm_erase_time_elapsed (fs_pm_flash_t gc)
{
  qword now;
  qword elapsed;

  fs_time_ms (now);
  fs_qw_sub (elapsed, now, gc->erase_start);
  if(fs_qw_hi (elapsed) != 0)
  {
    /* If erase time elapsed too large or negative, return 0; */
    return 0;
  }

  return fs_qw_lo (elapsed);
}

/* fs_pm_suspend_erase( gc, do_work)
     do_work (boolean) : indicates that erase work is allowed.
       When do_work is false, this function suspends the active
       erase and updates unfinished work
       When do_work is true, this function will wait until either
       the erase is done, or unfinished work is done.
*/
static void
fs_pm_suspend_erase (fs_pm_flash_t gc, boolean do_work)
{
  uint32 block_size = fs_device_block_size (gc->dev);
  boolean finish_erase = FALSE;
  int status;

  if( gc->erase_state == FS_PM_ERASE_SUSPEND && do_work &&
      (gc->unfinished_work > 0))
  {
    /* An erase was previously suspended, but work needs to be done */
    /* Restart the erase */
    fs_pm_inc_gc_erase (gc);
    /* State is now either ERASE_BUSY or ERASE_DONE.  Fall through
     * And do the necessary work. */
  }

  if( gc->erase_state == FS_PM_ERASE_BUSY)
  {
    int work = gc->unfinished_work;

    /* An erase was running, check erase status */
    if(fs_device_erase_status (gc->dev) == FS_DEVICE_DONE)
    {
      /* Erase complete */
      finish_erase = TRUE;
      /* Account for work remaining */
      if(gc->erase_countdown > 0) {
        gc->unfinished_work -= TIME_TO_WORK(gc, gc->erase_countdown,
                                            block_size);
      }
    } else {

      /* Remove amount of time consumed so far from unfinished work. */
      work -= TIME_TO_WORK(gc, fs_pm_erase_time_elapsed(gc), block_size);

      /* Loop wait for remaining erase work to complete */
      if(do_work && (work > 0))
      {
        do
        {
          fs_util_wait(FS_NOR_ERASE_INCREMENT);
          status = fs_device_erase_status(gc->dev);
          work -= TIME_TO_WORK(gc, FS_NOR_ERASE_INCREMENT, block_size);

        } while (status != FS_DEVICE_DONE && work > 0);

        if(status == FS_DEVICE_DONE)
        {
          /* Erase finished in the last 10ms */
          finish_erase = TRUE;

          /* Account for work remaining */
          if(gc->erase_countdown > 0) {
            gc->unfinished_work -= TIME_TO_WORK(gc, gc->erase_countdown,
                block_size);
          }
        }
      }

      /* The work is done, but the erase is still going */
      if(! finish_erase )
      {
        int elapsed_time;

        /* Erase is not done yet, suspend erase */
        status = fs_device_suspend_erase( gc->dev, FS_FOP_USER);
        elapsed_time = fs_pm_erase_time_elapsed(gc);

        if (status == FS_DEVICE_FAIL) {
          FS_ERR_FATAL ("Unable to suspend erase", 0, 0, 0);
        } else  if(status == FS_DEVICE_BUSY) {
          /* Erase suspended success */
          gc->erase_state = FS_PM_ERASE_SUSPEND;

          /* Remove amount of time consumed so far from unfinished work. */
          if(elapsed_time > gc->erase_countdown) {
            /* Erase finished early, only remove expected erase time from
               unfinished work. */
            if(gc->erase_countdown > 0) {
              gc->unfinished_work -= TIME_TO_WORK(gc, gc->erase_countdown,
                  block_size);
            }
          } else {
            /* Erase is still going, deduct elapsed time from work */
            gc->unfinished_work -= TIME_TO_WORK(gc, elapsed_time, block_size);
          }
          /* Reduce countdown by elapsed time */
          gc->erase_countdown -= elapsed_time;

        }
        else if( status == FS_DEVICE_DONE )
        {
          /* Erase just finished during the suspend */
          finish_erase = 1;
          if(gc->erase_countdown > 0) {
            gc->unfinished_work -= TIME_TO_WORK(gc, gc->erase_countdown,
                block_size);
          }
        }
      }
    }

    if(finish_erase)
    {
      /* An erase finished, add to gc->free_count, and log it */
      fs_pm_finish_erase( gc );
    }
  }
}

static void
fs_pm_do_page_moves (fs_pm_flash_t gc)
{
  int i = 0;

  while((gc->unfinished_work > 0) &&
        (gc->state == FS_PM_MOVING || gc->state == FS_PM_HEAD))
  {
    /* Move a page */
    fs_pm_single(gc, TRUE);
    gc->unfinished_work--;
    if(i++ > 40)
    {
      fs_pm_top_off_log(gc);
      i = 0;
    }
  }
}

static void
fs_pm_inc_gc_do_work (fs_pm_flash_t gc)
{
  boolean early_exit = 0;
  while((gc->unfinished_work > 0) && !early_exit)
  {
    switch(gc->erase_state)
    {
      case FS_PM_ERASE_IDLE:

        if(gc->state == FS_PM_ERASE)
        {
          /* Erase is the next stage of garbage collection */
          if(gc->unfinished_work <
              ((int) fs_device_block_size (gc->dev) * 3/2) *
                gc->erase_factor / 10)
          {
            /* This is not a full block erase, let it run after the
               operation completes */
            early_exit = 1;
          } else {
            /* 1.5 blocks need to be done, do a blocking erase */
            fs_pm_single(gc, FALSE); /* Erase and wait for it */
            gc->unfinished_work -= fs_device_block_size (gc->dev) *
              gc->erase_factor / 10;
          }
        } else {
          /* Do page moves */
          fs_pm_do_page_moves(gc);
          fs_pm_top_off_log(gc);
        }
        break;

      case FS_PM_ERASE_SUSPEND:
        if(gc->unfinished_work > 0)
        {
          /* Finish the unfinished work */
          fs_pm_suspend_erase(gc, TRUE);
        }
        break;

      case FS_PM_ERASE_BUSY:
        fs_pm_suspend_erase(gc, TRUE);
        break;
    }
  }

  /* Even if no work is being done, be sure to suspend the erase */
  if(gc->erase_state == FS_PM_ERASE_BUSY)
  {
    fs_pm_suspend_erase(gc, FALSE);
  }
}

/* Perform transaction start tasks for the incremental garbage collector */
static void
fs_pm_inc_gc_start_op (fs_pm_flash_t gc)
{
  fs_pm_inc_gc_do_work(gc);
  gc->trans_start_free_count = gc->free_count;

  fs_pm_inc_gc_do_work(gc);
}

/* Tuning values for how much garbage to collect based on the erased space.
 * a value 100, means collect garbage equal to pages used.  A value 0 means
 * collect no garbage.  The index of inc_gc_tune[] is the flash full ratio
 * to the nearest 10th.  tune[0] is the value for flash < 10% full.
 * tune[2] is the tune value for flash < 20% full. tune[11] is for
 * 110% full (when the reserved space for gc is being used).
 */
int inc_gc_tune[11] = {0,0,0,0,0,50,65,80,90,115,180};
/* Perform transaction end tasks for the incremental garbage collector */
static void
fs_pm_inc_gc_end_op (fs_pm_flash_t gc)
{
  int block_size;
  int garbage_ratio;  /* average garbage present in 1000 pages */
  int pages_used;     /* pages used from the erased region this operation */
  int fs_size;        /* Size of the whole filesystem in pages*/
  int data_size;      /* Size of the data in pages */
  int erase_size;     /* Size of the erased region in pages */
  int garbage_size;   /* Size of the garbage in pages */
  int pages_to_clean; /* Number of pages to clean up */
  int tune;           /* A tuning value from inc_gc_tune[] */
  uint32 block_count = fs_device_block_count (gc->dev);
  int work;

  /* Compute free space used by this operation */
  pages_used = gc->trans_start_free_count - gc->free_count;
  if(pages_used < 0)
  {
    FS_ERR_FATAL("pages consumed should be positive, %d\n", pages_used,0,0);
  }

  /* Compute the ratio of garbage to data in the non-erased pages */
  /* since those pages will be garbage collected.  Remove the reserved */
  /* 2 blocks and 20 pages that is needed for urgent garbage collection */
  block_size = fs_device_block_size(gc->dev);

  fs_size = fs_device_block_count(gc->dev) * block_size
                    - get_nor_urgency_free_count (gc);
  erase_size = gc->free_count - get_nor_urgency_free_count (gc);

  tune = 10  * (fs_size - erase_size) / fs_size;
  if(tune < 0) tune = 0;
  else if(tune > 10) tune = 10;

  /* Now look up the tuning value */
  tune = inc_gc_tune[tune];

  if(erase_size < 0) {
    /* free space is urgently low */
    /* increase fs_size enough to keep erase_size non-negative */
    fs_size -= erase_size;
    erase_size = 0;
  }

  /* Only perform garbage collection if ::
   *  tuning factor is non-zero &&
   *  erase_size is less then FS_NOR_MAX_ERASED_BLOCKS.
   *
   * If erase_size is more then FS_NOR_MAX_ERASED_BLOCKS then there
   * is enough free_space available between GC-ptr and Alloc-ptr.
   * Hence no need to do further GC.
   */
  if( (tune > 0) && (erase_size < FS_NOR_MAX_ERASED_BLOCKS * block_size))
  {
    /* If the allocation code is up enough to have registered a more
     * accurage counter, use it, otherwise estimate with the field from the
     * superblock. */
    if (gc->alloc_count_cb != NULL) {
      data_size = gc->alloc_count_cb (gc->alloc_count_data);
    } else {
      data_size = gc->ptable->super.data->upper_data[FS_FIELD_NUM_ALLOC];
    }

    garbage_size = fs_size - erase_size - data_size;

    /* Sanity check, since until the allocation code is up, the values
     * could estimate too much data.  Never allow these values to become
     * less than 1. */
    if (garbage_size < 1)
      garbage_size = 1;

    garbage_ratio = 1000 * garbage_size / (fs_size - erase_size);
    if (garbage_ratio < 1)
      garbage_ratio = 1;

    /* Compute the amount of garbage to clean up
     * tune is fixed point *100, and garbage_ratio is *1000 */
    pages_to_clean = tune * pages_used * 10 / garbage_ratio;

    work = pages_to_clean * (10 + gc->erase_factor)/10;

    if (block_count < 8 ) {
    /* If the file system is small then there is a chance of gc_next moving
     * fast and start garbage collecting the block which still has unwritten
     * log pages that are in the log queue to be written. In this instance if
     * gc_next crosses these pages and manages to erase the block, next log
     * flush might write these erased pages, but still the allocator might
     * allocate these Pages to different clusters. Then the write would fail
     * since the is not erased anymore.
     *
     * To overcome this, we can run the incremental garbage collector slowly
     * for small configuration. We can calculate whether the gc_next from the
     * current work load would write on a currently useful log page. Work
     * value is zeroed to stop the incremental GC in that case.
     */
      work = work/2;
      if (fs_log_written_count (&gc->log) != 0){
        uint32 gc_block, log_block, log_page;
         gc_block = ( ((gc->gc_next + work) > gc->max_page) ?
                       gc->min_page : (gc->gc_next + work))
                     >> gc->ptable->super.block_shift;
        log_page = fs_log_peek_written (&gc->log);
        log_block = log_page >> gc->ptable->super.block_shift;

        /* Check if future GC might happen on the block
         * where next log page will be written */
        if (log_block == gc_block){
          /* If yes, then incremental GC need not run. Stop it. */
          work = 0;
          gc->unfinished_work = 0;
        }
      }
    }
    gc->unfinished_work += work;
    /*printf("GC %d pages, op %d, data %d work (%d,%d) ratio %.3f alloc 0x%x"
           " next 0x%x\n",
           pages_to_clean, pages_used, data_size, work,gc->unfinished_work,
           garbage_ratio/1000.0, gc->solid_data_alloc, gc->gc_next);*/

    fs_pm_do_page_moves(gc);
    /* Any erase work will be done between operations, starting at
       fs_pm_inc_gc_erase() */
  }
}

static void
fs_pm_inc_gc_erase (fs_pm_flash_t gc)
{
  /* Resume a suspended erase */
  if( gc->erase_state == FS_PM_ERASE_SUSPEND)
  {
    /* Resume erase */
    if(fs_device_resume_erase (gc->dev) == FS_DEVICE_FAIL)
    {
      FS_ERR("resume erase failed",0,0,0);
      /* Ok, try a new erase if possible */
      gc->erase_state = FS_PM_ERASE_IDLE;
    } else {
      fs_time_ms(gc->erase_start);
      gc->erase_state = FS_PM_ERASE_BUSY;
/*      printf("resume erase at %d, left %d\n", qw_lo(gc->erase_start),
             gc->erase_countdown);*/
    }
  }

  /* If the garbage collector is about to start an erase, then do it */
  if(gc->state == FS_PM_ERASE && gc->erase_state == FS_PM_ERASE_IDLE)
  {
    /* Start the erase */
    fs_pm_single(gc, TRUE);
    fs_time_ms(gc->erase_start);
    gc->erase_countdown = gc->erase_time;
    gc->erase_state = FS_PM_ERASE_BUSY;
/*    printf("start erase at %d left %d\n", qw_lo(gc->erase_start),
           gc->erase_countdown);*/
  }
}

#ifdef FS_UNIT_TEST
/* Ensure the integrity of the filesystem. */
void
fs_pm_validate (fs_pm_t gen_gc)
{
  fs_pm_flash_t gc = (fs_pm_flash_t) gen_gc;

  fs_cluster_id cl;
  fs_page_id page, forw;
  int err_count = 0;

  /* Make sure all of the forward entries are correct in the reverse. */
  for (cl = 0; cl < gc->max_page; cl++) {
    page = gc->ptable->get_forward (gc->ptable, cl);
    if (page != FS_INVALID_CLUSTER_ID) {
      forw = gc->ptable->get_reverse (gc->ptable, page, NULL);
      if (forw != cl) {
        printf ("forw mismatch: 0x%x, 0x%x, 0x%x\n", cl, page, forw);
        err_count++;
      }
    }
  }

  /* Also make sure that every reverse entry pointing to a cluster points
   * back to the right page.  This help catch duplicate pages. */
  for (page = 0; page < gc->max_page; page++) {
    cl = gc->ptable->get_reverse (gc->ptable, page, NULL);
    if (!FS_RMAP_IS_SPECIAL (cl)) {
      forw = gc->ptable->get_forward (gc->ptable, cl);
      if (forw != page) {
        printf ("rev mismatch: 0x%x, 0x%x, 0x%x\n", page, cl, forw);
        err_count++;
      }
    } else if (FS_RMAP_IS_TABLE (cl)) {
      /* Tables are not yet checked. */
    } else {
      switch (cl) {
        case FS_RMAP_BLOCK_UNKNOWN:
        case FS_RMAP_BLOCK_ERASED:
        case FS_RMAP_BLOCK_BAD:
        case FS_RMAP_PAGE_GARBAGE:
        case FS_RMAP_PAGE_SUPER:
        case FS_RMAP_PAGE_LOG:
        case FS_RMAP_PAGE_RESERVED:
          break;

        default:
          printf ("Page of invalid state: 0x%x, 0x%x\n", page, cl);
          err_count++;
      }
    }
  }

  if (err_count > 0)
    FS_ERR_FATAL ("validate mismatch: %d", err_count, 0, 0);
}
#endif

static int
fs_pm_sync_start (fs_pm_t pm_data)
{
  (void) pm_data;
  return 1; /* Nothing to sync. */
}

static int
fs_pm_sync_wait (fs_pm_t pm_data)
{
  (void) pm_data;
  return 0; /* Nothing to sync. */
}

static int
fs_pm_sync_end (fs_pm_t pm_data)
{
  (void) pm_data;
  return 0; /* Nothing to sync. */
}


static int
fs_pm_sync_no_wait (fs_pm_t pm_data, fs_sync_token_t *sync_token)
{
  (void) pm_data;
  *sync_token = 0; /* Give the always success sync token. */
  return 1; /* Nothing to sync. */
}

static int
fs_pm_sync_get_status (fs_pm_t pm_data, fs_sync_token_t sync_token,
                       enum fs_sync_result *sync_result)
{
  (void) pm_data;
  (void) sync_token;
  *sync_result = FS_SYNC_COMPLETE;
  return 0; /* Nothing to sync. */
}

/* Creating golden copy for PM other than remote storage shall return error. */
static int
fs_pm_make_golden_copy (fs_pm_t pm_data)
{
  (void) pm_data;
  return -EPERM;
}

static int
fs_pm_fs_init_complete (fs_pm_t pm_data)
{
  (void) pm_data;
  return -EPERM;
}

static void
fs_pm_nor_finish_current_erase (fs_pm_flash_t pm)
{
  int result;

  /* If a erase was suspended, lets resume this erase first. */
  if (pm->erase_state == FS_PM_ERASE_SUSPEND)
  {
    if(fs_device_resume_erase (pm->dev) == FS_DEVICE_FAIL)
    {
      FS_ERR("resume erase failed",0,0,0);
      pm->erase_state = FS_PM_ERASE_IDLE;
    }
    else
    {
      fs_time_ms (pm->erase_start);
      pm->erase_state = FS_PM_ERASE_BUSY;
    }
  }

  /* If a erase is going on, lets wait here until this erase finishes. */
  if (pm->erase_state == FS_PM_ERASE_BUSY)
  {
    do
    {
      fs_util_wait (20);
      result = fs_device_erase_status (pm->dev);
    } while (result == FS_DEVICE_BUSY);

    if (result != FS_DEVICE_DONE)
    {
      /* Begin erase returned success, so when we check back the erase status
         it should either be BUSY or DONE. We do not expect erase to fail.
         If the NOR device returns anything other than DONE then the part is no
         longer working and we cannot recover from it. */
      FS_ERR_FATAL ("Flash erase failed : %d", result, 0, 0);
    }

  }

}

static int
fs_pm_lock_efs (fs_pm_t pm_data)
{
  fs_pm_flash_t gc = (fs_pm_flash_t) pm_data;
  int result = -EPERM;

  /* We support EFS locking only for SPI-NOR. */
  if (gc->dev->efs_device_type == FS_DEVICE_TYPE_EFS_SPI_NOR)
  {
    fs_pm_nor_finish_current_erase (gc);
    result = 0;
  }

  return result;
}

static int
fs_pm_borrow_ram (fs_pm_t pm_data, void **ram_addr, uint32 *ram_size)
{
  fs_pm_flash_t gc = (fs_pm_flash_t) pm_data;

  ASSERT (gc != NULL);
  *ram_addr = NULL;
  *ram_size = 0;
  return -ENOMEM;
}

static int
fs_pm_invalid_store_data_in_sb (fs_pm_t pm_data, const void *data,
                                uint32 data_size)
{
  fs_pm_flash_t gc = (fs_pm_flash_t) pm_data;

  (void) data; (void) data_size;

  ASSERT (gc != NULL);
  return -EPERM;
}

static void
fs_pm_invalid_page_write (fs_pm_t gen_gc, fs_cluster_id cluster,
                  void *buffer, uint16 tag)
{
  (void) gen_gc; (void) cluster; (void) buffer; (void) tag;
}

static void
fs_pm_invalid_store_info (fs_pm_t gen_gc, unsigned offset, uint32 data)
{
  (void) gen_gc; (void) offset; (void) data;
}

static void
fs_pm_invalid_start_transaction (fs_pm_t gen_gc)
{
  (void) gen_gc;
}

static void
fs_pm_invalid_end_transaction (fs_pm_t gen_gc)
{
  (void) gen_gc;
}

static int
fs_pm_invalid_shred (fs_pm_t gen_gc)
{
  (void) gen_gc;
  return -EPERM;
}

static int
fs_pm_invalid_scrub (fs_pm_t gen_gc)
{
  (void) gen_gc;
  return -EPERM;
}

static void
fs_pm_init_read_only_ops (struct fs_pm_ops *ops)
{
  ops->page_write = fs_pm_invalid_page_write;
  ops->page_read = fs_pm_page_read_core;
  ops->store_info = fs_pm_invalid_store_info;
  ops->get_info = fs_pm_get_info;
  ops->start_transaction = fs_pm_invalid_start_transaction;
  ops->end_transaction = fs_pm_invalid_end_transaction;
  ops->space_limit = fs_pm_space_limit;
  ops->register_free_check_cb = fs_pm_register_free_check_cb;
  ops->register_alloc_count_cb = fs_pm_register_alloc_count_cb;
  ops->shred = fs_pm_invalid_shred;
  ops->scrub = fs_pm_invalid_scrub;
  ops->read_iovec = NULL;
  ops->write_iovec = NULL;
  ops->sync_start = fs_pm_sync_start;
  ops->sync_wait  = fs_pm_sync_wait;
  ops->sync_end   = fs_pm_sync_end;
  ops->sync_no_wait    = fs_pm_sync_no_wait;
  ops->sync_get_status = fs_pm_sync_get_status;
  ops->make_golden_copy = fs_pm_make_golden_copy;
  ops->fs_init_complete = fs_pm_fs_init_complete;
  ops->lock_efs = fs_pm_lock_efs;
  ops->borrow_ram = fs_pm_borrow_ram;
  ops->store_data_in_sb = fs_pm_invalid_store_data_in_sb;
}


static void
fs_pm_init_ops (struct fs_pm_ops *ops)
{
  ops->page_write = fs_pm_page_write;
  ops->page_read = fs_pm_page_read;
  ops->store_info = fs_pm_store_info;
  ops->get_info = fs_pm_get_info;
  ops->start_transaction = fs_pm_start_transaction;
  ops->end_transaction = fs_pm_end_transaction;
  ops->space_limit = fs_pm_space_limit;
  ops->register_free_check_cb = fs_pm_register_free_check_cb;
  ops->register_alloc_count_cb = fs_pm_register_alloc_count_cb;
  ops->shred = fs_pm_shred;
  ops->scrub = fs_pm_scrub;
  ops->read_iovec = NULL;
  ops->write_iovec = NULL;
  ops->sync_start = fs_pm_sync_start;
  ops->sync_wait  = fs_pm_sync_wait;
  ops->sync_end   = fs_pm_sync_end;
  ops->sync_no_wait    = fs_pm_sync_no_wait;
  ops->sync_get_status = fs_pm_sync_get_status;
  ops->make_golden_copy = fs_pm_make_golden_copy;
  ops->fs_init_complete = fs_pm_fs_init_complete;
  ops->lock_efs = fs_pm_lock_efs;
  ops->borrow_ram = fs_pm_borrow_ram;
  ops->store_data_in_sb = fs_pm_invalid_store_data_in_sb;
}

/*******   Erased block circular array management routines   ********/

/* Put or enqueue an erased block into the array. The parameter 'blk_page'
 * is the starting page number of the erased block.
 * It returns if the queue can hold more items after this.
 * TRUE  : Queue has some more space, so can takeup some more blocks.
 * FALSE : Queue is filled, so cannot accept anymore blocks.
 */
static int
fs_pm_put_erased_blk (fs_page_id blk_page)
{
  /* Check if the queue has any space left */
  if (erased_blk_list.item_count >= MAX_ERASED_BLOCK_ARRAY_SIZE)
  {
    /* Cannot accept anymore blocks into Queue */
    return FALSE;
  }

  /* Enqueue the page number for erased block. */
  erased_blk_list.erased_blk[erased_blk_list.enqueue_pos] = blk_page;
  erased_blk_list.enqueue_pos++;
  erased_blk_list.item_count++;

  /* Adjust the enqueue pointer to wrap back if needed */
  if (erased_blk_list.enqueue_pos >= MAX_ERASED_BLOCK_ARRAY_SIZE)
    erased_blk_list.enqueue_pos = 0;

  /* Check if the queue is full. */
  if (erased_blk_list.item_count >= MAX_ERASED_BLOCK_ARRAY_SIZE)
  {
    /* Cannot accept anymore blocks into Queue */
    return FALSE;
  }

  /* Some more items can be accomodated */
  return TRUE;
}

/* Get or dequeue an erased block for use, from the erased block
 * circular array. If the block that is being dequeued is the last
 * block in the queue, it would refill it again appropriately. */
static fs_page_id
fs_pm_get_erased_blk (fs_pm_flash_t gc)
{
  fs_page_id free_page;

  /* We should never have a situation of running out of erased blocks
   * if we run GC appropriately to garbage collect and generate enough
   * free space we should never run out of erased blocks in any condition
   * of Garbage distribution. */
  if (erased_blk_list.item_count == 0)
    FS_ERR_FATAL ("No items found in the array \n", 0, 0, 0);

  /* Dequeue the erased block from the array */
  free_page = erased_blk_list.erased_blk[erased_blk_list.dequeue_pos];
  erased_blk_list.dequeue_pos++;
  erased_blk_list.item_count--;

  /* Adjust the dequeue pointer appropriately if wrap around is happening. */
  if (erased_blk_list.dequeue_pos >= MAX_ERASED_BLOCK_ARRAY_SIZE)
    erased_blk_list.dequeue_pos = 0;

  /* If we are just giving away the last available erased block in the queue,
   * then we should start searching from this block onwards till gc_next to
   * refill our array and keep it ready to serve the next request.
   */
  if (erased_blk_list.item_count == 0)
  {
    fs_page_id search_start_page = free_page;
    BUMP_BLOCK (gc, search_start_page);
    /* Fill the queue */
    fs_pm_search_and_queue_erased_blocks (gc, search_start_page);
  }

  return free_page;
}

/* Get the count of erased blocks present in the array. This can
 * be used while counting free pages or sometimes it can be checked
 * to see if there are some erased blocks enqueued before dequeueing.
 * The situation of array not having any erased blocks in the array
 * would arise on startup initialization or fresh startup. In all other
 * cases of normal operation, there should be a block present, otherwise
 * it would indicate a fatal condition of some failure. */
static uint32
fs_pm_get_erased_blk_count (void)
{
  return erased_blk_list.item_count;
}

#endif /* defined (FEATURE_EFS_EFS2_ON_NAND) || (FEATURE_EFS_EFS2_ON_NOR) ||
                  (FEATURE_EFS_EFS2_ON_SPI_NOR) */

#endif /* FEATURE_EFS_QEFS2 */

