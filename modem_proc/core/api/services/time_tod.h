#ifndef TIME_TOD_H
#define TIME_TOD_H

/**
@file time_tod.h
@brief
Implements time-keeping functions using the slow clock.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The ATS_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      ATS_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the time_tod group 
      description in the ATS_mainpage.dox file. 
=============================================================================*/

/*=============================================================================
  Copyright (c) 2003-2012 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
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

=============================================================================*/


/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/time_tod.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/29/10   scd     (Tech Pubs) Edited Doxygen markup and comments.
10/27/10   EBR     Doxygenated File.
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
#include "timer.h"


/*==============================================================================

                              DATA DEFINITIONS

=============================================================================*/
/** @addtogroup time_tod
@{ */

/** Enumerators for the Time of Day Real Time Clock type.
*/
typedef enum
{
  TS_INVALID = FALSE,  /**< TOD RTC is invalid. */
  TS_VALID   = TRUE,   /**< TOD RTC is valid. */
  TS_UNKNOWN = 0xff    /**< TOD RTC validity is unknown. */
} time_tod_rtc_status_type; 

/** Renames this variable type for other functions expecting the same data 
structure, but under a different name. */
typedef time_tod_rtc_status_type time_tod_status_type;



/*==============================================================================

                        REGIONAL FUNCTION DEFINITIONS

=============================================================================*/

/**
Returns the 64-bit system timestamp in units of 20 millisecond frame time 
(traffic, paging, or access channel frame time).

@param[out] ts_val Address of the qword in which to return 64-bit frame time.

@return
None.

@dependencies  
None.
*/
void time_tod_get_20ms_frame_time
(
  time_type                       ts_val
);


/**
Reads the TOD time offset from the NV and adds it to the timestamp. 

TOD is maintained as an offset from the PMIC RTC time. The PMIC RTC is 
never modified. The offset value is stored in NV and must be added to the PMIC 
time to get the TOD at bootup. This function is to be called during bootup after
all tasks are initialized.

@return
None.
 
@dependencies  
time_set_from_pmic must have been called before calling this function.

@sideeffects 
Increments the base timestamp value.
*/
void time_tod_add_rtc_offset( void );


/**
Gets the time from the TOD source in the number of milliseconds since the epoch.

@param[out] ts_val Location to place milliseconds since 6 Jan 1980 00:00:00.

@return
None.
 
@dependencies  
None.
*/
void time_tod_get_ms
(
  time_type                       ts_val
);


/** 
Sets the slow-clock-based TOD to the given time in seconds.

@param[in] secs Seconds from the epoch to which TOD is to be set.

@return
None.

@dependencies
None.
*/
void time_tod_set_secs 
( 
   uint32 secs 
);

/**
Sets the slow-clock-based TOD to the given timestamp.

@param[in] ts_val Timestamp to which the TOD is set.

@return
None.
 
@dependencies  
This function must be called with interrupts locked to ensure the time is 
set properly.

@sideeffects 
A diagnostic message is generated when there is a sudden jump in TOD.
*/
void time_tod_set
(
  time_type                       ts_val
);


/**
Gets the TOD RTC status.

@return
Status of the RTC.
 
@dependencies  
None.
*/
time_tod_rtc_status_type get_time_tod_rtc_status( void );

/**
Gets the slow-clock-based TOD for the corresponding slow-clock timetick value.

@param[out] ts_val Slow-clock based TOD in ts_timestamp format.
@param[in] sclk    Slow-clock timetick value to use to compute the timestamp.

@return
None.
  
@dependencies  
A valid slow-clock estimate must be available. \n       
time_{set,sync}_time_of_day( ) must have been called before calling this 
function.
*/
void time_tod_get_at
(
  time_type                       ts_val,
  timetick_type                   sclk
);


/** 
Receives the RTC time change requests from the applications processor and 
forces an update to the modem TOD.
 
@param[in] ts_val Timestamp to which the TOD is set.

@return
None.

@dependencies  
None.
 
@sideeffects 
Synchronizes the slow-clock-based TOD to a given timestamp.
*/
void time_tod_set_user_time
(
  time_type                       ts_val
);


/**
@brief  
Requests that the TOD be reset to 6 Jan 1980 00:00:00 and allows OTA time 
updates.
 
@return
None.

@dependencies  
None.
*/
void time_tod_request_reset( void );

/**
Resynchronizes the TOD before a rollover can occur.

The 32 kHz-based TOD is periodically resynchronized to the system time, if
available. When the TOD has not been resynchronized for 1.5 days, the 32 kHz 
counter rolls over, and the TOD jumps backward 1.5 days. To prevent this from 
occuring, this timer callback is used.

@param[in] unused_data Arbitrary timer data. Unused in the current code generation.

@return
None.

@dependencies  
None.
*/
void time_tod_resync
(
  timer_cb_data_type              unused_data
);

/** @} */ /* end_addtogroup time_tod */

#endif /* TIME_TOD_H */

