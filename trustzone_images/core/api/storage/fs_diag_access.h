/***********************************************************************
 * fs_diag_access.h
 *
 * Access validation to directories and files.
 * Copyright (C) 2006,2008-2010 Qualcomm Technologies, Inc.
 *
 * Types and function prototypes required to implement access
 * validation to directories and files.
 * Migrated from:
 * //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.1/fs_diag_access.h#2
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/storage/fs_diag_access.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2010-07-12   rp    Added defines for major and minor number for API files.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-09-11   s h   Removed FEATURE_EFS2 safety check to satisfy CMI.
2008-10-16   yog   Removed FEATURE_DIAG_FS_ACCESS_VALIDATION check.
2008-07-10    yg   Made FS_DIAG_MAX_ACCESS_VALIDATOR_CLIENTS configurable.
2008-05-23   rp    Introducing /inc folder as per new SCM.
2006-12-14   s h   Retain FS_DIAG_VALIDATE_ACCESS for old fsdiag.c
2006-12-14   s h   Require EFS2.  Remove unused definitions.
2006-11-02    rp   Increased FS_DIAG_MAX_ACCESS_VALIDATOR_CLIENTS from 4 to 5.
2006-11-01   s h   Moved from Diag VU to EFS

===========================================================================*/

#ifndef __FS_DIAG_ACCESS_H__
#define __FS_DIAG_ACCESS_H__

#include "comdef.h"
#include "customer.h"

#include "fs_sys_types.h"

/** Identifies the major version number of this API file. */
#define VER_FS_DIAG_ACCESS_H_MAJOR     1
/** Identifies the minor version number of this API file. */
#define VER_FS_DIAG_ACCESS_H_MINOR     1

/* ------------------------------------------------------------------------
 * EFS Access Validation API
 *   Provides an interface for a client to register an accessor validator
 *   function.  When registered, DIAG will call the function for each of
 *   the file system operations given by fs_diag_op_type.  The callback
 *   returns a value indicating whether the operation can proceed.  If
 *   the call back indicates the operation is not ok, DIAG returns an
 *   ERR packet indicating this is a bad command - otherwise the operation
 *   proceeds as normal.
 * ------------------------------------------------------------------------*/

/* Maximum length of a pathname. This varies depending on configuration. */
#define FS_DIAG_ACCESS_PATH_MAX FS_PATH_MAX

/* Enumeration of possible file operations */
typedef enum {
  DIAG_FS_CHECK_NONE,
  /* EFS1 operations and operations shared between EFS1 and EFS2 */
  DIAG_FS_CHECK_MKDIR,       /*  1     */
  DIAG_FS_CHECK_DISP_DIRS,
  DIAG_FS_CHECK_DISP_FILES,
  DIAG_FS_CHECK_RMDIR,
  DIAG_FS_CHECK_GET_ATTRIB, /*   5     */
  DIAG_FS_CHECK_SET_ATTRIB,
  DIAG_FS_CHECK_READ,
  DIAG_FS_CHECK_WRITE,
  DIAG_FS_CHECK_DELETE,
  DIAG_FS_CHECK_REMOTE_LINK,
  DIAG_FS_CHECK_ITERATE,

  /* Operations specific to EFS2 */
  DIAG_FS_CHECK_OPEN,
  DIAG_FS_CHECK_CLOSE,
  DIAG_FS_CHECK_SYMLINK,
  DIAG_FS_CHECK_READLINK,     /*  15     */
  DIAG_FS_CHECK_UNLINK,
  DIAG_FS_CHECK_OPENDIR,
  DIAG_FS_CHECK_READDIR,
  DIAG_FS_CHECK_CLOSEDIR,
  DIAG_FS_CHECK_RENAME,       /*  20     */
  DIAG_FS_CHECK_STAT,
  DIAG_FS_CHECK_LSTAT,
  DIAG_FS_CHECK_FSTAT,
  DIAG_FS_CHECK_CHMOD,
  DIAG_FS_CHECK_STATFS,       /*  25     */
  DIAG_FS_CHECK_ACCESS,
  DIAG_FS_CHECK_FORMAT,
  DIAG_FS_CHECK_MAX_OP    = 0x7fffffff
} diag_fs_op_type;

/* This function is queried to approve/deny access of this type to this path */
boolean fs_diag_access_check (const char *, diag_fs_op_type);

/* Macros for access validation based on the path */
#define FS_DIAG_VALIDATE_PATH_ACCESS(xx_op, xx_path)\
if (fs_diag_access_check (xx_path, DIAG_FS_CHECK_##xx_op) == FALSE) {\
  return (diagpkt_err_rsp (DIAG_BAD_CMD_F, req_ptr, pkt_len));}

/* The old macro used exclusively by services/diag/fsdiag.c */
#define FS_DIAG_VALIDATE_ACCESS(xx_op, xx_path)\
if (fs_diag_access_check (xx_path, DIAG_FS_CHECK_##xx_op) == FALSE) {\
  return (diagpkt_err_rsp(DIAG_BAD_CMD_F, req, pkt_len));}

/* Callback prototype to registering application
 *   Callback returns TRUE  if the operation is ALLOWED
 *                    FALSE otherwise
 */
typedef boolean (*diag_fs_access_check_fptr_type) (
  char  *file_name_ptr, /* Name of file being validated            */
  uint32 op_type        /* Operation type given by diag_fs_op_type */
);

/*===========================================================================

FUNCTION DIAG_FS_REGISTER_ACCESS_CHECK

DESCRIPTION
   Registers a function to be called for all EFS operations via DIAG for
   the list of directories provided by dir_list_ptr.  If dir_list_ptr
   is NULL, then the callback should be called for ALL access to ANY
   file in the file system accessed via DIAG.

   If dir_list_ptr is not NULL, then dir_list_ptr points to an array
   of dir_list_size NULL terminated strings naming the directories for
   which the access callback should be made.

   DIAG will point to the directory list if it is non NULL, clients must
   not alter this memory after registering with DIAG.
============================================================================*/
void diag_fs_register_access_check (
  const char                    **dir_list_ptr,
  uint32                          dir_list_size,
  diag_fs_access_check_fptr_type  access_cb_fptr);

#endif /* not __FS_DIAG_ACCESS_H__ */

