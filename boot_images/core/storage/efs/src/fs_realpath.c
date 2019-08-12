/**********************************************************************
 * Pathname canonicalization.
 *
 * Copyright (C) 2004,2007-2013 QUALCOMM Technologies, Inc.
 */

/*======================================================================
 *
 *                             EDIT HISTORY
 *
 * $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_realpath.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who   what, where, why
 * ----------   ----  ----------------------------------------------------
 * 2013-10-09   nr    Replace banned memory functions with safer versions.
 * 2012-01-17   wek   Remove unused utils header file.
 * 2011-11-02   wek   Use library supplied strl cpy and cat instead of fs_strl*
 * 2010-02-06   yog   Validate the input param for NULL check.
 * 2009-10-09   wek   Added define for symlink resolve limit.
 * 2009-04-15   ebb   Removed from EFS protected segment.
 * 2008-09-12   ebb   Fixed bounds argument in strlcpy to avoid overrun.
 * 2008-04-28   rp    Fix klocwork warnings.
 * 2007-10-01   rp    Added efs_normalize_path_string().
 * 2007-08-19   rp    Include fs_global_lock.h due to FS_GLOBAL_LOCK() cleanup.
 * 2007-05-25   umr   Added support for EFS memory protection feature.
 * 2007-01-12   dlb   Use safer string functions.
 * 2004-03-19   dlb   Created.
 *======================================================================*/

#include <stdio.h>
#include <string.h>
#include "stringl/stringl.h"

#include "fs_errno.h"
#include "fs_public.h"
#include "fs_efs2.h"
#include "fs_err.h"
#include "fs_util.h"
#include "fs_stdlib.h"
#include "fs_global_lock.h"

/* Posix(tm) realpath interface. */
char *
efs_realpath (const char *path, char *resolved_path)
{
  /* Null check for input param. */
  if (path == NULL || resolved_path == NULL)
  {
    efs_errno = EINVAL;
    return NULL;
  }

  return efs_normalize_path (path, resolved_path, FS_PATH_MAX, TRUE);
}

/* Local version. */
char *
efs_normalize_path (const char *path, char *resolved_path,
    int rpath_length, int resolve_last_link)
{
  int plen;
  char *pbase;
  char *pend;
  char *tmp;
  int rlen = 0;
  int old_rlen;
  int result;
  int link_count = 0;

  /* Static buffers (we lock to protect these). */
  /* Holds the name as we build it up.  The name is right justified, so the
   * trailing NUL will always be the last character. */
  static char todo_buf[FS_PATH_MAX+1];

  /* Destination of 'readlink'. */
  static char rl_buf[FS_PATH_MAX];

  /* Stat buf. */
  static struct fs_stat stat_buf;

  if (path == NULL || resolved_path == NULL) {
    efs_errno = EINVAL;
    return NULL;
  }

  /* An empty string for a path is invalid. */
  if (*path == 0) {
    efs_errno = ENOENT;
    return NULL;
  }

  plen = strlen (path);
  if (plen > FS_PATH_MAX) {
    efs_errno = ENAMETOOLONG;
    return NULL;
  }

  FS_GLOBAL_LOCK ();

  /* Copy the given path into the working buffer. */
  pbase = todo_buf + (FS_PATH_MAX - plen);
  strlcpy (pbase, path, plen + 1);

  /* XXX: Once chdir is implemented, begin by putting the cwd in the
   * buffer, without a trailing slash.  For now, we assume the cwd is the
   * root. */

  /* Walk through the working buffer, moving pieces in as necessary. */
  while (*pbase) {

    /* Skip over any slashes. */
    while (*pbase == '/')
      pbase++;

    /* If the name ended in slashes, finish. */
    if (*pbase == 0)
      break;

    /* Scan for the end of this name component. */
    for (pend = pbase; *pend != 0 && *pend != '/'; pend++)
      ;

    /* Check for excessive length. */
    if (rlen + (pend - pbase) + 1 > rpath_length) {
      efs_errno = ENAMETOOLONG;
      FS_GLOBAL_UNLOCK ();
      return NULL;
    }

    /* Copy the name component over. */
    old_rlen = rlen;
    resolved_path[rlen++] = '/';
    for (tmp = pbase; tmp < pend; tmp++)
      resolved_path[rlen++] = *tmp;
    resolved_path[rlen] = 0;

    /*******************
     * Resolve this partial component of the path.
     */
    result = efs_lstat (resolved_path, &stat_buf);
    if (result != 0) {
      FS_GLOBAL_UNLOCK ();
      return NULL;
    }

    if (pend - pbase == 1 && *pbase == '.') {
      /* Dot should just be removed from the path entirely. */
      rlen = old_rlen;
      resolved_path[rlen] = 0;
    } else if (pend - pbase == 2 && pbase[0] == '.' && pbase[1] == '.') {
      /* Remove components of the path if they are present. */
      rlen = old_rlen - 1;
      while (rlen > 0 && resolved_path[rlen] != '/')
        rlen--;
      if (rlen < 0)
        rlen = 0;
      resolved_path[rlen] = 0;
    } else if (S_ISREG (stat_buf.st_mode)) {
      /* Encountering a regular file must only happen at the end of the
       * path. */
      if (*pend != 0) {
        efs_errno = ENOTDIR;
        FS_GLOBAL_UNLOCK ();
        return NULL;
      }
    } else if (S_ISDIR (stat_buf.st_mode)) {
      /* Dirs are fine anywhere. */
    } else if (S_ISLNK (stat_buf.st_mode)) {
      if (resolve_last_link || *pend != 0) {
        link_count++;
        if (link_count > FS_SYMLINK_RESOLVE_LIMIT) {
          efs_errno = ELOOP;
          FS_GLOBAL_UNLOCK ();
          return NULL;
        }

        /* Resolve the symlink. */
        result = efs_readlink (resolved_path, rl_buf, FS_PATH_MAX);
        if (result <= 0) {
          FS_GLOBAL_UNLOCK ();
          return NULL;
        }
        if (result >= FS_PATH_MAX) {
          efs_errno = ENAMETOOLONG;
          FS_GLOBAL_UNLOCK ();
          return NULL;
        }

        /* Make sure there is enough room for the symlink. */
        if (pend - todo_buf <= result) {
          efs_errno = ENAMETOOLONG;
          FS_GLOBAL_UNLOCK ();
          return NULL;
        }

        /* Replace the string in the work buf. */
        fs_memscpy (pend - result, result, rl_buf, result);
        pend -= result;

        /* If the link is absolute, undo the entire path so far, otherwise
         * just remove the link. */
        if (rl_buf[0] == '/')
          rlen = 0;
        else
          rlen = old_rlen;
        resolved_path[rlen] = 0;
      }
    } else {
      /* Unknown file type. */
      efs_errno = ENOENT;  /* ??? */
      FS_GLOBAL_UNLOCK ();
      return NULL;
    }

    pbase = pend;
  }

  /* Special case.  If the path resolves to the root directory, insert the
   * slash. */
  if (rlen == 0) {
    resolved_path[0] = '/';
    resolved_path[1] = 0;
  }

  FS_GLOBAL_UNLOCK ();
  return resolved_path;
}


char *
efs_normalize_path_string (const char *path, char *resolved_path,
   int rpath_length)
{
  const char *pbase;
  const char *pend;
  const char *tmp;
  int plen;
  int rlen = 0;
  int old_rlen;

  if (path == NULL || resolved_path == NULL) {
    efs_errno = EINVAL;
    return NULL;
  }

  /* An empty string for a path is invalid. */
  if (*path == 0) {
    efs_errno = ENOENT;
    return NULL;
  }

  plen = strlen (path);
  if (plen > FS_PATH_MAX) {
    efs_errno = ENAMETOOLONG;
    return NULL;
  }

  /* start the loop with the first character of user-supplied path. */
  pbase = path;

  /* Walk through the working buffer, moving pieces in as necessary. */
  while (*pbase) {

    /* Skip over any slashes. */
    while (*pbase == '/')
      pbase++;

    /* If the name ended in slashes, finish. */
    if (*pbase == 0)
      break;

    /* Scan for the end of this name component. */
    for (pend = pbase; *pend != 0 && *pend != '/'; pend++)
      ;

    /* Check for excessive length. */
    if (rlen + (pend - pbase) + 1 > rpath_length) {
      efs_errno = ENAMETOOLONG;
      return NULL;
    }

    /* Copy the name component over. */
    old_rlen = rlen;
    resolved_path[rlen++] = '/';

    /* Special case: If the user has not started the path with a '/' then
     * don't put a '/', we may go past 1-byte beyond his supplied buffer. */
    if ((pbase == path) && (*pbase != '/'))
      rlen = 0;

    for (tmp = pbase; tmp < pend; tmp++)
      resolved_path[rlen++] = *tmp;
    resolved_path[rlen] = 0;


    if (pend - pbase == 1 && *pbase == '.') {
      /* Dot should just be removed from the path entirely. */
      rlen = old_rlen;
      resolved_path[rlen] = 0;
    } else if (pend - pbase == 2 && pbase[0] == '.' && pbase[1] == '.') {
      /* Remove components of the path if they are present. */
      rlen = old_rlen - 1;
      while (rlen > 0 && resolved_path[rlen] != '/')
        rlen--;
      if (rlen < 0)
        rlen = 0;
      resolved_path[rlen] = 0;
    }

    pbase = pend;
  }

  /* Special case.  If the path resolves to the root directory, insert the
   * slash. */
  if (rlen == 0) {
    resolved_path[0] = '/';
    resolved_path[1] = 0;
  }

  return resolved_path;
}

