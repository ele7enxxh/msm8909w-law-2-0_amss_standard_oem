/***********************************************************************
 * fs_factory.c
 *
 * Factory image construction/deconstruction.
 * Copyright (C) 2004-2014 QUALCOMM Technologies, Inc.
 *
 * Implementation of the factory gang image.
 * Note that the nature of this task necessitates having some knowledge of
 * internal structures of other parts of the filesystem.  The following
 * (non-exclusive) list gives some of these dependencies:
 *
 *   - Storage of the freemap in the buffer code (specifically, how the
 *     bitmap is represented, and that it starts in cluster #1).
 *   - Format and meaning of the superblocks.
 *   - Layout of the regions in the filesystem.
 *
 * In addition, since the factory image startup is an infrequent event, we
 * avoid, as much as possible, requiring RAM dedicated to this purpose.  As
 * such, the following data structures are used in a somewhat unnatural way:
 *
 *   - The fs_buffer buffer pool.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_factory.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-04-26   vm    Use correct FS_MSG macros to save RAM.
2013-10-09   nr    Replace banned memory functions with safer versions.
2013-03-26   wek   Rename macros from MSG_ to FS_MSG for efs code.
2012-07-05   nr    Fix the spm init to memset and clear the spm variable.
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2011-04-04   wek   Code cleanup.
2010-10-19   wek   Remove dependency with page size macro in super module.
2010-09-01   wek   Remove compile time calculations based on page size.
2010-07-30   wek   Dynamically allocate all buffers dependent on page size.
2010-05-10   dks   Added SPI_NOR device type.
2010-04-12   wek   Added 8K page support.
2009-05-18   rp    Removed obsolete feature FS_FEATURE_RMTEFS_SERVER.
2009-01-27   yg    Bump factory version, make super version backward compatible
2008-11-03   rp    Introduce device abstraction.
2008-06-02   rp    Return super-page id after writing super-page.
2007-10-29   rp    Fix for unaligned Diag buffers while extracting CEFS image.
2007-10-18   rp    Featurized QEFS2.
2007-08-10   rp    Align buffers passed to flash on a 32-byte boundary.
2007-07-30   sh    Revise buffer allocation in factory startup to be safer.
2007-07-25   umr   Added support for 4k Page size.
2007-07-06   yg    Added dual allocator related code.
2007-05-25   umr   Added support for EFS memory protection feature.
2007-02-23   dlb   Move read failures into device wrappers.
2006-12-11   dlb   Fix bad block problem in ptable region.
2006-09-11   dlb   Use flash driver wrappers.
2006-05-08   sh    Fixed RVCT warnings
2006-05-07   sh    Removed extra 'break;' statements
2006-04-17   dlb   Add CRC to superblock.
2006-03-23   dlb   Allow capture from non-512 byte page devices.
2006-03-17   dlb   Page order discipline fixes.
2005-10-30   sh    Lint cleanup.
2005-08-03   sh    Print a Diag MSG when Factory Start code finds an image.
2005-05-26   sd    Compilation fixes for L4.
2005-04-26   dlb   Add 2K page support.
2005-02-24   dlb   Fix restore of large images.
2005-02-15   dlb   Disallow different sized partition.
2005-01-27   dlb   Allow standalone builds.
2005-01-17   dlb   Cleanup from code review.
2005-01-04   dlb   Allow factory image restore in different sized partition.
2004-12-14   dlb   Create

===========================================================================*/

#include "fs_device.h"

#ifdef FEATURE_EFS_QEFS2
#if defined (FEATURE_EFS_EFS2_ON_NAND) ||   \
    defined (FEATURE_EFS_EFS2_ON_NOR)  ||   \
    defined (FEATURE_EFS_EFS2_ON_SPI_NOR)

#include "fs_sys_types.h"
#include "fs_util.h"
#include "fs_err.h"
#include "fs_errno.h"
#include "fs_buffer.h"
#include "msg.h"
#include "assert.h"

#include "crc.h"

#include "fs_factory.h"
#include "fs_malloc.h"
#include "fs_priv_funcs.h"
#include "fs_protected_memory.h"

/* The factory image consists of single page header followed by the active
 * clusters of the filesystem.  This code makes some assumptions about how
 * the freemap is stored by the buffer code in order to determine which
 * clusters are actually active. */
typedef struct factory_header
{
  uint32    magic1;
  uint32    magic2;
  uint16    fact_version;           /* Version of this cluster. */

  /* Fields needed for the superblock. */
  uint16    version;                /* Superblock version. */
  uint32    block_size;             /* Pages per block. */
  uint32    page_size;              /* Page size in bytes. */
  uint32    block_count;            /* Total blocks in device. */
  uint32    space_limit;            /* Total number of used pages (defines
                                       the size of the map). */
  uint32    upper_data[FS_UPPER_DATA_COUNT];
} *factory_header_t;

/* There are two magic numbers in the factory image information cluster
 * These are to help recognize the first non-bad block containing the
 * information cluster and cluster map pages*/
#define EFS_FACTIMAGE_MAGIC1    0x34856787u
#define EFS_FACTIMAGE_MAGIC2    0x92347759u

#define EFS_FACTIMAGE_VERSION   4

/* The factory images are produced in fixed sized chunks, defined by the
 * diag protocol. */
#define EFS_FACTIMAGE_PSIZE     512

/* These values masks and shifts manipulate the various EFS_FACTIMAGE_PSIZE
 * chunks of data contained within a single page. */
/* Number of EFS_FACTIMAGE_PSIZE that fit in a page Size */
static uint32 split_sub_count;
/* Number of shift needed to get split_sub_count */
static uint32 split_shift;

/* Split pages need a separate buffer to read the entire page into, since
 * the driver doesn't support partial page read.
 * Also we are using this buffer as temp-buffer for Page-Reads, as the
 * buffers from Diag are not aligned. */
static uint8 *fs_factory_page_buffer = NULL;

/* Generate the factory header, based on information from the current
 * superblock. */
static int
fs_make_factory_header (struct fs_factimage_read_info *info,
    byte *page_data,
    struct fsb_nand_data *buf,
    struct fs_pm_flash_data *pm)
{
  int i;
  factory_header_t head = (factory_header_t) page_data;

  memset (page_data, 0, EFS_FACTIMAGE_PSIZE);

  head->magic1 = EFS_FACTIMAGE_MAGIC1;
  head->magic2 = EFS_FACTIMAGE_MAGIC2;
  head->fact_version = EFS_FACTIMAGE_VERSION;

  head->version = pm->ptable->super.data->version;
  head->block_size = pm->ptable->super.data->block_size;
  head->page_size = pm->ptable->super.data->page_size;
  head->block_count = pm->ptable->super.data->block_count;
  head->space_limit = buf->space_limit;

  for (i = 0; i < FS_UPPER_DATA_COUNT; i++) {
    head->upper_data[i] = buf->fields[i];
  }

  /* Transition to outputting the freemap. */
  if (split_sub_count > 1)
    info->stream_state = 1;
  else
    info->stream_state = 2;

  info->cluster_data_seqno = 1;

  return 0;
}

static int
fs_send_factory_adjunct (struct fs_factimage_read_info *info,
    byte *page_data)
{
  memset (page_data, 0, EFS_FACTIMAGE_PSIZE);

  info->cluster_data_seqno++;
  if (info->cluster_data_seqno == split_sub_count) {
    info->stream_state = 2;
    info->cluster_data_seqno = 1 << split_shift;
  }

  return 0;
}

/* Output the pages of the data.  We rely on the buffer code telling us
 * that pages in the freemap are allocated. */
static int
fs_output_data (struct fs_factimage_read_info *info,
    byte *page_data,
    struct fsb_nand_data *buf,
    struct fs_pm_flash_data *pm)
{
  fs_cluster_id cluster;
  uint32 split_mask = split_sub_count - 1;

  while (1) {
    cluster = info->cluster_data_seqno >> split_shift;

    if (cluster >= buf->space_limit) {
      info->stream_state = 0;
      return 0;
    }

    if (fs_buf_is_allocated (buf, cluster)) {
      pm->parent.ops.page_read (&pm->parent, cluster, fs_factory_page_buffer);
      /* Check if we need to split pages */
      if (split_sub_count > 1)
      {
        fs_memscpy (page_data, EFS_FACTIMAGE_PSIZE, fs_factory_page_buffer +
          (info->cluster_data_seqno & (split_mask)) * EFS_FACTIMAGE_PSIZE,
          EFS_FACTIMAGE_PSIZE);
      }
      else
      {
        fs_memscpy (page_data, pm->ptable->super.data->page_size,
                    fs_factory_page_buffer, pm->ptable->super.data->page_size);
      }

      info->cluster_data_seqno++;
      return 0;
    }

    /* Otherwise, keep trying. */
    info->cluster_data_seqno++;
  }
}

/* External interface for extracting images.  The stream_state must start
 * at 0, and will be returned to 0 to indicate the transfer is finished.
 *
 * The state machine is indicated as follows:
 *   0 -> Initial (and final state).  Transmits the first piece of the
 *   header.
 *   1 -> Empty pages of the header page.
 *   2 -> Data pages.
 */
int
fs_fact_get_chunk (struct fs_factimage_read_info *info,
    byte *page_data,
    struct fsb_nand_data *buf)
{
  struct fs_pm_flash_data *pm = (struct fs_pm_flash_data *) buf->gc;

  switch (info->stream_state) {
    case 0:
      return fs_make_factory_header (info, page_data, buf, pm);

    case 1: /* We need to split pages. */
      ASSERT (split_sub_count > 1);
      return fs_send_factory_adjunct (info, page_data);

    case 2:
      return fs_output_data (info, page_data, buf, pm);

    default:
      FS_MSG_ERROR_1 ("Invalid stream state: %d", 0);
      return -EINVAL;
  }
}

/***********************************************************************
 * Freemap caching.
 ***********************************************************************/

/* The freemap should always be accessed sequentially, so a single page
 * cache of freemap pages should be sufficient.  We assume that the freemap
 * begins with the page immediately following the factory image header.
 * The code that skips bad blocks is specifically implemented so that the
 * sequential access will be faster. */
struct freemap_cache {
  uint8 *data;

  fs_cluster_id last_offset;
  fs_page_id cur_page;
  fs_cluster_id page_bit_mask;
  efs_device_t dev;
  unsigned block_size;
};

static struct freemap_cache fcache;

/* Initialize the fcache, header_page is the page number where the factory
 * image header is. */
static void
fcache_init (efs_device_t dev, uint8 *buffer, fs_page_id header_page)
{
  fcache.data = buffer;
  fcache.cur_page = header_page + 1;
  fcache.last_offset = 0;
  fcache.dev = dev;

  fs_device_read_page (dev, header_page + 1, fcache.data,
      FS_FOP_FACTORY_RECOVERY, FS_FLASH_MUST_WORK);

  fcache.page_bit_mask = (fs_device_page_size (dev) * 8) - 1;
  fcache.block_size = fs_device_block_size (dev);
}

/* Returns if a given cluster is marked as free.  Can only be called with a
 * cluster number of one greater than the cluster number used on the last
 * call. */
static int
fcache_is_free (fs_cluster_id cluster)
{
  unsigned offset, bit;

  ASSERT (cluster == fcache.last_offset + 1);

  if ((cluster & fcache.page_bit_mask) == 0) {
    fcache.cur_page++;

    while ((fcache.cur_page % fcache.block_size) == 0 &&
        fs_device_bad_block_check (fcache.dev,
          fcache.cur_page / fcache.block_size))
    {
      fcache.cur_page += fcache.block_size;
    }

    fs_device_read_page (fcache.dev, fcache.cur_page, fcache.data,
        FS_FOP_FACTORY_RECOVERY, FS_FLASH_MUST_WORK);
  }

  fcache.last_offset++;

  offset = (cluster & fcache.page_bit_mask) >> 3;
  bit = cluster & 7;

  return (fcache.data[offset] & (1 << bit)) != 0;
}

/***********************************************************************
 * Simple page table manager.
 ***********************************************************************/

/* Management information for a RAM copy of a single page table. */
struct single_page {
  unsigned      table;
        /* Which table is this (0 - forward, 1 - reverse). */

  fs_page_id       location;
        /* Ultimate destination for this page table. */

  fs_page_id       index;
        /* Page index of the first page covered by this table. */

  unsigned      level;
        /* Page table level.  0 indicates superblock (which won't be
         * covered here). */

  unsigned      ref;
        /* Reference count.  Since tables can be shared. */

  uint32        *buf;
        /* Specific page buffer. */
};

/* Table cache information.  One for each level of each of the 3 active
 * tables. */
struct table_cache {
  struct single_page *pg;
        /* Reference to the 'single_page' containing the appropriate
         * buffer. */
};

/* Maximum number of page table levels supported by this code.  Since the
 * level-0 page table is stored directly in the superblock, the page_depth
 * value in the superblock can be 1 greater than this.  3 times this many
 * buffers will be needed. */
#define FS_FACTORY_MAX_LEVELS   (FS_MAX_LEVELS-1)

#define FS_FACTORY_TOTAL_TABLES (FS_FACTORY_MAX_LEVELS*3)

/* Tables used by the simple page table manager.  See spm_table_set for
 * more info. */
#define SPM_TABLE_FORWARD    0
#define SPM_TABLE_REV        1
#define SPM_TABLE_PTABLE_REV 2
#define SPM_NUMBER_TABLES    3

/* Entries for recording the reallocations.  Please see comments on
 * spm_tmp_alloc for more information. */
struct reloc_entry {
  fs_page_id actual;
  fs_page_id desired;
};
typedef struct reloc_entry *reloc_t;

#define RELOCS_PER_PAGE \
   (spm.super->data->page_size / sizeof (struct reloc_entry))

struct simple_pm {
  efs_device_t dev;
  fs_super_t  super;

  fs_page_id     next;
  unsigned    last_level;

  fs_page_id     tmp_next;

  uint32      shifts[FS_MAX_LEVELS];
  uint32      masks[FS_MAX_LEVELS];
  uint32      mask_off[FS_MAX_LEVELS];

  struct single_page tables[FS_FACTORY_TOTAL_TABLES];
  struct table_cache cache[SPM_NUMBER_TABLES][FS_FACTORY_MAX_LEVELS];

  reloc_t     relocs;
  unsigned    reloc_count;

  fs_page_id    *reloc_pages;
  unsigned    reloc_page_count;

  void       *tmp_buf;
};
static struct simple_pm spm;

/* Within a given table, at a given level, determine the index of a
 * particular value within that table. */
#define PTABLE_INDEX(spm, id, level) \
   (((id) & (spm).masks[level]) >> ((spm).shifts[level]))

/* This is a clone of 'init_shifts_and_masks' from the fs_pm_ptable_nand.c
 * code. */
static void
spm_init_shifts_and_masks (uint32 page_size)
{
  int level;
  uint8         shift = 0;
  uint32        mask;
  uint32        mask_off = 0xFFFFFFFFu;
  uint32        pte_shift;
  uint32        pte_per_page;

  pte_per_page = page_size / sizeof (fs_page_id);
  pte_shift = fs_get_shifts (pte_per_page);
  mask = pte_per_page - 1;

  for (level = spm.super->data->u.nand.page_depth - 1;
      level >= 0;
      level--)
  {
    mask_off &= ~mask;

    spm.shifts[level] = shift;
    shift += pte_shift;
    spm.masks[level] = mask;
    mask <<= pte_shift;
    spm.mask_off[level] = mask_off;
  }
}

/* Initialize the simple page manager. */
void
spm_init (fs_super_t super, efs_device_t dev, struct fsb_nand_data *buf,
          int start_buffer)
{
  int i, j;

  memset (&spm, 0, sizeof(spm));
  spm.dev = dev;
  spm.super = super;

  spm.next = super->data->u.nand.regions[0] * super->data->block_size;
  spm.tmp_next = super->data->u.nand.regions[1] * super->data->block_size;

  spm.last_level = super->data->u.nand.page_depth - 1;

  spm.relocs = (reloc_t) buf->buffers[start_buffer];
  memset (spm.relocs, 0xFF, spm.super->data->page_size);
  spm.reloc_count = 0;
  start_buffer++;

  spm.reloc_pages = (fs_page_id *) buf->buffers[start_buffer];
  spm.reloc_page_count = 0;
  start_buffer++;

  for (i = 0; i < FS_FACTORY_TOTAL_TABLES; i++) {
    spm.tables[i].ref = 0;
    spm.tables[i].buf = (uint32 *) buf->buffers[start_buffer + i];
    if (start_buffer + i >= FS_BUFFER_NUMBER_BUFFERS)
      FS_ERR_FATAL ("Config error: not enough buffers for factory image",
          0, 0, 0);
  }

  /* The tmp_buf is used later, just set to the first of the buffers. */
  spm.tmp_buf = buf->buffers[start_buffer];

  for (i = 0; i < SPM_NUMBER_TABLES; i++) {
    for (j = 0; j < FS_FACTORY_MAX_LEVELS; j++) {
      spm.cache[i][j].pg = NULL;
    }
  }

  spm_init_shifts_and_masks (spm.super->data->page_size);
}

/* Allocate a page for use by the simple page manager. */
static fs_page_id
spm_alloc (void)
{
  fs_page_id res;
  int result;

  if ((spm.next & ~spm.super->block_mask) == 0) {
    /* Skip any necessary bad blocks. */
    while (fs_device_bad_block_check (spm.dev,
          spm.next >> spm.super->block_shift))
    {
      /* It isn't necessary to mark these as bad at this time, since the
       * garbage collector will notice them as still in state 'Unknown' and
       * redo the bad block check later. */
      spm.next += spm.super->data->block_size;
    }
    if (spm.next >= (spm.tmp_next & spm.super->block_mask))
      FS_ERR_FATAL ("Not enough space to compute page tables (1)", 0, 0, 0);

    /* Erase this block. */
    result = fs_device_erase_block (spm.dev,
        spm.next / spm.super->data->block_size, FS_FOP_FACTORY_RECOVERY);
    if (result != FS_DEVICE_OK) {
      FS_ERR_FATAL ("Erase failure in ptable region", 0, 0, 0);
    }
  }

  res = spm.next;
  spm.next++;
  /* XXX: Do a bounds check, in case there are too many bad blocks. */

  return res;
}

/* Allocate, and record, a temporary page.  Because the devices require all
 * pages within a block to be written in order, write the pages in the
 * order we get them in the temporary area, recording this location, and
 * then move them all once we have computed all of the data. */
static fs_page_id
spm_tmp_alloc (void)
{
  fs_page_id res;
  int result;

  if ((spm.tmp_next & ~spm.super->block_mask) == 0) {
    spm.tmp_next -= spm.super->data->block_size;
    if (spm.tmp_next <= (spm.next & spm.super->block_mask))
      FS_ERR_FATAL ("Not enough space to compute page tables (2)", 0, 0, 0);
    while (fs_device_bad_block_check (spm.dev,
          spm.tmp_next >> spm.super->block_shift))
    {
      spm.tmp_next -= spm.super->data->block_size;
      if (spm.tmp_next <= (spm.next & spm.super->block_mask))
        FS_ERR_FATAL ("Not enough space to compute page tables (3)", 0, 0, 0);
    }

    /* Erase this block. */
    result = fs_device_erase_block (spm.dev,
          spm.tmp_next / spm.super->data->block_size, FS_FOP_FACTORY_RECOVERY);
    if (result != FS_DEVICE_OK) {
      FS_ERR_FATAL ("Erase failure (2) in ptable region", 0, 0, 0);
    }
  }

  res = spm.tmp_next;

  /* Advance.  If it goes off of the end of the block, wrap it back to the
   * beginning, and the next time this function is called, it will be moved
   * back to a previous block. */
  spm.tmp_next = ((spm.tmp_next & spm.super->block_mask) |
      ((spm.tmp_next + 1) & ~spm.super->block_mask));

  return res;
}

/* Flush relocation page. */
static void
spm_reloc_flush (void)
{
  fs_page_id rel_page;
  int result;

  rel_page = spm_tmp_alloc ();
  result = fs_device_write_page (spm.dev, rel_page, spm.relocs,
      FS_FOP_FACTORY_RECOVERY);
  if (result != FS_DEVICE_OK) {
    FS_ERR_FATAL ("Unable to write page during recovery (2)", 0, 0, 0);
  }

  if (spm.reloc_page_count >=
      spm.super->data->page_size / sizeof (fs_page_id))
  {
    /* If there is enough data that this happens, we need to find another
     * place or way to store these.  On a 512-byte device, this value is
     * sufficient to handle a 256MB flash allowing for a few blocks to go
     * bad.  If we encounter larger 512-byte devices, this will have to be
     * changed. */
    FS_ERR_FATAL ("Too many reloc pages", 0, 0, 0);
  }
  /* Record this. */
  spm.reloc_pages[spm.reloc_page_count++] = rel_page;

  // printf ("Reloc: 0x%05x\n", rel_page);
  spm.reloc_count = 0;
  memset (spm.relocs, 0xFF, spm.super->data->page_size);
}

/* Record the movement of a relocated page. */
static void
spm_record_location (fs_page_id actual, fs_page_id desired)
{
  if (spm.reloc_count >= RELOCS_PER_PAGE) {
    spm_reloc_flush ();
  }

  spm.relocs[spm.reloc_count].actual = actual;
  spm.relocs[spm.reloc_count].desired = desired;
  spm.reloc_count++;
}

/* Flush out a single page entry.  Writes the page into a temporary
 * location, and the pages will be copied later. */
static void
spm_flush (struct single_page *pg)
{
  int result;
  fs_page_id real_dest;

  real_dest = spm_tmp_alloc ();
  // printf ("W 0x%05x (at 0x%05x)\n", pg->location, real_dest);
  result = fs_device_write_page (spm.dev, real_dest, pg->buf,
      FS_FOP_FACTORY_RECOVERY);
  if (result != FS_DEVICE_OK) {
    FS_ERR_FATAL ("Unable to write page table during recovery", 0, 0, 0);
  }
  spm_record_location (real_dest, pg->location);
}

/* Update a single entry in the given page table.  The table indexes are as
 * follows:
 *   0 - Forward table for data region.
 *   1 - Reverse table for data region.
 *   2 - Reverse table for ptable region.
 * Each table is assumed to be written consecutively, from the start.  The
 * higher level tables from the second two may overlap. */
static void
spm_table_set (uint32 key, uint32 value, unsigned table, unsigned level)
{
  uint32 table_index = key & spm.mask_off[level];
  uint32 true_table = (table == SPM_TABLE_FORWARD) ? FS_PTABLE : FS_RTABLE;
  struct single_page *pg;
  int i;

  if (level == 0) {
    fs_super_set_ptable (spm.super->data, true_table,
                         PTABLE_INDEX (spm, key, level), value);
    return;
  }

  pg = spm.cache[table][level-1].pg;

  /* Find the appropriate entry. */
  if (pg == NULL ||
      pg->index != table_index)
  {
    if (pg != NULL) {
      if (pg->ref == 1) {
        spm_flush (pg);
        pg->ref = 0;
      } else {
        pg->ref--;
      }
    }

    /* First, scan for an entry that matches. */
    for (i = 0; i < FS_FACTORY_TOTAL_TABLES; i++) {
      if (spm.tables[i].table == true_table &&
          spm.tables[i].index == table_index &&
          spm.tables[i].level == level)
        break;
    }

    if (i < FS_FACTORY_TOTAL_TABLES) {
      spm.tables[i].ref++;
      pg = &spm.tables[i];
    } else {
      /* Otherwise, just find an empty table. */
      for (i = 0; i < FS_FACTORY_TOTAL_TABLES; i++) {
        if (spm.tables[i].ref == 0)
          break;
      }

      if (i == FS_FACTORY_TOTAL_TABLES)
        FS_ERR_FATAL ("Internal error, not enough tables available", 0, 0, 0);
      pg = &spm.tables[i];

      pg->ref = 1;
      pg->table = true_table;
      pg->index = table_index;
      pg->level = level;
      pg->location = spm_alloc ();

      memset (pg->buf, 0xFF, spm.super->data->page_size);

      /* Record this in the previous table.  Note this is recursive. */
      spm_table_set (key & spm.mask_off[level], pg->location,
          table, level - 1);

      /* Also, record a reverse entry. */
      spm_table_set (pg->location,
          FS_RMAP_PAGE_TABLE (true_table, level, table_index),
          SPM_TABLE_PTABLE_REV,
          spm.last_level);

    }

    spm.cache[table][level-1].pg = pg;
  }

  pg->buf[PTABLE_INDEX (spm, key, level)] = value;
}

void
spm_set_reverse (fs_page_id page, fs_cluster_id cluster)
{
  spm_table_set (page, cluster, SPM_TABLE_REV, spm.last_level);
}

void
spm_set_forward (fs_cluster_id cluster, fs_page_id page)
{
  spm_table_set (cluster, page, SPM_TABLE_FORWARD, spm.last_level);
}

/* Go through the entire page table data (relocations), and copy the data,
 * in destination order, into the proper place.
 */
static void
spm_move_data (void)
{
  fs_page_id dest;
  unsigned reloc_page = 0;
  unsigned reloc_index = 0;
  unsigned wrap_count;
  reloc_t  reloc;
  int result;

  /* Loop through all of the physical pages. */
  for (dest = spm.super->data->u.nand.regions[0] * spm.super->data->block_size;
      dest < spm.next;
      dest++)
  {
    if (((dest & ~spm.super->block_mask) == 0) &&
        fs_device_bad_block_check (spm.dev, dest >> spm.super->block_shift))
    {
      /* Skip this block.  Set to the last page, and let the increment in
       * the loop do the advance. */
      dest |= ~spm.super->block_mask;
      continue;
    }
    wrap_count = 0;

    /* Scan until we find the the proper reloc entry.  Most will already be
     * the next entry, but out of order data will require that we scan
     * through the entire table. */
    while (1) {
      if (reloc_index == 0) {
        fs_device_read_page (spm.dev,
            spm.reloc_pages[reloc_page], spm.relocs,
            FS_FOP_FACTORY_RECOVERY, FS_FLASH_MUST_WORK);
      }

      reloc = &spm.relocs[reloc_index];

      reloc_index++;
      if (reloc_index >= RELOCS_PER_PAGE) {
        reloc_index = 0;
        reloc_page++;
        if (reloc_page >= spm.reloc_page_count) {
          reloc_page = 0;
          wrap_count++;

          /* If the scan wraps around several times without finding the
           * desired page, the page cannot be found.  This indicates
           * something wrong with the previously run code, and trying to
           * continue would result in filesystem corruption. */
          if (wrap_count == 3)
            FS_ERR_FATAL ("Unable to find destination page", 0, 0, 0);
        }
      }

      if (reloc->desired == dest)
        break;
    }

    fs_device_read_page (spm.dev, reloc->actual, spm.tmp_buf,
        FS_FOP_FACTORY_RECOVERY, FS_FLASH_MUST_WORK);
    result = fs_device_write_page (spm.dev, reloc->desired, spm.tmp_buf,
        FS_FOP_FACTORY_RECOVERY);
    if (result != FS_DEVICE_OK)
      FS_ERR_FATAL ("Unable to write reloc page", 0, 0, 0);
  }
}

void
spm_full_flush (void)
{
  int i;

  for (i = 0; i < FS_FACTORY_TOTAL_TABLES; i++) {
    if (spm.tables[i].ref > 0)
      spm_flush (&spm.tables[i]);
  }

  if (spm.reloc_count > 0)
    spm_reloc_flush ();

  spm_move_data ();
}

/***********************************************************************
 * Factory startup code.
 ***********************************************************************/

/* Scan for the presence of a factory image.  The image must start in the
 * first non-bad block of the EFS partition and occupy consecutive
 * good-blocks.  Returns FS_INVALID_PAGE_ID if there is no factory image, or
 * the page number of the start of the image. */
static fs_page_id
factory_scan (efs_device_t dev, uint8 *buffer)
{
  int block_size = fs_device_block_size (dev);
  fs_page_id page;
  int result, page_size, blk_size, blk_count;
  factory_header_t head;

  page = 0;
  while (1) {
    if (fs_device_bad_block_check (dev, page / block_size) != FS_DEVICE_OK) {
      page += block_size;
      continue;
    }

    result = fs_device_read_page (dev, page, buffer, FS_FOP_FACTORY_RECOVERY,
        FS_FLASH_FAIL_OK);
    if (result != FS_DEVICE_OK)
      return FS_INVALID_PAGE_ID;

    head = (factory_header_t) buffer;

    if (head->magic1 != EFS_FACTIMAGE_MAGIC1 ||
        head->magic2 != EFS_FACTIMAGE_MAGIC2)
      return FS_INVALID_PAGE_ID;

    /* Separate checks with more helpful logs. */
    if (head->fact_version != EFS_FACTIMAGE_VERSION) {
      FS_MSG_HIGH_2 ("Factimage version mismatch: %d, %d",
          head->fact_version, EFS_FACTIMAGE_VERSION);
      return FS_INVALID_PAGE_ID;
    }

    if (head->version > FS_SUPER_VERSION_NAND) {
      FS_MSG_HIGH_2 ("Factimage super version mismatch: %d, %d",
          head->version, FS_SUPER_VERSION_NAND);
      return FS_INVALID_PAGE_ID;
    }

    if (head->page_size != (unsigned) fs_device_page_size (dev) ||
        head->block_size != (unsigned) fs_device_block_size (dev) ||
        head->block_count != (unsigned) fs_device_block_count (dev))
    {
      FS_MSG_HIGH_3 (
                "Fact img dev layout differs PGSz:%d, BlkSz:%d, BlkCnt:%d",
                head->page_size, head->block_size, head->block_count);

      /* To keep lint happy (for error 666) */
      page_size = fs_device_page_size (dev);
      blk_size = fs_device_block_size (dev);
      blk_count = fs_device_block_count (dev);
      FS_MSG_HIGH_3 ("Expected : PGSz: %d, BlkSz: %d, BlkCnt: %d",
                      page_size, blk_size, blk_count);

      return FS_INVALID_PAGE_ID;
    }

    /* At this point, the factory image appears to be valid. */
    return page;
  }
}

/* Output the superblock into the appropriate region. */
static fs_page_id
fs_fact_output_superblock (fs_super_t super)
{
  uint32 block, eblock, crc;
  fs_page_id page, good_page;
  int result;
  unsigned bit;

  block = super->data->u.nand.regions[2];
  eblock = super->data->u.nand.regions[3];
  page = block << super->block_shift;

  /* Scan all of the blocks in the log region, and build the freemap. */
  super->data->u.nand.logr_badmap = 0;

  good_page = FS_INVALID_PAGE_ID;
  for (bit = 1;
      block < eblock;
      block++, page += super->data->block_size, bit <<= 1)
  {
    if (fs_device_bad_block_check (super->dev, block)) {
      super->data->u.nand.logr_badmap |= bit;
    } else if (good_page == FS_INVALID_PAGE_ID) {
      good_page = page;
    }
  }

  if (good_page == FS_INVALID_PAGE_ID)
    FS_ERR_FATAL ("No good blocks in log region", 0, 0, 0);

  result = fs_device_erase_block (super->dev,
      good_page >> super->block_shift, FS_FOP_FACTORY_RECOVERY);
  if (result != FS_DEVICE_OK)
    FS_ERR_FATAL ("Error erasing block in log region", 0, 0, 0);

  super->data->log_head = good_page + 1;

  /* Add CRC. */
  crc = crc_30_calc ((byte *) super->data, super->data->page_size * 8 - 32);
  fs_super_set_crc (super->data, crc);

  result = fs_device_write_page (super->dev, good_page, super->data,
      FS_FOP_FACTORY_RECOVERY);
  if (result != FS_DEVICE_OK)
    FS_ERR_FATAL ("Error writing page in log region", 0, 0, 0);

  return good_page;
}

void
fs_factory_init (uint32 page_size)
{
  fs_factory_page_buffer = fs_align_malloc (page_size);

  /* Initialize the factory module static variables. */
  split_sub_count = page_size / EFS_FACTIMAGE_PSIZE;
  split_shift = fs_get_shifts (split_sub_count);
}



fs_page_id
fs_fact_startup_check (fs_super_t super, efs_device_t dev)
{
  fs_page_id fact_start;
  fs_page_id page;
  fs_cluster_id cluster;
  int block_size;
  int i;
  struct fsb_nand_data *buf;
  uint8 *buffer;
  factory_header_t head;

  buf = fs_borrow_nand_data();
  ASSERT (buf != NULL);
  buffer = buf->buffers[0];
  ASSERT (buffer != NULL);
  head = (factory_header_t) buffer;

  fact_start = factory_scan (dev, buffer);

  if (fact_start == FS_INVALID_PAGE_ID)
    return FS_INVALID_PAGE_ID;

  fcache_init (dev, buf->buffers[1], fact_start);

  fs_pm_super_invalidate_superblocks ();

  super->data->version = FS_SUPER_VERSION_NAND;
  fs_super_create_superblock (super);

  /* Initialize the simple page manager. */
  spm_init (super, dev, buf, 2);

  block_size = fs_device_block_size (dev);

  /* Any blocks that preceed the factory header were detected as bad, so
   * mark them as such in the page tables. */
  for (page = 0; page < fact_start; page += block_size) {
    spm_set_reverse (page, FS_RMAP_BLOCK_BAD);
  }

  /* The factory header itself is just garbage, as far as the filesystem is
   * concerned. */
  spm_set_reverse (page, FS_RMAP_PAGE_GARBAGE);

  page = fact_start + 1;
  cluster = 1;

  while (cluster < head->space_limit) {
    /* When at a block boundary, make sure we skip to the next good block. */
    if ((page & ~super->block_mask) == 0) {
      while (fs_device_bad_block_check (dev, page >> super->block_shift)) {
        spm_set_reverse (page, FS_RMAP_BLOCK_BAD);
        page += super->data->block_size;
      }
    }

    /* Advance the cluster number to the next allocated cluster (from the
     * freemap. */
    while (cluster < head->space_limit &&
        fcache_is_free (cluster))
      cluster++;

    if (cluster < head->space_limit) {
      spm_set_forward (cluster, page);
      spm_set_reverse (page, cluster);
      cluster++;
      page++;
    }
  }

  /* Flush simple page table manager. */
  spm_full_flush ();

  /* Finalize superblock and write it out. */
  super->data->alloc_next[0] = page;
  super->data->gc_next[0] =(page & super->block_mask) +super->data->block_size;
  /* Meta data allocator need to be initialized. If it is
   * invalid page then PM will take care of placing a proper
   * value for this. */
  super->data->alloc_next[2] = FS_INVALID_PAGE_ID;

  super->data->alloc_next[1] = spm.next;
  super->data->gc_next[1] = (spm.next & super->block_mask) +
    super->data->block_size;

  for (i = 0; i < FS_UPPER_DATA_COUNT; i++) {
    super->data->upper_data[i] = head->upper_data[i];
  }

  FS_MSG_HIGH_0 ("EFS found and unpacked a Canonical EFS (CEFS) image.");
  return fs_fact_output_superblock (super);
}

#endif /* defined (FEATURE_EFS_EFS2_ON_NAND) || (FEATURE_EFS_EFS2_ON_NOR) ||
                  (FEATURE_EFS_EFS2_ON_SPI_NOR) */
#endif /* FEATURE_EFS_QEFS2 */

