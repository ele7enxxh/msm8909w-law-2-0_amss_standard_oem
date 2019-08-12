/***********************************************************************
 * fs_unicode.c
 *
 * Code common to Unicode conversion
 * Copyright (C) 2007, 2009, 2013 Qualcomm Technologies, Inc.
 *
 * These functions are not specific to the particular encoding.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/efs/src/fs_unicode.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-05-24   wek   Make unicode ops constant.
2009-04-22   ebb   Removed memory protection.
2007-05-25   umr   Added support for EFS memory protection feature.
2007-02-07   sh    Define a default conversion.
2007-01-05   sh    Create

===========================================================================*/

#include "fs_unicode.h"

/* This is a global reference to the active set of conversion functions. */
const struct unicode_conversion_ops *fs_convert =
                                             &EFS_DEFAULT_UNICODE_CONVERSION;
