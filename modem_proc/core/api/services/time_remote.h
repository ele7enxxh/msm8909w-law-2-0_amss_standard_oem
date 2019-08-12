#ifndef TIME_REMOTE_H
#define TIME_REMOTE_H
/**
@file time_remote.h
@brief
This module contains functions that allow remote calls from modem
to apps as well as apps to modem.

*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The ATS_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      ATS_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the time_remote group 
      description in the ATS_mainpage.dox file. 
=============================================================================*/

/*=============================================================================
  Copyright (c) 2005-2012 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*===========================================================================

                        T I M E R   S E R V I C E S
                        
              M O D E M   T O   A P P ' S   F U N C T I O N S

DESCRIPTION
  This header file contains definitions of:
  App's processor functions called remotely from modem processor (m to a).
  Modem's processor functions called remotely from apps processor (a to m).

REFERENCES
  None
===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //components/rel/core.mpss/3.5.c12.3/api/services/time_remote.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ 
$Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/15/10   EBR     Added function ats_client_force_modem_genoff 
11/29/10   scd     (Tech Pubs) Edited Doxygen markup and comments.
10/27/10   EBR     Doxygenated File.
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
/** @addtogroup time_remote
@{ */

#if defined(IMAGE_MODEM_PROC)

/**
Sets up the RPC proxy to call a callback in the future to make the RPC call 
to inform the applications processor of the change in TOD bases.

@param[in] force_update Indicates whether updates are to be forced.

@return
None.

@dependencies
None.
*/
void time_remote_update_apps_tod
(
  boolean                         force_update
);
#endif /* IMAGE_MODEM_PROC */


#if defined(IMAGE_MODEM_PROC)
/**
Modem processor SMSM-based code that synchronizes the TOD base.
@note1hang Sets up the smsm TOD callback, which is where all the work
           occurs.

@return
None.

@dependencies
None.
*/
void time_remote_init_time_of_day( void );
#endif /* IMAGE_MODEM_PROC */


#if defined(IMAGE_APPS_PROC)
/**
Sets up the RPC proxy to call a callback in the future to make the RPC call 
to set the modem TOD to the given timstamp.

@param[in] ts_val Timestamp to which the TOD is set.

@return
None.

@dependencies
None.
*/
void time_remote_set_user_time
(
  time_type                       ts_val
);

#endif /* IMAGE_APPS_PROC */


#if defined(IMAGE_APPS_PROC)
/**
Same as TIME_REMOTE_SET_USER_TIME, but the prototype is necessary for the 
renameing done in the autogenerated RPC files.

@param[in] ts_val Timestamp to which the TOD is set.

@return
None.

@dependencies
None.
*/
void time_remote_set_user_time_external
(
  time_type                       ts_val
);


/**
Request from an application to set the system time on the modem side.

@param[in] jul_time Timestamp to which the TOD is set (in Julian format).

@return
None.

@dependencies
A valid time (CDMA, HDR, TOD, etc.).

@sideeffects
Modifies the system timestamp.
*/
void time_remote_set_julian
(
  time_julian_type                *jul_time
);


/**
Requests the system time, in julian format from the modem.

@param[out] jul_time Timestamp to get TOD in Julian format.

@return
None.

@dependencies
A valid time (CDMA, HDR, TOD, etc.).
*/
void time_remote_get_julian
(
  time_julian_type                *jul_time
);

#ifdef FEATURE_SEC_TIME_MU
/**
Indicates whether the secure time is valid.

@return
TRUE  -- Secure time is valid. \n
FALSE -- Secure time is not valid.

@dependencies
Secure time must have been initialized on the modem side.
*/
boolean time_remote_secure_valid( void );


/**
Requests the secure time in timestamp format.

@param[out] ts_val Timestamp into which the secure TOD is put.

@return
None.

@dependencies
Secure time must have been initialized on the modem side.
*/
void time_remote_secure_get(
  uint64                          *ts_val
);


/**
Sets the secure time.

@param[out] ts_val Timestamp to which the secure TOD is set.

@return
None.

@dependencies
Secure time must have been initialized on the modem side.
*/
void time_remote_secure_set(
  uint64                          *ts_val
);


/**
Requests the secure time in timestamp format.

@param[out] julian Pointer to where to store the Secure TOD.

@return
None.

@dependencies
Secure time must have been initialized on the modem side.
*/
void time_remote_secure_get_julian(
  time_julian_type                  *julian
);


#endif /* #ifdef FEATURE_SEC_TIME_MU */


#endif /* IMAGE_APPS_PROC */


/**
Retrieves the information needed for BREW for calculating local offsets for 
time.

@param[out] local_timezone   Pointer to store the local timezone in seconds.
@param[out] daylight_savings Pointer to store whether daylight savings time is 
                             in effect.

@return
None.

@dependencies
None.
*/
extern void time_get_local_offset_info
(
  int32                           *local_timezone,
  boolean                         *daylight_savings
);


/**
Requests an update to the applications TOD bases.

@return
None.

@dependencies
None.
*/
void time_tod_request_apps_bases( void );


/**
Gets the applications user time.

@param[out] secs Pointer to store the applications user time.

@return
None.

@dependencies
Used for Windows<sup>&reg;</sup> Mobile builds only.
*/
void time_remote_get_apps_user_time
(
  uint32                          *secs
);


/**
Updates applications to the modem offset on the modem side.

@param[in] offset New offset to be used.

@return
None.

@dependencies
Used for Windows<sup>&reg;</sup> Mobile builds only.
*/
void time_remote_update_apps_to_modem_offset
(
  int32 offset
);


/**
Sets an alarm with the given expiration. The existing alarm is canceled.

@param[in] expiration Amount of time in milliseconds in which to expire the
                      timer.

@return
Zero  -- Alarm was set successfully. \n
Other -- Alarm was not set successfully.

@dependencies
None.
*/
int32 time_alarm_set_ms
(
  uint32 expiration
);


/**
Sets an alarm with the given expiration. The existing alarm is canceled.

@param[in] expiration Amount of time in seconds in which to expire the timer.

@return
Zero  -- Alarm was set successfully. \n
Other -- Alarm was not set successfully.

@dependencies
None.
*/
int32 time_alarm_set_secs
(
  uint32 expiration
);


/**
Requests the secure time in timestamp format.

@param[out] julian Pointer to location for the secure TOD.

@return
None.

@dependencies
Secure time must have been initialized on the modem side.
*/
void time_remote_secure_set_julian
(
  time_julian_type                  *julian
);


/**
Allows the client to register itself with the API.

@param[in] client_type   Type of client.
@param[in] client_id_ptr ID type of client.
@param[in] processor     Processor type of client.

@return
Error codes for the client.

@dependencies
None.

@sideeffects
Internal data initialized for the new client.
*/
ats_client_err_e_type ats_client_init
(
  ats_client_type_e_type   client_type,
  ats_client_id_type       *client_id_ptr,
  ats_client_processor_e_type  processor
);


/**
Allows the client to release itself from the API.

@param[out] client_id ID of the client to be released.

@return
Error codes for the client.

@dependencies
None.

@sideeffects
Internal data reset for this client.
*/
ats_client_err_e_type ats_client_release
(
  ats_client_id_type       client_id
);


/**
Allows the client to tell the API that it is not ready to use the services.

@param[out] client_id ID of the client.

@return
Error codes for the client.

@dependencies
None.

@sideeffects
Internal data is changed.
*/
ats_client_err_e_type ats_client_deactivate
(
  ats_client_id_type       client_id
);


/**
Allows the client to register its configuration event callback function. 
When a NULL pointer is passed, the callback is deregistered.

@param[in] client_id    ID of the client that wants to register its 
                        configuration event callback function.
@param[in] cfg_event_cb Callback event function to be registered.

@return
Error codes for the client.

@dependencies
None.

@sideeffects
Internal data is updated.
*/
ats_client_err_e_type ats_client_reg_cfg_cb
(
  ats_client_id_type       client_id,
  ats_cfg_event_cb_type    cfg_event_cb
);

/** 
Set a boolean value that will force the modem to send over it's generic offsets,
 even if the valid flag is set to FALSE.
 
@param[in] client_id ID of the client to set the value for.
@param[in] force_modem_genoff TRUE to force modem genoff values to be sent, FALSE otherwise.

@return
Error code for the client.

@dependencies
None.

@sideeffects
Internal data is updated.

*/
ats_client_err_e_type ats_client_force_modem_genoff
(
  boolean force_modem_genoff
);

/**
Allows the client to tell the API that it is ready to use the services.

@param[in] client_id ID for the client that is ready to use the services.

@return
Error code for the client.

@dependencies
None.

@sideeffects
Internal data is changed.
*/
ats_client_err_e_type ats_client_activate
(
  ats_client_id_type       client_id
);


/**
Notifies all clients of a configuration event.

@param[in] event              Registered client ID.
@param[in] cfg_event_info_ptr Pointer to the event information.

@return
None.

@dependencies
None.
*/
void ats_cfg_event_notify
(
  ats_cfg_event_e_type        event,
  ats_cfg_event_info_s_type   *cfg_event_info_ptr
);


/**
Sets/gets time based on the argument.

@param[in,out] pargs Pointer to time_genoff_args_type with genoff_ptr in
                     consideration.

@return
None.

@dependencies
None.
*/
void time_remote_atom_genoff_opr
(
  time_genoff_args_ptr         pargs
);


/**
Sets the generic offset.

@param[in] base   Base of the genoff in consideration.
@param[in] offset Generic offset to be set.

@return
None.

@dependencies
Used by remote calls only.
*/
void time_remote_atom_genoff_set_generic_offset
(
  time_bases_type base,
  int64           offset
);

/** @} */ /* end_addtogroup time_remote */

#endif /* TIME_REMOTE_H */

