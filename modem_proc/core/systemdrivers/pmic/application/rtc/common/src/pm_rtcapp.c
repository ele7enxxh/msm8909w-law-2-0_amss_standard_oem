/*! \file
*
*  \brief  pm_rtc_app.c ----File contains the implementation of the public APIs for RTCAPP resource type.
*  \details Implementation file for RTCAPP resourece type.
*  Each of the APIs checks for access and then if necessary directs
*  the call to Driver implementation or to RPC function for the master processor.
*
*    PMIC code generation Version: 2.0.0.22
*    This file contains code for Target specific settings and modes.
*
*  &copy; Copyright 2010 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/application/rtc/common/src/pm_rtcapp.c#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
10/29/2014   mr      RTC App API not returning correct timing (CR-698769)
08/04/2014   rk      MDM9x25 - time set to Jan61980 (CR 686401)

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_rtc_application.h"
#include "pmapp_rtc.h"


/*===========================================================================

                     FUNCTION DECLARATION

===========================================================================*/
void pm_rtc_app_rtc_conv_12hr_to_24hr_format(pm_rtc_julian_type *time_ptr);
void pm_rtc_app_tc_conv_24hr_to_12hr_format(pm_rtc_julian_type *time_ptr);
pm_hal_rtc_alarm pm_rtc_app_rtc_alarm_conv_app_to_hal(pm_rtc_alarm_type app_alarm);
dword pm_rtc_app_rtc_div_4x2(dword dividend, word divisor, word *rem_ptr);
uint32 pm_rtc_app_rtc_julian_to_secs(const pm_rtc_julian_type* julian_ptr);
pm_err_flag_type pm_rtc_app_rtc_secs_to_julian(uint32 secs, pm_rtc_julian_type *julian_ptr);


/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
pm_err_flag_type pm_rtc_init(void)
{
    pm_rtc_app_data_type* rtc_app_data = pm_rtc_app_get_data();
    if(rtc_app_data == NULL || rtc_app_data->init_complete == FALSE)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    return pm_reset_rtc_alarm(PM_RTC_ALL_ALARMS);
}

pm_err_flag_type pm_get_rtc_display_mode(pm_rtc_display_type *mode_ptr)
{
    pm_err_flag_type error = PM_ERR_FLAG__SUCCESS;
    pm_rtc_app_data_type* rtc_app_data = pm_rtc_app_get_data();
    if(rtc_app_data == NULL || rtc_app_data->init_complete == FALSE)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if(NULL != mode_ptr)
    {
        *mode_ptr = rtc_app_data->pm_app_rtc_current_display_mode;

        if ( *mode_ptr == PM_RTC_INVALID_MODE )
        {
            error =  PM_ERR_FLAG__RTC_BAD_DIS_MODE;
        }
    }
    else
    {
        error = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    return error;
}

pm_err_flag_type pm_set_rtc_display_mode(pm_rtc_display_type mode)
{
    pm_err_flag_type error = PM_ERR_FLAG__SUCCESS;
    pm_rtc_app_data_type* rtc_app_data = pm_rtc_app_get_data();
    if(rtc_app_data == NULL || rtc_app_data->init_complete == FALSE)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if ( mode >= PM_RTC_INVALID_MODE )
    {
        error =  PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    else
    {
        rtc_app_data->pm_app_rtc_current_display_mode = mode;
    }

    return error;
}

pm_err_flag_type pm_rtc_rw_cmd(pm_rtc_cmd_type cmd, pm_rtc_julian_type *current_time_ptr)
{
    pm_err_flag_type error = PM_ERR_FLAG__SUCCESS;
    pm_hal_rtc_time_type time_hal_format;
    pm_rtc_app_data_type* rtc_app_data = pm_rtc_app_get_data();

    if(rtc_app_data == NULL || rtc_app_data->init_complete == FALSE)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if ( cmd >= PM_RTC_INVALID_CMD )
    {
        error = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    else if ( !current_time_ptr )
    {
        error = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else if (rtc_app_data->pm_app_rtc_current_display_mode == PM_RTC_INVALID_MODE)
    {
        error = PM_ERR_FLAG__RTC_BAD_DIS_MODE;
    }
    else
    {
        switch (cmd)
        {
            case PM_RTC_SET_CMD:
                if ( !pm_is_rtc_valid(rtc_app_data->pm_app_rtc_current_display_mode, current_time_ptr))
                {
                    error = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
                }
                else
                {
                    if (rtc_app_data->pm_app_rtc_current_display_mode == PM_RTC_12HR_MODE)  /* time is in AM/PM mode? */
                    {
                        /* clk API only accepts 24 hr format. So we need to convert current */
                        /* time to 24 hr format before passing to clk API. */
                        pm_rtc_app_rtc_conv_12hr_to_24hr_format(current_time_ptr);
                    }

                    /* Assuming current_time_ptr is in 24 hour format */
                    time_hal_format.sec = pm_rtc_app_rtc_julian_to_secs(current_time_ptr);

                    // start RTC
                    error = pm_hal_rtc_start(&time_hal_format);
                }
                break;

            case PM_RTC_GET_CMD:
                error = pm_hal_rtc_get_time(&time_hal_format);

                /* current_time_ptr is in 24 hour format */
                if ( error == PM_ERR_FLAG__SUCCESS )
                {
                    error = pm_rtc_app_rtc_secs_to_julian (time_hal_format.sec, current_time_ptr ) ;
                    // Julian time is in 24hr format
                    // does the user need it in AM/PM format?
                    if (rtc_app_data->pm_app_rtc_current_display_mode == PM_RTC_12HR_MODE)
                    {
                        pm_rtc_app_tc_conv_24hr_to_12hr_format(current_time_ptr);
                    }
                }
                break;

            case PM_RTC_INVALID_CMD:
            /* this will never be reached here because sanity check for 'cmd' */
            /* has already been done above; just to satisfy LINT's curiousity! */
                break;

            /* we'll not have 'default' so as to leave open the possibility for LINT */
            /* to warn us when we do miss any new enums added in the future. */
        }
    }

    return error;
}

boolean pm_is_rtc_valid(pm_rtc_display_type rtcMode, const pm_rtc_julian_type *time)
{
    boolean is_time_valid = TRUE;
    pm_rtc_app_data_type* rtc_app_data = pm_rtc_app_get_data();
    if(rtc_app_data == NULL || rtc_app_data->init_complete == FALSE)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

     /* Is the time read within range? */
    if (     (time->year < rtc_app_data->PM_RTC_MIN_YEAR)
        ||  (time->year > rtc_app_data->PM_RTC_MAX_YEAR)
        ||  (time->month > rtc_app_data->PM_RTC_MAX_MONTH)
        /* ||   (time->day_of_week > rtc_app_data->PM_RTC_MAX_DOW) */
        ||  (time->day > rtc_app_data->PM_RTC_MAX_DAY)
        ||  (time->minute > rtc_app_data->PM_RTC_MAX_MINUTES)
        ||  (time->second > rtc_app_data->PM_RTC_MAX_SECONDS)       )
    {
       is_time_valid = FALSE;
    }
    else
    {
       // Is the number of hours read within range?
       // check for both 24 and 12 hour modes

       // Here is the list of valid values:
       // 24 HR-00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23
       // 12 HR-12 01 02 03 04 05 06 07 08 09 10 11 32 21 22 23 24 25 26 27 28 29 30 31
        switch(rtcMode)
        {
            case PM_RTC_12HR_MODE:
               if (    (time->hour > rtc_app_data->PM_RTC_MAX_12HR_HOUR)
                    || (time->hour < rtc_app_data->PM_RTC_MIN_12HR_HOUR)
                    || (   (time->hour > rtc_app_data->PM_RTC_MIDL_12HR_HOUR)
                        && (time->hour < rtc_app_data->PM_RTC_MIDH_12HR_HOUR) )
                  )
               {
                  is_time_valid = FALSE;
               }
               break;

            case PM_RTC_24HR_MODE:
               if (time->hour > rtc_app_data->PM_RTC_MAX_24HR_HOUR)
               {
                  is_time_valid = FALSE;
               }
               break;

            default:
               /* Invalid display mode */
               is_time_valid = FALSE;
        }
    }

    return is_time_valid;
}

pm_err_flag_type pm_rtc_set_time_adjust(uint8 time_adjust)
{
    pm_err_flag_type error = PM_ERR_FLAG__SUCCESS;
    uint8 time_adjust_hal_format = 0;

    if (time_adjust >= 128)
    {
        error = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    else
    {
        time_adjust_hal_format = (time_adjust < 64) ? (time_adjust + 64) : (128 - time_adjust);

           // Conversion Math: RTC_TIME_ADJ: correction = (X-64)*3.05ppm
           //   time_adjust  --->  time_adjust_hal_format
           //             0  --->  64        [No freq adjustment]
           //             1  --->  65
           //             2  --->  66
           //             ...
           //            63  --->  127
           //   --------------------------------------------------
           //            64  --->  64        [No freq adjustment]
           //            65  --->  63
           //            66  --->  62
           //            ...
           //           127  --->  1

        error = pm_hal_rtc_set_time_adjust(time_adjust_hal_format);
    }

    return error;
}

pm_err_flag_type pm_rtc_get_time_adjust(uint8 *time_adjust_ptr)
{
    pm_err_flag_type error = PM_ERR_FLAG__SUCCESS;
    uint8               time_adjust_hal_format = 0;

    error = pm_hal_rtc_get_time_adjust(&time_adjust_hal_format);

    if (PM_ERR_FLAG__SUCCESS == error)
    {
        // TODO: Ask Jeff why we set boundary instead of returning error?
        // If no error are return we will never know if the register is not returning
        // the right value.
        // time_adjust_hal_format will be in [0..127] but 0 (195 ppm) is not
        // allowed by pm_rtc_set_time_adjust() but anyway we do
        // checks to make sure that we have a valid value to work with...
        //
        if (time_adjust_hal_format <= 0)         // lower boundary check
        {
            time_adjust_hal_format = 1;
        }
        else if (time_adjust_hal_format > 127)   // upper boundary check
        {
            time_adjust_hal_format = 127;
        }

        *time_adjust_ptr = (time_adjust_hal_format < 64) ? (128 - time_adjust_hal_format) : (time_adjust_hal_format - 64);
         //   Conversion Math: RTC_TIME_ADJ: correction = (X-64)*3.05ppm
         //   time_adjust_hal_format  --->  *time_adjust_ptr
         //                 1  --->  127
         //                 2  --->  126
         //                 ...
         //                63  --->  65

         //   range for decreasing frequency or no change
         //   time_adjust_hal_format  --->  *time_adjust_ptr
         //                 64 --->  0       [No freq adjustment]
         //                 65 --->  1
         //                 ...
         //                127 --->  63
    }

    return error;
}

pm_err_flag_type pm_rtc_alarm_rw_cmd(pm_rtc_cmd_type cmd, pm_rtc_alarm_type what_alarm, pm_rtc_julian_type *alarm_time_ptr)
{
    pm_err_flag_type error = PM_ERR_FLAG__SUCCESS;
    pm_hal_rtc_alarm      alarm_hal_format  = PM_HAL_RTC_ALARM_INVALID;
    pm_hal_rtc_time_type  alarm_time_hal_format;
    pm_rtc_app_data_type* rtc_app_data = pm_rtc_app_get_data();

    if(rtc_app_data == NULL || rtc_app_data->init_complete == FALSE)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    // do some sanity checking on params ...
    if (cmd >= PM_RTC_INVALID_CMD)
    {
        error = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    else if ( (!what_alarm)                         // Is this any alarm?
            // Is this a valid one?
            || (what_alarm > PM_RTC_ALL_ALARMS)   //lint !e685 suppress "Relational operator '>' always evaluates to 'false'"
                                                  // Reason: PM_RTC_ALL_ALARMS is set up as an OR of all individual alarms
                                                  //         which themselves are varying powers of 2. So any alarm passed
                                                  //         to this func as something greater than PM_RTC_ALL_ALARMS will
                                                  //         be a bogus one, and this check will then evaluate to 'true'.
            // Does this include more than 1 alarm at the same time?
            || (what_alarm & (what_alarm-1)) )    //lint !e641 suppress "Converting enum 'pm_rtc_alarm_type' to int"
    {
        error = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else
    {
        alarm_hal_format = pm_rtc_app_rtc_alarm_conv_app_to_hal(what_alarm);

        if (!alarm_time_ptr)
        {
            error = PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
        }
        else
        {
            switch (cmd)
            {
                case PM_RTC_SET_CMD:
                    // Is the alarm time valid ?
                    if (!pm_is_rtc_valid(rtc_app_data->pm_app_rtc_current_display_mode, alarm_time_ptr))
                    {
                       error = PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
                    }
                    else
                    {
                        if (rtc_app_data->pm_app_rtc_current_display_mode == PM_RTC_12HR_MODE)  // time is in AM/PM mode?
                        {
                           // convert to a 24 hr time format understood by clk API
                           pm_rtc_app_rtc_conv_12hr_to_24hr_format(alarm_time_ptr);
                        }

                        // convert Julian time ---> seconds elapsed from a known time in the past
                        alarm_time_hal_format.sec = pm_rtc_app_rtc_julian_to_secs(alarm_time_ptr);
                        // enable alarm
                        error = pm_hal_rtc_enable_alarm(alarm_hal_format, &alarm_time_hal_format);
                    }
                    break;

                case PM_RTC_GET_CMD:
                    // get alarm time programmed from HAL
                    error = pm_hal_rtc_get_alarm_time(alarm_hal_format, &alarm_time_hal_format);

                    if (!error)
                    {
                        // convert seconds ---> Julian time
                        error = pm_rtc_app_rtc_secs_to_julian(alarm_time_hal_format.sec, alarm_time_ptr);

                        // Julian time is in 24hr format
                        // does the user need it in AM/PM format?
                        if (rtc_app_data->pm_app_rtc_current_display_mode == PM_RTC_12HR_MODE)
                        {
                           pm_rtc_app_tc_conv_24hr_to_12hr_format(alarm_time_ptr);
                        }
                    }
                    break;

                case PM_RTC_INVALID_CMD:
                    // this will never be reached here because sanity check for 'cmd'
                    // has already been done above; just to satisfy LINT's curiousity!
                    break;

                    // we'll not have 'default' so as to leave open the possibility for LINT
                    //   to warn us when we do miss any new enums added in the future.
            }
        }
    }

    return error;
}

pm_err_flag_type pm_reset_rtc_alarm(pm_rtc_alarm_type what_alarm)
{
    pm_err_flag_type error = PM_ERR_FLAG__SUCCESS;
    pm_hal_rtc_alarm      alarm_hal_format  = PM_HAL_RTC_ALARM_INVALID;
    pm_rtc_app_data_type* rtc_app_data = pm_rtc_app_get_data();

    if(rtc_app_data == NULL || rtc_app_data->init_complete == FALSE)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    // do some sanity checking on params ...
    if (what_alarm > PM_RTC_ALL_ALARMS)   //lint !e685 suppress "Relational operator '>' always evaluates to 'false'"
                                          // Reason: PM_RTC_ALL_ALARMS is set up as an OR of all individual alarms
                                          //         which themselves are varying powers of 2. So any alarm passed
                                          //         to this func as something greater than PM_RTC_ALL_ALARMS will
                                          //         be a bogus one, and this check will then evaluate to 'true'.
    {
       error = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    else
    {
        // reset ALARM_1 if required ...

        //lint -e{655} suppress "bit-wise operation uses (compatible) enum's"
        //lint -e{641} suppress "Converting enum 'pm_rtc_alarm_type' to int"
        if (PM_RTC_ALARM_1 == (what_alarm & PM_RTC_ALARM_1))
        {
           alarm_hal_format = pm_rtc_app_rtc_alarm_conv_app_to_hal(PM_RTC_ALARM_1);
           error = pm_hal_rtc_disable_alarm(alarm_hal_format);
        }
        // reset other alarms in the future here ...
    }

    return error;
}

pm_err_flag_type pm_get_rtc_alarm_status(uint8 *alarm_status)
{
    pm_err_flag_type error = PM_ERR_FLAG__SUCCESS;

    if(alarm_status == NULL)
    {
        error = PM_ERR_FLAG__INVALID_POINTER;
    }
    else
    {
        error = pm_hal_rtc_get_alarm_status(alarm_status);
    }

    return error;
}

void pm_rtc_pwr_rst_isr()
{
    pm_rtc_app_data_type* rtc_app_data = pm_rtc_app_get_data();

    // initialize the RTC driver after a RTC power reset condition
    // Clear all RTC alarms to make sure that we start off with a clean slate
    pm_reset_rtc_alarm(PM_RTC_ALL_ALARMS);

    // initialize the RTC driver after a RTC power reset condition
    // if anything else is needed as a result of RTC having been halted
    // (like setting new time, alarms, etc.), do those here ...

    rtc_app_data->pm_rtc_power_reset_status = TRUE;

    return;
}

boolean pm_rtc_get_power_reset_status()
{
    pm_rtc_app_data_type* rtc_app_data = pm_rtc_app_get_data();

    if(rtc_app_data == NULL || rtc_app_data->init_complete == FALSE)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        return rtc_app_data->pm_rtc_power_reset_status;
    }
}

/*************************************
NOTE: Regular METHOD IMPLEMENTATION
**************************************/
void pm_rtc_app_rtc_conv_12hr_to_24hr_format(pm_rtc_julian_type  *time_ptr)
{
    word hr = time_ptr->hour;

    if (hr == 12)
    {
        time_ptr->hour = 0;
    }
    else if (hr == 32)
    {
        time_ptr->hour = 12;
    }
    else if ( (hr >= 21) && (hr <= 31) )
    {
        time_ptr->hour -= 8;
    }
}

void pm_rtc_app_tc_conv_24hr_to_12hr_format(pm_rtc_julian_type  *time_ptr)
{
    word hr = time_ptr->hour;

    if (hr == 0)
    {
        time_ptr->hour = 12;
    }
    else if (hr == 12)
    {
        time_ptr->hour = 32;
    }
    else if ( (hr >= 13) && (hr <= 23) )
    {
        time_ptr->hour += 8;
    }
}

pm_hal_rtc_alarm pm_rtc_app_rtc_alarm_conv_app_to_hal(pm_rtc_alarm_type  app_alarm)
{
   pm_hal_rtc_alarm  alarm_hal_format = PM_HAL_RTC_ALARM_INVALID;

   switch (app_alarm)
   {
      case PM_RTC_ALARM_1:
         alarm_hal_format = PM_HAL_RTC_ALARM_1;
         break;

      default:
         break;
   }

   return alarm_hal_format;
}

dword pm_rtc_app_rtc_div_4x2(dword  dividend, word   divisor, word   *rem_ptr)
{
  dword quotient = 0 ;

  if ( divisor != 0 )
  {
    *rem_ptr = (word) (dividend % divisor);
    quotient = dividend / divisor ;
  }
  return quotient ;
}

uint32 pm_rtc_app_rtc_julian_to_secs( const  pm_rtc_julian_type* julian_ptr)
{
    pm_rtc_app_data_type* rtc_app_data = pm_rtc_app_get_data();

    if(rtc_app_data == NULL || rtc_app_data->init_complete == FALSE)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

  uint32  secs;  /* For computing and returning value. */

  /* First, compute number of days contained in the last whole leap
     year set. */

  secs = ( (julian_ptr->year - rtc_app_data->PM_RTC_BASE_YEAR) / 4 ) * rtc_app_data->PM_RTC_QUAD_YEAR;

  /* Now, add in days since last leap year to start of this month. */

  if ( (julian_ptr->year & 0x3) == 0 )
  {
    /* If this is a leap year, add in only days since the beginning of the
       year to start of this month. */

    secs += rtc_app_data->PM_RTC_LEAP_MONTH_TAB[ julian_ptr->month - 1 ];
  }

  else
  {
    /* If this is not a leap year, add in days since last leap year plus
       days since start of this year to start of this month. */

    secs += rtc_app_data->PM_RTC_YEAR_TAB[ julian_ptr->year & 0x3 ];

    secs += rtc_app_data->PM_RTC_NORMAL_MONTH_TAB[ julian_ptr->month - 1 ];
  }

  secs += julian_ptr->day - 1;  /* Add in days in current month. */

  secs = (secs * 24) + julian_ptr->hour;    /* Compute elapsed hours. */
  secs = (secs * 60) + julian_ptr->minute;  /* Compute elapsed minutes. */
  secs = (secs * 60) + julian_ptr->second;  /* Compute elapsed seconds. */

  secs -= rtc_app_data->RM_RTC_CLK_OFFSET_S; /* subtract number of seconds from user base date
                           of 6 Jan 1980 to calculation base date of
               1 Jan 1980 */
  return ( secs );  /* Return elapsed seconds. */
}

pm_err_flag_type pm_rtc_app_rtc_secs_to_julian(uint32  secs, pm_rtc_julian_type  *julian_ptr)
{
    pm_err_flag_type error = PM_ERR_FLAG__SUCCESS;
    pm_rtc_app_data_type* rtc_app_data = pm_rtc_app_get_data();

    if(rtc_app_data == NULL || rtc_app_data->init_complete == FALSE)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    int2  i;  /* For loops, etc. */
    uint16  days = 0;  /* For day computations. */

    /* Add number of seconds from Jan 1 to Jan 6 from input seconds
    in order to have number of seconds since Jan 1 1980 for calculation */
    secs += rtc_app_data->RM_RTC_CLK_OFFSET_S;

    /* Divide elapsed seconds by 60: remainder is seconds of Julian date;
    quotient is number of elapsed minutes. */

    secs = pm_rtc_app_rtc_div_4x2 ( secs, 60, &julian_ptr->second );

    /* Divide elapsed minutes by 60: remainder is minutes of Julian date;
    quotient is elapsed hours. */

    secs = pm_rtc_app_rtc_div_4x2 ( secs, 60, &julian_ptr->minute );

    /* Divide elapsed hours by 24: remainder is hours of Julian date;
    quotient is elapsed days. */

    secs = pm_rtc_app_rtc_div_4x2 ( secs, 24, &julian_ptr->hour );

    /* Now things get messier. We have number of elapsed days. The 1st thing
    we do is compute how many leap year sets have gone by. We multiply
    this value by 4 (since there are 4 years in a leap year set) and add
    in the base year. This gives us a partial year value. */

    julian_ptr->year = (word)( rtc_app_data->PM_RTC_BASE_YEAR + ( 4 * pm_rtc_app_rtc_div_4x2 ( secs, rtc_app_data->PM_RTC_QUAD_YEAR, &days ) ) );

    /* Now we use the year_tab to figure out which year of the leap year
    set we are in. */

    for ( i = 0; days >= rtc_app_data->PM_RTC_YEAR_TAB[ i + 1 ]; i++ );  /* Table search. */

    days -= rtc_app_data->PM_RTC_YEAR_TAB[ i ];  /* Subtract out days prior to current year. */

    julian_ptr->year += i;  /* Use search index to arrive at current year. */

    /* Now, we know year, hour, minute and second. We also know how many days
    we are into the current year. From this, we can compute day and month. */

    if ( i == 0 )  /* Is this a leap year? */
    {
        /* Use month table for leap years to find bracketing value. */

        for ( i = 0; days >= rtc_app_data->PM_RTC_LEAP_MONTH_TAB[ i + 1 ]; i++ );  /* Table seach. */

        julian_ptr->day = days - rtc_app_data->PM_RTC_LEAP_MONTH_TAB[ i ] + 1;  /* Find day of month. */

        julian_ptr->month = i + 1;  /* Find month. */
    }

    else
    {
        /* Use month table for non-leap years. */

        for ( i = 0; days >= rtc_app_data->PM_RTC_NORMAL_MONTH_TAB[ i + 1 ]; i++ );  /* Table search. */

        julian_ptr->day = days - rtc_app_data->PM_RTC_NORMAL_MONTH_TAB[ i ] + 1; /* Find day of month. */

        julian_ptr->month = i + 1;  /* Compute month. */
    }

    /* Initialize Day of week to 0 this data is calculated in time services each time */
    julian_ptr->day_of_week = 0 ;

    return error;
}

