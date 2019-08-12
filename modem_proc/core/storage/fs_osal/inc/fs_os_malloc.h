/***********************************************************************
 * fs_os_malloc.h
 *
 * Malloc wrapper
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fs_osal/inc/fs_os_malloc.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-05-15   dks   Create

===========================================================================*/

#ifndef __FS_OS_MALLOC_H__
#define __FS_OS_MALLOC_H__

#include "fs_os_config_i.h"
#include "comdef.h"

/**
  fs_os_malloc - Private malloc implemenation.

  Mallocs memory and can have additional checks for debugging

*/
void *fs_os_malloc (uint32 size);

/**
  fs_os_free - Private free implemenation.

  Frees memory allocated using fs_os_malloc and can have additional checks for
  debugging

*/
void fs_os_free (void *ptr);

#endif /* not __FS_OS_MALLOC_H__ */
