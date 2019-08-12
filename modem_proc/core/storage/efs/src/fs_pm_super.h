/**********************************************************************
 * fs_pm_super.h
 *
 * Super-block management for Embedded File System.
 * Copyright (C) 2002-2008, 2010, 2012-2013 QUALCOMM Technologies, Inc.
 *
 * This file describes the interface to the code the manages the EFS
 * superblock.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_pm_super.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2013-05-07    wek     Add EFS2 Read Only support.
2012-03-13    wek     Remove redundant header file inclusion fs_efs2_cust.
2010-10-19    wek     Remove dependency with page size macro in super module.
2010-09-01    wek     Remove compile time calculations based on page size.
2010-05-10    dks     Added SPI-NOR device type.
2010-04-12    wek     Added 8K page support.
2008-11-03    rp      Introduce device abstraction.
2008-06-02    rp      Store latest super-page-id in RAM.
2008-05-08    rp      Handle write-failure in log-region.
2007-10-18    rp      Featurized QEFS2.
2007-08-10    rp      Align buffers passed to flash on a 32-byte boundary.
2007-07-25    umr     Added support for 4k Page size.
2007-07-06    yg      Add new version for Dual allocators code.
2006-08-31    dlb     New minor version for DB GID fix.
2006-08-23    sh      Adjust the NAND LOG region size with page size.
2006-08-17    dlb     New minor version for LOG CRC improvements.
2006-05-12    sh      Swap the NOR/NAND version bits back again
2006-04-18    dlb     Add forward-updates of superblock.
2006-03-03    sh      Force NAND and NOR Superblock versions to be different
2006-03-02    sh      Bump the ver number because Soft Limit is removed
2006-01-27    sh      Renamed some Q&R defines to be more explanatory
2006-01-17    dlb     Support multiple NOR write styles.
2006-01-11    nrs     Fixed Copyright header
2005-12-14    sh      Expand Log Region from 8 to 32 blocks.
2005-11-09    nrs     Quota and Reservation cleanup
2005-10-18    nrs     Fixes for quotas and reservations
2005-07-20    nrs     Support for quotas and reservations
2005-04-20    dlb     Support 2k pages.
2005-01-04    dlb     Update copyright line.
2004-12-30    dlb     Utilize new factory image code.
2004-10-15    dlb     Update copyright header.
2004-10-07    dlb     Whitespace cleanup.
2004-04-26    drh     Update both NAND and NOR super version for release
2003-11-16     gr     Updated NAND super version so that 6300 3xxx builds
                      and 5xxx builds can be distinguished.
2003-06-17    jkl     Updated NOR and NAND SUPER VERSION.
2003-06-12    adm     Add support for factory start.
2003-04-24    bgc     Updated NOR_VERSION from 3 to 5 due to the old ptables
                      that lack the validity check are now invalid.
2002-08-08    drh     Created by dlb.  Added history header.

===========================================================================*/


#ifndef __FS_NOR_SUPER_H__
#define __FS_NOR_SUPER_H__

#include "fs_device.h"

#ifdef FEATURE_EFS_QEFS2
#if defined (FEATURE_EFS_EFS2_ON_NAND)      ||   \
    defined (FEATURE_EFS_EFS2_ON_NOR)       ||   \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)

#include "fs_sys_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The data associated with the superblock.  All fields of this structure
 * are internal. */
struct fs_super_data;
typedef struct fs_super_data *fs_super_t;

/* Initialize the superblock code.  Searches the device for an existing
 * superblock, and loads it.  If it is not found, then it will erase as
 * necessary and write a simple superblock out. */
void
fs_super_init (
    fs_super_t  super,
    efs_device_t dev,
    int *fresh_start);

/* Searches the device for an existing superblock, and loads it. It will
 * return 0 if it finds a superblock, -1 if it does not.
 * The parameter fresh_start indicates the same info, 0 if superblock
 * was found or 1 if superblock was not found. It is kept in order
 * to have similar code for read only and not read only modes. */
int
fs_super_read_only_init (fs_super_t  super,
                         efs_device_t dev,
                         int *fresh_start);

/* Write out an updated superblock to the given segment. */
int
fs_super_update (
    fs_super_t  super,
    fs_page_id  page);

/**********************************************************************
 *   _   _  ___ _____ _____
 *  | \ | |/ _ \_   _| ____|_
 *  |  \| | | | || | |  _| (_)
 *  | |\  | |_| || | | |___ _
 *  |_| \_|\___/ |_| |_____(_)
 *
 * Although this information is in a publically accessible structure (since
 * it is needed), please do not make changes to this data aside from
 * calling the above functions.
 */

/* First is the actual superblock itself.  This is the data written to the
 * device.  The in-memory copy will always be modified as changes are made,
 * even though the changes will not be written as timely.  It is important
 * that this structure be exactly the size of the smallest page size we
 * support (512 bytes). */

/* There are two version fields for alternative types of implementations.
 * These values _MUST_ be distinct, but it is possible to change only one
 * of them. */

/* Since the version codes now contain a bit indicating type, have some
 * macros to check these bits. */

/* This was the first version number that used the lowest bit to indicate
 * NOR/NAND. */
#define FS_SUPER_LOWEST_ENCODED_VERSION  0x0024

/* Macros to construct and deconstruct version numbers. */
#define FS_SUPER_NANDIFY_VERSION(x)   ((x << 1) | 1)
#define FS_SUPER_NORIFY_VERSION(x)    ((x << 1) | 0)
#define FS_SUPER_VERSION_IS_NAND(x)   (((x) & 1) == 1)
#define FS_SUPER_VERSION_IS_NOR(x)    (((x) & 1) == 0)
#define FS_SUPER_VERSIONOF(x)         ((x) >> 1)

/* Given a superblock pointer, these query macros are useful for deciding
 * which particular algorithms to use. */
#define FS_ISNOR(super)  FS_SUPER_VERSION_IS_NOR((super)->data->version)
#define FS_ISNAND(super) FS_SUPER_VERSION_IS_NAND((super)->data->version)

/* Older superblock versions.  These are named after the feature change
 * that caused the bump.  Non-forward-compatible changes should indicate
 * this change with a larger version bump (such as to the next multiple of
 * 0x100). */
#define FS_SUPER_VERSION_QANDR        0x0012
#define FS_SUPER_VERSION_LONGNAME     0x0013
#define FS_SUPER_VERSION_LOGCRC       0x0014
#define FS_SUPER_VERSION_DBGID        0x0015
#define FS_SUPER_VERSION_DUAL_ALLOC   0x0016

#define FS_SUPER_VERSION       FS_SUPER_VERSION_DUAL_ALLOC
#define FS_SUPER_VERSION_NOR   FS_SUPER_NORIFY_VERSION (FS_SUPER_VERSION)
#define FS_SUPER_VERSION_NAND  FS_SUPER_NANDIFY_VERSION (FS_SUPER_VERSION)

/* There are two magic numbers in the superblock itself.  These are to
 * reduce the likelyhood of deciding alternative data is a superblock.
 * In little endian, this is viewed in ASCII as "EFSSuper". */
#define FS_SUPER_MAGIC1    0x53534645
#define FS_SUPER_MAGIC2    0x72657075

/* Version 1 supports the superblock in this many diverse locations. */
#define SUPER_SEGMENTS  4

/* Maximum number of regions supported by this version of the superblock. */
#define FS_MAXIMUM_REGIONS      4

/* The number of superblock contained page table entries. This is chosen
 * to make the superblock fit inside a single page. The structure
 * superblock_data should fit in a single page. Based on the page size,
 * SUPERBLOCK_PAGE_ENTRIES is calculated to fit superblock_data in a whole
 * page. Validate the calculated value of SUPERBLOCK_PAGE_ENTRIES by asserting
 * the size of superblock_data with page size in fs_pm_init().
 */

/* SUPERBLOCK_DATA_SIZE is the size of structure superblock_data eliminating
 * the tables from structure nand_info. Any updates to structure
 * superblock_data or structure nand_info should reflect in
 * SUPER_BL_DATA_ENT_SIZE. Any failure to update the SUPERBLOCK_DATA_SIZE with
 * the change to superblock_data or nand_info would be caught at fs_pm_init ()
 * which would validate the calculations.
 */
#define SUPERBLOCK_DATA_SIZE   240

/* 8 here is derived from the structure member of nand_info, there are two
 * tables stored in superblock, reverse and forward top most page table.
 * It can be seen as a two arrays:
 * uint32 tables[2][SUPERBLOCK_PAGE_ENTRIES]  which is
 * (4 * 2 * SUPERBLOCK_PAGE_ENTRIES bytes)
 */
#define SUPERBLOCK_PAGE_ENTRIES(superdata)  (((superdata->page_size) -  \
                                          SUPERBLOCK_DATA_SIZE)/(8))

/* Definitions of which page table is which. */
#define FS_PTABLE       0
#define FS_RTABLE       1

/**********************************************************************/
/* Constants defining the region boundaries in NAND flash. */

/* Number of blocks used by the log region in NAND.  These are always at
 * the very end of flash.   Too few, and the wear in this region becomes
 * too concentrated and expires these blocks before others.
 * Too many, and it wastes space that could be used for data.
 * 512KB is a reasonable size.   A target may wish to override the value
 * by defining EFS_NAND_LOG_REGION_BLOCKS. */
/* Number of blocks to use for log region in a 512 page size device*/
#define FS_NAND_LOG_REGION_COUNT_PAGE512    32
/* Number of blocks to use for log region for page sizes != 512 */
#define FS_NAND_LOG_REGION_COUNT    8

/* Number of regions in the NAND flash. */
#define FS_NAND_REGION_COUNT            4

struct nor_info {
  fs_device_write_style_t   style;
};

struct nand_info {
  uint16        nodes_per_page;
  uint16        page_depth;             /* Depth of page table, including
                                           nodes within the superblock. */
  uint16        super_nodes;            /* Page nodes within the
                                           superblock. */
  uint16        num_regions;            /* How many regions are in this FS. */
  uint32        regions[FS_MAXIMUM_REGIONS];
                                        /* Offset of the start of each
                                         * region.  Region 0 is assumed to
                                         * start at 0, regions[0] indicates
                                         * the start of region 1, and so
                                         * on. */
  uint32        logr_badmap;            /* Map of bad blocks in the log
                                           region. */
  uint32        pad;
  uint32        tables[1];  /* The size of these tables are determined by the
                             * page size. They take all the space from here to
                             * almost the end of the page, leaving 16 bytes at
                             * the end of the superblock for padding and crc.*/
};

/* The superblock_data structure has the same size as the device page size.
 * For NAND we store the root page tables in the superblock.
 * The last 4 bytes in the superblock are CRC. */
struct superblock_data {
  uint32        page_header;
  uint16        version;
  uint16        age;
  uint32        magic1;
  uint32        magic2;
  uint32        block_size;             /* Number of pages per block. */
  uint32        page_size;              /* Page size in bytes. */
  uint32        block_count;            /* Total number of blocks in
                                           filesystem. */
  uint32        log_head;
  uint32        alloc_next[FS_MAXIMUM_REGIONS];
  uint32        gc_next[FS_MAXIMUM_REGIONS];

  /* data stored in the superblock on behalf of an upper layer of code */
  uint32        upper_data[FS_UPPER_DATA_COUNT];
  union {
    struct nand_info nand;
    struct nor_info nor;
  } u;
  /* Last 4 bytes in superblock are CRC, regardless of device page size. */
};
typedef struct superblock_data *superblock_data_t;

/* This is the data structure holding the superblock information.  This
 * also holds a cache of various other computed information. */

struct fs_super_data {
  /* Pointer to superblock-data aligned on a 32-byte address. */
  struct superblock_data   *data;

  /* Device this superblock resides in. */
  efs_device_t dev;

  /* Number of bits to shift a block into a page number.  This is the log
   * base 2 of the block size.  The block mask has the low bits cleared to
   * mask out and only get the block part of a page.*/
  int block_shift;
  uint32 block_mask;

  /* Number of blocks used by the log region in NAND. */
  uint32 log_block_count;

  /* A count of the total number of pages.  Again, just to save math. */
  uint32 total_pages;

  /* Locations where each superblock will be written.  These are block
   * numbers, and the superblock should be written as the first page of
   * that block.  These values can change if blocks wear out. */
  uint32 segment_offsets[SUPER_SEGMENTS];

  /* Tracking for updates.  During the update, the superblock will remain
   * at the older version.  Once it is finished, the next superblock update
   * will switch to the correct version. */
  uint32 desired_version;

  /* Where did we find the latest super-page? */
  fs_page_id latest_super_page;
};

/* Needed for factory code. */
/* Create an initial superblock.  Assumes that no other superblocks were
 * found (otherwise the new one wouldn't be always be the oldest. */
void fs_super_create_superblock (fs_super_t super);

/* Needed for wipeefs and for factory startup code.  Invalidates any
 * existing superblocks. */
void fs_pm_super_invalidate_superblocks (void);


/* Get the value of the nand page table in the superblock.
 *
 * param super  superblock data.
 * param type   forward or reverse table FS_PTABLE=forward FS_RTALBE=reverse
 * param offset offset into the page table.
 *
 * return value of page table.
 */
uint32 fs_super_get_ptable (struct superblock_data *super,
                             uint32 type, uint32 offset);

/* Set the value of the nand page table in the superblock.
 *
 * param super  superblock data.
 * param type   forward or reverse table FS_PTABLE=forward FS_RTALBE=reverse
 * param offset offset into the page table.
 * param value  new value for the page table.
 */
void fs_super_set_ptable (struct superblock_data *super, uint32 type,
                          uint32 offset, uint32 value);

/* Gets the value of the CRC field in the superblock. Basically the last 4
 * bytes of the given superblock.
 *
 * param super  superblock data.
 *
 * return value of CRC stored in superblock data.
 */
uint32 fs_super_get_crc (struct superblock_data *super);

/* Sets the value of the CRC field in the superblock.
 *
 * param super  superblock data.
 */
void fs_super_set_crc (struct superblock_data *super, uint32 value);

#ifdef __cplusplus
}
#endif

#endif /* defined (FEATURE_EFS_EFS2_ON_NAND) || (FEATURE_EFS_EFS2_ON_NOR) ||
                  (FEATURE_EFS_EFS2_ON_SPI_NOR) */

#endif /* FEATURE_EFS_QEFS2 */

#endif /* __FS_NOR_SUPER_H__ */
