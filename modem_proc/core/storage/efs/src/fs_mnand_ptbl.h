/***********************************************************************
 * fs_mnand_ptbl.h
 *
 * Page Table (PTbl) module for mNand.
 * Copyright (C) 2008, 2012 QUALCOMM Technologies, Inc.
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

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_mnand_ptbl.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2008-12-12   rp    Create

===========================================================================*/

#ifndef __FS_MNAND_PTBL_H__
#define __FS_MNAND_PTBL_H__

#include "fs_device.h"

#ifdef FEATURE_EFS_EFS2_ON_MNAND

#include "fs_mnand_config.h"
#include "fs_sys_types.h"
#include "fs_mnand_pcache.h"
#include "fs_journal.h"
#include "fs_mnand_log.h"
#include "fs_mnand_super.h"

typedef struct fs_mnand_ptbl* fs_mnand_ptbl_t;

/** These are PTbl ops exposed to PM. */
struct fs_mnand_ptbl_ops
{
  void (*set_fwd) (fs_mnand_ptbl_t ptbl, fs_cluster_id clust,
                   fs_sector_id sect);
  fs_sector_id (*get_fwd) (fs_mnand_ptbl_t ptbl, fs_cluster_id clust);
  void (*set_rev) (fs_mnand_ptbl_t ptbl, fs_sector_id sect,
                   fs_cluster_id clust);
  fs_cluster_id (*get_rev) (fs_mnand_ptbl_t ptbl, fs_sector_id sect);
};
typedef struct fs_mnand_ptbl_ops* fs_mnand_ptbl_ops_t;


/** This is main structure for the PTbl module. */
struct fs_mnand_ptbl
{
  struct fs_mnand_ptbl_ops     ops;

  efs_device_t                 dev;
  fs_mnand_super_t             super;
  fs_mnand_log_t               log;

  struct fs_journal_data       jrnl;
  struct fs_mnand_pcache       pcache;
};


/*************************************************************************//**
 * Initialize the PTbl module. Also initializes the journal.
 *
 * @param dev
 *    The dev pointer to the mnand device.
 *
 * @param super
 *    The pointer to the super module.
 *
 * @param log
 *    The pointer to the log module.
 *
 * @param fresh_start
 *    Is this a fresh_start?
 *
*****************************************************************************/
void fs_mnand_ptbl_init (fs_mnand_ptbl_t ptbl, efs_device_t dev,
                        fs_mnand_super_t super, fs_mnand_log_t log,
                        int fresh_start);


/*************************************************************************//**
 * For a given page, compute which sector and offset will hold this page's
 * status. Note that returned sector and offset are logical, Depending upon
 * which PTbl, add that PTbls region start address to get the physical address.
 *
 * @param page
 *    Which page's PTbl sector and offset are needed.
 *
 * @param ptbl_sect
 *    The sector that holds the supplied page's status.
 *
 * @param log
 *    The offset that holds the supplied page's status.
 *
*****************************************************************************/
void fs_mnand_ptbl_get_ptbl_sect_offset (uint32 page, uint32 *ptbl_sect,
                                         uint32 *offset);


#ifdef FS_UNIT_TEST
void mnand_ptbl_set_rev_for_sim (fs_mnand_ptbl_t ptbl, fs_sector_id sect,
                                 fs_cluster_id clust);
#endif /* FS_UNIT_TEST */



#endif /* FEATURE_EFS_EFS2_ON_MNAND */

#endif /* not __FS_MNAND_PTBL_H__ */

