#ifndef TIME_SVC_H
#define TIME_SVC_H

/**
@file time_svc.h
@brief
Implements time-keeping functions using various sources.

INITIALIZATION AND SEQUENCING REQUIREMENTS\n
time_init( ) must be called to initalize the time subsystem state variables
 and install necessary ISRs.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The ATS_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      ATS_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the time_remote group 
      description in the ATS_mainpage.dox file. 
=============================================================================*/

/*=============================================================================
  Copyright (c) 2003-2005, 2008-2014 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*==============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements time-keeping functions using various sources.


EXTERNALIZED FUNCTIONS
  time_init( )
    Initializes the time services subsystem

  time_to_ms( )
    Converts a time stamp into milliseconds.

  time_get( )
    Retrieves time according to CDMA, HDR, GSM, or slow-clock Time-of-Day

  time_get_ms( )
    Retrieves system time in milliseconds

  time_get_sec( )
    Retrieves system time, in seconds

  time_get_uptime_ms( )
    Retrieves the up-time, in milliseconds

  time_get_uptime_secs( )
    Retrieves the up-time, in seconds

  time_sync_time_of_day( )
    Syncs the current time, from CDMA, HDR, or GSM, to the slow-clock based
    TOD.

  time_set_from_pmic( )
    Set the time-of-day from the PMIC RTC (if present)

  time_to_ms_native()
    Convert the current time to milliseconds in native uint64 format.

  time_get_ms_native()
    Get the current time in milliseconds in native uint64 format.

  time_mpm_sync()
    Computes the offset between modem processor and MPM sclk counters.

 time_restart()
    Computes the offset between modem processor and MPM sclk counters.
    Readjusts the timetick counter values to sync up with the base timetick 
    counter when waking up from power collapse.

REGIONAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  time_init( ) must be called to initalize the time subsystem state variables
  and install necessary ISRs.

=============================================================================*/


/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/time_svc.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/29/14   abh     Added optimized call to get time using time_get_optimized_till_msecs.
11/29/10   scd     (Tech Pubs) Edited Doxygen markup and comments.
10/27/10   EBR     Doxygenated File.
02/22/10   din     Removed function time_get_local_secs().
12/23/09   din     Changed time_to_ms[sec] to use and time_conv__to_sec().
12/05/09   din     Moved definition of time_type to time_types.h
11/25/09   din     Added time_mpm_sync and time_restart
06/12/09   din     Added time_get_apps_user_time
02/23/09   din     Consoldation of time. Added functions 
                   time_get/set_julian() and time_to_sec().
12/15/08   tev     Removed time_compute_power_on from time_svc.h to private
                   time_svc_v.h header file. 
08/19/08   jhs     Removed reference to time.power_on_sec
11/18/05   mod/ajn Added MediaFLO time support.
05/10/05   ajn     Added time_set_from_pmic( ) function, to correct init order.
11/24/04   grl     Added support for getting time in ms in native uint64 types.
02/09/04   ajn     Renamed, to avoid conflict with WinCE test environment
08/08/03   ajn     Changed timestamp type from qword to time_type (a qword)
08/01/03   ajn     Moved _get_sec, get_ms from time_tod.  Added _get_local_sec
07/24/03   ajn     File created.

=============================================================================*/



/*==============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "qw.h"
#include "time_jul.h"

/*==============================================================================

                           DATA DEFINITIONS

=============================================================================*/


/*-----------------------------------------------------------------------------

                      SYSTEM TIMESTAMP FORMAT


   |<------------ 48 bits --------------->|<----- 16 bits ------->|
   +--------------------------------------+-----------------------+
   |      1.25 ms counter                 |   1/32 chip counter   |
   +--------------------------------------+-----------------------+
         (11K years of dynamic range)          (1.25 ms dynamic
                                                range. Rolls over
                                                at count 49152)

-----------------------------------------------------------------------------*/

/** @addtogroup time_services
@{ */

/*-----------------------------------------------------------------------------
  Time Source, returned by time_get( )
-----------------------------------------------------------------------------*/
/** Identifies the time source type.
*/
typedef enum
{
  TIME_SOURCE_32KHZ, /**< Time is based on the 32-kHz slow clock. */
  TIME_SOURCE_CDMA,  /**< Time is based on the CDMA concept of time. */
  TIME_SOURCE_HDR,   /**< Time is based on the HDR concept of time. */
  TIME_SOURCE_GSM,   /**< Time is based on the GSM concept of time. */
  TIME_SOURCE_WCDMA, /**< Time is based on the WCDMA concept of time. */
  TIME_SOURCE_GPS,   /**< Time is based on the GPS concept of time. */
  TIME_SOURCE_MFLO   /**< Time is based on the MediaFLO concept of time. */
}
time_source_enum_type;



/*=============================================================================

                            FUNCTION DEFINITIONS

=============================================================================*/

/**
Computes the offset between the modem processor and the MPM slow-clock counters. 
The MPM and the modem processor slow-clock hardware counters, while both count 
slow clocks, have an offset with regard to each other due to the modem 
processor power collapse. This difference is eliminated by adding an offset to 
the value read from the modem processor slow-clock counter.

@return
None.
 
@dependencies  
This is a regional function that can only be called from time*.c.
*/
void time_mpm_sync( void );



/**
Computes the offset between the modem processor and the MPM slow-clock 
counters. Readjusts the timetick counter values to synchronize with the base 
timetick counter when waking up from power collapse.

@return
None.
 
@dependencies
None.
*/
void time_restart( void );


/**
Initializes the timekeeping subsystem. This installs the timekeeping ISR,
timers, etc.

@return
None.
 
@dependencies  
None.
*/
void time_init( void );


/**
Registers for callback registration for time_tod remote updates.

@return
None.

@dependencies  
None.
*/
void time_init_config( void );


/**
Converts a timestamp from system time to millisecond units.

@param[in,out] time Value to be converted. The value is converted in place.

@return
None.

@dependencies  
None.
*/
void time_to_ms
(
  time_type                       time
);


/**
Converts a timestamp from system time to seconds units.

@param[in] ts_val Timestamp to be converted.

@return
The time in seconds.

@dependencies  
None.
*/
uint32 time_to_sec
(
  time_type                       ts_val
);

/**

FUNCTION time_get_sum_genoff_ts

DESCRIPTION
  Returns sum generic offset value of system base in timestamp format

DEPENDENCIES
  None

RETURN VALUE
  boolean as return value
  Provides sum generic ts offset value in sum_genoff_ts_val

SIDE EFFECTS
  None

**/
boolean time_get_sum_genoff_ts
( 
    time_type        sum_genoff_ts_val
);

/**
Returns the current time.

@param[out] ts_val Location to place the timestamp.

@return
Source of the time used.

@dependencies  
A valid time (CDMA, HDR, TOD, etc.).
*/
time_source_enum_type time_get
(
  time_type                       ts_val
);

/**
Returns the current time in msecs resolution.

@param[out] ts_val Location to place the timestamp.

@return
Source of the time used.

@dependencies  
A valid time (CDMA, HDR, TOD, etc.).
*/
time_source_enum_type time_get_optimized_till_msec
(
  time_type                       ts_val
);

/**
Get the system time in the number of milliseconds since the epoch.

@param[out] time Location to place the system time in millisecond format.

@return
None.

@dependencies  
A valid time (CDMA, HDR, TOD, etc.).
*/
void time_get_ms
(
  time_type                       time
);


/**
Gets the system time in the number of seconds since the epoch.

@return
The time in seconds since 6 Jan 1980 00:00:00.
 
@dependencies  
A valid time (CDMA, HDR, TOD, etc.).
*/
uint32 time_get_secs( void );


/**
Returns the time uptime of the system in timestamp format.

@param[out] ts_val Location to place the timestamp.

@return
Source of the time used.

@dependencies  
*/
time_source_enum_type time_get_uptime
(
  time_type                       ts_val
);

/**
Gets the duration of time the phone has been powered on.

@return
The number of seconds the phone has been powered on.
 
@dependencies  
A valid time (CDMA, HDR, TOD, etc.).
*/
uint32 time_get_uptime_secs( void );


/**
Gets the duration of time in milliseconds the phone has been powered on.

@param[out] uptime_ms Location to place the number of milliseconds the phone 
                      has been powered on.
 
@return
None.

@dependencies  
A valid time (CDMA, HDR, TOD, etc.).
*/
void time_get_uptime_ms
(
  time_type                       uptime_ms
);


/**
Synchronizes the slow-clock-based TOD to the current timestamp time.

@return
None.

@dependencies  
A valid time (CDMA, HDR, GSM, GPS, WCDMA, etc.). \n
time_get( ) must not be using time_tod_get( ) when this function is called.

@sideeffects 
A diagnostic message is generated if there is a sudden jump in TOD.
*/
void time_sync_time_of_day( void );


/**
Initializes the slow-clock-based TOD to the PMIC RTC time.

@return
None.
 
@dependencies  
TOD must not have been initialized by another time source.
*/
void time_set_from_pmic( void );


/**
Converts a timestamp from system time to millisecond units.

@param[in] time_stamp System time to be converted.

@return
The converted time in milliseconds.

@dependencies  
None.
*/
uint64 time_to_ms_native
(
  time_type                       time_stamp
);


/**
Gets a timestamp from system time in millisecond units from 6 Jan 1980 00:00:00.

@return
The time in milliseconds from the epoch.

@dependencies
None.
*/
uint64 time_get_ms_native( void );


/**
Sets the system time.

@param[in] jul_time Pointer to the time to be set in Julian format.
 
@return
None.

@dependencies  
A valid time (CDMA, HDR, TOD, etc.).
*/
void time_set_julian( time_julian_type *jul_time );

/**
Gets the applications processor user time.

@param[out] ts_val Location to place the current time.

@return
None.
 
@dependencies  
Used for Windows<sup>&reg;</sup> Mobile only. \n
This function returns the same as time_get on BREW.
*/
void time_get_apps_user_time
(
  time_type                       ts_val
);


/**
Gets the system time in Julian format.

@param[out] jul_time Pointer to the location to store the Julian-formatted 
                     system time.
 
@return
None.

@dependencies  
A valid time (CDMA, HDR, TOD, etc.).
*/
void time_get_julian
(
  time_julian_type *jul_time 
);


/**
Number of milliseconds since the epoch to which the system time needs to be set.

@param[in] a_time Time in milliseconds since 6 Jan 1980 00:00:00.
 
@dependencies  
A valid time (CDMA, HDR, TOD, etc.).
*/
void time_set_ms
(
  time_type                       a_time
);

/** @} */ /* end_addtogroup time_svc */

#endif /* TIME_H */

