/***********************************************************************
 * fs_timetick.h
 *
 * TimeTick module to read time in O(microseconds).
 * Copyright (C) 2012-2013 QUALCOMM Technologies, Inc.
 *
 * TimeTick module to read time in O(microseconds).
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_timetick.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-04-03   wek   Featurize timetick module and provide stubs when disabled.
2012-09-10   nr    Create

===========================================================================*/

#ifndef __FS_TIMETICK_H__
#define __FS_TIMETICK_H__

#include "comdef.h"

typedef uint64 fs_timetick;
typedef uint64 fs_time_us;

#ifndef FEATURE_EFS_DISABLE_TIMETICK

/* Initialize TimeTick module. */
void fs_timetick_init (void);

/* Returns current timetick in microseconds, or 0 in case of any error. */
fs_timetick fs_timetick_get_timetick (void);

/* Returns time-difference in microseconds, or 0 in case of any error. */
fs_time_us fs_timetick_timetick_diff_to_us (fs_timetick timetick_difference);

#else

/* Stub implementation of Timetick module, used when module is disabled. */

static inline void fs_timetick_init (void)
{
}

static inline fs_timetick fs_timetick_get_timetick (void)
{
  return 0;
}

static inline fs_time_us
fs_timetick_timetick_diff_to_us (fs_timetick timetick_difference)
{
  (void) timetick_difference;
  return 0;
}

#endif

#endif /* not __FS_TIMETICK_H__ */

