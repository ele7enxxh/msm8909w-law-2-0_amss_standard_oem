/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  E M B E D D E D   F I L E   S Y S T E M

GENERAL DESCRIPTION
  Compatibility interface for EFS2. Users of EFS2 should use the POSIX
  interface as much as possible because this interface is deprecated.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  fs_compat_init should be called before any calls to the API are made.
  FEATURE_EFS_ACCESS_METHODS needs to be turned on.

Copyright (C) 2002-2007, 2009-2013 by QUALCOMM Technologies, Inc.
All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_compat.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who    what, where, why
--------   ---    ----------------------------------------------------------
10/09/13   nr     Replace banned memory functions with safer versions.
03/26/13   wek    Rename macros from MSG_ to FS_MSG for efs code.
03/20/12   wek    Cleanup customer header file inclusion.
03/08/12   wek    Rename macros and remove header files to compile for boot.
02/29/12   rp     Make Tasks free-float (CPU affinity free)
11/02/11   wek    Use library supplied strl cpy and cat instead of fs_strl*.
04/28/10   vr     Relocate fs_task from EFS1 to EFS2
06/12/09   ebb    Moved rex_xxx calls to fs_osal.h abstraction.
01/02/07   dlb    Use safer string operations.
12/22/06   sh     Correct compiler warning when Hotplug is undefined.
11/16/06   umr    Increase fs_compat task's stack size
11/10/06   umr    Backoff Changes to fs_compat layer to execute as API
10/30/06   sh     Ensure that no FS Command Signals are received.
10/30/06   umr    Change fs_compat layer to execute as API
09/01/06   yg     Fixed compilation issue for non FTL enabled builds.
08/28/06   yg     Added code to handle sync on power down, offline and timer.
07/24/06   sch    Passed in the file name to efs_statvfs()
                  in fs_compat_statfs
06/29/06   dlb    Remove check for seek past end (too slow on SFAT).
06/27/06   dlb    Optimize directory iteration for SFAT.
06/07/06   sh     Featurized hotplug_format()
05/07/06   sh     Removed superfluous break/return statements.
03/28/06   sh     Added FORMAT operation handling
10/26/05   sh     Lint cleanup.
05/18/05   sh     Return the error code in failed fs_compat_seek_read/write
04/25/05   nrs    Fixed bug in rename check
04/04/05   nrs    Added check to see if file already exists prior to renaming
03/23/05   nrs    Added TIMETEST code for 6275u
10/15/04   dlb    Adjust copyright line.
10/07/04   dlb    Whitespace cleanup.
06/14/04   gr     Fixed compiler warnings.
05/11/04   gr     Return an error on an attempt to seek past the end of a file.
04/15/04   gr     Renamed errno to efs2_errno to avoid a conflict with the
                  errno macro.
11/17/03   gr     Fixed bugs in the read and write functions.
11/14/03   gr     Fixed a bug in fs_compat_get_attribs.
11/11/03   gr     Sets creation_date correctly rather than to a bogus constant.
10/18/03   gr     Changes to better mimic EFS1.
09/22/03   gr     Undid previous change. Apparently EFS1 does not overwrite
                  files by default. compat_open now returns the right error
                  code instead.
09/09/03   gr     Removed O_EXCL as a default option when creating a file. EFS1
                  allows users to overwrite existing files by default.
06/18/03   gr     Fixed a bug in fs_compat_file_size.
06/17/03  jkl     Clean up code.
04/18/03   gr     Fixed directory listing and enhanced trailing slashes code.
04/03/03   gr     The mkdir and rmdir functions now ignore trailing slashes,
                  to match EFS1 behavior.
03/14/03   gr     Fixed the read and write functions and the file and
                  directory removal functions to better mimic EFS1 return
                  values.
03/06/03   gr     Bug fix in the file_size function. Also got rid of a warning.
09/15/02   gr     Bug fixes in the directory iteration functions.
08/06/02   gr     Created.

===========================================================================*/

#include "fs_config_i.h"

#ifdef FEATURE_EFS_COMPATIBILITY

#include "fs_sys_types.h"
#include "fs_err.h"
#include "fs_public.h"
#include "fs_errno.h"
#include "fs_hotplug.h"
#include "fs_util.h"
#include "queue.h"
#include "fsi.h"
#include "fs_am.h"
#include "fs_osal.h"
#include "task.h"
#include "dog.h"
#include "fs_compat.h"
#include "fs_ftl_cache.h"

#include <stdio.h>
#include <string.h>
#include "stringl/stringl.h"

#ifdef TIMETEST
#error code not present
#endif

#ifdef FEATURE_EFS_ACCESS_METHODS

/* The stack size of the compatibility task.
 */
#define FS_COMPAT_STACK_SIZ         4096

#define FS_COMPAT_RPT_TIMER_SIG   0x0001
#define FS_COMPAT_CACHE_SYNC_SIG  0x0002

#define FTL_CACHE_SYNC_TIME       5000

#define FS_COMPAT_KICK_WATCHDOG()

/* TCB for the compatibility task.
 * Since this compatibility task doesn't have header file
 * the tcb is not included anywhere in headers, but since
 * tmc needs to access this tcb in order to send the power
 * down or offline signal this has been made global.
 * NOTE: Look at file "tmc.c" where this is accessed */
fs_os_tcb_type fs_compat_tcb;

/* Stack for the compatibility task */
static unsigned char fs_compat_stack [FS_COMPAT_STACK_SIZ];

/* Watchdog report timer for the compatibility task. */
static fs_os_timer_type fs_compat_rpt_timer;
static fs_os_timer_type fs_cache_sync_timer;

/* Our commands are placed in this queue. */
static q_type fs_compat_queue;

typedef enum {
  FS_COMPAT_TYPE_FILE,
  FS_COMPAT_TYPE_DIR
} fs_compat_file_type;

/***********************************************************************
 * FUNCTION      fs_compat_map_err
 *
 * DESCRIPTION   Maps EFS2 error codes to EFS1 error codes
 *
 ***********************************************************************/
static fs_status_type
fs_compat_map_err (int efs2_errno, fs_compat_file_type filetype)
{
  fs_status_type efs1_errno = FS_FAIL_S;

  if (filetype == FS_COMPAT_TYPE_FILE) {
    switch (efs2_errno) {
      case EBUSY:
        efs1_errno = FS_BUSY_S;
        break;

      case EBADF:
        efs1_errno = FS_FILE_NOT_OPEN_S;
        break;

      case ETXTBSY:
        efs1_errno = FS_FILE_OPEN_S;
        break;

      case EEXIST:
        efs1_errno = FS_FILE_ALREADY_EXISTS_S;
        break;

      case ENOENT:
      case EACCES:
      case EISDIR:
        efs1_errno = FS_NONEXISTENT_FILE_S;
        break;

      case EINVAL:
        efs1_errno = FS_ILLEGAL_OPERATION_S;
        break;

      case EPERM:
        efs1_errno = FS_PARAMETER_ERROR_S;
        break;

      case ENAMETOOLONG:
        efs1_errno = FS_BAD_FILE_NAME_S;
        break;

      case EEOF:
        efs1_errno = FS_EOF_S;
        break;

      case ENOSPC:
        efs1_errno = FS_SPACE_EXHAUSTED_S;
        break;

      case EMFILE:
        efs1_errno = FS_OPEN_TABLE_FULL_S;
        break;

      default:
        efs1_errno = FS_FAIL_S;
        break;
    }
  }
  else if (filetype == FS_COMPAT_TYPE_DIR) {
    switch (efs2_errno) {
      case EBUSY:
        efs1_errno = FS_BUSY_S;
        break;

      case EEXIST:
        efs1_errno = FS_DIR_ALREADY_EXISTS_S;
        break;

      case ENOENT:
      case ENOTDIR:
      case EACCES:
        efs1_errno = FS_NONEXISTENT_DIR_S;
        break;

      case ENOTEMPTY:
        efs1_errno = FS_DIR_NOT_EMPTY_S;
        break;

      case EINVAL:
        efs1_errno = FS_ILLEGAL_OPERATION_S;
        break;

      case EPERM:
        efs1_errno = FS_PARAMETER_ERROR_S;
        break;

      case ENOSPC:
        efs1_errno = FS_DIR_SPACE_EXHAUSTED_S;
        break;

      default:
        efs1_errno = FS_FAIL_S;
        break;
    }
  } else {
    FS_ERR_FATAL ("Unknown file type %d", filetype, 0, 0);
  }

  return efs1_errno;
}

/***********************************************************************
 * FUNCTION      fs_compat_open
 *
 * DESCRIPTION   Handles the FS_OPEN_OP command.
 *
 ***********************************************************************/
static fs_handle_type
fs_compat_open (fs_cmd_type *cmd_ptr)
{
  int            fs_compat_handle;
  fs_handle_type handle = FS_NULL_HANDLE;
  int            oflag;
  fs_mode_t      mode;

  switch (cmd_ptr->fs_op.parms.open.access_kind)
  {
    case FS_OA_CREATE:
      oflag = O_CREAT | O_RDWR | O_EXCL;
      break;

    case FS_OA_APPEND:
      oflag = O_APPEND | O_RDWR;
      break;

    case FS_OA_READONLY:
      oflag = O_RDONLY;
      break;

    case FS_OA_READWRITE:
      oflag = O_RDWR;
      break;

    case FS_OA_TRUNCATE:
      oflag = O_RDWR | O_TRUNC;
      break;

    case FS_OA_TEST:
      return FS_FAIL_S;

    default:
      FS_ERR ("Invalid access kind %d given to open request",
              cmd_ptr->fs_op.parms.open.access_kind, 0, 0);
      return FS_FAIL_S;
  }

  mode = S_IWRITE | S_IREAD;

  fs_compat_handle = efs_open (
      cmd_ptr->fs_op.parms.open.option.create.filename,
      oflag, mode);
  if (fs_compat_handle >= 0)
  {
    handle = (fs_handle_type) fs_compat_handle;
    cmd_ptr->fs_op.status = FS_OKAY_S;
  }
  else
    cmd_ptr->fs_op.status = fs_compat_map_err (efs_errno, FS_COMPAT_TYPE_FILE);
  return handle;
} /* END fs_compat_open */

/***********************************************************************
 * FUNCTION      fs_compat_write
 *
 * DESCRIPTION   Handles the FS_WRITE_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_write (fs_cmd_type *cmd_ptr)
{
  int        fs_compat_handle;
  fs_ssize_t result = 0;
  fs_size_t  nbytes;
  fs_size_t  bytes_written;
  char      *write_buf;

  fs_compat_handle = (int) cmd_ptr->fs_op.parms.write.fhandle;

  nbytes = (fs_size_t) cmd_ptr->fs_op.parms.write.length;
  bytes_written = 0;
  write_buf = (char *) cmd_ptr->fs_op.parms.write.buf;
  while (bytes_written < nbytes)
  {
    result = efs_write (fs_compat_handle,
                        (const void *) (write_buf + bytes_written),
                        nbytes - bytes_written);
    if (result <= 0)
      break;
    bytes_written += (fs_size_t) result;
  }

  cmd_ptr->rsp_msg->write.count = bytes_written;

  if ((result < 0) && (efs_errno != ENOSPC))
    return fs_compat_map_err (efs_errno, FS_COMPAT_TYPE_FILE);
  else
    return FS_OKAY_S;
} /* END fs_compat_write */

/***********************************************************************
 * FUNCTION      fs_compat_read
 *
 * DESCRIPTION   Handles the FS_READ_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_read (fs_cmd_type *cmd_ptr)
{
  int        fs_compat_handle;
  fs_ssize_t result = 0;
  fs_size_t nbytes;
  fs_size_t bytes_read;
  char *read_buf;

  fs_compat_handle = (int) cmd_ptr->fs_op.parms.read.fhandle;

  nbytes = (fs_size_t) cmd_ptr->fs_op.parms.read.length;
  bytes_read = 0;
  read_buf = (char *) cmd_ptr->fs_op.parms.read.buf;
  while (bytes_read < nbytes)
  {
    result = efs_read (fs_compat_handle,
                       (void *) (read_buf + bytes_read),
                       nbytes - bytes_read);
    if (result <= 0)
      break;
    bytes_read += (fs_size_t) result;
  }

  cmd_ptr->rsp_msg->read.count = (dword) bytes_read;

  if (result < 0)
    return fs_compat_map_err (efs_errno, FS_COMPAT_TYPE_FILE);
  else
    return FS_OKAY_S;
} /* END fs_compat_read */

/***********************************************************************
 * FUNCTION      fs_compat_seek_read
 *
 * DESCRIPTION   Handles the FS_SEEK_READ_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_seek_read (fs_cmd_type *cmd_ptr)
{
  int        handle;
  fs_ssize_t result = 0;
  int        origin;
  fs_size_t  nbytes;
  fs_size_t  bytes_read;
  char      *read_buf;
  fs_off_t   offset;

  handle = (int) cmd_ptr->fs_op.parms.seek_read.fhandle;
  offset = (fs_off_t) cmd_ptr->fs_op.parms.seek_read.position;

  switch (cmd_ptr->fs_op.parms.seek_read.origin) {
    case FS_SEEK_SET:
      origin = SEEK_SET;
      break;

    case FS_SEEK_CURRENT:
      origin = SEEK_CUR;
      break;

    case FS_SEEK_EOF:
      origin = SEEK_END;
      break;

    default:
      return FS_BAD_SEEK_POS_S;
  }

  if (efs_lseek (handle, offset, origin) == -1)
    return fs_compat_map_err (efs_errno, FS_COMPAT_TYPE_FILE);

  nbytes = (fs_size_t) cmd_ptr->fs_op.parms.seek_read.length;
  bytes_read = 0;
  read_buf = (char *) cmd_ptr->fs_op.parms.seek_read.buf;
  while (bytes_read < nbytes)
  {
    result = efs_read (handle,
                       (void *) (read_buf + bytes_read),
                       nbytes - bytes_read);
    if (result <= 0)
      break;
    bytes_read += (fs_size_t) result;
  }

  cmd_ptr->rsp_msg->seek_read.count = (dword) bytes_read;

  if (result < 0)
    return fs_compat_map_err (efs_errno, FS_COMPAT_TYPE_FILE);
  else
    return FS_OKAY_S;
} /* END fs_compat_seek_read */

/***********************************************************************
 * FUNCTION      fs_compat_seek_write
 *
 * DESCRIPTION   Handles the FS_SEEK_WRITE_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_seek_write (fs_cmd_type *cmd_ptr)
{
  int        handle;
  fs_ssize_t result = 0;
  int        origin;
  fs_size_t  nbytes;
  fs_size_t  bytes_written;
  char      *write_buf;
  fs_off_t   offset;

  handle = (int) cmd_ptr->fs_op.parms.seek_write.fhandle;
  offset = (fs_off_t) cmd_ptr->fs_op.parms.seek_write.position;

  switch (cmd_ptr->fs_op.parms.seek_write.origin) {
    case FS_SEEK_SET:
      origin = SEEK_SET;
      break;

    case FS_SEEK_CURRENT:
      origin = SEEK_CUR;
      break;

    case FS_SEEK_EOF:
      origin = SEEK_END;
      break;

    default:
      return FS_BAD_SEEK_POS_S;
  }

  if (efs_lseek (handle, offset, origin) == -1)
    return fs_compat_map_err (efs_errno, FS_COMPAT_TYPE_FILE);

  nbytes = (fs_size_t) cmd_ptr->fs_op.parms.seek_write.length;
  bytes_written = 0;
  write_buf = (char *) cmd_ptr->fs_op.parms.seek_write.buf;
  while (bytes_written < nbytes)
  {
    result = efs_write (handle,
                        (const void *) (write_buf + bytes_written),
                        nbytes - bytes_written);
    if (result <= 0)
      break;
    bytes_written += (fs_size_t) result;
  }

  cmd_ptr->rsp_msg->seek_write.count = (dword) bytes_written;

  if ((result < 0) && (efs_errno != ENOSPC))
    return fs_compat_map_err (efs_errno, FS_COMPAT_TYPE_FILE);
  else
    return FS_OKAY_S;
} /* END fs_compat_seek_write */

/***********************************************************************
 * FUNCTION      fs_compat_close
 *
 * DESCRIPTION   Handles the FS_CLOSE_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_close (fs_cmd_type *cmd_ptr)
{
  int handle;

  handle = (int) cmd_ptr->fs_op.parms.close.fhandle;

  if (efs_close (handle) == 0)
    return FS_OKAY_S;
  else
    return fs_compat_map_err (efs_errno, FS_COMPAT_TYPE_FILE);
} /* END fs_compat_close */

/***********************************************************************
 * FUNCTION      fs_compat_tell
 *
 * DESCRIPTION   Handles the FS_TELL_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_tell (fs_cmd_type *cmd_ptr)
{
  int      handle;
  fs_off_t result;

  handle = (int) cmd_ptr->fs_op.parms.tell.fhandle;

  result = efs_lseek (handle, 0, SEEK_CUR);
  if (result == -1)
    return fs_compat_map_err (efs_errno, FS_COMPAT_TYPE_FILE);
  else
  {
    cmd_ptr->rsp_msg->tell.position = (fs_file_position_type) result;
    return FS_OKAY_S;
  }
} /* END fs_compat_tell */

/***********************************************************************
 * FUNCTION      fs_compat_seek
 *
 * DESCRIPTION   Handles the FS_SEEK_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_seek (fs_cmd_type *cmd_ptr)
{
  int      handle;
  int      origin;
  fs_off_t offset;

  handle = (int) cmd_ptr->fs_op.parms.seek.fhandle;
  offset = (fs_off_t) cmd_ptr->fs_op.parms.seek.position;

  switch (cmd_ptr->fs_op.parms.seek.origin) {
    case FS_SEEK_SET:
      origin = SEEK_SET;
      break;

    case FS_SEEK_CURRENT:
      origin = SEEK_CUR;
      break;

    case FS_SEEK_EOF:
      origin = SEEK_END;
      break;

    default:
      return FS_BAD_SEEK_POS_S;
  }

  if (efs_lseek (handle, offset, origin) == -1)
    return fs_compat_map_err (efs_errno, FS_COMPAT_TYPE_FILE);
  else
    return FS_OKAY_S;
} /* END fs_compat_seek */

/***********************************************************************
 * FUNCTION      fs_compat_file_size
 *
 * DESCRIPTION   Handles the FS_FILE_SIZE_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_file_size (fs_cmd_type *cmd_ptr)
{
  int            result;
  struct fs_stat sbuf;

  result = efs_stat (cmd_ptr->fs_op.parms.file_size.filename, &sbuf);
  if (result != 0)
    return fs_compat_map_err (efs_errno, FS_COMPAT_TYPE_FILE);

  cmd_ptr->rsp_msg->file_size.size = (dword) sbuf.st_size;
  return FS_OKAY_S;
} /* END fs_compat_file_size */

/***********************************************************************
 * FUNCTION      fs_compat_nametest
 *
 * DESCRIPTION   Handles the FS_NAMETEST_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_nametest (fs_cmd_type *cmd_ptr)
{
  int              result;
  struct fs_stat   sbuf;
  boolean          name_found = FALSE;
  fs_nametest_type the_type = cmd_ptr->fs_op.parms.nametest.type;

  result = efs_stat (cmd_ptr->fs_op.parms.nametest.filename, &sbuf);

  /* Need to verify this. What file types are valid on EFS2?
   */
  if (result != 0)
  {
    /* If there was an error, it is definitely not present. */
    /* leave name_found false. */
  }
  else if (!S_ISDIR (sbuf.st_mode) && !S_ISREG (sbuf.st_mode))
  {
  }
  else if (the_type == FS_TEST_FILE_OR_DIR
              || (the_type == FS_TEST_FILE && S_ISREG (sbuf.st_mode))
              || (the_type == FS_TEST_DIR  && S_ISDIR (sbuf.st_mode)))
  {
    name_found = TRUE;
  }
  else
  {
    /* Otherwise, the mode doesn't match. */
  }

  cmd_ptr->rsp_msg->nametest.name_found = name_found;
  return FS_OKAY_S;
} /* END fs_compat_nametest */

/***********************************************************************
 * FUNCTION      fs_compat_format
 *
 * DESCRIPTION   Handles the FS_FORMAT_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_format (fs_cmd_type *cmd_ptr)
{
  return ((hotplug_format (cmd_ptr->fs_op.parms.format.dirname) == 0)
          ? FS_OKAY_S : FS_FAIL_S);
} /* END fs_compat_format */

/***********************************************************************
 * FUNCTION      fs_compat_get_attribs
 *
 * DESCRIPTION   Handles the FS_GET_ATTRIBS_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_get_attribs (fs_cmd_type *cmd_ptr)
{
  int            result;
  struct fs_stat sbuf;
  int            handle;
  fs_time_t      ctime;

  if (cmd_ptr->fs_op.parms.get_attribs.filename == NULL)
  {
    /* Get open file attributes. */
    handle = (int) cmd_ptr->fs_op.parms.get_attribs.fhandle;

    result = efs_fstat (handle, &sbuf);
  }
  else
    result = efs_stat (cmd_ptr->fs_op.parms.get_attribs.filename, &sbuf);
  if (result != 0)
    return fs_compat_map_err (efs_errno, FS_COMPAT_TYPE_FILE);

  /* XXX: Set the following fields correctly.
   */
  cmd_ptr->rsp_msg->get_attribs.attributes = FS_FA_UNRESTRICTED;
  ctime = sbuf.st_ctime;
  if (ctime > CDMA_TO_FS_TIME_DELTA)
    ctime -= CDMA_TO_FS_TIME_DELTA;
  else
    ctime  = 0;
  cmd_ptr->rsp_msg->get_attribs.creation_date = ctime;
  cmd_ptr->rsp_msg->get_attribs.buffering_option = FS_OB_PROHIBIT;
  cmd_ptr->rsp_msg->get_attribs.cleanup_option = FS_OC_CLOSE;
  return FS_OKAY_S;
} /* END fs_compat_get_attribs */

/***********************************************************************
 * FUNCTION      fs_compat_set_attribs
 *
 * DESCRIPTION   Handles the FS_SET_ATTRIBS_OP command.
 *
 ***********************************************************************/
/* Nothing for now. */
static fs_status_type
fs_compat_set_attribs (fs_cmd_type *cmd_ptr)
{
  /* XXX: Not implemented yet.
   */
  (void) cmd_ptr;
  return FS_OKAY_S;
} /* END fs_compat_set_attribs */

/***********************************************************************
 * FUNCTION      fs_compat_fmemset
 *
 * DESCRIPTION   Handles the FS_FMEMSET_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_fmemset (fs_cmd_type *cmd_ptr)
{
  static unsigned char buffer[256];   /* A bigger buffer would be faster,
                                         but would use more RAM. */
  int handle;
  fs_off_t result;
  fs_size_t count;
  fs_size_t this;

  handle = cmd_ptr->fs_op.parms.fmemset.fhandle;

  /* Find out where we are. */
  result = efs_lseek (handle, 0, SEEK_CUR);
  if (result == -1)
    return fs_compat_map_err (efs_errno, FS_COMPAT_TYPE_FILE);

  if (result >= (fs_off_t) cmd_ptr->fs_op.parms.fmemset.position)
  {
    /* XXX: Verify this behavior.
     * If we are already at or past this position, there is nothing more to
     * do, so just return success.  I have not determined if the EFS will
     * cause a seek to the new position, so no guarantees that this
     * behavior is the same as EFS. */

    return FS_OKAY_S;
  }

  count = cmd_ptr->fs_op.parms.fmemset.position - result;

  /* Now write out blocks of data until we have data. */
  memset (buffer, cmd_ptr->fs_op.parms.fmemset.fill_value, sizeof (buffer));

  while (count != 0) {
    this = sizeof (buffer);
    if (this > count)
      this = count;

    result = efs_write (handle, buffer, this);

    if (result != (int) this)
      /* We didn't write enough, or else some other kind of error. */
      return FS_FAIL_S;

    count -= this;
  }

  return FS_OKAY_S;
} /* END fs_compat_fmemset */

/***********************************************************************
 * FUNCTION      fs_compat_rmfile
 *
 * DESCRIPTION   Handles the FS_RMFILE_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_rmfile (fs_cmd_type *cmd_ptr)
{
  if (efs_unlink (cmd_ptr->fs_op.parms.rmfile.filename) != 0)
    return fs_compat_map_err (efs_errno, FS_COMPAT_TYPE_FILE);
  else
    return FS_OKAY_S;
} /* END fs_compat_rmfile */

/***********************************************************************
 * FUNCTION      fs_compat_rename
 *
 * DESCRIPTION   Handles the FS_RENAME_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_rename (fs_cmd_type *cmd_ptr)
{
  struct fs_stat sbuf;
  if (efs_stat (cmd_ptr->fs_op.parms.rename.new_filename, &sbuf) == 0)
  {
    return FS_FILE_ALREADY_EXISTS_S;
  }
  if ( efs_rename (cmd_ptr->fs_op.parms.rename.old_filename,
                   cmd_ptr->fs_op.parms.rename.new_filename) != 0)
    return fs_compat_map_err (efs_errno, FS_COMPAT_TYPE_FILE);
  else
    return FS_OKAY_S;
} /* END fs_compat_rename */

/***********************************************************************
 * FUNCTION      fs_compat_mkdir
 *
 * DESCRIPTION   Handles the FS_MKDIR_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_mkdir (fs_cmd_type *cmd_ptr)
{
  fs_status_type mkdir_status;
  char *dirname;
  int dirname_len;
  int i;
  int result;

  /* EFS1 ignores trailing slashes at the end of directory names, but
   * EFS2 does not. So remove trailing slashes before calling efs_mkdir.
   */
  dirname = cmd_ptr->fs_op.parms.mkdir.dirname;
  dirname_len = strlen (dirname);
  for (i = dirname_len-1; i > 0 && dirname[i] == '/'; i--)
    dirname[i] = '\0';

  result = efs_mkdir (dirname, S_IREAD|S_IWRITE|S_IEXEC);
  if (result == 0)
    mkdir_status = FS_OKAY_S;
  else
    mkdir_status = fs_compat_map_err (efs_errno, FS_COMPAT_TYPE_DIR);

  return mkdir_status;
} /* END fs_compat_mkdir */

/***********************************************************************
 * FUNCTION      fs_compat_rmdir
 *
 * DESCRIPTION   Handles the FS_RMDIR_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_rmdir (fs_cmd_type *cmd_ptr)
{
  char *dirname;
  int dirname_len;
  int i;

  /* EFS1 ignores trailing slashes at the end of directory names, but
   * EFS2 does not. So remove trailing slashes before calling efs_rmdir.
   */
  dirname = cmd_ptr->fs_op.parms.mkdir.dirname;
  dirname_len = strlen (cmd_ptr->fs_op.parms.rmdir.dirname);
  for (i = dirname_len-1; i > 0 && dirname[i] == '/'; i--)
    dirname[i] = '\0';

  if (efs_rmdir (dirname) != 0)
    return fs_compat_map_err (efs_errno, FS_COMPAT_TYPE_DIR);
  else
    return FS_OKAY_S;
} /* END fs_compat_rmdir */

/***********************************************************************
 * FUNCTION      fs_compat_truncate
 *
 * DESCRIPTION   Handles the FS_TRUNCATE_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_truncate (fs_cmd_type *cmd_ptr)
{
  int fs_compat_handle;

  fs_compat_handle = (int) cmd_ptr->fs_op.parms.truncate.fhandle;
  if (efs_ftruncate (fs_compat_handle,
                     cmd_ptr->fs_op.parms.truncate.position) != 0)
    return fs_compat_map_err (efs_errno, FS_COMPAT_TYPE_FILE);
  else
    return FS_OKAY_S;
} /* END fs_compat_truncate */

/* The following variable is used by fs_compat_list_files_or_dirs,
 * fs_compat_num_files_or_dirs and fs_compat_enum_move. Consider making
 * this a local variable in each of those functions.
 */
static char pathbuf[FS_FILENAME_MAX_LENGTH + 1];

static fs_status_type
fs_compat_list_files_or_dirs (
  fs_cmd_type *cmd_ptr,
  fs_enum_type file_type)
{
  char              *dirname;
  dword              copy_index;
  dword              buf_limit;
  dword              list_buf_index = (dword)0;
  char              *list_buf;
  EFSDIR            *dir_stream_ptr;
  struct fs_dirent  *result;
  struct fs_stat     stat_buf;
  fs_status_type     status = FS_OKAY_S;

  if (file_type == FS_ENUM_FILES)
  {
    dirname = cmd_ptr->fs_op.parms.list_files.dirname;
  }
  else
  {
    dirname = cmd_ptr->fs_op.parms.list_dirs.dirname;
  }

  dir_stream_ptr = efs_opendir (dirname);
  if (dir_stream_ptr == NULL)
  {
    status = FS_NONEXISTENT_DIR_S;
    goto list_end;
  }

  /* The first two entries returned will be "." and "..". Ignore these.
   */
  result = efs_readdir (dir_stream_ptr);
  if (result == NULL)
  {
    /* This is bad. A directory without a . entry???? */
    status = FS_NONEXISTENT_DIR_S;
    goto list_end;
  }
  result = efs_readdir (dir_stream_ptr);
  if (result == NULL)
  {
    /* This is bad. A directory without a .. entry???? */
    status = FS_NONEXISTENT_DIR_S;
    goto list_end;
  }

  /* Copy to caller's buffer (allow for 2 terminating NULLs) */
  if (file_type == FS_ENUM_FILES)
  {
    buf_limit = cmd_ptr->fs_op.parms.list_files.length - 2;
    list_buf  = (char *) cmd_ptr->fs_op.parms.list_files.buf;
  }
  else
  {
    buf_limit = cmd_ptr->fs_op.parms.list_dirs.length - 2;
    list_buf  = (char *) cmd_ptr->fs_op.parms.list_dirs.buf;
  }

  list_buf[0] = 0x00;

  while (1) {
    result = efs_readdir (dir_stream_ptr);
    if (result == NULL)
    {
      break;
    }

    if (strlen (dirname) + 1 + strlen (result->d_name)
        > FS_FILENAME_MAX_LENGTH)
    {
      /* The pathname is too long. Return an error. Check if there is a
       * better error we can return. */
      status = FS_NONEXISTENT_DIR_S;
      goto list_end;
    }

    strlcpy (pathbuf, dirname, sizeof (pathbuf));
    strlcat (pathbuf, "/", sizeof (pathbuf));
    strlcat (pathbuf, result->d_name, sizeof (pathbuf));
    if (efs_stat (pathbuf, &stat_buf) == -1)
    {
      /* Unable to stat the file or directory. Return an error. Check if there
       * is a better error we can return. */
      status = FS_NONEXISTENT_DIR_S;
      goto list_end;
    }

    if (((file_type == FS_ENUM_FILES) && S_ISREG (stat_buf.st_mode))
       || ((file_type == FS_ENUM_DIRECTORIES) && S_ISDIR (stat_buf.st_mode)))
    {
      copy_index = 0;
      while (list_buf_index < buf_limit) {
        list_buf[list_buf_index++] = pathbuf[copy_index++];
        if (pathbuf[copy_index] == 0x00)
        {
          /* Done with this file name */
          list_buf[list_buf_index++] = 0x00;
          break;
        }
      }
    }
    if (list_buf_index >= buf_limit)
    {
      break;
    }
  }

  /* Adjust index so that a name is not truncated */
  while ((list_buf_index != 0) && (list_buf[--list_buf_index] != 0x00)) {}
  list_buf_index++;
  list_buf[list_buf_index++] = 0x00;

  /* Set return parameter to number of bytes copied */
  if (file_type == FS_ENUM_FILES)
  {
    cmd_ptr->rsp_msg->list_files.count = list_buf_index;
  }
  else
  {
    cmd_ptr->rsp_msg->list_dirs.count = list_buf_index;
  }

list_end:
  if (dir_stream_ptr != NULL)
    (void) efs_closedir (dir_stream_ptr);
  return status;
} /* END fs_compat_list_files_or_dirs */

static fs_status_type
fs_compat_num_files_or_dirs (
  fs_cmd_type *cmd_ptr,
  fs_enum_type file_type)
{
  EFSDIR           *dir_stream_ptr;
  struct fs_dirent *result;
  struct fs_stat    stat_buf;
  int               num_entries = 0;
  char             *dirname;
  fs_status_type    status = FS_OKAY_S;

  if (file_type == FS_ENUM_FILES)
  {
    dirname = cmd_ptr->fs_op.parms.num_files.dirname;
  }
  else
  {
    dirname = cmd_ptr->fs_op.parms.num_dirs.dirname;
  }

  dir_stream_ptr = efs_opendir (dirname);
  if (dir_stream_ptr == NULL)
  {
    status = FS_NONEXISTENT_DIR_S;
    goto num_end;
  }

  /* The first two entries returned will be "." and "..". Ignore these.
   */
  result = efs_readdir (dir_stream_ptr);
  if (result == NULL)
  {
    /* This is bad. A directory without a . entry???? */
    status = FS_NONEXISTENT_DIR_S;
    goto num_end;
  }
  result = efs_readdir (dir_stream_ptr);
  if (result == NULL)
  {
    /* This is bad. A directory without a .. entry???? */
    status = FS_NONEXISTENT_DIR_S;
    goto num_end;
  }

  while (1) {
    result = efs_readdir (dir_stream_ptr);
    if (result == NULL)
    {
      break;
    }

    if (strlen (dirname) + 1 + strlen (result->d_name)
        > FS_FILENAME_MAX_LENGTH)
    {
      /* Pathname too long. Maybe return a better error here? */
      status = FS_NONEXISTENT_DIR_S;
      goto num_end;
    }

    strlcpy (pathbuf, dirname, sizeof (pathbuf));
    strlcat (pathbuf, "/", sizeof (pathbuf));
    strlcat (pathbuf, result->d_name, sizeof (pathbuf));
    if (efs_stat (pathbuf, &stat_buf) == -1)
    {
      /* Unable to stat file. Maybe return a better error here? */
      status = FS_NONEXISTENT_DIR_S;
      goto num_end;
    }

    if (((file_type == FS_ENUM_FILES) && S_ISREG (stat_buf.st_mode))
       || ((file_type == FS_ENUM_DIRECTORIES) && S_ISDIR (stat_buf.st_mode)))
    {
      num_entries++;
    }
  }

  if (file_type == FS_ENUM_FILES)
  {
    cmd_ptr->rsp_msg->num_files.num_files = num_entries;
  }
  else
  {
    cmd_ptr->rsp_msg->num_dirs.num_dirs = num_entries;
  }

num_end:
  if (dir_stream_ptr != NULL)
    (void) efs_closedir (dir_stream_ptr);
  return status;
} /* END fs_compat_num_files_or_dirs */

/***********************************************************************
 * FUNCTION      fs_compat_num_files
 *
 * DESCRIPTION   Handles the FS_NUM_FILES_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_num_files (fs_cmd_type *cmd_ptr)
{
  return fs_compat_num_files_or_dirs (cmd_ptr, FS_ENUM_FILES);
} /* END fs_compat_num_files */

/***********************************************************************
 * FUNCTION      fs_compat_num_dirs
 *
 * DESCRIPTION   Handles the FS_NUM_DIRS_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_num_dirs (fs_cmd_type *cmd_ptr)
{
  return fs_compat_num_files_or_dirs (cmd_ptr, FS_ENUM_DIRECTORIES);
} /* END fs_compat_num_dirs */

/***********************************************************************
 * FUNCTION      fs_compat_list_files
 *
 * DESCRIPTION   Handles the FS_LIST_FILES_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_list_files (fs_cmd_type *cmd_ptr)
{
  return fs_compat_list_files_or_dirs (cmd_ptr, FS_ENUM_FILES);
} /* END fs_compat_list_files */

/***********************************************************************
 * FUNCTION      fs_compat_list_dirs
 *
 * DESCRIPTION   Handles the FS_LIST_DIRS_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_list_dirs (fs_cmd_type *cmd_ptr)
{
  return fs_compat_list_files_or_dirs (cmd_ptr, FS_ENUM_DIRECTORIES);
} /* END fs_compat_list_dirs */

/* Enumeration operations.  POSIX and the EFS have differing semantics
 * for directory enumeration.  With EFS, you specify a
 * directory name and whether you want directories, files, or both.  It
 * then returns the first name, if present.  There are next and prev
 * operators for iterating through the directory.  The behavior is
 * undefined if the directory contents are changed while iterating.
 *
 * POSIX provides the opendir, readdir, rewinddir and closedir operations.
 * Also, the specific data is larger than for the EFS.  Rather than require
 * the EFS iterator data block to be large enough to contain the POSIX data
 * as well, we keep a local copy here.  This could be thought of as a cache.
 * This also handles the case of going in reverse, a reverse lookup is
 * simply a new search. */

typedef struct {
  EFSDIR                *dir_ptr;    /* Pointer to POSIX dir stream.      */
  struct fs_dirent      *dirent_ptr; /* Pointer to entry in dir stream    */
  dword                  index;      /* Which file are we "on".           */
  fs_enum_iterator_type *iterator;   /* Iterator used to match operation. */
} local_fscompat_iterate_t;

static local_fscompat_iterate_t idata;

static void
closeup_iterator (void)
{
  if (idata.dir_ptr)
  {
    efs_closedir (idata.dir_ptr);
    idata.dir_ptr    = NULL;
    idata.dirent_ptr = NULL;
  }
} /* END closeup_iterator */

/* Open the given iterator.  This does not seek or any such thing. */
static fs_status_type
open_iterator (fs_enum_iterator_type *iterator)
{
  EFSDIR *dir_stream_ptr;

  if (iterator->rootname[0] == 0)
  {
    dir_stream_ptr = efs_opendir ("/");
  }
  else
  {
    dir_stream_ptr = efs_opendir ((char *) iterator->rootname);
  }
  if (!dir_stream_ptr)
  {
    return FS_NONEXISTENT_DIR_S;
  }

  /* The first two entries in any directory are "." and "..". Ignore
   * these entries. */
  if (efs_readdir (dir_stream_ptr) == NULL)
  {
    return FS_NONEXISTENT_DIR_S;
  }
  if ((idata.dirent_ptr = efs_readdir (dir_stream_ptr)) == NULL)
  {
    return FS_NONEXISTENT_DIR_S;
  }

  idata.dir_ptr    = dir_stream_ptr;
  idata.index      = 0;
  idata.iterator   = iterator;
  return FS_OKAY_S;

} /* END open_iterator */

/* Given a particular iterator, "activate" it.  This makes sure that this
 * iterator is the open iterator, and that it is advanced properly to the
 * entry just before the one we want.  Note that all directory entries are
 * numbered, even the hidden ones that we do not see at the top level.  0
 * is the space before the first entry. */

static fs_status_type
setup_iterator (fs_enum_iterator_type *iterator)
{
  fs_status_type it_result;

  /* First, check if we are already set up exactly like we want to be. */
  if (idata.dir_ptr
      && (idata.iterator == iterator)
      && (idata.index == iterator->fsequence))
  {
    return FS_OKAY_S;
  }

  /* Next, check if we are on the previous entry.  This handles the normal
   * case of consecutive directory reads. */
  if (idata.dir_ptr
      && (idata.iterator == iterator)
      && (idata.index + 1 == iterator->fsequence))
  {
    idata.dirent_ptr = efs_readdir (idata.dir_ptr);
    idata.index++;

    if (idata.dirent_ptr == NULL)
    {
      closeup_iterator ();
      return FS_ENUM_DONE_S;
    }
    else
    {
      return FS_OKAY_S;
    }
  }

  /* Otherwise, this iterator is not correct, close any current iterator
   * down, and start over. */
  closeup_iterator ();

  it_result = open_iterator (iterator);
  if (it_result != FS_OKAY_S)
    return it_result;

  /* Now we must seek until we are at the correct position. */
  while (idata.index < iterator->fsequence) {
    idata.index++;
    idata.dirent_ptr = efs_readdir (idata.dir_ptr);

    if (idata.dirent_ptr == NULL)
    {
      /* Close the iterator to force failure in the future. */
      closeup_iterator ();
      return FS_ENUM_DONE_S;
    }
  }

  return FS_OKAY_S;
} /* END setup_iterator */

/***********************************************************************
 * FUNCTION      fs_compat_enum_init
 *
 * DESCRIPTION   Handles the FS_ENUM_INIT_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_enum_init (fs_cmd_type *cmd_ptr)
{
  fs_enum_iterator_type *it = cmd_ptr->fs_op.parms.enum_init.iterator_ptr;

  /* XXX: confirm that this is the correct return value. */
  if (strlen (cmd_ptr->fs_op.parms.enum_init.dirname) >=
      sizeof (it->rootname))
  {
    return FS_FAIL_S;
  }

  /* Init just copies over the information. */
  strlcpy ((char *) it->rootname, cmd_ptr->fs_op.parms.enum_init.dirname,
      sizeof (it->rootname));
  it->enum_kind = cmd_ptr->fs_op.parms.enum_init.enum_kind;
  it->fsequence = 0;
  it->remote_file_processed_index = 0;

  return FS_OKAY_S;
} /* END fs_compat_enum_init */

static fs_status_type
fs_compat_enum_move (fs_cmd_type *cmd_ptr, int direction)
{
  fs_status_type         result;
  fs_enum_data_type     *data_ptr;
  fs_enum_iterator_type *iterator;
  int                    finished = 0;
  int                    root_length;
  int                    our_length;
  dword                  last_good_sequence;
  struct fs_stat         stat_buf;

  iterator = cmd_ptr->fs_op.parms.enum_next.iterator_ptr;
  data_ptr = cmd_ptr->fs_op.parms.enum_next.data_ptr;

  /* Remember the current sequence position and back up to it if we run off
   * the end. */
  last_good_sequence = iterator->fsequence;

  do {
    if (direction < 0)
    {
      if (iterator->fsequence == 0)
      {
        /* Restore the sequence number. */
        iterator->fsequence = last_good_sequence;
        return FS_ENUM_DONE_S;
      }
    }
    iterator->fsequence += direction;

    result = setup_iterator (iterator);

    if (result == FS_ENUM_DONE_S)
    {
      /* We "fell" off the end, so backup the iterator. */
      iterator->fsequence = last_good_sequence;
    }

    if (result != FS_OKAY_S)
      return result;

    if (strlen ((char *) iterator->rootname) + 1 +
        strlen (idata.dirent_ptr->d_name) > FS_FILENAME_MAX_LENGTH)
    {
      // ERR_FATAL ("Figure out what to do here", 0, 0, 0);
      return FS_BAD_FILE_NAME_S;
    }

    if (FS_MASK_CHECK (idata.dirent_ptr->d_stats_present,
          FS_DIRENT_HAS_TYPE |
          FS_DIRENT_HAS_ST_SIZE))
    {
      stat_buf.st_mode = idata.dirent_ptr->d_stat.st_mode;
      stat_buf.st_size = idata.dirent_ptr->d_stat.st_size;
    } else {
      strlcpy (pathbuf, (char *) iterator->rootname, sizeof (pathbuf));
      strlcat (pathbuf, "/", sizeof (pathbuf));
      strlcat (pathbuf, idata.dirent_ptr->d_name, sizeof (pathbuf));
      if (efs_stat (pathbuf, &stat_buf) == -1)
      {
        // ERR_FATAL ("Figure out what to do here", 0, 0, 0);
        return FS_BAD_FILE_NAME_S;
      }
    }

    strlcpy (data_ptr->fullname, idata.dirent_ptr->d_name,
        sizeof (data_ptr->fullname));

    data_ptr->fullname_length = strlen (data_ptr->fullname);

    /* We need to determine if we are done or not. */
    if (iterator->enum_kind == FS_ENUM_FILES)
    {
      if (S_ISREG (stat_buf.st_mode))
      {
        finished = 1;
      }
    }
    else if (S_ISDIR (stat_buf.st_mode))
    {
      /* Directory. */
      finished = 1;
      data_ptr->dirname_length = 0;
    }
  } while (!finished);

  /* Fixup the pathname.  Prepend the rootname to our current name, and
   * then call the access manager to fixup the entire name. */
  root_length = strlen ((char *) iterator->rootname);
  our_length  = strlen (data_ptr->fullname);

  if (root_length + our_length + 1 + 1 > FS_FILENAME_MAX_LENGTH)
  {
    /* Hmm.  What is best to do here.  Let's log an error, so at
     * least this can be debugged. */
    FS_ERR ("fs_compat_enum_next: filename too long", 0, 0, 0);
    return FS_BAD_FILE_NAME_S;
  }

  fs_memsmove (data_ptr->fullname + root_length + 1,
               sizeof (data_ptr->fullname) - (root_length + 1),
               data_ptr->fullname, our_length + 1);
  fs_memsmove (data_ptr->fullname, sizeof (data_ptr->fullname),
               iterator->rootname, root_length);
  data_ptr->fullname[root_length] = '/';

  data_ptr->fullname_length = root_length + our_length + 1;
  data_ptr->dirname_length  = root_length + 1;
  data_ptr->logical_size = stat_buf.st_size;

  /* Now fixup the name that the user sees. */
  fs_am_fixup_enum (data_ptr, iterator);

  return FS_OKAY_S;
} /* END fs_compat_enum_move */

/***********************************************************************
 * FUNCTION      fs_compat_enum_next
 *
 * DESCRIPTION   Handles the FS_ENUM_NEXT_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_enum_next (fs_cmd_type *cmd_ptr)
{
  return fs_compat_enum_move (cmd_ptr, 1);
} /* END fs_compat_enum_next */

/***********************************************************************
 * FUNCTION      fs_compat_enum_prev
 *
 * DESCRIPTION   Handles the FS_ENUM_PREV_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_enum_prev (fs_cmd_type *cmd_ptr)
{
  return fs_compat_enum_move (cmd_ptr, -1);
} /* END fs_compat_enum_prev */

/***********************************************************************
 * FUNCTION      fs_compat_enum_finish
 *
 * DESCRIPTION   Handles the FS_ENUM_FINISH_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_enum_finish (fs_cmd_type *cmd_ptr)
{
  (void) cmd_ptr;

  closeup_iterator ();

  return FS_OKAY_S;
} /* END fs_compat_enum_finish */

/***********************************************************************
 * FUNCTION      fs_compat_statfs
 *
 * DESCRIPTION   Handles the FS_STATFS_OP command.
 *
 ***********************************************************************/
static fs_status_type
fs_compat_statfs (fs_cmd_type *cmd_ptr)
{
  statfs_info      *info = cmd_ptr->fs_op.parms.statfs.info;
  struct fs_statvfs efs_info;

  if (efs_statvfs (cmd_ptr->fs_op.parms.statfs.filename, &efs_info) == 0)
  {
    /* XXX: The f_type field setting is not correct. Ensure that the
     * other fields are being set correctly.
     */
    info->f_type    = STATFS_TYPE_FLASH;
    info->f_bsize   = efs_info.f_bsize;
    info->f_blocks  = efs_info.f_blocks;
    info->f_bfree   = efs_info.f_bfree;
    info->f_bavail  = efs_info.f_bavail;
    info->f_files   = efs_info.f_files;
    info->f_ffree   = efs_info.f_ffree;
    info->f_fsid    = efs_info.f_fsid;
    info->f_namelen = efs_info.f_namemax;
    return FS_OKAY_S;
  }
  else
    return FS_FAIL_S;
} /* END fs_compat_statfs */

/***********************************************************************
 * FUNCTION      fs_compat_process_commands
 *
 * DESCRIPTION   Top-level command processing function. Calls the
 *               appropriate handler depending on the operation code.
 *
 ***********************************************************************/
static void
fs_compat_process_commands (void)
{
  fs_cmd_type     *cmd_ptr;
  fs_os_tcb_type    *task_ptr;
  fs_os_sigs_type    task_sigs;
  fs_status_type   status;

  while ((cmd_ptr = (fs_cmd_type *) q_get (&fs_compat_queue)) != NULL) {

    switch (cmd_ptr->fs_op.op) {
      case FS_OPEN_OP:
        cmd_ptr->rsp_msg->open.handle = fs_compat_open (cmd_ptr);
        status = cmd_ptr->fs_op.status;
        cmd_ptr->rsp_msg->open.status = status;
        break;

      case FS_WRITE_OP:
      case FS_RSVD_WRITE_OP:
        /* Eventually, we may want to break this request up into pieces so
         * that other operations may proceed. */
        status = fs_compat_write (cmd_ptr);
        cmd_ptr->rsp_msg->write.status = status;
        break;

      case FS_READ_OP:
        /* May want to break this into pieces so that other operations may
         * be interleaved. */
        status = fs_compat_read (cmd_ptr);
        cmd_ptr->rsp_msg->read.status = status;
        break;

      case FS_CLOSE_OP:
        status = fs_compat_close (cmd_ptr);
        cmd_ptr->rsp_msg->close.status = status;
        break;

      case FS_TELL_OP:
        status = fs_compat_tell (cmd_ptr);
        cmd_ptr->rsp_msg->tell.status = status;
        break;

      case FS_FILE_SIZE_OP:
        status = fs_compat_file_size (cmd_ptr);
        cmd_ptr->rsp_msg->file_size.status = status;
        break;

      case FS_RMFILE_OP:
        status = fs_compat_rmfile (cmd_ptr);
        cmd_ptr->rsp_msg->file_size.status = status;
        break;

      case FS_RENAME_OP:
        status = fs_compat_rename (cmd_ptr);
        cmd_ptr->rsp_msg->rename.status = status;
        break;

      case FS_TRUNCATE_OP:
        status = fs_compat_truncate (cmd_ptr);
        cmd_ptr->rsp_msg->truncate.status = status;
        break;

      case FS_GET_ATTRIBS_OP:
        status = fs_compat_get_attribs (cmd_ptr);
        cmd_ptr->rsp_msg->get_attribs.status = status;
        break;

      case FS_SET_ATTRIBS_OP:
        status = fs_compat_set_attribs (cmd_ptr);
        cmd_ptr->rsp_msg->set_attribs.status = status;
        break;

      case FS_FMEMSET_OP:
        status = fs_compat_fmemset (cmd_ptr);
        cmd_ptr->rsp_msg->fmemset.status = status;
        break;

      case FS_SEEK_OP:
        status = fs_compat_seek (cmd_ptr);
        cmd_ptr->rsp_msg->seek.status = status;
        break;

      case FS_MKDIR_OP:
        status = fs_compat_mkdir (cmd_ptr);
        cmd_ptr->rsp_msg->mkdir.status = status;
        break;

      case FS_RMDIR_OP:
        status = fs_compat_rmdir (cmd_ptr);
        cmd_ptr->rsp_msg->rmdir.status = status;
        break;

      case FS_NAMETEST_OP:
        status = fs_compat_nametest (cmd_ptr);
        cmd_ptr->rsp_msg->nametest.status = status;
        break;

      case FS_FORMAT_OP:
        status = fs_compat_format (cmd_ptr);
        cmd_ptr->rsp_msg->format.status = status;
        break;

      case FS_NUM_FILES_OP:
        status = fs_compat_num_files (cmd_ptr);
        cmd_ptr->rsp_msg->num_files.status = status;
        break;

      case FS_NUM_DIRS_OP:
        status = fs_compat_num_dirs (cmd_ptr);
        cmd_ptr->rsp_msg->num_dirs.status = status;
        break;

      case FS_LIST_FILES_OP:
        status = fs_compat_list_files (cmd_ptr);
        cmd_ptr->rsp_msg->list_files.status = status;
        break;

      case FS_LIST_DIRS_OP:
        status = fs_compat_list_dirs (cmd_ptr);
        cmd_ptr->rsp_msg->list_dirs.status = status;
        break;

      case FS_ENUM_INIT_OP:
        status = fs_compat_enum_init (cmd_ptr);
        cmd_ptr->rsp_msg->enum_init.status = status;
        break;

      case FS_ENUM_NEXT_OP:
        status = fs_compat_enum_next (cmd_ptr);
        cmd_ptr->rsp_msg->enum_next.status = status;
        break;

      case FS_ENUM_PREV_OP:
        status = fs_compat_enum_prev (cmd_ptr);
        cmd_ptr->rsp_msg->enum_prev.status = status;
        break;

      case FS_ENUM_FINISH_OP:
        status = fs_compat_enum_finish (cmd_ptr);
        cmd_ptr->rsp_msg->enum_finish.status = status;
        break;

      case FS_SEEK_READ_OP:
        status = fs_compat_seek_read (cmd_ptr);
        cmd_ptr->rsp_msg->seek_read.status = status;
        break;

      case FS_SEEK_WRITE_OP:
        status = fs_compat_seek_write (cmd_ptr);
        cmd_ptr->rsp_msg->seek_write.status = status;
        break;

      case FS_STATFS_OP:
        status = fs_compat_statfs (cmd_ptr);
        cmd_ptr->rsp_msg->statfs.status = status;
        break;

      default:
        cmd_ptr->rsp_msg->open.status = FS_UNKNOWN_OPERATION_S;
        break;
    }

    fs_am_cleanup_after_command (cmd_ptr);

    /* Let them know it is finished. */
    task_ptr  = (fs_os_tcb_type *)(cmd_ptr->tcb_ptr); /*lint !e740*/
    task_sigs = cmd_ptr->sigs;

    if (task_ptr == NULL)
    {
      cmd_ptr->callback_p (cmd_ptr->rsp_msg);
    }
    else
    {
      (void) fs_os_set_sigs (task_ptr, task_sigs);
    }

    /* Restore the buffer to available. */
    fsi_free_cmd_buffer (cmd_ptr->index);
  }
} /* END fs_compat_process_commands */

/*===========================================================================

FUNCTION FS_COMPAT_TASK

DESCRIPTION
  This is the task that handles the compatibility interface.  The task enters
  an infinite loop, awaiting commands received  on its command queue.  Each
  command received is processed to completion  before the next command is
  processed. While in the main loop, the task kicks the watchdog periodically.

===========================================================================*/

static void
fs_compat_task
(
  dword         parm            /* from REX - ignored. */
)
{
  fs_os_sigs_type task_sigs;

  (void) parm;

  // FS_MSG_MED ("Starting FS Compatibility task", 0, 0, 0);

  fs_os_def_timer (&fs_compat_rpt_timer, &fs_compat_tcb,
      FS_COMPAT_RPT_TIMER_SIG);
  fs_os_def_timer (&fs_cache_sync_timer, &fs_compat_tcb,
      FS_COMPAT_CACHE_SYNC_SIG);

  /* Loop forever handling commands. */
  while (1) {
    fs_os_clr_sigs (&fs_compat_tcb, FS_CMD_Q_SIG);
    fs_compat_process_commands ();

    task_sigs = fs_os_wait (FS_COMPAT_RPT_TIMER_SIG |
                            FS_COMPAT_CACHE_SYNC_SIG |
                            FS_CMD_Q_SIG |
                            TASK_STOP_SIG |
                            TASK_OFFLINE_SIG);

    /* Check if we need to kick. */
    if ((task_sigs & FS_COMPAT_RPT_TIMER_SIG) != 0)
    {
      (void) fs_os_clr_sigs (&fs_compat_tcb, FS_COMPAT_RPT_TIMER_SIG);
    }

    /* If asked to go offline, acknowledge, and continue. */
    if ((task_sigs & TASK_OFFLINE_SIG) != 0)
    {
      (void) fs_os_clr_sigs (&fs_compat_tcb, TASK_OFFLINE_SIG);
#ifdef FEATURE_EFS_FTL
      ftl_cache_force_sync_all ();
#endif
      task_offline ();
    }

    /* If asked to powerdown, acknowledge. */
    if ((task_sigs & TASK_STOP_SIG) != 0)
    {
      (void) fs_os_clr_sigs (&fs_compat_tcb, TASK_STOP_SIG);
#ifdef FEATURE_EFS_FTL
      ftl_cache_force_sync_all ();
#endif
      task_stop ();
    }

    /* First let's kick watchdog, sync might take long time */
    FS_COMPAT_KICK_WATCHDOG ();

    if ((task_sigs & FS_COMPAT_CACHE_SYNC_SIG) != 0)
    {
      (void) fs_os_clr_sigs (&fs_compat_tcb, FS_COMPAT_CACHE_SYNC_SIG);

#ifdef FEATURE_EFS_FTL
      if (ftl_cache_periodic_sync () != 0)
      {
        fs_os_set_timer (&fs_cache_sync_timer, FTL_CACHE_SYNC_TIME);
      }
#endif
    }

  }
} /* END fs_compat_task */

/*===========================================================================

FUNCTION FS_COMPAT_OPS_CMD

DESCRIPTION
  This is the actual real entry point from EFS.  The EFS calls this
  routine to add an entry to our queue.

===========================================================================*/
void
fs_compat_ops_cmd (fs_cmd_type *cmd_ptr)
{
  /* Set the request status to busy.  Initialize and link the command onto
   * the command queue, send a signal to the compatibility task and return. */

  cmd_ptr->status = FS_BUSY_S;
  cmd_ptr->rsp_msg->open.status = FS_BUSY_S;
  (void) q_link (cmd_ptr, &cmd_ptr->link);
  q_put (&fs_compat_queue, &cmd_ptr->link);

  (void) fs_os_set_sigs (&fs_compat_tcb, FS_CMD_Q_SIG);

  return;
} /* END fs_compat_ops_cmd */

/*===========================================================================

FUNCTION FS_COMPAT_INIT

DESCRIPTION
   Initialization function for this module. This needs to be called
   before the rest of this module is used. Initializes some local data
   structures and starts up the fs_compat task.

===========================================================================*/
void
fs_compat_init (void)
{
  static int init_complete = 0;

  if (!init_complete)
  {
    /* Initialize. */
    (void) q_init (&fs_compat_queue);

    /* Mark the iterator as inactive */
    idata.dir_ptr = NULL;

#ifdef TIMETEST_FS_COMPAT_TASK_ID
    fs_compat_tcb.leds = TIMETEST_FS_COMPAT_TASK_ID;
#endif

    /* Start up the fs_compat task */
    fs_os_def_task_ext (&fs_compat_tcb,
                        fs_compat_stack,
                        FS_COMPAT_STACK_SIZ,
                        FS_COMPAT_PRI,
                        fs_compat_task,
                        0,
                        "FS Compat Task",
                        0,
                        DOG_FSCOMPAT_RPT);
    init_complete = 1;
  }
  return;
} /* END fs_compat_init */

#endif /* FEATURE_EFS_ACCESS_METHODS */

#else /* FEATURE_EFS_COMPATIBILITY */

/* Keep lint happy. */
extern int  __dont_complain_about_empty_file;

#endif /* FEATURE_EFS_COMPATIBILITY */
