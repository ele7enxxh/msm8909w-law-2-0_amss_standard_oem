
/***********************************************************************
 * fs_errno.h
 *
 * Posix interface for Embedded File System.
 * Copyright (C) 2002-2008, 2010 Qualcomm Technologies, Inc.
 *
 * This file declares error numbers used in the posix interface
 *
 ***********************************************************************/

 /*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/storage/fs_errno.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2010-07-12     rp     Added defines for major and minor number for API files.
2008-05-23     rp     Introducing /inc folder as per new SCM.
2007-06-09     rp     Doxygen comments markup.
2007-03-12     rp     Added EILSEQ and ENOERR.
2007-01-31     rp     Added EROLLBACK for Rollback feature.
2006-08-10    sch     Add code for unspecified HFAT errors.
2006-07-07    dlb     Add code for unspecified SFAT errors.
2006-05-10    dlb     Add ESTALE.
2006-01-11    nrs     Fixed Copyright header
2005-09-19    dlb     Update error numbers to use standard values.
2005-09-16    nrs     Added EDQUOT
2005-03-23    dlb     Add ENODEV.
2004-10-15    dlb     Update copyright line.
2004-10-07    dlb     Whitespace cleanup.
2004-07-19    dlb     Add EXDEV code.
2003-09-30     pg     Added ENOCARD and EBADFMT err code.
2003-06-17     gr     Added ETXTBSY err code.
2002-08-20    adm     Created.

===========================================================================*/


#ifndef _FS_ERRNO_H
#define _FS_ERRNO_H

/** Identifies the major version number of this API file. */
#define VER_FS_ERRNO_H_MAJOR     1
/** Identifies the minor version number of this API file. */
#define VER_FS_ERRNO_H_MINOR     1

/* These are constants for the errnos that we use in EFS2.  These really
 * should be merged with the REX errno definitions. */

#define ENOERR           0      /**< Success */
#define EPERM            1      /**< Operation not permitted */
#define ENOENT           2      /**< No such file or directory */
#define EEXIST           6      /**< File exists */
#define EBADF            9      /**< Bad file descriptor */
#define ENOMEM          12      /**< Out of memory. */
#define EACCES          13      /**< Permission denied. */
#define EBUSY           16      /**< Device or resource is busy */
#define EXDEV           18      /**< Attempt to cross device. */
#define ENODEV          19      /**< No such device. */
#define ENOTDIR         20      /**< Not a directory */
#define EISDIR          21      /**< Is a directory */
#define EINVAL          22      /**< Invalid argument */
#define EMFILE          24      /**< Too many open files */
#define ETXTBSY         26      /**< File or directory already open */
#define ENOSPC          28      /**< No space left on device */
#define ESPIPE          29      /**< Seek is not permitted */
#define FS_ERANGE       34      /**< Parameter out of range. */
#define ENAMETOOLONG    36      /**< File name too long */
#define ENOTEMPTY       39      /**< Directory not empty */
#define ELOOP           40      /**< Too many symbolic links encountered */
#define EILSEQ          92      /**< Illegal byte sequence */
#define ESTALE         116      /**< Stale remote file handle. */
#define EDQUOT         122      /**< Attempt to write beyond quota */

/* Non-posix errnos used within EFS. */
#define ENOCARD        301      /**< No Media present */
#define EBADFMT        302      /**< Bad Formated Media */
#define ENOTITM        303      /**< Not an EFS2 item file. */
#define EROLLBACK      304      /**< Rollback failed while backing orig file */

#define EEOF           0x8000   /**< End of file. Internal error code */

#define EUNKNOWN_SFAT  0x8001   /**< Untranslated SFAT error code. */
#define EUNKNOWN_HFAT  0x8002   /**< Untranslated HFAT error code. */

#endif /* End of _FS_ERRNO_H */
