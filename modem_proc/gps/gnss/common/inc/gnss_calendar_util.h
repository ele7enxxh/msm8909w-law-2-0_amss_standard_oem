#ifndef GNSS_CALENDAR_UTIL_H
#define GNSS_CALENDAR_UTIL_H
/*============================================================================
  @file gnss_calendar_util.h
 
  This file provides calendar utilities to be used across the GPS Subsystem.. 
 
  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

============================================================================*/
/*============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/inc/gnss_calendar_util.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 



when        who  what, where, why
----------  ---  -----------------------------------------------------------
2013-05-13  ssh   Util to convert from UTC timestamp to GPS time
2011-04-04  spn  Util to convert from UTC to GPS time
2011-02-07  spn  Initial revision.


=============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "pdapi.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/**
 @brief MACRO defition to test if a year is a leap year or not
 */
#define GNSS_IS_LEAP_YEAR(year )                                      \
                (( ( (year)%400 == 0 ) ||                              \
                   ( ( (year)%100 != 0 ) &&                             \
                   ( (year)%4 == 0 ) ) ) ? TRUE : FALSE )

/** 
 @brief macro definitions for day of the week

 The values of the macros currently matches the values as expected by a QMI
 client. 
 */
#define    GNSS_SUNDAY                0
#define    GNSS_MONDAY                1
#define    GNSS_TUESDAY               2
#define    GNSS_WEDNESDAY             3
#define    GNSS_THURSDAY              4
#define    GNSS_FRIDAY                5
#define    GNSS_SATURDAY              6
#define    GNSS_MAX_DAYS_IN_A_WEEK    (GNSS_SATURDAY+ 1)
#define    GNSS_UNKNOWN_DAY           GNSS_MAX_DAYS_IN_A_WEEK 


/**
 @brief  Macro definition, defining the number of seconds in a day
 */
#define GNSS_CAL_UTIL_SECONDS_IN_ONE_DAY    (24 * 60 * 60)

/**
 @brief  Macro definition, defining the number days in a week
 */
#define GNSS_CAL_UTIL_DAYS_IN_ONE_WEEK      (7)

/**
 @brief  Macro definition, which defines the delta between GPS and UTC time
         The Global Positioning System (GPS) epoch is January 6, 1980 and
         is synchronized to UTC. GPS is NOT adjusted for leap seconds.
 */
#define GNSS_CAL_UTIL_GPS_TO_UTC_DELTA_SECONDS    \
     (3657 * GNSS_CAL_UTIL_SECONDS_IN_ONE_DAY)



/**
 @brief  Macro definition, defining the number of seconds in a day
 */
#define GNSS_CAL_UTIL_SECONDS_IN_ONE_WEEK    \
    (GNSS_CAL_UTIL_DAYS_IN_ONE_WEEK * GNSS_CAL_UTIL_SECONDS_IN_ONE_DAY)


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/** 
 @brief Structure of a date

 */
  typedef struct
  {
    uint8 b_DayOfMonth;
    uint8 b_MonthOfYear;
    uint16 w_Year;
  } gnss_DateType;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------*//**
@brief:  Gets the number of Leap years that have elapsed since  
1970-01-01 00:00:00 at GMT, EXCLUDING the year that is passed in. 


@param[i] pz_UtcTime      :  Pointer to a structure with Valid UTC Time
@param[o] pd_NoOfLeapYears: No of Leap Years ( or Leap Days )  since 1970, excluding
                       the year passed in.

@return TRUE: pd_NoOfDays is populated
          FALSE: pd_NoOfDays is not populated           
*//* ------------------------------------------------------------------------*/
boolean gnss_GetNoOfLeapYearsFromUTC( 
                          const pdsm_utc_time_s_type* pz_UtcTime,
                          uint32 *pd_NoOfLeapYears );


/* -----------------------------------------------------------------------*//**
@brief Gets the number of days that have elapsed since  1970-01-01 00:00:00 at 
GMT



@param[i] pz_UtcTime:  Pointer to a structure with Valid UTC Time
@param[o] pt_NoOfDays: Return value which will be filled with the Days since 
                       Unix-Time Started 

@return TRUE: pt_NoOfDays is populated
          FALSE: pt_NoOfDays is not populated 
*//* ------------------------------------------------------------------------*/
boolean gnss_GetNoOfDaysFromUTC( 
                                       const pdsm_utc_time_s_type* pz_UtcTime,
                                       uint32 *pd_NoOfDays );

/* -----------------------------------------------------------------------*//**
@brief Gets the Unix Epoch, which is the number of seconds elapsed from 
1970-01-01 00:00:00 at GMT accounting for leapyears from the UTC Time. 

@param[i] pz_UtcTime:  Pointer to a structure with Valid UTC Time
@param[o] pt_UtcEpoch: Return value which will be filled with the Unix-Epoch

@return TRUE: pt_UtcEpoch is populated
      FALSE: pt_UtcEpoch is not populated 
*//* ------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
boolean gnss_GetUnixEpochFromUTC( 
                                        const pdsm_utc_time_s_type* pz_UtcTime,
                                        uint64 *pt_UtcEpoch );

#ifdef __cplusplus
}
#endif
/* -----------------------------------------------------------------------*//**
@brief  Gets the Day of week given the date

@param[i] pz_UtcTime:  Pointer to a structure with Valid UTC Time
@param[o] pz_DayOfWeek: Return value which will be filled with the Unix-Epoch

@return TRUE: pz_DayOfWeek is populated
        FALSE: pz_DayOfWeek is not populated 
*//* ------------------------------------------------------------------------*/
boolean gnss_GetDayOfWeekFromUTC ( 
                                  const pdsm_utc_time_s_type* pz_UtcTime,
                                  uint8 *pz_DayOfWeek );


/* -----------------------------------------------------------------------*//**
@brief  Parse's the Date in mmddyyyy format into a structure

@param[i] date:  Date in mmddyyyy format
@param[o] pz_ParsedInfo: Return value which has the information parsed

@return TRUE: pz_ParsedInfo is populated
        FALSE: pz_ParsedInfo is not populated 
*//* ------------------------------------------------------------------------*/
boolean gnss_ParseDate ( 
                                  uint32 d_Date,
                                  gnss_DateType *pz_ParsedInfo );

/* -----------------------------------------------------------------------*//**
@brief  Convert UTC time in MilliSeconds to GPS time in Milliseconds

@param[i] t_UtcMs:  UTC time in Milliseconds
@param[i] u_LeapSeconds: Leap Seconds till date
@param[o] pt_GpsMs: Output Val which has GPS time in milliseconds

@return TRUE: pt_GpsMs is populated
        FALSE: pt_GpsMs is not populated 
*//* ------------------------------------------------------------------------*/
boolean gnss_ConvertUtcMstoGpsMs ( 
                                   uint64 t_UtcMs,
                                   uint8  u_LeapSeconds,
                                    uint64* const pt_GpsMs );

/* -----------------------------------------------------------------------*//**
@brief  Convert UTC time in milliseconds to GPS Week and GPS Time-of-Week in 
        milliseconds

@param[i] t_UtcMs:  UTC time in Milliseconds
@param[i] u_LeapSeconds: Leap Seconds till date
@param[o] pw_GpsWeek: Output Val which has GPS Week Corresponding to input
@param[o] pq_GpsToWMs: Output Val which has GPS time of week in milliseconds
                      corresponding to inpit

@return TRUE: pw_GpsWeek, pq_GpsToWMs  is populated
        FALSE: pw_GpsWeek, pq_GpsToWMs  is not populated 
*//* ------------------------------------------------------------------------*/
boolean gnss_ConvertUtcMstoGpsTime ( 
                                    uint64 t_UtcMs,
                                    uint8  u_LeapSeconds,
                                    uint16* const pw_GpsWeek,
                                    uint32* const pq_GpsTowMs);

/* -----------------------------------------------------------------------*//**
@brief  Convert UTC time stamp in mm\dd\yy hh:mm:ss to GPS Week and GPS Time-of-Week in 
        milliseconds

@param[i] pz_UtcTime:  Pointer to a structure with Valid UTC Time
@param[i] u_LeapSeconds: Leap Seconds till date
@param[o] pw_GpsWeek: Output Val which has GPS Week Corresponding to input
@param[o] pq_GpsToWMs: Output Val which has GPS time of week in milliseconds

@return TRUE: pz_DayOfWeek is populated
        FALSE: pz_DayOfWeek is not populated 
*//* ------------------------------------------------------------------------*/
boolean gnss_ConvertUtcTimeStamptoGpsTime(const pdsm_utc_time_s_type  *pz_UtcTime,
                                          uint8  u_LeapSeconds,
                                          uint16* const pw_GpsWeek,
                                          uint32* const pq_GpsTowMs);


#endif /* #ifndef GNSS_CALENDAR_UTIL_H */

