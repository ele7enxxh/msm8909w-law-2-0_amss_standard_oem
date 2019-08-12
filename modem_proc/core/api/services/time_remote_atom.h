#ifndef TIME_REMOTE_ATOM_H
#define TIME_REMOTE_ATOM_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
@file time_remote_atom.h
@brief
This module contains functions that allow remote calls from apps to modem.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The ATS_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      ATS_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the XXX group 
      description in the ATS_mainpage.dox file. 
=============================================================================*/

/*=============================================================================
  Copyright (c) 2005-2012 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================

                        T I M E R   S E R V I C E S
                        
              A P P ' S   TO   M O D E M  F U N C T I O N S

DESCRIPTION
  This header file contains definitions of:
  Modem processor functions called remotely from Apps processor
  (a to m).
 
REFERENCES
  None
===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //components/rel/core.mpss/3.5.c12.3/api/services/time_remote_atom.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ 
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/29/10   scd     (Tech Pubs) Edited Doxygen markup and comments.
10/27/10   EBR     Doxygenated File.
12/5/09    din     Added functions for generic framework.
08/27/09   din     Added time_remote_secure_set_julian(). Regenerated RPC 
                   files with new version number.
06/29/09   din     Extended Alarm period by adding time_alarm_set_secs(). 
                   Also added time_alarm_set_ms().
06/12/09   din     Added time_remote_update_apps_to_modem_offset
03/12/09   din     Added time_remote_secure_get_julian()
08/19/08   jhs     Switch remote secure APIs to use uint64
07/25/08   jhs     Remove featurization in remote header file
04/25/08   jhs     Add time_remote_valid API
04/15/08    mk     Added Secure Time APIs
04/01/08    mk     Made ts_val argument to time_remote_set_user_time an 
                   explicit ptr
04/01/08    mk     Remoted time_get/set_julian APIS 
07/09/07   jhs     Support for Apps to request TOD update from Modem.
06/07/07    mk     Added time_get_user_time functionality
05/04/07   jhs     Added time_get_local_offset_info functionality.
03/07/07   jhs     Fix time_alarm_set function header.
12/01/06   jhs     Fix ATOM implementation.
09/27/06   gb      Created initial version of file
   

===========================================================================*/



/*============================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "time_types.h"
#include "time_tod.h"
#include "time_remote_reg.h"
#include "time_genoff.h"


/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items required by this module.

===========================================================================*/


/*===========================================================================

                             FUNCTION DEFINITIONS

===========================================================================*/
/** @addtogroup time_remote_atom
@{ */

/** 
Receives RTC time change requests from the applications processor and updates
the modem TOD.

@param[in] ts_val Timestamp to set the remote user time.            
 
@return
None.

@dependencies 
None.
 
@sideeffects 
Synchronizes the slow-clock-based TOD to a given timestamp.
*/
extern void time_remote_set_user_time
(
  time_type                ts_val 
);
/*~ FUNCTION time_remote_set_user_time CLNTNAME time_remote_set_user_time_external */



/** 
Sets an alarm with the given expiration. The existing alarm is cancelled.

@param[in] expiration Amount of time in milliseconds in which to expire the timer.
 
@return
The alarm time that is actually set is returned.

@dependencies 
None.
*/
extern int32 time_alarm_set
(
  uint32         expiration 
);
/*~ FUNCTION time_alarm_set */



/** 
@brief 
Retrieves the information needed for BREW for calculating local offsets for time.

@param[out] local_timezone   Pointer to the local timezone in seconds.
@param[out] daylight_savings Pointer to the value of daylight savings time.
 
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
/*~ FUNCTION time_get_local_offset_info */
/*~ PARAM OUT local_timezone POINTER */
/*~ PARAM OUT daylight_savings POINTER */



/** 
Updates the applications processor TOD bases.
 
@return
None.
 
@dependencies 
None.
*/
extern void time_tod_request_apps_bases
( 
  void 
);
/*~ FUNCTION time_tod_request_apps_bases */

/**                            
Allows the applications processor to set the system time on the modem side.
 
@param[in] jul_time Pointer to a Julian timestamp with the time to be set.

@return
None.

@dependencies 
A valid time (CDMA, HDR, TOD, etc.).
 
@sideeffects 
Modifies the system timestamp.
*/
extern void time_remote_set_julian
(
  time_julian_type                *jul_time 
);
/*~ FUNCTION time_remote_set_julian */
/*~ PARAM jul_time POINTER  */



/**
Requests the system time in Julian format from the modem.
 
@param[out] jul_time Pointer to where to place the Julian time.

@return
None.

@dependencies 
None.
*/
extern void time_remote_get_julian
(
  time_julian_type                *jul_time
);
/*~ FUNCTION time_remote_get_julian  */
/*~ PARAM OUT jul_time POINTER  */


/** 
Requests that TOD be reset to 6 Jan 1980 00:00:00 and allow OTA time updates.
 
@return
None.

@dependencies 
None.
*/
extern void time_tod_request_reset
( 
  void 
);
/*~ FUNCTION time_tod_request_reset */


/** 
Sets the secure time.
 
@param[in] ts_val Pointer to the secure time to be set.
 
@return
None

@dependencies 
Secure time must have been initialized on the modem side.
*/
extern void time_remote_secure_set(
  uint64                          *ts_val
);
/*~ FUNCTION time_remote_secure_set */
/*~ PARAM ts_val POINTER */


/** 
Requests the secure time in timestamp format.
 
@param[out] ts_val Pointer to the timestamp to get the secure time.
 
@return
None.

@dependencies
Secure time must have been initialized on the modem side.
*/
extern void time_remote_secure_get(

  uint64                *ts_val  
);
/*~ FUNCTION time_remote_secure_get */
/*~ PARAM OUT ts_val POINTER*/


/** 
Requests the secure time in Julian format.

@param[out] julian Pointer to where to store the Julian time.

@return
None.

@dependencies 
Secure time must have been initialized on the modem side.
*/
extern void time_remote_secure_get_julian(
  time_julian_type                  *julian
);
/*~ FUNCTION time_remote_secure_get_julian */
/*~ PARAM OUT julian POINTER*/


/** 
Returns whether the secure time is valid.
 
@return
TRUE  -- Secure time is valid. \n
FALSE -- Secure time is not valid.

@dependencies 
None.
*/
extern boolean time_secure_valid
( 
  void 
);
/*~ FUNCTION time_secure_valid CLNTNAME time_remote_secure_valid */


/** 
Sets an alarm with the given expiration. The existing alarm is canceled.
 
@param[in] expiration Value of the timer in milliseconds.
 
@return
Zero  -- Alarm was set successfully. \n
Other -- Alarm was not set successfully.
 
@dependencies 
Secure time must have been initialized from the modem side.
*/
extern int32 time_alarm_set_ms
(
  uint32 expiration 
);  
/*~ FUNCTION time_alarm_set_ms */


/**   
Sets an alarm with the given expiration. The existing alarm is canceled.
 
@param[in] expiration Timer value in seconds to which the alarm is set.

@return
Zero  -- Alarm was set successfully. \n
Other -- Alarm was not set successfully.

@dependencies 
None.
*/
extern int32 time_alarm_set_secs
(
  uint32 expiration 
);
/*~ FUNCTION time_alarm_set_secs */


/**
Updates the applications processor to the modem time offset on the modem.
 
@param[in] offset New offset value to use.

@return
None.

@dependencies 
Used for Windows<sup>&reg;</sup> Mobile only.
*/
void time_remote_update_apps_to_modem_offset
(
  int32 offset 
);
/*~ FUNCTION time_remote_update_apps_to_modem_offset */


/**
Requests the secure time in timestamp format.
 
@param[out] julian Pointer to where to place the secure TOD.

@return
None.

@dependencies 
Secure time must have been initialized on the modem side.
*/
extern void time_remote_secure_set_julian
(
  time_julian_type                  *julian 
);
/*~ FUNCTION time_remote_secure_set_julian */
/*~ PARAM julian POINTER */


/**  
Allows the client to register itself with the API.
 
@param[in] client_type       Type of the client.                        
@param[in,out] client_id_ptr Pointer to the client ID type.             
@param[in] processor         Processor type of the client.
 
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
/*~ FUNCTION ats_client_init */
/*~ PARAM INOUT client_id_ptr POINTER */
/*~ RELEASE_FUNC ats_client_release(*client_id_ptr)
    ONERROR return ATS_CLIENT_ERR_RPC */


/**
Allows the client to release itself from the API.

@param[in] client_id ID of the client to be released.

@return
Error codes for the client.
            
@dependencies 
None.
 
@sideeffects 
Internal data is reset for the new client.
*/
ats_client_err_e_type ats_client_release
(
  ats_client_id_type       client_id
);
/*~ FUNCTION ats_client_release
    ONERROR return ATS_CLIENT_ERR_RPC */

/**
Set a boolean value that will force the modem to send over it's 
 generic offsets, even if the valid flag is set to FALSE. 
 
@param[in] client_id ID of client to update. 
@param[in] force_modem_genoff TRUE to force modem genoff updates  to be sent, FALSE otherwise.
 
@return 
Error codes for the client.
 
@dependencies 
None.

@sideeffects 
Internal data is changed. 
*/
ats_client_err_e_type ats_client_force_modem_genoff
(
  boolean force_modem_genoff
);
/*~ FUNCTION ats_client_force_modem_genoff
    ONERROR return ATS_CLIENT_ERR_RPC */

/**   
Allows the client to tell the API that it is not ready to use the services.
 
@param[in] client_id ID of the client not ready to use the services.

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
/*~ FUNCTION ats_client_deactivate
    ONERROR return ATS_CLIENT_ERR_RPC */


/**
Allows the client to register its configuration event callback function. When a 
NULL pointer is passed, the callback is deregistered.
 
@param[in] client_id    ID of the client in use.                           
@param[in] cfg_event_cb Callback function to register.

@return
Error codes for the client.
 
@dependencies 
None.
 
@sideeffects 
Internal data is changed.
*/
ats_client_err_e_type ats_client_reg_cfg_cb
(
  ats_client_id_type       client_id,
  ats_cfg_event_cb_type    cfg_event_cb
);
/*~ FUNCTION ats_client_reg_cfg_cb
    ONERROR return ATS_CLIENT_ERR_RPC */

/**
Allows the client to tell the API that it is now ready to use the services.
 
@param[in] client_id ID of the client that is ready to use the services.

@return
Error codes for the client.

@dependencies 
None.
 
@sideeffects 
Internal data is changed.
*/
ats_client_err_e_type ats_client_activate
(
  ats_client_id_type       client_id
);
/*~ FUNCTION ats_client_activate
    ONERROR return ATS_CLIENT_ERR_RPC */


/**   
Sets/gets the time on the basis of the inputs.
 
@param[in] base        Time base to use.                        
@param[in] unit        Unit to set/get the offset.                                 
@param[in] operation   Get/set the operation to use.                          
@param[in] base_source Source of the base time.                               
@param[in,out] ts_val  Pointer to the time to be passed/returned.

@return
None.

@dependencies
None.
*/
void time_remote_genoff_opr
(
  time_bases_type                 base,
  time_unit_type                  unit,
  time_genoff_opr_type            operation,     
  base_time_source_type           base_source,
  time_remote_opr_type           *ts_val
);
/*~ FUNCTION time_remote_genoff_opr */
/*~ PARAM INOUT ts_val POINTER DISC unit */


/**  
Sets the generic offset.
 
@param[in] base   Time base to use.                                           
@param[in] offset Offset to set.

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
/*~ FUNCTION time_remote_atom_genoff_set_generic_offset */


/**  
Sets the timer_ipc in the timer subsystem.

@param[in] time    Time (in units set by the unit parameter) until the first 
                   timer expiry.     
@param[in] reload  Period (in units) between the repeated expirations 
                   (0 = not periodic). 
@param[in] unit    Unit by which to measure ticks.                      
@param[in] defer   If TRUE, attach the timer to a deferrable group.                  
@param[in] cfg_cb  Remote callback function to use.
@param[out] handle Handle for the timer.

@return
None.

@dependencies
None.
*/
void timer_ipc_remote_set
(
  timetick_type                   time,
  timetick_type                   reload,
  timer_unit_type                 unit,
  boolean                         defer,
  ats_cfg_event_cb_type           cfg_cb,
  uint32                          *handle
 
);
/*~ FUNCTION timer_ipc_remote_set */
/*~ PARAM OUT handle POINTER */


/** 
Clears the timer_ipc associated with the handle.
 
@param[in] handle  Timer handle.
@param[in] unit    Unit to measure the ticks before expiry.
@param[out] retval Pointer to the remaining time before expiry.

@return
None.

@dependencies 
Timer must be set.
*/
void timer_ipc_remote_clr
(
  uint32                          handle,
  timer_unit_type                 unit,
  timetick_type                   *retval
);
/*~ FUNCTION timer_ipc_remote_clr */
/*~ PARAM OUT retval POINTER */


/**
Pauses timer_ipc.
 
@param[in] handle Handle to the timer.

@return
None.

@dependencies 
Timer must be set.
*/
void timer_ipc_remote_pause
(
  uint32                             handle
);
/*~ FUNCTION timer_ipc_remote_pause */


/**  
Resumes timer_ipc if paused.
 
@param[in] handle Handle to the timer.

@return
None.

@dependencies 
Called only if timer_ipc is paused.
*/
void timer_ipc_remote_resume
(
  uint32                             handle
);
/*~ FUNCTION timer_ipc_remote_resume */


/**  
Gets the number of ticks before timer_ipc expires.
 
@param[in] handle  Handle to the timer.
@param[in] unit    Unit to measure ticks in.                             
@param[out] retval Number of ticks before the timer expires. Zero is returned 
                   for inactive timers and timers that have expired or are 
                   expiring. Fractional values that might truncate to zero, 
                   depending on the unit passed as a parameter, are rounded to 1.

@return
None.

@dependencies 
None.
*/
void timer_ipc_remote_get
(
  uint32                          handle,
  timer_unit_type                 unit,
  timetick_type                   *retval
);
/*~ FUNCTION timer_ipc_remote_get */
/*~ PARAM OUT retval POINTER */


/** 
Starts/stops the remote genoff updates for time-bases based on bits set in 
input uint32.

@param[in] curbs Starts/stops genoff updates for time_bases.

@return
None.
            
@dependencies 
Used by remote calls only.
*/
void time_remote_genoff_set_allow_remote_updates
(
  uint32 curbs
);
/*~ FUNCTION time_remote_genoff_set_allow_remote_updates */

#ifdef __cplusplus
} /* extern "C" */
#endif

/** @} */ /* end_addtogroup time_remote_atom */

#endif /* TIME_REMOTE_ATOM_H */

