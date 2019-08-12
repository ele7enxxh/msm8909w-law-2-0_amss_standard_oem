/*==============================================================================

          MODEM_CFG  -     File System  Wrapper API 

GENERAL DESCRIPTION
   Simple wrapper APIs to access Modem Configuration Items (incl Multi SIM support)
 
EXTERNALIZED FUNCTIONS
  mcfg_fs_read
  mcfg_fs_write

INITIALIZATION AND SEQUENCING REQUIREMENTS
  EFS must be initialized

Copyright (c) 2014-2015 by QUALCOMM Technologies Incorporated.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_nv/src/mcfg_fs.c#1 $
$DateTime: 2016/12/13 08:00:27 $
$Author: mplcsds1 $
$Change: 11985219 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------------- 
02/17/15 sbt  Initial creation

==============================================================================*/

/*==============================================================================

                      INCLUDE FILES FOR MODULE

==============================================================================*/

#include <stringl/stringl.h>
#include "fs_errno.h"
#include "fs_public.h"
#include "mcfg_fs.h"
#include "mcfg_int.h"

/*==============================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Private Constants and Macros
==============================================================================*/

/*==============================================================================
  Private Typedefs
==============================================================================*/


/*==============================================================================

                    FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/


/*==============================================================================
  Functions for internal module use only
==============================================================================*/

/*==========================================================================

  FUNCTION mcfg_fs_build_path

  DESCRIPTION
    This function turns an EFS path to a subs specific path to be used by 
    EFS. Performs validation of the sub_id and supported fs_type.

  DEPENDENCIES
    path_len must be <= MCFG_FS_MAX_EFS_PATH_LEN

  PARAMETERS
    fs_type        [in] FS type 
    subs_id        [in] subscription index
    base_path      [in] Base path (null terminated)
    subs_path      [out] Subscription specific path (null terminated)
    subs_path_len  [in] path length 
    
    
  RETURN VALUE
    MCFG_FS_STATUS_OK on success
    MCFG_NV_STATUS_INVALID_PARAM if subs is outside limit
    MCFG_FS_STATUS_UNSUPPORTED if FS type is not supported

  SIDE EFFECTS
    None.

===========================================================================*/
static mcfg_fs_status_e_type mcfg_fs_build_path(
  mcfg_fs_e_type    fs_type,
  mcfg_fs_sub_id_e_type    subs_id,
  const char *base_path,
  char* subs_path,
  uint32    subs_path_len
  )
{
  uint32 path_offset;
  mcfg_fs_status_e_type status;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  path_offset = strlen(base_path);
  status = MCFG_FS_STATUS_OK;

  if(subs_id >= MCFG_FS_NUM_OF_SUB_ID)
  {
    return MCFG_FS_STATUS_INVALID_PARAM; 
  }

  if (fs_type > MCFG_FS_TYPE_EFS)
  {
    return MCFG_FS_STATUS_UNSUPPORTED; 
  }

  memscpy(subs_path, subs_path_len, base_path, path_offset + 1);

  if(subs_id > MCFG_FS_SUBID_0)
  {
     path_offset += snprintf(&subs_path[path_offset], 
                             subs_path_len - path_offset,
                             "_Subscription%02d", 
                             (int)subs_id);

     subs_path[path_offset] = '\0';
  }

  return status;
} /* mcfg_fs_build_path() */

/*==============================================================================
  Functions exported in public header files
==============================================================================*/

/*===========================================================================
FUNCTION  mcfg_fs_write() 

DESCRIPTION
  This creates/updates the EFS item file and uses subs_id for multi-SIM NV items
 
DEPENDENCIES
  Requires core service tasks like NV and EFS be available 

PARAMETERS
 @param path
  The path of the item-file.

 @param data_ptr
  The start-address of the data.

 @param length
  The length indicates the size of the buffer 'data' in bytes.

 @param oflag
  'oflag' should be a bitwise OR of one of the following:\n
    MCFG_FS_O_CREAT   - Create the item if it does not exist.\n
    MCFG_FS_O_AUTODIR - Auto create parent directories.\n
    other flags (MCFG_FS_O_APPEND, MCFG_FS_O_TRUNC, O_EXCL) will have no effect.\n

  Recommended settigns for EFS files: MCFG_O_CREAT | MCFG_O_AUTODIR 

 @param mode
  'mode' will set the initial permission bits of the item file,
  Recommended settings for EFS Files MCFG_ALLPERMS 

 @param fs_type
  File system type = 0 for EFS, other values reserved for furture use

 @param sub_id
 Subscription ID or context ID starting with 0, the primary context
 
RETURN VALUE: mcfg_fs_e_type 
 
==============================================================================*/
mcfg_fs_status_e_type mcfg_fs_write(
   const char *path, 
   void *data_ptr, 
   uint32 length,
   int oflag, 
   int mode, 
   mcfg_fs_e_type fs_type, 
   mcfg_fs_sub_id_e_type sub_id )
{
  mcfg_fs_status_e_type status; 
  int  efs_ret;
  char f_name[MCFG_FS_MAX_PATH_LENGTH];

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (data_ptr == NULL)
  {
    status = MCFG_FS_STATUS_NULL_PTR;
  }
  else
  {
    status = mcfg_fs_build_path(MCFG_FS_TYPE_EFS, sub_id, path, f_name, MCFG_FS_MAX_PATH_LENGTH);

    f_name[MCFG_FS_MAX_PATH_LENGTH-1] = '\0';
    //MCFG_MSG_LOW_2("mcfg_fs_write -  sub_id: 0x%0x, filepath %s", sub_id, f_name);

    if (status == MCFG_FS_STATUS_OK)
    {
      efs_ret = efs_put(f_name, data_ptr, length, oflag, mode);
      if (efs_ret == MCFG_FS_ERROR)
      {
        status = MCFG_FS_STATUS_EFS_ERR;
      }
    } /* status == MCFG_FS_STATUS_OK */
  }

  return status;
} /* mcfg_fs_write() */


/*==========================================================================

  FUNCTION mcfg_fs_read

  DESCRIPTION
    Wrapper API to read EFS item from modem

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
 
 @param path
  The path of the file.

 @param data
  The start-address of the data.

 @param length
  The length indicates the size of the buffer 'data' in bytes.

 @return
  Returns mcfg_fs_status_e_type.

   RETURN VALUE
    MCFG_FS_STATUS_OK on success
    MCFG_FS_STATUS_INVALID_PARAM if subs_id incorrect
    MCFG_FS_STATUS_NOT_ACTIVE if item doesn't exist
    MCFG_NV_STATUS_EFS_ERR on EFS error
    MCFG_FS_STATUS_NULL_PTR if data_ptr is null

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies
 
 
==============================================================================*/
mcfg_fs_status_e_type mcfg_fs_read (
   const char *path, 
   void *data_ptr, 
   uint32 length, 
   mcfg_fs_e_type fs_type, 
   mcfg_fs_sub_id_e_type sub_id )

{
  mcfg_fs_status_e_type status; 
  int  efs_ret;
  char f_name[MCFG_FS_MAX_PATH_LENGTH];

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  //MCFG_MSG_LOW_1("mcfg_fs_read -  sub_id: 0x%0x", sub_id);
  if (data_ptr == NULL)
  {
    status = MCFG_FS_STATUS_NULL_PTR;
  }
  else
  {
    status = mcfg_fs_build_path(MCFG_FS_TYPE_EFS, sub_id, path, f_name, MCFG_FS_MAX_PATH_LENGTH);
    f_name[MCFG_FS_MAX_PATH_LENGTH-1] = '\0';
    //MCFG_MSG_LOW_2("mcfg_fs_read -  sub_id: 0x%0x, filepath %s", sub_id, f_name);

    if (status == MCFG_FS_STATUS_OK)
    {
      efs_ret = efs_get(f_name, data_ptr, length);
      if (efs_ret == MCFG_FS_ERROR)
      {
        switch (efs_errno)
        {
          case ENOENT:
            status = MCFG_FS_STATUS_NOT_ACTIVE;
            break;

          case FS_ERANGE:
            status = MCFG_FS_STATUS_BAD_SIZE;
            break;

          default:
            status = MCFG_FS_STATUS_EFS_ERR;
        }
      }
      else if (efs_ret != length)
      {
        status = MCFG_FS_STATUS_BAD_SIZE;
      }
    }
  }

  return status;
} /* mcfg_fs_read() */


/*==========================================================================

  FUNCTION mcfg_fs_delete

  DESCRIPTION
    This function deletes an item from the EFS.

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    path      [in] EFS item or file to
    fs_type    in] file system type
    subs_id   [in] Subscription ID for the Multi-SIM NV item to delete
                   or 0 if not a multi-SIM NV (if unsure, use 0)
 
  RETURN VALUE
    MCFG_NV_STATUS_OK on success or if file does not exist
    MCFG_NV_STATUS_BAD_ITEM if item is not in old NV or new MCFG NV range,
        or index is outside defined limit (new-style items only)
    MCFG_NV_STATUS_EFS_ERR on EFS error

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies

===========================================================================*/
mcfg_fs_status_e_type mcfg_fs_delete(
   const char *path, 
   mcfg_fs_e_type fs_type, 
   mcfg_fs_sub_id_e_type sub_id 
   )
{
  mcfg_fs_status_e_type status; 
  int  efs_ret;
  char f_name[MCFG_FS_MAX_PATH_LENGTH];

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  status = mcfg_fs_build_path(MCFG_FS_TYPE_EFS, sub_id, path, f_name, MCFG_FS_MAX_PATH_LENGTH);
  f_name[MCFG_FS_MAX_PATH_LENGTH-1] = '\0';

  if (status == MCFG_FS_STATUS_OK)
  {
    //MCFG_MSG_LOW_2("mcfg_fs_delete unlink -  sub_id: 0x%0x, filepath %s", sub_id, f_name);
    efs_ret = efs_unlink(f_name);

    /* Only error if deleting failed - if the file didn't exist in the first 
     * place then we are fine */
    if (efs_ret == -1 && efs_errno != ENOENT)
    {
      status = MCFG_FS_STATUS_EFS_ERR;
    }
  }

  return status;
} /* mcfg_fs_delete() */


/*==========================================================================

  FUNCTION mcfg_fs_stat

  DESCRIPTION
    This returns the information about the file given its path, uses sub_id
    to determine the correct path

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    path      [in] EFS item or file to
    buf       [out] Pointer to the fs_stat structure where the
              return values are expected
    fs_type    in] file system type
    subs_id   [in] Subscription ID for the Multi-SIM NV item to delete
 
  RETURN VALUE
    MCFG_NV_STATUS_OK on success or if file does not exist
    MCFG_NV_STATUS_BAD_ITEM if item is not in old NV or new MCFG NV range,
        or index is outside defined limit (new-style items only)
    MCFG_NV_STATUS_EFS_ERR on EFS error

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies

===========================================================================*/
mcfg_fs_status_e_type mcfg_fs_stat (
   const char *path, 
   struct fs_stat *buf,
   mcfg_fs_e_type fs_type, 
   mcfg_fs_sub_id_e_type sub_id 
   )
{
  mcfg_fs_status_e_type status; 
  int  efs_ret;
  char f_name[MCFG_FS_MAX_PATH_LENGTH];

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  status = mcfg_fs_build_path(MCFG_FS_TYPE_EFS, sub_id, path, f_name, MCFG_FS_MAX_PATH_LENGTH);
  f_name[MCFG_FS_MAX_PATH_LENGTH-1] = '\0';

  if (status == MCFG_FS_STATUS_OK)
  {
    efs_ret = efs_stat(f_name, buf);
    if (efs_ret == MCFG_FS_ERROR)
    {
      status = MCFG_FS_STATUS_EFS_ERR;
    }
  }

  return status;
} /* mcfg_fs_stat */


/* **** POSIX FS API Wrappers **** */

/*==========================================================================
 
  FUNCTION mcfg_fopen()

  DESCRIPTION
    Open a file in the filesystem.  If successful, the resulting file
    descriptor will be the smallest available non-negative integer that can be
    used as a handle to refer to this file in the calls described below.
    Each open file descriptor maintains its own file offset and open flags.

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    path      [in] path for file to open or create
    oflag     [in] The 'oflag' argument describes how this file should be opened
              MCFG_FS_O_RDONLY, MCFG_FS_O_CREATE 
    mode      [in] perms
    fs_type    in] file system type
    subs_id   [in] Subscription ID for the Multi-SIM NV item to delete
 
  RETURN VALUE
    file descriptor handle or -1 if error

  SIDE EFFECTS
===========================================================================*/
int mcfg_fopen (
   const char *path, 
   int oflag, 
   int mode, 
   mcfg_fs_e_type fs_type, 
   mcfg_fs_sub_id_e_type sub_id )
{
  mcfg_fs_status_e_type status; 
  int  fd;
  char f_name[MCFG_FS_MAX_PATH_LENGTH];

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  status = mcfg_fs_build_path(MCFG_FS_TYPE_EFS, sub_id, path, f_name, MCFG_FS_MAX_PATH_LENGTH);
  f_name[MCFG_FS_MAX_PATH_LENGTH-1] = '\0';

  if (status == MCFG_FS_STATUS_OK)
  {
    //MCFG_MSG_LOW_2("mcfg_fopen -  sub_id: 0x%0x, filepath %s", sub_id, f_name);
    fd = efs_open(f_name, oflag, mode);
  }
  else
  {
    fd=MCFG_FS_ERROR; // in case there are issues building the path
  }

  return fd;
}

/*==========================================================================
 
  FUNCTION mcfg_fclose

  DESCRIPTION
 
    Close the file
 
    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    filedes     [in] File descriptor
    fs_type     [in] file system type
 
  RETURN VALUE
    0 if success, -1 if error
  SIDE EFFECTS
===========================================================================*/
int mcfg_fclose(
   int filedes, 
   mcfg_fs_e_type fs_type)
{
  int efs_ret= MCFG_FS_ERROR;

  if (fs_type == MCFG_FS_TYPE_EFS)
  {
    efs_ret = efs_close (filedes);
  }

  return efs_ret;
}

/*==========================================================================
 
  FUNCTION mcfg_fwrite()

  DESCRIPTION
 
    Write to file  'nbytes' of data to the file associated with the
    specified file descriptor. The write ioperation may happen at the current
    file pointer or at the end-of-file if the file is opened with O_APPEND.
 
    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    filedes   [in] File descriptor
    buf       [out] Pointer to buf to write
    nbyte     [out] Number of bytes to be written
    fs_type   [in] file system type
 
  RETURN VALUE
    num of bytes written or -1 if error

  SIDE EFFECTS
===========================================================================*/
int mcfg_fwrite (
   int filedes, 
   void *buf, 
   uint32 nbyte, 
   mcfg_fs_e_type fs_type)
{
  int efs_ret= MCFG_FS_ERROR;

  if (fs_type == MCFG_FS_TYPE_EFS)
  {
    efs_ret = efs_write (filedes, buf, nbyte);
  }

  return efs_ret;
}

/*==========================================================================
 
  FUNCTION mcfg_fread

  DESCRIPTION
 
    read 'nbytes' of data from the file associated with the
    specified file descriptor.
    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
 
    filedes   [in] File descriptor
    buf       [out] Pointer to buf to read
    nbyte     [out] Number of bytes to be read
    fs_type   [in] file system type
 
  RETURN VALUE
    num of bytes read or -1 of error

  SIDE EFFECTS
===========================================================================*/
int mcfg_fread (
   int filedes, 
   void *buf, 
   uint32 nbyte, 
   mcfg_fs_e_type fs_type)
{
  int efs_ret=MCFG_FS_ERROR;

  if (fs_type == MCFG_FS_TYPE_EFS)
  {
    efs_ret = efs_read(filedes, buf, nbyte);
  }

  return efs_ret;
}

/*==========================================================================
 
  FUNCTION mcfg_mkdir

  DESCRIPTION
    Create a new directory

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    path      [in] Dir path
    mode      [in] Access perms
    fs_type    [in] file system type
 
  RETURN VALUE
    0 on success, -1 on error
 
  SIDE EFFECTS
===========================================================================*/
int mcfg_mkdir (
   const char *path, 
   int mode, 
   mcfg_fs_e_type fs_type)
{
  int efs_ret=MCFG_FS_ERROR;

  if (fs_type == MCFG_FS_TYPE_EFS)
  {
    efs_ret = efs_mkdir (path, mode);
  }

  return efs_ret;
}

/*==========================================================================
 
  FUNCTION mcfg_rmdir

  DESCRIPTION
    remove the directory
    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    path      [in] dir path
    fs_type    in] file system type
 
  RETURN VALUE
    0 on success, -1 on error

  SIDE EFFECTS
===========================================================================*/

int mcfg_rmdir (
   const char *path, 
   mcfg_fs_e_type fs_type)
{
  int efs_ret=MCFG_FS_ERROR;

  if (fs_type == MCFG_FS_TYPE_EFS)
  {
    efs_ret = efs_rmdir (path);
  }

  return efs_ret;
}

/*==========================================================================
 
  FUNCTION mcfg_fstat  

  DESCRIPTION
    Get the information about the file, with its open file handle
 
    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    path      [in] dir path
    fs_type    in] file system type
 
  RETURN VALUE
    0 on success, -1 on error

  SIDE EFFECTS
===========================================================================*/
int mcfg_fstat(int fildes, 
               struct fs_stat *buf,
               mcfg_fs_e_type fs_type)
{
  int efs_ret=MCFG_FS_ERROR;

  if(fs_type == MCFG_FS_TYPE_EFS)
  {
    efs_ret = efs_fstat(fildes, buf);
  }

  return efs_ret;
}

/*==========================================================================
 
  FUNCTION mcfg_lseek

  DESCRIPTION
    Change the file offset for the opened file descriptor
 
    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    filedes      [in] File descriptor
    offset       [out]new offset of the file
    whence        [in] 
        'whence' indicates how the new offset is computed:
       MCFG_SEEK_SET - Set to 'offset'.
       MCFG_SEEK_CUR - Set to 'offset' + current position.
       MCFG_SEEK_END - Set to 'offset' + file size
 
    fs_type   [in] file system type
      RETURN VALUE
    0 on success, -1 on error

  SIDE EFFECTS
===========================================================================*/
int mcfg_lseek (
   int filedes, 
   int offset, 
   int whence, /* MCFG_SEEK_SET, SEEK_CUR, SEEK_END */ 
   mcfg_fs_e_type fs_type)
{
  int efs_ret=MCFG_FS_ERROR;

  if (fs_type == MCFG_FS_TYPE_EFS)
  {
    efs_ret = efs_lseek (filedes, offset, whence);
  }

  return efs_ret;
}

/*==========================================================================
 
  FUNCTION mcfg_fs_errno

  DESCRIPTION

    Get EFS erno
 
  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
 
  RETURN VALUE

  SIDE EFFECTS
===========================================================================*/
int mcfg_fs_errno(mcfg_fs_e_type fs_type)
{
  int efs_ret=MCFG_FS_ERROR;

  if (fs_type == MCFG_FS_TYPE_EFS)
  {
    return efs_errno;
  }

  return efs_ret;
}

/*==========================================================================
 
  FUNCTION mcfg_fs_truncate

  DESCRIPTION
    Truncate a file to a specified length.  The file will be extended, or
    shrunk to the newly specified size.  On extension, the filesystem does
    not actually allocate space to the file, until data is written. When
    shortening a file, the data past the new end is lost.

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    path      [in] The path of the file whose length has to be changed.
    length    [out] new size of the file
    fs_type   [in] file system type
    subs_id   [in] Subscription ID for the Multi-SIM NV item to delete
 
  RETURN VALUE
    Returns 0 on success, or -1 for error.

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies
 
 
===========================================================================*/
int mcfg_fs_truncate (const char *path, 
                      int length,
                      mcfg_fs_e_type fs_type, 
                      mcfg_fs_sub_id_e_type sub_id )
{
  mcfg_fs_status_e_type status; 
  char f_name[MCFG_FS_MAX_PATH_LENGTH];
  int efs_ret=MCFG_FS_ERROR;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  status = mcfg_fs_build_path(MCFG_FS_TYPE_EFS, sub_id, path, f_name, MCFG_FS_MAX_PATH_LENGTH);
  f_name[MCFG_FS_MAX_PATH_LENGTH-1] = '\0';

  if (status == MCFG_FS_STATUS_OK)
  {
    efs_ret = efs_truncate(f_name, length);
  }

  return efs_ret;
}

/*==========================================================================
 
  FUNCTION mcfg_ftruncate

  DESCRIPTION
 
    Truncate an open file.  Similar to efs_truncate.  The file must be
    opened for writing.
    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    filedes   [in] File decriptor
    length    [out] new size of the file
    fs_type   [in] file system type
    subs_id   [in] Subscription ID for the Multi-SIM NV item to delete
 
  RETURN VALUE
    Returns 0 on success, or -1 for error.

  SIDE EFFECTS
===========================================================================*/
int mcfg_ftruncate (int fildes, 
                    int length,
                    mcfg_fs_e_type fs_type)
{

  int efs_ret=MCFG_FS_ERROR;

  if (fs_type == MCFG_FS_TYPE_EFS)
  {
    efs_ret = efs_ftruncate (fildes, length);
  }

  return efs_ret;
}

/*==========================================================================
 
  FUNCTION mcfg_fs_rename

  DESCRIPTION
 
    rename the file
 
    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    old path      [in] old path
    new path      [in] new path
    fs_type    in] file system type
    subs_id   [in] Subscription ID for the Multi-SIM NV item to delete
 
  RETURN VALUE
    Returns 0 on success, or -1 for error.

  SIDE EFFECTS
===========================================================================*/
int mcfg_fs_rename (const char *oldpath, 
                    const char *newpath,
                    mcfg_fs_e_type fs_type, 
                    mcfg_fs_sub_id_e_type sub_id )

{
  mcfg_fs_status_e_type status; 
  char old_f_name[MCFG_FS_MAX_PATH_LENGTH];
  char new_f_name[MCFG_FS_MAX_PATH_LENGTH];
  int efs_ret=MCFG_FS_ERROR;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  status = mcfg_fs_build_path(MCFG_FS_TYPE_EFS, sub_id, oldpath, old_f_name, MCFG_FS_MAX_PATH_LENGTH);
  old_f_name[MCFG_FS_MAX_PATH_LENGTH-1] = '\0';

  if (status == MCFG_FS_STATUS_OK)
  {
    status = mcfg_fs_build_path(MCFG_FS_TYPE_EFS, sub_id, newpath, new_f_name, MCFG_FS_MAX_PATH_LENGTH);
    new_f_name[MCFG_FS_MAX_PATH_LENGTH-1] = '\0';
    if (status == MCFG_FS_STATUS_OK)
    {
      efs_ret = efs_rename(old_f_name, new_f_name);
    }
  }

  return efs_ret;
}

/*==========================================================================
 
  FUNCTION mcfg_fs_deltree

  DESCRIPTION
 
   Delete the directory and all its contents recursively. 'path' can be a
   directory, file, node or symlink. If the argument is a directory all its
   contents along with itself is deleted. If a non directory is passed as
   argument it is deleted. symlinks encountered are not dereferenced, just
   the link is deleted.
 
    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    path      [in] EFS item or file to
    buf       [out] Pointer to the fs_stat structure where the
              return values are expected
    fs_type    in] file system type
    subs_id   [in] Subscription ID for the Multi-SIM NV item to delete
 
  RETURN VALUE
    Returns 0 on success, or -1 for error.

  SIDE EFFECTS
===========================================================================*/
int mcfg_fs_deltree(const char *path, 
                    mcfg_fs_e_type fs_type )
{
  int efs_ret=MCFG_FS_ERROR;

  if (fs_type == MCFG_FS_TYPE_EFS)
  {
    efs_ret = efs_deltree(path);
  }

  return efs_ret;
}

/*==========================================================================
 
  FUNCTION mcfg_fs_opendir

  DESCRIPTION
    Open a directory for iteration.  The pointer returned is an opaque
    handle that can be later passed to efs_readdir to iterate through
    the entries of the opened directory. This same pointer must be passed
    to closedir to close the iterator.\n
 
    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    dirname   [in] directory name
    buf       [out] Pointer to the fs_stat structure where the
              return values are expected
    fs_type   [in] file system type
 
  RETURN VALUE
    Returns valid directory iterator pointer on successfully opening the
    directory for iteration, otherwise NULL on error

  SIDE EFFECTS
===========================================================================*/
EFSDIR *mcfg_fs_opendir (const char *dirname, mcfg_fs_e_type fs_type )
{
  EFSDIR * dirp;

  if (fs_type == MCFG_FS_TYPE_EFS)
  {
    dirp = efs_opendir(dirname);
  }
  else
  {
    dirp = NULL;
  }

  return dirp;
}

/*==========================================================================
 
  FUNCTION mcfg_fs_closedir

  DESCRIPTION
 
    Close the directory iterator. This releases the iterator for reuse
 
    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    dirp       [in] The iterator handle obtained earlier from the ::efs_opendir() function
    fs_type    in] file system type
 
  RETURN VALUE
    Returns 0 on success, or -1 for error.
   
  SIDE EFFECTS
===========================================================================*/
int mcfg_fs_closedir (EFSDIR *dirp, mcfg_fs_e_type fs_type )
{
  int efs_ret=MCFG_FS_ERROR;

  if (fs_type == MCFG_FS_TYPE_EFS)
  {
    efs_ret = efs_closedir(dirp);
  }

  return efs_ret;
}

/*==========================================================================
 
  FUNCTION mcfg_fs_readdir
 
  Read next entry from the opened directory iterator
 
  DESCRIPTION
 
   Read next available entry from the opened directory iterator. If
   an entry is available then the fs_dirent structure filled with
   details about enumerated entry is returned. If no other entries
   are found then NULL is returned.
 
   Blocks the calling task until the EFS operation is complete.  This 
   involves acquiring the FS global lock, which may be held by another 
   task.  Therefore this function must not be called from a task-locked 
   context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    dirp       [in] The iterator handle obtained earlier from the ::efs_opendir() function
    fs_type    in] file system type
 
  RETURN VALUE
    On success, returns a pointer to a struct ::fs_dirent, which contains
    a field d_name which is the name of this directory component.  Returns
    a NULL pointer on error or end of file.
 
  SIDE EFFECTS
===========================================================================*/
struct fs_dirent *mcfg_fs_readdir (EFSDIR *dirp, 
                                   mcfg_fs_e_type fs_type )
{
  struct fs_dirent *dirent;

  if (fs_type == MCFG_FS_TYPE_EFS)
  { 
    dirent = efs_readdir (dirp); 
  }
  else
  { 
    dirent = NULL;
  }

  return dirent;
}

/*==========================================================================
 
  FUNCTION mcfg_fs_readdir_subid

  DESCRIPTION

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    dirp       [in] The iterator handle obtained earlier from the ::efs_opendir() function
    fs_type    in] file system type
    subs_id   [in] Subscription ID for the Multi-SIM NV item to delete
 
  RETURN VALUE
    On success, returns a pointer to a struct ::fs_dirent, which contains
    a field d_name which is the name of this directory component.  Returns
    a NULL pointer on error or end of file.

  SIDE EFFECTS
===========================================================================*/
struct fs_dirent *mcfg_fs_readdir_subid (EFSDIR *dirp, 
                                   mcfg_fs_e_type fs_type,
                                   mcfg_fs_sub_id_e_type sub_id )
{
  struct fs_dirent *dir_entry;
  uint16    name_len;
  mcfg_fs_sub_id_e_type cur_sub = MCFG_FS_SUBID_BIG;
  char *sub_char;

  if (fs_type != MCFG_FS_TYPE_EFS)
  { 
     return NULL;
  }

  do
  {
    dir_entry = efs_readdir(dirp);
    if (!dir_entry)
    {
      break;
    }

    name_len = strlen(dir_entry->d_name);
    if (name_len <= 15) // Len of _Subscriptionxx
    {
      cur_sub = MCFG_FS_SUBID_0;
    }
    else
    {
      sub_char = dir_entry->d_name+(name_len-15);
      if (strncmp(sub_char, "_Subscription01", 15)==0)
      {
        *sub_char = '\0';
        cur_sub = MCFG_FS_SUBID_1;
      }
      else if (strncmp(sub_char, "_Subscription02", 15)==0)
      {
        *sub_char = '\0';
        cur_sub = MCFG_FS_SUBID_2;
      }
      else
      {
        cur_sub = MCFG_FS_SUBID_0;
      }
    }

    if (cur_sub == sub_id)
    {
      break;
    }
    else
    {
      continue;
    }
  }while(dir_entry);

  return dir_entry;

} /* mcfg_fs_readdir_subid */

