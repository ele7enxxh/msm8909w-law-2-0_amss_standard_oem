/***********************************************************************
 * fs_mnand_ptbl.c
 *
 * Page Table (PTbl) module for mNand.
 * Copyright (C) 2008,2010,2012 QUALCOMM Technologies, Inc.
 *
 * Page Table (PTbl) module for mNand.
 * The status of the clusters are tracked using the PTbl pages.
 * Each cluster takes up 4 bytes in the PTbl page.
 * When a cluster's status changes, this information lives in the journal
 * and logs for a while and eventually gets into the PTbl page.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_mnand_ptbl.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2010-10-20   wek   Make journal independent of page size.
2008-12-12   rp    Create

===========================================================================*/
#include "fs_device.h"

#if defined (FEATURE_EFS_EFS2_ON_MNAND) &&    \
    defined (FEATURE_EFS_QEFS2)

#include "fs_mnand_ptbl.h"
#include "fs_mnand_config.h"
#include "fs_mnand_super.h"
#include "fs_err.h"
#include <string.h>
#include "fs_mnand_pm.h"
#include "fs_journal.h"
#include "fs_mnand_pcache.h"


static void fs_mnand_ptbl_init_ops (fs_mnand_ptbl_ops_t ptbl_ops);


void
fs_mnand_ptbl_init (fs_mnand_ptbl_t ptbl, efs_device_t dev,
                   fs_mnand_super_t super, fs_mnand_log_t log, int fresh_start)
{
  ptbl->dev = dev;
  ptbl->super = super;
  ptbl->log = log;
  fs_mnand_ptbl_init_ops (&ptbl->ops);
  /* For MNAND we need to study what is the max journal size. */
  fs_journal_init (&ptbl->jrnl, FS_MAX_JOURNAL_SIZE);
  fs_mnand_pcache_init (&ptbl->pcache, dev, super, fresh_start);
}


void
fs_mnand_ptbl_get_ptbl_sect_offset (uint32 page, uint32 *ptbl_sect,
                                    uint32 *offset)
{
  /* compute which ptbl sector */
  *ptbl_sect = page / FS_MNAND_PTBL_ENTRIES_PER_SECT;

  /* compute offset value */
  *offset = page & (FS_MNAND_PTBL_ENTRIES_PER_SECT -1);
}

static void
mnand_ptbl_set_rev (fs_mnand_ptbl_t ptbl, fs_sector_id sect,
                       fs_cluster_id clust)
{
  fs_sector_id flush_pg;
  unsigned int age;

  flush_pg = fs_mnand_log_peek_flush (ptbl->log);
  age = flush_pg & 0xFF;
  fs_journal_add (&ptbl->jrnl, sect, FS_MNAND_JOURNAL_KEY_TYPE_RTBL,
                  clust, age);
}

static fs_cluster_id
mnand_ptbl_get_rev (fs_mnand_ptbl_t ptbl, fs_sector_id sect)
{
  fs_sector_id ptbl_sect;
  uint32 offset;
  uint32 *ptbl_buff;
  fs_cluster_id clust_status;
  unsigned int age;

  if (fs_journal_lookup (&ptbl->jrnl, sect, FS_MNAND_JOURNAL_KEY_TYPE_RTBL,
                         &clust_status, &age))
  {
    return clust_status;
  }

  /* compute which ptbl sector */
  fs_mnand_ptbl_get_ptbl_sect_offset (sect, &ptbl_sect, &offset);

  ptbl_buff = fs_mnand_pcache_lookup (&ptbl->pcache, ptbl_sect,
                                      FS_MNAND_JOURNAL_KEY_TYPE_RTBL, 0);
  clust_status = ptbl_buff[offset];
  return clust_status;
}


static void
mnand_ptbl_set_fwd (fs_mnand_ptbl_t ptbl, fs_cluster_id clust,
                    fs_sector_id sect)
{
  fs_sector_id flush_pg;
  unsigned int age;

  flush_pg = fs_mnand_log_peek_flush (ptbl->log);
  age = flush_pg & 0xFF;
  fs_journal_add (&ptbl->jrnl, clust, FS_MNAND_JOURNAL_KEY_TYPE_FTBL,
                  sect, age);
}

static fs_sector_id
mnand_ptbl_get_fwd (fs_mnand_ptbl_t ptbl, fs_cluster_id clust)
{
  fs_sector_id ptbl_sect;
  uint32 offset;
  uint32 *ptbl_buff;
  fs_sector_id sect;
  unsigned int age;

  if (fs_journal_lookup (&ptbl->jrnl, clust, FS_MNAND_JOURNAL_KEY_TYPE_FTBL,
                         &sect, &age))
  {
    return sect;
  }

  /* compute which ptbl sector */
  fs_mnand_ptbl_get_ptbl_sect_offset (clust, &ptbl_sect, &offset);

  ptbl_buff = fs_mnand_pcache_lookup (&ptbl->pcache, ptbl_sect,
                                      FS_MNAND_JOURNAL_KEY_TYPE_FTBL, 0);
  sect = ptbl_buff[offset];
  return sect;
}

static void
fs_mnand_ptbl_init_ops (fs_mnand_ptbl_ops_t ptbl_ops)
{
  ptbl_ops->set_fwd = mnand_ptbl_set_fwd;
  ptbl_ops->get_fwd = mnand_ptbl_get_fwd;
  ptbl_ops->set_rev = mnand_ptbl_set_rev;
  ptbl_ops->get_rev = mnand_ptbl_get_rev;
}


#ifdef FS_UNIT_TEST

void
mnand_ptbl_set_rev_for_sim (fs_mnand_ptbl_t ptbl, fs_sector_id sect,
                            fs_cluster_id clust)
{
  mnand_ptbl_set_rev (ptbl, sect, clust);
}

#endif /* FS_UNIT_TEST */



#endif /* FEATURE_EFS_EFS2_ON_MNAND */
