/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              Time STUBS

GENERAL DESCRIPTION
  This module contains stubs for deprecated functions. Use of these functions
  are discouraged.

Copyright (c) 2010 - 2013 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/time_stubs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/11   din     Added time_tod_get() stub.
07/12/10   din     Created.
===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#include "time_jul.h"
#include "time_tod.h"
#include "time_secure.h"
#include "time_genoff.h"



/*=============================================================================

FUNCTION TIME_TOD_GET_20MS_FRAME_TIME

DESCRIPTION
  This procedure is returns the 64-bit system timestamp in units of
  20 millisecond frame time (traffic / paging / access channel frame
  time).

DEPENDENCIES
  *** Migration aid ***
  This function is deprecated.

RETURN VALUE
  20ms-time frame since 6 Jan 1980 00:00:00 returned to the time parameter 

SIDE EFFECTS
  None

=============================================================================*/

void time_tod_get_20ms_frame_time
(
  /* Address of qword in which to return 64-bit frame time. */
  time_type                       ts_val
)
{
  /* Arguments to be sent to set ATS_TOD */
  time_genoff_args_struct_type    temp_genoff_args;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  temp_genoff_args.base        = ATS_TOD; 
  temp_genoff_args.base_source = TIME_SCLK;
  temp_genoff_args.ts_val      = ts_val;
  temp_genoff_args.unit        = TIME_20MS_FRAME;
  temp_genoff_args.operation   = T_GET;
  temp_genoff_args.dal_timetick_val_valid = 0;
  temp_genoff_args.dal_timetick_val       = 0;
  time_genoff_opr( &temp_genoff_args );

} /* time_tod_get_20ms_frame_time */


/*=============================================================================

FUNCTION TIME_TOD_GET_MS

DESCRIPTION
  Get the time from TOD source, in # of milliseconds since "the beginning of 
  time".

DEPENDENCIES
  *** Migration aid ***
  This function is deprecated.

RETURN VALUE
  Milliseconds since 6 Jan 1980 00:00:00 returned to the time parameter

SIDE EFFECTS
  None

=============================================================================*/

void time_tod_get_ms
(
  /* Out: Time in milliseconds since 6 Jan 1980 00:00:00 */
  time_type                       ts_val
)
{
  /* Arguments to be sent to set ATS_TOD */
  time_genoff_args_struct_type    temp_genoff_args;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  temp_genoff_args.base        = ATS_TOD; 
  temp_genoff_args.base_source = TIME_SCLK;
  temp_genoff_args.ts_val      = ts_val;
  temp_genoff_args.unit        = TIME_MSEC;
  temp_genoff_args.operation   = T_GET;
  temp_genoff_args.dal_timetick_val_valid = 0;
  temp_genoff_args.dal_timetick_val       = 0;
  time_genoff_opr( &temp_genoff_args );

} /* time_tod_get_ms */



/*=============================================================================

FUNCTION TIME_TOD_GET_MS

DESCRIPTION
  Get the time from TOD source, in # of milliseconds since "the beginning of 
  time".

DEPENDENCIES
  *** Migration aid ***
  This function is deprecated.

RETURN VALUE
  Milliseconds since 6 Jan 1980 00:00:00 returned to the time parameter

SIDE EFFECTS
  None

=============================================================================*/

void time_tod_get
(
  /* Out: Time in milliseconds since 6 Jan 1980 00:00:00 */
  time_type                       ts_val
)
{
  /* Arguments to be sent to set ATS_TOD */
  time_genoff_args_struct_type    temp_genoff_args;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  temp_genoff_args.base        = ATS_TOD; 
  temp_genoff_args.base_source = TIME_SCLK;
  temp_genoff_args.ts_val      = ts_val;
  temp_genoff_args.unit        = TIME_STAMP;
  temp_genoff_args.operation   = T_GET;
  temp_genoff_args.dal_timetick_val_valid = 0;
  temp_genoff_args.dal_timetick_val       = 0;
  time_genoff_opr( &temp_genoff_args );

} /* time_tod_get */



/*=============================================================================

FUNCTION TIME_TOD_SET_SECS                                             REGIONAL

DESCRIPTION
  Set the slow-clock based time-of-day to the given time in seconds

DEPENDENCIES
  *** Migration aid ***
  This function is deprecated.

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/

void time_tod_set_secs
(
  /* Value in seconds to set the slow-clock based time-of-day to */
  uint32                            secs
)
{
  /* Arguments to be sent to set ATS_TOD */
  time_genoff_args_struct_type    temp_genoff_args; 
  
  time_type                       ts_val;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  qw_set ( ts_val, 0 , secs );

  temp_genoff_args.base        = ATS_TOD; 
  temp_genoff_args.base_source = TIME_SCLK;
  temp_genoff_args.ts_val      = ts_val;
  temp_genoff_args.unit        = TIME_SECS;
  temp_genoff_args.operation   = T_SET;
  temp_genoff_args.dal_timetick_val_valid = 0;
  temp_genoff_args.dal_timetick_val       = 0;
  time_genoff_opr( &temp_genoff_args );

} /* time_tod_set_secs */


/*=============================================================================

FUNCTION TIME_TOD_SET                                                  REGIONAL

DESCRIPTION
  Set the slow-clock based time-of-day to the given timestamp

DEPENDENCIES
  Should be called from an INTLOCK'd context, to ensure proper time-setting.

RETURN VALUE
  *** Migration aid ***
  This function is deprecated.

SIDE EFFECTS
  An diagnostic message is generated if there is a sudden jump in time-of-day.

=============================================================================*/

void time_tod_set
(
  /* Timestamp to set time of day to */
  time_type                       ts_val
)
{
  /* Arguments to be sent to set ATS_TOD */
  time_genoff_args_struct_type    temp_genoff_args;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  temp_genoff_args.base        = ATS_TOD; 
  temp_genoff_args.base_source = TIME_SCLK;
  temp_genoff_args.ts_val      = ts_val;
  temp_genoff_args.unit        = TIME_STAMP;
  temp_genoff_args.operation   = T_SET;
  temp_genoff_args.dal_timetick_val_valid = 0;
  temp_genoff_args.dal_timetick_val       = 0;
  time_genoff_opr( &temp_genoff_args );

} /* time_tod_set */


/*=============================================================================

FUNCTION GET_TIME_TOD_RTC_STATUS                                       REGIONAL

DESCRIPTION

DEPENDENCIES
  *** Migration aid ***
  This function is deprecated.

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/

time_tod_rtc_status_type get_time_tod_rtc_status( void )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   return TS_VALID;

} /* get_time_tod_rtc_status */


/*============================================================================

  FUNCTION          :  TIME_SECURE_VALID
                       
  DESCRIPTION       :  Returns if the secure time is valid 
                                              
  PARAMETERS        :  None
                       
  DEPENDENCIES      :  *** Migration aid ***
                       This function is deprecated.
                       
  RETURN VALUE      :  Boolean
                       
  SIDE EFFECTS      :  None
                       
============================================================================*/

boolean time_secure_valid( void )
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  return TRUE;

} /* time_secure_valid */


/*============================================================================

  FUNCTION          :  TIME_SECURE_GET_MS
                       
  DESCRIPTION       :  Returns the secure time in milliseconds
                                              
  PARAMETERS        :  ms_val - Secure Time in milliseconds
                       
  DEPENDENCIES      :  *** Migration aid ***
                       This function is deprecated.
                       
  RETURN VALUE      :  None
                       
  SIDE EFFECTS      :  None
                       
============================================================================*/

void time_secure_get_ms
(
  /* Secure time in milliseconds */
  time_type                       ms_val
)
{

  /* Arguments to be sent to set ATS_SECURE */
  time_genoff_args_struct_type    temp_genoff_args;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  temp_genoff_args.base        = ATS_SECURE; 
  temp_genoff_args.base_source = TIME_SCLK;
  temp_genoff_args.ts_val      = ms_val;
  temp_genoff_args.unit        = TIME_MSEC;
  temp_genoff_args.operation   = T_GET;
  temp_genoff_args.dal_timetick_val_valid = 0;
  temp_genoff_args.dal_timetick_val       = 0;
  time_genoff_opr( &temp_genoff_args );

}  /* time_secure_get_ms */


/*============================================================================

  FUNCTION          :  TIME_SECURE_SET
                       
  DESCRIPTION       :  Sets the secure time with the timestamp value provided                       
                       
  PARAMETERS        :  ts_val - Time to set in timestamp format
                       
  DEPENDENCIES      :  *** Migration aid ***
                       This function is deprecated.
                       
  RETURN VALUE      :  None
                       
  SIDE EFFECTS      :  None
                       
============================================================================*/

void time_secure_set
(
  /* Timestamp to set secure time to */
  time_type                       *ts_val
)
{
  /* Arguments to be sent to set ATS_SECURE */
  time_genoff_args_struct_type    temp_genoff_args;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  temp_genoff_args.base        = ATS_SECURE; 
  temp_genoff_args.base_source = TIME_SCLK;
  temp_genoff_args.ts_val      = ts_val;
  temp_genoff_args.unit        = TIME_STAMP;
  temp_genoff_args.operation   = T_SET;
  temp_genoff_args.dal_timetick_val_valid = 0;
  temp_genoff_args.dal_timetick_val       = 0;
  time_genoff_opr( &temp_genoff_args ); 
  
  
}  /* time_secure_set */


/*============================================================================

  FUNCTION          :  TIME_SECURE_SET_SECS
                       
  DESCRIPTION       :  Sets the secure time with the seconds provided                       
                       
  PARAMETERS        :  Time to set in seconds
                       
  DEPENDENCIES      :  *** Migration aid ***
                       This function is deprecated.
                       
  RETURN VALUE      :  None
                       
  SIDE EFFECTS      :  None
                       
============================================================================*/

void time_secure_set_secs
(
  /* Value in seconds to set the secure time to */
  uint32                          seconds
)
{

  /* Arguments to be sent to set ATS_SECURE */
  time_genoff_args_struct_type    temp_genoff_args;

  time_type                       ts_val;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  qw_set ( ts_val, 0 , seconds );

  temp_genoff_args.base        = ATS_SECURE; 
  temp_genoff_args.base_source = TIME_SCLK;
  temp_genoff_args.ts_val      = ts_val;
  temp_genoff_args.unit        = TIME_SECS;
  temp_genoff_args.operation   = T_SET;
  temp_genoff_args.dal_timetick_val_valid = 0;
  temp_genoff_args.dal_timetick_val       = 0;
  time_genoff_opr( &temp_genoff_args ); 

}  /* time_secure_set_secs */

#ifndef IMAGE_APPS_PROC
void time_tod_request_reset()
{
}
#endif

