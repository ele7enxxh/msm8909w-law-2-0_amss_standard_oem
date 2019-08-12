/*=============================================================================

       T I M E   S E R V I C E   A T S   R T C   S U B S Y S T E M

GENERAL DESCRIPTION
  Initializes and maintains RTC time in the  Generic Framework.

REGIONAL FUNCTIONS
  ats_rtc_init
    Initialize the ats_rtc subsystem

  ats_rtc_remote_update
    Update Time_rtcs from the current proc to another proc

  ats_rtc_genoff_cb
    Callback called to remotely update offset

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ats_rtc_init() must be called.


Copyright (c) 2009 - 2013 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/ats_rtc.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/26/11   din     Fixed compiler warnings.
12/09/10   din     Fixed kockwork errors.
07/15/10   din     Featured Out Init call for base ATS_RTC.
04/27/10   din     Fixed compiler warnings.
12/8/09    din     Removed compilation warnings.
10/07/09   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "time_genoff_v.h"
#include "ats_v.h"
#include "ats_rtc.h"
#include "time_jul.h"
#include "time_remote.h"
#include "dog.h"
#include "DALSys.h"
#include "time_fn_map.h"
#include "timetick.h"
#include "time_svc.h"
#include "timetick_sclk64.h"


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/
/* Frequency of Sclk used */
#define TIME_CONV_SCLK_FREQ            19200000
/* Number of ticks per ms */
#define TIME_CONV_SCLK_FREQ_MS         (TIME_CONV_SCLK_FREQ/1000)

/*-----------------------------------------------------------------------------
  Time RTC static information
-----------------------------------------------------------------------------*/

/* Enum-type of different bases */
static time_bases_type        time_base;

/* Pointer to ats_rtc in consideration */
static time_genoff_ptr        ptime_genoff;

static boolean                read_pmic_rtc;

static ats_rtc_status_type   rtc_status;

time_julian_type             julian_at_bootup = {0};
boolean                      gvalid1, gvalid2, gvalid3;
/*--------------------------------------------------------------------------
  Linting...
--------------------------------------------------------------------------*/

/*lint -esym(715,unused_*)  parameters prefixed with unused_ are not used */



/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/



/*=============================================================================

FUNCTION ATS_RTC_SET_FROM_PMIC_RTC

DESCRIPTION
  Initialize the time-rtc value from the PMIC's Real Time Clock,
  if available.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void ats_rtc_set_from_pmic_rtc( void )
{

  /* Seconds since January 6, 1980 00:00:00 */
  int64                           secs;
  
  /* Flag indicating that the PMIC RTC value is valid */
  boolean                         valid = FALSE;

  /* Julian time/date from the PMIC's Real Time Clock */
  time_julian_type                julian = {0};

  /* instace to send the display mode parameter. Initialize the display mode 
   to 2 which is an invalid display mode when using PMIC RTC*/
  time_fn_map_ds_type             rtc_disp_mode = {0};

  boolean                         temp_rtc_status =FALSE;
  
  uint64                           timetick = 0;

   #if (defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_TIME_NO_RPC)) \
                                        && defined(IMAGE_APPS_PROC)
   return;
   #else /* FEATURE_MULTIPROCESSOR && IMAGE_APPS_PROC */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If ats_rtc has already been read from the PMIC's RTC ... */
  if ( read_pmic_rtc )
  {
    /* ... don't bother re-reading it - not much point. */
    return;
  }


  /* If ats_rtc has already been set from CDMA or HDR system time ... */
  //if ( ats_rtc.initialized )
  //{
    /* ... we've got better than the PMIC RTC's 1 second resolution. */
    //return;
  //}

  rtc_disp_mode.display_mode = ATS_RTC_DISP_MODE_24HR; 

  /* Force display mode to 24 hours. Don't check for
   the result here since if its PM1000, its suppose
   to return a false.*/
  time_fn_map_set_rtc_display_mode(rtc_disp_mode);

  /* Retrieve the real time clock's Julian time/date from the PMIC*/
  gvalid1 = valid = time_fn_map_get_pmic_time(&julian);

  /* Be sure that the time/date is valid */
  valid &= time_fn_map_is_rtc_valid(&julian);
  gvalid2 = valid;

  gvalid3 = valid = time_fn_map_poll_time_for_boundary(&temp_rtc_status,valid, &julian);

  if(temp_rtc_status)
  {
    rtc_status = TS_RTC_VALID;
  }
  else
  {
    rtc_status = TS_RTC_INVALID;
  }
  
  julian_at_bootup = julian;
  /* If the PMIC Real Time Clock's date/time is valid, convert it to the
     timestamp format for the time-of-day base timestamp.  */

  if ( valid )
  {
    /* Convert RTC value to seconds from Jan 6, 1980 00:00:00 */
    secs = time_jul_to_secs ( &julian );

    secs = secs * 1000; /* convert to msecs */
    
    timetick = timetick_sclk64_get();
    
    timetick = timetick/TIME_CONV_SCLK_FREQ_MS; 

    /* Moving rtc time to bootup of target so that for time calculations 
       timetick can be used straightly as uptime instead of subtracting 
       base_timetick always */    
    secs -= timetick;  

    /* Set Generic Offset */
    time_genoff_set_generic_offset( ptime_genoff, secs );
  }

#endif /* FEATURE_MULTIPROCESSOR && IMAGE_MODEM_PROC */

} /* ats_rtc_set_from_pmic_rtc */


/*=============================================================================

FUNCTION ATS_PRINT_RTC_STATUS

DESCRIPTION
Prints the RTC bootup time & RTC time now

DEPENDENCIES

RETURN VALUE
Returns the value in rtc_statuc global var

SIDE EFFECTS

=============================================================================*/

ats_rtc_status_type ats_print_rtc_status( void )
{
  time_fn_map_ds_type             rtc_disp_mode = {0};

  /* Julian time/date now from the PMIC's Real Time Clock */
  time_julian_type                julian_local = {0};
  boolean                         valid = FALSE;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ATS_MSG_HIGH("TIME_DEBUG: Bootup RTC Date = %d-%d-%d ", 
             julian_at_bootup.day, 
			 julian_at_bootup.month,
			 julian_at_bootup.year);
  ATS_MSG_HIGH("TIME_DEBUG: Bootup RTC Time = %d:%d:%d ", 
             julian_at_bootup.hour, 
			 julian_at_bootup.minute,
			 julian_at_bootup.second);

   ATS_MSG_HIGH("TIME_DEBUG: gvalid1: %d, gvalid2: %d, gvalid3: %d",
              gvalid1,
              gvalid2,
              gvalid3);

   rtc_disp_mode.display_mode = ATS_RTC_DISP_MODE_24HR; 

  /* Force display mode to 24 hours. Don't check for
   the result here since if its PM1000, its suppose
   to return a false.*/
  time_fn_map_set_rtc_display_mode(rtc_disp_mode);

  /* Retrieve the real time clock's Julian time/date from the PMIC*/
  valid = time_fn_map_get_pmic_time(&julian_local);
  ATS_MSG_HIGH("TIME_DEBUG: rtc_status=%d, timetick_now=%d, current_valid=%d ",
            rtc_status,
            timetick_get(),
            valid);
  
  ATS_MSG_HIGH("TIME_DEBUG: RTC Date Now = %d-%d-%d ", 
             julian_local.day, 
			 julian_local.month,
			 julian_local.year);
  ATS_MSG_HIGH("TIME_DEBUG: RTC Time Now = = %d:%d:%d ", 
             julian_local.hour, 
			 julian_local.minute,
			 julian_local.second);
  
  return rtc_status;

} /* ats_print_rtc_status */


/*=============================================================================

FUNCTION ATS_RTC_REMOTE_UPDATE                                        REGIONAL

DESCRIPTION
  Update TOD from the current proc to another proc

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates remote generic offset if set to True

=============================================================================*/

void ats_rtc_remote_update(void)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update ats_rtc from the current proc to another proc */
  time_genoff_set_remote_genoff( ptime_genoff );

} /* ats_rtc_remote_update */



/*=============================================================================

FUNCTION ATS_RTC_INIT                                                 REGIONAL

DESCRIPTION
  Initializes the ats_rtc subsystem

DEPENDENCIES
  Must be called when the phone first powers on, from time_init( ).

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void ats_rtc_init( void )
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Add initialization function */
  rtc_status = TS_RTC_INVALID;
  read_pmic_rtc = FALSE;

  /* Get the pointer to ats_rtc instance */
  ptime_genoff = time_genoff_get_pointer( time_base );

  /* Initialize ats_rtc */
  time_genoff_pre_init( ptime_genoff, ATS_RTC );

  /* Add base subsys to ats_rtc */
  time_genoff_add_init_call( ptime_genoff, &ats_rtc_set_from_pmic_rtc );

  /* Initialize ats_rtc */
  time_genoff_post_init( ptime_genoff );

} /* ats_rtc_init */


#if defined(ATS_RTC_SYNC_PMIC_RTC) && \
    !defined(FEATURE_ATS_RTC_AS_RTC_OFFSET) && \
    (!defined(FEATURE_MULTIPROCESSOR) || defined(IMAGE_MODEM_PROC))


/*=============================================================================

FUNCTION ATS_RTC_SET_TO_PMIC_RTC

DESCRIPTION
  Synchronize the PMIC's Real Time Clock with the current time-of-day

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void ats_rtc_set_to_pmic_rtc( void )
{

  /* Seconds since January 6, 1980 00:00:00 */
  uint32                          secs;

  /* Julian time/date from the PMIC's Real Time Clock */
  time_julian_type                julian;

  boolean                         valid = FALSE;

  /* instace to send the display mode parameter */
  time_fn_map_ds_type             rtc_disp_mode = {0};

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the local time in seconds */
  secs = time_get_secs();

  /* Convert seconds to Julian time/date */
  time_jul_from_secs ( secs, &julian );

  /* Force display mode to 24 hours. Don't check for
   the result here since if its PM1000, its suppose
   to return a false.*/
  rtc_disp_mode.display_mode = ATS_RTC_DISP_MODE_24HR; 
  time_fn_map_set_rtc_display_mode(rtc_disp_mode);

  /*set the julian time*/
  valid = time_fn_map_set_pmic_time(&julian);

  if(FALSE == valid)
  {
    ATS_ERR_1("Could not set PMIC time. secs: 0x%x", secs);
  } 

} /* ats_rtc_set_to_pmic_rtc */

#endif /* ATS_RTC_SYNC_PMIC_RTC && !FEATURE_TIME_TOD_AS_RTC_OFFSET &&
       (!FEATURE_MULTIPROCESSOR || IMAGE_MODEM_PROC) */


