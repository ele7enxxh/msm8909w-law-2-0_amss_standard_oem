/***********************************************************************
 * fs_otp.c
 *
 * EFS OTP (One-Time-Programmable) module.
 * Copyright (C) 2012-2013 QUALCOMM Technologies, Inc.
 *
 * OTP is One-Time-Programmable partition in Nand/Nor flash memory.
 * This file mounts a OTP filesystem and maintains this file system
 * on the OTP partition on the flash device.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_otp.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-07-22   rp    Add new EFS API to store data in SuperBlock.
2013-06-19   dks   Added NAND Flash scrub support.
2013-01-11   dks   Move writing to Golden partition outside of efs_sync.
2012-12-14   wek   Implement modified efs_utime for item files.
2012-12-18   dks   Update golden copy strategy for new rmts layout.
2012-12-05   nr    Fix compiler warnings.
2012-01-17   wek   Remove unused utils header file.
2011-11-02   wek   Use library supplied strl cpy and cat instead of fs_strl*.
2011-09-21   nr    Added borrow_ram support to borrow ram from lower layers.
2011-08-17   rp    Create

===========================================================================*/

#include "fs_otp.h"

#ifdef FEATURE_EFS_OTP

#include "fs_device.h"
#include "fs_vnode.h"
#include "fs_err.h"
#include "fs_mount.h"
#include "fs_public.h"
#include "fs_otp_pcache.h"
#include "fs_stdlib.h"

#include "assert.h"
#include "crc.h"

#include <string.h>
#include "stringl/stringl.h"


/*****************************************************************************

  OTP partition layout :-
  --------------------------------

    The OTP region starts with a database and followed by OTP records. The OTP
    records are variable length records; Each OTP-record is an diretory or an
    EFS-Item-file. OTP partition changes are first done in RAM only when the
    whole page is populated with data the page gets written to the OTP
    partition. When an EFS-Sync is forced, dummy OTP-records are added to
    avoid holes in OTP region; This dummy OTP records will have zero filepath
    length and the file-size will be till the end of the current page.


        |-----------------------|
        |                       |
        |     OTP Database      | OTP database header. Size = 12 Bytes.
        |       header          | ( struct fs_otp_db )
        |-----------------------|
        |                       | Variable length OTP record.
        |    OTP record 1       | Each OTP record is a item-file/directory.
        |                       | ( struct fs_otp_record_header + data ).
        |                       | First file.
        |-----------------------|
        |                       | Variable length OTP record.
        |    OTP record 2       | Each OTP record is a item-file/directory.
        |                       | ( struct fs_otp_record_header + data ).
        |                       | Second file.
        |-----------------------|
        |        ----           |
        |        ----           | More files.
        |        ----           |
        |-----------------------|
        |                       | Variable length OTP record.
        |    OTP record N       | Each OTP record is a item-file/directory.
        |                       | ( struct fs_otp_record_header + data ).
        |                       | Nth file.
        |-----------------------|
        |        ----           |
        |        ----           | Not yet written pages in the OTP region.
        |        ----           |
        |-----------------------|

*****************************************************************************/

/*-----------------------------------------------------------------------------
  OTP database data-types.
-----------------------------------------------------------------------------*/

/* DB-Magic number, In little-endian, this is viewed in ASCII as "QOTP". */
#define FS_OTP_DB_MAGIC               0x50544F51

#define FS_OTP_DB_VERSION             1
#define FS_OTP_DB_RECORD_VERSION      1

/* Each file ends with this marker. */
#define FS_OTP_RECORD_END_MARKER      0xAA55

/* The OTP database header. The first OTP page begins with this. */
PACKED struct fs_otp_db
{
  uint32        db_magic;
  uint8         db_version;
  uint8         db_record_version;
  uint8         reserved1;
  uint8         reserved2;
  uint32        page_size;
} PACKED_POST;

/* File header for every file in the OTP region. */
PACKED struct fs_otp_record_header
{
  uint16        file_mode;
  uint16        file_path_len;
  uint16        file_size;
} PACKED_POST;

#define FS_OTP_RECORD_HEADER_SIZE   (sizeof (struct fs_otp_record_header))


/*-----------------------------------------------------------------------------
  Mount-Point data-types.
-----------------------------------------------------------------------------*/
struct fs_otp_mount
{
  struct fs_mount                   parent;
  struct fs_vnode                   *root_vnode;

  efs_device_t                      dev;
  uint32                            dev_page_size;
  uint32                            dev_total_pages;

  /* The entire OTP partition in flash is viewed as continous set of pages and
   * is accessed by using offsets, For example offset-zero is the first byte
   * of the first page in the OTP partition and max_offset is the last byte
   * of the last page in the OTP partition.
   * curr_offset is the offset at which we have to add the next
   * OTP-record. While adding new OTP-records, the new OTP-record will get
   * curr_offset as the inode number. */
  uint32                            curr_offset;
  uint32                            max_offset;

  struct fs_otp_db                  otp_db;
  struct fs_otp_pcache              pcache;

  char                              path_buff[FS_OTP_PATH_MAX];

  uint32                            sync_token;
};

static struct fs_otp_mount fs_otp_mounts[FS_MAX_OTP_MOUNTS];

/* Page buffers used by the OTP-pCache module to maintain OTP pages in RAM. */
static FS_ALIGN uint8
            fs_otp_pcache_pages[FS_OTP_MAX_PCACHE_PAGES][FS_OTP_MAX_PAGE_SIZE];

/* iNode number of the OTP root-directory. */
#define FS_OTP_ROOT_INODE       2

/* Mountpoint operations proto-types. */
static struct fs_vnode* fs_otp_mount_create_root_vnode (struct fs_mount *mp);
static void fs_otp_compute_next_write_page (struct fs_otp_mount *mp);
static void fs_otp_init_otp_db (struct fs_otp_mount *mp);


static int
m_otp_start (struct fs_mount *base_mp, const void *args)
{
  struct fs_otp_mount *mp;
  uint32 i;

  (void) args;

  mp = (struct fs_otp_mount *)base_mp;
  ASSERT (mp != NULL);

  mp->dev = fs_device_open (FS_DEVICE_TYPE_OTP_FLASH, 0);
  if (mp->dev == NULL)
    return -ENODEV;

  mp->root_vnode = fs_otp_mount_create_root_vnode (base_mp);
  if (mp->root_vnode == NULL)
    return -ENOSPC;

  /* Get the OTP partition size from the Flash device. */
  mp->dev_page_size = fs_device_page_size (mp->dev);
  mp->dev_total_pages = fs_device_block_size (mp->dev);
  mp->max_offset = mp->dev_page_size * mp->dev_total_pages;
  if (mp->dev_total_pages >= FS_OTP_MAX_OTP_PAGES)
    mp->dev_total_pages = FS_OTP_MAX_OTP_PAGES;

  fs_otp_compute_next_write_page (mp);

  /* pCache initialization. */
  memset (&mp->pcache, 0, sizeof (struct fs_otp_pcache));
  memset (fs_otp_pcache_pages, 0, sizeof (fs_otp_pcache_pages));
  for (i = 0; i < FS_OTP_MAX_PCACHE_PAGES; ++i)
  {
    mp->pcache.pages[i] = fs_otp_pcache_pages[i];
    mp->pcache.status[i].page_nbr = FS_OTP_INVALID_PAGE;
  }
  fs_otp_pcache_init (&mp->pcache, mp->dev, mp->dev_total_pages);

  /* OTB database initialization. */
  memset (&mp->otp_db, 0, sizeof (mp->otp_db));
  fs_otp_init_otp_db (mp);

  return 0;
}


static void
fs_otp_create_otp_db (struct fs_otp_mount *mp)
{
  uint8 *buff_addr;
  uint32 bytes_cnt;

  mp->otp_db.db_magic = FS_OTP_DB_MAGIC;
  mp->otp_db.db_version = FS_OTP_DB_VERSION;
  mp->otp_db.db_record_version = FS_OTP_DB_RECORD_VERSION;
  mp->otp_db.reserved1 = mp->otp_db.reserved2 = 0;
  mp->otp_db.page_size = fs_device_page_size (mp->dev);

  buff_addr = (uint8 *)&mp->otp_db;
  bytes_cnt = sizeof (struct fs_otp_db);
  fs_otp_pcache_write_bytes (&mp->pcache, 0, bytes_cnt, buff_addr);
  mp->curr_offset = bytes_cnt;
}

static void
fs_otp_verify_otp_db (struct fs_otp_db *otp_db, efs_device_t dev)
{
  uint32 dev_page_size;

  if (otp_db->db_magic != FS_OTP_DB_MAGIC)
  {
    FS_ERR_FATAL ("[%d-%d] OTP DB magic mismatch",
      otp_db->db_magic, FS_OTP_DB_MAGIC, 0);
  }

  ASSERT (otp_db->reserved1 == 0);
  ASSERT (otp_db->reserved2 == 0);

  if (otp_db->db_version != FS_OTP_DB_VERSION)
  {
    FS_ERR_FATAL ("[%d-%d] OTP DB version mismatch",
      otp_db->db_version, FS_OTP_DB_VERSION, 0);
  }

  if (otp_db->db_record_version != FS_OTP_DB_RECORD_VERSION)
  {
    FS_ERR_FATAL ("[%d-%d] OTP DB Record version mismatch",
      otp_db->db_record_version, FS_OTP_DB_RECORD_VERSION, 0);
  }

  dev_page_size = fs_device_page_size (dev);
  if (otp_db->page_size != dev_page_size)
  {
    FS_ERR_FATAL ("[%d-%d] OTP page-size mismatch",
      otp_db->page_size, dev_page_size, 0);
  }

}

static void
fs_otp_init_otp_db (struct fs_otp_mount *mp)
{
  fs_otp_pcache_read_bytes (&mp->pcache, 0, sizeof (struct fs_otp_db),
                            (uint8 *)&mp->otp_db);
  if (mp->otp_db.db_magic == FS_OTP_DB_MAGIC)
  {
    fs_otp_verify_otp_db (&mp->otp_db, mp->dev);
  }
  else
  {
    fs_otp_create_otp_db (mp);
  }
}

static void
fs_otp_compute_next_write_page (struct fs_otp_mount *mp)
{
  uint32 page_nbr;
  int result;

  page_nbr = 0;
  while (page_nbr < mp->dev_total_pages)
  {
    result = fs_device_read_page (mp->dev, page_nbr, fs_otp_pcache_pages[0],
                                  FS_FOP_USER, FS_FLASH_FAIL_OK);
    if (result == FS_DEVICE_FAIL_PAGE_ERASED)
      break;

    ++page_nbr;
  }

  mp->curr_offset = page_nbr * mp->dev_page_size;
  ASSERT (mp->curr_offset <= mp->max_offset);
}


static int
m_otp_stop (struct fs_mount *base_mp)
{
  struct fs_otp_mount *mp;

  mp = (struct fs_otp_mount *)base_mp;
  ASSERT (mp != NULL);

  return 0;
}

static int
m_otp_get_root (struct fs_mount *base_mp, struct fs_vnode **rp)
{
  struct fs_otp_mount *mp;

  mp = (struct fs_otp_mount *)base_mp;
  ASSERT (mp != NULL);

  *rp = mp->root_vnode;
  fs_vnode_ref (*rp);

  return 0;
}

static struct fs_mount_ops fs_otp_mount_ops = {
  m_otp_start,
  m_otp_stop,
  m_otp_get_root,
  NULL, /* m_otp_create_inode */
  NULL, /* m_otp_write_inode, */
  NULL, /* m_otp_read_inode */
  NULL, /* m_otp_read_named_inode */
  NULL, /* fs_otp_mount_op_cleanup */
};

/*-----------------------------------------------------------------------------
  OTP Iterators.
-----------------------------------------------------------------------------*/

/* OTP record iterator count : As many as OTP-Directory iterators +
                               1 for v_opt_lookup  */
#define FS_OTP_MAX_ITERS_I   (FS_OTP_MAX_ITERS + 1)

struct fs_otp_iter_record
{
  uint32                                offset;
  struct fs_otp_record_header           otp_rec;
  char                                  path_buff[FS_OTP_PATH_MAX];
};

struct fs_otp_iter
{
  int                               is_in_use;
  struct fs_otp_mount               *mp;

  uint32                            curr_offset;
  uint32                            max_offset;

  uint32                            rec_hdr_size;

  struct fs_otp_iter_record        iter_record;
};

struct fs_otp_iter fs_otp_iters[FS_OTP_MAX_ITERS_I];

int
fs_otp_iter_get_slot (void)
{
  int i;

  for (i = 0; i < FS_OTP_MAX_ITERS_I; ++i)
  {
    if (!fs_otp_iters[i].is_in_use)
    {
      memset (&fs_otp_iters[i], 0, sizeof (fs_otp_iters[0]));
      fs_otp_iters[i].is_in_use = 1;
      break;
    }
  }

  ASSERT (i < FS_OTP_MAX_ITERS_I);
  return i;
}


struct fs_otp_iter*
fs_otp_iter_open (struct fs_otp_mount *mp)
{
  int i;

  i = fs_otp_iter_get_slot ();

  fs_otp_iters[i].mp = mp;
  fs_otp_iters[i].rec_hdr_size = FS_OTP_RECORD_HEADER_SIZE;

  /* Start iterating 1st byte after the DB-Header. */
  fs_otp_iters[i].curr_offset = sizeof (struct fs_otp_db);
  fs_otp_iters[i].max_offset = mp->max_offset;

  return &fs_otp_iters[i];
}

struct fs_otp_iter_record*
fs_otp_iter_read (struct fs_otp_iter *otp_iter)
{
  struct fs_otp_iter_record *iter_rec;
  struct fs_otp_record_header *otp_rec;
  uint32 end_page_nbr, end_rec_offset, curr_page;
  uint16 end_marker;
  int found_valid_rec = 0;

  iter_rec = &otp_iter->iter_record;
  otp_rec = &iter_rec->otp_rec;

  while (!found_valid_rec)
  {
    if (otp_iter->curr_offset >= otp_iter->max_offset)
      goto end_iter;

    fs_otp_pcache_read_bytes (&otp_iter->mp->pcache, otp_iter->curr_offset,
                              otp_iter->rec_hdr_size, (uint8 *)otp_rec);

    if ((otp_rec->file_path_len == 0xffff) &&
        (otp_rec->file_size == 0xffff))
      goto end_iter;

    end_rec_offset = otp_iter->rec_hdr_size + otp_rec->file_path_len +
                     otp_rec->file_size;
    end_rec_offset += otp_iter->curr_offset;
    end_page_nbr = end_rec_offset / otp_iter->mp->dev_page_size;
    if (end_page_nbr >= otp_iter->mp->dev_total_pages)
      goto end_iter;

    fs_otp_pcache_read_bytes (&otp_iter->mp->pcache, end_rec_offset,
                              sizeof (end_marker), (uint8*)&end_marker);
    if (end_marker != FS_OTP_RECORD_END_MARKER)
    {
      curr_page = otp_iter->curr_offset / otp_iter->mp->dev_page_size;
      otp_iter->curr_offset = (curr_page + 1) * otp_iter->mp->dev_page_size;
      continue;
    }

    if (otp_rec->file_path_len != 0)
    {
      found_valid_rec = 1;

      iter_rec->offset = otp_iter->curr_offset;
      otp_iter->curr_offset += otp_iter->rec_hdr_size;
      memset (iter_rec->path_buff, 0, sizeof (iter_rec->path_buff));
      fs_otp_pcache_read_bytes (&otp_iter->mp->pcache, otp_iter->curr_offset,
                        otp_rec->file_path_len, (uint8*)iter_rec->path_buff);
    }
    otp_iter->curr_offset = end_rec_offset + sizeof (end_marker);

  }

  return iter_rec;

end_iter:
  return NULL;
}

int
fs_otp_iter_close (struct fs_otp_iter *otp_iter)
{
  ASSERT (otp_iter->is_in_use == 1);
  otp_iter->is_in_use = 0;
  return 0;
}

static uint32
fs_otp_search_file (struct fs_otp_mount *mp, const char *fname)
{
  struct fs_otp_iter *otp_iter;
  struct fs_otp_iter_record *iter_rec;
  int result;
  uint32 file_offset = 0;

  otp_iter = fs_otp_iter_open (mp);
  ASSERT (otp_iter != NULL);

  iter_rec = fs_otp_iter_read (otp_iter);
  while (iter_rec != NULL)
  {
    if (strncmp (fname, (const char *)iter_rec->path_buff,
                 sizeof (iter_rec->path_buff)) == 0)
    {
      file_offset = iter_rec->offset;
      break;
    }

    iter_rec = fs_otp_iter_read (otp_iter);
  }

  result = fs_otp_iter_close (otp_iter);
  ASSERT (result == 0);

  return file_offset;
}

/*-----------------------------------------------------------------------------
  OTP vNode operations.
-----------------------------------------------------------------------------*/
extern const struct vfs_ops fs_otp_dir_vops;
extern const struct vfs_ops fs_otp_item_vops;

static struct fs_vnode*
fs_otp_mount_create_root_vnode (struct fs_mount *base_mp)
{
  struct fs_vnode *vnode;

  vnode = fs_vnode_alloc ();
  if (vnode == NULL)
    return NULL;

  fs_inode_construct (&vnode->p.inode, S_IFDIR | 0777, FS_GROUP_ZERO,
                      FS_CURR_UID);
  vnode->mp = base_mp;
  vnode->vops = &fs_otp_dir_vops;

  vnode->inum = FS_OTP_ROOT_INODE;
  vnode->dirty = 0;
  vnode->mode = vnode->p.inode.mode;
  vnode->dev = base_mp->dev;

  return vnode;
}

char*
fs_otp_fix_path (const char *orig_path, char *fixed_path, uint32 path_size)
{
  char *fname = NULL;
  uint32 path_len;

  memset (fixed_path, 0, path_size);
  efs_normalize_path_string (orig_path, fixed_path, path_size);

  fname = fixed_path;
  if (*fname == '/')
    ++fname;

  path_len = strlen (fname);
  if (path_len < 4)
    return NULL;

  if (strncmp (fname, "otp", strlen ("otp")) == 0)
    fname += 4;

  if (*fname == '.')
    ++fname;

  if (*fname == '/')
    ++fname;

  return fname;
}


static int
v_otp_lookup (struct nameidata *ndp)
{
  struct fs_otp_mount *mp;
  struct fs_otp_record_header otp_rec;
  struct fs_vnode *vp;
  uint32 foffset, data_offset;
  char temp_ch, *nameend, *fname;
  uint32 file_path_len;
  int result = 0;

  mp = (struct fs_otp_mount *)ndp->dvp->mp;
  ASSERT (mp != NULL);

  /* terminate the path-string, till the place where we should look */
  nameend = (char *)(ndp->next + ndp->length);
  temp_ch = *nameend;
  *nameend = '\0';

  fname = fs_otp_fix_path (ndp->path, mp->path_buff, sizeof (mp->path_buff));
  if (fname == NULL)
  {
    result = -ENOENT;
    goto end;
  }

  file_path_len = strlen (fname);
  if (file_path_len >= FS_OTP_PATH_MAX)
  {
    result = -ENAMETOOLONG;
    goto end;
  }
  else if (file_path_len == 0)
  {
    ndp->vp = mp->root_vnode;
    fs_vnode_ref (ndp->vp);
    goto end;
  }

  foffset = fs_otp_search_file (mp, fname);
  if (foffset == 0)
  {
    result = -ENOENT;
    goto end;
  }

  fs_otp_pcache_read_bytes (&mp->pcache, foffset,
                            sizeof (otp_rec), (uint8* )&otp_rec);
  ASSERT (otp_rec.file_path_len == (uint16)file_path_len);

  vp = fs_vnode_lookup (&mp->parent, mp->parent.dev, foffset);
  if (vp == NULL)
    vp = fs_vnode_alloc ();
  if (vp == NULL)
    return -ENOMEM;

  data_offset = foffset + FS_OTP_RECORD_HEADER_SIZE + file_path_len;

  vp->mp = &mp->parent;
  vp->dev = mp->parent.dev;
  vp->inum = foffset;
  vp->mode = otp_rec.file_mode;
  vp->dirty = 0;
  vp->p.inode.mode = otp_rec.file_mode;
  vp->p.inode.size = (uint32)otp_rec.file_size;
  vp->p.inode.data[0] = data_offset;

  if (S_ISDIR (vp->mode))
    vp->vops = &fs_otp_dir_vops;
  else if (S_ISITM (vp->mode))
    vp->vops = &fs_otp_item_vops;
  else
    FS_ERR_FATAL ("[%d] invalid otp mode", vp->mode, 0, 0);

  ndp->vp = vp;

end:
  /* restore the path-string to its original state */
  *nameend = temp_ch;

  return result;
}

static int
fs_otp_add_file (struct fs_otp_mount *mp, const char *fname,
                 int mode, uint8 *data, int length)
{
  struct fs_otp_record_header otp_rec;
  uint32 bytes_cnt, temp_offset;
  uint16 end_marker;
  uint8* buff_addr;
  int path_len;

  path_len = strlen (fname);
  if (path_len >= FS_OTP_PATH_MAX)
    return -ENAMETOOLONG;

  if (length > FS_OTP_MAX_TRANS_BYTES)
    return -FS_ERANGE;

  otp_rec.file_mode = (uint16)mode;
  otp_rec.file_path_len = (uint16)path_len;
  otp_rec.file_size = (uint16)length;

  temp_offset = mp->curr_offset + FS_OTP_RECORD_HEADER_SIZE +
               otp_rec.file_path_len + otp_rec.file_size + sizeof (end_marker);
  if (temp_offset >= mp->max_offset)
    return -ENOSPC;

  /* Write the record-header. */
  buff_addr = (uint8* )&otp_rec;
  bytes_cnt = sizeof (otp_rec);
  fs_otp_pcache_write_bytes (&mp->pcache,mp->curr_offset,bytes_cnt,buff_addr);
  mp->curr_offset += bytes_cnt;

  /* Write the file path. */
  buff_addr = (uint8* )fname;
  bytes_cnt = (uint32 )path_len;
  fs_otp_pcache_write_bytes (&mp->pcache,mp->curr_offset,bytes_cnt,buff_addr);
  mp->curr_offset += bytes_cnt;

  /* Write the file data. */
  if (otp_rec.file_size != 0)
  {
    bytes_cnt = (uint32 )otp_rec.file_size;
    fs_otp_pcache_write_bytes (&mp->pcache, mp->curr_offset, bytes_cnt, data);
    mp->curr_offset += bytes_cnt;
  }

  /* Write the record end-marker. */
  end_marker = FS_OTP_RECORD_END_MARKER;
  buff_addr = (uint8* )&end_marker;
  bytes_cnt = (uint32 )sizeof (end_marker);
  fs_otp_pcache_write_bytes (&mp->pcache,mp->curr_offset,bytes_cnt,buff_addr);
  mp->curr_offset += bytes_cnt;

  return 0;
}

static int
v_otp_mkdir (struct nameidata *ndp, fs_mode_t mode)
{
  struct fs_otp_mount *mp;
  uint32 file_offset;
  char *fname;
  int result;

  mp = (struct fs_otp_mount *)ndp->dvp->mp;
  ASSERT (mp != NULL);

  fname = fs_otp_fix_path (ndp->path, mp->path_buff, sizeof (mp->path_buff));
  ASSERT (fname != NULL);

  file_offset = fs_otp_search_file (mp, fname);
  ASSERT (file_offset == 0);

  mode = S_IFDIR | (mode & 07777);
  result = fs_otp_add_file (mp, fname, mode, NULL, 0);
  return result;
}

static int
v_otp_put_item (struct nameidata *ndp, void *data, int length, int mode)
{
  struct fs_otp_mount *mp;
  uint32 file_offset;
  char *fname;
  int result;

  mp = (struct fs_otp_mount *)ndp->dvp->mp;
  ASSERT (mp != NULL);

  fname = fs_otp_fix_path (ndp->path, mp->path_buff, sizeof (mp->path_buff));
  ASSERT (fname != NULL);

  file_offset = fs_otp_search_file (mp, fname);
  if (file_offset != 0)
  {
    return -EEXIST;
  }

  mode = S_IFITM | (mode & 07777);
  result = fs_otp_add_file (mp, fname, mode, data, length);
  return result;
}

static int
v_otp_get_item (struct fs_vnode *vp, void *data, int length)
{
  struct fs_otp_mount *mp;
  uint32 data_offset;

  mp = (struct fs_otp_mount *)vp->mp;
  ASSERT (mp != NULL);

  if (vp->p.inode.size != (uint32)length)
    return -FS_ERANGE;

  data_offset = vp->p.inode.data[0];
  fs_otp_pcache_read_bytes (&mp->pcache, data_offset, length, data);

  return 0;
}

static int
v_otp_getstat (struct fs_vnode *vp, struct fs_stat *buf)
{
  struct fs_otp_mount *mp;

  mp = (struct fs_otp_mount *)vp->mp;
  ASSERT (mp != NULL);

  buf->st_dev = vp->dev;
  buf->st_ino = vp->inum;
  buf->st_mode = vp->mode;
  buf->st_nlink = 1;
  buf->st_size = vp->p.inode.size;
  buf->st_blksize = mp->dev_page_size;
  buf->st_blocks = vp->p.inode.size / mp->dev_page_size;
  if (vp->p.inode.size % mp->dev_page_size)
    ++buf->st_blocks;
  buf->st_atime = buf->st_mtime = buf->st_ctime = 0;
  buf->st_rdev = 0;
  buf->st_uid = 0;
  buf->st_gid = 0;

  return 0;
}


int
v_otp_getstatvfs (struct fs_vnode *vp, struct fs_statvfs *buf)
{
  struct fs_otp_mount *mp;
  uint32 curr_page;

  mp = (struct fs_otp_mount *)vp->mp;
  ASSERT (mp != NULL);

  memset (buf, 0, sizeof (struct fs_statvfs));

  curr_page = mp->curr_offset / mp->dev_page_size;

  buf->f_bsize = mp->dev_page_size;
  buf->f_blocks = mp->dev_total_pages;
  buf->f_bfree = mp->dev_total_pages - curr_page;
  buf->f_bavail = buf->f_bfree;
  buf->f_namemax = FS_OTP_PATH_MAX;
  buf->f_pathmax = FS_OTP_PATH_MAX;
  buf->f_maxwrite = FS_OTP_MAX_TRANS_BYTES;
  buf->f_is_case_sensitive = 1;
  buf->f_is_case_preserving = 1;
  buf->f_max_file_size = FS_OTP_MAX_TRANS_BYTES;
  buf->f_max_file_size_unit = 1;
  buf->f_name_rule = FS_FILENAME_RULE_8BIT_RELAXED;
  buf->f_name_encoding = FS_FILENAME_ENCODING_8BIT;
  return 0;
}


/*-----------------------------------------------------------------------------
  OTP Directory Iterators.
-----------------------------------------------------------------------------*/
struct fs_otp_dir_iter
{
  int                         in_use;
  struct fs_otp_iter          *otp_iter;
  int                         dots_count;
  char                        path_buff[FS_OTP_PATH_MAX];
};

static struct fs_otp_dir_iter  fs_otp_dir_iters[FS_OTP_MAX_ITERS];

static void*
v_otp_opendir (struct fs_vnode *vp)
{
  struct fs_otp_mount *mp;
  struct fs_otp_dir_iter *dir_iter = NULL;

  mp = (struct fs_otp_mount *)vp->mp;
  ASSERT (mp != NULL);

  if (fs_otp_dir_iters[0].in_use)
    return NULL;

  memset (&fs_otp_dir_iters[0], 0, sizeof (fs_otp_dir_iters[0]));
  fs_otp_dir_iters[0].in_use = 1;

  dir_iter = &fs_otp_dir_iters[0];

  if (vp->inum == FS_OTP_ROOT_INODE)
  {
    dir_iter->path_buff[0] = 0;
  }
  else
  {
    struct fs_otp_iter *otp_iter;
    struct fs_otp_iter_record *iter_rec;

    otp_iter = fs_otp_iter_open (mp);
    ASSERT (otp_iter != NULL);

    iter_rec = fs_otp_iter_read (otp_iter);
    while (iter_rec != NULL)
    {
      if (iter_rec->offset == vp->inum)
        break;

      iter_rec = fs_otp_iter_read (otp_iter);
    }

    ASSERT (iter_rec != NULL);
    strlcpy (dir_iter->path_buff, (const char *)iter_rec->path_buff,
                sizeof (dir_iter->path_buff));

    fs_otp_iter_close (otp_iter);
  }

  dir_iter->otp_iter = fs_otp_iter_open (mp);
  if (dir_iter->otp_iter == NULL)
  {
    memset (dir_iter, 0, sizeof (struct fs_otp_dir_iter));
    return NULL;
  }

  dir_iter->dots_count = 0;

  return dir_iter;
}



int
v_otp_readdir (struct fs_vnode *vp, void *dir,
               struct fs_dirent *dirent)
{
  struct fs_otp_mount *mp;
  struct fs_otp_dir_iter *dir_iter;
  struct fs_otp_iter_record *iter_rec;
  int len1, len2;

  mp = (struct fs_otp_mount *)vp->mp;
  ASSERT (mp != NULL);

  dir_iter = (struct fs_otp_dir_iter *)dir;
  if (dir_iter == NULL)
    return -EINVAL;

  ASSERT (dir_iter->otp_iter != NULL);

  if (dir_iter->dots_count == 0)
  {
    ++dir_iter->dots_count;
    strlcpy (dirent->d_name, ".", sizeof (dirent->d_name));
    return 0;
  }
  else if (dir_iter->dots_count == 1)
  {
    ++dir_iter->dots_count;
    strlcpy (dirent->d_name, "..", sizeof (dirent->d_name));
    return 0;
  }
  ASSERT (dir_iter->dots_count == 2);

  iter_rec = fs_otp_iter_read (dir_iter->otp_iter);
  while (iter_rec != NULL)
  {
    if (dir_iter->path_buff[0] == 0)
    {
      /* It is OTP root-directory */
      len1 = len2 = 0;
      while ((iter_rec->path_buff[len2] != 0) &&
             (iter_rec->path_buff[len2] != '/'))
      {
        ++len2;
      }

      if (iter_rec->path_buff[len2] == 0)
        break;
    }
    else
    {
      len1 = strlen (dir_iter->path_buff);

      if (strncmp (dir_iter->path_buff, iter_rec->path_buff, len1) == 0)
      {
        if (iter_rec->path_buff[len1] == '/')
        {
          ++len1;

          len2 = len1;

          while ((iter_rec->path_buff[len2] != 0) &&
                 (iter_rec->path_buff[len2] != '/'))
          {
            ++len2;
          }

          if (iter_rec->path_buff[len2] == 0)
            break;
        }
      }
    }

    iter_rec = fs_otp_iter_read (dir_iter->otp_iter);
  }

  if (iter_rec == NULL)
    return -EEOF;

  strlcpy (dirent->d_name, (iter_rec->path_buff + len1),
              sizeof (dirent->d_name));
  dirent->d_stats_present = 1;
  dirent->d_stat.st_dev = vp->dev;
  dirent->d_stat.st_ino = iter_rec->offset;
  dirent->d_stat.st_mode = iter_rec->otp_rec.file_mode;
  dirent->d_stat.st_nlink = 1;
  dirent->d_stat.st_size = iter_rec->otp_rec.file_size;
  dirent->d_stat.st_blksize = mp->dev_page_size;
  dirent->d_stat.st_blocks = iter_rec->otp_rec.file_size / mp->dev_page_size;
  if (iter_rec->otp_rec.file_size % mp->dev_page_size)
    ++dirent->d_stat.st_blocks;
  dirent->d_stat.st_atime = 0;
  dirent->d_stat.st_mtime = 0;
  dirent->d_stat.st_ctime = 0;
  dirent->d_stat.st_rdev = 0;
  dirent->d_stat.st_uid = 0;
  dirent->d_stat.st_gid = 0;

  return 0;
}

int
v_otp_closedir (struct fs_vnode *vp, void *dir)
{
  struct fs_otp_dir_iter *dir_iter;
  int result;

  ASSERT (vp != NULL);

  dir_iter = (struct fs_otp_dir_iter *)dir;
  if (dir_iter == NULL)
    return -EINVAL;

  if (!dir_iter->in_use)
    return -EINVAL;

  result = fs_otp_iter_close (dir_iter->otp_iter);
  ASSERT (result == 0);

  memset (dir_iter, 0, sizeof (struct fs_otp_dir_iter));
  return 0;
}

int
v_otp_sync_start (struct fs_vnode *vp)
{
  struct fs_otp_mount *mp;
  uint32 curr_page, next_page_offset, avail_bytes, file_size, rec_size;
  struct fs_otp_record_header otp_rec;
  uint32 last_offset;
  uint8 last_offset_val;
  uint16 end_marker;

  mp = (struct fs_otp_mount *)vp->mp;
  ASSERT (mp != NULL);

  if (mp->curr_offset % mp->dev_page_size == 0)
    return 1; /* Nothing to sync. */

  curr_page = mp->curr_offset / mp->dev_page_size;
  next_page_offset = (curr_page + 1) * mp->dev_page_size;
  avail_bytes = next_page_offset - mp->curr_offset;

  rec_size = FS_OTP_RECORD_HEADER_SIZE + sizeof (end_marker);

  if (avail_bytes >= rec_size)
    file_size = avail_bytes - rec_size;
  else
    file_size = (avail_bytes + mp->dev_page_size) - rec_size;

  otp_rec.file_mode = 0;
  otp_rec.file_path_len = 0;
  otp_rec.file_size = (uint16)file_size;

  fs_otp_pcache_write_bytes (&mp->pcache, mp->curr_offset,
                             FS_OTP_RECORD_HEADER_SIZE, (uint8*)&otp_rec);
  mp->curr_offset += FS_OTP_RECORD_HEADER_SIZE;

  if (avail_bytes < rec_size)
  {
    if (avail_bytes > FS_OTP_RECORD_HEADER_SIZE)
    {
      last_offset = next_page_offset - 1;
      last_offset_val = 0xFF;
      fs_otp_pcache_write_bytes (&mp->pcache, last_offset,1, &last_offset_val);
    }
    next_page_offset += mp->dev_page_size;
  }

  mp->curr_offset += file_size;
  end_marker = FS_OTP_RECORD_END_MARKER;
  fs_otp_pcache_write_bytes (&mp->pcache, mp->curr_offset,
                             sizeof (end_marker), (uint8*)&end_marker);

  mp->curr_offset += sizeof (end_marker);
  ASSERT (mp->curr_offset == next_page_offset);

  fs_otp_pcache_write_changed_pages (&mp->pcache);

  return 1; /* Sync complete and no need to wait. */
}

int
v_otp_sync_wait (struct fs_vnode *vp)
{
  struct fs_otp_mount *mp;

  mp = (struct fs_otp_mount *)vp->mp;
  ASSERT (mp != NULL);

  return 0;
}

int
v_otp_sync_end (struct fs_vnode *vp)
{
  struct fs_otp_mount *mp;

  mp = (struct fs_otp_mount *)vp->mp;
  ASSERT (mp != NULL);

  return 0;
}

int
v_otp_sync_no_wait (struct fs_vnode *vp, fs_sync_token_t *sync_token)
{
  struct fs_otp_mount *mp;
  int result;

  mp = (struct fs_otp_mount *)vp->mp;
  ASSERT (mp != NULL);

  result = v_otp_sync_start (vp);

  if (result == 0)
    *sync_token = mp->sync_token++;

  return result;
}

int
v_otp_sync_get_status (struct fs_vnode *vp, fs_sync_token_t sync_token,
                       enum fs_sync_result *sync_result)
{
  struct fs_otp_mount *mp;

  mp = (struct fs_otp_mount *)vp->mp;
  ASSERT (mp != NULL);

  *sync_result = (sync_token < mp->sync_token) ? FS_SYNC_COMPLETE :
                                                 FS_SYNC_PENDING;
  return 0;
}


const struct vfs_ops fs_otp_dir_vops =
{
  v_otp_lookup,
  fs_vnode_invalid_create,
  fs_vnode_invalid_link,
  fs_vnode_invalid_unlink,
  fs_vnode_invalid_write,
  fs_vnode_invalid_read,
  fs_vnode_invalid_truncate,
  v_otp_mkdir,
  fs_vnode_invalid_rmdir,
  v_otp_opendir,
  v_otp_readdir,
  v_otp_closedir,
  v_otp_getstat,
  fs_vnode_invalid_getsize,
  fs_vnode_invalid_rename,
  fs_vnode_invalid_symlink,
  fs_vnode_invalid_readlink,
  v_otp_getstatvfs,
  v_otp_put_item,
  fs_vnode_invalid_get_item,
  fs_vnode_invalid_mknod,
  fs_vnode_invalid_chmod,
  fs_vnode_invalid_chown,
  fs_vnode_invalid_set_reservation,
  fs_vnode_invalid_set_quota,
  fs_vnode_invalid_get_group_info,
  fs_vnode_invalid_shred,
  fs_vnode_invalid_scrub,
  fs_vnode_invalid_utime,
  fs_vnode_invalid_utime, /* For OTP utime_item is the same as utime. */
  v_otp_sync_start,
  v_otp_sync_wait,
  v_otp_sync_end,
  v_otp_sync_no_wait,
  fs_vnode_invalid_sync_get_status,
  fs_vnode_invalid_get_device_attr,
  fs_vnode_invalid_get_fs_data,
  fs_vnode_invalid_image_prepare,
  fs_vnode_invalid_get_volume_name,
  fs_vnode_invalid_set_volume_name,
  fs_vnode_invalid_make_golden_copy,
  fs_vnode_invalid_fs_init_complete,
  fs_vnode_invalid_lock_efs,
  fs_vnode_invalid_borrow_ram,
  fs_vnode_invalid_store_data_in_sb
};

const struct vfs_ops fs_otp_item_vops =
{
  fs_vnode_invalid_lookup,
  fs_vnode_invalid_create,
  fs_vnode_invalid_link,
  fs_vnode_invalid_unlink,
  fs_vnode_invalid_write,
  fs_vnode_invalid_read,
  fs_vnode_invalid_truncate,
  fs_vnode_invalid_mkdir,
  fs_vnode_invalid_rmdir,
  fs_vnode_invalid_opendir,
  fs_vnode_invalid_readdir,
  fs_vnode_invalid_closedir,
  v_otp_getstat,
  fs_vnode_invalid_getsize,
  fs_vnode_invalid_rename,
  fs_vnode_invalid_symlink,
  fs_vnode_invalid_readlink,
  v_otp_getstatvfs,
  fs_vnode_invalid_put_item,
  v_otp_get_item,
  fs_vnode_invalid_mknod,
  fs_vnode_invalid_chmod,
  fs_vnode_invalid_chown,
  fs_vnode_invalid_set_reservation,
  fs_vnode_invalid_set_quota,
  fs_vnode_invalid_get_group_info,
  fs_vnode_invalid_shred,
  fs_vnode_invalid_scrub,
  fs_vnode_invalid_utime,
  fs_vnode_invalid_utime, /* For OTP utime_item is the same as utime. */
  v_otp_sync_start,
  v_otp_sync_wait,
  v_otp_sync_end,
  v_otp_sync_no_wait,
  v_otp_sync_get_status,
  fs_vnode_invalid_get_device_attr,
  fs_vnode_invalid_get_fs_data,
  fs_vnode_invalid_image_prepare,
  fs_vnode_invalid_get_volume_name,
  fs_vnode_invalid_set_volume_name,
  fs_vnode_invalid_make_golden_copy,
  fs_vnode_invalid_fs_init_complete,
  fs_vnode_invalid_lock_efs,
  fs_vnode_invalid_borrow_ram,
  fs_vnode_invalid_store_data_in_sb
};

void
fs_otp_init (void)
{
  int i, result;

  memset (fs_otp_mounts, 0, sizeof (fs_otp_mounts));

  for (i = 0; i < FS_MAX_OTP_MOUNTS; ++i)
  {
    fs_otp_mounts[i].root_vnode = NULL;
    fs_otp_mounts[i].parent.ops = &fs_otp_mount_ops;
    fs_mount_register ("otp", &fs_otp_mounts[i].parent);
  }

  (void) efs_unlink ("/otp1");
  (void) efs_mkdir ("/otp1", 0644);
  result = efs_mount ("", "/otp1", "otp", 0, NULL);
  if (result != 0)
  {
    result = efs_mount ("", "/otp1", "nodev", 0, NULL);
    ASSERT (result == 0);
  }

}

#endif /* FEATURE_EFS_OTP */

