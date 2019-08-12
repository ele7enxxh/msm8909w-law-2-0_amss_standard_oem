/*===========================================================================

  fs_logr.c -  Log region handling.

  This module supports logging to a separate region in flash.

 * Copyright (C) 2003-2005, 2007-2008, 2010, 2012 QUALCOMM Technologies, Inc.


                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_logr.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2012-03-13    wek     Remove redundant header file inclusion fs_efs2_cust.
2010-05-10    dks     Added SPI_NOR device type.
2008-11-03    rp      Introduce device abstraction.
2008-05-08    rp      Handle write-failure in log-region.
2007-10-18    rp      Featurized QEFS2.
2005-12-14    sh      Expand Log Region from 8 to 32 blocks.
2005-01-04    dlb     Update copyright line.
2004-12-30    dlb     Remove excess infiltration of factory image code.
2004-10-15    dlb     Update copyright line.
2004-10-07    dlb     Whitespace cleanup.
2003-06-12    dlb     Added factory start up code.
2003-03-07    dlb     Added fs_logr_peek_alloc().
2003-02-28    dlb     Created.
===========================================================================*/

#ifndef __FS_LOGR_H__
#define __FS_LOGR_H__

#include "fs_device.h"

#ifdef FEATURE_EFS_QEFS2
#if defined (FEATURE_EFS_EFS2_ON_NAND) ||   \
    defined (FEATURE_EFS_EFS2_ON_NOR)  ||   \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)

#include "fs_pm_log.h"
#include "fs_pm_super.h"

struct fs_logr_data;
typedef struct fs_logr_data *fs_logr_t;

/* Initialize */
void fs_logr_init (
    fs_logr_t           logr,
    fs_super_t          super,
    int                 fresh_start);

fs_page_id fs_logr_alloc (fs_logr_t logr);

void fs_logr_iterate (fs_logr_t logr, fs_log_t log,
    void (*visit) (void *priv, fs_log_code_t code, uint32 *args),
    void *priv);

fs_page_id fs_logr_peek_alloc (fs_logr_t logr);

/* Will record the given block as bad in logr_badmap. */
void fs_logr_handle_bad_block (fs_logr_t logr, fs_block_id bad_blk);

/* Whenever we flush a super-block, log_head may advance to the next blocks,
 * on such occasions calling this function will go through every block between
 * the [latest super-block] and [log_head_block -1] and mark the blocks as
 * bad-blocks in flash as well by consulting with logr_badmap.
 */
void fs_logr_mark_bad_blocks (fs_logr_t logr, fs_page_id super_page,
                              fs_page_id log_head);

#define FS_LOGR_MAX_LOG_BLOCKS  32

struct fs_logr_data {
  fs_super_t            super;
  fs_block_id              block_base;

  /* Parameters. */

  /* Boundaries of the log region, in pages.  [stop] is 1 past the end of
   * the log region. */
  fs_page_id               start, stop;

  fs_block_id              write_block;
  fs_page_id               write_page;
};

#endif /* defined (FEATURE_EFS_EFS2_ON_NAND) || (FEATURE_EFS_EFS2_ON_NOR) ||
                  (FEATURE_EFS_EFS2_ON_SPI_NOR)  */
#endif /* FEATURE_EFS_QEFS2 */

#endif /* __FS_LOGR_H__ */
