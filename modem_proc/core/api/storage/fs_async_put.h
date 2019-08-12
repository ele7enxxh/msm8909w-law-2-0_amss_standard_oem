/***********************************************************************
 * fs_async_put.h
 *
 * Public API header for Asynchronous EFS Item file write support.
 * Copyright (C) 2011-2012 Qualcomm Technologies, Inc.
 *
 * This file documents API that can be used to request non-blocking writes to
 * EFS Item files.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/api/storage/fs_async_put.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-01-04   dks   Incorporate code review comments : include config file.
2011-11-21   dks   Create

===========================================================================*/
#ifndef __FS_ASYNC_PUT_H__
#define __FS_ASYNC_PUT_H__

#include "fs_async_put_config.h"
#include "fs_sys_types.h"
#include "fs_errno.h"
#include "fs_fcntl.h"
#include "fs_public.h"

/** Identifies the major version number of this API file. */
#define VER_FS_ASYNC_PUT_H_MAJOR     1
/** Identifies the minor version number of this API file. */
#define VER_FS_ASYNC_PUT_H_MINOR     1

/** \mainpage EFS_ASYNC_PUT Documentation
 *
 * This documentation assumes that you are familiar with the operation of the
 * Qualcomm Embedded file system (EFS2), as well as with the operation of AMSS
 * software. In addition, a familiarity with the Posix file system interface is
 * helpful.
 *
 * For assistance or clarification on information in this guide, submit a
 * Service Request to QUALCOMM CDMA Technologies at
 * <A HREF="https://support.cdmatech.com/"> https://support.cdmatech.com/ </A>
 *
 * If you do not have access to the World Wide Web, you may send email to
 * <A HREF="mailto:support.cdmatech@qualcomm.com"> support </A>
 *
 *
 *
 *
 * The EFS_ASYNC_PUT module provides support to asynchornously write to EFS2
 * special item files to time sensitive EFS2 clients that do not want to be
 * blocked until data is written to the storage.
 *
 */


/**
 * @defgroup efs_async_put EFS_ASYNC_PUT
 *
 */


/**
 * @defgroup efs_async_put_config EFS_ASYNC_PUT Configuration Options
 * @ingroup efs_async_put
 * \n \n
 * Targets can override these values in the cust files as needed.
 *
 * \link :: FS_ASYNC_PUT_MAX_DATA_BYTES Maximum data allowed to be written
 * \endlink \n
 * \link :: FS_ASYNC_PUT_CMD_QUEUE_SIZE Maximum number of outstanding
 * efs_async_put requests that can be queued \endlink \n
 * \link :: FS_PATH_MAX Maximum file path length allowed to be written to
 * \endlink \n
 */


/**
 * @defgroup efs_async_put_errno Error handling
 * @ingroup efs_async_put
 * \n \n
 *
 * For functions returning an integer value, the value '-1' is used
 * to indicate an error, and a NULL pointer is used for functions that
 * return a pointer. When this value is returned, the global variable
 * efs_errno will be set to a value giving more specific details as to the
 * reason for this error, To interpret this efs_errno see fs_errno.h
 */


/**
 * @defgroup efs_async_put_pathnames Pathnames
 * @ingroup efs_async_put
 *
 * The pathnames follow same syntax as EFS2 pathnames. They consist of null
 * terminated strings of UTF-8 encoded character data.  Directories
 * are separated using the forward slash character '/'.  Please note that
 * pathnames are not checked for UTF-8 validity, a name component is simply
 * terminated when a '/' or null is encountered.
 *
 * Absolute pathnames begin with a slash, and relative pathnames omit the
 * initial slash.  (Since the current implementation does not allow the
 * 'current' directory to be changed, these resolve the same).  There are
 * two special names: "." which refers to the current directory, and ".."
 * which refers to the parent directory.
 *
 * An individual name component of a path (between slashes) can be no more
 * than FS_NAME_MAX characters in length.  An entire path is limited
 * FS_PATH_MAX characters. See fs_config.h for FS_PATH_MAX configuration.
 */


/**
 * @defgroup efs_async_put_modes File Mode
 * @ingroup efs_async_put
 *
 * The mode argument specifies the permissions given to the file. \n
 * One or more of the following permission bits can be
 * OR'd as the mode parameter:
 *    ::S_IRUSR   : Read permission for User      \n
 *    ::S_IWUSR   : Write permission for User     \n
 *    ::S_IXUSR   : Execute permission for User   \n
 *    ::S_IRGRP   : Read permission for Group     \n
 *    ::S_IWGRP   : Write permission for Group    \n
 *    ::S_IXGRP   : Execute permission for Group  \n
 *    ::S_IROTH   : Read permission for Other     \n
 *    ::S_IWOTH   : Write permission for Other    \n
 *    ::S_IXOTH   : Execute permission for Other  \n
 *    ::S_ISUID   : Set UID on execution          \n
 *    ::S_ISGID   : Set GID on execution          \n
 *    ::S_ISVTX   : Sticky bit (Hidden attrib in FAT)    \n
 */


/**
 * @par Name:
 *  efs_async_put - Store a value in an EFS2 'special item' file.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_async_put.h"
 *    int efs_async_put (const char *path, void *data, fs_size_t length,
 *                       int oflag, int mode);
 *  @endcode
 *
 * @par Description:
 *  Queue request to store a value in an EFS2 'special item' file. It only
 *  queues the request and the write itself will be processed later by a low
 *  priority task when it gets scheduled to run. The API is similar to the EFS2
 *  API efs_put in that it requests writing to an item file but unlike efs_put
 *  it does not block the caller until data is commited to storage.
 *
 * @par Limitations:
 *    ::API can fail if the queue of requests is full because the processing
 *      task did not get scheduled to run yet.\n
 *    ::The processing task has very low priority and any higher priority task
 *      can cause it to starve.
 *    ::Requests are processed in FIFO order and not in requesting tasks'
 *      priorities.\n
 *    ::Since items in queue will not get written until the processing task
 *      becomes runnable a successfully queued item may not get committed
 *      to the disk in case of an ungraceful shutdown of the phone.\n
 *    ::Calling efs_get after a request to efs_async_put will return the value
 *      last successfully written to the storage which may not be the same as
 *      last reuqest made in a successful call to efs_async_put.
 *
 * @param path
 *  The path of the item-file.
 *
 * @param data
 *  The start-address of the data.
 *
 * @param length
 *  The length indicates the size of the item-file.
 *
 * @param oflag
 *  The 'oflag' argument describes how this file should be created, and
 *  'oflag' should be a bitwise OR of one of the following:\n
 *    O_CREAT   - Create the item if it does not exist.\n
 *    O_EXCL    - Fail if the item already exists.\n
 *    O_AUTODIR - Auto create parent directories.\n
 *
 * @param ...
 *    @b mode
 *    The mode argument specifies the permissions given to the file. \n
 *    One or more of the following permission bits can be
 *    OR'd as the mode parameter:
 *  @par
 *    ::S_IRUSR   : Read permission for User      \n
 *    ::S_IWUSR   : Write permission for User     \n
 *    ::S_IXUSR   : Execute permission for User   \n
 *    ::S_IRGRP   : Read permission for Group     \n
 *    ::S_IWGRP   : Write permission for Group    \n
 *    ::S_IXGRP   : Execute permission for Group  \n
 *    ::S_IROTH   : Read permission for Other     \n
 *    ::S_IWOTH   : Write permission for Other    \n
 *    ::S_IXOTH   : Execute permission for Other  \n
 *    ::S_ISUID   : Set UID on execution          \n
 *    ::S_ISGID   : Set GID on execution          \n
 *    ::S_ISVTX   : Sticky bit (Hidden attrib in FAT)    \n
 *
 *
 * @return
 *  Returns a zero if the item is successfully queued to be written or -1 for
 *  error.
 *
 * @par Errors:
 *  Upon error, the efs_errno will be set to one of the following:\n
 *    ::EINVAL       - Invalid parameter.\n
 *    ::ENAMETOOLONG - The 'path' specified, or a component of it is
 *                     longer than FS_PATH_MAX.\n
 *    ::FS_ERANGE    - 'length' is greater than FS_ASYNC_PUT_MAX_DATA_BYTES.\n
 *    ::ENOSPC       - The queue storing write requests is full and cannot take
 *                     new requests until existing requests are processed.\n
 * @sa
 *    efs_put, efs_get
 *
 **********************************************************************/
int efs_async_put (const char *path, void *data, fs_size_t length,
                   int oflag, int mode);

#endif /* not __FS_ASYNC_PUT_H__ */
