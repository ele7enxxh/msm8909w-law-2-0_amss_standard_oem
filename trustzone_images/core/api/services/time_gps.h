#ifndef TIME_GPS_H
#define TIME_GPS_H

/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M
                        G P S   T I M E   K E E P I N G

GENERAL DESCRIPTION
  Implements GPS time-keeping functions using the GPS data


EXTERNALIZED FUNCTIONS
  time_gps_sync80_event
    Set the system time at sync80 at the given timetick.

  time_gps_set_at
    Set the system time at any arbitrary point in time at the given timetick.

  time_gps_get
    Return system time based on the latest GPS time

  time_gps_wake
    Called to start the GPS timekeeping module.

  time_gps_sleep
    Called when GPS timekeeping is turned off.
  time_gps_get_base
    Read base time stamp based on GPS
    
  time_gps_is_sync80_sync
    Query whether the current GPS base time is sync80 synchronized

INITIALIZATION AND SEQUENCING REQUIREMENTS
  To start, time_gps_wake must be called by GPS module to start the time
  keeping module. Then the sync80 ISR will start calling time_gps_sync80_event
  to increment time given the time elapsed. From then, the GPS time becomes valid.
  In a power up case where no system time is available, GPS PE may set
  the time using time_gps_set_at once it acquires time through GPS demodulation.
  When the GPS tracking session ends, then time_gps_sleep will be called.

Copyright (c) 2003 - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/time_gps.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/23/09   din     Consolidation of time.
06/11/07   aar     Added declaration for time_gps_sync80_event().
09/20/05   ajn/cr  Added GPS based time.
07/18/05   cr      Original check in

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_svc.h"
#include "timetick.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

FUNCTION TIME_GPS_SYNC80_EVENT

DESCRIPTION
  This function should be called by sync80 ISR to maintain the sync80-based
  system time. The input is how much time has elapsed since the last sync80
  where this function was called, and the associated slow clock tick.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initializes the system timestamp and marks GPS time as valid 
  and sync80 synced.

=============================================================================*/

void time_gps_sync80_event
(
  /* Time elapsed since last sync80 where this function was called */
  uint32                          time_elapsed_cx8,

  /* Corresponding timetick value */
  timetick_type                   ticks
);

/*=============================================================================

FUNCTION TIME_GPS_SET_AT_SYNC80

DESCRIPTION
  This function sets the system time based on the GPS time.
  The time stamp by GPS is the time at the given timetick value.
  This function should only be called by GPSSRCH RTCDB module
  which increments time based on CDMA RTC at every sync80. 
  The time, ts_val, at "ticks" should represent the true sync80 time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initializes the system timestamp and marks GPS time as valid 
  and sync80 synced.

=============================================================================*/

void time_gps_set_at_sync80
(
  /* Timestamp to set time of day to */
  time_type                       ts_val,

  /* Corresponding timetick value */
  timetick_type                   ticks
);



/*=============================================================================

FUNCTION TIME_GPS_SET_AT

DESCRIPTION
  This function sets the system time based on the GPS time.
  The time stamp by GPS is the time at the given timetick value.
  This function should only be called by PE when it acquires the
  GPS time through demodulation. The GPS base time that is set
  using this function may or may not be sync80 synchronized.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initializes the system timestamp and marks GPS time as valid but not sync80
  synced.

=============================================================================*/

void time_gps_set_at
(
  /* Timestamp to set time of day to */
  time_type                       ts_val,

  /* Corresponding timetick value */
  timetick_type                   ticks
);



/*=============================================================================

FUNCTION TIME_GPS_GET

DESCRIPTION
  This procedure is called to read system timestamp based on the latest GPS 
  time.

DEPENDENCIES
  GPS time keeping must be active in order to for a good value to be returned.
  This function does not fall back on "Time Of Day".

RETURN VALUE
  TRUE if GPS system time is available, FALSE otherwise.
  Parameter ts_val is set to the current system time, in timestamp format.

SIDE EFFECTS
  None

=============================================================================*/

boolean  time_gps_get
(
  /* Address of qword in which to return 64-bit system timestamp. */
  time_type                       ts_val
);



/*=============================================================================

FUNCTION TIME_GPS_SLEEP

DESCRIPTION
  Invalidates GPS system time, in preparation for end of GPS tracking session.

DEPENDENCIES
  The GPS system clock must be accurate when this function is called.

RETURN VALUE
  The next master time-keeper after GPS time keeping is asleep.

SIDE EFFECTS
  Updates the slow-clock based "Time of Day" before invalidating GPS time.

=============================================================================*/

time_source_enum_type time_gps_sleep( void );



/*=============================================================================

FUNCTION TIME_GPS_WAKE

DESCRIPTION
  This function wakes up the GPS time keeping module. The time is not valid
  yet, the time will be valid when time_gps_set_at_sync80 is called.

DEPENDENCIES
  Since currently the GPS time is synced to s80, 1X PN Roll ISR must be on.

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/

void time_gps_wake( void );



/*=============================================================================

FUNCTION TIME_GPS_GET_BASE

DESCRIPTION
  This procedure is called to read base time stamp based on GPS. The base 
  time stamp is currently the sync80 time stamp (which can be GPS epoch ts).

DEPENDENCIES
  GPS time keeping must be active in order for a good value to be returned.
  This function does not fall back on "Time Of Day".

RETURN VALUE
  TRUE if GPS system time is available, FALSE otherwise.
  Parameter ts_val is set to the current system time, in timestamp format.

SIDE EFFECTS
  None

=============================================================================*/

boolean  time_gps_get_base
(
  /* Address of qword in which to return 64-bit system timestamp. */
  time_type                       ts_val
);



/*=============================================================================

FUNCTION TIME_GPS_IS_SYNC80_SYNC

DESCRIPTION
  This function queries whether the current GPS base time is sync80 
  synchronized.

DEPENDENCIES  
  None

RETURN VALUE  
  TRUE if GPS base time is sync80 synchronized, FALSE otherwise

SIDE EFFECTS
  None

=============================================================================*/

boolean  time_gps_is_sync80_sync( void );


#endif /* TIME_GPS_H */

