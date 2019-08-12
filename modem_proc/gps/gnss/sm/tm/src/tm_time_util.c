/**
 * @file
 * @brief TM Time Utilities
 *
 * This file contains implementations for TM time utilities.
 *
 * @ingroup loc_tm
 */
/*
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_time_util.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-07-09  va   Branched from tm_util.c. Refactored for milliseconds support
                   and updated documentation.

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "tm_common.h"
#include "tm_api.h"

//! @addtogroup loc_tm
//! {
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
//! @name Static Variable Definitions
//! @{

/**
 * @brief Number of days per month in a non-leap year
 * @sa u_days_in_month_leap
 */
static const uint8 u_days_in_month[12] =
{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
/**
 * @brief Number of days per month in a leap year
 * @sa u_days_in_month
 */
static const uint8 u_days_in_month_leap[12] =
{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//! @}
/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
//! @name Static Function Declarations and Definitions
//! @{

/**
 * @brief Helper for populating time component of Gregorian calendar.
 *
 * @param[in]  q_DayMsec  Milliseconds since midnight.
 * @param[out] pz_TimeOut Structure for filling in time components.
 */
static void fill_calendar_time
(
  uint32            q_DayMsec,
  tm_CalendarTimeT *pz_TimeOut
)
{
  uint32 q_Msecs, q_Hour, q_Minute, q_Second;

  q_Msecs = q_DayMsec;

  /* Break into hours, mins, secs and secs/10 */
  q_Hour   = q_Msecs / HOUR_MSECS;
  q_Msecs -= q_Hour * HOUR_MSECS;

  q_Minute = q_Msecs / MIN_MSECS;
  q_Msecs -= q_Minute * MIN_MSECS;

  q_Second = q_Msecs / SEC_MSECS;
  q_Msecs -= q_Second * SEC_MSECS;

  pz_TimeOut->q_Hour = q_Hour;
  pz_TimeOut->q_Minute = q_Minute;
  pz_TimeOut->q_Second = q_Second;
  pz_TimeOut->q_Millisecond = q_Msecs;
}

/**
 * @brief Helper for populating date component of Gregorian calendar.
 *
 * This function has a limitation to work for years between 1900 and 2100.
 *
 * @param[in]  q_StartingYear Starting year: 1980 or 1996. Shall be a leap year.
 * @param[in]  q_Days         Day number within four-year cycle.
 * @param[out] pz_TimeOut     Structure for filling in date components.
 */
static void fill_calendar_date
(
  uint32            q_StartingYear,
  uint32            q_Days,
  tm_CalendarTimeT *pz_TimeOut
)
{
  uint32 q_Month, q_FourYears, q_Year;
  const uint8 *p_DaysInMonth;

  /* How many 4 year cycles have occurred. */
  q_FourYears = q_Days / (4*365 + 1);

  /* q_Days represents the # of days into this 4 year period */
  q_Days   -= q_FourYears * (4*365 + 1);

  q_Year = q_StartingYear + (q_FourYears * 4);


  /* Day 0 to 365 of q_FourYears is in the leap year */
  if ( q_Days >= 366 )
  {
    uint32 q_YearsInPeriod;

    /* This year is not a leap year */
    q_Year         += 1;
    q_Days         -= 366;

    q_YearsInPeriod = q_Days / 365;
    q_Year         += q_YearsInPeriod;
    q_Days         -= q_YearsInPeriod * 365;

    p_DaysInMonth   = u_days_in_month;
  }
  else
  {
    /* This year is a leap year */
    p_DaysInMonth   = u_days_in_month_leap;
  }

  /* Subtract days in months for this year. */
  for (q_Month = 1; q_Month < 12 && q_Days >= *p_DaysInMonth; q_Month++)
  {
    q_Days -= *p_DaysInMonth;
    p_DaysInMonth++;
  }

  /* Need to add 1 to account for the fact the days in month
     start at 1 and not 0 */
  q_Days ++;

  pz_TimeOut->q_Year = q_Year;
  pz_TimeOut->q_Month = q_Month;
  pz_TimeOut->q_Day = q_Days;
}

//! @}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/**
 * @brief Converts CDMA timestamp into milliseconds
 *
 * This function takes a timestamp in the format generated by time_get()
 * and converts it to the implied total number of gps milliseconds.
 *
 * @param[in]  pt_CmdaTimestamp   Quadword with CDMA timestamp.
 * @param[out] pt_MillisecondsOut Quadword with timestamp in milliseconds.
 *
 * @return None.
 */
void gps_total_msec
(
  const qword pt_CdmaTimestamp,
  qword       pt_MillisecondsOut
)
{
  uint32 q_SubMs;
  uint32 q_Sub_1_25_ms;
  uint64 t_TempTs;

  t_TempTs = QW_CVT_Q2N(pt_CdmaTimestamp);

  /* Get "sub 1.25 ms" part of time stamp */
  q_Sub_1_25_ms = (uint32)t_TempTs & 0xFFFF;

  /* Rescale so original range spans 1 ms. instead of 1.25 ms. */
  q_Sub_1_25_ms = (q_Sub_1_25_ms * 5) >> 2;

  /*
   * Extract portion of timestamp in units of 1.25 ms. to create a time
   * in milliseconds.
   */
  t_TempTs >>= 16;

  /* Scale to units of 0.25 ms. */
  t_TempTs *= 5;

  /* Extract sub millisecond portion for 1 ms. adjustment to follow. */
  q_SubMs = t_TempTs & 0x3;

  /* Scale to units of milliseconds. */
  t_TempTs >>= 2;

  /*
   * Add a millisecond if sub 1.25 ms. and sub ms. portions add up to
   * more than a millisecond.
   */
  q_SubMs *= QUARTER_MS_VAL;
  q_Sub_1_25_ms += q_SubMs;
  if (q_Sub_1_25_ms >= 4 * QUARTER_MS_VAL)
  {
    t_TempTs++;
  }

  qw_set(pt_MillisecondsOut, (uint32)(t_TempTs>>32), (uint32)t_TempTs);
}

/**
 * @brief Convert quadword timestamp into GPS week number and milliseconds.
 *
 * This function takes a total number of gps milliseconds and returns the
 * week and the number of milliseconds in the week.
 *
 * @param[in]  pt_Milliseconds Quadword with milliseconds.
 * @param[out] pw_GpsWeekOut   GPS week number.
 * @param[out] pq_GpsMsOut     GPS millisecond since week start.
 *
 * @return None
 */
void gps_week_ms
(
  const qword pt_Milliseconds,
  uint16     *pw_GpsWeekOut,
  uint32     *pq_GpsMsOut
)
{
  uint64 t_Milliseconds = QW_CVT_Q2N(pt_Milliseconds);

  if (NULL != pw_GpsWeekOut)
  {
    *pw_GpsWeekOut = (uint16)(t_Milliseconds / MS_PER_WEEK);
  }
  if (NULL != pq_GpsMsOut)
  {
    *pq_GpsMsOut   = (uint32)(t_Milliseconds % MS_PER_WEEK);
  }
}

/**
 * @brief Helper function to correct GPS weekly seconds value.
 *
 * Function performs the fairly common check to see if a msec is indeed with
 * the allowable range of 0 thru WEEK_MSECS-1. Only values within a single week
 * are entertained. (ie- Multiple week adjustments are not performed
 *
 * Assigning a NULL pointer to pw_GpsWeek will disable the week adjustment
 * logic, so that only pl_GpsMsec will only be affected.
 *
 * @param[in,out] pl_GpsMsecs GPS milliseconds in range [-WEEK_MSECS;
 *                            2*WEEK_MSECS). The value is updated into range
 *                            [0..WEEK_MSECS);
 * @param[in,out] pw_GpsWeek  Optional value for week number correction.
 *                            When specified, the value is adjusted according to
 *                            *pl_GpsMsecs value change.
 *
 * @return None
 */
void gps_MsecWeekLimit
(
  int32  *pl_GpsMsecs,
  uint16 *pw_GpsWeek
)
{
  if (NULL != pl_GpsMsecs)
  {
    if (*pl_GpsMsecs < 0)
    {
      *pl_GpsMsecs += WEEK_MSECS;

      if (NULL != pw_GpsWeek)
      {
        (*pw_GpsWeek) --;
      }
    }
    else if ((uint32)*pl_GpsMsecs >= WEEK_MSECS)
    {
      (*pl_GpsMsecs) -= WEEK_MSECS;

      if (NULL != pw_GpsWeek)
      {
        (*pw_GpsWeek) ++;
      }
    }
  }
}

/**
 * @brief Convert GPS time into CDMA system time
 *
 * This function takes time stored in GPS Time and converts it into
 * CDMA system time.
 *
 * @param[in]  pz_Clock        GPS time structure.
 * @param[out] pt_TimestampOut Quadword pointer for CDMA time.
 *
 * @return None
 */
void gps_to_cdma_time
(
  const gps_TimeStructType *pz_Clock,
  qword                     pt_TimestampOut
)
{
  uint64 t_Timestamp;
  uint32 q_Sub_1_25_ms;

  /* Convert GPS time into milliseconds */
  t_Timestamp = (uint64)pz_Clock->w_GpsWeek * MS_PER_WEEK + pz_Clock->q_GpsMsec;
  /* Scale to 1.25 */
  t_Timestamp *= 4;
  q_Sub_1_25_ms = t_Timestamp % 5;
  t_Timestamp /= 5;

  /* Shift left 16 bits, Since the low 16 bits is defined to be
   * the sub 1.25ms.
  */
  t_Timestamp <<= 16;

  /* Now add back the sub 1.25ms value
   * (1.25 ms dynamic range. Rolls over at count 49152)
  */
  q_Sub_1_25_ms = (q_Sub_1_25_ms * QUARTER_MS_VAL * 4)/5;
  t_Timestamp |= (uint16)q_Sub_1_25_ms;

  qw_set(pt_TimestampOut, (uint32)(t_Timestamp >> 32), (uint32)(t_Timestamp));
}

/**
 * @brief Construct UTC time from GPS time fields.
 *
 * This function constructs UTC time YearMonthHourMinuteSecond using GPS
 * WeekNumber, GPS msec_of_week and UTC leap time adjusted msec_of_week.
 *
 * @param[in] w_FixWn          fix time GPS week;
 * @param[in] q_FixTimeGpsMsec fix time GPS msec. of the week;
 * @param[in] q_FixTimeUtcMsec fix time UTC msec (adj. for leap seconds ).
 *
 * @return UTC time YearMonthHourMinuteSecond. (if fails to construct UTC time,
 *         UTC will be set to all zero)
 */
utc_time tm_util_construct_utc_time
(
  uint16 w_FixWn,
  uint32 q_FixTimeGpsMsec,
  uint32 q_FixTimeUtcMsec
)
{
  utc_time z_UtcTime;
  tm_CalendarTimeT z_TmTime;

  if (!tm_util_construct_gps_time(w_FixWn,
                                  q_FixTimeGpsMsec,
                                  q_FixTimeUtcMsec,
                                  &z_TmTime) ||
      !tm_util_time_to_utc_time(&z_TmTime, &z_UtcTime))
  {
    memset(&z_UtcTime, 0, sizeof(z_UtcTime));
  }

  return z_UtcTime;
}

/**
 * @brief Converts Glonass time to UTC time.
 *
 * @param[in]  u_GloNumFourYear       Count of 4-year intervals since 1996.
 * @param[in]  w_GloNumDaysInFourYear Number of days within 4-year interval.
 * @param[in]  q_GloTimeMs            Number of milliseconds since midnight.
 * @param[out] pz_UtcTimeOut          Parsed UTC structure with decisecond
 *                                    accuracy.
 *
 * @retval TRUE  Conversion is successful.
 * @retval FALSE Conversion failed or invalid arguments.
 *
 * @deprecated Use tm_util_construct_glo_time
 * @sa tm_util_construct_glo_time
 */
boolean tm_util_glo_to_utc
(
  uint8 u_GloNumFourYear,
  uint16 w_GloNumDaysInFourYear,
  uint32 q_GloTimeMs,
  utc_time* pz_UtcTimeOut
)
{
  tm_CalendarTimeT z_Time;

  if (tm_util_construct_glo_time(u_GloNumFourYear,
                                 w_GloNumDaysInFourYear,
                                 q_GloTimeMs,
                                 &z_Time))
  {
    return tm_util_time_to_utc_time(&z_Time, pz_UtcTimeOut);
  }
  else
  {
    return FALSE;
  }
}

/**
 * @brief Computes UTC time from GPS or GLO time source.
 *
 * This function calculates UTC time YearMonthHourMinuteSecond using GPS
 * time or GLONASS time specified in fix report. If both GPS and GLONASS
 * time are valid, GPS time will be used to calculate UTC.
 *
 * This routine can return UTC in two different UTC formats (pdsm_utc_time
 * and utc_time), you can set one to NULL if not used. If you pass in both
 * pointers, two UTC formats will be returned.
 *
 * @param[in]  pz_Fix         Fix report pointer
 * @param[out] pz_PdsmTimeOut PDSM UTC structure pointer (optional, can be NULL)
 * @param[out] pz_UtcTimeOut  TM UTC structure pointer (optional, can be NULL)
 *
 * @retval TRUE  Successfully calculated UTC time
 * @retval FALSE Failed to calculate UTC time (UTC will be set to all zero)
 *
 * @deprecated Use tm_util_calculate_time, tm_util_time_to_utc_time,
 *             tm_util_time_to_pdsm_time
 *
 * @sa tm_util_calculate_time
 * @sa tm_util_time_to_utc_time
 * @sa tm_util_time_to_pdsm_time
 */
boolean tm_util_calculate_utc
(
  const sm_GnssFixRptStructType *pz_Fix,
  pdsm_utc_time_s_type          *pz_PdsmTimeOut,
  utc_time                      *pz_UtcTimeOut
)
{
  tm_CalendarTimeT z_Time;
  boolean v_RetVal = FALSE;

  if (NULL == pz_Fix || (NULL == pz_PdsmTimeOut && NULL == pz_UtcTimeOut))
  {
    MSG_ERROR("null ptr passed fixrpt=%x utc1=%x utc2=%x", 
              pz_Fix, pz_PdsmTimeOut, pz_UtcTimeOut);
    return FALSE;
  }

  v_RetVal = tm_util_get_fix_time(pz_Fix, &z_Time);

  if (v_RetVal && NULL != pz_PdsmTimeOut)
  {
    v_RetVal = tm_util_time_to_pdsm_time(&z_Time, pz_PdsmTimeOut);
  }
  if (v_RetVal && NULL != pz_UtcTimeOut)
  {
    v_RetVal = tm_util_time_to_utc_time(&z_Time, pz_UtcTimeOut);
  }

  return v_RetVal;
}
	
/**
 * @brief Adjusts time information according to GPS offset time (deltaT)
 *
 * This function processes GPS offset time (deltaT) to compute the GLONASS time
 * & set the appropriate GLONASS time fields in gnss_time_info struct.
 * The deltaT could be from GPS UTCModel AD and/or GLONASS TimeModel AD.
 *
 * @param[in]     b_DeltaT     Positive GPS time offset value.
 * @param[in,out] pz_AgnssTime Pointer to position time structure.
 * 
 * @retval TRUE  On success
 * @retval FALSE Failure indication.
 */
boolean tm_util_convert_to_glonass_time
(
  int8            b_DeltaT,
  gnss_time_info *pz_AgnssTime
)
{
  // the delta time (a.k.a LeapSecondOffset) for GPS to GLONASS should be positive value
  if (NULL == pz_AgnssTime || b_DeltaT < 0)
  {
    MSG_ERROR( "Invalid argument: 0x%x, %d", pz_AgnssTime, b_DeltaT, 0 );
    return FALSE;
  }

  pz_AgnssTime->b_deltaT = b_DeltaT;
  pz_AgnssTime->m.deltaTPresent = 1;

  // Convert GPS to GLONASS time
  if (TRUE == pz_AgnssTime->m.refTimeWeekNumValid &&
      TRUE == pz_AgnssTime->m.posTimeStampValid)
  {
    gps_TimeStructType z_GpsTime;
    memset(&z_GpsTime, 0, sizeof(z_GpsTime));
    memset(&pz_AgnssTime->z_GlonassTime, 0, sizeof(glo_TimeStructType));

    z_GpsTime.w_GpsWeek = pz_AgnssTime->w_ref_time_week_num;
    z_GpsTime.q_GpsMsec = pz_AgnssTime->t_pos_time_stamp_ms -
                      ((uint64)pz_AgnssTime->w_ref_time_week_num * MS_PER_WEEK);

    gnss_ConvertGpsTime2GloTime(&z_GpsTime, (uint8)pz_AgnssTime->b_deltaT,
                                &pz_AgnssTime->z_GlonassTime );

    pz_AgnssTime->m.glonassTimeValid = TRUE;
    MSG_6( MSG_SSID_GPSSM, MSG_LEGACY_LOW,
           "GLONASS time (%u, %u, %u) computed from GPS time (%u, %u) offset of %d",
           pz_AgnssTime->z_GlonassTime.u_FourYear,
           pz_AgnssTime->z_GlonassTime.w_Days,
           pz_AgnssTime->z_GlonassTime.q_GloMsec,
           z_GpsTime.w_GpsWeek,
           z_GpsTime.q_GpsMsec,
           pz_AgnssTime->b_deltaT );

    return TRUE;
  }
  else
  {
    pz_AgnssTime->m.glonassTimeValid = FALSE;
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,
          "GPS Reference Time (%u, %u) isn't valid to compute Glonass time",
          pz_AgnssTime->m.refTimeWeekNumValid,
          pz_AgnssTime->m.posTimeStampValid);
  }

  return FALSE;
}

/**
 * @brief Converts calendar time with milliseconds into UTC time structure.
 *
 * During the conversion the milliseconds are truncated to fit the storage type.
 *
 * @param[in]  pz_TimeIn  Calendar time with milliseconds.
 * @param[out] pz_TimeOut UTC time structure.
 *
 * @retval TRUE  Conversion is successful.
 * @retval FALSE Conversion failed or invalid arguments.
 */
boolean tm_util_time_to_utc_time
(
  const tm_CalendarTimeT *pz_TimeIn,
  utc_time               *pz_TimeOut
)
{
  if (NULL == pz_TimeIn || NULL == pz_TimeOut)
  {
    return FALSE;
  }

  pz_TimeOut->q_year     = pz_TimeIn->q_Year;
  pz_TimeOut->q_month    = pz_TimeIn->q_Month;
  pz_TimeOut->q_utc_days = pz_TimeIn->q_Day;
  pz_TimeOut->q_hours    = pz_TimeIn->q_Hour;
  pz_TimeOut->q_mins     = pz_TimeIn->q_Minute;
  pz_TimeOut->q_secs     = pz_TimeIn->q_Second;
  pz_TimeOut->q_tenths   = pz_TimeIn->q_Millisecond / TENTH_MSECS;

  return TRUE;
}

/**
 * @brief Converts calendar time with milliseconds into UTC time structure.
 *
 * During the conversion the milliseconds are truncated to fit the storage type.
 *
 * @param[in]  pz_TimeIn  Calendar time with milliseconds.
 * @param[out] pz_TimeOut UTC time structure.
 *
 * @retval TRUE  Conversion is successful.
 * @retval FALSE Conversion failed or invalid arguments.
 */
boolean tm_util_time_to_pdsm_time
(
  const tm_CalendarTimeT *pz_TimeIn,
  pdsm_utc_time_s_type   *pz_TimeOut
)
{
  if (NULL == pz_TimeIn || NULL == pz_TimeOut)
  {
    return FALSE;
  }

  // Date format decimal: DDMMYYYY
  pz_TimeOut->date       = pz_TimeIn->q_Day * 1000000 +
                           pz_TimeIn->q_Month * 10000 +
                           pz_TimeIn->q_Year;
  pz_TimeOut->hour       = pz_TimeIn->q_Hour;
  pz_TimeOut->minute     = pz_TimeIn->q_Minute;
  pz_TimeOut->sec        = pz_TimeIn->q_Second * 100 +
                           pz_TimeIn->q_Millisecond / 10;

  return TRUE;
}

/**
 * @brief Constructs calendar structure from GPS input.
 *
 * @param[in]  w_GpsWeek        GPS week number.
 * @param[in]  q_FixTimeGpsMsec GPS fix time in milliseconds.
 * @param[in]  q_FixTimeUtcMsec UTC fix time in milliseconds.
 * @param[out] pz_TimeOut       Conversion result.
 *
 * @retval TRUE  Conversion is successful.
 * @retval FALSE Conversion failed or invalid arguments.
 *
 * @sa tm_util_construct_glo_time
 */
boolean tm_util_construct_gps_time
(
  uint16            w_GpsWeek,
  uint32            q_FixTimeGpsMsec,
  uint32            q_FixTimeUtcMsec,
  tm_CalendarTimeT *pz_TimeOut
)
{
  uint32 q_Days;
  uint16 w_UtcWeekNo;

  if (NULL == pz_TimeOut)
  {
    return FALSE;
  }

  if (w_GpsWeek == C_GPS_WEEK_UNKNOWN)
  {
    MSG_MED("tm_util_construct_utc_time failed! GPSWeek=%u GPSMs=%u UTCMs=%u",
        w_GpsWeek, q_FixTimeGpsMsec, q_FixTimeUtcMsec);
    return FALSE;
  }

  /* Msecs in day from Msecs in week */
  fill_calendar_time(q_FixTimeUtcMsec % DAY_MSECS, pz_TimeOut);

  /* UTC time and GPS time are generally in the same week. Handle the
     13 to 14 second time span when this is not actually the case. GPS
     time leads UTC time (at least in our lifetimes), so checking for
     UTC bigger than GPS is sufficient */

  w_UtcWeekNo = w_GpsWeek;

  if ( q_FixTimeGpsMsec < q_FixTimeUtcMsec )
  {
    w_UtcWeekNo --;
  }

  q_Days = (7 * w_UtcWeekNo) + (q_FixTimeUtcMsec / DAY_MSECS);

  /* This is the number of days since the GPS clock started on
     Jan 6th, 1980. Add 5 days to make day 0 == Jan 1st */
  q_Days += 5;

  fill_calendar_date(1980, q_Days, pz_TimeOut);

  return TRUE;
}

/**
 * @brief Constructs calendar structure from GLO input.
 *
 * @param[in]  u_GloNumFourYear       GLO 4 year number from 1996.
 * @param[in]  w_GloNumDaysInFourYear GLO day number in 4 years.
 * @param[in]  q_GloTimeMs            GLO millisecond in the day.
 * @param[out] pz_TimeOut             Conversion result.
 *
 * @retval TRUE  Conversion is successful.
 * @retval FALSE Conversion failed or invalid arguments.
 *
 * @sa tm_util_construct_gps_time
 */
boolean tm_util_construct_glo_time
(
  uint8             u_GloNumFourYear,
  uint16            w_GloNumDaysInFourYear,
  uint32            q_GloTimeMs,
  tm_CalendarTimeT *pz_TimeOut
)
{
  uint64 t_TotalMsec = 0;
  uint32 q_Days, q_FourYears, q_Year;

  /* Sanity */
  MSG_MED("Glo Time Info: %u, %u, %u",
          u_GloNumFourYear, w_GloNumDaysInFourYear, q_GloTimeMs);

  if (0 ==  u_GloNumFourYear ||
      C_GLO_FOUR_YEAR_UNKNOWN == u_GloNumFourYear)
  {
    /* GLONASS time is invalid, no conversion */
    MSG_MED("Glo Time Info Invalid: %u", u_GloNumFourYear, 0, 0);
    return FALSE;
  }

  /* Total milliseconds since GLONASS started */
  t_TotalMsec = ((u_GloNumFourYear-1) * FOURYEAR_DAYS +
                 w_GloNumDaysInFourYear - 1) * (uint64)DAY_MSECS +
                q_GloTimeMs;

  /* Account for 3 hr Moscow time */
  if (GLO_UTC_MSECS > t_TotalMsec)
  {
    MSG_MED("UTC Msec From Glo Invalid", 0, 0, 0);
    return FALSE;
  }
  t_TotalMsec -= GLO_UTC_MSECS;

  // Break into UTC Time Structure
  fill_calendar_time(t_TotalMsec % DAY_MSECS, pz_TimeOut);

  /* Msecs in present day */
  q_Days = t_TotalMsec / DAY_MSECS;

  fill_calendar_date(1996, q_Days, pz_TimeOut);

  return TRUE;
}

/**
 * @brief Obtains calendar time from fix report.
 *
 * This function calculates Gregorian calendar time with millisecond accuracy
 * using GPS or GLONASS time specified in fix report. If both GPS and GLONASS
 * time fields are valid, GPS time will be used to calculate UTC.
 *
 * @param[in]  pz_Fix     Fix report.
 * @param[out] pz_TimeOut Computed values.
 *
 * @retval TRUE  Conversion is successful.
 * @retval FALSE Conversion failed or invalid arguments.
 */
boolean tm_util_get_fix_time
(
  const sm_GnssFixRptStructType *pz_Fix,
  tm_CalendarTimeT              *pz_TimeOut
)
{
  const sm_GnssNavPosition *pz_NavPos = NULL;

  if (NULL == pz_Fix || NULL == pz_TimeOut)
  {
    MSG_ERROR("null ptr passed in=%x out=%x", pz_Fix, pz_TimeOut, 0);
    return FALSE;
  }

  pz_NavPos = &pz_Fix->z_NavPos;

  if (C_GPS_WEEK_UNKNOWN != pz_NavPos->w_GpsWeek)
  {
    return tm_util_construct_gps_time(pz_NavPos->w_GpsWeek,
                                      pz_NavPos->q_GpsTimeMs,
                                      pz_NavPos->q_UtcTimeMs,
                                      pz_TimeOut);
  }
  else
  {
    return tm_util_construct_glo_time(pz_NavPos->u_GloNumFourYear,
                                      pz_NavPos->w_GloNumDaysInFourYear,
                                      pz_NavPos->q_GloTimeMs,
                                      pz_TimeOut);
  }
}

//! }
