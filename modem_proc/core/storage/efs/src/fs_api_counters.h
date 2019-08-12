/***********************************************************************
 * fs_api_counters.h
 *
 * The API counters module.
 * Copyright (C) 2012-2013 QUALCOMM Technologies, Inc.
 *
 * This file provides api to measure api flash, timer and usage counters.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_api_counters.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-12-26   nr    Implement support of Extensive profiling.
2012-11-28   nr    Create

===========================================================================*/

#ifndef __FS_API_COUNTERS_H__
#define __FS_API_COUNTERS_H__

#include "fs_profile.h"

/* Initialize the api counters */
void fs_api_counters_init (void);

/* Marker to start profiling flash numbers.
 *  @param profile_id
 *    The enum profile id of the api being profiled.
 */
void fs_api_counters_start_flash_profile (enum fs_profile_id profile_id);

/* Marker to stop profiling flash numbers.
 *  @param profile_id
 *    The enum profile id of the api being profiled.
 */
void fs_api_counters_stop_flash_profile (enum fs_profile_id profile_id);

/* Marker to update the total time for the api.
 *  @param profile_id
 *    The enum profile id of the api being profiled.
 *
 *  @param api_time
 *    The net time spent in this particualr call of the api.
 */
void fs_api_counters_update_time (enum fs_profile_id profile_id,
                                  uint64 api_time);

/* Marker to update the count of the api.
 *  @param profile_id
 *    The enum profile id of the api being profiled.
 */
void fs_api_counters_update_count (enum fs_profile_id profile_id);

/* Flush the data structures to the disk. */
void fs_api_counters_flush_logs (void);

#endif /* not __FS_API_COUNTERS_H__ */
