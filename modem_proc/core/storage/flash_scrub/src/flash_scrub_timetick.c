/***********************************************************************
 * flash_scrub_timetick.c
 *
 * Flash-Scrub TimeTick module.
 * Copyright (C) 2013-2014 QUALCOMM Technologies, Inc.
 *
 * Flash-Scrub TimeTick module used for time-profiling.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash_scrub/src/flash_scrub_timetick.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-04-28   vm    Use correct MSG macros to save RAM.
2013-08-21   rp    Create

===========================================================================*/


#include "flash_scrub_timetick.h"
#include "flash_scrub_err.h"

#include "DDITimetick.h"

static DalDeviceHandle *flash_scrub_timetick_handle = NULL;

void
flash_scrub_timetick_init (void)
{
  DALResult result;

  flash_scrub_timetick_handle = NULL;

  result = DalTimetick_Attach ("SystemTimer", &flash_scrub_timetick_handle);
  if ((result != DAL_SUCCESS) || (flash_scrub_timetick_handle == NULL))
  {
    FLASH_SCRUB_MSG_ERR_2 ("[%d,%d] EFS: Attach fail", result,
                         (uint32) flash_scrub_timetick_handle);
    flash_scrub_timetick_handle = NULL;
    return;
  }

  result = DalTimetick_InitTimetick64 (flash_scrub_timetick_handle);
  if ((result != DAL_SUCCESS))
  {
    FLASH_SCRUB_MSG_ERR_1 ("[%d] InitTimetick64 fail", result);
    flash_scrub_timetick_handle = NULL;
  }
}

uint64
flash_scrub_timetick_get_timetick (void)
{
  uint64 timetick = 0;
  DALResult result;

  if (flash_scrub_timetick_handle != NULL)
  {
    result = DalTimetick_GetTimetick64 (flash_scrub_timetick_handle,&timetick);
    if (result != DAL_SUCCESS)
    {
      FLASH_SCRUB_MSG_ERR_1 ("[%d] GetTimetick64 fail", result);
      timetick = 0;
    }
  }
  return timetick;
}

uint64
flash_scrub_timetick_convert_to_us (uint64 timetick)
{
  uint64 time_us = 0;
  DALResult result;

  if (flash_scrub_timetick_handle != NULL)
  {
    result = DalTimetick_CvtFromTimetick64 (flash_scrub_timetick_handle,
                                            timetick, T_USEC, &time_us);
    if (result != DAL_SUCCESS)
    {
      FLASH_SCRUB_MSG_ERR_1 ("[%d] CvtFromTimetick64 fail", result);
      time_us = 0;
    }
  }
  return time_us;
}
