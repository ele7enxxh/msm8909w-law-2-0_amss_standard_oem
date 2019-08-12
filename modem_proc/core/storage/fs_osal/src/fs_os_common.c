/***********************************************************************
 * fs_os_common.c
 *
 * OS common functionality
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 * Verbose Description
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fs_osal/src/fs_os_common.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-03-13   dks   Create

===========================================================================*/

#include "fs_os_config_i.h"
#include "fs_os_timetick.h"
#include "fs_public.h"

void
fs_os_init (void)
{
  fs_os_timetick_init();
}

#ifndef FEATURE_FS_OS_EFS_FULL_LIB

static int fs_os_qurt_efs_errno = 0;

int*
efs_get_errno_address (void)
{
  return &fs_os_qurt_efs_errno;
}

#endif
