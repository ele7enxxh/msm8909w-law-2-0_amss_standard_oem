/***********************************************************************
 * fs_hotplug_config.h
 *
 * Hotplug configuration.
 * Copyright (C) 2009-2010 Qualcomm Technologies, Inc.
 *
 * This file will configure the Hotplug module.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/storage/fs_hotplug_config.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-04-06   wek   Avoid direct inclusion of customer header file.
2010-09-29   dks   Moved hotplug configurable variables to fs_hotplug_config*.h
2010-07-12   rp    Added defines for major and minor number for API files.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-11-16   rp    Create

===========================================================================*/

#ifndef __FS_HOTPLUG_CONFIG_H__
#define __FS_HOTPLUG_CONFIG_H__

#ifdef CUST_H
  #include CUST_H
#endif

/** Identifies the major version number of this API file. */
#define VER_FS_HOTPLUG_CONFIG_H_MAJOR     1
/** Identifies the minor version number of this API file. */
#define VER_FS_HOTPLUG_CONFIG_H_MINOR     1

#define FS_HOTPLUG_NAME_MAX          (20)


/*************************************************************************//**
 * This is the starting interval (in milliseconds) to use for polling
 * any external media. It is actually the time between polls, so if
 * the poll takes a while (MMC takes 600ms+), the overall interval
 * will be this plus the poll time.
 *
 * The target can set this value in cust, which allows a different
 * default to be used without changing this file.  This default value
 * will be used if none is previously defined.
 *
 * This interval is used at startup, until a call to
 * hotplug_set_polling_interval() changes it.
 *
 *
*****************************************************************************/
#ifndef FS_HOTPLUG_DEFAULT_POLL_INTERVAL_MS
  #define FS_HOTPLUG_DEFAULT_POLL_INTERVAL_MS   10000 /* 10 seconds */
#endif


#endif /* not __FS_HOTPLUG_CONFIG_H__ */
