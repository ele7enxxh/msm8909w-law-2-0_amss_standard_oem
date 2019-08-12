/**********************************************************************
 * fs_stdio.h
 * DEPRECATED EFS Stdio implementation.
 *
 * Standard IO library for EFS.
 * Copyright (C) 2004, 2006-2010, Qualcomm Technologies, Inc.
 *
 * Most of the calls in this library have a direct replacement on the
 * enhanced version of stdio (fs_estdio.h). The enhanced version has
 * the following advantages:
 * - Detailed documentation of API's.
 * - Thread Safe.
 * - Allows buffered and non buffered I/O.
 * - The buffer can be any size (not only power of two)
 * - Supports Append Mode.
 * - Bug fixes.
 * - More Posix Compliant.
 * - Added fflush function.
 *
 * Don't use the API's defined here in new code, existing code should
 * move to the new API's. All of the API's defined here are deprecated.
 *
 */

/*==========================================================================
 *
 *                          EDIT HISTORY FOR MODULE
 *
 *  This section contains comments describing changes made to the module.
 *  Notice that changes are listed in reverse chronological order.
 *
 *  $Header: //components/rel/core.mpss/3.5.c12.3/api/storage/fs_stdio.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 * when        who  what, where, why
 * ---------   ---  -------------------------------------------------------
 * 2010-07-12   rp  Added defines for major and minor number for API files.
 * 2010-02-08   rp  CMI compliance, remove #ifdef from API files.
 * 2009-08-10  wek  Fix comments for doxygen.
 * 2009-06-12  ebb  Moved rex_xxx calls to fs_osal.h abstraction.
 * 2009-01-26  wek  Added deprecation comments.
 * 2008-05-23   rp  Introducing /inc folder as per new SCM.
 * 2007-06-09   rp  Doxygen comments markup.
 * 2007-03-01   rp  Removed efs_fget_rbk_file_nbr() and efs_fget_rbk_file_fd().
 * 2007-01-31   rp  Add efs_fget_rbk_file_nbr() and efs_fget_rbk_file_fd().
 * 2006-11-02   rp  Add efs_stdio_init().
 * 2006-10-14   sh  Add efs_fpending().  And comments.
 * 2006-06-09  dlb  Add efs_fdopen().
 * 2006-05-30  dlb  Add efs_fileno().
 * 2004-02-10   gr  Added support for asynchronous writes.
 * 2003-01-25  dlb  Implement with basic write buffering.
 *
 ==========================================================================*/

#ifndef __FS_STDIO_H__
#define __FS_STDIO_H__

#include "fs_sys_types.h"
#include "fs_fcntl.h"

/** Identifies the major version number of this API file. */
#define VER_FS_STDIO_H_MAJOR     1
/** Identifies the minor version number of this API file. */
#define VER_FS_STDIO_H_MINOR     1

/** @deprecated use EFS_EFILE instead. */
typedef struct _efs_file_io EFS_FILE;

/** @deprecated */
void efs_stdio_init (void);

/** @deprecated replacement call is  efs_efopen(). */
EFS_FILE *efs_fopen (const char *path, const char *mode);

/** @deprecated replacement call is efs_efdopen(). */
EFS_FILE *efs_fdopen (int filedes, const char *mode);

/** @deprecated replacement call is efs_efread(). */
fs_size_t efs_fread (void *ptr, fs_size_t size, fs_size_t nitems,
  EFS_FILE *stream);

/** @deprecated replacement call is efs_efwrite(). */
fs_size_t efs_fwrite (void *ptr, fs_size_t size, fs_size_t nitems,
  EFS_FILE *stream);

/** @deprecated replacement call is efs_efclose(). */
int efs_fclose (EFS_FILE *stream);

/** @deprecated replacement call is efs_efseek(). */
int efs_fseek (EFS_FILE *stream, long offset, int whence);

/** @deprecated replacement call is efs_eftell(). */
long efs_ftell (EFS_FILE *stream);

/** @deprecated replacement call is efs_esetvbuf(). */
int efs_setvbuf (EFS_FILE *stream, char *buf, int mode, fs_size_t size);

/** @deprecated replacement call is efs_efileno(). */
int efs_fileno (EFS_FILE *stream);

/** @deprecated If needed the application can keep track of this number. */
fs_size_t efs_fpending (EFS_FILE *stream);

/** @deprecated This API is deprecated and has no direct replacement on
 *  fs_estdio.h */
int efs_set_sync (EFS_FILE *stream);

/** @deprecated This API is deprecated and has no direct replacement on
 *  fs_estdio.h */
int efs_set_async_params (EFS_FILE *stream, void* caller_tcb,
  unsigned long caller_sigs, void *writer_tcb,
  unsigned long writer_sigs);

/** @deprecated This API is deprecated and has no direct replacement on
 *  fs_estdio.h */
void efs_process_async_writes (void);

#endif /* not __FS_STDIO_H__ */
