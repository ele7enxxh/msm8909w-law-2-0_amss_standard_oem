
/*
==============================================================================

FILE:         timetickLegacy.c

DESCRIPTION:  
  This is the legacy timetick file. It access the timetick dal to read 
  the timetick count register. All the timetick values returned from this file 
  are calculated assuming that the underlying frequency is sclk frequency 
  (32768 Hz). 

===============================================================================
*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "timetick.h"
#include "DDITimetick.h"

/*=========================================================================   
   
                              DATA DEFINITIONS
   
==========================================================================*/
/*------------------------------------------------------------------------
  Constants used by the timetick driver
------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------
  Microsecond threshold for slow clock
     50000 us * 60 kHz < 2^32 micro-cycles
------------------------------------------------------------------------*/
#define TIMETICK_US_THRESHOLD     50000uL

/*-----------------------------------------------------------------------
  Millisecond threshold for slow clock
     60000 ms * 60 kHz < 2^32 milli-cycles
------------------------------------------------------------------------*/
#define TIMETICK_MS_THRESHOLD     60000uL

/*-------------------------------------------------------------------------
  Division factor to convert from sclk ticks to QTimer ticks and viceversa
--------------------------------------------------------------------------*/
#define SCLK_DIV_FACTOR           586

/*
 * phDalTimetickLegacyHandle
 *
 * Handle for communicating with the Timetick DAL.
 */
static DalDeviceHandle *phDalTimetickLegacyHandle = NULL;
static DALSYSSyncHandle phDalTimetickLegacySyncHandle;
static DALSYSSyncObj pDalTimetickLegacySyncObj;
/*=============================================================================

                   FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

FUNCTION CVT_TICKS_TO_MS

DESCRIPTION
  Converts the given 32KHz ticks to milliseconds

DEPENDENCIES
  Valid ticks 

RETURN VALUE
  # of milliseconds which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

static uint32 cvt_ticks_to_ms
(
  uint32                ticks
    /* Duration to be converted into milliseconds */
)
{
  uint32                milliseconds = 0;
  uint64         ticks_times_sclkdiv = (uint64)ticks * SCLK_DIV_FACTOR;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ticks_times_sclkdiv >= 19200 )
  {
    milliseconds  = ticks_times_sclkdiv / 19200;
    ticks_times_sclkdiv%=  19200;
  }
  if (ticks_times_sclkdiv >= 9600 )
  {
    /*rounding */
    milliseconds ++;
  }
  return milliseconds;

} /* cvt_ticks_to_ms */

/*=============================================================================

FUNCTION CVT_MS_TO_TICKS

DESCRIPTION
  Converts the given millisecond amount to 32KHzticks

DEPENDENCIES
  Valid ticks

RETURN VALUE
  # of ticks which correspond to the given time value

SIDE EFFECTS
  None

=============================================================================*/

static uint32 cvt_ms_to_ticks
(
  uint32                msecs
    /* Duration to be converted into ticks */
)
{
  uint32      ticks = 0;
    /* Tick duration */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ticks = ((uint64)msecs * 19200)/SCLK_DIV_FACTOR;
  return (ticks);

} /* cvt_ms_to_ticks */

/*=============================================================================

FUNCTION CVT_TIMETICK64_TO_MS

DESCRIPTION
  Converts the given 32KHz ticks value to milliseconds

DEPENDENCIES
  Valid ticks

RETURN VALUE
  # of milliseconds which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

static uint64 cvt_timetick64_to_ms
(
  uint64                          ticks
    /* Duration to be converted into milliseconds */
)
{
  uint64                          milliseconds = 0;
  uint64         ticks_times_sclkdiv = ticks * SCLK_DIV_FACTOR;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ticks_times_sclkdiv >= 19200 )
  {
    milliseconds  = ticks_times_sclkdiv / 19200;
    ticks_times_sclkdiv %=  19200;
  }
  if (ticks_times_sclkdiv >= 9600 )
  {
    /*rounding */
    milliseconds ++;
  }
  return milliseconds;

} /* cvt_timetick64_to_ms */


/*=============================================================================

FUNCTION CVT_MS_TO_TIMETICK64

DESCRIPTION
  Converts the given millisecond amount to 32KHz ticks

DEPENDENCIES
  Valid timetick estimate

RETURN VALUE
  # of slow clocks which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

static uint64 cvt_ms_to_timetick64
(
  uint64                          msecs
    /* Duration to be converted into ticks */
)
{
  uint64                          ticks = 0;
    /* Tick duration */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ticks = (msecs * 19200)/SCLK_DIV_FACTOR;
  return (ticks);

} /* cvt_ms_to_timetick64 */


/*=============================================================================

FUNCTION CVT_US_TO_TICKS

DESCRIPTION
  Converts the given microsecond value to 32KHz ticks

DEPENDENCIES
  Valid ticks

RETURN VALUE
  # of ticks which correspond to the given time value

SIDE EFFECTS
  None

=============================================================================*/

static uint32 cvt_us_to_ticks
(
  uint32                microseconds
  /* Duration to be converted into ticks */
)
{

  uint32                          ticks = 0;

    /* Duration in ticks */
  ticks = ((uint64)microseconds * 192)/ (SCLK_DIV_FACTOR * 10);
  return (ticks);

} /* cvt_us_to_ticks */


/*=============================================================================

FUNCTION CVT_US_TO_TIMETICK64

DESCRIPTION
  Converts the given microsecond value to 32KHz ticks

DEPENDENCIES
  Valid ticks

RETURN VALUE
  # of ticks which correspond to the given time value

SIDE EFFECTS
  None

=============================================================================*/

static uint64 cvt_us_to_timetick64
(
  /* Duration to be converted into slow clocks */
  uint64                          microseconds
    
)
{

  uint64                          ticks = 0;
    
  /* Duration in ticks */
  ticks = (microseconds * 192)/ (SCLK_DIV_FACTOR * 10);
  return (ticks);

} /* cvt_us_to_timetick64 */


/*=============================================================================

FUNCTION timetick_attach

DESCRIPTION
  Attaches to the Timetick DAL if it was not already done

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Gives a handle to the Timetick DAL

=============================================================================*/
 
static void timetick_attach(void)
{

  /* Check if the handle for timetick DAL already exists */
  if (phDalTimetickLegacyHandle == NULL)
  {
    DalDeviceHandle        *phTempTimetick;
    if(DAL_SUCCESS == DalTimetick_Attach("SystemTimer", &phTempTimetick))
    {
       phDalTimetickLegacyHandle = phTempTimetick;
       if(phDalTimetickLegacySyncHandle == NULL)
       {
         /* Create a sync object to guard critical sections */
         DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, 
                             &phDalTimetickLegacySyncHandle, 
                             &pDalTimetickLegacySyncObj);
       }
	       
    }
  }
}
/*=============================================================================

FUNCTION TIMETICK_GET_MIN_REARM_SCLK

DESCRIPTION
  Returns 0 as we do not use it with QTimer.

DEPENDENCIES
  None

RETURN VALUE
  MIN_REARM_SCLK

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get_min_rearm_sclk (void)
{
  return 0;
}
/*=============================================================================

FUNCTION TIMETICK_CVT_TO_SCLK

DESCRIPTION
  Converts the given time value to slow clocks

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  # of slow clocks which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_cvt_to_sclk
(
  /* Time interval to convert to slow clocks */
  timetick_type                   time,

  /* Units of time interval */
  timer_unit_type                 unit
)
{
  switch ( unit )
  {
    case T_HOUR:
      /* Convert hours to minutes */
      time *= 60;
      /*lint -fallthrough */

    case T_MIN:
      /* Convert minutes to seconds */
      time *= 60;
      /*lint -fallthrough */

    case T_SEC:
      /* Seconds to slow clocks */
      time =((uint64)time * 19200000)/SCLK_DIV_FACTOR;
      break;

    case T_MSEC:
      /* Milliseconds to slow clocks */
      time = cvt_ms_to_ticks( time );
      break;

    case T_USEC:
      /* Microseconds to slow clocks */
      time = cvt_us_to_ticks( time );
      break;

    case T_TICK:
      /* time = time; */
      break;

    default:
      break;
  }

  return time;

} /* timetick_cvt_to_sclk */


/*=============================================================================

FUNCTION TIMETICK_SCLK_TO_PREC_US

DESCRIPTION
  Converts the given slow clock value to precision microseconds. This function
  also supports the full range of sclk values.

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  # of microseconds which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

uint64 timetick_sclk_to_prec_us
(
  uint32      time
    /* Duration in sclks to be converted into microseconds */
)
{

 return (((uint64)time * SCLK_DIV_FACTOR * 10) / 192);

} /* timetick_sclk_to_prec_us */


/*=============================================================================

FUNCTION TIMETICK_CVT_FROM_SCLK

DESCRIPTION
  Converts the slow clock time value to the given unit

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  Time in the unit requested

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_cvt_from_sclk
(
  /* Time interval to convert to slow clocks */
  timetick_type                   time,

  /* Units of time interval */
  timer_unit_type                 unit
)
{
   /* Factor for converting to hours or minutes */
  uint32                          scale = 1;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( unit )
  {
    case T_HOUR:
      /*  Scale minutes to hours */
      scale *= 60;
      /*lint -fallthrough */

    case T_MIN:
      /* Scale seconds to minutes */
      scale *= 60;
      /*lint -fallthrough */

    case T_SEC:
      /* Convert slow clocks to seconds (or minutes, or hours) */
      time = ((uint64)time * SCLK_DIV_FACTOR) / ((uint64) scale * 19200000 );
      break;

    case T_MSEC:
      /* Convert slow clocks to milliseconds */
      time = cvt_ticks_to_ms( time );
      break;

    case T_USEC:
      /* Convert slow clocks to microseconds */
      time = cvt_ticks_to_ms( time * 10 ) * 100;
      break;

    case T_TICK:
      /* time = time; */
      break;

    default:
      break;
  }

  return time;

} /* timetick_cvt_from_sclk */

/*=============================================================================

FUNCTION TIMETICK_GET

DESCRIPTION
  Returns the equivalent current timetick count w.r.t 32KHz frequency. 
  The actual count returned from the DAL APIs is 19.2MHz based. We divide it 
  by SCLK_DIV_FACTOR to get the equivalent sclk ticks. 

DEPENDENCIES
  None.

RETURN VALUE
  Slow clock timetick counter value

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get( void )
{

  DalTimetickTime64Type           count64 = 0;

  timetick_attach();

  if (phDalTimetickLegacyHandle != NULL)
  {
    DalTimetick_GetTimetick64(phDalTimetickLegacyHandle, &count64);
  }

  return ((timetick_type)(count64/SCLK_DIV_FACTOR));

} /* timetick_get */


/*=============================================================================

FUNCTION TIMETICK_INIT_SCLK64

DESCRIPTION
  Does nothing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timetick_init_sclk64( void )
{
  /* do nothing */
} /* timetick_init_sclk64 */


/*=============================================================================

FUNCTION TIMETICK_GET_SCLK64

DESCRIPTION
  Reads the 64 bit timetick count and converts into ticks based on 
  32KHz frequency.

DEPENDENCIES
  None

RETURN VALUE
  64-bit tick count

SIDE EFFECTS
  None

=============================================================================*/


uint64 timetick_get_sclk64( void )
{
 /* timetick in 64 bits */
  uint64                          sclk64 = 0;

  timetick_attach();

  if (phDalTimetickLegacyHandle != NULL)
  {
    DalTimetick_GetTimetick64(phDalTimetickLegacyHandle, &sclk64);
  }
  /* Return 64-bit sclk */
  return (sclk64/SCLK_DIV_FACTOR);

} /* timetick_get_sclk64 */



/*=============================================================================

FUNCTION TIMETICK_GET_ELAPSED

DESCRIPTION
  Compute the time elapsed from a previous timetick value

DEPENDENCIES
  Valid slow clock estimate.

RETURN VALUE
  Elapsed time, in the unit provided

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get_elapsed
(
  /* Time tick value at start of interval */
  timetick_type                   start,

  /* Units to return time interval in */
  timer_unit_type                 unit
)
{
  timetick_type                 end = 0;

  end = timetick_get();
  return timetick_diff(start, end, unit);

} /* timetick_get_elapsed */



/*=============================================================================

FUNCTION TIMETICK_DIFF

DESCRIPTION
  Compute the difference between two slow clock tick counts

DEPENDENCIES
  Valid slow clock estimate.

RETURN VALUE
  Time difference between the two slow clock tick counts, in the unit given

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_diff
(
  /* Time tick value at start of interval */
  timetick_type                   start,

  /* Time tick value at end of interval */
  timetick_type                   end,

  /* Units to return time interval in */
  timer_unit_type                 unit
)
{
  timetick_type delta = end - start;

  return timetick_cvt_from_sclk(delta, unit);

} /* timetick_diff */



/*=============================================================================

FUNCTION TIMETICK_GET_MS

DESCRIPTION
  Returns a monotonically increasing millisecond count.
  *** This function sacrifices accuracy for speed ***

DEPENDENCIES
  None

RETURN VALUE
  Millisecond tick count

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get_ms( void )
{
  uint64                   ticks = 0;
  timetick_type            ms   = 0;
  
  timetick_attach(); 
  DALSYS_SyncEnter(phDalTimetickLegacySyncHandle);  
  ticks = timetick_get_sclk64();
  ms = (timetick_type) cvt_timetick64_to_ms(ticks);
  DALSYS_SyncLeave(phDalTimetickLegacySyncHandle);
    
  return ms;


} /* timetick_get_ms */


/*=============================================================================

FUNCTION TIMETICK_GET_RAW

DESCRIPTION
  Read the SLEEP_XTAL_TIMETICK counter, and return the current count.

DEPENDENCIES
  None

RETURN VALUE
  Slow clock timetick counter value

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get_raw( void )
{
  return timetick_get();
} /* timetick_get_raw */

/*=============================================================================

FUNCTION TIMETICK_GET_SAFE

DESCRIPTION
    Returns the SLEEP_XTAL_TIMETICK/AGPT timer count.
  
DEPENDENCIES

RETURN VALUE
  Slow clock timetick counter value

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get_safe( void )
{
  return timetick_get();
} /* timetick_get_safe */


/*=============================================================================

FUNCTION TIMETICK_ENABLE

DESCRIPTION
  Enables the Timetick module

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timetick_enable (void)
{
  // do nothing
} /* timetick_enable */

/*=============================================================================

FUNCTION TIMETICK_CVT_TO_SCLK64

DESCRIPTION
  Converts the given time value to slow clocks

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  # of slow clocks which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

uint64 timetick_cvt_to_sclk64
(
  /* Time interval to convert to slow clocks */
  uint64                   time,

  /* Units of time interval */
  timer_unit_type                 unit
)
{

  switch ( unit )
  {
    case T_HOUR:
      /* Convert hours to minutes */
      time *= 60;
      /* fall-through */

    case T_MIN:
      /* Convert minutes to seconds */
      time *= 60;
      /* fall-through */

    case T_SEC:
      /* Seconds to ticks */
      time = (time * 19200000)/SCLK_DIV_FACTOR;
      break;

    case T_MSEC:
      /* Milliseconds to ticks */
      time = cvt_ms_to_timetick64(time );
      break;

    case T_USEC:
      /* Microseconds to ticks */
      time = cvt_us_to_timetick64(time );
      break;

    case T_TICK:
      /* time = time; */
      break;

    default:
     break;
  }
  return time;

} /* timetick_cvt_to_sclk64 */


/*=============================================================================

FUNCTION TIMETICK_CVT_FROM_SCLK64

DESCRIPTION
  Converts the slow clock time value to the given unit

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  Time in the unit requested

SIDE EFFECTS
  None

=============================================================================*/

uint64 timetick_cvt_from_sclk64
(
  /* Time interval to convert to slow clocks */
  uint64                   time,

  /* Units of time interval */
  timer_unit_type                 unit
)
{
 /* Factor for converting to hours or minutes */
  uint64                          scale = 1;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( unit )
  {
    case T_HOUR:
      /*  Scale minutes to hours */
      scale *= 60;
      /* fall-through */

    case T_MIN:
      /* Scale seconds to minutes */
      scale *= 60;
      /* fall-through */

    case T_SEC:
      /* Convert slow clocks to seconds (or minutes, or hours) */
      time =  (time * SCLK_DIV_FACTOR)/(19200000 * scale);
      break;

    case T_MSEC:
      /* Convert ticks to milliseconds */
      time = cvt_timetick64_to_ms( time );
      break;

    case T_USEC:
      /* Convert ticks to microseconds */
      time = cvt_timetick64_to_ms( time * 10 ) * 100;
      break;

    case T_TICK:
      /* time = time; */
      break;

    default:
      break;
  }
  return time;

} /* timetick_cvt_from_sclk64 */

/*=============================================================================

FUNCTION TIMETICK_CVT_FROM_USEC

DESCRIPTION
  Converts the slow clock time value to the given unit

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  Time in the unit requested

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_cvt_from_usec
(
  /* Time interval to convert to slow clocks */
  uint64                   time,

  /* Units of time interval */
  timer_unit_type                 unit
)
{
  /* Factor for converting to hours or minutes */
  uint32                          scale = 1;

  /* Converted value in 32-bit integer */
  uint32           converted_time = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( unit )
  {
    case T_HOUR:
      /*  Scale minutes to hours */
      scale *= 60;
      /* fall-through */

    case T_MIN:
      /* Scale seconds to minutes */
      scale *= 60;
      /* fall-through */

    case T_SEC:
      /* Convert slow clocks to seconds (or minutes, or hours) */
      scale *= 1000;

    case T_MSEC:
      scale *= 1000;

    case T_USEC:
      converted_time = (timetick_type)(time/scale);
      break;

    case T_TICK:
      converted_time = cvt_us_to_ticks(time);
      break;

    default:
      break;
  }
  return converted_time;

} /* timetick_cvt_from_usec */


/*=============================================================================



FUNCTION TIMETICK_CVT_TO_USEC

DESCRIPTION
  Converts the given time value to slow clocks

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  # of slow clocks which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

uint64 timetick_cvt_to_usec
(
  /* Time interval to convert to slow clocks */
  timetick_type                   time,

  /* Units of time interval */
  timer_unit_type                 unit
)
{
  uint64   usecs;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  usecs = (uint64)time;

  switch ( unit )
  {
    case T_HOUR:
      /* Convert hours to minutes */
      usecs *= 60;
      /* fall-through */
    case T_MIN:
      /* Convert minutes to seconds */
      usecs *= 60;
      /* fall-through */
    case T_SEC:
      /* Seconds to slow clocks */
      usecs *= 1000;
    case T_MSEC:
      /* Milliseconds to slow clocks */
      usecs = usecs * 1000;
    case T_USEC:
      break;
    case T_TICK:
      usecs = ((uint64)time * SCLK_DIV_FACTOR * 10) / 192;
      break;

    default:
      break;
  }

  return usecs;

} /* timetick_cvt_to_usec */
