/***********************************************************************
 * fs_mnand_pm.c
 *
 * Page Manager for mNand.
 * Copyright (C) 2008,2010-2013 QUALCOMM Technologies, Inc.
 *
 * The Page Manager exposes the mNand device as a set of clusters to the
 * buffer layer. The buffer layer then can use the PM ops to read/write
 * the clusters at any time.
 *
 * The PM maintains the transaction state, so that the buffer-layers
 * writes either all of them make it to the mNand device or none at
 * all, thus power-safe.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_mnand_pm.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-10-09   nr    Replace banned memory functions with safer versions.
2013-07-22   rp    Add new EFS API to store data in SuperBlock.
2013-06-19   dks   Added NAND Flash scrub support.
2013-01-11   dks   Move writing to Golden partition outside of efs_sync.
2012-12-18   dks   Update golden copy strategy for new rmts layout.
2012-05-16   nr    Make MNAND cluster/page size configurable at runtime.
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2011-10-11   nr    Added borrow_ram support to borrow ram from lower layers.
2011-03-08   rp    Add support to lock  EFS.
2010-11-18   rp    Support TAR image Golden-Copy for RMTS.
2010-11-08   wek   Remove impractical and unused paranoia module.
2010-10-01   wek   Rename mnand macros from clusters to page.
2010-09-30   wek   Remove device iovec interface.
2010-09-01   wek   Remove compile time calculations based on page size.
2010-07-30   wek   Dynamically allocate all buffers dependent on page size.
2010-05-21   rp    Fix compiler warnings seen on target.
2010-05-13   wek   Make sync_no_wait non blocking.
2010-05-06   wek   Implement golden copy for remote storage page manager.
2010-01-19   vr    Added efs_sync_no_wait and efs_sync_get_status APIs.
2008-12-12   rp    Create

===========================================================================*/
#include "fs_device.h"

#if defined (FEATURE_EFS_EFS2_ON_MNAND) &&    \
    defined (FEATURE_EFS_QEFS2)

#include "fs_mnand_pm.h"
#include "fs_mnand_config.h"
#include "fs_mnand_super.h"
#include "fs_mnand_ptbl.h"
#include "fs_mnand_log.h"
#include "assert.h"
#include "fs_buffer.h"
#include "fs_err.h"
#include "fs_util.h"
#include "fs_mnand_pcache.h"
#include "fs_protected_memory.h"
#include "fs_babylon.h"
#include "fs_errno.h"
#include "fs_malloc.h"


/** Advance page by one, upon reaching last page, wrap-around to first. */
#define FS_MNAND_PM_ADVANCE_PAGE(pm, pg) \
  do \
  { \
    ++(pg); \
    if ((pg) == (pm)->max_page) \
      (pg) = 0; \
  } while (0); \

/** Advance a page to the first page in the next block. */
#define FS_MNAND_PM_ADVANCE_BLOCK(pm, pg) \
  do  \
  { \
    (pg) |= (FS_MNAND_PAGES_PER_BLOCK -1); \
    ++(pg); \
    if ((pg) >= (pm)->max_page) \
      (pg) = 0; \
  } while (0);

/** Get the block number of a page. */
#define FS_MNAND_PM_BLOCK_NBR(pg)  ((pg) / FS_MNAND_PAGES_PER_BLOCK)

/** Instance of the PM. */
static struct fs_mnand_pm mnand_pm_inst;

/** The Buffer's fs_iovec are sorted as liquid/solid and copied into these. */
static struct fs_iovec liquid_trans[FS_MAX_IOVEC_ITEMS_COUNT];
static struct fs_iovec solid_trans[FS_MAX_IOVEC_ITEMS_COUNT];

/* prototypes: */
static void fs_mnand_pm_init_ops (fs_pm_t pm_data);
static fs_sector_id fs_mnand_pm_solid_alloc (fs_mnand_pm_t pm);
static fs_sector_id fs_mnand_pm_liquid_alloc (fs_mnand_pm_t pm);
static void fs_mnand_pm_flush_super (fs_mnand_pm_t pm);
static void fs_mnand_pm_free_log_page (fs_mnand_pm_t pm, fs_sector_id log_pg);
static fs_sector_id fs_mnand_pm_get_phy_sector (fs_mnand_pm_t pm,
                                                fs_sector_id page);


/**********************************************************************
 * This cheater function allows us to share the mnand combined buffer
 * with mnand fill region function module on a fresh_start.
 * On a fresh_start, we initialize the mnand regions to 0xFF, during
 * this time if we use this huge buffers, the initialization writes
 * should be fast.
 *
 * The mnand module needs this buffer only to do the one-time initialization
 * of its regions on a fresh_startup.
 *
 * Note that NO CODE other than mnand fresh startup is permitted to borrow
 * this buffer.  This is a special and unique arrangement, done only
 * to conserve RAM footprint.
 *
 **********************************************************************/
uint8*
fs_borrow_data_buffers (uint32 *bufsize_bytes)
{
  *bufsize_bytes =  FS_MAX_IOVEC_ITEMS_COUNT * mnand_pm_inst.page_size;
  /* Make sure it was allocated */
  ASSERT (mnand_pm_inst.combined_buffer != NULL);
  return (mnand_pm_inst.combined_buffer);
}

fs_pm_t
fs_mnand_pm_init (efs_device_t dev)
{
  fs_mnand_pm_t pm;
  fs_sector_id max_sect;
  uint32 tmp;
  uint32 replayed_log_pg_cnt;
  fs_sector_id free_log_pg;

  EFS_BAB_START (EFS_BAB_MNAND_PM_INIT);

  pm = &mnand_pm_inst;
  ASSERT (pm != NULL);
  memset (&mnand_pm_inst, 0, sizeof (mnand_pm_inst));

  pm->dev = dev;
  pm->page_size = fs_device_page_size (pm->dev);
  pm->sectors_per_page = pm->page_size / FS_MNAND_SECTOR_SIZE;
  /** The SDCC device performs better when we pass the sectors that are in
   * sequence in continous memory. So this array is used to concatenate
   * all the sectors that are in sequence. */
  pm->combined_buffer = fs_align_malloc (FS_MAX_IOVEC_ITEMS_COUNT *
                                         pm->page_size);
  pm->combined_buffer_size = FS_MAX_IOVEC_ITEMS_COUNT * pm->page_size;
  ASSERT (FS_OKAY_FOR_DM (pm->combined_buffer));

  fs_mnand_pm_init_ops (&pm->parent);

  /* Initialize the Super module. */
  pm->fresh_start = 0;
  fs_mnand_super_init (&pm->super, pm->dev, &pm->fresh_start);

  /* Compute min and max page. */
  pm->min_page = 0;
  pm->max_page = (pm->super.data->data.end - pm->super.data->data.start)
                    / pm->sectors_per_page;
  max_sect = pm->max_page * pm->sectors_per_page;
  ASSERT (max_sect == (pm->super.data->data.end - pm->super.data->data.start));
  ASSERT ((max_sect + pm->super.data->data.start) == pm->super.data->data.end);

  tmp = (pm->max_page * FS_MNAND_OVERHEAD_PERCENT) / 100;
  pm->avail_pages = pm->max_page - tmp;

  fs_mnand_ptbl_init (&pm->ptbl, pm->dev, &pm->super,
                      &pm->log, pm->fresh_start);

  fs_mnand_log_init (&pm->log, dev, &pm->super, pm->fresh_start);

  if (pm->fresh_start)
  {
    pm->liquid_alloc = 0;
    pm->solid_alloc = FS_MNAND_PAGES_PER_BLOCK;
    fs_mnand_pm_flush_super (pm);
  }
  else
  {
    pm->solid_alloc = pm->super.data->solid_alloc;
    pm->liquid_alloc = pm->super.data->liquid_alloc;
    replayed_log_pg_cnt = fs_mnand_log_replay_logs (&pm->log, pm);
    if (replayed_log_pg_cnt >= FS_MNAND_LOG_WRITTEN_LOGS)
    {
      free_log_pg = fs_mnand_log_peek_free (&pm->log);
      if (pm->super.data->log_head != free_log_pg)
      {
        /* The log_head in the super-block is really old, and we have
         * accumulated enough logs, now would be the ideal time to bring
         * the log_head in the super-block to be in sync with our log-queue.*/
        pm->super.data->log_head = free_log_pg;
        fs_mnand_pm_flush_super (pm);
      }
    }
  }

  EFS_BAB_END (EFS_BAB_MNAND_PM_INIT);

  return &pm->parent;
}


/** Allocate a new-page and write this cluster.  */
static void
fs_mnand_pm_page_write (fs_pm_t pm_data, fs_cluster_id cluster,
                        void *buffer, uint16 tag)
{
  fs_mnand_pm_t pm = (fs_mnand_pm_t )pm_data;
  uint32 old_page, new_page;
  fs_sector_id phy_sect;

  EFS_BAB_START (EFS_BAB_MNAND_PM_WRITE);

  /* The supplied cluster MUST be in the avail_pages range. */
  ASSERT (pm != NULL);
  ASSERT (cluster <= pm->avail_pages);

  /* If user-data, then use solid-alloc. */
  if (tag == FS_IOVEC_DATA_TYPE_PERMENANT_DATA)
  {
    new_page = fs_mnand_pm_solid_alloc (pm);
  }
  else
  {
    new_page = fs_mnand_pm_liquid_alloc (pm);
  }

  /* compute the physical sector for this new_page. */
  ASSERT (new_page <= pm->max_page);
  phy_sect = fs_mnand_pm_get_phy_sector (pm, new_page);
  fs_device_write_page (pm->dev, phy_sect, buffer, FS_FOP_USER);

  /* If applicable, mark the old page as invalid. */
  old_page = pm->ptbl.ops.get_fwd (&pm->ptbl, cluster);
  if (old_page != FS_INVALID_CLUSTER_ID)
  {
    ASSERT (old_page <= pm->max_page);
    pm->ptbl.ops.set_rev (&pm->ptbl, old_page, FS_INVALID_CLUSTER_ID);
  }

  /* Associate the cluster with this new_page. */
  pm->ptbl.ops.set_fwd (&pm->ptbl, cluster, new_page);
  pm->ptbl.ops.set_rev (&pm->ptbl, new_page, cluster);

  /* Record this page assignment in the log. */
  fs_mnand_log_add (&pm->log, FS_MNAND_LOG_CODE_CLUSTER_MOVE,
                         cluster, old_page, new_page, 0);

  EFS_BAB_END (EFS_BAB_MNAND_PM_WRITE);
}

/* Read the page for the supplied cluster. */
static void
fs_mnand_pm_page_read (fs_pm_t pm_data, fs_cluster_id cluster, void *buffer)
{
  fs_mnand_pm_t pm = (fs_mnand_pm_t )pm_data;
  uint32 page;
  fs_sector_id phy_sect;

  EFS_BAB_START (EFS_BAB_MNAND_PM_READ);

  ASSERT (pm != NULL);

  page = pm->ptbl.ops.get_fwd (&pm->ptbl, cluster);
  if (page == FS_INVALID_CLUSTER_ID)
  {
    memset (buffer, 0xFF, FS_MNAND_SECTOR_SIZE);
  }
  else
  {
    phy_sect = fs_mnand_pm_get_phy_sector (pm, page);
    fs_device_read_page (pm->dev, phy_sect, buffer,
                         FS_FOP_USER, FS_FLASH_MUST_WORK);
  }

  EFS_BAB_END (EFS_BAB_MNAND_PM_READ);
}

/** Store the upper-data's info in the super-page and also record in log. */
static void
fs_mnand_pm_store_info (fs_pm_t pm_data, unsigned offset, uint32 data)
{
  fs_mnand_pm_t pm = (fs_mnand_pm_t )pm_data;

  ASSERT (pm != NULL);
  pm->super.data->upper_data[offset] = data;
  fs_mnand_log_add (&pm->log, FS_MNAND_LOG_CODE_UPPER_DATA,
                       offset, data, 0, 0);
}

/** Get the upper-data's info from the super-page. */
static uint32
fs_mnand_pm_get_info (fs_pm_t pm_data, unsigned offset)
{
  fs_mnand_pm_t pm = (fs_mnand_pm_t )pm_data;

  ASSERT (pm != NULL);
  return pm->super.data->upper_data[offset];
}

static void
fs_mnand_pm_start_transaction (fs_pm_t pm_data)
{
  fs_mnand_pm_t pm = (fs_mnand_pm_t )pm_data;
  int jrnl_free_cnt, journal_size;
  uint32 log_written_cnt;
  fs_sector_id log_pg;

  EFS_BAB_START (EFS_BAB_MNAND_PM_START_TRANS);
  EFS_BAB_START (EFS_BAB_MNAND_PM_ONE_TRANSACTION);

  ASSERT (pm != NULL);

  /* No nested transactions please */
  ASSERT (pm->inside_xact == 0);

  /* At the end of this transaction, we want to write the super-page
   * only if the log_head moved further. This is to avoid too frequent
   * updates to the super-page. */
  pm->write_super_pg = 0;

  /* make sure that there are enough free log-pages as configured to carry
     out this entire transaction. */
  fs_mnand_allocate_logs (&pm->log);

  jrnl_free_cnt = fs_journal_free_count (&pm->ptbl.jrnl);
  if (jrnl_free_cnt < 50)
    FS_ERR_FATAL ("Journal needs to be dealt with", 0, 0, 0);

  log_written_cnt = fs_mnand_log_written_cnt (&pm->log);

  journal_size = fs_journal_get_size (&pm->ptbl.jrnl);
  /* If needed, free up space in Log and Journal. */
  while ( (jrnl_free_cnt < (journal_size / 2)) ||
          (log_written_cnt > FS_MNAND_LOG_WRITTEN_LOGS))
  {
    /* Free up the oldest written log-page. */
    log_pg = fs_mnand_log_get_written (&pm->log);
    fs_mnand_pm_free_log_page (pm, log_pg);
    fs_mnand_log_add (&pm->log, FS_MNAND_LOG_CODE_LOG_FLUSH, log_pg, 0, 0, 0);

    /* Also update the super-page, when this transaction ends. */
    pm->write_super_pg = 1;

    /* check if we need to free some more space in Log and Journal. */
    jrnl_free_cnt = fs_journal_free_count (&pm->ptbl.jrnl);
    log_written_cnt = fs_mnand_log_written_cnt (&pm->log);
  }

  /* Now we are good to proceed with this transaction. */
  fs_mnand_log_start_transaction (&pm->log);
  fs_mnand_log_add (&pm->log, FS_MNAND_LOG_CODE_XACT_START, 0, 0, 0, 0);
  pm->inside_xact = 1;

  EFS_BAB_END (EFS_BAB_MNAND_PM_START_TRANS);
}

static void
fs_mnand_pm_end_transaction (fs_pm_t pm_data)
{
  fs_mnand_pm_t pm = (fs_mnand_pm_t )pm_data;

  EFS_BAB_START (EFS_BAB_MNAND_PM_END_TRANS);

  ASSERT (pm != NULL);

  /* make sure we are inside an transaction. */
  ASSERT (pm->inside_xact == 1);

  /* Also tell log that the current transaction has been ended. */
  fs_mnand_log_add (&pm->log, FS_MNAND_LOG_CODE_XACT_END, 0, 0, 0, 0);
  fs_mnand_log_end_transaction (&pm->log);

  /* Update the super-page. */
  if (pm->write_super_pg)
  {
    pm->super.data->log_head = fs_mnand_log_peek_free (&pm->log);
    fs_mnand_pm_flush_super (pm);
  }

  pm->inside_xact = 0;

  EFS_BAB_END (EFS_BAB_MNAND_PM_END_TRANS);
  EFS_BAB_END (EFS_BAB_MNAND_PM_ONE_TRANSACTION);
}

/** return how many clusters are available to the Buffer layer for writing. */
static uint32
fs_mnand_pm_space_limit (fs_pm_t pm_data)
{
  fs_mnand_pm_t pm = (fs_mnand_pm_t )pm_data;

  ASSERT (pm != NULL);
  return pm->avail_pages;
}

/** These Buffer layer callbacks are not used. */
static void
fs_mnand_pm_register_free_check_cb (fs_pm_t pm_data, int (*is_used_cb)
          (void *, fs_cluster_id cluster), void *data)
{
  fs_mnand_pm_t pm = (fs_mnand_pm_t )pm_data;
  ASSERT (pm != NULL);

  (void)is_used_cb;
  (void)data;
}

/** These Buffer layer callbacks are not used. */
static void
fs_mnand_pm_register_alloc_count_cb (fs_pm_t pm_data,
          uint32 (*alloc_count_cb) (void *), void *data)
{
  fs_mnand_pm_t pm = (fs_mnand_pm_t )pm_data;
  ASSERT (pm != NULL);

  (void)alloc_count_cb;
  (void)data;
}

/** Try and combine as many sectors that are in sequence and then write
 *  them in one shot. */
static void
fs_mnand_pm_write_combine_iovec (fs_mnand_pm_t pm, struct fs_iovec iovec[],
                                 uint32 old_iovec_cnt)
{
  uint32 i, j;
  uint32 clust_size = pm->sectors_per_page;
  uint32 next_sect_seq, combined_sect_cnt, buff_idx = 0;

  for (i=0; i<old_iovec_cnt; i=j)
  {
    EFS_BAB_START (EFS_BAB_MNAND_PM_CMB_IOVEC);

    /* Start with this sector. */
    EFS_BAB_START (EFS_BAB_MNAND_PM_MEMCPY);
    fs_memscpy (pm->combined_buffer, pm->combined_buffer_size,
                iovec[i].buff_base, pm->page_size);
    EFS_BAB_END (EFS_BAB_MNAND_PM_MEMCPY);
    buff_idx = pm->page_size;
    next_sect_seq = iovec[i].sector_id + clust_size;
    combined_sect_cnt = clust_size;

    /* Try and concatenate as many sectors that are in sequence. */
    for (j=i+1; j<old_iovec_cnt; ++j)
    {
      if (next_sect_seq != iovec[j].sector_id)
        break; /* Next io-vec is not in sequence */

      EFS_BAB_START (EFS_BAB_MNAND_PM_MEMCPY);
      fs_memscpy ((pm->combined_buffer + buff_idx),
                  (pm->combined_buffer_size - buff_idx),
                  iovec[j].buff_base, pm->page_size);
      EFS_BAB_END (EFS_BAB_MNAND_PM_MEMCPY);

      buff_idx += pm->page_size;
      combined_sect_cnt += clust_size;
      next_sect_seq = iovec[j].sector_id + clust_size;
    }

    /* We took this much time to combine io-vec's in one big buffer. */
    EFS_BAB_END (EFS_BAB_MNAND_PM_CMB_IOVEC);

    EFS_BAB_START (EFS_BAB_MNAND_PM_WRITE_SECTORS);
    fs_device_write_sectors (pm->dev, iovec[i].sector_id,
        pm->combined_buffer, combined_sect_cnt, FS_FOP_USER);
    EFS_BAB_END (EFS_BAB_MNAND_PM_WRITE_SECTORS);
  }
}

/** Helper function to do the actual iovec_write. */
static void
fs_mnand_pm_page_mwrite_helper (fs_mnand_pm_t pm, struct fs_iovec iovec[],
                                uint32 iovec_cnt)
{
  uint32 i;
  fs_page_id cluster;
  fs_page_id old_page, new_page;
  fs_sector_id phy_sect;

  for (i=0; i<iovec_cnt; ++i)
  {
    cluster = iovec[i].cluster_id;
    ASSERT (cluster <= pm->avail_pages);

    /* If applicapable, mark the previously assigned page as free. */
    old_page = pm->ptbl.ops.get_fwd (&pm->ptbl, cluster);
    if (old_page != FS_INVALID_CLUSTER_ID)
    {
      ASSERT (old_page <= pm->max_page);
      pm->ptbl.ops.set_rev (&pm->ptbl, old_page, FS_INVALID_CLUSTER_ID);
    }

    /* Associate the new page with this cluster. Note that the new_page
     * is already entered into the journal as soon as it was allocated. */
    new_page = iovec[i].sector_id;
    ASSERT (new_page <= pm->max_page);
    pm->ptbl.ops.set_fwd (&pm->ptbl, cluster, new_page);

    /* Record this page assignment in the log. */
    fs_mnand_log_add (&pm->log, FS_MNAND_LOG_CODE_CLUSTER_MOVE,
                         cluster, old_page, new_page, 0);

    /* compute the physical sector for this new_page. */
    phy_sect = fs_mnand_pm_get_phy_sector (pm, new_page);
    iovec[i].sector_id = phy_sect;
  }

  if (iovec_cnt == 1)
  {
    fs_device_write_sectors (pm->dev, iovec[0].sector_id, iovec[0].buff_base,
                             pm->sectors_per_page, FS_FOP_USER);
  }
  else
  {
    fs_mnand_pm_write_combine_iovec (pm, iovec, iovec_cnt);
  }
}


/** Allocate as many new_pages as needed, sort the writes as either
  * solid/liquid and then call the above helper functions to carry
  * on the write once for solid then again for liquid. */
static void
fs_mnand_pm_write_iovec (fs_pm_t pm_data, fs_iovec_t iovec,
                         uint32 iovec_cnt)
{
  fs_mnand_pm_t pm = (fs_mnand_pm_t )pm_data;
  uint32 solid_alloc_cnt, liquid_alloc_cnt;
  uint32 new_page;
  uint32 i;
  fs_iovec_t curr_iovec = NULL;

  ASSERT (pm != NULL);

  ASSERT (pm->inside_xact);

  if (iovec_cnt == 0)
    return;

  EFS_BAB_START (EFS_BAB_MNAND_PM_WRITE_IOVEC);

  solid_alloc_cnt = liquid_alloc_cnt = 0;

  for (i=0; i<iovec_cnt; ++i)
  {
    curr_iovec = NULL;
    if (iovec[i].data_type == FS_IOVEC_DATA_TYPE_PERMENANT_DATA)
    {
      new_page = fs_mnand_pm_solid_alloc (pm);
      curr_iovec = &solid_trans[solid_alloc_cnt];
      ++solid_alloc_cnt;
    }
    else
    {
      new_page = fs_mnand_pm_liquid_alloc (pm);
      curr_iovec = &liquid_trans[liquid_alloc_cnt];
      ++liquid_alloc_cnt;
    }
    ASSERT (curr_iovec != NULL);

    curr_iovec->buff_base = iovec[i].buff_base;
    curr_iovec->cluster_id = iovec[i].cluster_id;
    curr_iovec->data_type = iovec[i].data_type;
    curr_iovec->sector_id = new_page;

    /* Mark this newly allocated page immediately as taken, to prevent from
     * the other allocator seeing this page as available.  */
    pm->ptbl.ops.set_rev (&pm->ptbl, new_page, iovec[i].cluster_id);
  }

  ASSERT (solid_alloc_cnt < FS_MAX_IOVEC_ITEMS_COUNT);
  ASSERT (liquid_alloc_cnt < FS_MAX_IOVEC_ITEMS_COUNT);
  ASSERT ((solid_alloc_cnt+liquid_alloc_cnt) == iovec_cnt);

  if (solid_alloc_cnt > 0)
  {
    fs_mnand_pm_page_mwrite_helper (pm, &solid_trans[0], solid_alloc_cnt);
  }

  if (liquid_alloc_cnt > 0)
  {
    fs_mnand_pm_page_mwrite_helper (pm, &liquid_trans[0], liquid_alloc_cnt);
  }

  EFS_BAB_END (EFS_BAB_MNAND_PM_WRITE_IOVEC);
}

/** No support yet. */
static void
fs_mnand_pm_read_iovec (fs_pm_t pm_data, fs_iovec_t iovec,
                        uint32 iovec_cnt)
{
  fs_mnand_pm_t pm = (fs_mnand_pm_t )pm_data;
  ASSERT (pm != NULL);

  (void)iovec;
  (void)iovec_cnt;

  ASSERT (0);
}

/** Allocate a new page using liquid_alloc. */
static fs_page_id
fs_mnand_pm_liquid_alloc (fs_mnand_pm_t pm)
{
  uint32 cnt = 0, start_page;
  fs_cluster_id clust;
  fs_page_id new_page = FS_INVALID_PAGE_ID;
  uint32 avoid_blk;
  int skip_first = 1;

  EFS_BAB_START (EFS_BAB_MNAND_PM_LIQUID_ALLOC);

  start_page = pm->liquid_alloc;
  avoid_blk = FS_MNAND_PM_BLOCK_NBR (pm->solid_alloc);

  while (cnt < 1)
  {
    if (pm->liquid_alloc == start_page)
    {
      if (skip_first == 0)
      {
        FS_ERR_FATAL ("Ran out of liquid pages...",0,0,0);
      }
      skip_first = 0;
    }

    /* if liquid-alloc lands in solid-alloc block then jump to next block */
    if (FS_MNAND_PM_BLOCK_NBR (pm->liquid_alloc) == avoid_blk)
    {
      FS_MNAND_PM_ADVANCE_BLOCK (pm, pm->liquid_alloc);
    }

    clust = pm->ptbl.ops.get_rev (&pm->ptbl, pm->liquid_alloc);
    if (clust == FS_INVALID_CLUSTER_ID)
    {
      new_page = pm->liquid_alloc;
      ++cnt;
    }

    FS_MNAND_PM_ADVANCE_PAGE (pm, pm->liquid_alloc);
  }

  ASSERT (new_page != FS_INVALID_PAGE_ID);

  EFS_BAB_END (EFS_BAB_MNAND_PM_LIQUID_ALLOC);

  return new_page;
}


/** Allocate a new page using solid_alloc. */
static fs_page_id
fs_mnand_pm_solid_alloc (fs_mnand_pm_t pm)
{
  uint32 cnt = 0, start_page;
  fs_cluster_id clust;
  fs_page_id new_page = FS_INVALID_PAGE_ID;
  uint32 avoid_blk;
  int skip_first = 1;

  EFS_BAB_START (EFS_BAB_MNAND_PM_SOLID_ALLOC);

  start_page = pm->solid_alloc;
  avoid_blk = FS_MNAND_PM_BLOCK_NBR (pm->liquid_alloc);

  while (cnt < 1)
  {
    if (pm->solid_alloc == start_page)
    {
      if (skip_first == 0)
      {
        FS_ERR_FATAL ("Ran out of Solid pages...",0,0,0);
      }
      skip_first = 0;
    }

    // if solid-alloc lands in liquid-alloc block then jump to next block
    if ( FS_MNAND_PM_BLOCK_NBR (pm->solid_alloc) == avoid_blk)
    {
      FS_MNAND_PM_ADVANCE_BLOCK (pm, pm->solid_alloc);
    }

    clust = pm->ptbl.ops.get_rev (&pm->ptbl, pm->solid_alloc);
    if (clust == FS_INVALID_CLUSTER_ID)
    {
      new_page = pm->solid_alloc;
      ++cnt;
    }

    FS_MNAND_PM_ADVANCE_PAGE (pm, pm->solid_alloc);
  }

  ASSERT (new_page != FS_INVALID_PAGE_ID);

  EFS_BAB_END (EFS_BAB_MNAND_PM_SOLID_ALLOC);

  return new_page;
}


/** Update the super-page, also record the current alloc positions. */
void
fs_mnand_pm_flush_super (fs_mnand_pm_t pm)
{
  pm->super.data->solid_alloc = pm->solid_alloc;
  pm->super.data->liquid_alloc = pm->liquid_alloc;
  fs_mnand_super_write_super (&pm->super);
}

/* Free up all the transactions recorded in journal against this log-page.
 * Upon return from this function, we would no longer need this log-pages's
 * contents. */
static void
fs_mnand_pm_free_log_page (fs_mnand_pm_t pm, fs_sector_id log_pg)
{
  unsigned int age, key_type, key_type2;
  uint32 key, val;
  uint32 min_key, max_key;
  fs_sector_id ptbl_pg;
  uint32 offset;
  uint32 *ptbl_buff;

  EFS_BAB_START (EFS_BAB_MNAND_PM_FREE_ONE_LOG_PG);

  /* Tell journal to deal with only entries marked with this log-page. */
  age = log_pg & 0xFF;
  fs_journal_setup_age_walk (&pm->ptbl.jrnl, age);

  while (fs_journal_age_advance (&pm->ptbl.jrnl, TRUE))
  {
    fs_journal_age_info (&pm->ptbl.jrnl, &key, &key_type, &val, &age);

    ASSERT (key != FS_INVALID_SECTOR_ID);
    ASSERT ( (key_type == FS_MNAND_JOURNAL_KEY_TYPE_RTBL) ||
             (key_type == FS_MNAND_JOURNAL_KEY_TYPE_FTBL) );

    key_type2 = key_type;

    fs_mnand_ptbl_get_ptbl_sect_offset (key, &ptbl_pg, &offset);
    ptbl_buff = fs_mnand_pcache_lookup (&pm->ptbl.pcache, ptbl_pg,
                                       (enum fs_journal_key_type)key_type, 1);
    ptbl_buff[offset] = val;

    min_key = (key & (~(FS_MNAND_PTBL_ENTRIES_PER_SECT-1)));
    max_key = (key | (FS_MNAND_PTBL_ENTRIES_PER_SECT-1));

    /* Now tell journal to get all the updates for this PTbl page. */
    fs_journal_setup_key_range_walk (&pm->ptbl.jrnl, key_type,
                                     min_key, max_key);

    while (fs_journal_key_range_advance (&pm->ptbl.jrnl, TRUE))
    {
      fs_journal_key_range_info (&pm->ptbl.jrnl, &key, &key_type, &val, &age);

      ASSERT (key_type2 == key_type);

      ASSERT ( (key_type == FS_MNAND_JOURNAL_KEY_TYPE_RTBL) ||
               (key_type == FS_MNAND_JOURNAL_KEY_TYPE_FTBL) );

      ASSERT ( (key >= min_key) && (key <= max_key) );

      offset = key & (FS_MNAND_PTBL_ENTRIES_PER_SECT -1);
      ptbl_buff[offset] = val;
    }

    fs_mnand_pcache_write (&pm->ptbl.pcache, ptbl_pg,
                           (enum fs_journal_key_type)key_type);

    fs_mnand_log_add (&pm->log, FS_MNAND_LOG_CODE_KEYRANGE_WALK,
                         key_type, min_key, max_key, 0);
  }

  EFS_BAB_END (EFS_BAB_MNAND_PM_FREE_ONE_LOG_PG);
}


void
fs_mnand_pm_replay_log_entry (void *pm_data, uint8 log_code,
                              uint32 args_cnt, uint32 args[4])
{
  static int inside_xact = 0;
  fs_sector_id log_pg;
  fs_mnand_pm_t pm = (fs_mnand_pm_t)pm_data;

  (void )args_cnt;

  switch (log_code)
  {
    case FS_MNAND_LOG_CODE_XACT_START:
      inside_xact = 1;
      break;

    case FS_MNAND_LOG_CODE_XACT_END:
      ASSERT (inside_xact == 1);
      inside_xact = 0;
      break;

    case FS_MNAND_LOG_CODE_CLUSTER_MOVE:
      ASSERT (args[0] <= pm->avail_pages);
      ASSERT (args[2] <= pm->max_page);
      pm->ptbl.ops.set_fwd (&pm->ptbl, args[0], args[2]);
      if (args[1] != FS_INVALID_SECTOR_ID)
        pm->ptbl.ops.set_rev (&pm->ptbl, args[1], FS_INVALID_CLUSTER_ID);
      pm->ptbl.ops.set_rev (&pm->ptbl, args[2], args[0]);
      break;

    case FS_MNAND_LOG_CODE_UPPER_DATA:
      ASSERT (args[0] <= FS_UPPER_DATA_COUNT);
      pm->super.data->upper_data[args[0]] = args[1];
      break;

    case FS_MNAND_LOG_CODE_LOG_FLUSH:
      log_pg = fs_mnand_log_peek_free (&pm->log);
      if (log_pg == args[0])
       (void )fs_mnand_log_get_written (&pm->log);
      break;

    case FS_MNAND_LOG_CODE_KEYRANGE_WALK:
      fs_journal_setup_key_range_walk (&pm->ptbl.jrnl, args[0],
                                       args[1], args[2]);
      while (fs_journal_key_range_advance (&pm->ptbl.jrnl, TRUE))
        ;
      break;

    default:
      FS_ERR_FATAL ("Invalid MNAND log-code : %d", log_code, 0, 0);
      break;
  }
}

/** For the given page compute the physical address in the Data region. */
static fs_sector_id
fs_mnand_pm_get_phy_sector (fs_mnand_pm_t pm, fs_sector_id page)
{
  fs_sector_id phy_sect;
  phy_sect = page * pm->sectors_per_page;
  phy_sect += pm->super.data->data.start;
  ASSERT (phy_sect <= pm->super.data->data.end);
  return phy_sect;
}

void
fs_mnand_pm_erase_efs (void)
{
  fs_mnand_super_erase_super_page (&mnand_pm_inst.super);
}

static int
fs_mnand_pm_sync_start (fs_pm_t pm_data)
{
  (void) pm_data;
  return 1; /* Nothing to sync. */
}

static int
fs_mnand_pm_sync_wait (fs_pm_t pm_data)
{
  (void) pm_data;
  return 0; /* Nothing to sync. */
}

static int
fs_mnand_pm_sync_end (fs_pm_t pm_data)
{
  (void) pm_data;
  return 0; /* Nothing to sync. */
}

static int
fs_mnand_pm_sync_no_wait (fs_pm_t pm_data, fs_sync_token_t *sync_token)
{
  (void) pm_data;
  *sync_token = 0; /* Give the always success sync token. */
  return 1; /* Nothing to sync. */
}

static int
fs_mnand_pm_sync_get_status (fs_pm_t pm_data, fs_sync_token_t sync_token,
                             enum fs_sync_result *sync_result)
{
  (void) pm_data;
  (void) sync_token;
  *sync_result = FS_SYNC_COMPLETE;
  return 0; /* Nothing to sync. */
}

static int
fs_mnand_pm_make_golden_copy (fs_pm_t pm_data)
{
  (void) pm_data;

  return -EPERM;
}

static int
fs_mnand_pm_fs_init_complete (fs_pm_t pm_data)
{
  (void) pm_data;

  return -EPERM;
}

static int
fs_mnand_pm_lock_efs (fs_pm_t pm_data)
{
  (void) pm_data;

  return -EPERM;
}

static int
fs_mnand_pm_borrow_ram (fs_pm_t pm_data, void **ram_addr, uint32 *ram_size)
{
  (void) pm_data;

  *ram_addr = NULL;
  *ram_size = 0;
  return -ENOMEM;
}

static int
fs_mnand_pm_store_data_in_sb (fs_pm_t pm_data, const void *data,
                              uint32 data_size)
{
  (void) pm_data, (void) data; (void) data_size;

  return -EPERM;
}

static void
fs_mnand_pm_init_ops (fs_pm_t pm_data)
{
  ASSERT (pm_data != NULL);
  pm_data->ops.page_write = fs_mnand_pm_page_write;
  pm_data->ops.page_read = fs_mnand_pm_page_read;
  pm_data->ops.store_info = fs_mnand_pm_store_info;
  pm_data->ops.get_info = fs_mnand_pm_get_info;
  pm_data->ops.start_transaction = fs_mnand_pm_start_transaction;
  pm_data->ops.end_transaction = fs_mnand_pm_end_transaction;
  pm_data->ops.space_limit = fs_mnand_pm_space_limit;
  pm_data->ops.register_free_check_cb = fs_mnand_pm_register_free_check_cb;
  pm_data->ops.register_alloc_count_cb = fs_mnand_pm_register_alloc_count_cb;
  pm_data->ops.shred = NULL;
  pm_data->ops.scrub = NULL;
  pm_data->ops.read_iovec = fs_mnand_pm_read_iovec;
  pm_data->ops.write_iovec = fs_mnand_pm_write_iovec;
  pm_data->ops.sync_start = fs_mnand_pm_sync_start;
  pm_data->ops.sync_wait  = fs_mnand_pm_sync_wait;
  pm_data->ops.sync_end   = fs_mnand_pm_sync_end;
  pm_data->ops.sync_no_wait = fs_mnand_pm_sync_no_wait;
  pm_data->ops.sync_get_status = fs_mnand_pm_sync_get_status;
  pm_data->ops.make_golden_copy = fs_mnand_pm_make_golden_copy;
  pm_data->ops.fs_init_complete = fs_mnand_pm_fs_init_complete;
  pm_data->ops.lock_efs = fs_mnand_pm_lock_efs;
  pm_data->ops.borrow_ram = fs_mnand_pm_borrow_ram;
  pm_data->ops.store_data_in_sb = fs_mnand_pm_store_data_in_sb;
}

#ifdef FS_UNIT_TEST

fs_sector_id
fs_mnand_pm_liquid_alloc_for_sim (fs_mnand_pm_t pm)
{
  return fs_mnand_pm_liquid_alloc (pm);
}

fs_sector_id
fs_mnand_pm_solid_alloc_for_sim (fs_mnand_pm_t pm)
{
  return fs_mnand_pm_solid_alloc (pm);
}

uint32
fs_mnand_pm_block_nbr_for_sim (uint32 page)
{
  return FS_MNAND_PM_BLOCK_NBR (page);
}

#endif /* FS_UNIT_TEST */

#endif /* FEATURE_EFS_EFS2_ON_MNAND */

