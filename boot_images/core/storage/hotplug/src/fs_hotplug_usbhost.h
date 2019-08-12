/***********************************************************************
 * fs_hotplug_usbhost.h
 *
 * Header file for the glue logic between hotplug and the host usb
 * stack.
 *
 * Copyright (C) 2009-2010 Qualcomm Technologies, Inc.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/hotplug/src/fs_hotplug_usbhost.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2010-06-16   rp    Featurized Hotplug USB code.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Migrate file from robusta to peaberry.
2009-05-18   rp    Fix compiler warning for 8200 which has gcc with more -Ws
2009-05-08   ebb   Create

===========================================================================*/

#ifndef __FS_HOTPLUG_USBHOST_H__
#define __FS_HOTPLUG_USBHOST_H__

#include "fs_hotplug_config_i.h"

#ifdef FEATURE_FS_HOTPLUG_USB

  #include "fs_hotplug_i.h"
  #include "usbhost_api.h"

  void hotplug_usbhost_register (void);
  void hotplug_usbhost_open (void);
  void hotplug_usbhost_close (void);

  /* Unique ID that USBHOST uses to recognize clients (like Hotplug) */
  extern usbhost_client_id_type client_id;

#else /* FEATURE_FS_HOTPLUG_USB */

  static inline void hotplug_usbhost_register (void) { }
  static inline void hotplug_usbhost_open (void) { }
  static inline void hotplug_usbhost_close (void) { }

#endif /* FEATURE_FS_HOTPLUG_USB */

#endif /* not __HOTPLUG_USBHOST_H__ */
