#ifndef TIME_TOD_H
#define TIME_TOD_H
/*==============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements time-keeping functions using the slow clock.

  Slow Clock Frequency         Granularity      Time Range
    30.000kHz (lower limit)      33.3 us         39.7 hrs
    32.768kHz (nominal freq)     30.5 us         36.4 hrs
    60.000kHz (upper limit)      16.6 us         19.8 hrs


EXTERNALIZED FUNCTIONS
  get_time_tod_rtc_status 

  time_tod_set Set the slow-clock based time-of-day to the given timestamp

  time_tod_set_secs Set the slow-clock based time-of-day to the given time 
  in seconds

  time_tod_get_ms Get the time from TOD source, in # of milliseconds since 
  "the beginning of time".

  time_tod_add_rtc_offset Time of day is maintained as an offset from PMIC 
  RTC time. 

  time_tod_get_20ms_frame_time This procedure is returns the 64-bit system 
  timestamp in units of 20 millisecond frame time (traffic / paging / access 
  channel frame time).

INITIALIZATION AND SEQUENCING REQUIREMENTS
    time_tod_init() must be called from time_init()
    time_tod_set() must be called before time_tod_get().
    A valid slow clock estimate is required for time_tod_get().


Copyright (c) 2003 - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/time_tod.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/23/09   din     Removed time_tod_set_apps_bases function 
09/22/09   din     time_tod_resync is made public
02/23/09   din     Consoldation of time.
01/05/09   din     Added functions time_tod_status and time_tod_sec_status from 
                   MSM6280 branch
09/23/08   sep     Add function time_tod_secs_to_timestamp
09/06/08   jhs     Include timer.h for new time_tod_resync decleration.
09/06/08   jhs     Fix time_tod_resync decleration.
11/16/07   jhs     Fix support for time set from the Apps not properly getting
                   set to the Modem and the RTC offset.
11/02/06   bn      Added functions prototypes to get TOD time in ms and 20ms
06/09/06   cab     Added time_tod_status
08/11/05   jhs     Added time_tod_set_apps_bases prototype
05/10/05   ajn     PMIC RTC read moved out of time_tod_init.
11/12/04   ajn     Added time_tod_get_at() to compute future timestamps
02/09/04   ajn     Renamed time.h to time_svc.h to avoid OS conflict
08/08/03   ajn     Changed timestamp type from qword to time_type (a qword)
08/01/03   ajn     Move _get_ms, _get_sec functions to time.h
07/18/03   ajn     File created.

=============================================================================*/



/*==============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_svc.h"
#include "timetick.h"
#include "timer.h"

/**
@mainpage Time Services CDMA
Implements time-of-day time-keeping functions using the slow clock. 
*/

/**
@defgroup Time Services CDMA  - Public Interface
This section contains definitions for various public interface defined 
by this module.
*/

/** 
@ingroup Functions for Time of Day  - Public Interface
@file time_tod.h
This file contains public interface defined by this module
*/



/*==============================================================================

                              DATA DEFINITIONS

=============================================================================*/

/**  Enums for TOD RTC type.                   */
typedef enum
{
  /** INVALID */
  TS_INVALID = FALSE,
  /** VALID */
  TS_VALID   = TRUE,
  /** UNKNOWN */
  TS_UNKNOWN = 0xff
} time_tod_rtc_status_type; 

typedef time_tod_rtc_status_type time_tod_status_type;



/*==============================================================================

                        REGIONAL FUNCTION DEFINITIONS

=============================================================================*/

/** 
@ingroup Functions for Time of Day
         
@brief  
 This procedure is returns the 64-bit system timestamp in units of      
 20 millisecond frame time (traffic / paging / access channel frame     
 time).                                                                 \n\n

@param[in]                                                               
 time_type  Address of qword in which to return 64-bit frame time.    \n\n  
 
@dependencies  
 None                                                                   \n\n  
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None																	 \n\n

*/

void time_tod_get_20ms_frame_time
(
  time_type                       ts_val
);

/** 
@ingroup Functions for Time of Day
         
@brief  
 Time of day is maintained as an offset from PMIC RTC time. PMIC RTC   
 is never   modified. The offset value is stored in NV and needs to     
 added to PMIC time to get TOD at bootup. time_tod_add_rtc_offset reads 
 time tod offset from NV and adds it to the timestamp. Should be called 
 during bootup after all tasks are initialized.                        \n\n                                         

@param[in]                                                               
 None                                                                 \n\n 
 
@dependencies  
 time_set_from_pmic should've been called                               \n\n  
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 Increments base timestamp value.                               		 \n\n

*/
void time_tod_add_rtc_offset( void );

/** 
@ingroup Functions for Time of Day
         
@brief  
 Get the time from TOD source, in # of milliseconds since "the         
 beginning of time".                                                   \n\n

@param[in]                                                               
 time_type Milliseconds since 6 Jan 1980 00:00:00 returned to the    
 time parameter                                                       \n\n 
 
@dependencies  
 None                                                                   \n\n 
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None                                                                   \n\n 

*/
void time_tod_get_ms
(
  time_type                       ts_val
);

/** 
@ingroup Functions for Time of Day
         
@brief  
 Set the slow-clock based time-of-day to the given time in seconds    \n\n

@param[in]                                                               
 uint32 - Secs from epoch that time of day should be set to           \n\n 
 
@dependencies  
 None                                                                   \n\n 
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None                                                                   \n\n 

*/
void time_tod_set_secs 
( 
   uint32 secs 
);

/** 
@ingroup Functions for Time of Day
         
@brief  
 Set the slow-clock based time-of-day to the given timestamp          \n\n

@param[in]                                                               
 time_type - Timestamp to set time of day to                           \n\n 
 
@dependencies  
 Should be called from an INTLOCK'd context, to ensure proper           
 time-setting.                                                          \n\n 
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 An diagnostic message is generated if there is a sudden jump in TOD    \n\n 

*/

void time_tod_set
(
  time_type                       ts_val
);

/** 
@ingroup Functions for Time of Day
         
@brief  
 Gets the TOD RTC status                                               \n\n

@param[in]                                                               
 None                                                                   \n\n 
 
@dependencies  
 None                                                                     \n\n 
 
@param[out]
 time_tod_rtc_status_type - status of RTC                               \n\n
 
@sideffects 
 None                                                                   \n\n 

*/
time_tod_rtc_status_type get_time_tod_rtc_status( void );

/** 
@ingroup Functions for Time of Day
         
@brief  
 Get the slow-clock based time-of-day for the corresponding slow-clock  
 timetick value.                                                        \n\n
 
                                                               
@param[in] time_type - Slow-clock based time-of-day in ts_timestamp format         
@param[in] timetick_type - Slow-clock time-tick value to compute the timestamp at \n\n
   
@dependencies  
 A valid slow clock estimate must be available.                           \n       
 time_{set,sync}_time_of_day( ) must have been called first.              \n\n
 
@param[out]
  None                                                                   \n\n 
 
@sideffects 
  None                                                                   \n\n 

*/
void time_tod_get_at
(
  time_type                       ts_val,
  timetick_type                   sclk
);


/** 
@ingroup Functions for Time of Day
         
@brief  
 Receive RTC time change requests from Apps and force update Modem     
 time-of-day.                                                          \n\n
 
@param[in]                                                               
 time_type - Timestamp to set time of day to                            \n\n 
      
@dependencies  
 None                                                                   \n\n 
 
@param[out]
 None                                                                   \n\n 
 
@sideffects 
 Synchronizes the slow-clock based time of day to a given timestamp.    \n\n 

*/
void time_tod_set_user_time
(
  time_type                       ts_val
);


/** 
@ingroup Functions for Time of Day
         
@brief  
 Request that the TOD be reset to 6 Jan 1980 00:00:00 and allow OTA     
 time updates.                                                          \n\n
 
@param[in]                                                               
 None                                                                   \n\n 
      
@dependencies  
 None                                                                   \n\n 
 
@param[out]
 None                                                                   \n\n 
 
@sideffects 
 None                                                                   \n\n 

*/

void time_tod_request_reset( void );

/** 
@ingroup Functions for Time of Day
         
@brief  
 The 32kHz-based time-of-day is periodically resynced to system time,if 
 available.  If it has not been re-synced for 1.5 days,the 32kHz counter
 will 'rollover', and time-of-day would jump backwards 1.5 days.        
 To prevent this from occuring, this timer callback is used to resync   
 time-of-day before the rollover will occur.                            \n\n

@param[in]                
   timer_cb_data_type -  Arbitrary timer data.                           \n 
                         Not used but required for function prototype.   \n\n 
      
@dependencies  
 None                                                                   \n\n 
 
@param[out]
 None                                                                   \n\n 
 
@sideffects 
 None                                                                   \n\n 

*/
void time_tod_resync
(
  timer_cb_data_type              unused_data
);

#endif /* TIME_TOD_H */

