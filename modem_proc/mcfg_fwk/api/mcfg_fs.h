#ifndef MCFG_FS_H
#define MCFG_FS_H
/*==============================================================================

        MODEM_CFG  -     File System  Wrapper API 

GENERAL DESCRIPTION
  Simple wrapper APIs to access Modem Configuration Items (incl Multi SIM support)

Copyright (c) 2014-2015 by QUALCOMM Technologies Incorporated.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/api/mcfg_fs.h#1 $
$DateTime: 2016/12/13 08:00:27 $ 
$Author: mplcsds1 $
$Change: 11985219 $

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
02/24/15 sbt   Initial creation

==============================================================================*/

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
#include "fs_sys_types.h"

/*==============================================================================
  Constants and Macros
==============================================================================*/

#define MCFG_FS_NUM_OF_SUB_ID  (3)

#define MCFG_FS_MAX_PATH_LENGTH (128)

#define MCFG_FS_ERROR           (-1)

/* Oflags - derived from fs_fcntl.h */
#define MCFG_FS_O_RDONLY             00
#define MCFG_FS_O_WRONLY             01
#define MCFG_FS_O_RDWR               02
#define MCFG_FS_O_CREAT            0100 /**< not fcntl */
#define MCFG_FS_O_EXCL             0200 /**< not fcntl */
#define MCFG_FS_O_NOCTTY           0400 /**< not fcntl */
#define MCFG_FS_O_TRUNC           01000 /**< not fcntl */
#define MCFG_FS_O_APPEND          02000
#define MCFG_FS_O_NONBLOCK        04000
#define MCFG_FS_O_SYNC           010000
#define MCFG_FS_O_AUTODIR      02000000 /**< Allow auto-creation of directories */

/* Perms - derived from fs_systypes.h */
/* Common combinations of bits. */
#define MCFG_FS_ALLPERMS        0777
#define MCFG_FS_DEFFILEMODE     0666

#define MCFG_SEEK_SET       0       /**< Seek from beginning of file.  */
#define MCFG_SEEK_CUR       1       /**< Seek from current position.  */
#define MCFG_SEEK_END       2       /**< Seek from end of file.  */

/*==============================================================================
  Typedefs
==============================================================================*/
/* File System Type */
typedef enum mcfg_fs_e {

  MCFG_FS_TYPE_EFS  = 0,
    /* Request completed successfully.  Equivalent to NV_DONE_S */

  MCFG_FS_TYPE_SFS = 1,
    /* Invalid params */

  MCFG_FS_TYPE_BIG = 0x7FFFFFFF 
    /* Reserved (force enum to 32-bit width) */
} mcfg_fs_e_type;


/* Enum type for subs_id, not mandated by API */
typedef enum mcfg_fs_sub_id_e {

  MCFG_FS_SUBID_NONE = -1, /* Non subs specific, Equivalent to SYS_MODEM_AS_ID_NONE */

  MCFG_FS_SUBID_0 = 0,
    /* Context0: Sub id 0 or primary subscription, Equivalent to SYS_MODEM_AS_ID_1  */

  MCFG_FS_SUBID_1 = 1,
      /* Context1: Sub id 1 or Sec subscription, Equivalent to SYS_MODEM_AS_ID_2  */
  
  MCFG_FS_SUBID_2 = 2,
      /* Context2: Sub id 2 or Tertiary, Equivalent to SYS_MODEM_AS_ID_1  */

  MCFG_FS_SUBID_BIG = 0x7FFFFFFF 
      /* Reserved (force enum to 32-bit width) */
} mcfg_fs_sub_id_e_type;

/* 
** Status enum returned by mcfg_fs_read, mcfg_fs_write, 
** mcfg_fs_stat, mcfg_delete
** Applications wishing to compare against FS status enum settings can use 
** a uint32 instead of  mcfg_fs_status_e_type, for example:
**   uint32 retval = mcfg_fs_write(...);  
**   if (retval == MCFG_FS_STATUS_OK) ...
** This behavior is supported but not recommended for new code.
*/

typedef enum  mcfg_fs_status_e {

  /* The values of the following enums are equal in value and (mostly) meaning
   * of the listed nv_stat_enum_type */

  MCFG_FS_STATUS_OK = 0,
    /* Request completed successfully.  Equivalent to NV_DONE_S */

  MCFG_FS_STATUS_INVALID_PARAM = 2,
    /* Invalid params */
     
  MCFG_FS_STATUS_EFS_ERR = 4,
    /* Internal EFS error.  Equivalent to NV_FAIL_S */
    /* For getting the actual error from FS, call mcfg_errno() */

  MCFG_FS_STATUS_NOT_ACTIVE = 5,
    /* File doesn't exist.  Equivalent to NV_NOTACTIVE_S */

  MCFG_FS_STATUS_UNSUPPORTED = 11,
    /* FS Type not supported */

  /* The following enums are not included in nv_stat_enum_type */

  MCFG_FS_STATUS_BAD_SIZE = 128,
    /* On read: data size doesn't match current item size
     */

  MCFG_FS_STATUS_NULL_PTR = 129,



  MCFG_FS_STATUS_BIG = 0x7FFFFFFF 
    /* Reserved (force enum to 32-bit width) */
} mcfg_fs_status_e_type;


/*==============================================================================
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
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
 File system type = 0 for EFS, other values not supported yet

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
   mcfg_fs_sub_id_e_type sub_id);

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
   mcfg_fs_sub_id_e_type sub_id );


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
    fs_type   [in] file system type
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
   mcfg_fs_sub_id_e_type sub_id );


/*==========================================================================

  FUNCTION mcfg_fs_stat

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
   );


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
   mcfg_fs_sub_id_e_type sub_id );

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
   mcfg_fs_e_type fs_type);

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
   mcfg_fs_e_type fs_type);

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
   mcfg_fs_e_type fs_type);

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
   mcfg_fs_e_type fs_type);

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
   mcfg_fs_e_type fs_type);

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
               mcfg_fs_e_type fs_type);

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
   mcfg_fs_e_type fs_type);

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
int mcfg_fs_errno(mcfg_fs_e_type fs_type);

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
                      mcfg_fs_sub_id_e_type sub_id );

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
                    mcfg_fs_e_type fs_type);

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
                    mcfg_fs_sub_id_e_type sub_id );

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
                    mcfg_fs_e_type fs_type );


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
EFSDIR *mcfg_fs_opendir (const char *dirname, mcfg_fs_e_type fs_type );

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
int mcfg_fs_closedir (EFSDIR *dirp, mcfg_fs_e_type fs_type );

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
                                   mcfg_fs_e_type fs_type );


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
                                   mcfg_fs_sub_id_e_type sub_id );


#endif /* MCFG_FS_H */

