/**********************************************************************
 * Filesystem standard library.
 *
 * Copyright (C) 2004,2007-2008 Qualcomm Technologies, Inc.
 */

/*======================================================================
 *
 *                             EDIT HISTORY
 *
 * $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/storage/fs_stdlib.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
 *
 * when        who   what, where why
 * ----------  ---   -----------------------------------------------------
 * 2010/07/12  rp    Added defines for major and minor number for API files.
 * 2008/05/23  rp    Introducing /inc folder as per new SCM.
 * 2007/10/01  rp    Added efs_normalize_path_string().
 * 2004/03/29  dlb   Created.
 *======================================================================*/

#ifndef __FS_STDLIB_H__
#define __FS_STDLIB_H__

/** Identifies the major version number of this API file. */
#define VER_FS_STDLIB_H_MAJOR     1
/** Identifies the minor version number of this API file. */
#define VER_FS_STDLIB_H_MINOR     1

/** Expand symlinks and resolve path to an absolute path.  'resolved_path'
 * must have at least FS_PATH_MAX bytes available.  Returns a pointer to
 * resolved_path, if there is no error, otherwise returns a NULL pointer
 * and sets efs_errno to the error value. */
char *efs_realpath (const char *path, char *resolved_path);

/** Similar to above, but allows the maximum buffer to be specified.
 * Setting 'resolve_last_link' to TRUE will resolve all links.  False will
 * cause the last component to not be resolved, if it is a symlink.  Return
 * result is the same as efs_realpath. */
char *efs_normalize_path (const char *path, char *resolved_path,
    int rpath_length, int resolve_last_link);

/** Similar to efs_normalize_path(), but this is a light version, it will not
 * stat filepath, will not acquire global-lock, treats the file-path as a
 * string and gets rid of '.' and takes care of '..' in the path. Return
 * result is the same as efs_realpath.
 * Note: The intermediary directory-names, symlinks etc are not checked
 * for existence. */
char *efs_normalize_path_string (const char *path, char *resolved_path,
    int rpath_length);

#endif /* not __FS_STDLIB_H__ */
