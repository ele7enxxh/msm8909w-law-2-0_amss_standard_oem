/***********************************************************************
 * fs_efs2_names.c
 *
 * Short description.
 * Copyright (C) 2006--2007, 2012-2014 QUALCOMM Technologies, Inc.
 *
 * Verbose Description
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_efs2_names.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-01-28   nr    Fix klocwork issues.
2013-10-09   nr    Replace banned memory functions with safer versions.
2013-07-17   wek   Move MD5 stubs into util header file.
2012-09-28   wek   Implement write and truncate for item files.
2012-09-12   wek   Fix error in setting the directory size for item files.
2012-03-13   wek   Remove redundant header file inclusion fs_efs2_cust.
2007-10-18   rp    Featurized QEFS2.
2007-05-25   umr   Added support for EFS memory protection feature.
2006-12-13   sh    Lint fixes.
2006-11-30   sh    Never claim to have d_stats_present in EFS2.
2006-09-20   dlb   Lint cleanup.
2006-07-13   dlb   Fix longname as last entry in last dir.
2006-06-27   dlb   Pass stat through readdir.
2006-06-14   sh    Fix two-character filenames starting with "."
2006-05-31   dlb   Fix problem with multiple iterators.
2006-05-05   dlb   Fix off-by-one on full-length names.
2006-04-13   dlb   Create

===========================================================================*/

#include "fs_config_i.h"

#ifdef FEATURE_EFS_QEFS2

#include <string.h>

#include "assert.h"
#include "fs_err.h"
#include "fs_sys_types.h"
#include "fs_efs2_names.h"
#include "fs_errno.h"
#include "fs_protected_memory.h"
#include "fs_util.h"

/*****************************************************************
 * Introduction.
 *
 * Unlike most traditional Posix filesystems (but like many modern
 * filesystems), EFS2 directories do not themselves contain a list of
 * filename mappings.  Instead EFS2 maintains a single B+tree which
 * maintains the contents of all directories in the filesystem.
 *
 * The B+tree is a simple dictionary abstraction, mapping arbitrary keys
 * (strings of bytes) to arbitrary values.  However, because of the small
 * page sizes of some devices supported by EFS2, the lengths of these keys
 * and values are somewhat limited (defined elsewhere).  Filenames that are
 * short enough are directly used to construct the key.  The key for longer
 * names is based on part of the name and a hash of the entire name.  This
 * entry will be followed by additional database entities giving the rest
 * of the filename.
 *
 *****************************************************************
 * Key formats.
 *
 * At this point, the only key entries in the database indicate the
 * contents of directories.  Each directory in the filesystem is given a
 * unique number of type fs_inode_t.  The inode number can be used to
 * lookup additional information on the directory (size, attributes, and
 * such).
 *
 * As far as the database is concerned, there is nothing special about the
 * root directory.  The info block contains the inode number of this
 * directory so that it can be used as a starting point by name lookup.
 *
 * Because the B+tree does iterator lookups in a lexigraphically sorted
 * order, we can take advantage of that to properly construct keys that
 * sort conveniently.  The first requirement is that all of the entries for
 * the files in a given directory are sorted together.  This allows readdir
 * to simply walk through a set of consecutive B+tree entities.  The second
 * is that we want longname entities to immediately follow their key entry
 * so that readdir can reassemble the long filename.
 *
 * All directory entry keys are prefixed by FS_KEY_DIR, followed by a
 * little-endian encoding of the inode number of the specific directory.
 *
 * If the name is less than the FS_SHORTNAME_THRESHOLD, then the name
 * itself occupies the remainder of the filename.  The short filename never
 * contains a '\000' (nul) character.  The database keeps a length field so
 * termination would be redundant, plus we take advantage of it for long
 * names.
 *
 * Short names:
 *   'd' inum[4] filename
 *
 * There are two special cases for the "." and ".." entries, needed to make
 * sure these entities sort first.  The "." entity is simply represented by
 * a key containing no filename at all, and the ".." entity is represented
 * by a filename consisting of a single '\000' character.
 *
 * When the name is greater or equal to the FS_SHORTNAME_THRESHOLD, then
 * the inum[4] field is first followed by the first FS_SHORTNAME_PREFIX_LEN
 * characters of the full filename, and then by the 16-byte MD5 hash of the
 * full filename, and finally by a trailing '\000' character.
 *
 *   'd' inum[4] filename-prefix MD5-hash '\000'
 *
 * The '\000' at the end serves two purposes.  First, it distinguishes the
 * long filename entity from a shorter filename.  Second, the subsequent
 * entries containing the full long filename will increment this field.
 *
 * The B+tree value for this longname entry ending in '\000' will be the
 * ordinary value used by the filesystem.
 *
 * Following this entry, are successive enties with the above key changed
 * only by incrementing the '\000' at the end.  These values are of type
 * FS_VAL_LONGNAME, and contain the chunks of the long filename.
*/

/**********************************************************************
 * EFS2 is currently single threaded (with a global lock ouside of all
 * calls), so we'll just use a few scratch buffers for keys.
 */

static uint8 static_name_key[1 + sizeof (fs_inode_t) + FS_NAME_MAX + 1];
static uint8 static_name_value[FS_MAX_DB_VALUE + 1];

#define INVALID_LENGTH 0xFFFFFFFF

static unsigned static_name_key_length = INVALID_LENGTH;
static unsigned static_name_value_length = INVALID_LENGTH;

/* Buffers for computing MD5 hashes. */
static MD5_CTX hash_context;
static unsigned char hash_result[16];

/* Construct a single key into static_name_key, returning the length of the
 * new key.  'is_long' will be set to true if this filename needs longname
 * support.  'is_long' can be null if the information is not needed. */
static unsigned
construct_name_key (fs_inode_t dir_inum, const char *name, unsigned len,
    unsigned *is_long)
{
  unsigned i, j;
  unsigned pos;
  static const char null_name[] = "";

  if (len == 1 && name[0] == '.')
    len = 0;
  else if (len == 2 && name[0] == '.' && name[1] == '.') {
    /* Replace the name with the null that we have sitting around. */
    name = null_name;
    len = 1;
  }

  ASSERT (len <= FS_NAME_MAX);

  pos = 0;
  static_name_key[pos++] = FS_KEY_DIR;

  /* Encode the directory inode number, little-endian into the key. */
  for (i = 0; i < sizeof (fs_inode_t); i++) {
    static_name_key[pos++] = dir_inum & 0xFF;
    dir_inum >>= 8;
  }
  ASSERT (pos == sizeof (fs_inode_t) + 1);

  if (len > FS_SHORTNAME_THRESHOLD) {
    /* These ASSERTS are needed to convince Lint that we're okay */
    ASSERT (name != null_name);
    ASSERT (pos + FS_SHORTNAME_PREFIX_LEN < sizeof(static_name_key));

    /* Copy over the prefix. */
    for (j = 0; j < FS_SHORTNAME_PREFIX_LEN; j++) {
      static_name_key[pos++] = name[j];
    }

    MD5Init (&hash_context);
    MD5Update (&hash_context, (void *) name, len);
    MD5Final (hash_result, &hash_context);
    FS_ASSERT (pos + 16 < sizeof(static_name_key));
    fs_memscpy (static_name_key + pos, sizeof(static_name_key) - pos,
                hash_result, 16);
    pos += 16;
    static_name_key[pos++] = '\000';

    if (is_long != NULL)
      *is_long = 1;
  } else {
    /* Store the entire filename. */
    ASSERT ((name != null_name) || (len == 1));
    fs_memscpy(static_name_key + pos,sizeof(static_name_key) - pos, name, len);
    pos += len;

    if (is_long != NULL)
      *is_long = 0;
  }

  return pos;
}

int
fs_efs2_name_key_add (
    fs_db_t db,
    const char *key,
    unsigned key_len,
    const void *value,
    unsigned value_len,
    fs_gid_t gid,
    int allow_overwrite,
    int *entry_added)
{
  int result = -ENOENT;
  int added = 0;

  ASSERT (value_len <= FS_MAX_DB_VALUE);

  /* Result keeps the -ENOENT value unless we are successful at adding the
   * entry, or get an error. */

  /* If we are allowed to overwrite an entry, try doing so.  If there
   * wasn't something to overwrite, or we're not allowed to, indicate there
   * was no item and we need to add one. */
  if (allow_overwrite) {
    if (fs_db_update (db, key, key_len, value, value_len, gid, FS_CURR_UID) ==
        FS_DB_TRUE)
      result = 0;
  }

  if (result == -ENOENT) {
    if (fs_db_add (db, key, key_len, value, value_len, gid, FS_CURR_UID) ==
        FS_DB_TRUE) {
      added = 1;
      result = 0;
    }
    else {
      result = -ENOSPC;
    }
  }

  if (entry_added != NULL)
    *entry_added = added;

  return result;
}

int
fs_efs2_name_add (
    fs_db_t db,
    const char *name,
    unsigned name_len,
    fs_inode_t parent_dir,
    const void *value,
    unsigned value_len,
    fs_gid_t gid,
    int allow_overwrite,
    int *entry_added)
{
  unsigned len;
  unsigned is_long;
  unsigned name_offset, count;
  int result;

  len = construct_name_key (parent_dir, name, name_len, &is_long);

  result = fs_efs2_name_key_add (db, (char *)static_name_key, len, value,
                               value_len, gid, allow_overwrite, entry_added);

  /* If it is a long file name, add the rest of the file name entries to
   * the database. */
  if (result == 0 && is_long) {
    /* Create the entries for long filenames.  Add entries for
     * FS_LONGNAME_PIECE_SIZE chunks of the name. */
    for (name_offset = 0;
        result == 0 && name_offset < name_len;
        name_offset += FS_LONGNAME_PIECE_SIZE)
    {
      static_name_value[0] = FS_VAL_LONGNAME;
      count = name_len - name_offset;
      if (count > FS_LONGNAME_PIECE_SIZE)
        count = FS_LONGNAME_PIECE_SIZE;
      fs_memscpy (static_name_value + 1, sizeof (static_name_value) - 1,
                  name + name_offset, count);

      /* Bump the 'count' field of the key. */
      static_name_key[len - 1]++;

      if (fs_db_add (db, static_name_key, len,
            static_name_value, 1 + count, gid, FS_CURR_UID) != FS_DB_TRUE)
        result = -ENOSPC;
    }
  }

  return result;
}

int fs_efs2_name_remove (
    fs_db_t db,
    const char *name,
    unsigned name_len,
    fs_inode_t parent_dir)
{
  unsigned len;
  unsigned is_long;
  int result;

  len = construct_name_key (parent_dir, name, name_len, &is_long);

  result = fs_db_delete (db, static_name_key, len) == FS_DB_TRUE ? 0 : -ENOENT;

  if (result == 0 && is_long) {
    /* For a long filename, delete entries, until we get one that doesn't
     * exist. */

    while (1) {
      static_name_key[len - 1]++;
      if (fs_db_delete (db, static_name_key, len) != FS_DB_TRUE)
        break;
    }
  }

  return result;
}

int fs_efs2_name_lookup (
    fs_db_t db,
    const char *name,
    unsigned name_len,
    fs_inode_t parent_dir)
{
  int result;

  unsigned len;
  unsigned rec_len = sizeof (static_name_value);

  len = construct_name_key (parent_dir, name, name_len, NULL);
  static_name_key_length = len;
  result = fs_db_lookup (db, static_name_key, len,
      static_name_value, &rec_len) == FS_DB_TRUE ? 0 : -ENOENT;

  if (result == 0) {
    static_name_value_length = rec_len;
  } else {
    static_name_value_length = INVALID_LENGTH;
  }

  return result;
}

void fs_efs2_name_last_key (const uint8 **key, unsigned *key_len)
{
  *key = static_name_key;
  *key_len = static_name_key_length;
}

void fs_efs2_name_last_value (const uint8 **value, unsigned *value_len)
{
  ASSERT (static_name_value_length != INVALID_LENGTH);

  *value = static_name_value;
  *value_len = static_name_value_length;
}

int
fs_efs2_name_key_lookup (fs_db_t db, const uint8 *key, unsigned key_len)
{
  int result;
  unsigned rec_len = sizeof (static_name_value);

  result = fs_db_lookup (db, key, key_len,
      static_name_value, &rec_len) == FS_DB_TRUE ? 0 : -ENOENT;

  if (result == 0) {
    static_name_value_length = rec_len;
  } else {
    static_name_value_length = INVALID_LENGTH;
  }

  return result;
}

/* Does this key represent the same directory as the specified one? */
static int
same_key_dir (uint8 *key, unsigned len, fs_inode_t inum)
{
  fs_inode_t tmp;
  unsigned i;

  if (len < (1 + sizeof (fs_inode_t)))
    return 0;

  if (key[0] != FS_KEY_DIR)
    return 0;

  /* Extract the little-endian encoded inode number from the passed in key. */
  tmp = 0;
  for (i = sizeof (fs_inode_t); i > 0; i--) {
    tmp <<= 8;
    tmp |= (uint8) key[i];
  }

  return tmp == inum;
}

/* Extract the name from a shortname key.  If there is room, the name will
 * be placed in 'name', and 'name_len' set to the new length.  Otherwise,
 * result will be -1. */
static int
extract_key_name (uint8 *key, unsigned len, char *name, unsigned *name_len)
{
  unsigned new_len;
  const char *namep = (const char *) key + (1 + sizeof (fs_inode_t));

  new_len = len - (1 + sizeof (fs_inode_t));

  if (new_len == 0) {
    namep = ".";
    new_len = 1;
  } else if (new_len == 1 && namep[0] == '\000') {
    namep = "..";
    new_len = 2;
  }

  if (new_len <= *name_len) {
    fs_memscpy (name, *name_len, namep, new_len);
    *name_len = new_len;
    return 0;
  } else
    return -1;
}

void *
fs_efs2_iter_begin (
    fs_db_t db,
    fs_inode_t dir_inum)
{
  unsigned len;

  len = construct_name_key (dir_inum, ".", 1, NULL);

  return fs_db_iter_start (db, static_name_key, len);
}

/* Read the next directory entry.  The name is filled in (if possible), and
 * the iterator is advanced.  Returns 0 on success, -EEOF if there are no
 * more, or another negative error code to indicate a failure.  The
 * resulting name will be null terminated. */
int
fs_efs2_iter_next (void *iter,
    fs_inode_t dir_inum,
    struct fs_dirent *dirent)
{
  int result;
  unsigned len;
  unsigned name_len;
  unsigned is_long = 0;
  unsigned value_len;
  unsigned seqno, count;

  /* Ensure that we never claim to have stats present in results from EFS2. */
  dirent->d_stats_present = 0;

  len = sizeof (static_name_key);
  result = fs_db_iter_get_key ((fs_db_iter_t) iter,
      static_name_key, &len);
  if (result != 0)
    return result;

  if (!same_key_dir (static_name_key, len, dir_inum))
    result = -EEOF;

  ASSERT (result != 0 || len > 1);

  if (result == 0 &&
      len > 1 + sizeof (fs_inode_t) + 2 &&
      static_name_key[len - 1] == '\000')
    is_long = 1;

  if (result == 0 && !is_long) {
    name_len = FS_NAME_MAX;
    result = extract_key_name (static_name_key, len, dirent->d_name,
        &name_len);
    if (result == 0)
      dirent->d_name[name_len] = 0;

    (void) fs_db_iter_next ((fs_db_iter_t) iter);
    return result;
  }

  if (result == 0 && is_long) {
    seqno = 1;
    name_len = 0;

    /* Walk through the next database entries, and append each piece of the
     * filename. */
    while (1) {
      (void) fs_db_iter_next ((fs_db_iter_t) iter);

      /* Exit either when we've gone to a different directory, fallen off
       * of the end, or the sequence number doesn't match. */
      result = fs_db_iter_get_key ((fs_db_iter_t) iter,
          static_name_key, &len);
      if (result != 0) {
        /* Ran off, reset error code, and break out of loop. */
        result = 0;
        break;
      }

      /* If we hit a key in a different directory, break out of the loop as
       * well. */
      if (!same_key_dir (static_name_key, len, dir_inum) ||
          static_name_key[len - 1] != seqno)
        break;
      value_len = sizeof (static_name_value);
      result = fs_db_iter_get_value ((fs_db_iter_t) iter,
          static_name_value, &value_len);
      ASSERT (result == 0);
      ASSERT (static_name_value[0] == FS_VAL_LONGNAME);
      count = value_len - 1;
      if (name_len + count > FS_NAME_MAX)
        count = FS_NAME_MAX - name_len;
      ASSERT (count <= sizeof (static_name_value) - 1);
      fs_memscpy (dirent->d_name + name_len, sizeof(dirent->d_name) - name_len,
                  static_name_value + 1, count);
      name_len += count;
      seqno++;
    }
    ASSERT (name_len < sizeof (dirent->d_name));
    if (name_len >= sizeof (dirent->d_name))
    {
      name_len = sizeof (dirent->d_name) - 1;
    }
    dirent->d_name[name_len] = '\000';
  }

  return result;
}

/* Close the directory iterator.  Returns 0 for success. */
int
fs_efs2_iter_close (void *iter)
{
  fs_db_iter_free ((fs_db_iter_t) iter);
  return 0;
}

#endif /* FEATURE_EFS_QEFS2 */
