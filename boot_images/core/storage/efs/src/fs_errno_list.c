/***********************************************************************
 * fs_errno_list.c
 *
 * List of errno values defined in this build.
 * Copyright (C) 2011-2012 Qualcomm Technologies, Inc.
 *
 * The errno values are defined by the header file errno.h. This file
 * may have different values because it comes from different sources.
 * This file contains the list of error codes used by EFS and their
 * names. This file will be pre-processed allowing us to know what is
 * the numeric value for each error code.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_errno_list.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-10-23   nr    Add ETIMEDOUT
2011-08-14   wek   Create. Get the errno from global header file.

===========================================================================*/

#include "fs_errno.h"

const unsigned int fs_EACCES = EACCES;
const unsigned int fs_EBADF = EBADF;
const unsigned int fs_EBADFMT = EBADFMT;
const unsigned int fs_EBUSY = EBUSY;
const unsigned int fs_EDQUOT = EDQUOT;
const unsigned int fs_EEOF = EEOF;
const unsigned int fs_EEXIST = EEXIST;
const unsigned int fs_EILSEQ = EILSEQ;
const unsigned int fs_EINVAL = EINVAL;
const unsigned int fs_EISDIR = EISDIR;
const unsigned int fs_ELOOP = ELOOP;
const unsigned int fs_EMFILE = EMFILE;
const unsigned int fs_ENAMETOOLONG = ENAMETOOLONG;
const unsigned int fs_ENOCARD = ENOCARD;
const unsigned int fs_ENODEV = ENODEV;
const unsigned int fs_ENOENT = ENOENT;
const unsigned int fs_ENOERR = ENOERR;
const unsigned int fs_ENOMEM = ENOMEM;
const unsigned int fs_ENOSPC = ENOSPC;
const unsigned int fs_ENOTDIR = ENOTDIR;
const unsigned int fs_ENOTEMPTY = ENOTEMPTY;
const unsigned int fs_ENOTITM = ENOTITM;
const unsigned int fs_EPERM = EPERM;
const unsigned int fs_EROLLBACK = EROLLBACK;
const unsigned int fs_ESPIPE = ESPIPE;
const unsigned int fs_ESTALE = ESTALE;
const unsigned int fs_ETIMEDOUT = ETIMEDOUT;
const unsigned int fs_ETXTBSY = ETXTBSY;
const unsigned int fs_EUNKNOWN_HFAT = EUNKNOWN_HFAT;
const unsigned int fs_EUNKNOWN_SFAT = EUNKNOWN_SFAT;
const unsigned int fs_EXDEV = EXDEV;
const unsigned int fs_FS_ERANGE = FS_ERANGE;
