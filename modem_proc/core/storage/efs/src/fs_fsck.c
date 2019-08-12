/***********************************************************************
 * fs_fsck.c
 *
 * File system sanity check code.
 * Copyright (C) 2006-2008,2010,2012 QUALCOMM Technologies, Inc.
 *
 * Code for checking sanity of the filesystem.  Generally used in
 * off-target tests.
 *
 * This test makes use of dynamic memory allocation.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_fsck.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-12-27   mj    Remove function calls with ASSERT macro.
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2010-11-03   wek   Make DB page size independent.
2008-11-03   rp    Introduce device abstraction.
2007-10-18   rp    Featurized QEFS2.
2006-09-20   dlb   Lint cleanup.
2006-04-13   dlb   Create

===========================================================================*/

#include "fs_config_i.h"


#ifdef FS_UNIT_TEST

#ifdef FEATURE_EFS_QEFS2

/**********************************************************************
 * The filesystem check has numerous stages, not all of which will be
 * initially implemented.
 *
 *   - Verify database integrity (not implemented)
 *   - Verify tree heirarchy (only scans root).
 *   - Verify directory names (part of tree heirarchy).
 *   - Verify inodes (not implemented).
 *   - Verify allocation (not implemented).
 */

#include <stdio.h>
#include "assert.h"
#include "fs_buffer.h"
#include "fs_efs2.h"
#include "fs_db.h"
#include "pdump.h"

/* This structure holds state used by the filesystem checker. */
struct fsck_state {
  fsb_t buf;
  fs_cluster_id info_cluster;

  struct efs_info_data   info;
  fs_db_t                db;

  uint8 search_key[1 + sizeof (fs_inode_t) + FS_NAME_MAX + 1];
  uint8 found_key[1 + sizeof (fs_inode_t) + FS_NAME_MAX + 1];

  uint8 value[1 + FS_PATH_MAX];
};

static struct fsck_state fsck;

static void fs_fsck_tree (fs_inode_t dir_inum);

/* Perform a filesystem check of the filesystem specified. */
void
fs_fsck (struct fs_mount_efs *mp)
{
  void *tmp;

  /* Read in the info block. */
  fsck.buf = mp->buf;
  fsck.db = mp->db;
  fsck.info_cluster = fsck.buf->get_field (fsck.buf, FS_FIELD_FS_INFO);

  tmp = fsck.buf->read_wire (fsck.buf, fsck.info_cluster);
  fsck.info = *((struct efs_info_data *) tmp);
  fsck.buf->free_wire (fsck.buf, fsck.info_cluster);

  fs_fsck_tree (fsck.info.root);
}

/* Check the integrity of the filesystem tree, starting at the specified
 * inode number. */
static void
fs_fsck_tree (fs_inode_t dir_inum)
{
  unsigned state;
  unsigned tmp, i;
  unsigned keylen;
  fs_db_iter_t iter;
  unsigned found_len, value_len;
  int result, db_iter_val;
  // fsb_t buf = fsck.buf;

  printf ("fsck tree: 0x%x\n", dir_inum);

  /* Build the search key appropriate for this inode. */
  fsck.search_key[0] = FS_KEY_DIR;
  tmp = dir_inum;
  for (i = 0; i < sizeof (fs_inode_t); i++) {
    fsck.search_key[i+1] = tmp & 0xFF;
    tmp >>= 8;
  }
  keylen = 1 + i;

  iter = fs_db_iter_start (fsck.db, fsck.search_key, keylen);
  ASSERT (iter != NULL);

  state = 0;
  while (1) {
    found_len = sizeof (fsck.found_key);
    result = fs_db_iter_get_key (iter, fsck.found_key, &found_len);
    if (result != 0)
      break;

    /* Also break out if the key indicates a different directory. */
    if (memcmp (fsck.search_key, fsck.found_key,
          1 + sizeof (fs_inode_t)) != 0)
      break;

    printf ("key:\n");
    pdump (fsck.found_key, found_len);

    value_len = sizeof (fsck.value);
    db_iter_val = fs_db_iter_get_value (iter, fsck.value, &value_len);
    ASSERT (db_iter_val == 0);

    printf ("value:\n");
    pdump (fsck.value, value_len);

    printf ("---\n");

    switch (state) {
      case 0:
        /* Check for "." entry. */
        ASSERT (keylen == found_len);
        state++;
        break;

      case 1:
        /* Check for ".." entry. */
        ASSERT (found_len == keylen + 1);
        ASSERT (fsck.found_key[found_len - 1] == '\000');
        state++;
        break;

      default:
        break;
    }

    (void) fs_db_iter_next (iter);
  }

  /* Make sure we had "." and ".." in this directory. */
  ASSERT (state >= 2);
}

#endif /* FEATURE_EFS_QEFS2 */

#endif /* FS_UNIT_TEST */
