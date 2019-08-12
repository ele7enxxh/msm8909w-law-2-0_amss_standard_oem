/***********************************************************************
 * fs_factory.h
 *
 * Factory image construction/deconstruction.
 * Copyright (C) 2004-2010, 2012 Qualcomm Technologies, Inc.
 *
 * This module is responsible for both the construction and deconstruction
 * of a canonicalized EFS factory image.  The factory image is a compact
 * representation that can be placed in a fresh flash device (occupying
 * consecutive good blocks).  Upon startup, this code is then able to
 * reconstruct a live filesystem from the image.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_factory.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2010-10-19   wek   Remove dependency with page size macro in super module.
2010-09-01   wek   Remove compile time calculations based on page size.
2010-05-10   dks   Added SPI_NOR device type.
2009-05-18   rp    Fix compiler warning for 8200 which has gcc with more -Ws.
2008-11-03   rp    Introduce device abstraction.
2008-06-02   rp    Return super-page id after writing super-page.
2007-10-18   rp    Featurized QEFS2.
2006-03-09   dlb   Add a few function exports for testing.
2005-01-04   dlb   Update copyright line.
2004-12-14   dlb   Create

===========================================================================*/

#ifndef __FS_FACTORY_H__
#define __FS_FACTORY_H__

#include "fs_device.h"

#ifdef FEATURE_EFS_QEFS2
#if defined (FEATURE_EFS_EFS2_ON_NAND) ||   \
    defined (FEATURE_EFS_EFS2_ON_NOR)  ||   \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)

#include "fs_sys_types.h"
#include "fs_buffer.h"

/* Read the next chunk of the factory image data.  Uses the fields in the
 * read_info structure to keep track of state (this structure is sent back
 * and forth with the protocol).  The specific fields are historical,
 * because they were incorporated into a protocol.
 *
 * Returns 0 on success, or a negative errno value to indicate an error.
 * Normal completion is indicated by a return with the
 * info->stream_state==0.  Note that when the end is indicated this way, a
 * bogus page of data is also returned.  This page should not be included
 * in the image.
 *
 * The assumption is made that the filesystem is locked for the entire
 * duration of the image transfer.  Changes to the filesystem between calls
 * to fs_fact_get_chunk will almost certainly result in a corrupted
 * filesystem in the target system.
 */
int fs_fact_get_chunk (struct fs_factimage_read_info *info,
    byte *page_data,
    struct fsb_nand_data *buf);

/* Initialize factory module. */
void fs_factory_init (uint32 page_size);

/* This function is called on startup to check for the presence of a
 * factory image, and if detected, reconstruct the necessary tables, and
 * superblocks.  It should return the offset of the constructed superblock,
 * which will allow the ordinary startup code to proceed. */
fs_page_id fs_fact_startup_check (fs_super_t super, efs_device_t dev);

/* Export these function, so they can be tested. */
void spm_init (fs_super_t super, efs_device_t dev,
    struct fsb_nand_data *buf,
    int start_buffer);
void spm_set_reverse (fs_page_id page, fs_cluster_id cluster);
void spm_set_forward (fs_cluster_id cluster, fs_page_id page);
void spm_full_flush (void);


#else

static inline int
fs_fact_get_chunk (struct fs_factimage_read_info *info, byte *page_data,
                   struct fsb_nand_data *buf)
{
  (void) info; (void) page_data; (void) buf;
  return -1;
}


#endif /* defined (FEATURE_EFS_EFS2_ON_NAND) || (FEATURE_EFS_EFS2_ON_NOR) ||
                  (FEATURE_EFS_EFS2_ON_SPI_NOR) */

#endif /* FEATURE_EFS_QEFS2 */

#endif /* not __FS_FACTORY_H__ */
