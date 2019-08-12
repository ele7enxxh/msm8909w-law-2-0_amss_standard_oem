#ifndef RF_COMMON_TIME_PROFILE_H
#define RF_COMMON_TIME_PROFILE_H

/*!
   @file
   rfcommon_time_profile.h

   @brief
   This file contains structures for time profiling.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_time_profile.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/17/13   aro     Temporarily added scratch pad for compiler warning removal
04/17/13   aro     Cleaned up unused time profiler code
03/05/13   aro     Migrated to XO for time profiling
03/05/13   aro     Created common time snapshot and conversion functions to
                   be used by FTM timer
01/03/13   aro     Support to fallback to timetick for the case when 
                   hardware time counter is not running
08/22/12   aro     Added inteface to calculate the difference between the
                   start and end time ticks
08/22/12   aro     Renamed interface names and type definitions
07/30/12   cvd     Implement the  rf_timetick_get and rf_timetick_get_difference 
                   interfaces. 
07/19/12   cvd     Initial version

==============================================================================*/

#include "comdef.h"
#include "DDITimetick.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Type definition for DAL 64-bit time tick */
typedef DalTimetickTime64Type rf_time_tick_fine;

/*----------------------------------------------------------------------------*/
/*! Type definition for coarse time tick */
typedef uint32 rf_time_tick_coarse;

/*----------------------------------------------------------------------------*/
/*! Data type for time value */
typedef uint64 rf_time_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data type for time ticks

  @details
  It contains 'coarse_timetick' which stores the coarse timeticks count and 
  fine_timeticks which stores the fine timeticks count. These will be used 
  to calculate the total time elapsed. The hardware timer (fine timer) may wrap
  around before the profiling is complete. That is why we use the software timer
  (coarse timer) to calculate the number of wraparounds, to get the actual time
  difference. This structure is opaque to the client, who does not have to deal
  with these details.
*/
typedef struct
{
  rf_time_tick_fine fine_timetick;
  /*!< Save the fine timeticks count */

  uint8 sratch_pad;
  /*!< Scratch Pad */
} rf_time_tick_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enum for units of time
*/
typedef enum 
{
  RF_TICK,
  /*!< Return time in Ticks */

  RF_USEC,
  /*!< Return time in Microseconds */

  RF_MSEC,
  /*!< Return time in Milliseconds */

  RF_SEC,
  /*!< Return time in Seconds */

  RF_MIN,
  /*!< Return time in Minutes */

  RF_HOUR,
  /*!< Return time in Hours */

}rf_time_unit_type;

/*----------------------------------------------------------------------------*/
rf_time_tick_fine
rf_time_get_timetick64_snapshot
(
  void
);

/*----------------------------------------------------------------------------*/
uint64
rf_time_xotick_to_microsecond
(
  uint64 rf_time_tick
);

/*----------------------------------------------------------------------------*/
uint64
rf_time_microsecond_to_xotick
(
  uint64 rf_time_us
);

/*----------------------------------------------------------------------------*/
rf_time_tick_type
rf_time_get_tick
(
  void
);

/*----------------------------------------------------------------------------*/
rf_time_type
rf_time_get_elapsed
(
  rf_time_tick_type start,
  rf_time_unit_type unit
);

/*----------------------------------------------------------------------------*/
rf_time_type
rf_time_get_difference
(
  rf_time_tick_type start,
  rf_time_tick_type end,
  rf_time_unit_type unit
);

#ifdef __cplusplus
}
#endif

#endif /* RF_COMMON_TIME_PROFILE_H */