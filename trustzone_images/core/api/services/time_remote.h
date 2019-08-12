#ifndef TIME_REMOTE_H
#define TIME_REMOTE_H

/*===========================================================================

                        T I M E R   S E R V I C E S
                        
              M O D E M   T O   A P P ' S   F U N C T I O N S

DESCRIPTION
  This header file contains definitions of:
  App's processor functions called remotely from modem processor (m to a).
  Modem's processor functions called remotely from apps processor (a to m).

REFERENCES
  None

Copyright (c) 2005 - 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/time_remote.h#1 $ 
$DateTime: 2016/06/17 14:31:11 $ 
$Author: pwbldsvc $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/20/10   din     Removed FEATURE_TOD_MULTIPROC_SYNC 
12/08/09   din     Removed old dem portion
12/05/09   din     Added function for Generic framework.
08/27/09   din     Added time_remote_secure_set_julian().
06/29/09   din     Extended Alarm period by adding time_alarm_set_secs(). 
                   Also added time_alarm_set_ms().
04/30/09   din     Added time_remote_update_apps_to_modem_offset()
03/12/09   din     Added time_remote_secure_get_julian()
03/11/09   din     Added time_remote_get_apps_user_time()
03/03/09   din     Added FETAURE_TIME_DEM_SYNCHRONIZE to consolidate with Time 
                   branch with NEW DEM SYNCHRONIZATION
02/23/09   din     Consolidation of time.
08/19/08   jhs     Switch remote secure APIs to use uint64
04/15/08    mk     Added Secure Time APIs
04/01/08    mk     Made ts_val argument to time_remote_set_user_time an 
                   explicit ptr
04/01/08    mk     Remoted time_get/set_julian APIS 
03/20/07   jhs     Removed #ifdef IMAGE_MODEM_PROC protection from
                   time_get_user_time definition.
09/26/07   jhs     Removed SMEM_LOG_EVENT define to fix compiler warning.
07/09/07   jhs     Added ability to force TOD update to Apps.
06/07/07    mk     Added time_get_user_time functionality
05/04/07   jhs     Added time_get_local_offset_info functionality.
12/19/06   jhs     Fix for not passing TOD bases to Apps after first time.
05/11/06   taw     Added function, time_remote_restore, to be used to resync
                   timers after apps power collapse.
04/25/06   taw     Added function, time_remote_slow_clock_sync_poll, to be 
                   called when the modem is in RESET and the timers need
                   to be synced.
03/09/06   jhs     Added time_remote_init_time_of_day to sync time of 
                   day before acquiring.
09/26/05   jhs     Added time_remote_init.
08/11/05   jhs     Updates.
02/07/05   jhs     Initial revision.

===========================================================================*/



/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "time_types.h"
#include "time_genoff.h"
#include "time_remote_reg.h"



/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items required by this module.

===========================================================================*/




/*===========================================================================

                             FUNCTION DEFINITIONS

===========================================================================*/


#if defined(IMAGE_MODEM_PROC)
/*=============================================================================

FUNCTION TIME_REMOTE_UPDATE_APPS_TOD

DESCRIPTION
  Setup a the RPC proxy to call a call back in the future to make the
  RPC call to inform the Apps processor of the change in TOD bases. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_update_apps_tod
(
  /* Indicator to force updates */
  boolean                         force_update
);
#endif /* IMAGE_MODEM_PROC */



#if defined(IMAGE_MODEM_PROC)
/*=============================================================================

FUNCTION TIME_REMOTE_INIT_TIME_OF_DAY
 
DESCRIPTION
  Modem processor SMSM based code which synchronizes the time of day base.

  Sets up the smsm time of day callback - which is where all the work occurs.

DEPENDENCIES
  SMSM must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_init_time_of_day( void );
#endif /* IMAGE_MODEM_PROC */



#if defined(IMAGE_APPS_PROC)
/*=============================================================================

FUNCTION TIME_REMOTE_SET_USER_TIME

DESCRIPTION
  Setup a the RPC proxy to call a call back in the future to make the
  RPC call to set the modem time of day to the given timstamp.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_set_user_time
(
  time_type                       ts_val
);

#endif /* IMAGE_APPS_PROC */



#if defined(IMAGE_APPS_PROC)
/*=============================================================================

FUNCTION TIME_REMOTE_SET_USER_TIME_EXTERNAL

DESCRIPTION
  Same as TIME_REMOTE_SET_USER_TIME, but the prototype is necessary
  for the renameing done in the autogenerated RPC files.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_set_user_time_external
(
  /* Timestamp to set time of day to */
  time_type                       ts_val
);
/*=============================================================================

FUNCTION TIME_REMOTE_SET_JULIAN

DESCRIPTION
  Request from apps to set the system time on the modem side

DEPENDENCIES
  A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.

RETURN VALUE
  None

SIDE EFFECTS
  Modifies system timestamp 

=============================================================================*/
void time_remote_set_julian
(
  /* Timestamp to set time of day to */
  time_julian_type                *jul_time
);


/*=============================================================================

FUNCTION TIME_REMOTE_GET_JULIAN

DESCRIPTION
  Request the system time, in julian format from the modem

DEPENDENCIES
  A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.

RETURN VALUE
  None

SIDE EFFECTS
  Modified input parameter and store julian time

=============================================================================*/
void time_remote_get_julian
(
  /* Timestamp to get time of day in julian format */
  time_julian_type                *jul_time
);

#ifdef FEATURE_SEC_TIME_MU
/*=============================================================================

FUNCTION TIME_REMOTE_SECURE_VALID

DESCRIPTION
  Returns if the secure time is valid

DEPENDENCIES
  Secure time must have been initialized in the modem side

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

boolean time_remote_secure_valid( void );

/*=============================================================================

FUNCTION TIME_REMOTE_SECURE_GET

DESCRIPTION
  Request the secure time in timestamp format

DEPENDENCIES
  Secure time must have been initialized in the modem side

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_secure_get(
  /* Timestamp to put Secure time of day */
  uint64                          *ts_val
);

/*=============================================================================

FUNCTION TIME_REMOTE_SECURE_SET

DESCRIPTION
  Set the secure time 

DEPENDENCIES
  Secure time must have been initialized in the modem side

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_remote_secure_set(
  /* Timestamp to set Secure time of day to */
  uint64                          *ts_val
);

/*=============================================================================

FUNCTION TIME_REMOTE_SECURE_GET_JULIAN

DESCRIPTION
  Request the secure time in timestamp format

DEPENDENCIES
  Secure time must have been initialized in the modem side

RETURN VALUE
  None

SIDE EFFECTS
  Modified input parameter and store julian time

=============================================================================*/

void time_remote_secure_get_julian(
  /* Variable to put Secure time of day */
  time_julian_type                  *julian
);


#endif /* #ifdef FEATURE_SEC_TIME_MU */


#endif /* IMAGE_APPS_PROC */



/*=============================================================================

FUNCTION TIME_GET_LOCAL_OFFSET_INFO

DESCRIPTION
  Retrieves the information needed for BREW for calculating Local
  Offsets for time.

DEPENDENCIES
  None

RETURN VALUE
  Aqcuired System Modem

SIDE EFFECTS
  None

=============================================================================*/

extern void time_get_local_offset_info
(
  /* Value of the Local Timezone in seconds */
  int32                           *local_timezone,
  /* Value of the Day Light Savings */
  boolean                         *daylight_savings
);



/*=============================================================================

FUNCTION TIME_TOD_REQUEST_APPS_BASES

DESCRIPTION
  Request an update to the Apps TOD bases.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_tod_request_apps_bases( void );


/*=============================================================================

FUNCTION TIME_REMOTE_GET_APPS_USER_TIME

DESCRIPTION
  Gets apps user time 

DEPENDENCIES
  Used only for Win-Mobile builds

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_get_apps_user_time
(
  /* Used to get apps user time */
  uint32                          *secs
);


/*=============================================================================

FUNCTION TIME_REMOTE_UPDATE_APPS_TO_MODEM_OFFSET

DESCRIPTION
  Updates apps to modem offset on Modem

DEPENDENCIES
  Used only for WM . 

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_update_apps_to_modem_offset
(
  int32 offset
);

/*=============================================================================

FUNCTION TIME_ALARM_SET_MS

DESCRIPTION
  Set an alarm with the given expiration.  Existing alarm will be canceled.

DEPENDENCIES
  None

RETURN VALUE
  0 - Success
  Other - Failure

SIDE EFFECTS
  None

=============================================================================*/
int32 time_alarm_set_ms
(
  /* Amount of time in milliseconds in which to expire the timer */
  uint32 expiration
);


/*=============================================================================

FUNCTION TIME_ALARM_SET_SECS

DESCRIPTION
  Set an alarm with the given expiration.  Existing alarm will be canceled.

DEPENDENCIES
  None

RETURN VALUE
  0 - Success
  Other - Failure

SIDE EFFECTS
  None

=============================================================================*/

int32 time_alarm_set_secs
(
  /* Amount of time in seconds in which to expire the timer */
  uint32 expiration
);


/*=============================================================================

FUNCTION TIME_REMOTE_SECURE_SET_JULIAN

DESCRIPTION
  Request the secure time in timestamp format

DEPENDENCIES
  Secure time must have been initialized in the modem side

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_secure_set_julian
(
  /* Variable to set Secure time of day */
  time_julian_type                  *julian
);

/*=========================================================================
FUNCTION
  ATS_CLIENT_INIT

DESCRIPTION
  Allow the client to register itself with the API.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data initialized for the new client.

=========================================================================*/
ats_client_err_e_type ats_client_init
(
  ats_client_type_e_type   client_type,
  ats_client_id_type       *client_id_ptr,
  ats_client_processor_e_type  processor
);

/*=========================================================================
FUNCTION
  ATS_CLIENT_RELEASE

DESCRIPTION
  Allow the client to release itself from the API.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data reset for this client.

=========================================================================*/
ats_client_err_e_type ats_client_release
(
  ats_client_id_type       client_id
);

/*=========================================================================
FUNCTION
  ATS_CLIENT_DEACTIVATE

DESCRIPTION
  The client tells the API that it is not ready to use the services.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data changed.

=========================================================================*/
ats_client_err_e_type ats_client_deactivate
(
  ats_client_id_type       client_id
);

/*=========================================================================
FUNCTION
  ATS_CLIENT_REG_CFG_CB

DESCRIPTION
  Allow the client to register its configuration event callback function.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
ats_client_err_e_type ats_client_reg_cfg_cb
(
  ats_client_id_type       client_id,
  ats_cfg_event_cb_type    cfg_event_cb
);

/*=========================================================================
FUNCTION
  ATS_CLIENT_ACTIVATE

DESCRIPTION
  The client tells the API that it is ready to use the services.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data changed.

=========================================================================*/
ats_client_err_e_type ats_client_activate
(
  ats_client_id_type       client_id
);

/*===========================================================================
FUNCTION ATS_CFG_EVENT_NOTIFY

DESCRIPTION
  Notify all clients of a configuration event.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ats_cfg_event_notify
(
  /* Registered Client ID */
  ats_cfg_event_e_type        event,
  
  /* Pointer to event info ptr */
  ats_cfg_event_info_s_type   *cfg_event_info_ptr
);


/*=============================================================================

FUNCTION TIME_REMOTE_GENOFF_OPR

DESCRIPTION
  Does time set/get operation on the basis of Inputs.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_atom_genoff_opr
(
  /* Pointer to time_genoff_args_type with genoff_ptr in consideration */
  time_genoff_args_ptr         pargs
);



/*=============================================================================

FUNCTION TIME_REMOTE_ATOM_GENOFF_SET_GENERIC_OFFSET                               

DESCRIPTION
  Sets Generic offset.

DEPENDENCIES
  Should be used only by remote calls

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_atom_genoff_set_generic_offset
(
  /* Base of Genoff in consideration */
  time_bases_type base,

  /* Generic Offset to be set */
  int64           offset
);

#endif /* TIME_REMOTE_H */

