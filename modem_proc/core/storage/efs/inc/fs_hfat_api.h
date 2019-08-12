/***********************************************************************
 * fs_hfat_api.h
 *
 * Short description
 * Copyright (C) 2009, 2011 QUALCOMM Technologies, Inc.
 *
 * Verbose description.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/inc/fs_hfat_api.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2011-10-04  dks    Make EFS VU independent of Hotplug and Hfat VUs.
2009-10-08   rp    Create.

===========================================================================*/

#ifndef __FS_HFAT_API_H__
#define __FS_HFAT_API_H__

#ifdef FEATURE_FS_HOTPLUG

#include "fs_hotplug.h"

#ifdef FEATURE_EFS_EXTFS

int fs_hfat_format (struct hotplug_device *hdev);
int fs_hfat_checkfat (struct hotplug_device *hdev, int param, int *error_mask,
                      void (*callback) (void *), void *cparam);

#else /* !FEATURE_EFS_EXTFS */

static inline int
fs_hfat_format (struct hotplug_device *hdev)
{
  (void) hdev;
  return -1;
}

static inline int
fs_hfat_checkfat (struct hotplug_device *hdev, int param,
                  int *error_mask, void (*callback) (void *), void *cparam)
{
  (void) hdev;     (void) param; (void) error_mask;
  (void) callback; (void) cparam;
  return -1;
}

#endif /* FEATURE_EFS_EXTFS */

#endif /* FEATURE_FS_HOTPLUG */

#endif /* not __FS_HFAT_API_H__ */
