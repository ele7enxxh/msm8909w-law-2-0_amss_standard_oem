/*!
   @file
   rfcommon_time_profile.c

   @brief
   This file contains functions for time profiling.

   @details
   The functions here are used to profile execution time between points in a 
   program.
 
   @addtogroup RF_COMMON_TIME_PROFILE
   @{
*/

/*==============================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/rfcommon_time_profile.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/17/13   aro     Temporarily added scratch pad for compiler warning removal
04/17/13   aro     Cleaned up unused time profiler code
03/014/13  sar     Fixed compiler errors in APQ target. 
03/11/13   sar     Updated file for APQ, GNSS only target.
03/05/13   aro     Migrated to XO for time profiling
03/05/13   aro     Created common time snapshot and conversion functions to
                   be used by FTM timer
01/18/13   aro     Added code to fallback to timetick
01/03/13   aro     Support to fallback to timetick for the case when 
                   hardware time counter is not running
08/22/12   aro     Renamed time profling functions
08/22/12   aro     Added implementation for rf_time_get_difference
08/22/12   aro     Added inteface to calculate the difference between the
                   start and end time ticks
08/22/12   aro     Renamed interface names and type definitions
07/30/12   cvd     Implement the  rf_timetick_get and rf_timetick_get_difference 
                   interfaces. 
07/19/12   cvd     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfcommon_time_profile.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "rfcommon_msm.h"
#endif
#include "rfcommon_msg.h"
#include "DalDevice.h"

/*----------------------------------------------------------------------------*/
/*! Pointer to Dal Handle */
static DalDeviceHandle* rf_timetick_handle = NULL;

/*----------------------------------------------------------------------------*/
/*! Frequency of clock MHz*10 */
#define CLOCK_FREQ_MHZ_X10 192

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the time elapsed between start and end using timetick

  @details
  This function returns the amount of time elapsed between the time the ticks 
  were set in 'start' and 'end'. 
 
  @param start
  The time counts set at the start of profiling.
 
  @param end
  The time counts set at the end of profiling.
 
  @param unit
  The units in which the time should be returned.

  @return
  The amount of time elapsed between 'start' and 'end' in the units specified
  by 'unit'.
*/
static rf_time_type
get_rf_timetick64_difference
(
   rf_time_tick_type start,
   rf_time_tick_type end,
   rf_time_unit_type unit
)
{
  rf_time_type prof_time = 0;
  rf_time_tick_fine tick_fine_diff = 0;
  rf_time_type prof_time_us = 0;
   
  /* Attach to Timetick DAL */ 
  if ( rf_timetick_handle == NULL ) 
  {
    RF_MSG( RF_ERROR, "get_rf_timetick64_difference: DAL timetick "
                      "Handle NULL" );
    return 0;
  } /* if ( rf_timetick_handle == NULL )  */ 

  if ( end.fine_timetick < start.fine_timetick )
  {
    RF_MSG_2( RF_ERROR, "get_rf_timetick64_difference: END (0x%x) cannot be "
              "smaller than START (0x%x)", end.fine_timetick, 
              start.fine_timetick );
    return 0;
  } /* if ( end <= start ) */

  tick_fine_diff = end.fine_timetick - start.fine_timetick;

  /* Convert to microseconds */
  prof_time_us = rf_time_xotick_to_microsecond(tick_fine_diff);

  switch (unit)
  {
     case RF_USEC:
       prof_time = prof_time_us;
       break;
     case RF_MSEC:
       prof_time = prof_time_us / 1000;
       break;
     case RF_SEC:
       prof_time = prof_time_us / 1000000;
       break;
     case RF_MIN:
       prof_time = prof_time_us / 60000000;
       break;
     case RF_HOUR:
       prof_time = prof_time_us / 360000000;
       break;
     case RF_TICK:
     default:
       prof_time = tick_fine_diff;
       break;
  }

  return prof_time;

} /* get_rf_timetick64_difference */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Time snapdhot
 
  @details
  This function gets the current time snapshot
  
  @return
  Time tick Status of snapshot.
*/
rf_time_tick_fine
rf_time_get_timetick64_snapshot
(
  void
)
{
  DALResult dal_res;
  rf_time_tick_fine rf_tick = 0;
  /*!< Handle to be used by ATS timer to perform timetick operation */
  
  /* Attach to Timetick DAL */ 
  if ( rf_timetick_handle == NULL ) 
  {
    dal_res = DalTimetick_Attach( "SystemTimer", &(rf_timetick_handle) );

    if ( ( dal_res != DAL_SUCCESS ) || ( rf_timetick_handle == NULL ) )
    {
      RF_MSG( RF_ERROR, "rf_time_get_timetick64_snapshot: DAL timetick"
                        "attach failure" );
      return 0;
    } /* if ( ( dal_res != DAL_SUCCESS ) || ( rf_timetick_handle == NULL ) )*/
  } /* if ( rf_timetick_handle == NULL )  */ 

  /* Get the one-time reference time. All the future strobes will be
  based on this reference time to avoid time drift issue. */
  if ( DalTimetick_GetTimetick64( rf_timetick_handle, &rf_tick ) != 
       DAL_SUCCESS )
  {
    RF_MSG( RF_ERROR, "rf_time_get_timetick64_snapshot: Reference"
                      "Time read failed" );
    return 0;
  } /* if (DalTimetick_GetTimetick64( rf_timetick_handle,
                                     &rf_tick ) != DAL_SUCCESS ) */

  return rf_tick;
} /* rf_time_get_timetick64_snapshot */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Convert XO tick to microseconds
 
  @details
  This funtion will convert the given time in XO ticks to microseconds.
  Assumption is that XO is running at 19.2 MHz
 
  @param rf_time_us
  RF time in XO ticks to be converted to microsecodns
 
  @return
  RF time in microseconds
*/
uint64
rf_time_xotick_to_microsecond
(
  uint64 rf_time_tick
)
{
  uint64 rf_time_us = 0;

  /* Convert to microseconds */
  rf_time_us = (rf_time_tick*10) / CLOCK_FREQ_MHZ_X10;

  return rf_time_us;

} /* rf_time_xotick_to_microsecond */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Convert microseconds to XO tick
 
  @details
  This funtion will convert the given time in microseconds to XO ticks.
  Assumption is that XO is running at 19.2 MHz
 
  @param rf_time_us
  RF time in microseconds to be converted to tick
 
  @return
  XO ticks
*/
uint64
rf_time_microsecond_to_xotick
(
  uint64 rf_time_us
)
{
  uint64 rf_time_tick = 0;

  /* Convert to ticks */
  rf_time_tick = (rf_time_us * CLOCK_FREQ_MHZ_X10) / 10;

  return rf_time_tick;

} /* rf_time_microsecond_to_xotick */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the current coarse and fine time counts.

  @details
  This function returns the current coarse and fine time count. The coarse time
  count is required as the fine time count might wrap around before the 
  profiling is complete. Thus we use the coarse time count to calculate the 
  number of wraparounds of the fine timer so that we can calculate the actual
  time difference. This calculation is opaque to the client who gets the final
  time value on calling 'rf_timetick_difference()'.

  @return
  The current coarse and fine time counts.
*/
rf_time_tick_type
rf_time_get_tick
(
  void
)
{
  rf_time_tick_type rf_timetick;

  rf_timetick.fine_timetick = 0;
  rf_timetick.sratch_pad = 0;

  #ifndef FEATURE_GNSS_ONLY_NO_WWAN
  rf_timetick.fine_timetick = rf_time_get_timetick64_snapshot();
  #endif /* FEATURE_GNSS_ONLY_NO_WWAN */

  return rf_timetick;

} /* rf_time_get_tick */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the time elapsed from 'start'.

  @details
  This function returns the amount of time elapsed from the time the ticks were
  set in 'start'. The coarse time count is required as the fine time count might
  wrap around before the profiling is complete. Thus we use the coarse time
  count to calculate the number of wraparounds of the fine timer so that we can
  calculate the actual time difference.
 
  @image html rfcommon_time_profile.jpg
 
  @param start
  The time counts set at the start of profiling.

  @param unit
  The units in which the time should be returned.

  @return
  The amount of time elapsed from 'start' in the units specified by 'unit'.
*/
rf_time_type
rf_time_get_elapsed
(
  rf_time_tick_type start,
  rf_time_unit_type unit
)
{
  rf_time_type rf_time = ~0;

  #ifndef FEATURE_GNSS_ONLY_NO_WWAN
  rf_time = get_rf_timetick64_difference( start, 
                                          rf_time_get_tick(),
                                          unit );
  #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
  return rf_time;

} /* rf_time_get_elapsed */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the time elapsed between start and end.

  @details
  This function returns the amount of time elapsed between the time the ticks 
  were set in 'start' and 'end'. The coarse time count is required as the fine 
  time count might wrap around before the profiling is complete. Thus we use the 
  coarse time count to calculate the number of wraparounds of the fine timer so 
  that we can calculate the actual time difference.
 
  @param start
  The time counts set at the start of profiling.
 
  @param end
  The time counts set at the end of profiling.
 
  @param unit
  The units in which the time should be returned.

  @return
  The amount of time elapsed between 'start' and 'end' in the units specified
  by 'unit'.
*/
rf_time_type
rf_time_get_difference
(
  rf_time_tick_type start,
  rf_time_tick_type end,
  rf_time_unit_type unit
)
{
  rf_time_type rf_time = ~0;

  #ifndef FEATURE_GNSS_ONLY_NO_WWAN
  rf_time = get_rf_timetick64_difference( start, end, unit );
  #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
  return rf_time;

} /* rf_time_get_elapsed */


/*! @} */


