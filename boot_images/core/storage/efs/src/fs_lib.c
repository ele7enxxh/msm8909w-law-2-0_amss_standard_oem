/***********************************************************************
 * fs_lib.c
 *
 * Public interface for EFS2
 * Copyright (C) 2002-2014 QUALCOMM Technologies, Inc.
 *
 * This file contains Library API functions which EFS2
 * can utilize.
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_lib.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2014-06-12    wek     Bringup of hotplug for XBL.
2014-04-26    vm      Use correct FS_MSG macros to save RAM.
2014-03-28    nr      Klocwork warning fixes in Modem code.
2013-10-09    nr      Replace banned memory functions with safer versions.
2013-07-17    wek     Move MD5 stubs into util header file.
2013-05-17    wek     Fix bug in extensive profiling logging wrong errno.
2013-03-28    nr      Ensure fixed log column count for Extensive profiling.
2013-03-26    wek     Rename macros from MSG_ to FS_MSG for efs code.
2013-02-22    wek     Fix efs put failing when temp$ file is present.
2012-12-26    nr      Implement support of Extensive profiling.
2012-11-28    nr      Implement EFS Profile framework.
2012-10-23    wek     Fix the oflags when calling put on temporary item file.
2012-10-22    wek     Move efs_raw_put/get functions from public to lib.
2012-10-18    wek     Make efs_raw_put/get wrappers around efs_put/get.
2012-08-30    wek     Allow zero size item files.
2012-04-12    nr      Fix bugs in efs_get and efs_put.
2012-03-20    nr      Fix bug in auto_mkdir.
2012-03-08    wek     Rename macros and remove header files to compile for boot
2012-01-17    wek     Remove unused utils header file.
2011-12-21    nr      Fix mode in efs_put for files larger than item file.
2011-11-02    wek     Use library supplied strl cpy and cat instead of fs_strl*
2011-11-02    nr      Klocwork error fixes in Modem code.
2011-07-01    wek     Create a TAR file from a given directory.
2011-01-12    vr      Support EFS removal based on FEATURE_EFS2.
2010-10-29    wek     Make lib independent of page size macro.
2010-10-28    rp      Avoid possible infinite loop in efs_auto_mkdir()
2010-10-10    wek     Protect temporary global buffer for concurrent access.
2010-08-10    dks     Made efs_iter non-static for access from fs_benchamrk.
2010-02-06    yog     Validate the input param for NULL check.
2010-03-05    vr      Add support for QEFS2 in BOOT
2009-09-20    sh      Remove unsafe Babylon messages for put/get
2009-05-15    sh      Corrected types for MD5 functions.
2009-01-27    sh      Added PUT/GET babylon messages to track NV use
2008-12-12    rp      In efs_iter, handle efs_mkdir failures.
2008-12-11    rp      Fix bug in efs_iter() to iterate the entire "/" dir.
2008-12-05    rp      Allow efs_iter to iterate the entire "/" dir in simulator
2008-10-17    yog     Included err.h for usgae of macro ERR_FATAL.
2008-03-31    yg      Fix klocwork error.
2008-02-22    np      Added md5sum calculation function.
2007-11-16    rp      Added efs_chown_recursive() and efs_chmod_recursive().
2007-08-19    rp      Include fs_global_lock.h due to FS_GLOBAL_LOCK() cleanup.
2007-01-12    dlb     Use safer string functions.
2006-12-05    dlb     Max transaction size configurable.
2006-11-02     rp     Moved fs_path_buffer[] to fs_patbuff.c
2006-10-17    umr     Added FS_PATH_BUFFER_LOCK around fs_path_buffer
2006-09-25    umr     Added efs_deltree() for deleting a whole tree.
2006-09-20    dlb     Lint fixes
2006-06-19    nrs     Allow renames on different size items
2006-03-30    sh      Lint fixes
2005-11-17    nrs     Fixed bug in efs_get, efs_errno
2005-10-28    nrs     Changed length fields from int to fs_size_t
2004-12-16    nrs     Fixed possible cases where Global Unlock may not occur
2004-11-01    nrs     Removed excessive string copies from auto create
2004-11-01    nrs     Changed l_temp_path from local stack to global variable.
2004-10-28    nrs     Add functionality to autocreate directories on efs_put ()
2004-10-08    nrs     Added rename in efs_put () for robustness.
2004-10-08    dlb     Cleanup indenting.
2004-09-30    nrs     Created.

===========================================================================*/

#include "fs_errno.h"
#include "fs_lib.h"
#include "fs_pathbuff.h"
#include "fs_public.h"
#include "fs_efs2.h"
#include "assert.h"
#include "fs_global_lock.h"
#include "fs_privatedir.h"
#include "fs_err.h"
#include <stdio.h>
#include "fs_babylon.h"
#include "fs_priv_funcs.h"
#include "fs_stdlib.h"
#include "fs_profile.h"
#include "stringl/stringl.h"
#include "fs_util.h"

#ifdef FEATURE_EFS2

/* This structure is used to store the User-Id and Group-Id to help
   efs_chown_recursive() to use them later in its callback function. */
struct chown_recursive_callback_param
{
  int uid_val;
  int gid_val;
};

static int efs_put_core (const char *path, void *data, fs_size_t length,
                        int oflag, int mode);

int
efs_raw_put (const char *path, void *data, fs_size_t length,
    int oflag, int mode)
{
  return efs_put (path, data, length, oflag, mode);
}

/**********************************************************************
 * FUNCTION      efs_put
 *
 * Store a value in a special item file.
 */
int
efs_put (const char *path, void *data, fs_size_t length, int oflag, int mode)
{
  int result = 0;
  struct fs_profile_handle profile_handle;

  fs_global_lock ();
  fs_profile_open (&profile_handle, FS_PROFILE_ID_PUT);
  fs_profile_start (&profile_handle);
  fs_global_unlock ();

  /* check validity for the file path and Null check for data. */
  result = fs_is_path_valid (path);
  if (result != 0 || (data == NULL && length != 0))
  {
    efs_errno = EINVAL;
    result = -1;
    goto cleanup;
  }

  EFS_BAB_PRINT (EFS_BAB_PUT, path);
  EFS_BAB_VALUE (EFS_BAB_PUT, (uint32)length, TRUE, TRUE, TRUE);

  result = efs_put_core (path, data, length, oflag, mode);
  if (result < 0)
  {
    if ((efs_errno == ENOENT) && (O_AUTODIR & oflag))
    {
      result = efs_auto_mkdir (path);
      if (result == 0)
      {
        result = efs_put_core (path, data, length, oflag, mode);
      }
    }
  }

cleanup:
  fs_global_lock ();
  fs_profile_stop (&profile_handle);
  fs_profile_log_int (&profile_handle, length);
  fs_profile_log_int (&profile_handle, oflag);
  fs_profile_log_int (&profile_handle, mode);

  if (result < 0)
  {
    fs_profile_log_int (&profile_handle, efs_errno);
  }
  else
  {
    fs_profile_log_int (&profile_handle, 0);
  }

  fs_profile_log_string (&profile_handle, path);
  fs_profile_close (&profile_handle);

  fs_global_unlock ();

  EFS_BAB_RETURN (EFS_BAB_PUT, result);

  return result;
}

/* Reads the data from an item file, if this fails it will try to read the
 * data as a regular file. */
static int
efs_get_core (const char *path, void *data, fs_size_t length)
{
  int result = 0, fd = 0;
  struct fs_stat buf;
  fs_size_t total_read = 0;

  result = efs_pure_get (path, data, length);

  /* efs_pure_get is successful, kick it back */
  if (result >= 0)
    return result;

  /* efs_pure_get couldn't read the item. Return an error unless the path
   * wasn't an S_ITM type. */

  if (efs_errno != EINVAL)
  {
    return result;
  }

  /* Check to ensure that size of data buffer is equal to file size */
  if (efs_stat (path, &buf) != 0)
  {
    return -1;
  }
  /* Ensure that this is a regular file! */
  if (S_ISREG(buf.st_mode) != 1)
  {
    efs_errno = EINVAL;
    return -1;
  }
  /* Size mismatch return error */
  if (length != buf.st_size)
  {
     efs_errno = FS_ERANGE;
     return -1;
  }

  fd = efs_open (path, O_RDONLY);
  /* If the file does not exist, then the current file handle will be less
   * than zero */
  if (fd < 0)
  {
    return -1;
  }

  while (length > 0)
  {
    result = efs_read (fd, data, length);
    /* Check to see if the read status and break in case of failure. */
    if (result < 0)
    {
      break;
    }

    data = (void *) ((uint8 *) data + result);
    length -= result;
    total_read += result;
  }

  (void) efs_close (fd);
  return total_read;
}

int
efs_raw_get (const char *path, void *data, fs_size_t length)
{
  return efs_get (path, data, length);
}

/* Read contents of item. */
int
efs_get (const char *path, void *data, fs_size_t length)
{
  int result;
  struct fs_profile_handle profile_handle;

  fs_global_lock ();
  fs_profile_open (&profile_handle, FS_PROFILE_ID_GET);
  fs_profile_start (&profile_handle);
  fs_global_unlock ();

  /* check validity for the file path and Null check for data. */
  result = fs_is_path_valid (path);
  if (result != 0 || (data == NULL && length != 0))
  {
    efs_errno = EINVAL;
    result = -1;
    goto cleanup;
  }

  EFS_BAB_PRINT (EFS_BAB_GET, path);
  EFS_BAB_VALUE (EFS_BAB_GET, (uint32)length, TRUE, TRUE, TRUE);

  result = efs_get_core (path, data, length);

  EFS_BAB_RETURN (EFS_BAB_GET, result);

cleanup:
  fs_global_lock ();
  fs_profile_stop (&profile_handle);

  fs_profile_log_int (&profile_handle, length);

  if (result < 0)
  {
    fs_profile_log_int (&profile_handle, efs_errno);
  }
  else
  {
    fs_profile_log_int (&profile_handle, 0);
  }

  fs_profile_log_string (&profile_handle, path);
  fs_profile_close (&profile_handle);

  fs_global_unlock ();

  return result;
}

/**********************************************************************
 * deltree_truncate_path
 * Chops off the characters starting from the last character in the passed
 * string all the way upto the occurrence of '/' character.
 */
static int
deltree_truncate_path (char* path, word max_count)
{
  word slen = 0;
  int status = -1, i;

  slen = strlen (path);

  if (slen > FS_PATH_MAX)
  {
    return -1;
  }

  for (i = slen - 1; i >= max_count; i--)
  {
    if (path[i] == '/')
    {
      /* Found the delimitation */
      path[i] = 0;
      status = 0;
      break;
    }
  }
  /* Have not found any sub directories */
  return status;
}

/**********************************************************************
 * readdir_helper
 *
 * Reads a directory and returns the next occurring node by skipping if a
 * "." or ".." is found. Assumes the directory stream pointer is valid.
 * No stats or any validation is performed. It is up to the calling functions
 * to make sure the passed pointer is validated.
 */

static struct fs_dirent*
readdir_helper (EFSDIR *dir_stream_ptr)
{
  struct fs_dirent  *dirent = 0;
  int skip = 0;

  do
  {
    skip = 0;
    dirent = efs_readdir (dir_stream_ptr);
    if (dirent
        && dirent->d_name[0] == '.'
        && dirent->d_name[1] == 0)
    {
      /* We have found a '.' file so proceed to read further */
      skip = TRUE;
    }
    else if (dirent
             && dirent->d_name[0] == '.'
             && dirent->d_name[1] == '.'
             && dirent->d_name[2] == 0)
    {
      /* We have found a '..' file so proceed for next entry */
      skip = TRUE;
    }
  } while (skip);

  return dirent;
}

/**********************************************************************
 * efs_deltree
 *
 * Deletes the directory and all its contents. 'path' can be a directory
 * file, node or sym link. If a non directory is passed as argument it is
 * simply deleted. If the argument is a directory all its contents and itself
 * is deleted. Returns 0 on success and -1 on failure. If a failure occurs,
 * the tree is only partially deleted. On a failure it is unknown as to how
 * many files or directories are left in the parent directory.
 */

int
efs_deltree (const char *path)
{
  EFSDIR            *dir_stream_ptr = 0;
  struct fs_dirent  *dirent = 0;
  struct fs_stat     stat_buf;
  int status = TRUE;
  word dlen = 0;
  int result;

  /* check validity for the input dir tree path. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  if (efs_lstat (path, &stat_buf) != 0)
  {
    /* Unable to stat the path. Return an error */
    return -1;
  }

  if (!S_ISDIR (stat_buf.st_mode))
  {
    /* This is not a directory */
    if (efs_unlink (path) != 0)
    {
      /* Failed to delete the entity return fail */
      status = -1;
    }
    /* We are done here so return 0 */
    status = 0;
  }
  else
  {
    /* This is a directory */
    dir_stream_ptr = efs_opendir (path);
    if (!dir_stream_ptr)
    {
      /* Unable to open the directory */
      status = -1;
    }
    dlen = strlen (path);

    if (dlen > FS_PATH_MAX)
    {
      efs_errno = ENAMETOOLONG;
      return -1;
    }

    /*
     * Lock the Buffer so that other functions do not use it while the buffer
     * is in use.
     */
    FS_PATH_BUFF_LOCK ();
    strlcpy (fs_path_buff[0], path, sizeof (fs_path_buff[0]));

    while (status > 0)
    {
      dirent = readdir_helper (dir_stream_ptr);

      if (dirent == NULL)
      {
        /* Empty. Delete this directory and return */
        if (efs_closedir (dir_stream_ptr) < 0)
        {
          status = -1;
        }
        if ((status > 0) && (efs_rmdir (fs_path_buff[0]) < 0))
        {
          status = -1;
        }
        /* Make sure we have reached the head directory */
        if ((status > 0) && (dlen == strlen (fs_path_buff[0])))
        {
          /* We have completely erased the tree successfully return. */
          status = 0;
          break;
        }
        else
        {
          /* Go one path back. There is more to do */
          if (deltree_truncate_path (fs_path_buff[0], dlen) == 0)
          {
            /* It has to be a directory don't have to lstat again */
            /* Open the directory and continue the while loop */
            dir_stream_ptr = efs_opendir (fs_path_buff[0]);
            if (!dir_stream_ptr)
            {
              status = -1;
            }
          }
        }
      }
      else
      {
        /* There is some thing more, check it out and process */
        strlcat (fs_path_buff[0], "/", sizeof (fs_path_buff[0]));
        strlcat (fs_path_buff[0], dirent->d_name, sizeof (fs_path_buff[0]));

        if (efs_lstat (fs_path_buff[0], &stat_buf) == -1)
        {
          /* Unable to stat the file or directory. Return an error */
          status = -1;
        }
        else if (S_ISDIR (stat_buf.st_mode))
        {
          /* Close the parent directory so that we could reuse
           * dir_stream_ptr.
           */
          if (efs_closedir (dir_stream_ptr) < 0)
          {
            /* Unable to Close directory. Return an error */
            status = -1;
          }

          /* Open the directory and continue the while loop */
          if (status > 0)
          {
            dir_stream_ptr = efs_opendir (fs_path_buff[0]);
          }
          if (!dir_stream_ptr)
          {
            status = -1;
          }
        }
        else
        {
          /* It should be a file or a link delete this and get back to
           * while loop to look for another.
           */
          if (efs_unlink (fs_path_buff[0]) != 0)
          {
            /* Failed to unlink */
            status = -1;
          }

          /* Get the directory name to the path so as to use again */
          if (deltree_truncate_path (fs_path_buff[0], dlen) < 0)
          {
            status = -1;
          }
        }
      }
    }
    /*
     * Release the buffer so other functions could use.
     */
    FS_PATH_BUFF_UNLOCK ();
  }
  /* We might get here even if one of the above efs_closedir () fails
   * It should be OK to try closing the directory once more.
   */
  efs_closedir (dir_stream_ptr);
  /* Lets make sure one last time if the function was successful */
  if ((status == 0) && (efs_lstat (path, &stat_buf) == 0))
  {
    /* Status is 0, no errors found and the directory still exists,
     * which is seriously wrong. Directory should have not exist if there
     * were no errors. ERR_FATAL for further investigation.
     */
    FS_ERR_FATAL (" efs_deltree () Failed ",0,0,0);
  }

  return status;
}

/**********************************************************************
 * efs_auto_mkdir
 *
 * When using efs_put to insert a new item, the directory desired may
 * not exist. Therefore this function will scan the path, and then create
 * the directory. After the directory is created, it will return to efs_put ()
 * to continue inserting the item. Only the path untill the last "/" char is
 * created. If the path is "/a/b/c/d" only the path till /a/b/c is created
 * but if the path given is "/a/b/c/d/" then even d is created.
 *
 * The case where multiple threads attempt to create directories in the same
 * path is handled by ignoring the EXIST errors. The case where directories in
 * the path are removed by another thread is not handled and is thus a
 * limitation of this fucntion.
 *
 * Returns a zero on success, or -1 for an error.  Upon error, efs_errno
 * will be set to one of the following:
 *   ENAMETOOLONG - If the supplied path exceeds max possible path.
 *   EINVAL - If the path is invalid.
 *   ENOSPC - If we run out of space.
 */

int
efs_auto_mkdir(const char * path)
{
  int result = 0;
  int index = 0;
  int path_len = 0;
  char *temp_path;

  if (path == NULL)
  {
    efs_errno = EINVAL;
    return -1;
  }

  path_len = strlen(path);

  if (path_len == 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  /* If path length is more than we can store in the buffer we have,
   * then return with error. The buffer has an extra space allocated
   * for the null terminator also. */
  if (path_len > FS_PATH_MAX)
  {
    efs_errno = ENAMETOOLONG;
    return -1;
  }

  FS_PATH_BUFF_LOCK();
  temp_path = fs_path_buff[0];
  strlcpy (temp_path, path, sizeof (fs_path_buff[0]));

  /* Set index to path_len - 1 as there is atleast one char in the path.
   * and then go backwards in the path and try to create the directories.
   * We exit when the path runs out or we create one directory sucessfully or
   * if an error is encountered.
   */

  for (index = path_len - 1; index > 0; index--)
  {
    if (path[index] == '/')
    {
      temp_path[index] = 0x00;
      /* Attempt to make subdirectories */
      result = efs_mkdir (temp_path, 0777);

      /* Exit if we are sucessful in creating a directory. */
      if (result == 0)
      {
        break;
      }

      /* Exit if we encounter an error other than ENOENT. ENOENT implies the
       * parent directory of the directory we tried to create doesn't exist so
       * we try to create that directory. Any other error means we must exit.
       */
      if ((result != 0) && (efs_errno != ENOENT))
      {
        break;
      }
    }
  }

  if (result == 0)
  {
    /* If the current index was a '/' put it back. */
    if (path[index] == '/')
    {
      temp_path[index++] = '/';
    }

    for (/* Current index value */; index < path_len; index++)
    {
      /* Go forward up the path until we reach the end */
      if (path[index] == '/')
      {
        /* Attempt to make subdirectories */
        result = efs_mkdir(temp_path, 0777);

        /* If we fail to create a directory and it doesn't exist we exit .*/
        if (result != 0)
        {
          if (efs_errno == EEXIST)
          {
            result = 0; /* The directory exists we can proceed */
          }
          else
          {
            break;
          }
        }

        temp_path[index] = '/';
      }
    }
  }
  else if (efs_errno == EEXIST)
  {
    result = 0;
  }

  FS_PATH_BUFF_UNLOCK ();
  return result;
}

/* Writes the input buffer into an item file, if this fails because the
 * buffer size is too large, write the data into a regular file. */
static int
efs_put_core (const char *path, void *data, fs_size_t length, int oflag,
    int mode)
{
  int fd = 0;
  int result = 0;
  int path_len = 0;
  char *temp_path;
  int temp_errno = 0;

  path_len = strlen(path);

  if (path_len > FS_PATH_MAX)
  {
    efs_errno = ENAMETOOLONG;
    return -1;
  }

  FS_PATH_BUFF_LOCK();
  FS_GLOBAL_LOCK ();

  temp_path = fs_path_buff[0];
  strlcpy (temp_path, path, sizeof (fs_path_buff[0]));

  if (path_len < FS_PATH_MAX)
    strlcat (temp_path, "$", sizeof (fs_path_buff[0]));
  else
    temp_path[FS_PATH_MAX-1] = '$';


  result = efs_pure_put (path, data, length, oflag, mode);
  /* On success nothing else to do, cleanup and exit. */
  if (result == 0)
    goto cleanup;

  /* If it is not an item, then we need to create the item and
   * then we need to handle things differently. */
  if (result < 0 && efs_errno == EEXIST)
  {
    (void) efs_unlink (temp_path);
    result = efs_pure_put (temp_path, data, length, O_CREAT, mode);
    if (result == 0)
    {
      result = efs_rename (temp_path, path);
      if (result != 0)
      {
        temp_errno = efs_errno;
        (void) efs_unlink (temp_path);
        efs_errno = temp_errno;
      }
      goto cleanup;
    }
  }

  if (result < 0 && efs_errno != FS_ERANGE)
  {
    /* At this point efs_pure_put() failed, it failed in the original path or
     * on the temp_path. If the error of the last efs_pure_put() call was
     * FS_ERANGE, then we should try to create an regular file instead
     * of an item file. How the error was not FS_ERANGE, exit with error. */
    goto cleanup;
  }

  /* Since the length is too long for a normal item, we need to open/create the
   * file at the path specified
   */

  /* In order to properly handle large files we need to loop the writes.
   * For normal items since the length isn't greater than 8k, we can call
   * the efs_pure_put function from fs_public.c
   */

  /* In order to make this function robust, when the rename() function is
   * fixed close the newly opened file, open a new temp file with a unique
   * name, write the data to the temp file, then after completion rename the
   * file to the desired name. */

  fd = efs_open(temp_path, O_RDWR | O_CREAT | O_TRUNC, mode);

  if (fd < 0)
  {
    /* There was an error opening the temporary file */
    result = -1;
    goto cleanup;
  }

  while (length > 0)
  {
    result = efs_write (fd, data, length);

    if (result < 0)
    {
      temp_errno = efs_errno;
      break;
    }
    data = (void *) ((uint8 *) data + result);
    length -= result;
  }

  /* And now don't forget to close the file */
  (void) efs_close (fd);

  /* If it comes out of the loop properly, then the write was successful we
   * move the file from the temporary file to the desired file using rename()
   * or in case of error we remove the temp file. */
  if (result > 0)
  {
    result = efs_rename (temp_path, path);
  }
  else
  {
    (void) efs_unlink (temp_path);
    efs_errno = temp_errno;
  }

cleanup:
  FS_GLOBAL_UNLOCK ();
  FS_PATH_BUFF_UNLOCK ();
  return result;
}

/*=============================================================================
FUNCTION    efs_iter_helper_push_bookmark

DESCRIPTION
    This function creates a symlink pointing which points to the supplied
    bookmark_path in our private folder. This bookmark_path can be retreived
    later by calling efs_iter_helper_pop_bookmark().

PARAMETERS
    bookmark_path -> [IN]
        The directory-path for which we have to create a symbolic link in our
        private folder.
    symlink_nbr <- [OUT]
        This dword gets incremented by 1 and is used to construct the name for
        the symlink. Pass the same dword again for pushing more bookmark_path's
        and each time this dword will get incremented by 1.

Return value
    [-1] if error.
    [ 0] if a symlink to the given bookmark_path was created successfully.
=============================================================================*/
static int
efs_iter_helper_push_bookmark (const char *bookmark_path, dword *symlink_nbr)
{
  /* see efs_iter() function to see why slot 3 */
  char *symlink_path = fs_path_buff[3];

  /* Validate the supplied parameters. */
  ASSERT (bookmark_path != NULL);
  ASSERT (symlink_nbr != NULL);

  /* Do not push empty paths */
  if (bookmark_path[0] == '\0')
  {
    return 0;
  }

  /* construct a name for symlink. */
  ++(*symlink_nbr);
  (void)snprintf (symlink_path, sizeof(fs_path_buff[0]),
      FS_EFS_PRIVATE_DIR_ITER_PATH"/%08ld.lnk", *symlink_nbr);

  return efs_symlink (bookmark_path, symlink_path);
}

/*=============================================================================
FUNCTION    efs_iter_helper_pop_bookmark

DESCRIPTION
    This function retreives the bookmark_path that was pushed earlier by
    calling the efs_iter_helper_push_bookmark() function.

PARAMETERS
    bookmark_path <- [OUT]
        The place holder to store the retreived path, must be atleast as big
        as FS_PATH_MAX+1 bytes.

Return value
    [-1] if error.
    [ 0] if there are no more paths to retreive or if we have retreived a valid
         path.
=============================================================================*/
static int
efs_iter_helper_pop_bookmark (char *bookmark_path)
{
  /* see efs_iter() function to see why slot 3 */
  char *symlink_path = fs_path_buff[3];
  EFSDIR *dirp = NULL;
  struct fs_dirent *direntp;
  struct fs_stat stat_buf;
  int result = 0;

  (void) strlcpy (symlink_path, FS_EFS_PRIVATE_DIR_ITER_PATH,
          sizeof(fs_path_buff[0]));

  dirp = efs_opendir (symlink_path);
  if (dirp == NULL)
  {
    result = -ENOENT;
    goto Error;
  }

  direntp = readdir_helper (dirp);
  if (direntp == NULL)
  {
    /* no more symlinks found, so return success. */
    goto Success;
  }

  (void) strlcpy (symlink_path, FS_EFS_PRIVATE_DIR_ITER_PATH,
                      sizeof(fs_path_buff[0]));
  (void) strlcat (symlink_path, "/", sizeof(fs_path_buff[0]));
  (void) strlcat (symlink_path, direntp->d_name, sizeof(fs_path_buff[0]));
  result = efs_stat (symlink_path, &stat_buf);
  if ( (result != 0) || (!S_ISDIR (stat_buf.st_mode)) )
  {
    /* The symlink is not pointing to a directory, so something wrong. */
    result = -ENOTDIR;
    goto Error;
  }

  memset (bookmark_path, '\0', sizeof(fs_path_buff[0]));
  result = efs_readlink (symlink_path, bookmark_path, sizeof(fs_path_buff[0]));
  if (result == -1)
  {
    goto Error;
  }

  result = efs_unlink (symlink_path);

  goto Success;

Success:
  goto Cleanup;

Error:
  ASSERT (result < 0);
  goto Cleanup;

Cleanup:
  if (dirp != NULL)
  {
    efs_closedir(dirp);
  }

  return result;
}

/*=============================================================================
    This function can be used to iterate through all the entries of any given
    directory recursively. For every entry found in the directory the supplied
    callback function will be invoked once. Pass 1 as want_dirs if you want to
    be called for sub-directories too. If the callback function's return value
    is not 0 then an error is assumed and the iteration proceeds with the next
    entry. If iterated through all entries without any error, returns 0.
=============================================================================*/

int
efs_iter (const char *path, int want_dirs, EFS_ITER_CALLBACK efs_iter_callback,
          const void *param)
{
  int result = 0, deltree_result;
  int callback_fail_cnt = 0, callback_efs_errno = 0;
  dword symlink_nbr = 0;
  int make_callback;
  EFSDIR *dirp;
  struct fs_dirent *direntp;
  struct fs_stat stat_buf;

  /* For this algorithm we are using 4 path-buffs as follows:
     slot [0] -> is used to store the full-path name, The full-path name is
                 concatenation of dir-path and direntp->d_name.
     slot [1] -> is used to store the dir-path whose content we are reading now
     slot [2] -> is used to store the bookmark_path. The bookmark_path is the
                 latest dir that we have encountered, but have not pushed
                 this dir yet, we have it in RAM for optimization. We push this
                 latest-dir upon encountering one more dir in the same level.
     slot [3] -> is used to store the symlink-path used by the above push and
                 pop functions. */
  char *full_path = fs_path_buff[0];
  char *dir_name = fs_path_buff[1];
  char *bookmark_path = fs_path_buff[2];

  /* Validate the supplied args. */
  ASSERT (path != NULL);
  ASSERT (efs_iter_callback != NULL);
  ASSERT (strlen(path) <= FS_PATH_MAX);

  FS_PATH_BUFF_LOCK();

  /* Prepare the iter directory for storing the symlinks, if there exists any
     file by the same name first remove it, then create the directory. */
  (void)efs_unlink (FS_EFS_PRIVATE_DIR_ITER_PATH);
  result = efs_mkdir (FS_EFS_PRIVATE_DIR_ITER_PATH, 0644);
  if (result < 0)
    return -1;

  (void) strlcpy (full_path, path, sizeof(fs_path_buff[0]));

  result = efs_lstat (full_path, &stat_buf);
  if (result != 0)
  {
    /* Unable to stat the path, so bail. */
    --callback_fail_cnt;
    callback_efs_errno = efs_errno;
    goto Error;
  }

  if (!S_ISDIR (stat_buf.st_mode))
  {
    /* Given path is a file, so just invoke callback for this single file
       and bail. */
    result = (*efs_iter_callback)(full_path, &stat_buf, param);
    if (result != 0)
    {
      --callback_fail_cnt;
      callback_efs_errno = efs_errno;
      goto Error;
    }
    goto Success;
  }

  if (want_dirs)
  {
    result = (*efs_iter_callback)(full_path, &stat_buf, param);
    if (result != 0)
    {
      /* callback function failed, so just record and proceed. */
      --callback_fail_cnt;
      callback_efs_errno = efs_errno;
    }
  }

  /*===========================================================================
  Alogorithm:
  ----------
  1. Start with empty bookmark_path
  2. Loop through all the items in the current directory
    2.1. If it is a file,then just invoke the callback and proceed to next item
    2.2  If it is a directory,push the bookmark_path,mark the current-directory
         as the bookmark_path
  3. When we reach end-of-directory, then consult bookmark_path
  4. If bookmark_path is non-empty then open this dir and proceed to step-2
  5. If boomark_path is empty then pop the pushed item and proceed to step-2
  6. If there are no more items to pop, then return.

  Note: While pushing bookmark_path, we shall ignore empty bookmarks
  ===========================================================================*/

  bookmark_path[0] = '\0';
  (void) strlcpy (dir_name, path, sizeof(fs_path_buff[0]));

  dirp = efs_opendir (path);
  while (dirp != NULL)
  {
    direntp = readdir_helper (dirp);
    while (direntp != NULL)
    {
      /* construct the full-path. */
      (void) strlcpy (full_path, dir_name, sizeof(fs_path_buff[0]));
      (void) strlcat (full_path, "/", sizeof(fs_path_buff[0]));
      (void) strlcat (full_path, direntp->d_name, sizeof(fs_path_buff[0]));

      result = efs_lstat (full_path, &stat_buf);
      if (result != 0)
      {
        int ignore_this_failure = 0;
#ifdef FS_UNIT_TEST
        if (efs_errno ==  ENODEV)
        {
          /* No device is mounted at this moutpoint, so ignore this error. */
          ignore_this_failure = 1;
          /* Also, treat this null mount-point as a file, don't try to iterate
           * this directory. */
          stat_buf.st_mode = S_IFREG;
        }
#endif /* FS_UNIT_TEST */

        if (ignore_this_failure == 0)
        {
          --callback_fail_cnt;
          callback_efs_errno = efs_errno;
          goto Error;
        }
      }

      make_callback = 1;

      if (S_ISDIR (stat_buf.st_mode))
      {
        result = efs_iter_helper_push_bookmark(bookmark_path, &symlink_nbr);
        if (result != 0)
        {
          --callback_fail_cnt;
          callback_efs_errno = efs_errno;
          goto Error;
        }
        (void) strlcpy (bookmark_path, full_path, sizeof(fs_path_buff[0]));
        make_callback = want_dirs;
      }

      if (make_callback)
      {
        result = (*efs_iter_callback)(full_path, &stat_buf, param);
        if (result != 0)
        {
          /* callback function failed, so just record and proceed. */
          --callback_fail_cnt;
          callback_efs_errno = efs_errno;
        }
      }

      /* proceed to the next item in the current directory. */
      direntp = readdir_helper (dirp);
    }

    /* Visited all the items in this directory, so consult bookmark. */
    efs_closedir (dirp);
    if (strcmp (bookmark_path, "") == 0)
    {
      /* bookmark_path is empty, so try to pop the first pushed item. */
      result = efs_iter_helper_pop_bookmark (bookmark_path);
      if (result != 0)
      {
        --callback_fail_cnt;
        callback_efs_errno = efs_errno;
        goto Error;
      }
      if (strcmp (bookmark_path, "") == 0)
      {
        /* bookmark_path is empty and no-more items to pop, so we should have
           visited all the items in the supplied path, so return success. */
        goto Success;
      }
    }

    /* Now bookmark_path will point to the dir that we should proceed with. */
    (void) strlcpy (dir_name, bookmark_path, sizeof(fs_path_buff[0]));
    bookmark_path[0] = '\0';
    dirp = efs_opendir (dir_name);
  }

  goto Success;

Success:
  goto Cleanup;

Error:
  ASSERT(result < 0);
  goto Cleanup;

Cleanup:
  FS_PATH_BUFF_UNLOCK();
  /* Remove the iter directory along with its contents if any. */
  deltree_result = efs_deltree (FS_EFS_PRIVATE_DIR_ITER_PATH);
  ASSERT (deltree_result == 0);
  ((void)deltree_result);

  result = callback_fail_cnt;
  efs_errno = callback_efs_errno;

  return result;
}

/* Holds the information about the recursive iterators. */
struct fs_dir_recursive_data
{
  uint8 in_use;     /* Indicates if iterator is in use. */
  uint8 first_done; /* On the first time of returning read we return the
                     * same path given to opendir_recursive. */
  int32 err_count;  /* Number of errors encountered while iterating. */
  uint32 push_count;/* Number of directories pushed into the simulated
                       directory stack */
  EFSDIR *dir_data; /* Current directory iterator from efs_opendir.  */
  char *parent_dir; /* Holds the directory name for the current EFSDIR */
  char *curr_path;  /* Holds the path to the current entry. */
};

/* We use all four fs_path_buff while iterating recursively, therefore we can
 * have only one recursive iterator. */
static struct fs_dir_recursive_data all_recursive_iter[1] =
                                           {{ 0, 0, 0, 0, NULL, NULL, NULL }};

/* Open/Read/Close dir recursive.
 * Equivalent to open/read/close dir calls, but they will recurse into all
 * subdirectories.*/

struct fs_dir_recursive_data *
efs_opendir_recursive (const char *dirname)
{
  struct fs_dir_recursive_data *iterator = NULL;
  size_t len;
  EFSDIR *dir_iter;

  /* We need to hold the pathbuf lock between open and close because
     open/read/close recursive use fs_path_buff to transfer info
     between each function call. */
  FS_PATH_BUFF_LOCK ();

  /* For this algorithm we are using 4 path-buffs as follows:
     slot [0] -> stores the parent dir path while iterating a directory.
     slot [1] -> is used to store the full-path name, The full-path name is
     concatenation of the parent dir path and direntp->d_name.
     slot [3] -> is used to store the symlink-path used by push and
     pop functions. They are used internally by the functions. */

  /* If the iterator is in use return error.  */
  if (all_recursive_iter[0].in_use != 0)
  {
    efs_errno = EMFILE;
    goto open_dir_error;
  }

  /* Check input path lengh is OK. */
  len = strlen (dirname);
  if (len >= FS_PATH_MAX)
  {
    efs_errno = ENAMETOOLONG;
    goto open_dir_error;
  }

  dir_iter = efs_opendir (dirname);
  if (dir_iter != NULL)
  {
    int result;
    char *pathbuf;

    /* Cleanup the directory stack, there may be stale from previous runs. */
    (void) efs_deltree (FS_EFS_PRIVATE_DIR_ITER_PATH);
    result = efs_mkdir (FS_EFS_PRIVATE_DIR_ITER_PATH, 0644);
    if (result < 0)
    {
      (void) efs_closedir (dir_iter);
      goto open_dir_error;
    }

    iterator = &all_recursive_iter[0];

    pathbuf = fs_path_buff[0];
    ASSERT (efs_normalize_path_string (dirname, pathbuf,
                                       sizeof(fs_path_buff[0])));
    /* Recursive iterators don't handle relative paths, make sure this
     * path is an absolute path by adding a / at the begining. */
    if (pathbuf[0] != '/')
    {
      int len = strlen (pathbuf);
      fs_memsmove (pathbuf + 1, sizeof (fs_path_buff[0]) - 1, pathbuf, len+1);
      pathbuf[0] = '/';
    }

    iterator->in_use = 1;
    iterator->first_done = 0;
    iterator->err_count = 0;
    iterator->push_count = 0;
    iterator->dir_data = dir_iter;
    iterator->parent_dir = pathbuf;
    iterator->curr_path = fs_path_buff[1];
    iterator->curr_path[0] = '\0';
  }

open_dir_error:
  if (iterator == NULL)
    FS_PATH_BUFF_UNLOCK ();

  return iterator;
}

static const char *
efs_readdir_rec_check_entry (struct fs_dirent *dent,
                              struct fs_dir_recursive_data *rec_dirp)
{
  struct fs_stat stat_buf;
  char *curr_path;
  char *parent_dir;
  int result;
  uint32 len;

  if (dent == NULL || rec_dirp == NULL)
  {
    return NULL;
  }
  curr_path = rec_dirp->curr_path;
  parent_dir = rec_dirp->parent_dir;

  /* Construct the current path and check if it is a directory. */
  (void) strlcpy (curr_path, parent_dir, sizeof (fs_path_buff[0]));

  /* Concatenate a / only when it is not present. */
  len = strlen (curr_path);
  if (len < (sizeof (fs_path_buff[0]) - 2) && curr_path [len - 1] != '/')
  {
    curr_path [len] = '/';
    curr_path [len + 1] = '\0';
  }

  len = strlcat (curr_path, dent->d_name, sizeof (fs_path_buff[0]));
  /* Check if the destination path can hold the whole path. */
  if (len >= sizeof (fs_path_buff[0]))
  {
    rec_dirp->err_count++;
    return NULL;
  }

  /* Skip the direcory we are using as a stack. */
  if (strcmp (curr_path, FS_EFS_PRIVATE_DIR_ITER_PATH) == 0)
    return NULL;

  result = efs_lstat (curr_path, &stat_buf);
  if (result != 0)
  {
    rec_dirp->err_count++;
    return NULL;
  }

  if (S_ISDIR (stat_buf.st_mode))
  {
    result = efs_iter_helper_push_bookmark (curr_path, &rec_dirp->push_count);
    if (result != 0)
      rec_dirp->err_count++;
  }

  return curr_path;
}


/* Closes the previous directory iterator, gets and opens the next directory.
 *
 * return  Returns 0 when a new directory iterator was opened and needs to
 *         be iterated. Returns 1 when the stack is empty, when there is no
 *         more directories to iterate.
 **/
static int
efs_recursive_get_next_dir (struct fs_dir_recursive_data *rec_dirp)
{
  int result, error;
  EFSDIR *dir_iter = NULL;

  result = efs_closedir (rec_dirp->dir_data);
  if (result != 0)
  {
    error = efs_errno;  /* To keep lint happy */
    FS_MSG_LOW_1 ("%d Error while closing recursive iterator", error);
  }

  result = 0;
  do
  {
    /* Clear the current parent dir and get the next directory. */
    rec_dirp->parent_dir[0] = '\0';
    result = efs_iter_helper_pop_bookmark (rec_dirp->parent_dir);
    /* To check if we stack is empty we check that a new parent directory
       was not returned. Pop bookmark does not have a way to indicate an
       empty stack */
    if (rec_dirp->parent_dir[0] == '\0')
    {
      result = 1;
      break;
    }

    /* Open the poped directory. */
    dir_iter = efs_opendir (rec_dirp->parent_dir);
    if (dir_iter == NULL)
    {
      error = efs_errno;  /* To keep lint happy */
      FS_MSG_LOW_1 ("%d Error while opening recursive iterator", error);
    }

  } while (dir_iter == NULL);

  rec_dirp->dir_data = dir_iter;
  return result;
}

/* returns the path to the current entry. */
const char *
efs_readdir_recursive (struct fs_dir_recursive_data *rec_dirp)
{
  struct fs_dirent *dent;
  const char *full_path;

  if (rec_dirp == NULL || rec_dirp->in_use != 1)
  {
    efs_errno = EINVAL;
    return NULL;
  }

  /* Check if we have returned the first entry (the parent directory),
   * if not, return it. */
  if (rec_dirp->first_done == 0)
  {
    rec_dirp->first_done = 1;
    return rec_dirp->parent_dir;
  }

  /* We finished scaning this directory, check if there are any more left. */
  do
  {
    dent = readdir_helper (rec_dirp->dir_data);

    /* Valid directory entry, check if it is a dir and push it.*/
    full_path = efs_readdir_rec_check_entry (dent, rec_dirp);

    /* If full path is NULL, we have finished with the current directory.
       pop the next. */
    if (full_path == NULL)
      if (efs_recursive_get_next_dir (rec_dirp) > 0)
        break;

  } while (full_path == NULL);

  return full_path;
}

/* returns number of errors encountered while iterating. */
int
efs_closedir_recursive (struct fs_dir_recursive_data *rec_dirp)
{
  int32 err_count;

  if (rec_dirp == NULL || rec_dirp->in_use != 1)
  {
    efs_errno = EINVAL;
    return -1;
  }

  (void) efs_closedir (rec_dirp->dir_data);

  rec_dirp->in_use = 0;
  rec_dirp->first_done = 0;
  rec_dirp->dir_data = NULL;
  err_count = rec_dirp->err_count;

  (void) efs_deltree (FS_EFS_PRIVATE_DIR_ITER_PATH);

  FS_PATH_BUFF_UNLOCK ();

  return err_count;
}

#ifdef FS_UNIT_TEST
/* These are helper functions available only for the simulator build to
   call the static functions in this file. The test-files need to call
   these static function for testing them. */
int
efs_iter_for_simulator (const char *path, int want_dirs,
          EFS_ITER_CALLBACK efs_iter_callback, const void *param)
{
  return efs_iter (path, want_dirs, efs_iter_callback, param);
}

int
efs_iter_helper_push_bookmark_for_simulator (const char *bookmark_path,
          dword *symlink_nbr)
{
  return efs_iter_helper_push_bookmark (bookmark_path, symlink_nbr);
}
#endif /* FS_UNIT_TEST */

/*=============================================================================
FUNCTION    efs_chown_recursive_callback

DESCRIPTION
    Callback function to support efs_chown_recursive(). For every file
    found in the directory supplied to efs_chown_recursive()API, this function
    will get called once, so we could efs_chown() every single file here.

PARAMETERS
    path -> [IN]
        The file-path whom we have to chown().
    stat_buf -> [IN]
        stat info of this file, unused in this function.
    param -> [IN]
        This is a pointer to the struct chown_recursive_callback_param.

Return value
    [-ve] if error.
    [  0] if efs_chown() is success.
=============================================================================*/
static int
efs_chown_recursive_callback(char *path, struct fs_stat *stat_buf,
    const void *param)
{
  struct chown_recursive_callback_param *chown_data_ptr;

  /* Validate the supplied args. */
  ASSERT (path != NULL);
  ASSERT (stat_buf != NULL);
  ASSERT (param != NULL);

  chown_data_ptr = (struct chown_recursive_callback_param *)param;
  return efs_chown (path, chown_data_ptr->uid_val, chown_data_ptr->gid_val);
}

int
efs_chown_recursive (const char *path, int uid_val, int gid_val)
{
  int result;
  struct chown_recursive_callback_param chown_data;

  /* check validity for the given path. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  chown_data.uid_val = uid_val;
  chown_data.gid_val = gid_val;
  return efs_iter(path, 1, efs_chown_recursive_callback, &chown_data);
}

/*=============================================================================
FUNCTION    efs_chmod_recursive_callback

DESCRIPTION
    Callback function to support efs_chmod_recursive(). For every entry
    found in the directory supplied to efs_chmod_recursive()API, this function
    will get called once, so we could efs_chmod() every single entry here.

PARAMETERS
    path -> [IN]
        The file-path whom we have to chown().
    stat_buf -> [IN]
        stat info of this entry, unused in this function.
    param -> [IN]
        This is the mode of type fs_mode_t.

Return value
    [-ve] if error.
    [  0] if efs_chmod() is success.
=============================================================================*/
static int
efs_chmod_recursive_callback(char *path, struct fs_stat *stat_buf,
    const void *param)
{
  uint32 param_mode  = (uint32 )param;
  fs_mode_t mode = (fs_mode_t )param_mode;

  /* Validate the args. */
  ASSERT (path != NULL);
  ASSERT (stat_buf != NULL);

  return efs_chmod (path, mode);
}

int
efs_chmod_recursive (const char *path, fs_mode_t mode)
{
  uint32 param_mode  = mode;
  int result;

  /* check validity for the given path. */
  result = fs_is_path_valid (path);
  if (result != 0)
  {
    efs_errno = EINVAL;
    return -1;
  }

  return efs_iter(path, 1, efs_chmod_recursive_callback,
              (const void *)param_mode);
}

/* For efs_md5sum to be re-entrant, the memory for data read has to be
 * allocated for each call. Caller passing the memory or dynamic memory were
 * some of the options. The chosen option is allocating a small chunk of
 * memory.  MD5_BUF_SIZE is the amount of memory used for reading the
 * data from the file.
 */
#define MD5_BUF_SIZE 64
/* Calculate the md5 hash file with name "path". */
int
efs_md5sum (const char *path, struct fs_md5sum *hash_out_data)
{
  int read_size;
  unsigned char mem_read_buffer[MD5_BUF_SIZE];
  MD5_CTX hash_context;
  int fd, result;

  /* check validity for the given path. */
  result = fs_is_path_valid (path);
  if ( (result != 0) || (hash_out_data == NULL) )
  {
    efs_errno = EINVAL;
    return -1;
  }

  if (NULL == hash_out_data) {
    return -1;
  }
  fd = efs_open (path, O_RDONLY);

  if (fd < 0) {
    return -1;
  }

  /* Initialize md5 hash calculation. */
  MD5Init (&hash_context);

  while ((read_size = efs_read (fd, mem_read_buffer, MD5_BUF_SIZE)) > 0) {

    /* Perform hash calculation for the latest data-read. */
    MD5Update (&hash_context, mem_read_buffer, read_size);
  }

  /* Copy the md5sum value to hash_out_data.  */
  MD5Final (hash_out_data->hash, &hash_context);
  if (efs_close (fd)) {
    return -1;
  }

  /* Check to see if the read failed.
   */
  if (read_size < 0) {
    return -1;
  }

  return 0;
}

#endif /* FEATURE_EFS2 */
