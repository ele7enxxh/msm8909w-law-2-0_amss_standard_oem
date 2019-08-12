/**********************************************************************
 * fs_pm_types.h
 *
 * Copyright (C) 2003-2008, 2010, 2012-2013 QUALCOMM Technologies, Inc.
 * Page manager types.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_pm_types.h#1 $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2013-06-19    dks     Added NAND Flash scrub support.
2012-03-13    wek     Remove redundant header file inclusion fs_efs2_cust.
2010-09-01    wek     Remove compile time calculations based on page size.
2010-04-12    wek     Added 8K page support.
2008-11-03    rp      Introduce device abstraction.
2007-10-18    rp      Featurized QEFS2.
2007-10-08    yg      Removed unused preproc statement.
2007-09-19    yg      Changed NULL Tag name.
2007-08-31    yg      Added some comments.
2007-08-10    rp      Added #define for FS_MAX_CACHED_PTABLE_PAGES.
2007-07-25    umr     Added support for 4k Page size.
2007-07-06    yg      Add Tag related masks.
2006-12-08    dlb     Moved transaction limits into fs_config.h
2005-12-14    dlb     Add log entry for log alloc.
2005-10-21    dlb     Mark unsigned constants as such.
2005-04-26    dlb     Add 2K page support.
2004-10-15    dlb     Update copyright header.
2004-05-05    dlb     Handle page write failures in data region.
2003-06-17    bgc     Added FS_LOG_ENTRY_GARBAGE.
                      Added change log.
===========================================================================*/

#ifndef __FS_PM_TYPES_H__
#define __FS_PM_TYPES_H__

#include "fs_config_i.h"

#ifdef FEATURE_EFS_QEFS2

#include "fs_device_types.h"

/* Entries that are possible in the reverse map.  The first three are block
 * states (indicates the entire rest of the block is the same state).
 * These indicate the state of blocks that have no data in them.  UNKNOWN
 * is the same as INVALID_CLUSTER_ID, since that is what we will get from
 * tables when they are created.
 * FS_RMAP_BLOCK_ERASED can be placed at any page within a block to
 * indicate that the rest of that block is erased. */

/* All the entries are encoded using some of the top bits.  If the high bit
 * is clear, this is a cluster_id or page_id, this gives us 31 bit id's,
 * which is still 1TB with 512 byte clusters.
 *
 * If the high bit is set, bit 30 indicates the speciality of the value.  A
 * 1 indicates a marker other than a page table entry, a 0 indicates a page
 * table entry.  This is chosen so that the erased value (0xFFFFFFFF) can
 * be a special entry.
 *
 * For a page table entry, the next 3 bits indicate the level of the table
 * affected.  The rest of the bits store the affected page table index,
 * shifted down appropriately (so that it fits),  As long as at least 5
 * bits worth of entries fit in a page (32), this scheme works.  This does
 * restrict our minimum page size to 128 bytes, however. */

#define FS_RMAP_BLOCK_UNKNOWN   (FS_INVALID_CLUSTER_ID)
#define FS_RMAP_BLOCK_ERASED    ((fs_cluster_id) 0xFFFFFFF1U)
#define FS_RMAP_BLOCK_BAD       ((fs_cluster_id) 0xFFFFFFF2U)

/* Once a block has data, each page can be a valid cluster_id, or one of
 * these. */
#define FS_RMAP_PAGE_GARBAGE    ((fs_cluster_id) 0xFFFFFFF4U)
#define FS_RMAP_PAGE_SUPER      ((fs_cluster_id) 0xFFFFFFF7U)
#define FS_RMAP_PAGE_LOG        ((fs_cluster_id) 0xFFFFFFF8U)
#define FS_RMAP_PAGE_RESERVED   ((fs_cluster_id) 0xFFFFFFF9U)

/*
 *  Status Assignment :
 *                       33222222 22221111 11111100 00000000
 *               Bits :  10987654 32109876 54321098 76543210
 *                       -----------------------------------
 *            Special :  11SSSSSS SSSSSSSS SSSSSSSS SSSSSSSS
 *            Table   :  10YLLLII IIIIIIII IIIIIIII IIIIIIII
 *            Tag     :  00TTTTTT CCCCCCCC CCCCCCCC CCCCCCCC
 *
 *  Where bits [31:30] differentiate between status types as mentioned above.
 *
 *   S : Special status for page or blocks.
 *   Y : Table type. (0: Fwd, 1: Rev)
 *   L : Table Level (0 - 7)
 *   I : Table Index
 *   T : Data tag.
 *   C : Cluster number (This limits the maximum number of clusters in EFS).
 */
#define FS_RMAP_IS_SPECIAL(x)   ((((uint32) x) >> 31) == 1)

#define FS_RMAP_IS_TABLE(x)     ((((uint32) x) >> 30) == 2)

/* Construct the table entry for a given page table. */
#define FS_RMAP_PAGE_TABLE(table, level, index) \
        (0x80000000U | \
        ((table) << 29) | \
        ((level) << 26) | \
        ((index) >> 6))

/* Get the various fields. */
#define FS_RMAP_GET_TABLE(entry)  (((entry) >> 29) & 1)
#define FS_RMAP_GET_LEVEL(entry)  (((entry) >> 26) & 0x7)
#define FS_RMAP_GET_INDEX(entry)  (((entry) & 0x03FFFFFFU) << 6)

/* Data tag related definitions. Refer to detailed comments above for
 * various type and range of assignments for status.
 * Since we are storing the data tags in status, we need
 * to be able to distinguish between special status, page tables
 * (if we merge the regions later), and the cluster number along
 * with the tag.
 * */
#define TAG_SHIFT          24
#define TAG_MASK         0x3F
#define CLUSTER_MASK     0xFFFFFF

/* This NULL tag can be used when the status being set is a special
 * status which also occupies the tag position in the status bits.
 * If status being set is special status, set_reverse will ignore
 * the tag anyway, so this const can be used to show that its not
 * used, and is being used for just readability.
 * Example to this special status is GARBAGE which is 0xFFFFFFF4 */
#define FS_PM_NULL_TAG         0

/* Possible log file entries. */
#define FS_LOG_ENTRY_ERASE_START        FS_LOG_CODE_MAKE (1, 1)
#define FS_LOG_ENTRY_ERASE_FINISH       FS_LOG_CODE_MAKE (2, 1)
#define FS_LOG_ENTRY_LOG_FLUSH          FS_LOG_CODE_MAKE (3, 1)
#define FS_LOG_ENTRY_PAGE_MOVE          FS_LOG_CODE_MAKE (4, 3)
#define FS_LOG_ENTRY_NEW_DATA           FS_LOG_CODE_MAKE (5, 2)
#define FS_LOG_ENTRY_PTABLE_MOVE        FS_LOG_CODE_MAKE (6, 3)
#define FS_LOG_ENTRY_UPPER_DATA         FS_LOG_CODE_MAKE (7, 2)
#define FS_LOG_ENTRY_XACT_START         FS_LOG_CODE_MAKE (8, 0)
#define FS_LOG_ENTRY_XACT_END           FS_LOG_CODE_MAKE (9, 0)
#define FS_LOG_ENTRY_KEYRANGE           FS_LOG_CODE_MAKE (10, 2)
#define FS_LOG_ENTRY_GC_MOVE            FS_LOG_CODE_MAKE (11, 3)
#define FS_LOG_ENTRY_XACT_SKIP          FS_LOG_CODE_MAKE (12, 0)
#define FS_LOG_ENTRY_GC_DEALLOC         FS_LOG_CODE_MAKE (13, 2)
#define FS_LOG_ENTRY_GARBAGE            FS_LOG_CODE_MAKE (14, 1)
#define FS_LOG_ENTRY_ERASE_FAIL         FS_LOG_CODE_MAKE (15, 1)
#define FS_LOG_ENTRY_WRITE_FAIL         FS_LOG_CODE_MAKE (16, 1)
#define FS_LOG_ENTRY_LOG_ALLOC          FS_LOG_CODE_MAKE (17, 1)

/* Encode an decode table/level into a journal type. */
#define FS_JOURNAL_ENCODE(table, level) (((table) << 6) | (level))
#define FS_JOURNAL_TABLEOF(jt)          ((jt) >> 6)
#define FS_JOURNAL_LEVELOF(jt)          ((jt) & 0x3F)


/* NAND regions GC Scrub states */
typedef enum
{
  FS_SCRUB_IDLE,           /* Indicates GC's SCRUB is idle. */
  FS_SCRUB_RUNNING,        /* Indicates GC's SCRUB is running. */
} fs_pm_scrub_state_type;

#endif /* FEATURE_EFS_QEFS2 */

#endif /* __FS_PM_TYPES_H__ */
