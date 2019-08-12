/***********************************************************************
 * fs_async_put_config.h
 *
 * efs_async_put module configuration settings.
 * Copyright (C) 2011-2012 Qualcomm Technologies, Inc.
 *
 * This file contains all of the configurable parameters used in efs_async_put
 * module.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/api/storage/fs_async_put_config.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-20   wek   Cleanup customer header file inclusion.
2011-11-21   dks   Create

===========================================================================*/

#ifndef __FS_ASYNC_PUT_CONFIG_H__
#define __FS_ASYNC_PUT_CONFIG_H__

#include "fs_config.h"

/** Identifies the major version number of this API file. */
#define VER_FS_ASYNC_PUT_CONFIG_H_MAJOR     1
/** Identifies the minor version number of this API file. */
#define VER_FS_ASYNC_PUT_CONFIG_H_MINOR     1

/**********************************************************************
 * CONFIGURATION ITEMS
 *
 * These parameters can be configured as desired.  Note that changing their
 * values doesn't affect existing files in the filesystem.  If a build is
 * loaded with smaller parameters for these values than earlier builds,
 * files may become inaccessible.
 */

/**
 * Maximum size of a item file that can be successfully queued to be written.
 * Since each command structure will store the full data, increase this value
 * with caution if memory usage needs to be kept low.
 */
#ifndef FS_ASYNC_PUT_MAX_DATA_BYTES
  #define FS_ASYNC_PUT_MAX_DATA_BYTES 1024
#endif


/**
 * Maximum number of outstanding efs_async_put requests that can be queued.
 * Each command structure will store the full path and data requested to be
 * written. So the size of each instance can be 2k bytes or more based on
 * FS_PATH_MAX and FS_ASYNC_PUT_MAX_DATA_BYTES values.
 * Keep caution not to increase the memory footprint on low memory config
 * targets by increasing this value to a high value.
 */
#ifndef FS_ASYNC_PUT_CMD_QUEUE_SIZE
  #define FS_ASYNC_PUT_CMD_QUEUE_SIZE   10
#endif


#endif /* not __FS_ASYNC_PUT_CONFIG_H__ */
