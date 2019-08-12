#ifndef TIME_SVC_H
#define TIME_SVC_H
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
    Syncs the current time, from CDMA, HDR, or GSM, to the slow clock based
    time-of-day.

  time_set_from_pmic( )
    Set the time-of-day from the PMIC's RTC (if present)

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


Copyright (c) 2003 - 2005, 2008 - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/time_svc.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
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

/**
@mainpage Time Services CDMA
Implements time-keeping functions using various sources. 
*/

/**
@defgroup Time Services CDMA  - Public Interface
This section contains definitions for various public interface defined 
by this module.
*/

/** 
@ingroup Functions for Time  - Public Interface
@file time_svc.h
This file contains public interface defined by this module
*/


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

/*-----------------------------------------------------------------------------
  Time Source, returned by time_get( )
-----------------------------------------------------------------------------*/
/**  Enums for time source type.                   */
typedef enum
{
  /** Time was from 32kHz slow clock */
  TIME_SOURCE_32KHZ,              
  /** Time was from CDMA's concept of time */
  TIME_SOURCE_CDMA,               
  /** Time was from HDR's concept of time */
  TIME_SOURCE_HDR,                
  /** Time was from GSM's concept of time */
  TIME_SOURCE_GSM,                
  /** Time was from WCDMA's concept of time */
  TIME_SOURCE_WCDMA,              
  /** Time was from GPS's concept of time */
  TIME_SOURCE_GPS,                
  /** Time was from MFLO's concept of time */
  TIME_SOURCE_MFLO                
}
time_source_enum_type;



/*=============================================================================

                            FUNCTION DEFINITIONS

=============================================================================*/

/** 
@ingroup Functions for Time
         
@brief  
 Computes the offset between modem processor and MPM sclk counters.      
 The MPM and modem processors' sclk h/w counters, while both count slow  
 clocks, will have an offset w.r.t. each other, due to modem processor   
 power collapse.  This difference is eliminated by adding an offset to   
 the value read from the modem processor's sclk counter.                 \n\n

@param[in]                                                               
 None                                                                  \n\n  
 
@dependencies  
 Regional function.  It may only be called from time*.c                  \n\n 
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None																	 \n\n

*/
void time_mpm_sync( void );



/** 
@ingroup Functions for Time
         
@brief  
 Computes the offset between modem processor and MPM sclk counters.     
 Readjusts the timetick counter values to sync up with the base timetick 
 counter when waking up from power collapse.                            \n\n

@param[in]                                                               
 None                                                                  \n\n  
 
@dependencies  
 None                                                                   \n\n 
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None																	 \n\n

*/
void time_restart( void );



/** 
@ingroup Functions for Time
         
@brief  
 Initialize Timekeeping Subsystem                                       \n\n

@param[in]                                                               
 None                                                                  \n\n  
 
@dependencies  
 None                                                                   \n\n 
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 Installs timekeeping ISR, timers, etc.              					 \n\n

*/

void time_init( void );


/** 
@ingroup Functions for Time
         
@brief  
 Registers for Callback Registration for Time_tod updates remote       
 updates                                                               \n\n

@param[in]                                                               
 None                                                                  \n\n  
 
@dependencies  
 None                                                                   \n\n 
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None                                               					 \n\n

*/

void time_init_config( void );



/** 
@ingroup Functions for Time
         
@brief  
 Convert a timestamp from System Time to millisecond units             \n\n

@param[in]                                                               
 time_type The converted value is stored in the 'time' parameter.     \n\n  
 
@dependencies  
 None                                                                   \n\n 
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None                                               					 \n\n

*/
void time_to_ms
(
  time_type                       time
);


/** 
@ingroup Functions for Time
         
@brief  
 Convert a timestamp from System Time to second units                  \n\n

@param[in]                                                               
 time_type - Time in time-stamp format;                                \n\n  
 
@dependencies  
 None                                                                   \n\n 
 
@param[out]
 uint32 time in seconds                                                 \n\n
 
@sideffects 
 None                                               					 \n\n

*/

uint32 time_to_sec
(
  time_type                       ts_val
);


/** 
@ingroup Functions for Time
         
@brief  
 Returns the current time                                              \n\n

@param[in]                                                               
 time_type Time-stamp returned to ts_val parameter                     \n\n
 
@dependencies  
 A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.   \n\n 
 
@param[out]
 time_source_enum_type - Source of time                                 \n\n

@sideffects 
 None                                               					 \n\n

*/
time_source_enum_type time_get
(
  time_type                       ts_val
);


/** 
@ingroup Functions for Time
         
@brief  
 Get the system time,in # of milliseconds since "the beginning of time"\n\n

@param[in]                                                               
 time_type msec time-stamp returned to ts_val parameter                
                Milliseconds since 6 Jan 1980 00:00:00 returned to the 
				time parameter                                         \n\n
 
@dependencies  
 A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.   \n\n 
 
@param[out]
  None                                                                   \n\n

@sideffects 
 None                                               					 \n\n

*/
void time_get_ms
(
  time_type                       time
);


/** 
@ingroup Functions for Time
         
@brief  
 Get the system time, in # of seconds since "the beginning of time".   
 36 year range, beginning 6 Jan 1980 00:00:00.                         \n\n

@param[in]                                                               
 None                                                                 \n\n
 
@dependencies  
 A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.   \n\n 
 
@param[out]
 uint32 - Returns time in seconds since 6 Jan 1980 00:00:00             \n\n

@sideffects 
 None                                               					 \n\n

*/
uint32 time_get_secs( void );


/** 
@ingroup Functions for Time
         
@brief  
 Get time the phone has been powered on for                            \n\n

@param[in]                                                               
 None                                                                  \n\n
 
@dependencies  
 A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.   \n\n 
 
@param[out]
 uint32 - Number of seconds phone has been powered on for.              \n\n

@sideffects 
 None                                               					 \n\n

*/
uint32 time_get_uptime_secs( void );


/** 
@ingroup Functions for Time
         
@brief  
 Get time in msec the phone has been powered on for                    \n\n

@param[in]                                                               
 time_type - Number of msecs phone has been powered on for.            \n\n
 
@dependencies  
 A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.   \n\n 
 
@param[out]
 None                                                                   \n\n

@sideffects 
 None                                               					 \n\n

*/
void time_get_uptime_ms
(
  time_type                       uptime_ms
);


/** 
@ingroup Functions for Time
         
@brief  
 Synchronize the slow-clock based time-of-day to the current "ts" time.\n\n

@param[in]                                                               
 None                                                                  \n\n
 
@dependencies  
 A valid CDMA, HDR, GSM, GPS, WCDMA (etc) time. time_get( ) must not be \n
 using time_tod_get( ) when this function is called.                    \n\n 
 
@param[out]
 None                                                                   \n\n

@sideffects 
 An diagnostic message is generated if there is a sudden jump in TOD     \n\n

*/
void time_sync_time_of_day( void );


/** 
@ingroup Functions for Time
         
@brief  
 Initialize the slow-clock based time-of-day to the PMIC's RTC time.   \n\n

@param[in]                                                               
 None                                                                  \n\n
 
@dependencies  
 Time-of-day must not have been initialized be another time source.     \n\n 
 
@param[out]
 None                                                                   \n\n

@sideffects 
 None                                                                    \n\n

*/
void time_set_from_pmic( void );


/** 
@ingroup Functions for Time
         
@brief  
 Convert a timestamp from System Time to millisecond units             \n\n

@param[in]                                                               
 time_type  The current time in timestamp format.                      \n\n
 
@dependencies  
 None                                                                   \n\n 
 
@param[out]
 uint64 - milliseconds corresponding to the timestamp provided          \n\n

@sideffects 
 None                                                                    \n\n

*/
uint64 time_to_ms_native
(
  time_type                       time_stamp
);


/** 
@ingroup Functions for Time
         
@brief  
 Get a timestamp from System Time in millisecond units from 6 Jan 1980 
 00:00:00.                                                             \n\n

@param[in]                                                               
 None                                                                  \n\n
 
@dependencies  
 None                                                                   \n\n 
 
@param[out]
 uint64 - The time in ms from 6 Jan 1980 00:00:00.                      \n\n

@sideffects 
 None                                                                    \n\n

*/
uint64 time_get_ms_native( void );


/** 
@ingroup Functions for Time
         
@brief  
 Set the system time                                                   \n\n

@param[in]                                                               
 time_julian_type* - time to be set in julian format                   \n\n
 
@dependencies  
 A valid CDMA time, or a valid HDR time, or a valid Time-of-Day etc.   \n\n 
 
@param[out]
 None                                                                   \n\n 

@sideffects 
 None                                                                    \n\n 

*/

void time_set_julian( time_julian_type *jul_time );

/** 
@ingroup Functions for Time
         
@brief  
 Get the apps user time                                                \n\n

@param[in]                                                               
 time_type - The current time                                          \n\n
 
@dependencies  
 Used only for WM . This will return same as time_get on Brew.          \n\n 
 
@param[out]
 None                                                                   \n\n 

@sideffects 
 None                                                                    \n\n 

*/

void time_get_apps_user_time
(
  time_type                       ts_val
);

/** 
@ingroup Functions for Time
         
@brief  
 Get the system time in julian format                                  \n\n

@param[in]                                                               
 time_julian_type* - To get the julian time                             \n\n
 
@dependencies  
 A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.   \n\n 
 
@param[out]
 None                                                                   \n\n 

@sideffects 
 Modified input parameter and store julian time                         \n\n 

*/
void time_get_julian
(
  time_julian_type *jul_time 
);

/** 
@ingroup Functions for Time
         
@brief  
 Number of milliseconds since "the beginning of time" that the system  
 time needs to be set to                                               \n\n

@param[in]                                                               
 time_type - Time in milliseconds since 6 Jan 1980 00:00:00           \n\n
 
@dependencies  
 A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.   \n\n 
 
@param[out]
 None                                                                   \n\n 

@sideffects 
 Timestamp changed to the a_time sent as parameter to this function.    \n\n 

*/

void time_set_ms
(
  time_type                       a_time
);

#endif /* TIME_H */

