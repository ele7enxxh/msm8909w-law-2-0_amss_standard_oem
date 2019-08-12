/***********************************************************************
 * fs_timetick.c
 *
 * TimeTick module to read time in O(microseconds).
 * Copyright (C) 2012-2014 QUALCOMM Technologies, Inc.
 *
 * TimeTick module to read time in O(microseconds)
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs/src/fs_timetick.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-04-26   vm    Use correct FS_MSG macros to save RAM.
2013-03-21   dks   Fix compiler warning from variable declaration.
2012-09-10   nr    Create

===========================================================================*/

#include "fs_timetick.h"
#include "DDITimetick.h"
#include "fs_err.h"

static DalDeviceHandle *fs_timetick_handle = NULL;

void
fs_timetick_init (void)
{
  DALResult result;
  fs_timetick_handle = NULL;

  result = DalTimetick_Attach ("SystemTimer", &fs_timetick_handle);
  if ((result != DAL_SUCCESS) || (fs_timetick_handle == NULL))
  {
    FS_MSG_ERROR_2 ("[%d,%d] EFS: DalTimetick_Attach fail", result,
                  (uint32) fs_timetick_handle);
    fs_timetick_handle = NULL;
    return;
  }

  result = DalTimetick_InitTimetick64 (fs_timetick_handle);
  if ((result != DAL_SUCCESS))
  {
    FS_MSG_ERROR_1 ("[%d] DalTimetick_InitTimetick64 fail", result);
    fs_timetick_handle = NULL;
  }
}

fs_timetick
fs_timetick_get_timetick (void)
{
  fs_timetick timetick = 0;
  DALResult result;

  if (fs_timetick_handle != NULL)
  {
    result = DalTimetick_GetTimetick64 (fs_timetick_handle, &timetick);
    if (result != DAL_SUCCESS)
    {
      FS_MSG_ERROR_1 ("[%d] DalTimetick_GetTimetick64 fail", result);
      timetick = 0;
    }
  }
  return timetick;
}

fs_time_us
fs_timetick_timetick_diff_to_us (fs_timetick timetick_difference)
{
  fs_time_us time_us = 0;
  DALResult result;

  if (fs_timetick_handle != NULL)
  {
    result = DalTimetick_CvtFromTimetick64 (fs_timetick_handle,
               timetick_difference, T_USEC, &time_us);

    if (result != DAL_SUCCESS)
    {
      FS_MSG_ERROR_1 ("[%d] DalTimetick_CvtFromTimetick64 fail", result);
      time_us = 0;
    }
  }
  return time_us;
}

