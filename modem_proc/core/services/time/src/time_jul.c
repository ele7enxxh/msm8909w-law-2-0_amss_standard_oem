/*=============================================================================

                  T I M E   S E R V I C E   S U B S Y S T E M

                  J U L I A N   T I M E   C O N V E R S I O N


GENERAL DESCRIPTION
  This module contains procedures to convert seconds to and from Julian
  calendar units.

  IMPORTANT NOTE: The time calculations in this module are only valid for
  dates and times from 6 Jan 1988, 00:00:00 to 28 Feb 2100, 23:59:59.  This
  is due to the fact that this module assumes that all years evenly divisible
  by 4 are leap years. Unfortunately, century years (e.g., 1900, 2000, 2100)
  must also be evenly divisible by 400 in order to be leap years, and so the
  year 2100 does not qualify as a leap year.


EXTERNALIZED FUNCTIONS
  time_jul_from_secs
    This function converts a specified number of elapsed seconds since
    the base date to its equivalent Julian date and time.

  time_jul_to_secs
    This function converts a specified Julian date and time to an
    equivalent number of elapsed seconds since the base date.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.


Copyright (c) 1994 - 2013 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/time_jul.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/08/09   din     removed millisecond field from time_julian_type
06/17/09   din     Included missing time_jul.h
02/23/09   din     Consoldation of time.
12/05/08   tev     type cast (uint32) (QW_CVT_Q2N(time_ms) % 1000) to (uint16)
10/22/08   sep     Add time_jul_from_ms function
09/23/08   sep     Moved system_time_to_julian_time to time_jul.c
08/08/03   ajn     Coding standard update.
07/24/03   ajn     Xfer'd clkjul.c from MSM#### CLOCK VU's to general TIME VU

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_svc.h"
#include "misc.h"       /* Prototype for direct call to div4x2 */
#include "time_jul.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


/*-----------------------------------------------------------------------------
                       Time Translation Tables

  The following tables are used for making conversions between
  Julian dates and number of seconds.
-----------------------------------------------------------------------------*/

/* This is the number of days in a leap year set.
   A leap year set includes 1 leap year, and 3 normal years. */
#define TIME_JUL_QUAD_YEAR        (366+(3*365))

/* The year_tab table is used for determining the number of days which
   have elapsed since the start of each year of a leap year set. It has
   1 extra entry which is used when trying to find a 'bracketing' year.
   The search is for a day >= year_tab[i] and day < year_tab[i+1]. */

static const uint16 year_tab[] = {
  0,                              /* Year 0 (leap year) */
  366,                            /* Year 1             */
  366+365,                        /* Year 2             */
  366+365+365,                    /* Year 3             */
  366+365+365+365                 /* Bracket year       */
};


/* The norm_month_tab table holds the number of cumulative days that have
   elapsed as of the end of each month during a non-leap year. */

static const uint16 norm_month_tab[] = {
  0,                                    /* --- */
  31,                                   /* Jan */
  31+28,                                /* Feb */
  31+28+31,                             /* Mar */
  31+28+31+30,                          /* Apr */
  31+28+31+30+31,                       /* May */
  31+28+31+30+31+30,                    /* Jun */
  31+28+31+30+31+30+31,                 /* Jul */
  31+28+31+30+31+30+31+31,              /* Aug */
  31+28+31+30+31+30+31+31+30,           /* Sep */
  31+28+31+30+31+30+31+31+30+31,        /* Oct */
  31+28+31+30+31+30+31+31+30+31+30,     /* Nov */
  31+28+31+30+31+30+31+31+30+31+30+31   /* Dec */
};

/* The leap_month_tab table holds the number of cumulative days that have
   elapsed as of the end of each month during a leap year. */

static const uint16 leap_month_tab[] = {
  0,                                    /* --- */
  31,                                   /* Jan */
  31+29,                                /* Feb */
  31+29+31,                             /* Mar */
  31+29+31+30,                          /* Apr */
  31+29+31+30+31,                       /* May */
  31+29+31+30+31+30,                    /* Jun */
  31+29+31+30+31+30+31,                 /* Jul */
  31+29+31+30+31+30+31+31,              /* Aug */
  31+29+31+30+31+30+31+31+30,           /* Sep */
  31+29+31+30+31+30+31+31+30+31,        /* Oct */
  31+29+31+30+31+30+31+31+30+31+30,     /* Nov */
  31+29+31+30+31+30+31+31+30+31+30+31   /* Dec */
};

/* The day_offset table holds the number of days to offset
   as of the end of each year. */

static const uint16 day_offset[] = {
  1,                                    /* Year 0 (leap year) */
  1+2,                                  /* Year 1             */
  1+2+1,                                /* Year 2             */
  1+2+1+1                               /* Year 3             */
};



/*-----------------------------------------------------------------------------
  Date conversion constants
-----------------------------------------------------------------------------*/

/* 5 days (duration between Jan 1 and Jan 6), expressed as seconds. */

#define TIME_JUL_OFFSET_S         432000UL


/* This is the year upon which all time values used by the Clock Services
** are based.  NOTE:  The user base day (GPS) is Jan 6 1980, but the internal
** base date is Jan 1 1980 to simplify calculations */

#define TIME_JUL_BASE_YEAR        1980



/*=============================================================================

FUNCTION TIME_JUL_FROM_SECS

DESCRIPTION
  This procedure converts a specified number of elapsed seconds   
  since the base date to its equivalent Julian date and time.     

DEPENDENCIES
  None

RETURN VALUE
  The specified Julian date record is filled in with equivalent date/time,
  and returned into the area pointed to by julian_ptr.

SIDE EFFECTS
  None

=============================================================================*/

void time_jul_from_secs
(
  /* Number of seconds since base date */
  uint32                          secs,

  /* OUT: Pointer to Julian date record */
  time_julian_type                *julian
)
{
  /* Loop index */
  unsigned int /* fast */         i;

  /* Days since beginning of year or quad-year */
  uint16                          days;

  /* Quad years since base date */
  unsigned int /* fast */         quad_years;

  /* Leap-year or non-leap year month table */
  const uint16                    *month_table;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Add number of seconds from Jan 1 to Jan 6 from input seconds
     in order to have number of seconds since Jan 1, 1980 for calculation */

  secs += TIME_JUL_OFFSET_S;


  /* Divide elapsed seconds by 60: remainder is seconds of Julian date;
     quotient is number of elapsed minutes. */

  secs = div4x2 ( secs, 60, &julian->second );


  /* Divide elapsed minutes by 60: remainder is minutes of Julian date;
     quotient is elapsed hours. */

  secs = div4x2 ( secs, 60, &julian->minute );


  /* Divide elapsed hours by 24: remainder is hours of Julian date;
     quotient is elapsed days. */

  secs = div4x2 ( secs, 24, &julian->hour );


  /* Now things get messier. We have number of elapsed days. The 1st thing
     we do is compute how many leap year sets have gone by. We multiply
     this value by 4 (since there are 4 years in a leap year set) and add
     in the base year. This gives us a partial year value. */

  quad_years = div4x2( secs, TIME_JUL_QUAD_YEAR, &days );

  julian->year = TIME_JUL_BASE_YEAR + (4 * quad_years);


  /* Now we use the year_tab to figure out which year of the leap year
     set we are in. */

  for ( i = 0; days >= year_tab[ i + 1 ]; i++ )
  {
    /* No-op. Table seach. */
  }

  /* Subtract out days prior to current year. */
  days -= year_tab[ i ];
  
  /* Use search index to arrive at current year. */
  julian->year += i;  


  /* Take the day-of-week offset for the number of quad-years, add in
     the day-of-week offset for the year in a quad-year, add in the number
     of days into this year. */

  julian->day_of_week =
        (day_offset[3] * quad_years + day_offset[i] + days) % 7;


  /* Now we know year, hour, minute and second. We also know how many days
     we are into the current year. From this, we can compute day & month. */


  /* Use leap_month_tab in leap years, and norm_month_tab in other years */

  month_table = (i == 0) ? leap_month_tab : norm_month_tab;


  /* Search month-table to compute month */

  for ( i = 0; days >= month_table[ i + 1 ]; i++ )
  {
    /* No-op. Table seach. */
  }


  /* Compute & store day of month. */
  julian->day = days - month_table[ i ] + 1;  

  /* Store month. */
  julian->month = i + 1;

} /* time_jul_from_secs */

/*=============================================================================

FUNCTION TIME_JUL_FROM_MS

DESCRIPTION
  This procedure converts a specified number of elapsed milliseconds   
  since the base date to its equivalent Julian date and time.     

DEPENDENCIES
  None

RETURN VALUE
  The specified Julian date record is filled in with equivalent date/time,
  and returned into the area pointed to by julian_ptr.

SIDE EFFECTS
  None

=============================================================================*/
void time_jul_from_ms
(
  /* Number of milliseconds since base date */
  time_type                        time_ms,

  /* OUT: Pointer to Julian date record */
  time_julian_type                *julian
)
{
    /* Seconds since base date */
    uint32 secs;

    /* Seconds since base date stored as a qword */
    time_type qw_seconds;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /*Convert the milliseconds to seconds*/
    qw_div(qw_seconds, time_ms, 1000);
    secs = qw_lo(qw_seconds);

    /*Get the julian time from the seconds*/
    time_jul_from_secs(secs, julian);
}



/*=============================================================================

FUNCTION TIME_JUL_TO_SECS

DESCRIPTION
  This procedure converts a specified Julian date and time to an  
  equivalent number of elapsed seconds since the base date.    

DEPENDENCIES
  None

RETURN VALUE
  Number of elapsed seconds since base date.       

SIDE EFFECTS
  None

=============================================================================*/

uint32 time_jul_to_secs
(
  /* Pointer to Julian date record */
  const time_julian_type          *julian
)
{
  /* Time in various units (days, hours, minutes, and finally seconds) */
  uint32                          time;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* First, compute number of days contained in the last whole leap
     year set. */

  time = ( (julian->year - TIME_JUL_BASE_YEAR) / 4L ) * TIME_JUL_QUAD_YEAR;


  /* Now, add in days since last leap year to start of this month. */
  if (( julian->month > 0 ) && ( julian->month <= 13 )){
     if ((julian->year & 0x3) == 0 )
     {
       /* If this is a leap year, add in only days since the beginning of the
          year to start of this month. */

       time += leap_month_tab[ julian->month - 1 ];
     }
     else
     {
       /* If this is not a leap year, add in days since last leap year plus
          days since start of this year to start of this month. */

       time += year_tab[ julian->year & 0x3 ];

       time += norm_month_tab[ julian->month - 1 ];
     }
  }

  /* Add in days in current month. */
  time += julian->day - 1;

  /* Add in elapsed hours, minutes, and seconds  */
  time = time * 24  +  julian->hour;
  time = time * 60  +  julian->minute;
  time = time * 60  +  julian->second;


  /* Subtract number of seconds from base (GPS) date of 6 Jan 1980 to
     calculation base date of 1 Jan 1980 */

  time -= TIME_JUL_OFFSET_S;


  /* Return elapsed seconds. */
  return time;

} /* time_jul_to_secs */

